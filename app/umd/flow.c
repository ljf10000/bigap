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

// todo: use hash later
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
    umd_intf_t *intf = conn.intf;
    
    return (ip & intf->mask)==(intf->ip & intf->mask);
}

STATIC bool
umd_is_lan_ip(uint32 ip)
{
    umd_lan_t *lan;
    int i;

    for (i=0; i<os_count_of(umd.lan); i++) {
        lan = &umd.lan[i];

        if ((ip & lan->mask)==(lan->ip & lan->mask)) {
            return true;
        }
    }

    return false;
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
#define umd_add_flow_good(_type)    umd_add_flow_total(_type, umd_pkt_check_good)
#define umd_add_flow_bad(_type)     umd_add_flow_total(_type, umd_pkt_check_bad)
#define umd_add_flow_all(_type)     umd_add_flow_total(_type, umd_pkt_check_all)

STATIC void
umd_set_flow_dev(void)
{
    umd_add_flowst(&flow.st.dev[conn.flow_type][conn.flow_dir]);
}

STATIC void
umd_set_flow_suser(void)
{
    conn.usermac= flow.eth->ether_shost;
    conn.userip = flow.iph->ip_src.s_addr;
}

STATIC void
umd_set_flow_duser(void)
{
    conn.usermac= flow.eth->ether_dhost;
    conn.userip = flow.iph->ip_dst.s_addr;
}

STATIC int
umd_pkt_handle(sock_server_t *server)
{
    int err, len;
    socklen_t addrlen = sizeof(server->addr.ll);
    
    conn.usermac= NULL;
    conn.userip = 0;
    conn.intf   = umd_getintf_byserver(server);
    conn.user   = NULL;

    err = recvfrom(server->fd, flow.packet, sizeof(flow.packet), 0, &server->addr.c, &addrlen);
    if (err<0) { /* yes, <0 */
        return err;
    }
    flow.len    = err;
    
    if (__is_ak_debug_packet) {
        os_println("recv packet length=%d", flow.len);
        
        os_dump_buffer(flow.packet, flow.len);
    }

    return 0;
}

STATIC int
umd_intf_handle(sock_server_t *server)
{
    if (server->addr.ll.sll_ifindex == conn.intf->index) {
        return 0;
    } else {
        return -EBADIDX;
    }
}

STATIC int
umd_eth_handle(sock_server_t *server)
{
    struct ether_header *eth = flow.eth = (struct ether_header *)flow.packet;
    
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
umd_vlan_handle(sock_server_t *server)
{
    if (flow.eth->ether_type == umd.ether_type_vlan) {
        struct vlan_header *vlan = flow.vlan = (struct vlan_header *)(flow.eth+1);

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
umd_ip_source_dev(uint32 sip, uint32 dip)
{
    if (umd_is_dev_ip(dip)) {
        /*
        * dev==>dev, invalid
        */
        
        return -EFORMAT;
    }
    else if (umd_is_user_ip(dip)) {
        /*
        * dev==>user
        *
        * lan down for user
        */
        conn.flow_dir    = umd_flow_dir_down;
        conn.flow_type   = umd_flow_type_lan;
        
        umd_set_flow_duser();
    }
    else if (umd_is_lan_ip(dip)) {
        /*
        * dev==>lan
        *
        * lan up for dev
        */
        conn.flow_dir    = umd_flow_dir_up;
        conn.flow_type   = umd_flow_type_lan;

        umd_set_flow_dev();
    }
    else { /* dip is wan */
        /*
        * dev==>wan
        *
        * wan up for dev
        */
        conn.flow_dir    = umd_flow_dir_up;
        conn.flow_type   = umd_flow_type_wan;

        umd_set_flow_dev();
    }

    return 0;
}

STATIC int
umd_ip_source_user(uint32 sip, uint32 dip, bool first)
{
    if (umd_is_dev_ip(dip)) {
        /*
        * user==>dev
        *
        * lan up for user
        */
        conn.flow_dir    = umd_flow_dir_up;
        conn.flow_type   = umd_flow_type_lan;

        umd_set_flow_suser();
    }
    else if (umd_is_user_ip(dip)) {
        if (sip==dip) {
            return -EFORMAT;
        }

        if (first) {
            /*
            * {user}==>user
            *
            * first, lan up for {user}
            */
            conn.flow_dir    = umd_flow_dir_up;
            conn.flow_type   = umd_flow_type_lan;

            umd_set_flow_suser();

            return 1; // try again
        } else {
            /*
            * user==>{user}
            *
            * again, lan down for {user}
            */
            conn.flow_dir    = umd_flow_dir_down;
            conn.flow_type   = umd_flow_type_lan;

            umd_set_flow_duser();

            // not again
        }
    }
    else if (umd_is_lan_ip(dip)) {
        /*
        * user==>lan
        *
        * lan up for user
        */
        conn.flow_dir    = umd_flow_dir_up;
        conn.flow_type   = umd_flow_type_lan;

        umd_set_flow_suser();
    }
    else { /* dip is wan */
        /*
        * user==>wan
        *
        * wan up for user
        */
        conn.flow_dir    = umd_flow_dir_up;
        conn.flow_type   = umd_flow_type_wan;

        umd_set_flow_suser();
    }

    return 0;
}

STATIC int
umd_ip_source_lan(uint32 sip, uint32 dip)
{
    if (umd_is_dev_ip(dip)) {
        /*
        * lan==>dev
        *
        * lan down for dev
        */
        conn.flow_dir    = umd_flow_dir_down;
        conn.flow_type   = umd_flow_type_lan;

        umd_set_flow_dev();
    }
    else if (umd_is_user_ip(dip)) {
        /*
        * lan==>user
        *
        * lan down for user
        */
        conn.flow_dir    = umd_flow_dir_down;
        conn.flow_type   = umd_flow_type_lan;

        umd_set_flow_duser();
    }
    else if (umd_is_lan_ip(dip)) {
        /*
        * lan==>lan, invalid
        */
        return -EFORMAT;
    }
    else { /* dip is wan */
        /*
        * lan==>wan, invalid
        */
        return -EFORMAT;
    }

    return 0;
}

STATIC int
umd_ip_source_wan(uint32 sip, uint32 dip)
{
    if (umd_is_dev_ip(dip)) {
        /*
        * wan==>dev
        *
        * wan down for dev
        */
        conn.flow_dir    = umd_flow_dir_down;
        conn.flow_type   = umd_flow_type_wan;

        umd_set_flow_dev();
    }
    else if (umd_is_user_ip(dip)) {
        /*
        * wan==>user
        *
        * wan down for user
        */
        conn.flow_dir   = umd_flow_dir_down;
        conn.flow_type  = umd_flow_type_wan;
        
        umd_set_flow_duser();
    }
    else if (umd_is_lan_ip(dip)) {
        /*
        * wan==>lan, invalid
        */
        
        return -EFORMAT;
    }
    else { /* dip is wan */
        /*
        * wan==>wan, invalid
        */
        return -EFORMAT;
    }

    return 0;
}

STATIC int
umd_flow_handle(sock_server_t *server);

STATIC int
umd_ip_handle_helper(sock_server_t *server, bool first)
{
    struct ip *iph;
    bool again = false;
    int err;
    
    umd_add_flow_all(umd_pkt_type_ip);

    if (flow.eth->ether_type == umd.ether_type_ip) {
        iph = flow.iph = (struct ip *)(flow.eth+1);
    }
    else if (flow.eth->ether_type == umd.ether_type_vlan) {
        iph = flow.iph = (struct ip *)(flow.vlan+1);
    }
    else {
        if (first) {
            umd_add_flow_bad(umd_pkt_type_ip);
        }
        
        return -EFORMAT;
    }

    uint32 sip = iph->ip_src.s_addr;
    uint32 dip = iph->ip_dst.s_addr;

    if (4!=iph->ip_v) {
        if (first) {
            umd_add_flow_bad(umd_pkt_type_ip);
        }
        
        return -EFORMAT;
    }
    else if (5!=iph->ip_hl) {
        if (first) {
            umd_add_flow_bad(umd_pkt_type_ip);
        }
        
        return -EFORMAT;
    }

    if (umd_is_dev_ip(sip)) {
        err = umd_ip_source_dev(sip, dip);
    }
    else if (umd_is_user_ip(sip)) {
        err = umd_ip_source_user(sip, dip, first);
        again = (1==err);
    }
    else if (umd_is_lan_ip(sip)) {
        err = umd_ip_source_lan(sip, dip);
    }
    else { /* sip is wan */
        err = umd_ip_source_wan(sip, dip);
    }
    if (err<0) {
        if (first) {
            umd_add_flow_bad(umd_pkt_type_ip);
        }
        
        return err;
    }

    if (first) {
        umd_add_flow_good(umd_pkt_type_ip);
    }
    
    if (__is_ak_debug_flow) {
        char sipstring[1+OS_IPSTRINGLEN];
        char dipstring[1+OS_IPSTRINGLEN];
        char  ipstring[1+OS_IPSTRINGLEN];
        char macstring[1+MACSTRINGLEN_L];
        
        os_strcpy(sipstring, os_ipstring(iph->ip_src.s_addr));
        os_strcpy(dipstring, os_ipstring(iph->ip_dst.s_addr));
        os_strcpy( ipstring, os_ipstring(conn.userip));
        os_strcpy( macstring, os_macstring(conn.usermac));
        
        debug_flow("recv packet"
                        " sip=%s"
                        " dip=%s"
                        " protocol=%d"
                        " dir=%s"
                        " type=%s"
                        " userip=%s"
                        " usermac=%s",
            sipstring,
            dipstring,
            iph->ip_p,
            umd_flow_dir_getnamebyid(conn.flow_dir),
            umd_flow_type_getnamebyid(conn.flow_type),
            ipstring,
            macstring);
    }

    if (NULL==conn.usermac || 0==conn.userip) {
        return -ENOSUPPORT;
    }
    
    umd_flow_handle(server);

    return again?1:0;
}

STATIC int
umd_ip_handle(sock_server_t *server)
{
    int err = umd_ip_handle_helper(server, true);
    if (err>0) { // yes > 0, again
        err = umd_ip_handle_helper(server, false);
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
umd_flow_handle(sock_server_t *server)
{
    umd_user_t *user;

    user = umd_user_get(conn.usermac);
    if (NULL==user) {
        switch(umd.cfg.autouser) {
            case UMD_AUTO_BIND:
                user = umd_user_bind(conn.usermac, conn.userip);
                break;
            case UMD_AUTO_FAKE:
                user = umd_user_fake(conn.usermac, conn.userip);
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
        umd_flow_update(user, conn.flow_type, conn.flow_dir);
        umd_user_debug_helper("user-flow-update", user, __is_ak_debug_flow);

        umd_flow_reauth(user, conn.flow_type, conn.flow_dir);
        umd_overflow(user, conn.flow_type, conn.flow_dir);

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
    static int (*handle[])(sock_server_t *) = {
        umd_pkt_handle,
        umd_intf_handle,
        umd_eth_handle,
        umd_vlan_handle,
        umd_ip_handle,
    };

    sock_server_t *server = (sock_server_t *)watcher->user;
    int i, err;

    for (i=0; i<os_count_of(handle); i++) {
        err = (*handle[i])(server);
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
