/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      smd
#endif

#define __DEAMON__
#define __THIS_USAGE \
    "smc usage:"                                            __crlf \
    __tab "smc insert deamon {name} {pidfile} {command}"    __crlf \
    __tab "smc insert normal {name} {command}"              __crlf \
    __tab "smc remove {name}"                               __crlf \
    __tab "smc show [name]"                                 __crlf \
    /* end */
#include "utils.h"
#include "sm/sm.h"
/******************************************************************************/
OS_INITER;

#ifndef SM_NORMAL_SLEEP
#define SM_NORMAL_SLEEP       500 /* ms */
#endif

#ifndef SM_DEAMON_WAIT
#define SM_DEAMON_WAIT        3 /* second */
#endif

#if 1
#define SM_STATE_ENUM_MAPPER(_)     \
    _(SM_STATE_INIT,  0, "init"),   \
    _(SM_STATE_FORK,  1, "fork"),   \
    _(SM_STATE_RUN,   2, "run"),    \
    _(SM_STATE_DIE,   3, "die"),    \
    /* end */
DECLARE_ENUM(sm_state, SM_STATE_ENUM_MAPPER, SM_STATE_END);

static inline enum_ops_t *sm_state_ops(void);
static inline bool is_good_sm_state(int id);
static inline char *sm_state_getnamebyid(int id);
static inline int sm_state_getidbyname(const char *name);

#define SM_STATE_INIT   SM_STATE_INIT
#define SM_STATE_FORK   SM_STATE_FORK
#define SM_STATE_RUN    SM_STATE_RUN
#define SM_STATE_DIE    SM_STATE_DIE
#define SM_STATE_END    SM_STATE_END
#endif

enum {
    SMD_HASH_NAME,
    SMD_HASH_PID,

    SMD_HASH_END
};
#define SMD_HASH_SIZE   256

typedef struct {
    STREAM f;    /* todo: cache entry */
    uint32 time;

    loop_t loop;

    h2_table_t table;
} smd_control_t;

#define SMD_INITER {                    \
    .loop = LOOP_INITER,                \
}   /* end */

static smd_control_t smd = SMD_INITER;

enum {
    SM_F_STATIC   = 0x01,
};

typedef struct {
    char *name;
    char *command;
    char *cname;
    char *pidfile;

    int pid;
    int forks;
    int state;
    int flags;
    int offset; /* todo: in cache file's offset */
    
    uint32 time[SM_STATE_END];
    
    h2_node_t node;
} sm_entry_t;

#define DAEMONS(_name, _command, _pidfile) {  \
    .name   = _name,                \
    .command= _command,             \
    .pidfile= _pidfile,             \
    .flags  = SM_F_STATIC,    \
}   /* end */

#if 1
DECLARE_DB_H2(&smd.table, smd, sm_entry_t, node);

static inline sm_entry_t *
smd_h2_entry(h2_node_t *node);

static inline sm_entry_t *
smd_hx_entry(hash_node_t *node, hash_idx_t nidx);

static inline int
smd_foreach(mv_t (*foreach)(sm_entry_t *entry), bool safe);
#endif

ALWAYS_INLINE bool
smd_is_normal(sm_entry_t *entry)
{
    return NULL==entry->pidfile;
}
#define smd_is_deamon(_entry)       (false==smd_is_normal(_entry))

ALWAYS_INLINE bool
smd_is_timeout(sm_entry_t *entry)
{
    return smd.time - entry->time[entry->state] > SM_DEAMON_WAIT;
}

ALWAYS_INLINE char *
smd_type(sm_entry_t *entry)
{
    return smd_is_normal(entry)?"normal":"deamon";
}

ALWAYS_INLINE bool
smd_is_run(sm_entry_t *entry)
{
    return SM_STATE_RUN==entry->state;
}

ALWAYS_INLINE bool
smd_have_pid(sm_entry_t *entry)
{
    return SM_STATE_FORK==entry->state || SM_STATE_RUN==entry->state;
}

STATIC int
smd_get_normal_pid(sm_entry_t *entry)
{
    int i, pid = 0;

    if (entry->pid) {
        return entry->pid;
    }
    
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
smd_get_deamon_pid(sm_entry_t *entry)
{
    int pid = __os_get_pid(entry->pidfile);

    debug_trace("get normal(%s) pid(%d)", entry->name, pid);

    return pid;
}

STATIC void
smd_set_time(sm_entry_t *entry, char *prefix)
{
    entry->time[entry->state] = smd.time;

    debug_trace("%s: set entry(%s) state(%s) time(%d)",
        prefix,
        entry->name, 
        sm_state_getnamebyid(entry->state),
        entry->time[entry->state]);
}

STATIC hash_idx_t
smd_hashname(char *name)
{
    return hash_bybuf(name, os_strlen(name), SMD_HASH_SIZE - 1);
}

STATIC hash_idx_t
smd_hashpid(int pid)
{
    return hash_bybuf((byte *)&pid, sizeof(pid), SMD_HASH_SIZE - 1);
}

STATIC hash_idx_t
smd_nodehashname(hash_node_t *node)
{
    sm_entry_t *entry = smd_hx_entry(node, SMD_HASH_NAME);

    return smd_hashname(entry->name);
}

STATIC hash_idx_t
smd_nodehashpid(hash_node_t *node)
{
    sm_entry_t *entry = smd_hx_entry(node, SMD_HASH_PID);

    return smd_hashpid(entry->pid);
}

STATIC int
__smd_remove(sm_entry_t *entry)
{
    if (os_hasflag(entry->flags, SM_F_STATIC)) {
        return 0;
    }
    else if (false==in_hx_table(&entry->node)) {
        return 0;
    }

    h2_del(&smd.table, &entry->node);

    debug_entry("insert %s:%d:%s", entry->name, entry->pid, entry->command);
    
    return 0;
}

STATIC int
__smd_insert(sm_entry_t *entry)
{
    static hash_node_calc_f *ncalc[SMD_HASH_END] = {
        [SMD_HASH_NAME] = smd_nodehashname, 
        [SMD_HASH_PID]  = smd_nodehashpid
    };
    
    if (in_hx_table(&entry->node)) {
        return 0;
    }
    
    h2_add(&smd.table, &entry->node, ncalc);

    debug_entry("insert %s:%d:%s", entry->name, entry->pid, entry->command);

    return 0;
}

STATIC void
smd_dump_all(char *name)
{
    mv_t foreach(sm_entry_t *entry)
    {
        debug_trace("entry:%s pid:%d forks:%d command:%s ", 
            entry->name,
            entry->pid,
            entry->forks,
            entry->command);

        return mv2_ok;
    }

    if (__is_ak_debug_trace) {
        smd_foreach(foreach, false);
    }
}

STATIC sm_entry_t *
smd_getbyname(char *name)
{
    sm_entry_t *entry;
    
    if (NULL==name) {
        return NULL;
    }

    hash_idx_t dcalc(void)
    {
        return smd_hashname(name);
    }
    
    bool eq(hash_node_t *node)
    {
        sm_entry_t *entry = smd_hx_entry(node, SMD_HASH_NAME);
        
        return os_streq(entry->name, name);
    }
    
    return smd_h2_entry(h2_find(&smd.table, SMD_HASH_NAME, dcalc, eq));
}

STATIC sm_entry_t *
smd_getbynormal(int pid)
{
    hash_idx_t dcalc(void)
    {
        return smd_hashpid(pid);
    }
    
    bool eq(hash_node_t *node)
    {
        sm_entry_t *entry = smd_hx_entry(node, SMD_HASH_PID);
        
        return pid==entry->pid;
    }
    
    return smd_h2_entry(h2_find(&smd.table, SMD_HASH_PID, dcalc, eq));
}

STATIC void
smd_change(sm_entry_t *entry, int state, int pid, char *prefix)
{
    int err;
    
    if (pid!=entry->pid) {
        debug_entry("%s: set entry(%s) state(%s==>%s), pid(%d==>%d)",
            prefix,
            entry->name, 
            sm_state_getnamebyid(entry->state),
            sm_state_getnamebyid(state),
            entry->pid, pid);
        
        __smd_remove(entry);
        entry->pid = pid;
        __smd_insert(entry);
    }

    if (state!=entry->state) {
        entry->state = state;
        smd_set_time(entry, prefix);
    }
}

STATIC int
smd_run(sm_entry_t *entry, char *prefix)
{
    int err, pid;
    
    pid = vfork();
    if (pid<0) {
        debug_trace("fork error:%d", -errno);
        
        return -errno;
    }
    else if (pid>0) { // father
        entry->forks++;
        
        smd_change(entry, SM_STATE_FORK, pid, prefix);

        jinfo("%o", 
            "exec", jobj_oprintf("%s%d%%d%s",
                        "name",     entry->name,
                        "pid",      entry->pid,
                        "forks",    entry->forks,
                        "command",  entry->command));

        return 0;
    }
    else { // child
#if 1
        char *argv[] = {"bash", "-c", entry->command, NULL};
        
        err = execvp("/bin/bash", argv);
        
        debug_error("exec %s error:%d", entry->command, -errno);
#else
        cmd_table_t *cmd = cmd_argv(entry->command);
        
        err = execvp(cmd->argv[0], cmd->argv);
        
        debug_error("exec %s error:%d", cmd->argv[0], -errno);
#endif
        exit(err);
    }
}

STATIC void
smd_die(sm_entry_t *entry, char *prefix)
{
    smd_change(entry, SM_STATE_DIE, 0, prefix);
}

STATIC void
smd_killall(sm_entry_t *entry)
{
    os_system("killall '%s'", entry->cname);
}

STATIC void
smd_kill_deamon(sm_entry_t *entry)
{
    if (smd_is_deamon(entry)) {
        smd_killall(entry);
    }
}

STATIC int
smd_kill(sm_entry_t *entry)
{
    if (false==smd_have_pid(entry)) {
        return 0;
    }
    
    int pid = entry->pid;
    if (os_pid_exist(pid)) {
        kill(pid, SIGKILL);
    }
    
    smd_kill_deamon(entry);
    
    jinfo("%o", 
        "kill", jobj_oprintf("%s%d%d%s",
                    "name",     entry->name,
                    "pid",      entry->pid,
                    "forks",    entry->forks,
                    "state",    sm_state_getnamebyid(entry->state)));
                    
    smd_die(entry, "in kill");

    return 0;
}

STATIC void
smd_wait_error(sm_entry_t *entry)
{
    char prefix[1+OS_LINE_SHORT] = {0};
    int pid = entry->pid;

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
smd_wait_ok(sm_entry_t *entry)
{
    char prefix[1+OS_LINE_SHORT] = {0};
    int pid = 0;
    
    if (smd_is_normal(entry)) {
        pid = entry->pid;
    }
    else {
        pid = smd_is_run(entry)?entry->pid:smd_get_deamon_pid(entry);
    }
    
    os_snprintf(prefix, OS_LINE_SHORT, "int wait %s(%s)",
        smd_type(entry),
        sm_state_getnamebyid(entry->state));

    if (os_pid_exist(pid)) {
        smd_change(entry, SM_STATE_RUN, pid, prefix);
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
    
    sm_entry_t *entry = smd_getbynormal(pid);
    if (NULL==entry) {
        debug_trace("wait son:%d, but no-found entry, maybe have been removed", pid);

        return;
    }

    if (smd_is_normal(entry)) {
        jinfo("%o", 
            "wait-son", jobj_oprintf("%s%d%d%s",
                            "name",     entry->name,
                            "pid",      entry->pid,
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
        smd_change(entry, entry->state, 0, "in wait son(deamon)");
    }
}

STATIC int
smd_wait(void)
{
    int i, err, pid, count = 0;
    
#if 0
    /*
    * use these code, some app always fork state
    */
    int pids[h2_count(&smd.table)];
    
    while((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        pids[count++] = pid;
    }

    for (i=0; i<count; i++) {
        smd_wait_son(pids[i]);
    }
#else
    while((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        smd_wait_son(pid);
    }
#endif

    /*
    * check run
    */
    mv_t foreach(sm_entry_t *entry)
    {       
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

        return mv2_ok;
    }
    
    err = smd_foreach(foreach, true);
    
    return err;
}

STATIC int
smd_insert(sm_entry_t *entry)
{
    int err, pid = 0;

    err = __smd_insert(entry);
    if (err<0) {
        return err;
    }
    
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
            debug_error("run %s error:%d", entry->name, err);
            
            return err;
        }
    } else {
        /*
        * re-init, just change state and save pid, needn't run it
        */
        if (smd_is_normal(entry)) {
            smd_change(entry, SM_STATE_RUN, pid, "in insert(normal exist)");
        } else {
            smd_change(entry, SM_STATE_RUN, pid, "in insert(deamon exist)");
        }
    }
    
    return 0;
}

STATIC void
smd_destroy(sm_entry_t *entry)
{
    if (entry) {
        os_free(entry->name);
        os_free(entry->cname);
        os_free(entry->command);
        os_free(entry->pidfile);

        os_free(entry);
    }
}

STATIC int
smd_remove(sm_entry_t *entry)
{
    int err;

    err = __smd_remove(entry);
    if (err<0) {
        return err;
    }

    smd_kill(entry);
    smd_destroy(entry);
    
    return 0;
}

STATIC sm_entry_t *
smd_create(char *name, char *command, char *pidfile)
{
    sm_entry_t *entry = (sm_entry_t *)os_zalloc(sizeof(*entry));
    if (NULL==entry) {            
        return NULL;
    }
    
    if (pidfile) {
        entry->pidfile  = os_strdup(pidfile);
    }
    entry->name     = os_strdup(name);
    
    entry->command  = os_strdup(command);
    cmd_table_t *cmd= cmd_argv(os_str_skip_env(command));
    entry->cname    = os_strdup(cmd->argv[0]);

    smd_set_time(entry, "in create");

    return entry;
}

STATIC int
smd_handle_insert(cli_table_t *table, int argc, char *argv[])
{
    char *type = argv[1];
    char *name = argv[2];
    char *pidfile = NULL;
    char *command = NULL;

    if (4==argc && os_streq(type, "normal")) {
        command = argv[3];
    }
    else if (5==argc && os_streq(type, "deamon")) {
        pidfile = argv[3];
        command = argv[4];
    }
    else {
        return cli_help(-EINVAL0);
    }

    if (os_strlen(name) > SM_NAMESIZE) {
        return cli_help(-ETOOBIG);
    }
    else if (os_strlen(command) > SM_CMDSIZE) {
        return cli_help(-ETOOBIG);
    }
    else if (os_strlen(pidfile) > SM_PIDFILE) {
        return cli_help(-ETOOBIG);
    }

    sm_entry_t *entry = smd_getbyname(name);
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

    int err = smd_insert(entry);
    if (err<0) {
        smd_remove(entry);

        return err;
    }
    
    return 0;
}

STATIC int
smd_handle_remove(cli_table_t *table, int argc, char *argv[])
{
    char *name = argv[1];
    
    if (2!=argc) {
        return cli_help(-EINVAL1);
    }
    else if (os_strlen(name) > SM_NAMESIZE) {
        return cli_help(-ETOOBIG);
    }

    sm_entry_t *entry = smd_getbyname(name);
    if (NULL==entry) {
        return -ENOEXIST;
    }

    return smd_remove(entry);
}

STATIC int
smd_handle_clean(cli_table_t *table, int argc, char *argv[])
{
    if (1!=argc) {
        return cli_help(-EINVAL2);
    }
    
    mv_t foreach(sm_entry_t *entry)
    {
        smd_remove(entry);

        return mv2_ok;
    }
    
    return smd_foreach(foreach, true);
}

STATIC void
smd_show(sm_entry_t *entry)
{
    cli_sprintf("%s %c%d/%d %d %s '%s'" __crlf,
        entry->name,
        os_hasflag(entry->flags, SM_F_STATIC)?'*':' ',
        smd_is_normal(entry)?entry->pid:0,
        smd_is_deamon(entry)?entry->pid:0,
        entry->forks,
        sm_state_getnamebyid(entry->state),
        entry->command);
}


STATIC int
smd_handle_show(cli_table_t *table, int argc, char *argv[])
{
    char *name = argv[1];
    bool empty = true;
    
    if (1!=argc && 2!=argc) {
        return cli_help(-EINVAL3);
    }
    else if (name && os_strlen(name) > SM_NAMESIZE) {
        return cli_help(-ETOOBIG);
    }
    
    cli_sprintf("#name pid/dpid forks state command" __crlf);

    mv_t foreach(sm_entry_t *entry)
    {
        if (NULL==name || os_streq(entry->name, name)) {
            smd_show(entry);

            empty = false;
        }

        return mv2_ok;
    }
    
    if (empty) {
        /*
        * drop head line
        */
        __clib_clear();
        
        debug_trace("show monitor(%s) nofound", name);
    }
    
    return smd_foreach(foreach, false);
}

STATIC int
smd_cli(loop_watcher_t *watcher, time_t now)
{
    static cli_table_t tables[] = {
        /*
        * help must first
        */
        CLI_TCP_ENTRY("help",   cli_handle_help),
        
        CLI_TCP_ENTRY("insert", smd_handle_insert),
        CLI_TCP_ENTRY("remove", smd_handle_remove),
        CLI_TCP_ENTRY("clean",  smd_handle_clean),
        CLI_TCP_ENTRY("show",   smd_handle_show),
    };

    return cli_response(watcher->fd, tables);
}

STATIC int
smd_init_server(void)
{
    int err;
    
    err = os_loop_cli_tcp(&smd.loop, smd_cli, NULL);
    if (err<0) {
        debug_error("add loop cli error:%d", err);
    }
    
    debug_ok("init server ok");
    
    return 0;
}

STATIC int
smd_timer(loop_watcher_t *watcher, time_t now)
{
    smd.time += SM_TIMER;

    if (0==(smd.time % os_second(CLI_TIMEOUT))) {
        smd_wait();
    }
    
    return 0;
}

STATIC int 
smd_init_timer(void)
{
    struct itimerspec tm = OS_ITIMESPEC_INITER(SM_TIMER, 0);
    
    return os_loop_add_timer(&smd.loop, smd_timer, &tm);
}

STATIC int
smd_load(void)
{
    static sm_entry_t deamons[] = {
#if 1
        /* insert static daemon */
#endif
    };
    
    int i;

    /*
    * load static
    */
    for (i=0; i<os_count_of(deamons); i++) {
        debug_trace("load static %s", deamons[i].name);
        
        smd_insert(&deamons[i]);
    }

    if (os_file_exist(SCRIPT_SMD_INIT)) {
        os_system_helper(SCRIPT_SMD_INIT_RUN);

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
    
    int hash_size[SMD_HASH_END] = { SMD_HASH_SIZE, SMD_HASH_SIZE };
    err = h2_init(&smd.table, hash_size);
    if (err<0) {
        return err;
    }

    return 0;
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

    return os_loop(&smd.loop);
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(smd_exit);
    setup_signal_ignore(NULL);
    setup_signal_callstack(NULL);
    
    int err = os_call(smd_init, smd_fini, smd_main_helper, argc, argv);

    return shell_error(err);
}
/******************************************************************************/
