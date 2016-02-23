#ifndef __THIS_APP
#define __THIS_APP      umc
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     "umc"
#endif

#define __CLI__

#include "utils.h"
#include "um/um.h"

OS_INITER;
DECLARE_CLI_BUFFER;

static cli_client_t umc = CLI_CLIENT_INITER(UM_TIMEOUT, UMD_UNIX);

static int
usage(int error)
{
    os_eprintln(__THIS_NAME " bind   {mac} {ip}");
    os_eprintln(__THIS_NAME " unbind {mac}");
    os_eprintln(__THIS_NAME " auth   {mac} {group} {json}");
    os_eprintln(__THIS_NAME " reauth {mac}");
    os_eprintln(__THIS_NAME " deauth {mac}");
    os_eprintln(__THIS_NAME " tag    {mac} {key} [value]");
#if UM_USE_MONITOR
    os_eprintln(__THIS_NAME " enter  {mac} {json}");
    os_eprintln(__THIS_NAME " leave  {mac}");
#endif
    os_eprintln(__THIS_NAME " gc");
    os_eprintln(__THIS_NAME " show [stat | json]");

    return error;
}

#define umc_handle(_action, _argc, _argv) \
    cli_c_handle(_action, true, _argc, _argv, &umc.server, &umc.client, umc.timeout)

/*
* online
*/
static int
cmd_online(int argc, char *argv[])
{
    if (0!=argc) {
        return -EFORMAT;
    }

    return umc_handle("online", argc, argv);
}

/*
* offline
*/
static int
cmd_offline(int argc, char *argv[])
{
    if (0!=argc) {
        return -EFORMAT;
    }

    return umc_handle("offline", argc, argv);
}

#if UM_USE_MONITOR
/*
* enter {mac} {json}
*/
static int
cmd_enter(int argc, char *argv[])
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
    
    return umc_handle("enter", argc, argv);
}

/*
* leave {mac}
*/
static int
cmd_leave(int argc, char *argv[])
{
    char *mac= argv[0];
    
    if (1!=argc) {
        return -EFORMAT;
    }
    else if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return -EFORMAT;
    }

    return umc_handle("leave", argc, argv);
}
#endif

/*
* bind {mac} {ip}
*/
static int
cmd_bind(int argc, char *argv[])
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

    return umc_handle("bind", argc, argv);
}

/*
* unbind {mac}
*/
static int
cmd_unbind(int argc, char *argv[])
{
    char *mac= argv[0];
    
    if (1!=argc) {
        return -EFORMAT;
    }
    else if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return -EFORMAT;
    }

    return umc_handle("unbind", argc, argv);
}

/*
* auth {mac} {group} {json}
*/
static int
cmd_auth(int argc, char *argv[])
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
static int
cmd_reauth(int argc, char *argv[])
{
    char *mac = argv[0];
    
    if (1!=argc) {
        return -EFORMAT;
    }
    else if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return -EFORMAT;
    }

    return umc_handle("reauth", argc, argv);
}

/*
* deauth {mac}
*/
static int
cmd_deauth(int argc, char *argv[])
{
    char *mac = argv[0];
    
    if (1!=argc) {
        return -EFORMAT;
    }
    else if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return -EFORMAT;
    }

    return umc_handle("deauth", argc, argv);
}

static int
show_by(int argc, char *argv[])
{
    char *json = argv[0];

    if (is_good_json(json) || 0==os_strcmp("stat", json)) {
        return umc_handle("show", argc, argv);
    }
    else {
        debug_error("bad json %s", json);
        
        return -EFORMAT;
    }
}

/*
* show {mac} {json}
*/
static int
cmd_show(int argc, char *argv[])
{
    switch(argc) {
        case 0:
            return umc_handle("show", argc, argv);
        case 1: {
            char *json = argv[0];

            if (is_good_json(json) || 0==os_strcmp("stat", json)) {
                return umc_handle("show", argc, argv);
            }
            else {
                debug_error("bad json %s", json);
                
                return -EFORMAT;
            }
        }
        default:
            return usage(-EHELP);
    }
}

/*
* tag {mac} {key} [value]
*/
static int
cmd_tag(int argc, char *argv[])
{
    char *mac = argv[0];

    if (2!=argc || 3!=argc) {
        __cli_argv_dump(println, argc, argv);
        
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
cmd_gc(int argc, char *argv[])
{
    if (0!=argc) {
        return -EFORMAT;
    }

    return umc_handle("gc", argc, argv);
}

static int
command(int argc, char *argv[])
{
    static cli_table_t table[] = {
#if UM_USE_MONITOR
        CLI_ENTRY("enter",  cmd_enter),
        CLI_ENTRY("leave",  cmd_leave),
#endif
        CLI_ENTRY("bind",   cmd_bind),
        CLI_ENTRY("unbind", cmd_unbind),
        CLI_ENTRY("auth",   cmd_auth),
        CLI_ENTRY("reauth", cmd_reauth),
        CLI_ENTRY("deauth", cmd_deauth),
        CLI_ENTRY("show",   cmd_show),
        CLI_ENTRY("tag",    cmd_tag),
        CLI_ENTRY("gc",     cmd_gc),
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
init_env(void) 
{
    int err;
    
    umc.timeout = get_um_timeout_env();

    err = get_umc_path_env(&umc.client);
    if (err<0) {
        return err;
    }
    
    err = get_umd_path_env(&umc.server);
    if (err<0) {
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
    if (err<0) {
        return err;
    }
    
    err = command(argc-1, argv+1);
    if (err<0) {
        /* just log, NOT return */
    }

    return 0;
}

#ifndef __BUSYBOX__
#define umc_main  main
#endif

int umc_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(__main, argc, argv);
}

/******************************************************************************/
