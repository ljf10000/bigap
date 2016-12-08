/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      tmc
#endif

#include "utils.h"
#include "tm/tm.h"

OS_INITER;

static cli_client_t tmc = CLI_CLIENT_INITER("tmd");

#define tmc_request(_table, _argc, _argv) \
    clic_request(&tmc, _table, _argc, _argv)

STATIC int
tmc_usage(int error)
{
    os_eprintln(__THIS_APPNAME " insert {name} delay(second) interval(second) limit command");
    os_eprintln(__THIS_APPNAME " remove {name}");
    os_eprintln(__THIS_APPNAME " show [name]");

    return error;
}

STATIC int
tmc_cmd_insert(cli_table_t *table, int argc, char *argv[])
{
    char *name      = argv[1];
    char *delay     = argv[2];
    char *interval  = argv[3];
    char *limit     = argv[4];
    char *command   = argv[5];
    
    if (6!=argc) {
        return tmc_usage(-EINVAL0);
    }
    else if (os_strlen(name) > TM_NAMESIZE) {
        return tmc_usage(-ETOOBIG);
    }
    else if (os_strlen(command) > TM_CMDSIZE) {
        return tmc_usage(-ETOOBIG);
    }
    
    int i_delay     = os_atoi(delay);
    int i_interval  = os_atoi(interval);
    int i_limit     = os_atoi(limit);
    
    if (false==is_good_tm_args(i_delay, i_interval, i_limit)) {
        return tmc_usage(-EINVAL);
    }
    
    return tmc_request(table, argc, argv);
}

STATIC int
tmc_cmd_remove(cli_table_t *table, int argc, char *argv[])
{
    char *name = argv[1];
    
    if (2!=argc) {
        return tmc_usage(-EINVAL1);
    }
    else if (os_strlen(name) > TM_NAMESIZE) {
        return tmc_usage(-ETOOBIG);
    }
    else {
        return tmc_request(table, argc, argv);
    }
}

STATIC int
tmc_cmd_clean(cli_table_t *table, int argc, char *argv[])
{
    if (1!=argc) {
        return tmc_usage(-EINVAL1);
    }
    else {
        return tmc_request(table, argc, argv);
    }
}

STATIC int
tmc_cmd_show(cli_table_t *table, int argc, char *argv[])
{
    char *name = argv[1];
    
    if (1!=argc && 2!=argc) {
        return tmc_usage(-EINVAL2);
    }
    else if (name && os_strlen(name) > TM_NAMESIZE) {
        return tmc_usage(-ETOOBIG);
    }
    else {
        return tmc_request(table, argc, argv);
    }
}

STATIC int
tmc_command(int argc, char *argv[])
{
    static cli_table_t tables[] = {
        CLI_TCP_ENTRY("insert", tmc_cmd_insert),
        CLI_TCP_ENTRY("remove", tmc_cmd_remove),
        CLI_TCP_ENTRY("clean",  tmc_cmd_clean),
        CLI_TCP_ENTRY("show",   tmc_cmd_show),
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
tmc_main_helper(int argc, char *argv[])
{
    int err;
    
    if (1==argc) {
        return tmc_usage(-EHELP);
    }

    tmc.timeout = env_geti(OS_ENV(TIMEOUT), tmc.timeout);
    
    err = tmc_command(argc-1, argv+1);
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
    
    return os_main(tmc_main_helper, argc, argv);
}
/******************************************************************************/
