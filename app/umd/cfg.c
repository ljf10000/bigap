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

extern cli_server_t um_cli_server;
extern cli_server_t um_flow_server;
extern cli_server_t um_timer_server;

static init
init_cfg_lan(void)
{
    int i;
    
    for (i=0; i<os_count_of(umd.cfg.lan); i++) {
        umd.cfg.lan[i].ip   = inet_addr(umd.cfg.lan[i].ipstring);
        umd.cfg.lan[i].mask = inet_addr(umd.cfg.lan[i].maskstring);
    }

    return 0;
}

static int
init_cfg_gc(jobj_t jcfg)
{
    jobj_t jobj = jobj_get(jcfg, "gc");
    if (jobj) {
        umd.cfg.gc = jobj_get_bool(jobj);

        debug_cfg("gc=%d", umd.cfg.gc);
    }

    return 0;
}

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

#if UMD_USE_PROMISC
        err = intf_set_promisc(intf->name);
        if (err<0) {
            return err;
        }
#endif
    }

    return 0;
}

static int
init_cfg_server(int count)
{
    cli_server_t **server;
    int i;
    
    umd.server_count = count + UM_SERVER_FLOW;
    server = (cli_server_t **)os_zalloc(umd.server_count*sizeof(cli_server_t *));
    if (NULL==server) {
        return -ENOMEM;
    }
    server[UM_SERVER_TIMER] = &um_timer_server;
    server[UM_SERVER_CLI]   = &um_cli_server;

    for (i=UM_SERVER_FLOW; i<count; i++) {
        server[i] = (cli_server_t *)os_zalloc(sizeof(cli_server_t));
        if (NULL==server[i]) {
            return -ENOMEM;
        }
        os_objcpy(server[i], &um_flow_server);
        server[i]->id = i;
    }

    umd.server = server;
    
    return 0;
}

static int
init_cfg_instance_one(jobj_t jinstance, int id)
{
    struct um_intf *intf = &umd.cfg.instance.intf[id];
    
    jobj_t jobj = jobj_get(jinstance, "ingress");
    if (jobj) {
        char *ifname = jobj_get_string(jobj);
        
        os_strdcpy(intf->name, ifname);

        debug_cfg("ingress=%s", ifname);
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
        if (jinstance) {
            err = init_cfg_instance_one(jinstance, i);
            if (err<0) {
                return err;
            }
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

int init_cfg(void)
{
    int err = 0;

    jobj_t jcfg = jobj_byfile(UMD_CONFIG);
    if (NULL==jcfg) {
        debug_error("bad " UMD_CONFIG);
        
        err = -EBADCONF; goto error;
    }

    err = init_cfg_lan();
    if (err<0) {
        goto error;
    }

    err = init_cfg_gc(jcfg);
    if (err<0) {
        goto error;
    }

    err = init_cfg_instance(jcfg);
    if (err<0) {
        goto error;
    }

error:
    jobj_put(jcfg);

    return err;
}
/******************************************************************************/
