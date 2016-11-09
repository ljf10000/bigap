/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      tmc
#endif

#define __CLI_TCP__     0
#include "utils.h"
#include "tm/tm.h"

OS_INITER;

static cli_client_t tmc = CLI_CLIENT_INITER("tmd");

#define tmc_handle(_action, _argc, _argv) \
    clic_sync_handle(&tmc, _action, _argc, _argv)

static int
usage(int error)
{
    os_eprintln(__THIS_APPNAME " insert {name} delay(second) interval(second) limit command");
    os_eprintln(__THIS_APPNAME " remove {name}");
    os_eprintln(__THIS_APPNAME " show [name]");

    return error;
}

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
    
    return tmc_handle("insert", argc, argv);
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
        return tmc_handle("remove", argc, argv);
    }
}

static int
cmd_clean(int argc, char *argv[])
{
    if (0!=argc) {
        return usage(-EINVAL1);
    }
    else {
        return tmc_handle("clean", argc, argv);
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
        return tmc_handle("show", argc, argv);
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
