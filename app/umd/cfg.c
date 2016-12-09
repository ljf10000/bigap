/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      umd
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     cfg
#endif

#define __DEAMON__
#include "umd.h"

extern sock_server_t umd_cli_server;
extern sock_server_t umd_flow_server;
extern sock_server_t umd_timer_server;

STATIC void
umd_init_cfg_intf_deft(umd_intf_t *intf, char *ifname, int id)
{
    os_strdcpy(intf->name, UMD_IFNAME_INGRESS);

    intf->id    = id;
    intf->auth  = umd_auth_type_deft;
    intf->mode  = umd_forward_mode_deft;
}

STATIC int
umd_init_cfg_intf_pre(int count)
{
    umd_intf_t *intf;
    int i;
    
    intf = (umd_intf_t *)os_zalloc(count*sizeof(umd_intf_t));
    if (NULL==intf) {
        return -ENOMEM;
    }

    for (i=0; i<count; i++) {
        umd_init_cfg_intf_deft(intf, UMD_IFNAME_INGRESS, i);
    }
    umd.cfg.instance.intf = intf;
    umd.cfg.instance.count = count;

    debug_config("intf count %d", count);
    
    return 0;
}

STATIC int
umd_init_cfg_intf_post(void)
{
    int i, err;
    umd_intf_t *intf;

    for (i=0; i<umd.cfg.instance.count; i++) {
        intf = &umd.cfg.instance.intf[i];
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

#if 0
        err = intf_set_promisc(intf->name);
        if (err<0) {
            return err;
        }
#endif
        debug_config("init intf %s", intf->name);
    }

    return 0;
}

STATIC int
umd_init_cfg_server(int count)
{
    sock_server_t **server;
    int i;
    
    umd.server_count = count + UMD_SERVER_FLOW;
    debug_config("server count %d", umd.server_count);
    
    server = (sock_server_t **)os_zalloc(umd.server_count*sizeof(sock_server_t *));
    if (NULL==server) {
        return -ENOMEM;
    }
    server[UMD_SERVER_TIMER] = &umd_timer_server;
    server[UMD_SERVER_TIMER]->id = UMD_SERVER_TIMER;
    debug_config("setup timer server");
    
    server[UMD_SERVER_CLI]   = &umd_cli_server;
    server[UMD_SERVER_CLI]->id = UMD_SERVER_CLI;
    debug_config("setup cli server");
    
    for (i=UMD_SERVER_FLOW; i<umd.server_count; i++) {
        server[i] = (sock_server_t *)os_zalloc(sizeof(sock_server_t));
        if (NULL==server[i]) {
            return -ENOMEM;
        }
        os_objcpy(server[i], &umd_flow_server);
        server[i]->id = i;
        debug_config("setup flow server[%d]", i);
    }

    umd.server = server;
    
    return 0;
}

STATIC int
umd_init_cfg_instance_one(jobj_t jinstance, int id)
{
    umd_intf_t *intf = &umd.cfg.instance.intf[id];
    jobj_t jval;
    
    jval = jobj_get(jinstance, "ingress");
    if (jval) {
        char *ifname = jobj_get_string(jval);

        os_strdcpy(intf->name, ifname);

        debug_config("ingress=%s", ifname);
    }

    jj_byeq(intf, jinstance, auth, umd_auth_type_getidbyname);
    jj_byeq(intf, jinstance, mode, umd_forward_mode_getidbyname);

    return 0;
}

STATIC int
umd_init_cfg_instance(jobj_t jcfg)
{
    umd_intf_t *intf;
    int i, err, count;
    
    jobj_t jarray = jobj_get(jcfg, "instance");
    if (NULL==jarray) {
        return -EBADCONF;
    }

    count = jarray_length(jarray);
    if (count<1) {
        return -EBADCONF;
    }
    
    err = umd_init_cfg_intf_pre(count);
    if (err<0) {
        return err;
    }
    
    for (i=0; i<count; i++) {
        jobj_t jinstance = jarray_get(jarray, i);
        if (NULL==jinstance) {
            return -EBADCONF;
        }
        
        err = umd_init_cfg_instance_one(jinstance, i);
        if (err<0) {
            return err;
        }
    }
    
    err = umd_init_cfg_intf_post();
    if (err<0) {
        return err;
    }
    
    err = umd_init_cfg_server(count);
    if (err<0) {
        return err;
    }
    
    return 0;
}

int umd_init_cfg(void)
{
    jobj_t jobj = JOBJ_MAPFILE(umd.conf, UMD_CFG_JMAPPER);
    if (NULL==jobj) {
        return -EBADCONF;
    }

    int err = umd_init_cfg_instance(jobj);
    if (err<0) {
        return err;
    }

    jobj_put(jobj);
    
    return 0;
}
/******************************************************************************/
