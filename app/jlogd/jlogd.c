/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      jlogd
#endif

#define __JLOGD__
#define __DEAMON__

#include "utils.h"

OS_INITER;

#ifndef JLOGD_BUFSIZE
#define JLOGD_BUFSIZE       (1*1024*1024-1)
#endif

#ifndef JLOGD_PRI
#define JLOGD_PRI           LOG_CRIT
#endif

#ifndef JLOGD_TIMEOUT
#define JLOGD_TIMEOUT       500
#endif

#ifndef JLOGD_CUTCOUNT
#define JLOGD_CUTCOUNT      PC_VAL(10, 1000)
#endif

#ifndef JLOG_LPATH
#define JLOG_LPATH          "/tmp/log/jlog"
#endif

#ifndef JLOG_RPATH
#define JLOG_RPATH          "/tmp/tftp/log/jlog"
#endif

#ifndef JLOG_LFILE
#define JLOG_LFILE          JLOG_LPATH "/md.log"
#endif

#ifndef JLOG_LCACHE
#define JLOG_LCACHE         JLOG_LPATH "/.md.log"
#endif

#ifndef JLOG_RFILE
#define JLOG_RFILE          JLOG_RPATH "/ap.log"
#endif

#ifndef JLOG_RCACHE
#define JLOG_RCACHE         JLOG_RPATH "/.ap.log"
#endif

#define SCRIPT_CUT          PC_VAL("./jlogcut", SCRIPT_FILE("jlog/cut.cb"))
#define SCRIPT_PUSH         PC_VAL("./jlogpush", SCRIPT_FILE("jlog/push.cb"))

static char jlogb[1 + JLOGD_BUFSIZE];

typedef struct {
    char *name;
    int family;
    int fd;
    os_sockaddr_t addr;
    
    struct {
        STREAM stream;
        
        env_string_t *envar;

        uint32 count;
    } log, cache;
} jlog_server_t;

#define JLOGD_SERVER(_name, _family, _var_log, _var_cache) { \
    .name       = _name,        \
    .family     = _family,      \
    .fd         = INVALID_FD,   \
    .addr = {                   \
        .c  = {                 \
            .sa_family=_family, \
        }                       \
    },                          \
    .log  = {                   \
        .envar  = _var_log,     \
    },                          \
    .cache  = {                 \
        .envar  = _var_cache,   \
    },                          \
}   /* end */

static env_string_t envar_ulog  = ENV_VAR_INITER("__JLOG_LFILE__",  JLOG_LFILE);
static env_string_t envar_ucache= ENV_VAR_INITER("__JLOG_LCACHE__", JLOG_LCACHE);

static env_string_t envar_ilog  = ENV_VAR_INITER("__JLOG_RFILE__",  JLOG_RFILE);
static env_string_t envar_icache= ENV_VAR_INITER("__JLOG_RCACHE__", JLOG_RCACHE);

enum {
    JLOGD_USERVER,
    JLOGD_ISERVER,
    
    JLOGD_END_SERVER
};

typedef struct {
    ak_var_t pri;
    ak_var_t timeout;
    ak_var_t cut;
    
    uint64 seq;
    uint32 event;
    char mac[1+MACSTRINGLEN_L];

    loop_t loop;
    
    jlog_server_t server[JLOGD_END_SERVER];
} jlogd_t;

STATIC jlogd_t jlogd = {
    .pri    = AK_VAR_INITER("pri", JLOGD_PRI),
    .timeout= AK_VAR_INITER("timeout", JLOGD_TIMEOUT),
    .cut    = AK_VAR_INITER("cut", JLOGD_CUTCOUNT),
    
    .mac    = {0},
    .loop   = LOOP_INITER,
    .server = {
        [JLOGD_USERVER] = JLOGD_SERVER("userver", AF_UNIX, &envar_ulog, &envar_ucache),
        [JLOGD_ISERVER] = JLOGD_SERVER("iserver", AF_INET, &envar_ilog, &envar_icache),
    },
};

#define jlogserver0         (&jlogd.server[0])
#define jlogservermax       (&jlogd.server[os_count_of(jlogd.server)])

#define foreach_server(_server) \
    for(_server=jlogserver0; _server<jlogservermax; _server++)

STATIC int
jlogd_jcut(jlog_server_t *server, int cut)
{
    /*
    * open cache file
    */
    if (NULL==server->cache.stream) {
        server->cache.stream = os_fopen(server->cache.envar->value, "a+");
        if (NULL==server->cache.stream) {
        	__debug_error("open file:%s error:%d", server->cache.envar->value, -errno);
            return -errno;
        }

        __debug_trace("open %s", server->cache.envar->value);
    }
    
    /*
    * cut log file
    */
    if ((0==cut || server->log.count >= cut) && server->log.stream) {
        os_fclose(server->log.stream);
        
        os_system(SCRIPT_CUT " %s %d", server->log.envar->value, server->log.count);
        __debug_trace("cut %s", server->log.envar->value);

        os_fclean(server->log.envar->value);
        server->log.count = 0;
    }
    
    /*
    * reopen log file
    */
    if (NULL==server->log.stream) {
        server->log.stream = os_fopen(server->log.envar->value, "a+");
        if (NULL==server->log.stream) {
        	__debug_error("open file:%s error:%d", server->log.envar->value, -errno);
            return -errno;
        }

        __debug_trace("open %s", server->log.envar->value);
    }

    return 0;
}

STATIC void
jlogd_jtrycut(void)
{
    if (os_hasbit(jlogd.event, SIGUSR1)) {
        jlog_server_t *server;
        
        os_clrbit(jlogd.event, SIGUSR1);

        foreach_server(server) {
            jlogd_jcut(server, 0);
        }
    }
}

STATIC int
jlogd_jadd(jlog_server_t *server)
{
    int len = 0, pri, err;
    jobj_t obj = NULL;
    
    obj = jobj_byjson(jlogb);
    if (NULL==obj) {
        len = -EBADJSON; goto error;
    }

    jobj_t header = jobj_get(obj, JLOG_KEY_HEADER);
    if (NULL==header) {
        __debug_error("no-found header");
        
        len = -EFORMAT; goto error;
    }
    
    jobj_t opri = jobj_get(header, JLOG_KEY_PRI);
    if (NULL==opri) {
        __debug_error("no-found pri");
        
        len = -EFORMAT; goto error;
    }
    pri = LOG_PRI(jobj_get_i32(opri));
    
    err = jobj_add_u64(header, JLOG_KEY_SEQ, ++jlogd.seq);
    if (err<0) {
        __debug_error("add seq error");
        
        len = err; goto error;
    }

    char *mac = os_getmacby(SCRIPT_GETBASEMAC);
    if (mac) {
        err = jobj_add_string(header, JLOG_KEY_MAC, mac);
        if (err<0) {
            __debug_error("add mac error");
            
            len = err; goto error;
        }
    }

    char *json = jobj_json(obj);
    if (NULL==json) {
        __debug_error("obj==>json failed");
        
        len = -EFORMAT; goto error;
    }
    len = os_strlen(json);
    
    os_strmcpy(jlogb, json, len);
    
    if (pri <= ak_var_get(&jlogd.pri)) {
        os_system(SCRIPT_PUSH " '%s' &", jlogb);
    }
    
error:
    jobj_put(obj);
    
    return len;
}

STATIC int
jlogd_jhandle(jlog_server_t *server)
{
    int err, len;
    os_sockaddr_t client = OS_SOCKADDR_INITER(server->family);
    socklen_t addrlen = os_sockaddr_len(&client.c);
    
    len = __io_recvfrom(server->fd, jlogb, sizeof(jlogb), 0, &client.c, &addrlen);
    if (len<0) {
        __debug_error("read error:%d", len);
        return len;
    }

    if (is_abstract_sockaddr(&client.c)) {
        set_abstract_sockaddr_len(&client.un, addrlen);

        __debug_trace("recv from:%s", get_abstract_path(&client.un));
    }
    
    jlogb[len] = 0;
    __debug_trace("read:%s, len:%d", jlogb, len);

    err = jlogd_jadd(server);
    if (err<0) { /* yes, <0 */
        /* log */
    } else {
        len = err;
    }

    jlogb[len++] = '\n';
    jlogb[len] = 0;

    err = os_fwrite(server->log.stream, jlogb, len);
        __debug_trace_error(err, "write %s", server->log.envar->value);
    err = os_fwrite(server->cache.stream, jlogb, len);
        __debug_trace_error(err, "write %s", server->cache.envar->value);

    server->log.count++;
    server->cache.count++;

    int cut = (int)ak_var_get(&jlogd.cut);
    if (server->log.count >= cut) {
        return jlogd_jcut(server, cut);
    } else {
        return 0;
    }
}

STATIC int 
jlogd_handle(loop_watcher_t *watcher, time_t now)
{
    jlog_server_t *server = (jlog_server_t *)watcher->user;

    jlogd_jhandle(server);

    jlogd_jtrycut();
}

STATIC int
jlogd_init_env(void) 
{
    jlog_server_t *server;
    int err;

    ak_var_init(&jlogd.pri);
    ak_var_init(&jlogd.timeout);
    ak_var_init(&jlogd.cut);
    
    foreach_server(server) {
        err = env_string_init(server->log.envar);
        if (err<0) {
            return err;
        }
        
        err = env_string_init(server->cache.envar);
        if (err<0) {
            return err;
        }
    }

    err = __env_copy(OS_ENV(UNIX), JLOG_UNIX, 
            get_abstract_path(&jlogd.server[JLOGD_USERVER].addr.un), 
            abstract_path_size);
    if (err<0) {
        return err;
    }

    jlogd.server[JLOGD_ISERVER].addr.in.sin_port = env_geti(OS_ENV(PORT), JLOG_PORT);
    {
        uint32 ip;
        char ipaddress[32] = {0};

        err = env_copy(OS_ENV(SERVER), JLOG_SERVER, ipaddress);
        if (err<0) {
            return err;
        }
        
        ip = os_ipaddr(ipaddress);
        if (false==is_good_ipaddr(ip)) {
            return -EFORMAT;
        }
        
        jlogd.server[JLOGD_ISERVER].addr.in.sin_addr.s_addr = ip;
    }
    
    return 0;
}

STATIC int
jlogd_init_server(jlog_server_t *server)
{
    int fd, err, protocol, addrlen, cut = (int)ak_var_get(&jlogd.cut);

    err = jlogd_jcut(server, cut);
    if (err<0) {
        return err;
    }

    if (AF_UNIX==server->family) {
        protocol    = 0;
        addrlen     = get_abstract_sockaddr_len(&server->addr.un);
    } else {
        protocol    = IPPROTO_UDP;
        addrlen     = sizeof(sockaddr_in_t);
    }
    
    fd = socket(server->family, SOCK_DGRAM, protocol);
    if (false==is_good_fd(fd)) {
    	__debug_error("server %s create socket error:%d", server->name, -errno);
        return -errno;
    }
    os_closexec(fd);

    err = bind(fd, &server->addr.c, addrlen);
    if (err<0) {
        __debug_error("server %s bind error:%d", server->name, -errno);
        return -errno;
    }
    
    server->fd = fd;

    os_loop_add_normal(&jlogd.loop, fd, jlogd_handle, server);
    
    return 0;
}

STATIC int 
jlogd_init_all_server(void)
{
    jlog_server_t *server;
    int err;

    foreach_server(server) {
        err = jlogd_init_server(server);
            __debug_trace_error(err, "init %s", server->name);
        if (err < 0) {
            return err;
        }
    }
    
    return 0;
}

STATIC void
jlogd_usr1(int sig)
{
    __debug_signal("recv USR1");
    
    if (false==os_hasbit(jlogd.event, SIGUSR1)) {
        os_setbit(jlogd.event, SIGUSR1);
    }
}

STATIC int 
jlogd_main_helper(int argc, char *argv[])
{
    int err;

    os_system_helper("mkdir -p " JLOG_LPATH " " JLOG_RPATH);

    err = jlogd_init_env();
        __debug_trace_error(err, "init env");
    if (err<0) {
        return err;
    }

    err = jlogd_init_all_server();
    if (err<0) {
        return err;
    }

    return os_loop(&jlogd.loop);
}

int allinone_main(int argc, char *argv[])
{
    int cut[] = {SIGUSR1};

    setup_signal(jlogd_usr1, cut);
    setup_signal_exit(NULL);
    setup_signal_ignore(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(jlogd_main_helper, argc, argv);
}
/******************************************************************************/
