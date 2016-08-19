/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      umd
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     umd
#endif

#define __DEAMON__
#include "utils.h"
#include "umd.h"

OS_INITER;

extern cli_server_t um_cli_server;
extern cli_server_t um_flow_server;
extern cli_server_t um_timer_server;

struct um_control umd = {
    .server = {
        [UM_SERVER_TIMER]   = &um_timer_server,
        [UM_SERVER_CLI]     = &um_cli_server,
        [UM_SERVER_FLOW]    = &um_flow_server,
    },

    .intf = {
        [UM_INTF_FLOW] = {
            .name = UMD_INTF_FLOW_DEFT,
        },
        [UM_INTF_TC] = {
            .name = UMD_INTF_TC_DEFT,
        },
    },

    .hash_size = {
        [UM_USER_NIDX_MAC]  = UMD_HASHSIZE,
        [UM_USER_NIDX_IP]   = UMD_HASHSIZE,
    },
};

static int
init_env(void)
{
    int err;

    umd.gc = get_umd_gc_env();

    return 0;
}

static int
init_intf(void)
{
    int err;
    struct um_intf *intf;

    intf = &umd.intf[UM_INTF_FLOW];
    {
        intf->index = if_nametoindex(intf->name);

        err = intf_get_mac(intf->name, intf->mac);
        if (err<0) {
            return err;
        }
        os_maccpy(umd.basemac, intf->mac);
    }

    intf = &umd.intf[UM_INTF_TC];
    {
        intf->index = if_nametoindex(intf->name);
        
        err = intf_get_mac(intf->name, intf->mac);
        if (err<0) {
            return err;
        }
        
        err = intf_get_ip(intf->name, &intf->ip);
        if (err<0) {
            return err;
        }
        
        err = intf_get_netmask(intf->name, &intf->mask);
        if (err<0) {
            return err;
        }

#if UMD_USE_PROMISC
        err = intf_set_promisc(intf->name);
        if (err<0) {
            return err;
        }
#endif
    }
    
    return 0;
}

static void
__umevent_fini(void)
{
    if (false==umd.deinit) {
        umd.deinit = true;
        
        os_system(UMD_SCRIPT_EVENT " fini");
    }
}

static int
__fini(void)
{
    __umevent_fini();
    os_fini();

    return 0;
}

static void 
__exit(int sig)
{
    __fini();
    
    exit(sig);
}

static int
__init(void)
{
    int err;
    
    err = os_init();
    if (err<0) {
        return err;
    }

    err = init_env();
    if (err<0) {
        return err;
    }
    
    err = h2_init(&umd.table, umd.hash_size);
    if (err<0) {
        return err;
    }
    
    err = init_intf();
    if (err<0) {
        return err;
    }

    err = cli_server_init(umd.server);
    if (err<0) {
        return err;
    }

    os_system(UMD_SCRIPT_EVENT " init");

    return 0;
}

static int
__main(int argc, char **argv)
{
    int err = 0;

    update_limit_test();

    while(1) {
        cli_server_run(umd.server);
    }
    
    return err;
}

#ifndef __BUSYBOX__
#define umd_main  main
#endif

int umd_main(int argc, char *argv[])
{
    setup_signal_exit(__exit);
    setup_signal_callstack(NULL);
    
    int err = os_call(__init, __fini, __main, argc, argv);

    return shell_error(err);
}
/******************************************************************************/
