/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      smc
#endif

#include "utils.h"
#include "sm/sm.h"

OS_INITER;

static cli_client_t smc = CLI_CLIENT_INITER("smd");

#define smc_request(_table, _argc, _argv) \
    clic_request(&smc, _table, _argc, _argv)

STATIC int
smc_usage(int error)
{
    os_eprintln(__THIS_APPNAME " insert deamon {name} {pidfile} {command}");
    os_eprintln(__THIS_APPNAME " insert normal {name} {command}");
    os_eprintln(__THIS_APPNAME " remove {name}");
    os_eprintln(__THIS_APPNAME " show [name]");

    return error;
}

STATIC int
smc_cmd_insert(cli_table_t *table, int argc, char *argv[])
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
        return smc_usage(-EINVAL0);
    }

    if (os_strlen(name) > SM_NAMESIZE) {
        return smc_usage(-ETOOBIG);
    }
    else if (os_strlen(command) > SM_CMDSIZE) {
        return smc_usage(-ETOOBIG);
    }
    else if (os_strlen(pidfile) > SM_PIDFILE) {
        return smc_usage(-ETOOBIG);
    }
    
    return smc_request(table, argc, argv);
}

STATIC int
smc_cmd_remove(cli_table_t *table, int argc, char *argv[])
{
    char *name = argv[1];
    
    if (2!=argc) {
        return smc_usage(-EINVAL1);
    }
    else if (os_strlen(name) > SM_NAMESIZE) {
        return smc_usage(-ETOOBIG);
    }
    else {
        return smc_request(table, argc, argv);
    }
}

STATIC int
smc_cmd_clean(cli_table_t *table, int argc, char *argv[])
{
    if (1!=argc) {
        return smc_usage(-EINVAL2);
    }
    else {
        return smc_request(table, argc, argv);
    }
}

STATIC int
smc_cmd_show(cli_table_t *table, int argc, char *argv[])
{
    char *name = argv[1];
    
    if (1!=argc && 2!=argc) {
        return smc_usage(-EINVAL3);
    }
    else if (name && os_strlen(name) > SM_NAMESIZE) {
        return smc_usage(-ETOOBIG);
    }
    else {
        return smc_request(table, argc, argv);
    }
}

STATIC int
smc_command(int argc, char *argv[])
{
    static cli_table_t tables[] = {
        CLI_TCP_ENTRY("insert", smc_cmd_insert),
        CLI_TCP_ENTRY("remove", smc_cmd_remove),
        CLI_TCP_ENTRY("clean",  smc_cmd_clean),
        CLI_TCP_ENTRY("show",   smc_cmd_show),
    };
    int err;

    err = cli_argv_handle(tables, argc, argv);
    if (err<0) {
        debug_error("%s error:%d", argv[0], err);

        return err;
    }
    
    return 0;
}

STATIC int 
smc_main_helper(int argc, char *argv[])
{
    int err;
    
    if (1==argc) {
        return smc_usage(-EHELP);
    }
    
    err = smc_command(argc-1, argv+1);
    if (err<0) {
        /* just log, NOT return */
    }

    return err;
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_ignore(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(smc_main_helper, argc, argv);
}
/******************************************************************************/
