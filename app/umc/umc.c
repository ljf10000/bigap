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

#define umc_handle(_action, _argc, _argv) \
    clic_sync_handle(&umc, _action, _argc, _argv)

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
umc_handle_none(char *action, int argc, char *argv[])
{
    if (0!=argc) {
        return -EFORMAT;
    }

    return umc_handle(action, argc, argv);
}


/*
* ACTION {mac} {ip}
*/
STATIC int
umc_handle_mac_ip(char *action, int argc, char *argv[])
{
    char *mac= argv[0];
    char *ip = argv[1];
    
    if (2!=argc) {
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

    return umc_handle(action, argc, argv);
}

/*
* ACTION {mac} {json}
*/
STATIC int
umc_handle_mac_json(char *action, int argc, char *argv[])
{
    char *mac   = argv[0];
    char *json  = argv[1];
    
    if (2!=argc) {
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
    
    return umc_handle(action, argc, argv);
}

/*
* ACTION {mac}
*/
STATIC int
umc_handle_mac(char *action, int argc, char *argv[])
{
    char *mac= argv[0];
    
    if (1!=argc) {
        return -EFORMAT;
    }
    else if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return -EFORMAT;
    }

    return umc_handle(action, argc, argv);
}

/*
* create {mac}
*/
STATIC int
umc_cmd_create(int argc, char *argv[])
{
    return umc_handle_mac("create", argc, argv);
}

/*
* delete {mac}
*/
STATIC int
umc_cmd_delete(int argc, char *argv[])
{
    return umc_handle_mac("delete", argc, argv);
}

/*
* block {mac}
*/
STATIC int
umc_cmd_block(int argc, char *argv[])
{
    return umc_handle_mac("block", argc, argv);
}

/*
* unblock {mac}
*/
STATIC int
umc_cmd_unblock(int argc, char *argv[])
{
    return umc_handle_mac("unblock", argc, argv);
}

/*
* bind {mac} {ip}
*/
STATIC int
umc_cmd_bind(int argc, char *argv[])
{
    return umc_handle_mac_ip("bind", argc, argv);
}

/*
* unbind {mac}
*/
STATIC int
umc_cmd_unbind(int argc, char *argv[])
{
    return umc_handle_mac("unbind", argc, argv);
}

/*
* fake {mac} {ip}
*/
STATIC int
umc_cmd_fake(int argc, char *argv[])
{
    return umc_handle_mac_ip("fake", argc, argv);
}

/*
* unfake {mac}
*/
STATIC int
umc_cmd_unfake(int argc, char *argv[])
{
    return umc_handle_mac("unfake", argc, argv);
}

/*
* auth {mac} {group} {json}
*/
STATIC int
umc_cmd_auth(int argc, char *argv[])
{
    char *mac   = argv[0];
    char *group = argv[1];
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
    
    return umc_handle("auth", argc, argv);
}

/*
* deauth {mac}
*/
STATIC int
umc_cmd_deauth(int argc, char *argv[])
{
    return umc_handle_mac("deauth", argc, argv);
}

/*
* reauth {mac}
*/
STATIC int
umc_cmd_reauth(int argc, char *argv[])
{
    return umc_handle_mac("reauth", argc, argv);
}

STATIC int
umc_show_by(int argc, char *argv[])
{
    char *json = argv[0];

    if (is_good_json(json) || os_streq("stat", json)) {
        return umc_handle("show", argc, argv);
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
umc_cmd_sync(int argc, char *argv[])
{
    
    return umc_handle_mac_json("sync", argc, argv);
}

/*
* show {mac} {json}
*/
STATIC int
umc_cmd_show(int argc, char *argv[])
{
    switch(argc) {
        case 0:
            return umc_handle("show", argc, argv);
        case 1: {
            char *json = argv[0];

            if (is_good_json(json) 
                || os_streq("stat", json)
                || os_streq("count", json)) {
                return umc_handle("show", argc, argv);
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
umc_cmd_tag(int argc, char *argv[])
{
    char *mac = argv[0];

    if (2!=argc && 3!=argc) {        
        return -EFORMAT;
    }
    else if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return -EFORMAT;
    }

    return umc_handle("tag", argc, argv);
}

/*
* gc
*/
static int
umc_cmd_gc(int argc, char *argv[])
{
    return umc_handle_none("gc", argc, argv);
}

STATIC int
umc_command(int argc, char *argv[])
{
    static cli_table_t table[] = {
        CLI_ENTRY("create", umc_cmd_create),
        CLI_ENTRY("delete", umc_cmd_delete),
        
        CLI_ENTRY("block",  umc_cmd_block),
        CLI_ENTRY("unblock",umc_cmd_unblock),
        
        CLI_ENTRY("bind",   umc_cmd_bind),
        CLI_ENTRY("unbind", umc_cmd_unbind),
        
        CLI_ENTRY("fake",   umc_cmd_fake),
        CLI_ENTRY("unfake", umc_cmd_unfake),
        
        CLI_ENTRY("auth",   umc_cmd_auth),
        CLI_ENTRY("reauth", umc_cmd_reauth),
        CLI_ENTRY("deauth", umc_cmd_deauth),
        
        CLI_ENTRY("show",   umc_cmd_show),
        CLI_ENTRY("sync",   umc_cmd_sync),
        CLI_ENTRY("tag",    umc_cmd_tag),
        CLI_ENTRY("gc",     umc_cmd_gc),
    };
    int err;

    err = cli_argv_handle(table, os_count_of(table), argc, argv);
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
