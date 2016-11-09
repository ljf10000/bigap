/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      smc
#endif

#ifndef __COMMAND__
#define __COMMAND__
#endif

#define __CLI_TCP__     0
#include "utils.h"
#include "sm/sm.h"

OS_INITER;

static cli_client_t smc = CLI_CLIENT_INITER("smd");

#define smc_handle(_action, _argc, _argv) \
    clic_sync_handle(&smc, _action, _argc, _argv)

static int
usage(int error)
{
    os_eprintln(__THIS_APPNAME " insert deamon {name} {pidfile} {command}");
    os_eprintln(__THIS_APPNAME " insert normal {name} {command}");
    os_eprintln(__THIS_APPNAME " remove {name}");
    os_eprintln(__THIS_APPNAME " show [name]");

    return error;
}

static int
cmd_insert(int argc, char *argv[])
{
    char *type = argv[0];
    char *name = argv[1];
    char *pidfile = NULL;
    char *command = NULL;

    if (3==argc && os_streq(type, "normal")) {
        command = argv[2];
    }
    else if (4==argc && os_streq(type, "deamon")) {
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
cmd_clean(int argc, char *argv[])
{
    if (0!=argc) {
        return usage(-EINVAL2);
    }
    else {
        return smc_handle("clean", argc, argv);
    }
}

static int
cmd_show(int argc, char *argv[])
{
    char *name = argv[0];
    
    if (0!=argc && 1!=argc) {
        return usage(-EINVAL3);
    }
    else if (name && os_strlen(name) > SM_NAMESIZE) {
        return usage(-ETOOBIG);
    }
    else {
        return smc_handle("show", argc, argv);
    }
}

static int
command(int argc, char *argv[])
{
    static cli_table_t table[] = {
        CLI_ENTRY("insert", cmd_insert),
        CLI_ENTRY("remove", cmd_remove),
        CLI_ENTRY("clean",  cmd_clean),
        CLI_ENTRY("show",   cmd_show),
    };
    int err;

    err = cli_argv_handle(table, os_count_of(table), argc, argv);
    if (err<0) {
        debug_error("%s error:%d", argv[0], err);

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

    err = command(argc-1, argv+1);
    if (err<0) {
        /* just log, NOT return */
    }

    return err;
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(__main, argc, argv);
}
/******************************************************************************/
