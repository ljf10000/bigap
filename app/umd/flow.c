/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      umd
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     flow
#endif

#define __DEAMON__
#include "umd.h"

static umd_flow_t flow;
static umd_conn_t conn;

typedef int umd_pkt_handle_f(sock_server_t *server, time_t now);

static inline struct ether_header *
umd_get_eth(void)
{
    return (struct ether_header *)flow.packet;
}

static inline struct vlan_header *
umd_get_vlan(void)
{
    return (struct vlan_header *)(flow.eth + 1);
}

static inline struct ip *
umd_get_iph(void)
{
    if (flow.eth->ether_type == umd.ether_type_ip) {
        return (struct ip *)(flow.eth + 1);
    }
    else if (flow.eth->ether_type == umd.ether_type_vlan) {
        return (struct ip *)(flow.vlan + 1);
    }
    else {       
        return NULL;
    }
}

STATIC void
umd_add_flowst(umd_flowst_t *st)
{
    st->packets++;
    st->bytes += flow.len;
}

STATIC void
umd_add_flow_total(int type, int valid)
{
    umd_add_flowst(&flow.st.total[type][valid]);
}

STATIC void
umd_add_flow_good(int type)
{
    umd_add_flow_total(type, umd_pkt_check_good);
}

STATIC void
umd_add_flow_bad(int type)
{
    umd_add_flow_total(type, umd_pkt_check_bad);
}

STATIC void
umd_add_flow_all(int type)
{
    umd_add_flow_total(type, umd_pkt_check_all);
}

STATIC void
umd_add_flow_dev(int flow_type, int flow_dir)
{
    umd_add_flowst(&flow.st.dev[flow_type][flow_dir]);
}

STATIC bool
umd_is_dev_mac(byte mac[])
{
    int i;

    for (i=0; i<umd.cfg.instance.count; i++) {
        if (os_maceq(mac, umd_getintf_byid(i)->mac)) {
            return true;
        }
    }

    return false;
}

STATIC bool
umd_is_dev_ip(uint32 ip)
{
    umd_intf_t *intf;
    int i;

    for (i=0; i<umd.cfg.instance.count; i++) {
        intf = umd_getintf_byid(i);
        
        if (ip==intf->ip) {
            return true;
        }
    }

    return false;
}

STATIC bool
umd_is_user_ip(uint32 ip)
{
    umd_intf_t *intf = umd_getintf_byid(conn.intf_id);
    
    return (ip & intf->mask)==(intf->ip & intf->mask);
}

STATIC bool
umd_is_lan_ip(uint32 ip)
{
    umd_plan_t *plan;
    int i;

    for (i=0; i<os_count_of(umd.plan); i++) {
        plan = &umd.plan[i];

        if ((ip & plan->mask)==(plan->ip & plan->mask)) {
            return true;
        }
    }

    return false;
}

STATIC int
umd_conn_handle(umd_conn_t *cn);

STATIC bkdr_t
umd_conn_bkdr(umd_conn_t *cn)
{
    bkdr_t bkdr = 0;

    bkdr = os_bin_bkdr_push(bkdr, cn->smac, sizeof(cn->smac));
    bkdr = os_bin_bkdr_push(bkdr, cn->dmac, sizeof(cn->dmac));
    bkdr = os_bin_bkdr_push(bkdr, &cn->sip, sizeof(cn->sip));
    bkdr = os_bin_bkdr_push(bkdr, &cn->dip, sizeof(cn->dip));
    bkdr = os_bin_bkdr_push(bkdr, &cn->protocol, sizeof(cn->protocol));

    return bkdr;
}

STATIC bkdr_t
umd_conn_eq(umd_conn_t *a, umd_conn_t *b)
{
    return a->bkdr == b->bkdr
        && os_maceq(a->smac, b->smac)
        && os_maceq(a->dmac, b->dmac)
        && a->sip == b->sip
        && a->dip == b->dip
        && a->protocol == b->protocol;
}

STATIC hash_idx_t 
umd_conn_hash(umd_conn_t *cn)
{
    return cn->bkdr & (umd.cfg.connhashsize - 1);
}

STATIC hash_idx_t
umd_conn_nhash(hash_node_t *node)
{
    umd_conn_t *cn = umd_conn_hx_entry(node);
    
    return umd_conn_hash(cn);
}

STATIC void
umd_conn_destroy(umd_conn_t *cn)
{
    os_free(cn);
}

STATIC umd_conn_t *
umd_conn_new(umd_conn_t *tmpl)
{
    umd_conn_t *cn = (umd_conn_t *)os_zalloc(sizeof(*cn));
    if (cn) {
        os_objcpy(cn, tmpl);
    }

    return cn;
}

STATIC int
umd_conn_insert(umd_conn_t *cn)
{
    return h1_add(&umd.head.conn, &cn->node.conn, umd_conn_nhash);
}

STATIC int
umd_conn_remove(umd_conn_t *cn)
{
    return h1_del(&umd.head.conn, &cn->node.conn);
}

STATIC umd_conn_t *
umd_conn_get(umd_conn_t *q)
{
    umd_conn_t *cn;
    
    hash_idx_t hash(void)
    {
        return umd_conn_hash(q);
    }
    
    bool eq(hash_node_t *node)
    {
        umd_conn_t *cn = umd_conn_hx_entry(node);
        
        return umd_conn_eq(q, cn);
    }

    return umd_conn_h1_entry(h1_find(&umd.head.conn, hash, eq));

}

STATIC umd_conn_t *
umd_conn_getEx(umd_conn_t *tmpl)
{
    umd_conn_t *cn = umd_conn_get(tmpl);
    if (cn) {
        return cn;
    }

    cn = umd_conn_new(tmpl);
    if (NULL==cn) {
        return NULL;
    }
    
    int err = umd_conn_insert(cn);
    if (err<0) {
        umd_conn_destroy(cn);

        return NULL;
    }

    return cn;
}

STATIC void
umd_conn_init(umd_conn_t *cn, struct ether_header *eth, struct ip *iph, time_t now)
{
    os_maccpy(cn->smac, eth->ether_shost);
    os_maccpy(cn->dmac, eth->ether_dhost);
    cn->sip = iph->ip_src.s_addr;
    cn->dip = iph->ip_dst.s_addr;
    cn->protocol = iph->ip_p;
    cn->hit = now;
    cn->bkdr = umd_conn_bkdr(cn);
}

STATIC void
umd_conn_clean(umd_conn_t *cn, sock_server_t *server)
{
    cn->intf_id     = umd_intf_id(server->id);
    cn->conn_dir    = 0;
    cn->flow_dir    = 0;
    cn->flow_type   = 0;
}

STATIC int
umd_conn_dir(uint32 sip, uint32 dip)
{
    if (umd_is_dev_ip(sip)) {
        if (umd_is_dev_ip(dip)) {
            return umd_conn_dir_dev2dev;
        }
        else if (umd_is_user_ip(dip)) {
            return umd_conn_dir_dev2user;            
        }
        else if (umd_is_lan_ip(dip)) {
            return umd_conn_dir_dev2lan;
        }
        else { /* dip is wan */
            return umd_conn_dir_dev2wan;
        }
    }
    else if (umd_is_user_ip(sip)) {
        if (umd_is_dev_ip(dip)) {
            return umd_conn_dir_user2dev;
        }
        else if (umd_is_user_ip(dip)) {
            return umd_conn_dir_user2user;
        }
        else if (umd_is_lan_ip(dip)) {
            return umd_conn_dir_user2lan;
        }
        else { /* dip is wan */
            return umd_conn_dir_user2wan;
        }
    }
    else if (umd_is_lan_ip(sip)) {
        if (umd_is_dev_ip(dip)) {
            return umd_conn_dir_lan2dev;
        }
        else if (umd_is_user_ip(dip)) {
            return umd_conn_dir_lan2dev;
        }
        else if (umd_is_lan_ip(dip)) {
            return umd_conn_dir_lan2lan;
        }
        else { /* dip is wan */
            return umd_conn_dir_lan2wan;
        }
    }
    else { /* sip is wan */
        if (umd_is_dev_ip(dip)) {
            return umd_conn_dir_wan2dev;
        }
        else if (umd_is_user_ip(dip)) {
            return umd_conn_dir_wan2user;
        }
        else if (umd_is_lan_ip(dip)) {
            return umd_conn_dir_wan2lan;
        }
        else { /* dip is wan */
            return umd_conn_dir_wan2wan;
        }
    }
}

STATIC int
umd_conn_set_user(umd_conn_t *cn, int flow_type, int flow_dir)
{
    cn->flow_type   = flow_type;
    cn->flow_dir    = flow_dir;
    cn->suser       = umd_flow_dir_up==flow_dir;

    return umd_conn_handle(cn);
}

STATIC int
umd_conn_not_support(umd_conn_t *cn)
{
    return -EFORMAT;
}

STATIC int
umd_conn_dev2user(umd_conn_t *cn)
{
    /*
    * dev==>user
    *
    * lan down for user
    */
    return umd_conn_set_user(cn, umd_flow_type_lan, umd_flow_dir_down);
}

STATIC int
umd_conn_dev2lan(umd_conn_t *cn)
{
    /*
    * dev==>lan
    *
    * lan up for dev
    */
    umd_add_flow_dev(umd_flow_type_lan, umd_flow_dir_up);
    
    return -EFORMAT;
}

STATIC int
umd_conn_dev2wan(umd_conn_t *cn)
{
    /*
    * dev==>wan
    *
    * wan up for dev
    */
    umd_add_flow_dev(umd_flow_type_wan, umd_flow_dir_up);
    
    return -EFORMAT;
}

STATIC int
umd_conn_user2dev(umd_conn_t *cn)
{
    /*
    * user==>dev
    *
    * lan up for user
    */
    return umd_conn_set_user(cn, umd_flow_type_lan, umd_flow_dir_up);
}

STATIC int
umd_conn_user2user(umd_conn_t *cn)
{
    int err;
    
    if (cn->sip==cn->dip) {
        return -EFORMAT;
    }
    
    /*
    * user==>{user}
    *
    * first, lan down for {user}
    */
    err = umd_conn_set_user(cn, umd_flow_type_lan, umd_flow_dir_down);
    if (err<0) {
        return err;
    }
    
    /*
    * {user}==>user
    *
    * again, lan up for {user}
    */
    err = umd_conn_set_user(cn, umd_flow_type_lan, umd_flow_dir_up);
    if (err<0) {
        return err;
    }
    
    return 0;
}

STATIC int
umd_conn_user2lan(umd_conn_t *cn)
{
    /*
    * user==>lan
    *
    * lan up for user
    */
    return umd_conn_set_user(cn, umd_flow_type_lan, umd_flow_dir_up);
}

STATIC int
umd_conn_user2wan(umd_conn_t *cn)
{
    /*
    * user==>wan
    *
    * wan up for user
    */
    return umd_conn_set_user(cn, umd_flow_type_wan, umd_flow_dir_up);
}

STATIC int
umd_conn_lan2dev(umd_conn_t *cn)
{
    /*
    * lan==>dev
    *
    * lan down for dev
    */
    umd_add_flow_dev(umd_flow_type_lan, umd_flow_dir_down);
    
    return -EFORMAT;
}

STATIC int
umd_conn_lan2user(umd_conn_t *cn)
{
    /*
    * lan==>user
    *
    * lan down for user
    */
    return umd_conn_set_user(cn, umd_flow_type_lan, umd_flow_dir_down);
}

STATIC int
umd_conn_wan2dev(umd_conn_t *cn)
{
    /*
    * wan==>dev
    *
    * wan down for dev
    */
    umd_add_flow_dev(umd_flow_type_wan, umd_flow_dir_down);
    
    return -EFORMAT;
}

STATIC int
umd_conn_wan2user(umd_conn_t *cn)
{
    /*
    * wan==>user
    *
    * wan down for user
    */
    return umd_conn_set_user(cn, umd_flow_type_wan, umd_flow_dir_down);
}

STATIC int
umd_conn_update(umd_conn_t *cn, time_t now)
{
    static umd_conn_handle_f *handler[umd_conn_dir_end] = {
        [umd_conn_dir_dev2dev]  = umd_conn_not_support,
        [umd_conn_dir_dev2user] = umd_conn_dev2user,
        [umd_conn_dir_dev2lan]  = umd_conn_dev2lan,
        [umd_conn_dir_dev2wan]  = umd_conn_dev2wan,
        [umd_conn_dir_user2dev] = umd_conn_user2dev,
        [umd_conn_dir_user2user]= umd_conn_user2user,
        [umd_conn_dir_user2lan] = umd_conn_user2lan,
        [umd_conn_dir_user2wan] = umd_conn_user2wan,
        [umd_conn_dir_lan2dev]  = umd_conn_lan2dev,
        [umd_conn_dir_lan2user] = umd_conn_lan2user,
        [umd_conn_dir_lan2lan]  = umd_conn_not_support,
        [umd_conn_dir_lan2wan]  = umd_conn_not_support,
        [umd_conn_dir_wan2dev]  = umd_conn_wan2dev,
        [umd_conn_dir_wan2user] = umd_conn_wan2user,
        [umd_conn_dir_wan2lan]  = umd_conn_not_support,
        [umd_conn_dir_wan2wan]  = umd_conn_not_support,
    };

    if (false==is_good_umd_conn_dir(cn->conn_dir)) {
        cn->conn_dir = umd_conn_dir(cn->sip, cn->dip);
    }
    cn->hit = now;
    
    umd_conn_handle_f *f = handler[cn->conn_dir];
    if (f) {
        return (*f)(cn);
    } else {
        return -EFORMAT;
    }
}

mv_t
umd_conn_gc(umd_conn_t *cn)
{
    umd_conn_remove(cn);
    umd_conn_destroy(cn);

    return mv2_ok;
}

STATIC bool
is_umd_conn_gc(umd_conn_t *cn, time_t now)
{
    time_t hit = cn->hit;
    
    return umd.cfg.connectable 
        && umd.cfg.gcconn 
        && (hit < now) 
        && (now - hit > umd.cfg.gcconn);
}

STATIC void
umd_conn_gc_auto(umd_conn_t *cn, time_t now)
{
    if (is_umd_conn_gc(cn, now)) {
        umd_conn_gc(cn);
    }
}

STATIC void
umd_conn_timer_handle(umd_conn_t *cn, time_t now)
{
    static umd_conn_timer_f *handler[] = {
        umd_conn_gc_auto,
    };
    
    int i;

    for (i=0; i<os_count_of(handler); i++) {
        (*handler[i])(cn, now);
    }
}

int
umd_conn_timer(struct loop_watcher *watcher, time_t now)
{
    mv_t foreach(umd_conn_t *cn)
    {
        umd_conn_timer_handle(cn, now);

        return mv2_ok;
    }

    if (umd.cfg.connectable) {
        umd_conn_foreach(foreach, true);
    }

    return 0;
}

STATIC int
umd_pkt_handle(sock_server_t *server, time_t now)
{
    socklen_t addrlen = sizeof(server->addr.ll);
    int err;

    umd_conn_clean(&conn, server);

    err = flow.len = recvfrom(server->fd, flow.packet, sizeof(flow.packet), 0, &server->addr.c, &addrlen);
    if (err<0) { /* yes, <0 */
        return err;
    }

    if (__is_ak_debug_packet) {
        os_println("recv packet length=%d", flow.len);
        
        os_dump_buffer(flow.packet, flow.len);
    }

    return 0;
}

STATIC int
umd_intf_handle(sock_server_t *server, time_t now)
{
    umd_intf_t *intf = umd_getintf_byid(conn.intf_id);
    
    if (server->addr.ll.sll_ifindex == intf->index) {
        return 0;
    } else {
        return -EBADIDX;
    }
}

STATIC int
umd_eth_handle(sock_server_t *server, time_t now)
{
    struct ether_header *eth = flow.eth = umd_get_eth();
    
    byte *smac = eth->ether_shost;
    byte *dmac = eth->ether_dhost;

    umd_add_flow_all(umd_pkt_type_eth);
    
    if (eth->ether_type != umd.ether_type_ip &&
        eth->ether_type != umd.ether_type_vlan) {
        /*
        * bad ether type
        */
        umd_add_flow_bad(umd_pkt_type_eth);
        
        return -EFORMAT;
    }
    else if (false==is_good_mac(smac)) {
        /*
        * bad src mac
        */
        umd_add_flow_bad(umd_pkt_type_eth);
        
        return -EFORMAT;
    }
    else if (false==is_good_mac(dmac)) {
        /*
        * bad dst mac
        */
        umd_add_flow_bad(umd_pkt_type_eth);
        
        return -EFORMAT;
    }

    umd_add_flow_good(umd_pkt_type_eth);

    if (__is_ak_debug_flow) {
        char smacstring[1+MACSTRINGLEN_L];
        char dmacstring[1+MACSTRINGLEN_L];
        
        os_strcpy(smacstring, os_macstring(smac));
        os_strcpy(dmacstring, os_macstring(dmac));
        
        debug_flow("recv packet smac=%s dmac=%s type=0x%x", 
            smacstring,
            dmacstring,
            ntohs(eth->ether_type));
    }

    return 0;
}

STATIC int
umd_vlan_handle(sock_server_t *server, time_t now)
{
    if (flow.eth->ether_type == umd.ether_type_vlan) {
        struct vlan_header *vlan = flow.vlan = umd_get_vlan();

        umd_add_flow_all(umd_pkt_type_vlan);
        
        if (vlan->type != umd.ether_type_ip) {
            umd_add_flow_bad(umd_pkt_type_vlan);
            
            return -EFORMAT;
        }

        if (__is_ak_debug_flow) {
            debug_flow("recv packet vlan=%d type=%d", 
                vlan->vid,
                vlan->type);
        }
        
        umd_add_flow_good(umd_pkt_type_vlan);
    }
    
    return 0;
}

STATIC int
umd_ip_handle(sock_server_t *server, time_t now)
{
    umd_conn_t *cn;
    int err = 0;
    
    umd_add_flow_all(umd_pkt_type_ip);

    struct ip *iph = flow.iph = umd_get_iph();
    if (NULL==iph) {
        err = -EFORMAT; goto error;
    }
    else if (4!=iph->ip_v) {
        err = -EFORMAT; goto error;
    }
    else if (5!=iph->ip_hl) {
        err = -EFORMAT; goto error;
    }

    umd_conn_init(&conn, flow.eth, iph, now);

    if (umd.cfg.connectable) {
        cn = umd_conn_getEx(&conn);
        if (NULL==cn) {
            err = -ENOMEM; goto error;
        }
    } else {
        cn = &conn;
    }

    err = umd_conn_update(cn, now);
error:
    if (err<0) {
        umd_add_flow_bad(umd_pkt_type_ip);
    } else {
        umd_add_flow_good(umd_pkt_type_ip);
    }
    
    if (__is_ak_debug_flow) {
        char sipstring[1+OS_IPSTRINGLEN];
        char dipstring[1+OS_IPSTRINGLEN];
        char  ipstring[1+OS_IPSTRINGLEN];
        char macstring[1+MACSTRINGLEN_L];
        
        os_strcpy(sipstring, os_ipstring(cn->sip));
        os_strcpy(dipstring, os_ipstring(cn->dip));
        os_strcpy( ipstring, os_ipstring(umd_conn_userip(cn)));
        os_strcpy( macstring, os_macstring(umd_conn_usermac(cn)));

        debug_flow("recv packet"
                        " sip=%s"
                        " dip=%s"
                        " protocol=%d"
                        " dir=%s"
                        " type=%s"
                        " userip=%s"
                        " usermac=%s"
                        " err=%d",
            sipstring,
            dipstring,
            cn->protocol,
            umd_flow_dir_getnamebyid(cn->flow_dir),
            umd_flow_type_getnamebyid(cn->flow_type),
            ipstring,
            macstring,
            err);
    }
    
    return err;
}

STATIC bool
umd_is_flow_reauth_helper(umd_user_t *user, int type, int dir)
{
    uint64 reauth = umd_flow_reauthor(user, type, dir);
    uint64 now    = umd_flow_now(user, type, dir);
    
    bool is = reauth && reauth < now;
    if (is) {
        debug_flow("user(%s) type(%s) dir(%s) flow reauth(%llu) > now(%llu)",
            os_macstring(user->mac),
            umd_flow_type_getnamebyid(type),
            umd_flow_dir_getnamebyid(dir),
            reauth,
            now);
    }
    
    return is;
}

STATIC bool
umd_is_flow_reauth(umd_user_t *user, int type, int dir)
{
    return umd_is_flow_reauth_helper(user, type, dir)
        || umd_is_flow_reauth_helper(user, type, umd_flow_dir_all);
}

STATIC void
umd_flow_reauth(umd_user_t *user, int type, int dir)
{
    if (umd.cfg.reauthable
            && umd_flow_type_wan==type
            && is_user_auth(user)
            && umd_is_flow_reauth(user, type, dir)) {
        umduser_reauth(user);
    }
}

STATIC bool
umd_is_overflow_helper(umd_user_t *user, int type, int dir)
{
    uint64 max    = umd_flow_max(user, type, dir);
    uint64 now    = umd_flow_now(user, type, dir);
    
    bool is = max && max < now;
    if (is) {
        debug_flow("user(%s) type(%s) dir(%s) flow max(%llu) > now(%llu)",
            os_macstring(user->mac),
            umd_flow_type_getnamebyid(type),
            umd_flow_dir_getnamebyid(dir),
            max,
            now);
    }

    return is;
}

STATIC bool
umd_is_overflow(umd_user_t *user, int type, int dir)
{
    return umd_is_overflow_helper(user, type, dir)
        || umd_is_overflow_helper(user, type, umd_flow_dir_all);
}

STATIC void
umd_overflow(umd_user_t *user, int type, int dir)
{
    if (umd_flow_type_wan==type && is_user_auth(user) && umd_is_overflow(user, type, dir)) {
        umduser_deauth(user, UMD_DEAUTH_FLOWLIMIT);
    }

    /*
    * lan's flow include wan's flow
    */
    if (umd_is_overflow(user, umd_flow_type_lan, dir)) {
        umduser_unbind(user, UMD_DEAUTH_FLOWLIMIT);
    }
}

STATIC void
umd_flow_update(umd_user_t *user, int type, int dir)
{
    if (umd_flow_dir_all!=dir) {
        umd_flow_now(user, type, dir) += flow.len;
    }
    umd_flow_now(user, type, umd_flow_dir_all) += flow.len;
    
    /*
    * lan' flow include wan's flow
    */
    if (umd_flow_type_wan==type) {
        umd_flow_update(user, umd_flow_type_lan, dir);
    }
}

STATIC int
umd_conn_handle(umd_conn_t *cn)
{
    umd_user_t *user;
    byte *usermac = umd_conn_usermac(cn);
    uint32 userip = umd_conn_userip(cn);
    
    user = umd_user_get(usermac);
    if (NULL==user) {
        switch(umd.cfg.autouser) {
            case UMD_AUTO_BIND:
                user = umd_user_bind(usermac, userip);
                break;
            case UMD_AUTO_FAKE:
                user = umd_user_fake(usermac, userip);
                break;
            case UMD_AUTO_NONE:
            default:
                break;
        }
        
        if (NULL==user) {
            return -ENOEXIST;
        }
    }

    user->hitime = time(NULL);

    if (is_user_have_bind(user)) {
        umd_flow_update(user, cn->flow_type, cn->flow_dir);
        umd_user_debug_helper("user-flow-update", user, __is_ak_debug_flow);

        umd_flow_reauth(user, cn->flow_type, cn->flow_dir);
        umd_overflow(user, cn->flow_type, cn->flow_dir);

        umd_update_aging(user, false);
    }

    return 0;
}

STATIC jobj_t
umd_jflow_st(umd_flowst_t *st)
{
    jobj_t jst = jobj_new_object();

    jobj_add_i32(jst, "packets", st->packets);
    jobj_add_i64(jst, "bytes", st->bytes);

    return jst;
}

STATIC jobj_t
umd_jflow_total(int type, int valid)
{
    return umd_jflow_st(&flow.st.total[type][valid]);
}

STATIC jobj_t
umd_jflow_dev(int type, int dir)
{
    return umd_jflow_st(&flow.st.dev[type][dir]);
}

jobj_t
umd_jflow(void)
{
    jobj_t jflow, jtotal, jdev, jtype, jst;
    int i, j;
    
    jflow = jobj_new_object();
    
    jtotal = jobj_new_object();
    for (i=0; i<umd_pkt_type_end; i++) {
        jtype = jobj_new_object();
        for (j=0; j<umd_pkt_check_end; j++) {
            jst = umd_jflow_total(i, j);
            jobj_add(jtype, umd_pkt_check_getnamebyid(j), jst);
        }
        jobj_add(jtotal, umd_pkt_check_getnamebyid(i), jtype);
    }
    jobj_add(jflow, "total", jtotal);
    
    jdev = jobj_new_object();
    for (i=0; i<umd_flow_type_end; i++) {
        jtype = jobj_new_object();
        for (j=0; j<umd_flow_dir_end; j++) {
            jst = umd_jflow_dev(i, j);
            jobj_add(jtype, umd_flow_dir_getnamebyid(j), jst);
        }
        jobj_add(jdev, umd_flow_type_getnamebyid(i), jtype);
    }
    jobj_add(jflow, "dev", jdev);
    
    return jflow;
}

STATIC int
umd_flower(struct loop_watcher *watcher, time_t now)
{
    static umd_pkt_handle_f *handler[] = {
        umd_pkt_handle,
        umd_intf_handle,
        umd_eth_handle,
        umd_vlan_handle,
        umd_ip_handle,
    };

    sock_server_t *server = (sock_server_t *)watcher->user;
    int i, err;

    for (i=0; i<os_count_of(handler); i++) {
        err = (*handler[i])(server, now);
        if (err<0) {
            return err;
        }
    }

    return 0;
}

STATIC int
umd_flow_server_init(sock_server_t *server)
{
    int fd, err;

    fd = socket(AF_PACKET, SOCK_RAW, umd.ether_type_all);
    if (fd<0) {
        debug_error("socket error:%d", -errno);

        return -errno;
    }
    os_closexec(fd);
    os_noblock(fd);

    umd_intf_t *intf = umd_getintf_byserver(server);

    sockaddr_ll_t addr = {
        .sll_family     = AF_PACKET,
        .sll_protocol   = umd.ether_type_all,
        .sll_ifindex    = intf->index,
    };

    err = bind(fd, (sockaddr_t *)(&addr), sizeof(addr));
    if (err<0) {
        debug_error("bind error:%d", -errno);
        
        return -errno;
    }

    server->fd = fd;

    err = os_loop_add_normal(&umd.loop, fd, umd_flower, server);
    if (err<0) {
        debug_error("add loop intf %s flow error:%d", intf->name, err);

        return err;
    }
    debug_ok("init flow server ok");
    
    return 0;
}

sock_server_t umd_flow_server = 
    SOCK_SERVER_INITER(UMD_SERVER_FLOW, AF_PACKET, umd_flow_server_init, NULL);
/******************************************************************************/
