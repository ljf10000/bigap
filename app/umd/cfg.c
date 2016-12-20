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
umd_init_cfg_ingress_deft(umd_ingress_t *ingress, char *ifname, int id)
{
    os_strdcpy(ingress->name, UMD_IFNAME_INGRESS);

    ingress->id     = id;
    ingress->auth   = umd_auth_type_deft;
    ingress->mode   = umd_forward_mode_deft;
}

STATIC int
umd_init_cfg_ingress_pre(int count)
{
    umd_ingress_t *ingress;
    int i;
    
    ingress = (umd_ingress_t *)os_zalloc(count*sizeof(umd_ingress_t));
    if (NULL==ingress) {
        return -ENOMEM;
    }

    for (i=0; i<count; i++) {
        umd_init_cfg_ingress_deft(ingress, UMD_IFNAME_INGRESS, i);
    }
    umd.cfg.instance.ingress= ingress;
    umd.cfg.instance.count  = count;

    debug_config("ingress count %d", count);
    
    return 0;
}

STATIC int
umd_init_cfg_ingress_post(void)
{
    int i, err;
    umd_ingress_t *ingress;

    for (i=0; i<umd.cfg.instance.count; i++) {
        ingress = &umd.cfg.instance.ingress[i];
        ingress->index = if_nametoindex(ingress->name);

        if (is_good_ipstring(ingress->ipaddr) && is_good_ipstring(ingress->ipmask)) {
            ingress->ip    = os_ipaddr(ingress->ipaddr);
            ingress->mask  = os_ipaddr(ingress->ipmask);
        } else {
            err = intf_get_ip(ingress->name, &ingress->ip);
            if (err<0) {
                return err;
            }

            err = intf_get_netmask(ingress->name, &ingress->mask);
            if (err<0) {
                return err;
            }
        }

        err = intf_get_mac(ingress->name, ingress->mac);
        if (err<0) {
            return err;
        }
        
        if (umd.cfg.promisc) {
            err = intf_set_promisc(ingress->name);
            if (err<0) {
                return err;
            }
        }

        if (__ak_debug_config) {
            char ipstring[1+OS_IPSTRINGLEN];
            char ipmaskstring[1+OS_IPSTRINGLEN];
            char macstring[1+MACSTRINGLEN_L];
        
            os_strcpy(ipstring, os_ipstring(ingress->ip));
            os_strcpy(ipmaskstring, os_ipstring(ingress->mask));
            os_strcpy(macstring, os_macstring(ingress->mac));

            debug_config("init ingress %s mac=%s ip=%s mask=%s", 
                ingress->name,
                macstring,
                ipstring,
                ipmaskstring);
        }
    }

    return 0;
}

STATIC int
umd_init_cfg_server(int count)
{
    sock_server_t **servers;
    int i;
    
    umd.server.count = count + UMD_SERVER_FLOW;
    debug_config("server count %d", umd.server.count);
    
    servers = (sock_server_t **)os_zalloc(umd.server.count*sizeof(sock_server_t *));
    if (NULL==servers) {
        return -ENOMEM;
    }
    servers[UMD_SERVER_TIMER] = &umd_timer_server;
    servers[UMD_SERVER_TIMER]->id = UMD_SERVER_TIMER;
    debug_config("setup timer server");

    servers[UMD_SERVER_CLI]   = &umd_cli_server;
    servers[UMD_SERVER_CLI]->id = UMD_SERVER_CLI;
    debug_config("setup cli server");
    
    for (i=UMD_SERVER_FLOW; i<umd.server.count; i++) {
        servers[i] = (sock_server_t *)os_zalloc(sizeof(sock_server_t));
        if (NULL==servers[i]) {
            return -ENOMEM;
        }
        os_objcpy(servers[i], &umd_flow_server);
        servers[i]->id = i;
        debug_config("setup flow server[%d]", i);
    }

    umd.server.servers = servers;
    
    return 0;
}

STATIC int
umd_init_cfg_instance_one(jobj_t jinstance, int id)
{
    umd_ingress_t *ingress = &umd.cfg.instance.ingress[id];
    jobj_t jval;

    __jj_strcpy(ingress, jinstance, "ingress", name);
    jj_strcpy(ingress, jinstance, ipaddr);
    jj_strcpy(ingress, jinstance, ipmask);
    
    jj_byeq(ingress, jinstance, auth, umd_auth_type_getidbyname);
    jj_byeq(ingress, jinstance, mode, umd_forward_mode_getidbyname);

    return 0;
}

STATIC int
umd_init_cfg_instance(jobj_t jcfg)
{
    int i, err, count;
    
    jobj_t jarray = jobj_get(jcfg, "instance");
    if (NULL==jarray) {
        return -EBADCONF;
    }

    count = jarray_length(jarray);
    if (count<1) {
        return -EBADCONF;
    }
    
    err = umd_init_cfg_ingress_pre(count);
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
    
    err = umd_init_cfg_ingress_post();
    if (err<0) {
        sleep(5);
        
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
    int err;
    
    jobj_t jobj = JOBJ_MAPFILE(umd.conf, UMD_CFG_JMAPPER);
    if (NULL==jobj) {
        return -EBADCONF;
    }

    err = umd_init_cfg_instance(jobj);
    if (err<0) {
        return err;
    }

    jobj_put(jobj);
    
    return 0;
}
/******************************************************************************/
