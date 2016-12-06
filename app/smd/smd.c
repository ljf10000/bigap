/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      smd
#endif

#define __DEAMON__

#include "utils.h"
#include "sm/sm.h"

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
INLINE_ENUM(sm_state, SM_STATE_ENUM_MAPPER, SM_STATE_END);

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

typedef struct {
    STREAM f;    /* todo: cache entry */
    uint32 time;
    
    struct list_head list;

    loop_t loop;
} smd_control_t;

#define SMD_INITER {                    \
    .list = LIST_HEAD_INIT(smd.list),   \
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

    int normal;
    int deamon;
    int forks;
    int state;
    int flags;
    int offset; /* todo: in cache file's offset */
    
    uint32 time[SM_STATE_END];
    
    struct list_head node;
} sm_entry_t;

#define DAEMONS(_name, _command, _pidfile) {  \
    .name   = _name,                \
    .command= _command,             \
    .pidfile= _pidfile,             \
    .flags  = SM_F_STATIC,    \
}   /* end */

STATIC bool
smd_is_normal(sm_entry_t *entry)
{
    return NULL==entry->pidfile;
}
#define smd_is_deamon(_entry)     (false==smd_is_normal(_entry))

STATIC bool
smd_is_timeout(sm_entry_t *entry)
{
    return smd.time - entry->time[entry->state] > SM_DEAMON_WAIT;
}

STATIC char *
smd_type(sm_entry_t *entry)
{
    return smd_is_normal(entry)?"normal":"deamon";
}

STATIC bool
smd_is_run(sm_entry_t *entry)
{
    return SM_STATE_RUN==entry->state;
}

STATIC bool
smd_have_pid(sm_entry_t *entry)
{
    return SM_STATE_FORK==entry->state || SM_STATE_RUN==entry->state;
}

STATIC int
smd_get_normal_pid(sm_entry_t *entry)
{
    int i, pid = 0;

    if (entry->normal) {
        return entry->normal;
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

STATIC void
smd_change(sm_entry_t *entry, int state, int normal, int deamon, char *prefix)
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
    sm_entry_t *entry;

    if (__is_ak_debug_trace) {
        list_for_each_entry(entry, &smd.list, node) {
            os_println("smd_dump_all entry name:%s", entry->name);
        
            debug_trace("entry:%s pid:%d/%d forks:%d command:%s ", 
                entry->name,
                entry->normal, entry->deamon,
                entry->forks,
                entry->command);
        }
    }
}

STATIC sm_entry_t *
smd_getbyname(char *name)
{
    sm_entry_t *entry;
    
    if (NULL==name) {
        return NULL;
    }

    list_for_each_entry(entry, &smd.list, node) {
        os_println("smd_getbyname entry name:%s", entry->name);
            
        if (os_streq(name, entry->name)) {
            return entry;
        }
    }
    
    return NULL;
}

STATIC sm_entry_t *
smd_getbynormal(int pid)
{
    sm_entry_t *entry;

    list_for_each_entry(entry, &smd.list, node) {
        os_println("smd_getbynormal entry name:%s", entry->name);
        
        if (pid==entry->normal) {
            return entry;
        }
    }

    return NULL;
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
    smd_change(entry, SM_STATE_DIE, 0, 0, prefix);
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
smd_wait_error(sm_entry_t *entry)
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
smd_wait_ok(sm_entry_t *entry)
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
    
    sm_entry_t *entry = smd_getbynormal(pid);
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
    sm_entry_t *entry;
    char *prefix;

    while((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        smd_wait_son(pid);
    }
    os_println("smd_wait ...");
    
    /*
    * check run
    */
    list_for_each_entry(entry, &smd.list, node) {
        os_println("smd_wait entry name:%s", entry->name);
        
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
    os_println("smd_wait ok.");
    
    return 0;
}

STATIC int
smd_insert(sm_entry_t *entry)
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
            debug_error("run %s error:%d", entry->name, err);
            os_println("run %s error:%d", entry->name, err);
            
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

    os_println("smd_handle_insert get ...");
    sm_entry_t *entry = smd_getbyname(name);
    if (entry) {
        /*
        * have exist, do nothing
        */
        debug_trace("%s exist", name);
        
        return 0;
    }
    os_println("smd_handle_insert get ok.");

    os_println("smd_handle_insert create ...");
    entry = smd_create(name, command, pidfile);
    if (NULL==entry) {
        return -ENOMEM;
    }
    os_println("smd_handle_insert create ok.");

    os_println("smd_handle_insert insert ...");
    err = smd_insert(entry);
    if (err<0) {
        os_println("smd_handle_insert remove ...");
        smd_remove(entry);
        os_println("smd_handle_insert remove ok.");

        return err;
    }
    os_println("smd_handle_insert insert ok.");
    
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
    
    sm_entry_t *entry = smd_getbyname(name);
    if (NULL==entry) {
        return -ENOEXIST;
    }

    return smd_remove(entry);
}

STATIC int
smd_handle_clean(char *args)
{
    sm_entry_t *entry, *tmp;

    list_for_each_entry_safe(entry, tmp, &smd.list, node) {
        os_println("smd_handle_clean entry name:%s", entry->name);
        smd_remove(entry);
    }

    return 0;
}

STATIC void
smd_show(sm_entry_t *entry)
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
    sm_entry_t *entry;
    bool empty = true;
    
    cli_sprintf("#name pid/dpid forks state command" __crlf);

    list_for_each_entry(entry, &smd.list, node) {
        os_println("smd_handle_show entry name:%s", entry->name);
        
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
smd_cli(struct loop_watcher *watcher, time_t now)
{
    static cli_table_t table[] = {
        CLI_ENTRY("insert", smd_handle_insert),
        CLI_ENTRY("remove", smd_handle_remove),
        CLI_ENTRY("clean",  smd_handle_clean),
        CLI_ENTRY("show",   smd_handle_show),
    };
    int err, ret;

    os_println("smd_cli ...");
    ret = clis_handle(watcher->fd, table);
    os_println("smd_cli ok.");

    os_println("smd_cli del fd ...");
    err = os_loop_del_watcher(&smd.loop, watcher->fd);
    if (err<0) {
        debug_trace("del loop cli error:%d", err);
        
        return err;
    }
    os_println("smd_cli del fd ok.");
    
    return ret;
}

STATIC int
smd_init_server(void)
{
    int err;
    
    err = os_loop_add_cli(&smd.loop, smd_cli);
    if (err<0) {
        debug_error("add loop cli error:%d", err);
    }
    
    debug_ok("init server ok");
    
    return 0;
}

STATIC int
smd_timer(struct loop_watcher *watcher, time_t now)
{
    os_println("smd_timer %u ...", now);
    smd.time += SM_TIMER;

    if (0==(smd.time % os_second(CLI_TIMEOUT))) {
        smd_wait();
    }
    os_println("smd_timer %u ok", now);
    
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

    os_loop(&smd.loop);
    
    return 0;
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
