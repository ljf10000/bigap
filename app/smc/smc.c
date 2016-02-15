/*******************************************************************************
Copyright (c) 2015-2016, xxx Networks. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      smc
#endif

#define __SIMPILE_RES__

#include "utils.h"
#include "sm/sm.h"

OS_INITER;
DECLARE_SIMPILE_RES;

static simpile_client_t smc = SIMPILE_CLIENT_INITER(SM_TIMEOUT, SMD_UNIX);

static int
usage(int error)
{
    os_eprintln(__THIS_NAME " insert deamon name pidfile command");
    os_eprintln(__THIS_NAME " insert normal name command");
    os_eprintln(__THIS_NAME " remove name");
    os_eprintln(__THIS_NAME " show");

    return error;
}

#define smc_handle(_action, _argc, _argv) \
    simpile_c_handle(_action, true, _argc, _argv, &smc.server, &smc.client, smc.timeout)

static int
cmd_insert(int argc, char *argv[])
{
    char *type = argv[0];
    char *name = argv[1];
    char *pidfile = NULL;
    char *command = NULL;

    if (3==argc && 0==os_strcmp(type, "normal")) {
        command = argv[2];
    }
    else if (4==argc && 0==os_strcmp(type, "deamon")) {
        pidfile = argv[2];
        command = argv[3];
    }
    else {
        return usage(-EINVAL0);
    }

    if (os_strlen(name) > SM_NAMESIZE) {
        return usage(-ETOOBIG);
    }
    else if (os_strlen(command) > SM_CMDSIZE) {
        return usage(-ETOOBIG);
    }
    else if (os_strlen(pidfile) > SM_PIDFILE) {
        return usage(-ETOOBIG);
    }
    
    return smc_handle("insert", argc, argv);
}

static int
cmd_remove(int argc, char *argv[])
{
    char *name = argv[0];
    
    if (1!=argc) {
        return usage(-EINVAL1);
    }
    else if (os_strlen(name) > SM_NAMESIZE) {
        return usage(-ETOOBIG);
    }
    else {
        return smc_handle("remove", argc, argv);
    }
}

static int
cmd_show(int argc, char *argv[])
{
    if (0!=argc) {
        return usage(-EINVAL3);
    }
    else {
        return smc_handle("show", argc, argv);
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
    
    smc.timeout = get_sm_timeout_env();

    err = get_smc_path_env(&smc.client);
    if (err) {
        return err;
    }
    
    err = get_smd_path_env(&smc.server);
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
#define smc_main  main
#endif

int smc_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(__main, argc, argv);
}
/******************************************************************************/
