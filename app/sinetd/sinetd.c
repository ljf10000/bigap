/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      sinetd
#endif

#include "utils.h"

#ifndef MAX_SCRIPT
#define MAX_SCRIPT      127
#endif

#ifndef MAX_CLIENTS
#define MAX_CLIENTS     32
#endif

#ifndef MAX_RUNTIME
#define MAX_RUNTIME     300 /* second */
#endif

#ifndef MAX_DELAY
#define MAX_DELAY       (500*1000)
#endif

#ifndef MIN_DELAY
#define MIN_DELAY       (100*1000)
#endif

struct app {
    int pid;
    time_t create;

    struct {
        struct list_head list;
    } node;
};

static struct {
    uint32 ip;
    int port;
    
    int fd;
    int pid;
    char script[1+MAX_SCRIPT];  /* fullname, include path */
    char *script_name;          /* just filename, NOT include path */

    struct {
        struct list_head list;
        int count;
    } head;
} C = {
    .fd     = INVALID_FD,
    .head   = {
        .list = LIST_HEAD_INIT(C.head.list),
    },
};

static struct app *
app_new(int pid)
{
    struct app *app = (struct app *)os_zalloc(sizeof(*app));
    if (NULL==app) {
        debug_error("app_new no memory");
        
        return NULL;
    }
    
    INIT_LIST_HEAD(&app->node.list);
    app->pid = pid;
    time(&app->create);
    
    return app;
}

static inline void
app_free(struct app *app)
{
    free(app);
}

static int
app_insert(struct app *app)
{
    if (NULL==app) {
        return -EINVAL;
    }
    
    list_add_tail(&app->node.list, &C.head.list);
    C.head.count++;

    return 0;
}

static int
app_remove(struct app *app)
{
    if (NULL==app) {
        return -EINVAL;
    }
    
    C.head.count--;
    list_del(&app->node.list);

    return 0;
}

static int
app_foreach(mv_t (*cb)(struct app *app))
{
    struct app *app, *n;
    mv_u mv;
    
    list_for_each_entry_safe(app, n, &C.head.list, node.list) {
        mv.v = (*cb)(app);
        if (is_mv2_break(mv)) {
            return mv2_error(mv);
        }
    }

    return 0;
}

static struct app *
app_find(int pid)
{
    struct app *found = NULL;
    
    mv_t cb(struct app *app)
    {
        if (pid==app->pid) {
            found = app;

            return mv2_break(0);
        } else {
            return mv2_ok;
        }
    }

    app_foreach(cb);

    return found;
}

static int
app_create(int pid)
{
    if (app_find(pid)) {
        return -EEXIST;
    } else {
        return app_insert(app_new(pid));
    }
}

static int
app_delete(int pid)
{
    mv_t cb(struct app *app)
    {
        if (pid==app->pid) {
            app_remove(app);
            app_free(app);
            
            return mv2_break(0);
        } else {
            return mv2_ok;
        }
    }
    
    app_foreach(cb);

    return 0;
}

static int
app_timeout(void)
{
    time_t now = time(NULL);

    mv_t cb(struct app *app)
    {
        if ((now - app->create) > MAX_RUNTIME) {
            debug_trace("app pid:%d timeout", app->pid);
            
            kill(app->pid, SIGKILL);
            
            app_remove(app);
            app_free(app);
        }

        return mv2_ok;
    }
    
    return app_foreach(cb);
}

static void
app_wait(void)
{
    int pid;

    while((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        debug_trace("waited pid:%d", pid);
        
        app_delete(pid);
    }
}

static int
child(void)
{
    int fd;
    int err = 0;
    
    sockaddr_in_t addr;
    socklen_t size = sizeof(addr);
    
    fd = accept(C.fd, (sockaddr_t *)&addr, &size);
    if (fd<0) {
        debug_error("accept error:%d", -errno);
        
        return -errno;
    }

    int pid = fork();
    if (pid<0) {
        debug_error("fork error:%d", -errno);
        
        err = -errno;
    }
    else if (pid>0) { // father
        err = app_create(pid);
    }
    else { // child
        close(0);
        close(1);
        
        dup2(fd, 0);
        dup2(fd, 1);
        
        os_close(fd);
        if (fd!=C.fd) {
            os_close(C.fd);
        }
        
        execl(C.script, C.script_name, NULL);

        debug_error("execl error:%d", -errno);
        err = -errno;
    }

    os_close(fd);

    return err;
}

static int
is_new(bool *new)
{
    fd_set rset;
    struct timeval tv = {
        .tv_sec     = 0,
        .tv_usec    = MAX_DELAY,
    };
    
    FD_ZERO(&rset);
    FD_SET(C.fd, &rset);
    
    int err = select(C.fd+1, &rset, NULL, NULL, &tv);
    switch(err<0) {
        case -1:/* error */
            if (EINTR==errno) {
                break;
            } else {
                return -errno;
            }
        case 0: /* timeout, retry */
            return -ETIMEDOUT;
        default: /* to accept */
            *new = true;
            break;
    }
    
    return 0;
}

static void
service(void)
{
    int err;
    
    while(1) {
        bool new = 0;

        err = is_new(&new);
        if (err<0) {
            usleep(MIN_DELAY);
        }
        else if (new) {
            err = child();
        }

        app_wait();
        
        app_timeout();
    }
}

static int
run(void)
{
    int fd = INVALID_FD;
    int err = 0;
   
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd<0) {
        debug_error("socket error:%d", -errno);
        
        err = -errno; goto error;
    }
    os_closexec(fd);
    
    C.fd = fd;
    
    int opt = 1;
    err = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (err<0) {
        err = -errno; goto error;
    }
    
    sockaddr_in_t addr = OS_SOCKADDR_INET(C.ip, C.port);
    
    err = bind(fd, (sockaddr_t *)&addr, sizeof(addr));
    if (err<0) {
        debug_error("bind error:%d", -errno);
        
        err = -errno; goto error;
    }

    err = listen(fd, MAX_CLIENTS);
    if (err<0) {
        debug_error("listen error:%d", -errno);
        
        err = -errno; goto error;
    }

    C.pid = getpid();
    
    service();
error:
    os_close(fd);

    return err;
}

static int
usage(int argc, char *argv[])
{
    os_eprintln(__THIS_APPNAME);
    os_eprintln(__tab __THIS_APPNAME " ip port script");

    return -EINVAL;
}

/*
* sinetd ip port script
*/
static int
__main(int argc, char *argv[])
{
    char *ip    = argv[1];
    char *port  = argv[2];
    char *script= argv[3];
    int err = 0;

    if (4!=argc) {
        return usage(argc, argv);
    }
    
    /*
    * check script
    */
    struct stat st;
    err = stat(script, &st);
    if (err<0) {
        os_println("bad script %s", script);
        
        return -EINVAL;
    }
    else if (0x100 != (0x100 & st.st_mode)) {
        return -EINVAL;
    }
    
    int len = strlen(script);
    if (len > MAX_SCRIPT) {
        os_println("script too big, NOT more than %d", MAX_SCRIPT);
        
        return -EINVAL;
    }
    os_memcpy(C.script, script, len);
    C.script_name = basename(C.script);
    openlog(C.script_name, 0, LOG_USER);

    struct in_addr addr;
    if (0==inet_aton(ip, &addr)) {
        os_println("bad ip:%s", ip);
        
        return -EINVAL;
    }
    C.ip = addr.s_addr;
    
    int iport = os_atoi(port);
    if (0==iport) {
        os_println("bad port:%s", port);
        
        return -EINVAL;
    }
    C.port = htons(iport);
    
    err = run();
    if (err<0) {
        return err;
    }
    
    return 0;
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(__main, argc, argv);
}

/******************************************************************************/
