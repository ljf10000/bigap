/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      tmd
#endif

#define __DEAMON__
#define __CLI_TCP__     0

#include "utils.h"
#include "tm/tm.h"

OS_INITER;

static struct {    
    struct {
        int fd;
        int timeout;/* ms */
    } timer;

    struct {
        int fd;
        sockaddr_un_t addr;
    } server;

    h1_table_t table;
}
tmd = {
    .timer = {
        .fd     = INVALID_FD,
        .timeout= CLI_TIMEOUT,
    },

    .server = {
        .fd     = INVALID_FD,
        .addr   = OS_SOCKADDR_ABSTRACT("tmd"),
    },
};

struct xtimer {
    char name[1+TM_NAMESIZE];
    char command[1+TM_CMDSIZE];
    
    int delay;      /* second */
    int interval;   /* second */
    int limit;
    
    int triggers;

    h1_node_t node;
    tm_node_t timer;
};

STATIC struct xtimer *
tmd_get_entry_bytm(tm_node_t *timer)
{
    return container_of(timer, struct xtimer, timer);
}

STATIC struct xtimer *
tmd_get_entry_byhashnode(hash_node_t *node)
{
    return hx_entry(node, struct xtimer, node, 0);
}

STATIC struct xtimer *
tmd_get_entry_byh1(h1_node_t *node)
{
    return container_of(node, struct xtimer, node);
}

STATIC hash_idx_t 
tmd_hash(char *name)
{
    return os_str_bkdr(name) & (h1_size(&tmd.table) - 1);
}

STATIC int
tmd_insert(struct xtimer *entry)
{
    if (NULL==entry) {
        return -EKEYNULL;
    }

    hash_idx_t hash(hash_node_t *node)
    {
        return tmd_hash(entry->name);
    }
    
    return h1_add(&tmd.table, &entry->node, hash);
}

STATIC int
tmd_remove(struct xtimer *entry)
{
    if (NULL==entry) {
        return -EKEYNULL;
    }

    tm_remove(&entry->timer);

    return h1_del(&tmd.table, &entry->node);
}

STATIC struct xtimer *
tmd_get(char *name)
{
    if (NULL==name) {
        return NULL;
    }

    hash_idx_t hash(void)
    {
        return tmd_hash(name);
    }
    
    bool eq(hash_node_t *node)
    {
        struct xtimer *entry = tmd_get_entry_byhashnode(node);
        
        return os_straeq(entry->name, name);
    }

    h1_node_t *node = h1_find(&tmd.table, hash, eq);
    if (NULL==node) {
        return NULL;
    }

    return tmd_get_entry_byh1(node);
}

STATIC int
tmd_foreach(mv_t (*cb)(struct xtimer *entry), bool safe)
{
    mv_t foreach(h1_node_t *node)
    {
        struct xtimer *entry = tmd_get_entry_byh1(node);

        return (*cb)(entry);
    }

    if (safe) {
        return h1_foreach_safe(&tmd.table, foreach);
    } else {
        return h1_foreach(&tmd.table, foreach);
    }
}

STATIC struct xtimer *
tmd_create(char *name, char *command, int delay, int interval, int limit)
{
    struct xtimer *entry = tmd_get(name);

    if (NULL==entry) {
        /*
        * no found, create new
        */
        entry = (struct xtimer *)os_zalloc(sizeof(*entry));
        if (NULL==entry) {
            return NULL;
        }

        os_strdcpy(entry->name, name);
    } else if (false==tm_is_pending(&entry->timer)) {
        /*
        * have exist and timer is NOT pending, re-use it
        */
        tm_remove(&entry->timer);
    } else {
        /*
        * have exist and timer is pending, do nothing
        */
        return entry;
    }
    
    os_strdcpy(entry->command, command);
    
    entry->delay    = delay;
    entry->interval = interval;
    entry->limit    = limit;
    
    entry->triggers = 0;
    
    tmd_insert(entry);

    return entry;
}

STATIC void
tmd_destroy(struct xtimer *entry)
{
    if (entry) {
        tmd_remove(entry);
        tm_remove(&entry->timer);
        os_free(entry);
    }
}

STATIC bool
tmd_is_cycle(struct xtimer *entry)
{
    if (entry->interval) {
        return entry->limit?false:true;
    } else {
        return false;
    }
}

STATIC mv_t 
tmd_xtimer_cb(tm_node_t *timer)
{
    struct xtimer *entry = tmd_get_entry_bytm(timer);
    
    entry->triggers++;
    os_system("%s &", entry->command);

    debug_trace("trigger timer:%s"
                ", command:%s"
                ", delay:%d"
                ", interval:%d"
                ", limit:%d"
                ", triggers:%d",
                entry->name,
                entry->command,
                entry->delay,
                entry->interval,
                entry->limit,
                entry->triggers);

    if (tmd_is_cycle(entry) || entry->triggers < entry->limit) {
        debug_trace("re-insert timer:%s", entry->name);
        
        tm_insert(&entry->timer, 
            os_ms2tick(1000*entry->interval, TM_TICKS),
            tmd_xtimer_cb, 
            true);
    }

    return tm_SAFE(0);
}

STATIC int
tmd_handle_insert(char *args)
{
    char *name      = args; cli_shift(args);
    char *delay     = args; cli_shift(args);
    char *interval  = args; cli_shift(args);
    char *limit     = args; cli_shift(args);
    char *command   = args;
    int err;
    
    if (NULL==name      ||
        NULL==delay     ||
        NULL==interval  ||
        NULL==limit     ||
        NULL==command) {
        debug_error("NULL name|delay|interval|limit|command");
        
        return -EINVAL1;
    }

    debug_test("name:%s delay:%s interval:%s, limit:%s, command:%s", 
        name, 
        delay,
        interval,
        limit,
        command);

    int i_delay     = os_atoi(delay);
    int i_interval  = os_atoi(interval);
    int i_limit     = os_atoi(limit);
    
    if (false==is_good_tm_args(i_delay, i_interval, i_limit)) {
        debug_error("invalid args, delay:%d, interval:%d, limit:%d",
            i_delay, i_interval, i_limit);
        
        return -EINVAL2;
    }

    struct xtimer *entry = tmd_create(name, command, i_delay, i_interval, i_limit);
    if (NULL==entry) {
        return -ENOMEM;
    }

    int after = i_delay?i_delay:1;
    
    debug_trace("insert timer:%s"
                ", command:%s"
                ", delay:%d"
                ", interval:%d"
                ", limit:%d"
                ", after:%d"
                ", cycle:%d"
                ", pending:%s",
                entry->name,
                entry->command,
                entry->delay,
                entry->interval,
                entry->limit,
                after,
                tmd_is_cycle(entry),
                tm_is_pending(&entry->timer)?__true:__false);
    
    err = tm_insert(&entry->timer, 
            os_ms2tick(1000*after, TM_TICKS),
            tmd_xtimer_cb, 
            true);
    if (err<0) {
        return -EEXIST;
    }

    return 0;
}

STATIC int
tmd_handle_remove(char *args)
{
    char *name = args; cli_shift(args);
    if (NULL==name) {
        debug_trace("remove timer without name");
        
        return -EINVAL3;
    }
    
    struct xtimer *entry = tmd_get(name);
    if (NULL==entry) {
        debug_trace("remove timer(%s) nofound", name);
        
        return -ENOEXIST;
    }

    debug_trace("remove timer(%s)", name);
    
    tmd_destroy(entry);
    
    return 0;
}

STATIC int
tmd_handle_clean(char *args)
{
    mv_t cb(struct xtimer *entry)
    {
        tmd_remove(entry);

        return mv2_ok;
    }
    
    tmd_foreach(cb, true);
    
    return 0;
}

STATIC void
tmd_show(struct xtimer *entry)
{
    cli_sprintf("%s %d %d %d %d %d %s" __crlf,
        entry->name,
        entry->delay,
        entry->interval,
        entry->limit,
        entry->triggers,
        tm_left(&entry->timer) * TM_TICKS / 1000,
        entry->command);
}

STATIC int
tmd_handle_show(char *args)
{
    char *name = args; cli_shift(args);
    bool empty = true;
    
    cli_sprintf("#name delay interval limit triggers left command" __crlf);
    
    mv_t cb(struct xtimer *entry)
    {
        if (NULL==name || os_straeq(entry->name, name)) {
            tmd_show(entry);

            empty = false;
        }

        return mv2_ok;
    }
    
    tmd_foreach(cb, false);
    
    if (empty) {
        /*
        * drop head line
        */
        __clib_clear();

        debug_trace("show timer(%s) nofound", name);
    }

    return 0;
}

STATIC int
tmd_server_handle_one(fd_set *r)
{
    static cli_table_t table[] = {
        CLI_ENTRY("insert", tmd_handle_insert),
        CLI_ENTRY("remove", tmd_handle_remove),
        CLI_ENTRY("clean",  tmd_handle_clean),
        CLI_ENTRY("show",   tmd_handle_show),
    };
    int err = 0;
    
    if (FD_ISSET(tmd.timer.fd, r)) {
        err = tm_fd_trigger(tmd.timer.fd);
    }

    if (FD_ISSET(tmd.server.fd, r)) {
        err = clis_handle(tmd.server.fd, table);
    }

    return err;
}

STATIC int
tmd_server_handle(void)
{
    fd_set rset;
    struct timeval tv = {
        .tv_sec     = os_second(tmd.timer.timeout),
        .tv_usec    = os_usecond(tmd.timer.timeout),
    };
    int maxfd = OS_MAX(tmd.timer.fd, tmd.server.fd);
    int err;

    while(1) {
        FD_ZERO(&rset);
        FD_SET(tmd.timer.fd, &rset);
        FD_SET(tmd.server.fd, &rset);
        
        err = select(maxfd+1, &rset, NULL, NULL, &tv);
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
                return tmd_server_handle_one(&rset);
        }
    }
}

STATIC int
tmd_init_env(void) 
{
    tm_unit_set(TM_TICKS);

    return 0;
}

STATIC int
tmd_init_timerfd(void)
{
    int fd = tm_fd(os_second(TM_TICKS), os_nsecond(TM_TICKS));
    if (fd<0) {
        return fd;
    } else {
        tmd.timer.fd = fd;

        return 0;
    }
}

STATIC int
tmd_init_server(void)
{
    int fd;
    int err;
    
    fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (fd<0) {
    	debug_error("socket error:%d", -errno);
        return -errno;
    }
    os_closexec(fd);
    
    err = bind(fd, (sockaddr_t *)&tmd.server.addr, get_abstract_sockaddr_len(&tmd.server.addr));
    if (err<0) {
        debug_error("bind error:%d", -errno);
        return -errno;
    }

    tmd.server.fd = fd;

    return 0;
}

#ifndef TMD_HASHSIZE
#define TMD_HASHSIZE    1024
#endif

STATIC int
tmd_fini(void)
{
    tm_fini();
    os_fini();
    
    return 0;
}

STATIC int
tmd_load(void)
{
    if (os_file_exist(SCRIPT_TMD_INIT)) {
        __os_system(SCRIPT_TMD_INIT_RUN);

        debug_trace(SCRIPT_TMD_INIT);
    }

    return 0;
}

STATIC void
tmd_exit(int sig)
{
    tmd_fini();
    
    exit(sig);
}

STATIC int
tmd_init(void)
{
    int err;
    
    err = os_init();
    if (err<0) {
        return err;
    }

    tm_init();
    
    err = h1_init(&tmd.table, TMD_HASHSIZE);
        debug_trace_error(err, "tmd_init h1");
    if (err<0) {
        return err;
    }

    tmd_load();
    
    err = tmd_init_env();
        debug_trace_error(err, "init env");
    if (err<0) {
        return err;
    }

    err = tmd_init_timerfd();
        debug_trace_error(err, "init timerfd");
    if (err<0) {
        return err;
    }
    
    err = tmd_init_server();
        debug_trace_error(err, "init server");
    if (err<0) {
        return err;
    }
    
    return 0;
}

STATIC int 
tmd_main_helper(int argc, char *argv[])
{
    while (1) {
        tmd_server_handle();
    }
    
    return 0;
}

int allinone_main(int argc, char *argv[])
{    
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    int err = os_call(tmd_init, tmd_fini, tmd_main_helper, argc, argv);

    return shell_error(err);
}
/******************************************************************************/
