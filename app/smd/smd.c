/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      smd
#endif

#define __DEAMON__
#define __CLI_TCP__     0

#include "utils.h"
#include "sm/sm.h"

OS_INITER;

#ifndef SM_NORMAL_SLEEP
#define SM_NORMAL_SLEEP       500 /* ms */
#endif

#ifndef SM_DEAMON_WAIT
#define SM_DEAMON_WAIT        3 /* second */
#endif

static struct {
    struct {
        int fd;
        
        sockaddr_un_t addr;
    } server;

    STREAM f;    /* todo: cache entry */
    int timeout;
    uint32 time;
    
    struct list_head list;
}
smd = {
    .server = {
        .fd     = INVALID_FD,
        .addr   = OS_SOCKADDR_ABSTRACT("smd"),
    },

    .timeout = CLI_TIMEOUT,

    .list = LIST_HEAD_INIT(smd.list),
};

enum {
    SM_F_STATIC   = 0x01,
};

struct sm {
    char *name;
    char *command;
    char *pidfile;
    
    int normal;
    int deamon;
    int forks;
    int state;
    int flags;
    int offset; /* todo: in cache file's offset */
    
    uint32 time[SM_STATE_END];
    
    struct list_head node;
};

#define DAEMONS(_name, _command, _pidfile) {  \
    .name   = _name,                \
    .command= _command,             \
    .pidfile= _pidfile,             \
    .flags  = SM_F_STATIC,    \
}   /* end */

STATIC struct sm smd_daemons[] = {
#if 1
    /* insert static daemon */
#endif
};

STATIC bool
smd_is_normal(struct sm *entry)
{
    return NULL==entry->pidfile;
}
#define smd_is_deamon(_entry)     (false==smd_is_normal(_entry))

STATIC bool
smd_is_timeout(struct sm *entry)
{
    return smd.time - entry->time[entry->state] > SM_DEAMON_WAIT;
}

STATIC char *
smd_type(struct sm *entry)
{
    return smd_is_normal(entry)?"normal":"deamon";
}

STATIC bool
smd_is_run(struct sm *entry)
{
    return SM_STATE_RUN==entry->state;
}

STATIC bool
smd_have_pid(struct sm *entry)
{
    return SM_STATE_FORK==entry->state || SM_STATE_RUN==entry->state;
}

STATIC int
smd_get_normal_pid(struct sm *entry)
{
    int i, pid = 0;

    for (i=0; i<3; i++) {
        os_v_pgeti(&pid, 
            "ps |"
                " grep %s |"
                " grep -v monitor |"
                " grep -v \"\\[\" |"
                " grep -v grep |"
                " head -n 1 |"
                " awk '{print $1}'",
            entry->name);
        if (pid) {
            break;
        }

        usleep(os_usecond(SM_NORMAL_SLEEP));
    }

    debug_trace("get normal(%s) pid(%d)", entry->name, pid);

    return pid;
}

STATIC int
smd_get_deamon_pid(struct sm *entry)
{
    int pid = __os_get_pid(entry->pidfile);

    debug_trace("get normal(%s) pid(%d)", entry->name, pid);

    return pid;
}

STATIC void
smd_set_time(struct sm *entry, char *prefix)
{
    entry->time[entry->state] = smd.time;

    debug_trace("%s: set entry(%s) state(%s) time(%d)",
        prefix,
        entry->name, 
        sm_state_getnamebyid(entry->state),
        entry->time[entry->state]);
}

STATIC void
smd_change(struct sm *entry, int state, int normal, int deamon, char *prefix)
{
    if (normal!=entry->normal && deamon!=entry->deamon) {
        debug_trace("%s: set entry(%s) state(%s==>%s), normal pid(%d==>%d), deamon pid(%d==>%d)",
            prefix,
            entry->name, 
            sm_state_getnamebyid(entry->state),
            sm_state_getnamebyid(state),
            entry->normal, normal,
            entry->deamon, deamon);
        
        entry->normal = normal;
        entry->deamon = deamon;
    }
    else if (normal!=entry->normal) {
        debug_trace("%s: set entry(%s) state(%s==>%s), normal pid(%d==>%d)",
            prefix,
            entry->name, 
            sm_state_getnamebyid(entry->state),
            sm_state_getnamebyid(state),
            entry->normal, normal);
        
        entry->normal = normal;
        entry->deamon = deamon;
    }
    else if (deamon!=entry->deamon) {
        debug_trace("%s: set entry(%s) state(%s==>%s), deamon pid(%d==>%d)",
            prefix,
            entry->name, 
            sm_state_getnamebyid(entry->state),
            sm_state_getnamebyid(state),
            entry->deamon, deamon);
        
        entry->deamon = deamon;
    }

    if (state!=entry->state) {
        entry->state = state;
        smd_set_time(entry, prefix);
    }
}

STATIC void
smd_dump_all(char *name)
{
    struct sm *entry;

    if (__is_ak_debug_trace) {
        list_for_each_entry(entry, &smd.list, node) {
            debug_trace("entry:%s pid:%d/%d forks:%d command:%s ", 
                entry->name,
                entry->normal, entry->deamon,
                entry->forks,
                entry->command);
        }
    }
}

STATIC struct sm *
smd_getbyname(char *name)
{
    struct sm *entry;
    
    if (NULL==name) {
        return NULL;
    }

    list_for_each_entry(entry, &smd.list, node) {
        if (os_streq(name, entry->name)) {
            return entry;
        }
    }
    
    return NULL;
}

STATIC struct sm *
smd_getbynormal(int pid)
{
    struct sm *entry;

    list_for_each_entry(entry, &smd.list, node) {
        if (pid==entry->normal) {
            return entry;
        }
    }

    return NULL;
}

STATIC int
smd_run(struct sm *entry, char *prefix)
{
    int err, pid;
    
    pid = vfork();
    if (pid<0) {
        debug_trace("fork error:%d", -errno);
        
        return -errno;
    }
    else if (pid>0) { // father
        entry->forks++;

        smd_change(entry, SM_STATE_FORK, pid, 0, prefix);

        jinfo("%o", 
            "exec", jobj_oprintf("%s%d%d%s",
                        "name",     entry->name,
                        "normal",   entry->normal,
                        "forks",    entry->forks,
                        "command",  entry->command));

        return 0;
    }
    else { // child
        cmd_table_t *cmd = cmd_argv(entry->command);
        
        err = execvp(cmd->argv[0], cmd->argv);
        
        debug_error("exec %s error:%d", cmd->argv[0], -errno);
        
        exit(err);
    }
}

STATIC void
smd_die(struct sm *entry, char *prefix)
{
    smd_change(entry, SM_STATE_DIE, 0, 0, prefix);
}

STATIC void
smd_killall(struct sm *entry)
{
    os_system("killall '%s'", entry->name);
}

STATIC void
smd_kill_deamon(struct sm *entry)
{
    if (smd_is_deamon(entry)) {
        smd_killall(entry);
    }
}

STATIC int
smd_kill(struct sm *entry)
{
    if (false==smd_have_pid(entry)) {
        return 0;
    }
    
    int pid = smd_is_normal(entry)?entry->normal:entry->deamon;
    if (os_pid_exist(pid)) {
        kill(pid, SIGKILL);
    }
    
    smd_kill_deamon(entry);
    
    jinfo("%o", 
        "kill", jobj_oprintf("%s%d%d%d%s",
                    "name",     entry->name,
                    "normal",   entry->normal,
                    "deamon",   entry->deamon,
                    "forks",    entry->forks,
                    "state",    sm_state_getnamebyid(entry->state)));
                    
    smd_die(entry, "in kill");

    return 0;
}

STATIC void
smd_wait_error(struct sm *entry)
{
    char prefix[1+OS_LINE_SHORT] = {0};
    int pid;

    if (smd_is_normal(entry)) {
        pid = entry->normal;
    } else {
        pid = entry->deamon;
    }
    
    os_snprintf(prefix, OS_LINE_SHORT, "int wait %s(%s)",
        smd_type(entry),
        sm_state_getnamebyid(entry->state));
    
    jerror("%o", 
        "wait-error", jobj_oprintf("%s%d%d%s",
                            "name",     entry->name,
                            smd_type(entry),  pid,
                            "forks",    entry->forks,
                            "state",    sm_state_getnamebyid(entry->state)));

    smd_kill_deamon(entry);
    smd_run(entry, prefix);
}

STATIC void
smd_wait_ok(struct sm *entry)
{
    char prefix[1+OS_LINE_SHORT] = {0};
    int pid = 0, normal = 0, deamon = 0;
    
    if (smd_is_normal(entry)) {
        pid = normal = entry->normal;
    }
    else {
        pid = deamon = smd_is_run(entry)?entry->deamon:smd_get_deamon_pid(entry);
    }
    
    os_snprintf(prefix, OS_LINE_SHORT, "int wait %s(%s)",
        smd_type(entry),
        sm_state_getnamebyid(entry->state));

    if (os_pid_exist(pid)) {
        smd_change(entry, SM_STATE_RUN, normal, deamon, prefix);
    } else {
        jinfo("%o", 
            "wait-ok", jobj_oprintf("%s%d%d%s",
                                    "name",     entry->name,
                                    smd_type(entry),  pid,
                                    "forks",    entry->forks,
                                    "state",    sm_state_getnamebyid(entry->state)));

        smd_kill_deamon(entry);
        smd_die(entry, prefix);
        smd_run(entry, prefix);
    }
}

STATIC void
smd_wait_son(int pid)
{
    debug_trace("wait son:%d", pid);
    
    struct sm *entry = smd_getbynormal(pid);
    if (NULL==entry) {
        debug_trace("wait son:%d, but no-found entry, maybe have been removed", pid);

        return;
    }

    if (smd_is_normal(entry)) {
        jinfo("%o", 
            "wait-son", jobj_oprintf("%s%d%d%s",
                            "name",     entry->name,
                            "normal",   entry->normal,
                            "forks",    entry->forks,
                            "state",    sm_state_getnamebyid(entry->state)));
        char *prefix = "in wait son(normal)";
        
        smd_die(entry, prefix);
        smd_run(entry, prefix);
    } else {
        /*
        * deamon fake die, just clear normal pid
        *
        * check deamon real die in __wait_fork
        */
        smd_change(entry, entry->state, 0, 0, "in wait son(deamon)");
    }
}

STATIC int
smd_wait(void)
{
    int pid;
    struct sm *entry;
    char *prefix;

    while((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        smd_wait_son(pid);
    }
    
    /*
    * check run
    */
    list_for_each_entry(entry, &smd.list, node) {
        switch(entry->state) {
            case SM_STATE_DIE:    /* down */
            case SM_STATE_INIT:
                smd_wait_error(entry);
                
                break;
            case SM_STATE_FORK:
                if (smd_is_timeout(entry)) {
                    smd_wait_ok(entry);
                }
                
                break;
            case SM_STATE_RUN:
                smd_wait_ok(entry);
                
                break;
            default:
                break;
        }
    }
    
    return 0;
}

STATIC int
smd_insert(struct sm *entry)
{
    int err, pid = 0;
    
    if (is_in_list(&entry->node)) {
        return -EINLIST;
    }
    
    list_add(&entry->node, &smd.list);

    /*
    * maybe smd is re-start
    *   check the app is run?
    */
    if (smd_is_normal(entry)) {
        pid = smd_get_normal_pid(entry);
    } else {
        os_v_fgeti(&pid, "%s", entry->pidfile);
        
        if (!os_pid_exist(pid)) {
            pid = 0;
        }
    }
    
    if (0==pid) {
        /*
        * no-found the app is run
        */
        err = smd_run(entry, "in insert(not exist)");
        if (err<0) {
            return err;
        }
    } else {
        /*
        * re-init, just change state and save pid, needn't run it
        */
        if (smd_is_normal(entry)) {
            smd_change(entry, SM_STATE_RUN, pid, 0, "in insert(normal exist)");
        } else {
            smd_change(entry, SM_STATE_RUN, 0, pid, "in insert(deamon exist)");
        }
    }
    
    return 0;
}

STATIC void
smd_destroy(struct sm *entry)
{
    if (entry) {
        os_free(entry->name);
        os_free(entry->command);
        os_free(entry->pidfile);

        os_free(entry);
    }
}

STATIC int
smd_remove(struct sm *entry)
{
    if (os_hasflag(entry->flags, SM_F_STATIC)) {
        return 0;
    }
    else if (false==is_in_list(&entry->node)) {
        return -ENOINLIST;
    }

    debug_trace("remove %s:%s", entry->name, entry->command);
    
    list_del(&entry->node);
    smd_kill(entry);
    smd_destroy(entry);
    
    return 0;
}

STATIC struct sm *
smd_create(char *name, char *command, char *pidfile)
{
    struct sm *entry = (struct sm *)os_zalloc(sizeof(*entry));
    if (NULL==entry) {            
        return NULL;
    }

    entry->name     = strdup(name);
    entry->command  = strdup(command);
    if (pidfile) {
        entry->pidfile  = strdup(pidfile);
    }

    smd_set_time(entry, "in create");
    
    return entry;
}

STATIC int
smd_handle_insert(char *args)
{
    char *type      = args; cli_shift(args);
    char *name      = args; cli_shift(args);
    char *pidfile   = NULL;
    char *command   = NULL;
    int err = 0;

    if (NULL==type) {
        debug_error("NULL type");
        
        return -EINVAL1;
    }
    else if (NULL==name) {
        debug_error("NULL name");
        
        return -EINVAL2;
    }

    if (os_streq(type, "normal")) {
        /*
        * insert normal name command
        */
        command = args; /* NOT shift */
    }
    else if (os_streq(type, "deamon")) {
        /*
        * insert deamon name pidfile command
        */
        pidfile = args; cli_shift(args);
        command = args; /* NOT shift */
    }
    else {
        return -EINVAL3;
    }

    if (NULL==command) {
        debug_error("NULL command");
        
        return -EINVAL4;
    }
    
    struct sm *entry = smd_getbyname(name);
    if (entry) {
        /*
        * have exist, do nothing
        */
        debug_trace("%s exist", name);
        
        return 0;
    }
    
    entry = smd_create(name, command, pidfile);
    if (NULL==entry) {
        return -ENOMEM;
    }

    err = smd_insert(entry);
    if (err<0) {
        smd_destroy(entry);

        return err;
    }
    
    return 0;
}

STATIC int
smd_handle_remove(char *args)
{
    char *name = args; cli_shift(args);
    if (NULL==name) {
        debug_trace("remove monitor without name");
        
        return -EINVAL5;
    }
    
    struct sm *entry = smd_getbyname(name);
    if (NULL==entry) {
        return -ENOEXIST;
    }

    return smd_remove(entry);
}

STATIC int
smd_handle_clean(char *args)
{
    struct sm *entry, *tmp;

    list_for_each_entry_safe(entry, tmp, &smd.list, node) {
        smd_remove(entry);
    }

    return 0;
}

STATIC void
smd_show(struct sm *entry)
{
    cli_sprintf("%s %c%d/%d %d %s '%s'" __crlf,
        entry->name,
        os_hasflag(entry->flags, SM_F_STATIC)?'*':' ',
        entry->normal,
        entry->deamon,
        entry->forks,
        sm_state_getnamebyid(entry->state),
        entry->command);
}

STATIC int
smd_handle_show(char *args)
{
    char *name = args; cli_shift(args);
    struct sm *entry;
    bool empty = true;
    
    cli_sprintf("#name pid/dpid forks state command" __crlf);

    list_for_each_entry(entry, &smd.list, node) {
        if (NULL==name || os_streq(entry->name, name)) {
            smd_show(entry);

            empty = false;
        }
    }

    if (empty) {
        /*
        * drop head line
        */
        __clib_clear();
        
        debug_trace("show monitor(%s) nofound", name);
    }
    
    return 0;
}

STATIC int
smd_server_handle_one(fd_set *r)
{
    static cli_table_t table[] = {
        CLI_ENTRY("insert", smd_handle_insert),
        CLI_ENTRY("remove", smd_handle_remove),
        CLI_ENTRY("clean",  smd_handle_clean),
        CLI_ENTRY("show",   smd_handle_show),
    };
    int err = 0;

    if (FD_ISSET(smd.server.fd, r)) {
        err = clis_handle(smd.server.fd, table);
    }
    
    return err;
}

STATIC int
smd_server_handle(void)
{
    fd_set rset;
    struct timeval tv = {
        .tv_sec     = os_second(smd.timeout),
        .tv_usec    = os_usecond(smd.timeout),
    };
    int err;

    while(1) {
        FD_ZERO(&rset);
        FD_SET(smd.server.fd, &rset);
        
        err = select(smd.server.fd+1, &rset, NULL, NULL, &tv);
        switch(err) {
            case -1:/* error */
                if (EINTR==errno) {
                    // is breaked
                    debug_event("select breaked");
                    continue;
                } else {
                    debug_trace("select error:%d", -errno);
                    return -errno;
                }
            case 0: /* timeout, retry */
                debug_timeout("select timeout");
                return -ETIMEOUT;
            default: /* to accept */
                return smd_server_handle_one(&rset);
        }
    }

    return 0;
}

STATIC int
smd_init_server(void)
{
    int fd;
    int err;
    
    fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (fd<0) {
    	debug_error("socket error:%d", -errno);
        return -errno;
    }
    os_closexec(fd);
    
    err = bind(fd, (sockaddr_t *)&smd.server.addr, get_abstract_sockaddr_len(&smd.server.addr));
    if (err<0) {
        debug_error("bind error:%d", -errno);
        return -errno;
    }
    
    smd.server.fd = fd;
    
    debug_ok("init server ok");
    
    return 0;
}

STATIC int 
smd_init_timer(void)
{
    return setup_timer(SM_TIMER, 0);
}

STATIC int
smd_load(void)
{
    int i;

    /*
    * load static
    */
    for (i=0; i<os_count_of(smd_daemons); i++) {
        debug_trace("load static %s", smd_daemons[i].name);
        
        smd_insert(&smd_daemons[i]);
    }

    if (os_file_exist(SCRIPT_SMD_INIT)) {
        __os_system(SCRIPT_SMD_INIT_RUN);

        debug_trace(SCRIPT_SMD_INIT);
    }

    return 0;
}

STATIC int
smd_fini(void)
{
    os_fini();

    return 0;
}

STATIC int
smd_init(void)
{
    int err;
    
    err = os_init();
    if (err<0) {
        return err;
    }

    err = smd_init_timer();
    if (err<0) {
        return err;
    }
    
    err = smd_init_server();
    if (err<0) {
        return err;
    }

    return 0;
}

STATIC void
smd_timer(int sig)
{
    smd.time++;
}

STATIC void
smd_exit(int sig)
{
    smd_fini();
    
    exit(sig);
}

STATIC int 
smd_main_helper(int argc, char *argv[])
{
    smd_load();
    
    while (1) {
        smd_server_handle();
        smd_wait();
    }
    
    return 0;
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(smd_exit);
    setup_signal_timer(smd_timer);
    setup_signal_callstack(NULL);
    
    int err = os_call(smd_init, smd_fini, smd_main_helper, argc, argv);

    return shell_error(err);
}
/******************************************************************************/
