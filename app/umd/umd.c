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

DECLARE_ENUM(umd_user_auto,     UMD_AUTO_ENUM_MAPPER,           UMD_AUTO_END);
DECLARE_ENUM(umd_user_state,    UMD_STATE_ENUM_MAPPER,          UMD_STATE_END);
DECLARE_ENUM(umd_deauth_reason, UMD_DEAUTH_ENUM_MAPPER,         UMD_DEAUTH_END);
DECLARE_ENUM(umd_flow_type,     UMD_FLOW_TYPE_ENUM_MAPPER,      umd_flow_type_end);
DECLARE_ENUM(umd_flow_dir,      UMD_FLOW_DIR_ENUM_MAPPER,       umd_flow_dir_end);
DECLARE_ENUM(umd_auth_type,     UMD_AUTH_TYPE_ENUM_MAPPER,      umd_auth_type_end);
DECLARE_ENUM(umd_forward_mode,  UMD_FORWARD_MODE_ENUM_MAPPER,   umd_forward_mode_end);
DECLARE_ENUM(umd_pkt_type,      UMD_PKT_TYPE_ENUM_MAPPER,       umd_pkt_type_end);
DECLARE_ENUM(umd_pkt_check,     UMD_PKT_CHECK_ENUM_MAPPER,      umd_pkt_check_end);

sock_server_t *
umd_get_server_by_intf(umd_intf_t *intf)
{
    return umd.server[umd_server_id(intf->id)];
}

umd_intf_t *
umd_get_intf_by_id(int intf_id)
{
    return &umd.cfg.instance.intf[intf_id];
}

umd_intf_t *
umd_get_intf_by_server(sock_server_t *server)
{
    return umd_get_intf_by_id(umd_intf_id(server->id));
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

    for (i=0; i<os_count_of(umd.lan); i++) {
        umd.lan[i].ip   = inet_addr(umd.lan[i].ipstring);
        umd.lan[i].mask = inet_addr(umd.lan[i].maskstring);
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
    
    err = h2_init(&umd.table, hash_size);
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
    int err = 0;

    umd_update_limit_test();

    while(1) {
        sock_servers_run(umd.server, umd.server_count);
    }
    
    return err;
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
