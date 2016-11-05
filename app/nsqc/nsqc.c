/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      nsqc
#endif

#include "nsq/nsq.h"

OS_INITER;

static cli_client_t nsqc = CLI_CLIENT_INITER(nsqa);

#define nsqc_handle(_action, _argc, _argv) \
    cli_c_sync(_action, _argc, _argv, &nsqc)

static int
usage(int error)
{
    os_eprintln(__THIS_APPNAME " insert {json}");
    os_eprintln(__THIS_APPNAME " remove {name}");
    os_eprintln(__THIS_APPNAME " show [name]");

    return error;
}

static int
cmd_insert(int argc, char *argv[])
{
    char *json = argv[0];
    
    if (1!=argc) {
        return usage(-EINVAL0);
    }
    else if (false==is_good_json(json)) {
        return usage(-EBADJSON);
    }
    
    return nsqc_handle("insert", argc, argv);
}

static int
cmd_remove(int argc, char *argv[])
{
    char *name = argv[0];
    
    if (1!=argc) {
        return usage(-EINVAL1);
    }
    else if (os_strlen(name) > NSQ_NAMESIZE) {
        return usage(-ETOOBIG);
    }
    else {
        return nsqc_handle("remove", argc, argv);
    }
}

static int
cmd_show(int argc, char *argv[])
{
    char *name = argv[0];
    
    if (0!=argc && 1!=argc) {
        return usage(-EINVAL2);
    }
    else if (name && os_strlen(name) > NSQ_NAMESIZE) {
        return usage(-ETOOBIG);
    }
    else {
        return nsqc_handle("show", argc, argv);
    }
}

static int
command(int argc, char *argv[])
{
    static cli_table_t table[] = {
        CLI_ENTRY("insert", cmd_insert),
        CLI_ENTRY("remove", cmd_remove),
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
    
    init_cli_client(&nsqc);

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
