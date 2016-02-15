/*******************************************************************************
Copyright (c) 2015-2016, xxx Networks. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      tmc
#endif

#define __SIMPILE_RES__

#include "utils.h"
#include "tm/tm.h"

OS_INITER;
DECLARE_SIMPILE_RES;

static simpile_client_t tmc = SIMPILE_CLIENT_INITER(TM_TIMEOUT, TMD_UNIX);

static int
usage(int error)
{
    os_eprintln(__THIS_NAME " insert name delay(second) interval(second) limit command");
    os_eprintln(__THIS_NAME " remove name");
    os_eprintln(__THIS_NAME " show [name]");

    return error;
}

#define stimerc_handle(_action, _argc, _argv) \
    simpile_c_handle(_action, true, _argc, _argv, &tmc.server, &tmc.client, tmc.timeout)

static int
cmd_insert(int argc, char *argv[])
{
    char *name      = argv[0];
    char *delay     = argv[1];
    char *interval  = argv[2];
    char *limit     = argv[3];
    char *command   = argv[4];
    
    if (5!=argc) {
        return usage(-EINVAL0);
    }
    else if (os_strlen(name) > TM_NAMESIZE) {
        return usage(-ETOOBIG);
    }
    else if (os_strlen(command) > TM_CMDSIZE) {
        return usage(-ETOOBIG);
    }
    
    int i_delay     = os_atoi(delay);
    int i_interval  = os_atoi(interval);
    int i_limit     = os_atoi(limit);
    
    if (false==is_good_tm_args(i_delay, i_interval, i_limit)) {
        return usage(-EINVAL);
    }
    
    return stimerc_handle("insert", argc, argv);
}

static int
cmd_remove(int argc, char *argv[])
{
    char *name = argv[0];
    
    if (1!=argc) {
        return usage(-EINVAL1);
    }
    else if (os_strlen(name) > TM_NAMESIZE) {
        return usage(-ETOOBIG);
    }
    else {
        return stimerc_handle("remove", argc, argv);
    }
}

static int
cmd_show(int argc, char *argv[])
{
    char *name = argv[0];
    
    if (0!=argc && 1!=argc) {
        return usage(-EINVAL2);
    }
    else if (name && os_strlen(name) > TM_NAMESIZE) {
        return usage(-ETOOBIG);
    }
    else {
        return stimerc_handle("show", argc, argv);
    }
}

static int
command(int argc, char *argv[])
{
    static cmd_table_t table[] = {
        CMD_ENTRY("insert",  cmd_insert),
        CMD_ENTRY("remove",  cmd_remove),
        CMD_ENTRY("show",    cmd_show),
    };
    int err;

    err = cmd_argv_handle(table, os_count_of(table), argc, argv);
    if (err) {
        debug_error("%s error:%d", argv[0], err);

        return err;
    }
    
    return 0;
}

static int
init_env(void) 
{
    int err;
    
    tmc.timeout = get_tm_timeout_env();

    err = get_tmc_path_env(&tmc.client);
    if (err) {
        return err;
    }
    
    err = get_tmd_path_env(&tmc.server);
    if (err) {
        return err;
    }

    return 0;
}

static int
__main(int argc, char *argv[])
{
    int err;
    
    if (1==argc) {
        return usage(-EHELP);
    }
    
    err = init_env();
        debug_trace_error(err, "init env");
    if (err) {
        return err;
    }
    
    err = command(argc-1, argv+1);
    if (err) {
        /* just log, NOT return */
    }

    return err;
}

#ifndef __BUSYBOX__
#define tmc_main  main
#endif

int tmc_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(__main, argc, argv);
}
/******************************************************************************/
