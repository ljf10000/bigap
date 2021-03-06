/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      rsha
#endif
    
#ifndef __THIS_FILE
#define __THIS_FILE     cli
#endif

#define __DEAMON__
#define __THIS_USAGE \
    "rshc usage:"                   __crlf \
    __tab "rshc insert {sp} {json}" __crlf \
    __tab "rshc remove {sp}"        __crlf \
    __tab "rshc show [sp]"          __crlf \
    /* end */
#include "rsha.h"
/******************************************************************************/
/*
* remove {name}
*/
STATIC int
rsha_handle_remove(cli_table_t *table, int argc, char *argv[])
{
    char *sp = argv[1];
    
    if (2 != argc) {
        return cli_help(-EINVAL0);
    }
    else if (os_strlen(sp) > RSHI_SPSIZE) {
        return cli_help(-ETOOBIG);
    }

    return rshi_remove(sp);
}

/*
* insert {json}
*/
STATIC int
rsha_handle_insert(cli_table_t *table, int argc, char *argv[])
{
    char *sp    = argv[1];
    char *json  = argv[2];
    
    if (3!=argc) {
        return cli_help(-EINVAL0);
    }

    return rshi_insert(sp, json);
}

/*
* show [name]
*/
STATIC int
rsha_handle_show(cli_table_t *table, int argc, char *argv[])
{
    char *sp = argv[1];
    
    if (2 != argc && 1 != argc) {
        return cli_help(-EINVAL0);
    }
    else if (sp && os_strlen(sp) > RSHI_SPSIZE) {
        return cli_help(-ETOOBIG);
    }

    return rshi_show(sp);
}

STATIC int
rsha_cli(loop_watcher_t *watcher, time_t now)
{
    static cli_table_t tables[] = {
        /*
        * help must first
        */
        CLI_TCP_ENTRY("help",   cli_handle_help),
        
        CLI_TCP_ENTRY("insert", rsha_handle_insert),
        CLI_TCP_ENTRY("remove", rsha_handle_remove),
        CLI_TCP_ENTRY("show",   rsha_handle_show),
    };

    return cli_response(watcher->fd, tables);
}

int
init_rsha_cli(void)
{
    int err;

    err = os_loop_cli_tcp(&rsha.loop, rsha_cli, NULL);
    if (err<0) {
        return err;
    }
    
    return 0;
}
/******************************************************************************/
