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

extern sock_server_t um_cli_server;
extern sock_server_t um_flow_server;
extern sock_server_t um_timer_server;

static int
init_cfg_intf_pre(int count)
{
    struct um_intf *intf;
    int i;
    
    intf = (struct um_intf *)os_zalloc(count*sizeof(struct um_intf));
    if (NULL==intf) {
        return -ENOMEM;
    }

    for (i=0; i<count; i++) {
        intf->id = i;
        
        os_strdcpy(intf->name, UMD_IFNAME_INGRESS);
    }
    umd.cfg.instance.intf = intf;
    umd.cfg.instance.count = count;

    debug_config("intf count %d", count);
    
    return 0;
}


static int
init_cfg_intf_post(void)
{
    int i, err;
    struct um_intf *intf;

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

static int
init_cfg_server(int count)
{
    sock_server_t **server;
    int i;
    
    umd.server_count = count + UM_SERVER_FLOW;
    debug_config("server count %d", umd.server_count);
    
    server = (sock_server_t **)os_zalloc(umd.server_count*sizeof(sock_server_t *));
    if (NULL==server) {
        return -ENOMEM;
    }
    server[UM_SERVER_TIMER] = &um_timer_server;
    server[UM_SERVER_TIMER]->id = UM_SERVER_TIMER;
    debug_config("setup timer server");
    
    server[UM_SERVER_CLI]   = &um_cli_server;
    server[UM_SERVER_CLI]->id = UM_SERVER_CLI;
    debug_config("setup cli server");
    
    for (i=UM_SERVER_FLOW; i<umd.server_count; i++) {
        server[i] = (sock_server_t *)os_zalloc(sizeof(sock_server_t));
        if (NULL==server[i]) {
            return -ENOMEM;
        }
        os_objcpy(server[i], &um_flow_server);
        server[i]->id = i;
        debug_config("setup flow server[%d]", i);
    }

    umd.server = server;
    
    return 0;
}

static int
__init_cfg_instance(jobj_t jinstance, int id)
{
    struct um_intf *intf = &umd.cfg.instance.intf[id];
    
    jobj_t jobj = jobj_get(jinstance, "ingress");
    if (jobj) {
        char *ifname = jobj_get_string(jobj);
        
        os_strdcpy(intf->name, ifname);

        debug_config("ingress=%s", ifname);
    }

    return 0;
}

static int
init_cfg_instance(jobj_t jcfg)
{
    struct um_intf *intf;
    int i, err, count;
    
    jobj_t jarray = jobj_get(jcfg, "instance");
    if (NULL==jarray) {
        return -EBADCONF;
    }

    count = jarray_length(jarray);
    if (count<1) {
        return -EBADCONF;
    }
    
    err = init_cfg_intf_pre(count);
    if (err<0) {
        return err;
    }
    
    for (i=0; i<count; i++) {
        jobj_t jinstance = jarray_get(jarray, i);
        if (NULL==jinstance) {
            return -EBADCONF;
        }
        
        err = __init_cfg_instance(jinstance, i);
        if (err<0) {
            return err;
        }
    }
    
    err = init_cfg_intf_post();
    if (err<0) {
        return err;
    }
    
    err = init_cfg_server(count);
    if (err<0) {
        return err;
    }
    
    return 0;
}

#define UMD_JOBJ_LOADER(_)          \
    _(&umd.cfg, string, script_event)       \
    _(&umd.cfg, string, script_getipbymac)  \
    _(&umd.cfg, string, script_getmacbyip)  \
    _(&umd.cfg, bool, syncable)     \
    _(&umd.cfg, bool, reauthable)   \
    _(&umd.cfg, u32, gc)            \
    _(&umd.cfg, u32, sniff_count)   \
    _(&umd.cfg, u32, ticks)         \
    _(&umd.cfg, u32, idle)          \
    _(&umd.cfg, u32, fake)          \
    _(&umd.cfg, u32, machashsize)   \
    _(&umd.cfg, u32, iphashsize)    \
    _(&umd.cfg, u32, autouser)      \
    /* end */

int init_cfg(void)
{
    DECLARE_JOBJ_LOADER(UMD_JOBJ_LOADER);
    
    jobj_loader_f *map[] = JOBJ_MAPPER(UMD_JOBJ_LOADER);
    jobj_t jobj = NULL;
    int err = 0;
    
    jobj = jobj_byfile(umd.conf);
    if (NULL==jobj) {
        debug_error("bad %s", umd.conf);
        
        return -EBADCONF;
    }
    
    err = jobj_load(jobj, map, os_count_of(map));
    if (err<0) {
        return err;
    }

    err = init_cfg_instance(jobj);
    if (err<0) {
        return err;
    }

    jobj_put(jobj);
    
    return 0;
}
/******************************************************************************/
