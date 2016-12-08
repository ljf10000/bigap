/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      umc
#endif

#include "utils.h"
#include "um/um.h"

OS_INITER;

static cli_client_t umc = CLI_CLIENT_INITER("umd");

#define umc_request(_table, _argc, _argv) \
    clic_request(&umc, _table, _argc, _argv)

STATIC int
umc_usage(int error)
{
    os_eprintln(__THIS_APPNAME " bind   {mac} {ip}");
    os_eprintln(__THIS_APPNAME " unbind {mac}");
    os_eprintln(__THIS_APPNAME " fake   {mac} {ip}");
    os_eprintln(__THIS_APPNAME " unfake {mac}");
    os_eprintln(__THIS_APPNAME " auth   {mac} {group} {json}");
    os_eprintln(__THIS_APPNAME " deauth {mac}");
    os_eprintln(__THIS_APPNAME " reauth {mac}");
    os_eprintln(__THIS_APPNAME " tag    {mac} {key} [value]");
    os_eprintln(__THIS_APPNAME " gc");
    os_eprintln(__THIS_APPNAME " show [stat | json]");

    return error;
}

STATIC int
umc_handle_none(cli_table_t *table, int argc, char *argv[])
{
    if (1!=argc) {
        return -EFORMAT;
    }

    return umc_request(table, argc, argv);
}


/*
* ACTION {mac} {ip}
*/
STATIC int
umc_handle_mac_ip(cli_table_t *table, int argc, char *argv[])
{
    char *mac= argv[0];
    char *ip = argv[1];
    
    if (3!=argc) {
        return -EFORMAT;
    }
    else if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return -EFORMAT;
    }
    else if (false==is_good_ipstring(ip)) {
        debug_trace("bad ip %s", ip);

        return -EFORMAT;
    }

    return umc_request(table, argc, argv);
}

/*
* ACTION {mac} {json}
*/
STATIC int
umc_handle_mac_json(cli_table_t *table, int argc, char *argv[])
{
    char *mac   = argv[1];
    char *json  = argv[2];
    
    if (3!=argc) {
        return -EFORMAT;
    }
    else if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return -EFORMAT;
    }
    else if (false==is_good_json(json)) {
        debug_trace("bad json %s", json);

        return -EFORMAT;
    }
    
    return umc_request(table, argc, argv);
}

/*
* ACTION {mac}
*/
STATIC int
umc_handle_mac(cli_table_t *table, int argc, char *argv[])
{
    char *mac= argv[1];
    
    if (2!=argc) {
        return -EFORMAT;
    }
    else if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return -EFORMAT;
    }

    return umc_request(table, argc, argv);
}

/*
* create {mac}
*/
STATIC int
umc_cmd_create(cli_table_t *table, int argc, char *argv[])
{
    return umc_handle_mac(table, argc, argv);
}

/*
* delete {mac}
*/
STATIC int
umc_cmd_delete(cli_table_t *table, int argc, char *argv[])
{
    return umc_handle_mac(table, argc, argv);
}

/*
* block {mac}
*/
STATIC int
umc_cmd_block(cli_table_t *table, int argc, char *argv[])
{
    return umc_handle_mac(table, argc, argv);
}

/*
* unblock {mac}
*/
STATIC int
umc_cmd_unblock(cli_table_t *table, int argc, char *argv[])
{
    return umc_handle_mac(table, argc, argv);
}

/*
* bind {mac} {ip}
*/
STATIC int
umc_cmd_bind(cli_table_t *table, int argc, char *argv[])
{
    return umc_handle_mac_ip(table, argc, argv);
}

/*
* unbind {mac}
*/
STATIC int
umc_cmd_unbind(cli_table_t *table, int argc, char *argv[])
{
    return umc_handle_mac(table, argc, argv);
}

/*
* fake {mac} {ip}
*/
STATIC int
umc_cmd_fake(cli_table_t *table, int argc, char *argv[])
{
    return umc_handle_mac_ip(table, argc, argv);
}

/*
* unfake {mac}
*/
STATIC int
umc_cmd_unfake(cli_table_t *table, int argc, char *argv[])
{
    return umc_handle_mac(table, argc, argv);
}

/*
* auth {mac} {group} {json}
*/
STATIC int
umc_cmd_auth(cli_table_t *table, int argc, char *argv[])
{
    char *mac   = argv[1];
    char *group = argv[2];
    char *json  = argv[3];
    
    if (4!=argc) {
        return -EFORMAT;
    }
    else if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return -EFORMAT;
    }
    else if (false==is_good_json(json)) {
        debug_trace("bad json %s", json);

        return -EFORMAT;
    }
    
    return umc_request(table, argc, argv);
}

/*
* deauth {mac}
*/
STATIC int
umc_cmd_deauth(cli_table_t *table, int argc, char *argv[])
{
    return umc_handle_mac(table, argc, argv);
}

/*
* reauth {mac}
*/
STATIC int
umc_cmd_reauth(cli_table_t *table, int argc, char *argv[])
{
    return umc_handle_mac(table, argc, argv);
}

STATIC int
umc_show_by(cli_table_t *table, int argc, char *argv[])
{
    char *json = argv[0];

    if (is_good_json(json) || os_streq("stat", json)) {
        return umc_request(table, argc, argv);
    }
    else {
        debug_error("bad json %s", json);
        
        return -EFORMAT;
    }
}

/*
* sync {mac} {json}
*/
STATIC int
umc_cmd_sync(cli_table_t *table, int argc, char *argv[])
{
    return umc_handle_mac_json(table, argc, argv);
}

/*
* show [stat | json]
*/
STATIC int
umc_cmd_show(cli_table_t *table, int argc, char *argv[])
{
    switch(argc) {
        case 1:
            return umc_request(table, argc, argv);
        case 2: {
            char *json = argv[1];

            if (is_good_json(json) 
                || os_streq("stat", json)
                || os_streq("count", json)) {
                return umc_request(table, argc, argv);
            }
            else {
                debug_error("bad json %s", json);
                
                return -EFORMAT;
            }
        }
        default:
            return umc_usage(-EHELP);
    }
}

/*
* tag {mac} {key} [value]
*/
STATIC int
umc_cmd_tag(cli_table_t *table, int argc, char *argv[])
{
    char *mac = argv[1];

    if (3!=argc && 4!=argc) {        
        return -EFORMAT;
    }
    else if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return -EFORMAT;
    }

    return umc_request(table, argc, argv);
}

/*
* gc
*/
static int
umc_cmd_gc(cli_table_t *table, int argc, char *argv[])
{
    return umc_handle_none(table, argc, argv);
}

STATIC int
umc_command(int argc, char *argv[])
{
    static cli_table_t tables[] = {
        CLI_TCP_ENTRY("create", umc_cmd_create),
        CLI_TCP_ENTRY("delete", umc_cmd_delete),
        
        CLI_TCP_ENTRY("block",  umc_cmd_block),
        CLI_TCP_ENTRY("unblock",umc_cmd_unblock),
        
        CLI_TCP_ENTRY("bind",   umc_cmd_bind),
        CLI_TCP_ENTRY("unbind", umc_cmd_unbind),
        
        CLI_TCP_ENTRY("fake",   umc_cmd_fake),
        CLI_TCP_ENTRY("unfake", umc_cmd_unfake),
        
        CLI_TCP_ENTRY("auth",   umc_cmd_auth),
        CLI_TCP_ENTRY("reauth", umc_cmd_reauth),
        CLI_TCP_ENTRY("deauth", umc_cmd_deauth),
        
        CLI_TCP_ENTRY("show",   umc_cmd_show),
        CLI_TCP_ENTRY("sync",   umc_cmd_sync),
        CLI_TCP_ENTRY("tag",    umc_cmd_tag),
        CLI_TCP_ENTRY("gc",     umc_cmd_gc),
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
umc_main_helper(int argc, char *argv[])
{
    int err;
        
    if (1==argc) {
        return umc_usage(-EHELP);
    }

    umc.timeout = env_geti(OS_ENV(TIMEOUT), umc.timeout);
    
    err = umc_command(argc-1, argv+1);
    if (err<0) {
        /* just log, NOT return */
    }

    return 0;
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_ignore(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(umc_main_helper, argc, argv);
}

/******************************************************************************/
