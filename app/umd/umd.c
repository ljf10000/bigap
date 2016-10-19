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

    .hash_size = {
        [UM_USER_NIDX_MAC]  = UMD_HASHSIZE,
        [UM_USER_NIDX_IP]   = UMD_HASHSIZE,
    },

    .cfg = {
        .intf = {
            [um_intf_type_base] = {
                .name = UMD_IFNAME_BASE,
            },
            [um_intf_type_ingress] = {
                .name = UMD_IFNAME_INGRESS,
            },
        },

        .forward = um_forward_mode_rt,

        .lan = {
            {
                .ipstring   = "192.168.0.0",
                .maskstring = "255.255.255.0",
            },
            {
                .ipstring   = "172.16.0.0",
                .maskstring = "255.240.0.0",
            },
            {
                .ipstring   = "10.0.0.0",
                .maskstring = "255.0.0.0",
            },
        },
    },
};

static int
init_cfg(void)
{
    jobj_t jcfg = NULL, jobj;
    char *string;
    int i;
    
    jcfg = jobj_byfile(UMD_CONFIG);
    if (NULL==jcfg) {
        /* 
        * bad config file, use default config
        */
        return 0;
    }

    jobj = jobj_get(jcfg, "gc");
    if (jobj) {
        umd.cfg.gc = jobj_get_bool(jobj);

        debug_cfg("gc=%d", umd.cfg.gc);
    }

    jobj = jobj_get(jcfg, "forward");
    if (NULL==jobj) {
        string = jobj_get_string(jobj);
        umd.cfg.forward = forward_mode_idx(string);

        debug_cfg("forward=%d", umd.cfg.forward);
    }

    jobj_t jintf = jobj_get(jcfg, "interface");
    if (jintf) {
        for (i=0; i<um_intf_type_end; i++) {
            char *type_string = intf_type_string(i);
            jobj = jobj_get(jintf, type_string);
            if (jobj) {
                string = jobj_get_string(jobj);
                os_strdcpy(umd.cfg.intf[i].name, string);

                debug_cfg("%s intf=%s", type_string, string);
            }
        }
    }

    if (jcfg) {
        jobj_put(jcfg);
    }

    return 0;
}

static int
init_env(void)
{
    return 0;
}

static int
init_intf(void)
{
    int i, err;
    struct um_intf *intf;

    for (i=0; i<um_intf_type_end; i++) {
        intf = &umd.cfg.intf[i];
        intf->index = if_nametoindex(intf->name);

        err = intf_get_mac(intf->name, intf->mac);
        if (err<0) {
            return err;
        }
    }
    os_maccpy(umd.basemac, umd.cfg.intf[um_intf_type_base].mac);
    
    intf = &umd.cfg.intf[um_intf_type_ingress];
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

    for (i=0; i<os_count_of(umd.cfg.lan); i++) {
        umd.cfg.lan[i].ip   = inet_addr(umd.cfg.lan[i].ipstring);
        umd.cfg.lan[i].mask = inet_addr(umd.cfg.lan[i].maskstring);
    }

    return 0;
}

static void
__umevent_fini(void)
{
    if (false==umd.deinit) {
        umd.deinit = true;
        
        os_system(UMD_SCRIPT_EVENT " fini &");
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

    err = init_cfg();
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

    os_system(UMD_SCRIPT_EVENT " init &");

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

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(__exit);
    setup_signal_callstack(NULL);
    
    int err = os_call(__init, __fini, __main, argc, argv);

    return shell_error(err);
}
/******************************************************************************/
