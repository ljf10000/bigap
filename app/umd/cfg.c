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

#define init_jcfg_string(_jcfg, _field) ({ \
    jobj_t jobj = jobj_get(_jcfg, #_field); \
    if (jobj) { \
        umd.cfg._field = os_strdup(jobj_get_string(jobj)); \
        debug_cfg(#_field "=%d", umd.cfg._field); \
    } \
    0; \
})  /* end */

#define init_jcfg_bytype(_jcfg, _field, _type) ({ \
    jobj_t jobj = jobj_get(_jcfg, #_field); \
    if (jobj) { \
        umd.cfg._field = jobj_get_##_type(jobj); \
        debug_cfg(#_field "=%d", umd.cfg._field); \
    } \
    0; \
})  /* end */
#define init_jcfg_u32(_jcfg, _field)    init_jcfg_bytype(_jcfg, _field, u32)
#define init_jcfg_i32(_jcfg, _field)    init_jcfg_bytype(_jcfg, _field, i32)
#define init_jcfg_bool(_jcfg, _field)   init_jcfg_bytype(_jcfg, _field, bool)

static int
init_cfg_script_event(jobj_t jcfg)
{
    return init_jcfg_string(jcfg, script_event);
}

static int
init_cfg_script_getipbymac(jobj_t jcfg)
{
    return init_jcfg_string(jcfg, script_getipbymac);
}

static int
init_cfg_script_getmacbyip(jobj_t jcfg)
{
    return init_jcfg_string(jcfg, script_getmacbyip);
}

static int
init_cfg_gc(jobj_t jcfg)
{
    return init_jcfg_u32(jcfg, gc);
}

static int
init_cfg_sniff_count(jobj_t jcfg)
{
    return init_jcfg_u32(jcfg, sniff_count);
}

static int
init_cfg_ticks(jobj_t jcfg)
{
    return init_jcfg_u32(jcfg, ticks);
}

static int
init_cfg_idle(jobj_t jcfg)
{
    return init_jcfg_u32(jcfg, idle);
}

static int
init_cfg_fake(jobj_t jcfg)
{
    return init_jcfg_u32(jcfg, fake);
}

static int
init_cfg_machashsize(jobj_t jcfg)
{
    return init_jcfg_u32(jcfg, machashsize);
}

static int
init_cfg_iphashsize(jobj_t jcfg)
{
    return init_jcfg_u32(jcfg, iphashsize);
}

static int
init_cfg_autouser(jobj_t jcfg)
{
    return init_jcfg_i32(jcfg, autouser);
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

    debug_cfg("intf count %d", count);
    
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
        debug_cfg("init intf %s", intf->name);
    }

    return 0;
}

static int
init_cfg_server(int count)
{
    cli_server_t **server;
    int i;
    
    umd.server_count = count + UM_SERVER_FLOW;
    debug_cfg("server count %d", umd.server_count);
    
    server = (cli_server_t **)os_zalloc(umd.server_count*sizeof(cli_server_t *));
    if (NULL==server) {
        return -ENOMEM;
    }
    server[UM_SERVER_TIMER] = &um_timer_server;
    debug_cfg("setup timer server");
    
    server[UM_SERVER_CLI]   = &um_cli_server;
    debug_cfg("setup cli server");
    
    for (i=UM_SERVER_FLOW; i<umd.server_count; i++) {
        server[i] = (cli_server_t *)os_zalloc(sizeof(cli_server_t));
        if (NULL==server[i]) {
            return -ENOMEM;
        }
        os_objcpy(server[i], &um_flow_server);
        server[i]->id = i;
        debug_cfg("setup flow server[%d]", i);
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
            err = __init_cfg_instance(jinstance, i);
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
    static int (*map[])(jobj_t jcfg) = {
        init_cfg_script_event,
        init_cfg_script_getipbymac,
        init_cfg_script_getmacbyip,
        init_cfg_gc,
        init_cfg_sniff_count,
        init_cfg_ticks,
        init_cfg_idle,
        init_cfg_fake,
        init_cfg_machashsize,
        init_cfg_iphashsize,
        init_cfg_autouser,
        
        init_cfg_instance,
    };
    int i, err = 0;
    
    jobj_t jcfg = jobj_byfile(umd.cfg.conf);
    if (NULL==jcfg) {
        debug_error("bad %s", umd.cfg.conf);
        
        err = -EBADCONF; goto error;
    }

    for (i=0; i<os_count_of(map); i++) {
        err = (*map[i])(jcfg);
        if (err<0) {
            goto error;
        }
    }
    
error:
    jobj_put(jcfg);

    return err;
}
/******************************************************************************/
