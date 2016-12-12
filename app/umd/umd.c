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
#include "umd.h"
/******************************************************************************/
OS_INITER;

umd_control_t umd = UMD_INITER;

umd_intf_t *
umd_getintf_byid(int intf_id)
{
    return &umd.cfg.instance.intf[intf_id];
}

sock_server_t *
umd_getserver_byid(int server_id)
{
    return umd.server[server_id];
}

sock_server_t *
umd_getserver_byintf(umd_intf_t *intf)
{
    return umd_getserver_byid(umd_server_id(intf->id));
}

umd_intf_t *
umd_getintf_byserver(sock_server_t *server)
{
    return umd_getintf_byid(umd_intf_id(server->id));
}
/******************************************************************************/
STATIC int
umd_init_pre(void)
{
    int i;
    char *basemac = os_getmacby(SCRIPT_GETBASEMAC);
    if (NULL==basemac) {
        return -EBADBASEMAC;
    }
    __os_getmac_bystring(umd.basemac, basemac);

    umd.ether_type_ip  = htons(ETHERTYPE_IP);
    umd.ether_type_vlan= htons(ETHERTYPE_VLAN);
    umd.ether_type_all = htons(ETH_P_ALL);

    for (i=0; i<os_count_of(umd.plan); i++) {
        umd.plan[i].ip   = inet_addr(umd.plan[i].ipstring);
        umd.plan[i].mask = inet_addr(umd.plan[i].maskstring);
    }

    umd.conf = env_gets(OS_ENV(CONF), UMD_CONF);
    
    return 0;
}

STATIC int
umd_init_post(void)
{
    int err;
    int hash_size[UM_USER_NIDX_END] = {
        [UM_USER_NIDX_MAC] = umd.cfg.machashsize,
        [UM_USER_NIDX_IP]  = umd.cfg.iphashsize,
    };
    
    err = h2_init(&umd.head.user, hash_size);
    if (err<0) {
        return err;
    }

    err = h1_init(&umd.head.conn, umd.cfg.connhashsize);
    if (err<0) {
        return err;
    }

    err = sock_servers_init(umd.server, umd.server_count);
    if (err<0) {
        return err;
    }

    os_system("%s init &", umd.cfg.script_event);

    return 0;
}

STATIC int
umd_fini(void)
{
    if (false==umd.deinit) {
        umd.deinit = true;
        
        os_system("%s fini &", umd.cfg.script_event);
    }
    
    os_fini();

    return 0;
}

STATIC void 
umd_exit(int sig)
{
    umd_fini();
    
    exit(sig);
}

extern int umd_init_cfg(void);

STATIC int
umd_init(void)
{
    int err;

    err = os_init();
    if (err<0) {
        return err;
    }

    err = umd_init_pre();
    if (err<0) {
        return err;
    }

    err = umd_init_cfg();
    if (err<0) {
        return err;
    }

    err = umd_init_post();
    if (err<0) {
        return err;
    }
    
    return 0;
}

STATIC int
umd_main_helper(int argc, char **argv)
{
    umd_update_limit_test();

    return os_loop(&umd.loop);
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(umd_exit);
    setup_signal_ignore(NULL);
    setup_signal_callstack(NULL);
    
    int err = os_call(umd_init, umd_fini, umd_main_helper, argc, argv);

    return shell_error(err);
}
/******************************************************************************/
