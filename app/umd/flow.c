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

static struct um_flow   flow;

static inline bool
is_dev_mac(byte mac[])
{
    int i;

    for (i=0; i<umd.cfg.instance.count; i++) {
        if (os_maceq(mac, get_intf_by_id(i)->mac)) {
            return true;
        }
    }
    
    return false;
}

static inline bool
is_dev_ip(uint32 ip)
{
    int i;

    for (i=0; i<umd.cfg.instance.count; i++) {
        if (ip==get_intf_by_id(i)->ip) {
            return true;
        }
    }

    return false;
}

static inline bool
is_user_ip(uint32 ip)
{
    struct um_intf *intf = flow.intf;
    
    return (ip & intf->mask)==(intf->ip & intf->mask);
}

static inline bool
is_lan_ip(uint32 ip)
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

static inline void
add_flowst(struct um_flowst *st)
{
    st->packets++;
    st->bytes += flow.len;
}

static inline void
add_flow_total(int type, int valid)
{
    add_flowst(&flow.total[type][valid]);
}
#define add_flow_good(_type)    add_flow_total(_type, um_pkt_check_good)
#define add_flow_bad(_type)     add_flow_total(_type, um_pkt_check_bad)
#define add_flow_all(_type)     add_flow_total(_type, um_pkt_check_all)

static inline void
set_flow_dev(void)
{
    add_flowst(&flow.dev[flow.type][flow.dir]);
}

static inline void
set_flow_suser(void)
{
    flow.usermac= flow.eth->ether_shost;
    flow.userip = flow.iph->ip_src.s_addr;
}

static inline void
set_flow_duser(void)
{
    flow.usermac= flow.eth->ether_dhost;
    flow.userip = flow.iph->ip_dst.s_addr;
}

static int
pkt_handle(cli_server_t *server)
{
    int err, len;
    socklen_t addrlen = sizeof(server->addr.ll);

    err = recvfrom(server->fd, flow.packet, sizeof(flow.packet), 0, &server->addr.c, &addrlen);
    if (err<0) { /* yes, <0 */
        return err;
    }
    flow.len    = err;
    flow.usermac= NULL;
    flow.userip = 0;
    flow.intf   = get_intf_by_server(server);
    
    if (__is_ak_debug_packet) {
        os_println("recv packet length=%d", flow.len);
        
        __os_dump_buffer(flow.packet, flow.len, NULL);
    }
    
    return 0;
}

static int
intf_handle(cli_server_t *server)
{
    if (server->addr.ll.sll_ifindex == flow.intf->index) {
        return 0;
    } else {
        return -EBADIDX;
    }
}

static int
eth_handle(cli_server_t *server)
{
    struct ether_header *eth = flow.eth = (struct ether_header *)flow.packet;
    
    byte *smac = eth->ether_shost;
    byte *dmac = eth->ether_dhost;

    add_flow_all(um_pkt_type_eth);
    
    if (eth->ether_type != umd.ether_type_ip &&
        eth->ether_type != umd.ether_type_vlan) {
        /*
        * bad ether type
        */
        add_flow_bad(um_pkt_type_eth);
        
        return -EFORMAT;
    }
    else if (false==is_good_mac(smac)) {
        /*
        * bad src mac
        */
        add_flow_bad(um_pkt_type_eth);
        
        return -EFORMAT;
    }
    else if (false==is_good_mac(dmac)) {
        /*
        * bad dst mac
        */
        add_flow_bad(um_pkt_type_eth);
        
        return -EFORMAT;
    }

    add_flow_good(um_pkt_type_eth);

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

static int
vlan_handle(cli_server_t *server)
{
    if (flow.eth->ether_type == umd.ether_type_vlan) {
        struct vlan_header *vlan = flow.vlan = (struct vlan_header *)(flow.eth+1);

        add_flow_all(um_pkt_type_vlan);
        
        if (vlan->type != umd.ether_type_ip) {
            add_flow_bad(um_pkt_type_vlan);
            
            return -EFORMAT;
        }

        if (__is_ak_debug_flow) {
            debug_flow("recv packet vlan=%d type=%d", 
                vlan->vid,
                vlan->type);
        }
        
        add_flow_good(um_pkt_type_vlan);
    }
    
    return 0;
}

static int
ip_source_dev(uint32 sip, uint32 dip)
{
    if (is_dev_ip(dip)) {
        /*
        * dev==>dev, invalid
        */
        
        return -EFORMAT;
    }
    else if (is_user_ip(dip)) {
        /*
        * dev==>user
        *
        * lan down for user
        */
        flow.dir    = um_flow_dir_down;
        flow.type   = um_flow_type_lan;
        
        set_flow_duser();
    }
    else if (is_lan_ip(dip)) {
        /*
        * dev==>lan
        *
        * lan up for dev
        */
        flow.dir    = um_flow_dir_up;
        flow.type   = um_flow_type_lan;

        set_flow_dev();
    }
    else { /* dip is wan */
        /*
        * dev==>wan
        *
        * wan up for dev
        */
        flow.dir    = um_flow_dir_up;
        flow.type   = um_flow_type_wan;

        set_flow_dev();
    }

    return 0;
}

static int
ip_source_user(uint32 sip, uint32 dip, bool first)
{
    if (is_dev_ip(dip)) {
        /*
        * user==>dev
        *
        * lan up for user
        */
        flow.dir    = um_flow_dir_up;
        flow.type   = um_flow_type_lan;

        set_flow_suser();
    }
    else if (is_user_ip(dip)) {
        if (sip==dip) {
            return -EFORMAT;
        }

        if (first) {
            /*
            * {user}==>user
            *
            * first, lan up for {user}
            */
            flow.dir    = um_flow_dir_up;
            flow.type   = um_flow_type_lan;

            set_flow_suser();

            return 1; // try again
        } else {
            /*
            * user==>{user}
            *
            * again, lan down for {user}
            */
            flow.dir    = um_flow_dir_down;
            flow.type   = um_flow_type_lan;

            set_flow_duser();

            // not again
        }
    }
    else if (is_lan_ip(dip)) {
        /*
        * user==>lan
        *
        * lan up for user
        */
        flow.dir    = um_flow_dir_up;
        flow.type   = um_flow_type_lan;

        set_flow_suser();
    }
    else { /* dip is wan */
        /*
        * user==>wan
        *
        * wan up for user
        */
        flow.dir    = um_flow_dir_up;
        flow.type   = um_flow_type_wan;

        set_flow_suser();
    }

    return 0;
}

static int
ip_source_lan(uint32 sip, uint32 dip)
{
    if (is_dev_ip(dip)) {
        /*
        * lan==>dev
        *
        * lan down for dev
        */
        flow.dir    = um_flow_dir_down;
        flow.type   = um_flow_type_lan;

        set_flow_dev();
    }
    else if (is_user_ip(dip)) {
        /*
        * lan==>user
        *
        * lan down for user
        */
        flow.dir    = um_flow_dir_down;
        flow.type   = um_flow_type_lan;

        set_flow_duser();
    }
    else if (is_lan_ip(dip)) {
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

static int
ip_source_wan(uint32 sip, uint32 dip)
{
    if (is_dev_ip(dip)) {
        /*
        * wan==>dev
        *
        * wan down for dev
        */
        flow.dir    = um_flow_dir_down;
        flow.type   = um_flow_type_wan;

        set_flow_dev();
    }
    else if (is_user_ip(dip)) {
        /*
        * wan==>user
        *
        * wan down for user
        */
        flow.dir    = um_flow_dir_down;
        flow.type   = um_flow_type_wan;
        
        set_flow_duser();
    }
    else if (is_lan_ip(dip)) {
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

static int
flow_handle(cli_server_t *server);

static int
__ip_handle(cli_server_t *server, bool first)
{
    struct ip *iph;
    bool again = false;
    int err;
    
    add_flow_all(um_pkt_type_ip);

    if (flow.eth->ether_type == umd.ether_type_ip) {
        iph = flow.iph = (struct ip *)(flow.eth+1);
    }
    else if (flow.eth->ether_type == umd.ether_type_vlan) {
        iph = flow.iph = (struct ip *)(flow.vlan+1);
    }
    else {
        if (first) {
            add_flow_bad(um_pkt_type_ip);
        }
        
        return -EFORMAT;
    }

    uint32 sip = iph->ip_src.s_addr;
    uint32 dip = iph->ip_dst.s_addr;

    if (4!=iph->ip_v) {
        if (first) {
            add_flow_bad(um_pkt_type_ip);
        }
        
        return -EFORMAT;
    }
    else if (5!=iph->ip_hl) {
        if (first) {
            add_flow_bad(um_pkt_type_ip);
        }
        
        return -EFORMAT;
    }

    if (is_dev_ip(sip)) {
        err = ip_source_dev(sip, dip);
    }
    else if (is_user_ip(sip)) {
        err = ip_source_user(sip, dip, first);
        again = (1==err);
    }
    else if (is_lan_ip(sip)) {
        err = ip_source_lan(sip, dip);
    }
    else { /* sip is wan */
        err = ip_source_wan(sip, dip);
    }
    if (err<0) {
        if (first) {
            add_flow_bad(um_pkt_type_ip);
        }
        
        return err;
    }

    if (first) {
        add_flow_good(um_pkt_type_ip);
    }
    
    if (__is_ak_debug_flow) {
        char sipstring[1+OS_IPSTRINGLEN];
        char dipstring[1+OS_IPSTRINGLEN];
        char  ipstring[1+OS_IPSTRINGLEN];
        char macstring[1+MACSTRINGLEN_L];
        
        os_strcpy(sipstring, os_ipstring(iph->ip_src.s_addr));
        os_strcpy(dipstring, os_ipstring(iph->ip_dst.s_addr));
        os_strcpy( ipstring, os_ipstring(flow.userip));
        os_strcpy( macstring, os_macstring(flow.usermac));
        
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
            flow_dir_string(flow.dir),
            flow_type_string(flow.type),
            ipstring,
            macstring);
    }

    if (NULL==flow.usermac || 0==flow.userip) {
        return -ENOSUPPORT;
    }
    
    flow_handle(server);

    return again?1:0;
}

static int
ip_handle(cli_server_t *server)
{
    int err = __ip_handle(server, true);
    bool again = (1==err);
    
    if (1==err) {
        err = __ip_handle(server, false);
    }
    
    return err;
}

static inline bool
__is_flow_reauth(struct um_user *user, int type, int dir)
{
    uint64 reauth = __flow_reauth(user, type, dir);
    uint64 now    = __flow_now(user, type, dir);
    
    bool is = reauth && reauth < now;
    if (is) {
        debug_flow("user(%s) type(%s) dir(%s) flow reauth(%llu) > now(%llu)",
            os_macstring(user->mac),
            flow_type_string(type),
            flow_dir_string(dir),
            reauth,
            now);
    }
    
    return is;
}

static inline bool
is_flow_reauth(struct um_user *user, int type, int dir)
{
    return __is_flow_reauth(user, type, dir)
        || __is_flow_reauth(user, type, um_flow_dir_all);
}

static void
flow_reauth(struct um_user *user, int type, int dir)
{
    if (umd.cfg.reauthable
            && um_flow_type_wan==type
            && is_user_auth(user)
            && is_flow_reauth(user, type, dir)) {
        user_reauth(user);
    }
}

static inline bool
__is_overflow(struct um_user *user, int type, int dir)
{
    uint64 max    = __flow_max(user, type, dir);
    uint64 now    = __flow_now(user, type, dir);
    
    bool is = max && max < now;
    if (is) {
        debug_flow("user(%s) type(%s) dir(%s) flow max(%llu) > now(%llu)",
            os_macstring(user->mac),
            flow_type_string(type),
            flow_dir_string(dir),
            max,
            now);
    }

    return is;
}

static inline bool
is_overflow(struct um_user *user, int type, int dir)
{
    return __is_overflow(user, type, dir)
        || __is_overflow(user, type, um_flow_dir_all);
}

static void
overflow(struct um_user *user, int type, int dir)
{
    if (um_flow_type_wan==type && is_user_auth(user) && is_overflow(user, type, dir)) {
        user_deauth(user, UM_DEAUTH_FLOWLIMIT);
    }

    /*
    * lan's flow include wan's flow
    */
    if (is_overflow(user, um_flow_type_lan, dir)) {
        user_unbind(user, UM_DEAUTH_FLOWLIMIT);
    }
}

static void
flow_update(struct um_user *user, int type, int dir)
{
    if (um_flow_dir_all!=dir) {
        __flow_now(user, type, dir) += flow.len;
    }
    __flow_now(user, type, um_flow_dir_all) += flow.len;
    
    /*
    * lan' flow include wan's flow
    */
    if (um_flow_type_wan==type) {
        flow_update(user, um_flow_type_lan, dir);
    }
}

static int
flow_handle(cli_server_t *server)
{
    struct um_user *user;

    user = um_user_get(flow.usermac);
    if (umd.cfg.autouser && NULL==user) {
        switch(umd.cfg.autouser) {
            case UM_AUTO_BIND:
                user = um_user_bind(flow.usermac, flow.userip);
                break;
            case UM_AUTO_FAKE:
                user = um_user_fake(flow.usermac, flow.userip);
                break;
        }
    }
    if (NULL==user) {
        return -ENOEXIST;
    }

    user->hitime = time(NULL);

    if (is_user_have_bind(user)) {
        flow_update(user, flow.type, flow.dir);
        um_user_debug("user-flow-update", user, __is_ak_debug_flow);

        flow_reauth(user, flow.type, flow.dir);
        overflow(user, flow.type, flow.dir);

        update_aging(user, false);
    }

    return 0;
}

static jobj_t
__um_jflow_st(struct um_flowst *st)
{
    jobj_t jst = jobj_new_object();

    jobj_add_i32(jst, "packets", st->packets);
    jobj_add_i64(jst, "bytes", st->bytes);

    return jst;
}

static jobj_t
um_jflow_total(int type, int valid)
{
    return __um_jflow_st(&flow.total[type][valid]);
}

static jobj_t
um_jflow_dev(int type, int dir)
{
    return __um_jflow_st(&flow.dev[type][dir]);
}

jobj_t
um_jflow(void)
{
    jobj_t jflow, jtotal, jdev, jtype, jst;
    int i, j;
    
    jflow = jobj_new_object();
    
    jtotal = jobj_new_object();
    for (i=0; i<um_pkt_type_end; i++) {
        jtype = jobj_new_object();
        for (j=0; j<um_pkt_check_end; j++) {
            jst = um_jflow_total(i, j);
            jobj_add(jtype, pkt_check_string(j), jst);
        }
        jobj_add(jtotal, pkt_type_string(i), jtype);
    }
    jobj_add(jflow, "total", jtotal);
    
    jdev = jobj_new_object();
    for (i=0; i<um_flow_type_end; i++) {
        jtype = jobj_new_object();
        for (j=0; j<um_flow_dir_end; j++) {
            jst = um_jflow_dev(i, j);
            jobj_add(jtype, flow_dir_string(j), jst);
        }
        jobj_add(jdev, flow_type_string(i), jtype);
    }
    jobj_add(jflow, "dev", jdev);
    
    return jflow;
}

static int
flow_server_init(cli_server_t *server)
{
    int fd, err;
    
    fd = socket(AF_PACKET, SOCK_RAW, umd.ether_type_all);
    if (fd<0) {
        debug_error("socket error:%d", -errno);

        return -errno;
    }
    
    fcntl(fd, F_SETFL, O_NONBLOCK);
    
    struct um_intf *intf = get_intf_by_server(server);

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

static int
__flow_server_handle(cli_server_t *server)
{
    static int (*handle[])(cli_server_t *) = {
        pkt_handle,
        intf_handle,
        eth_handle,
        vlan_handle,
        ip_handle,
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

static int
flow_server_handle(cli_server_t *server)
{
    int i, err;

    for (i=0; i<umd.cfg.sniff_count; i++) {
        err = __flow_server_handle(server);
        if (err<0) {
            return err;
        }
    }

    return 0;
}

cli_server_t um_flow_server = 
    CLI_SERVER_INITER(UM_SERVER_FLOW, AF_PACKET, flow_server_init, flow_server_handle);
/******************************************************************************/
