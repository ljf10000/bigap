/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      tmd
#endif

#define __DEAMON__

#define __THIS_USAGE \
    "tmc usage:"                                                            __crlf \
    __tab "tmc insert {name} delay(second) interval(second) limit command"  __crlf \
    __tab "tmc remove {name}"                                               __crlf \
    __tab "tmc show [name]"                                                 __crlf \
    /* end */
#include "utils.h"
#include "tm/tm.h"
/******************************************************************************/
OS_INITER;

typedef struct {    
    struct {
        int fd;
        int timeout;/* ms */
    } timer;

    struct {
        int fd;
        sockaddr_un_t addr;
    } server;

    h1_table_t table;

    loop_t loop;
} tmd_control_t;

#define TMD_INITER  {           \
    .timer = {                  \
        .fd     = INVALID_FD,   \
        .timeout= CLI_TIMEOUT,  \
    },                          \
    .loop = LOOP_INITER,        \
                                \
    .server = {                 \
        .fd     = INVALID_FD,   \
        .addr   = OS_SOCKADDR_ABSTRACT("tmd"), \
    },                          \
}   /* end */

static tmd_control_t tmd = TMD_INITER;

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

#if 1
DECLARE_DB_H1(&tmd.table, tmd, struct xtimer, node);

static inline struct xtimer *
tmd_hx_entry(hash_node_t *node);

static inline struct xtimer *
tmd_h1_entry(h1_node_t *node);

static inline int
tmd_foreach(mv_t (*foreach)(struct xtimer *entry), bool safe);
#endif

STATIC struct xtimer *
tmd_tm_entry(tm_node_t *timer)
{
    return tm_timer_entry(timer, struct xtimer, timer);
}

STATIC hash_idx_t 
tmd_hash(char *name)
{
    return os_str_bkdr(name) & (h1_size(&tmd.table) - 1);
}

STATIC hash_idx_t 
tmd_nhash(hash_node_t *node)
{
    struct xtimer *entry = tmd_hx_entry(node);
    
    return tmd_hash(entry->name);
}

STATIC int
tmd_insert(struct xtimer *entry)
{
    return h1_add(&tmd.table, &entry->node, tmd_nhash);
}

STATIC int
tmd_remove(struct xtimer *entry)
{
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
        struct xtimer *entry = tmd_hx_entry(node);
        
        return os_straeq(entry->name, name);
    }

    return tmd_h1_entry(h1_find(&tmd.table, hash, eq));
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
    struct xtimer *entry = tmd_tm_entry(timer);
    
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
tmd_handle_insert(cli_table_t *table, int argc, char *argv[])
{
    char *name      = argv[1];
    char *delay     = argv[2];
    char *interval  = argv[3];
    char *limit     = argv[4];
    char *command   = argv[5];
    int err;

    if (6!=argc) {
        return cli_help(-EINVAL0);
    }
    else if (os_strlen(name) > TM_NAMESIZE) {
        return cli_help(-ETOOBIG);
    }
    else if (os_strlen(command) > TM_CMDSIZE) {
        return cli_help(-ETOOBIG);
    }
    
    debug_cli("name:%s delay:%s interval:%s, limit:%s, command:%s", 
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

        return cli_help(-EINVAL);
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

    if (false==tm_is_pending(&entry->timer)) {
        err = tm_insert(&entry->timer, 
                os_ms2tick(1000*after, TM_TICKS),
                tmd_xtimer_cb, 
                true);
        if (err<0) {
            return -EEXIST;
        }
    }
    
    return 0;
}

STATIC int
tmd_handle_remove(cli_table_t *table, int argc, char *argv[])
{
    char *name = argv[1];
    
    if (2!=argc) {
        return cli_help(-EINVAL1);
    }
    else if (os_strlen(name) > TM_NAMESIZE) {
        return cli_help(-ETOOBIG);
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
tmd_handle_clean(cli_table_t *table, int argc, char *argv[])
{
    if (1!=argc) {
        return cli_help(-EINVAL1);
    }
    
    mv_t foreach(struct xtimer *entry)
    {
        tmd_remove(entry);

        return mv2_ok;
    }
    
    tmd_foreach(foreach, true);
    
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
tmd_handle_show(cli_table_t *table, int argc, char *argv[])
{
    char *name = argv[1];
    
    if (1!=argc && 2!=argc) {
        return cli_help(-EINVAL2);
    }
    else if (name && os_strlen(name) > TM_NAMESIZE) {
        return cli_help(-ETOOBIG);
    }

    bool empty = true;

    cli_sprintf("#name delay interval limit triggers left command" __crlf);
    
    mv_t foreach(struct xtimer *entry)
    {
        if (NULL==name || os_straeq(entry->name, name)) {
            tmd_show(entry);

            empty = false;
        }

        return mv2_ok;
    }
    
    tmd_foreach(foreach, false);
    
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
tmd_init_env(void) 
{
    tm_unit_set(TM_TICKS);

    return 0;
}

STATIC int
tmd_timer(loop_watcher_t *watcher, time_t now)
{
    tm_fd_trigger(watcher->fd);

    return 0;
}

STATIC int
tmd_init_timer(void)
{
    struct itimerspec tm = OS_ITIMESPEC_INITER(os_second(TM_TICKS), os_nsecond(TM_TICKS));
    
    return os_loop_add_timer(&tmd.loop, tmd_timer, &tm);
}

STATIC int
tmd_cli(loop_watcher_t *watcher, time_t now)
{
    static cli_table_t tables[] = {
        /*
        * help must first
        */
        CLI_TCP_ENTRY("help",   cli_handle_help),

        CLI_TCP_ENTRY("insert", tmd_handle_insert),
        CLI_TCP_ENTRY("remove", tmd_handle_remove),
        CLI_TCP_ENTRY("clean",  tmd_handle_clean),
        CLI_TCP_ENTRY("show",   tmd_handle_show),
    };

    return cli_response(watcher->fd, tables);
}

STATIC int
tmd_init_server(void)
{
    int err;
    
    err = os_loop_cli_tcp(&tmd.loop, tmd_cli, NULL);
    if (err<0) {
        debug_error("add loop cli error:%d", err);
    }
    
    debug_ok("init server ok");
    
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
        os_system_helper(SCRIPT_TMD_INIT_RUN);

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

    err = tmd_init_timer();
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
    return os_loop(&tmd.loop);
}

int allinone_main(int argc, char *argv[])
{    
    setup_signal_exit(NULL);
    setup_signal_ignore(NULL);
    setup_signal_callstack(NULL);

    int err = os_call(tmd_init, tmd_fini, tmd_main_helper, argc, argv);

    return shell_error(err);
}
/******************************************************************************/
