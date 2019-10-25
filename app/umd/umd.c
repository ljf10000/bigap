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

umd_ingress_t *
umd_getingress_byid(int ingress_id)
{
    return &umd.cfg.instance.ingress[ingress_id];
}

sock_server_t *
umd_getserver_byid(int server_id)
{
    return umd.server.servers[server_id];
}

sock_server_t *
umd_getserver_byingress(umd_ingress_t *ingress)
{
    return umd_getserver_byid(umd_server_id(ingress->id));
}

umd_ingress_t *
umd_getingress_byserver(sock_server_t *server)
{
    return umd_getingress_byid(umd_ingress_id(server->id));
}
/******************************************************************************/
STATIC int
umd_intf_dump(umd_intf_t *intf, char *action, int err)
{
    debug_intf("%s intf %s mac %s err=%d",
        action,
        intf->name,
        unsafe_macstring(intf->mac),
        err);

    return err;
}

STATIC hash_idx_t 
umd_intf_hash(byte mac[])
{
    return os_macbkdr(mac) & (umd.cfg.intfhashsize - 1);
}

STATIC hash_idx_t
umd_intf_nhash(hash_node_t *node)
{
    umd_intf_t *intf = umd_intf_hx_entry(node);
    
    return umd_intf_hash(intf->mac);
}

STATIC void
umd_intf_destroy(umd_intf_t *intf)
{
    os_free(intf);
}

STATIC umd_intf_t *
umd_intf_new(char *ifname, byte mac[])
{
    umd_intf_t *intf = (umd_intf_t *)os_zalloc(sizeof(*intf));
    if (intf) {
        os_strcpy(intf->name, ifname);
        os_maccpy(intf->mac, mac);

        umd_intf_dump(intf, "create", 0);
    }

    return intf;
}

STATIC int
umd_intf_insert(umd_intf_t *intf)
{
    int err = h1_add(&umd.head.intf, &intf->node.intf, umd_intf_nhash);

    return umd_intf_dump(intf, "insert", err);
}

STATIC int
umd_intf_remove(umd_intf_t *intf)
{
    int err = h1_del(&umd.head.intf, &intf->node.intf);

    return umd_intf_dump(intf, "remove", err);
}

umd_intf_t *
umd_intf_get(byte mac[])
{
    static umd_intf_t *last;
    
    hash_idx_t hash(void)
    {
        return umd_intf_hash(mac);
    }
    
    bool eq(hash_node_t *node)
    {
        umd_intf_t *intf = umd_intf_hx_entry(node);
        
        return os_maceq(mac, intf->mac);
    }

    if (last && os_maceq(mac, last->mac)) {
        return last;
    }
    
    umd_intf_t *found = umd_intf_h1_entry(h1_find(&umd.head.intf, hash, eq));
    if (found && found != last) {
        last = found;
    }

    return found;
}

STATIC umd_intf_t *
umd_intf_getEx(char *ifname, byte mac[])
{
    umd_intf_t *intf = umd_intf_get(mac);
    if (intf) {
        return intf;
    }
    
    intf = umd_intf_new(ifname, mac);
    if (NULL==intf) {
        return NULL;
    }

    int err = umd_intf_insert(intf);
    if (err<0) {
        umd_intf_destroy(intf);

        return NULL;
    }
    
    return intf;
}

STATIC int
umd_intf_init(void)
{
    mv_t foreach(char *ifname)
    {
        byte mac[OS_MACSIZE];
        
        int err = intf_get_mac(ifname, mac);
        if (err<0) {
            return mv2_go(err);
        }
        
        umd_intf_t *intf = umd_intf_getEx(ifname, mac);
        if (NULL==intf) {
            return mv2_go(-ENOMEM);
        }

        return mv2_ok;
    }
    
    return intf_foreachEx(true, foreach);
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

    umd.ether_type.ip  = htons(ETHERTYPE_IP);
    umd.ether_type.vlan= htons(ETHERTYPE_VLAN);
    umd.ether_type.all = htons(ETH_P_ALL);

    for (i=0; i<os_count_of(umd.plan); i++) {
        umd.plan[i].ip   = os_ipaddr(umd.plan[i].ipstring);
        umd.plan[i].mask = os_ipaddr(umd.plan[i].maskstring);
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

    err = h1_init(&umd.head.intf, umd.cfg.intfhashsize);
    if (err<0) {
        return err;
    }

    err = umd_intf_init();
    if (err<0) {
        return err;
    }
    
    err = sock_servers_init(umd.server.servers, umd.server.count);
    if (err<0) {
        return err;
    }

    os_system("%s init &", umd.cfg.script_event);

    return 0;
}

STATIC int
umd_fini(void)
{
    if (0==umd.fini++) {
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
