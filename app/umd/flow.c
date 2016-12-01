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

STATIC struct um_flow umd_flow;

STATIC bool
umd_is_dev_mac(byte mac[])
{
    int i;

    for (i=0; i<umd.cfg.instance.count; i++) {
        if (os_maceq(mac, umd_get_intf_by_id(i)->mac)) {
            return true;
        }
    }
    
    return false;
}

STATIC bool
umd_is_dev_ip(uint32 ip)
{
    int i;

    for (i=0; i<umd.cfg.instance.count; i++) {
        if (ip==umd_get_intf_by_id(i)->ip) {
            return true;
        }
    }

    return false;
}

STATIC bool
umd_is_user_ip(uint32 ip)
{
    struct um_intf *intf = umd_flow.intf;
    
    return (ip & intf->mask)==(intf->ip & intf->mask);
}

STATIC bool
umd_is_lan_ip(uint32 ip)
{
    int i;
    
    for (i=0; i<os_count_of(umd.lan); i++) {
        struct um_lan *lan = &umd.lan[i];

        if ((ip & lan->mask)==(lan->ip & lan->mask)) {
            return true;
        }
    }

    return false;
}

STATIC void
umd_add_flowst(struct um_flowst *st)
{
    st->packets++;
    st->bytes += umd_flow.len;
}

STATIC void
umd_add_flow_total(int type, int valid)
{
    umd_add_flowst(&umd_flow.total[type][valid]);
}
#define umd_add_flow_good(_type)    umd_add_flow_total(_type, um_pkt_check_good)
#define umd_add_flow_bad(_type)     umd_add_flow_total(_type, um_pkt_check_bad)
#define umd_add_flow_all(_type)     umd_add_flow_total(_type, um_pkt_check_all)

STATIC void
umd_set_flow_dev(void)
{
    umd_add_flowst(&umd_flow.dev[umd_flow.type][umd_flow.dir]);
}

STATIC void
umd_set_flow_suser(void)
{
    umd_flow.usermac= umd_flow.eth->ether_shost;
    umd_flow.userip = umd_flow.iph->ip_src.s_addr;
}

STATIC void
umd_set_flow_duser(void)
{
    umd_flow.usermac= umd_flow.eth->ether_dhost;
    umd_flow.userip = umd_flow.iph->ip_dst.s_addr;
}

STATIC int
umd_pkt_handle(sock_server_t *server)
{
    int err, len;
    socklen_t addrlen = sizeof(server->addr.ll);

    err = recvfrom(server->fd, umd_flow.packet, sizeof(umd_flow.packet), 0, &server->addr.c, &addrlen);
    if (err<0) { /* yes, <0 */
        return err;
    }
    umd_flow.len    = err;
    umd_flow.usermac= NULL;
    umd_flow.userip = 0;
    umd_flow.intf   = umd_get_intf_by_server(server);
    
    if (__is_ak_debug_packet) {
        os_println("recv packet length=%d", umd_flow.len);
        
        __os_dump_buffer(umd_flow.packet, umd_flow.len, NULL);
    }
    
    return 0;
}

STATIC int
umd_intf_handle(sock_server_t *server)
{
    if (server->addr.ll.sll_ifindex == umd_flow.intf->index) {
        return 0;
    } else {
        return -EBADIDX;
    }
}

STATIC int
umd_eth_handle(sock_server_t *server)
{
    struct ether_header *eth = umd_flow.eth = (struct ether_header *)umd_flow.packet;
    
    byte *smac = eth->ether_shost;
    byte *dmac = eth->ether_dhost;

    umd_add_flow_all(um_pkt_type_eth);
    
    if (eth->ether_type != umd.ether_type_ip &&
        eth->ether_type != umd.ether_type_vlan) {
        /*
        * bad ether type
        */
        umd_add_flow_bad(um_pkt_type_eth);
        
        return -EFORMAT;
    }
    else if (false==is_good_mac(smac)) {
        /*
        * bad src mac
        */
        umd_add_flow_bad(um_pkt_type_eth);
        
        return -EFORMAT;
    }
    else if (false==is_good_mac(dmac)) {
        /*
        * bad dst mac
        */
        umd_add_flow_bad(um_pkt_type_eth);
        
        return -EFORMAT;
    }

    umd_add_flow_good(um_pkt_type_eth);

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
    if (umd_flow.eth->ether_type == umd.ether_type_vlan) {
        struct vlan_header *vlan = umd_flow.vlan = (struct vlan_header *)(umd_flow.eth+1);

        umd_add_flow_all(um_pkt_type_vlan);
        
        if (vlan->type != umd.ether_type_ip) {
            umd_add_flow_bad(um_pkt_type_vlan);
            
            return -EFORMAT;
        }

        if (__is_ak_debug_flow) {
            debug_flow("recv packet vlan=%d type=%d", 
                vlan->vid,
                vlan->type);
        }
        
        umd_add_flow_good(um_pkt_type_vlan);
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
        umd_flow.dir    = um_flow_dir_down;
        umd_flow.type   = um_flow_type_lan;
        
        umd_set_flow_duser();
    }
    else if (umd_is_lan_ip(dip)) {
        /*
        * dev==>lan
        *
        * lan up for dev
        */
        umd_flow.dir    = um_flow_dir_up;
        umd_flow.type   = um_flow_type_lan;

        umd_set_flow_dev();
    }
    else { /* dip is wan */
        /*
        * dev==>wan
        *
        * wan up for dev
        */
        umd_flow.dir    = um_flow_dir_up;
        umd_flow.type   = um_flow_type_wan;

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
        umd_flow.dir    = um_flow_dir_up;
        umd_flow.type   = um_flow_type_lan;

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
            umd_flow.dir    = um_flow_dir_up;
            umd_flow.type   = um_flow_type_lan;

            umd_set_flow_suser();

            return 1; // try again
        } else {
            /*
            * user==>{user}
            *
            * again, lan down for {user}
            */
            umd_flow.dir    = um_flow_dir_down;
            umd_flow.type   = um_flow_type_lan;

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
        umd_flow.dir    = um_flow_dir_up;
        umd_flow.type   = um_flow_type_lan;

        umd_set_flow_suser();
    }
    else { /* dip is wan */
        /*
        * user==>wan
        *
        * wan up for user
        */
        umd_flow.dir    = um_flow_dir_up;
        umd_flow.type   = um_flow_type_wan;

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
        umd_flow.dir    = um_flow_dir_down;
        umd_flow.type   = um_flow_type_lan;

        umd_set_flow_dev();
    }
    else if (umd_is_user_ip(dip)) {
        /*
        * lan==>user
        *
        * lan down for user
        */
        umd_flow.dir    = um_flow_dir_down;
        umd_flow.type   = um_flow_type_lan;

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
        umd_flow.dir    = um_flow_dir_down;
        umd_flow.type   = um_flow_type_wan;

        umd_set_flow_dev();
    }
    else if (umd_is_user_ip(dip)) {
        /*
        * wan==>user
        *
        * wan down for user
        */
        umd_flow.dir    = um_flow_dir_down;
        umd_flow.type   = um_flow_type_wan;
        
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
    
    umd_add_flow_all(um_pkt_type_ip);

    if (umd_flow.eth->ether_type == umd.ether_type_ip) {
        iph = umd_flow.iph = (struct ip *)(umd_flow.eth+1);
    }
    else if (umd_flow.eth->ether_type == umd.ether_type_vlan) {
        iph = umd_flow.iph = (struct ip *)(umd_flow.vlan+1);
    }
    else {
        if (first) {
            umd_add_flow_bad(um_pkt_type_ip);
        }
        
        return -EFORMAT;
    }

    uint32 sip = iph->ip_src.s_addr;
    uint32 dip = iph->ip_dst.s_addr;

    if (4!=iph->ip_v) {
        if (first) {
            umd_add_flow_bad(um_pkt_type_ip);
        }
        
        return -EFORMAT;
    }
    else if (5!=iph->ip_hl) {
        if (first) {
            umd_add_flow_bad(um_pkt_type_ip);
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
            umd_add_flow_bad(um_pkt_type_ip);
        }
        
        return err;
    }

    if (first) {
        umd_add_flow_good(um_pkt_type_ip);
    }
    
    if (__is_ak_debug_flow) {
        char sipstring[1+OS_IPSTRINGLEN];
        char dipstring[1+OS_IPSTRINGLEN];
        char  ipstring[1+OS_IPSTRINGLEN];
        char macstring[1+MACSTRINGLEN_L];
        
        os_strcpy(sipstring, os_ipstring(iph->ip_src.s_addr));
        os_strcpy(dipstring, os_ipstring(iph->ip_dst.s_addr));
        os_strcpy( ipstring, os_ipstring(umd_flow.userip));
        os_strcpy( macstring, os_macstring(umd_flow.usermac));
        
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
            flow_dir_getnamebyid(umd_flow.dir),
            flow_type_getnamebyid(umd_flow.type),
            ipstring,
            macstring);
    }

    if (NULL==umd_flow.usermac || 0==umd_flow.userip) {
        return -ENOSUPPORT;
    }
    
    umd_flow_handle(server);

    return again?1:0;
}

STATIC int
umd_ip_handle(sock_server_t *server)
{
    int err = umd_ip_handle_helper(server, true);
    bool again = (1==err);
    
    if (1==err) {
        err = umd_ip_handle_helper(server, false);
    }
    
    return err;
}

STATIC bool
umd_is_flow_reauth_helper(struct um_user *user, int type, int dir)
{
    uint64 reauth = umd_flow_reauth(user, type, dir);
    uint64 now    = umd_flow_now(user, type, dir);
    
    bool is = reauth && reauth < now;
    if (is) {
        debug_flow("user(%s) type(%s) dir(%s) flow reauth(%llu) > now(%llu)",
            os_macstring(user->mac),
            flow_type_getnamebyid(type),
            flow_dir_getnamebyid(dir),
            reauth,
            now);
    }
    
    return is;
}

STATIC bool
umd_is_flow_reauth(struct um_user *user, int type, int dir)
{
    return umd_is_flow_reauth_helper(user, type, dir)
        || umd_is_flow_reauth_helper(user, type, um_flow_dir_all);
}

STATIC void
umd_flow_reauth(struct um_user *user, int type, int dir)
{
    if (umd.cfg.reauthable
            && um_flow_type_wan==type
            && is_user_auth(user)
            && umd_is_flow_reauth(user, type, dir)) {
        user_reauth(user);
    }
}

STATIC bool
umd_is_overflow_helper(struct um_user *user, int type, int dir)
{
    uint64 max    = umd_flow_max(user, type, dir);
    uint64 now    = umd_flow_now(user, type, dir);
    
    bool is = max && max < now;
    if (is) {
        debug_flow("user(%s) type(%s) dir(%s) flow max(%llu) > now(%llu)",
            os_macstring(user->mac),
            flow_type_getnamebyid(type),
            flow_dir_getnamebyid(dir),
            max,
            now);
    }

    return is;
}

STATIC bool
umd_is_overflow(struct um_user *user, int type, int dir)
{
    return umd_is_overflow_helper(user, type, dir)
        || umd_is_overflow_helper(user, type, um_flow_dir_all);
}

STATIC void
umd_overflow(struct um_user *user, int type, int dir)
{
    if (um_flow_type_wan==type && is_user_auth(user) && umd_is_overflow(user, type, dir)) {
        user_deauth(user, UM_DEAUTH_FLOWLIMIT);
    }

    /*
    * lan's flow include wan's flow
    */
    if (umd_is_overflow(user, um_flow_type_lan, dir)) {
        user_unbind(user, UM_DEAUTH_FLOWLIMIT);
    }
}

STATIC void
umd_flow_update(struct um_user *user, int type, int dir)
{
    if (um_flow_dir_all!=dir) {
        umd_flow_now(user, type, dir) += umd_flow.len;
    }
    umd_flow_now(user, type, um_flow_dir_all) += umd_flow.len;
    
    /*
    * lan' flow include wan's flow
    */
    if (um_flow_type_wan==type) {
        umd_flow_update(user, um_flow_type_lan, dir);
    }
}

STATIC int
umd_flow_handle(sock_server_t *server)
{
    struct um_user *user;

    user = umd_user_get(umd_flow.usermac);
    if (umd.cfg.autouser && NULL==user) {
        switch(umd.cfg.autouser) {
            case UM_AUTO_BIND:
                user = umd_user_bind(umd_flow.usermac, umd_flow.userip);
                break;
            case UM_AUTO_FAKE:
                user = umd_user_fake(umd_flow.usermac, umd_flow.userip);
                break;
        }
    }
    if (NULL==user) {
        return -ENOEXIST;
    }

    user->hitime = time(NULL);

    if (is_user_have_bind(user)) {
        umd_flow_update(user, umd_flow.type, umd_flow.dir);
        umd_user_debug("user-flow-update", user, __is_ak_debug_flow);

        umd_flow_reauth(user, umd_flow.type, umd_flow.dir);
        umd_overflow(user, umd_flow.type, umd_flow.dir);

        umd_update_aging(user, false);
    }

    return 0;
}

STATIC jobj_t
umd_jflow_st(struct um_flowst *st)
{
    jobj_t jst = jobj_new_object();

    jobj_add_i32(jst, "packets", st->packets);
    jobj_add_i64(jst, "bytes", st->bytes);

    return jst;
}

STATIC jobj_t
umd_jflow_total(int type, int valid)
{
    return umd_jflow_st(&umd_flow.total[type][valid]);
}

STATIC jobj_t
umd_jflow_dev(int type, int dir)
{
    return umd_jflow_st(&umd_flow.dev[type][dir]);
}

jobj_t
umd_jflow(void)
{
    jobj_t jflow, jtotal, jdev, jtype, jst;
    int i, j;
    
    jflow = jobj_new_object();
    
    jtotal = jobj_new_object();
    for (i=0; i<um_pkt_type_end; i++) {
        jtype = jobj_new_object();
        for (j=0; j<um_pkt_check_end; j++) {
            jst = umd_jflow_total(i, j);
            jobj_add(jtype, pkt_check_getnamebyid(j), jst);
        }
        jobj_add(jtotal, pkt_check_getnamebyid(i), jtype);
    }
    jobj_add(jflow, "total", jtotal);
    
    jdev = jobj_new_object();
    for (i=0; i<um_flow_type_end; i++) {
        jtype = jobj_new_object();
        for (j=0; j<um_flow_dir_end; j++) {
            jst = umd_jflow_dev(i, j);
            jobj_add(jtype, flow_dir_getnamebyid(j), jst);
        }
        jobj_add(jdev, flow_type_getnamebyid(i), jtype);
    }
    jobj_add(jflow, "dev", jdev);
    
    return jflow;
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

    struct um_intf *intf = umd_get_intf_by_server(server);

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
    
    debug_ok("init flow server ok");
    
    return 0;
}

STATIC int
umd_flow_server_handle_helper(sock_server_t *server)
{
    static int (*handle[])(sock_server_t *) = {
        umd_pkt_handle,
        umd_intf_handle,
        umd_eth_handle,
        umd_vlan_handle,
        umd_ip_handle,
    };
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
umd_flow_server_handle(sock_server_t *server)
{
    int i, err;

    for (i=0; i<umd.cfg.sniff_count; i++) {
        err = umd_flow_server_handle_helper(server);
        if (err<0) {
            return err;
        }
    }

    return 0;
}

sock_server_t um_flow_server = 
    SOCK_SERVER_INITER(UM_SERVER_FLOW, AF_PACKET, umd_flow_server_init, umd_flow_server_handle);
/******************************************************************************/
