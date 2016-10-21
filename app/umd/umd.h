#ifndef __UM_H_c4e41de0b2154a2aa5e5b4c8fd42dc23__
#define __UM_H_c4e41de0b2154a2aa5e5b4c8fd42dc23__
/******************************************************************************/
#include "um/um.h"

#ifndef UMD_USE_AUTOUSER
#define UMD_USE_AUTOUSER        2
#endif

#ifndef UMD_USE_PROMISC
#define UMD_USE_PROMISC         0
#endif

#ifndef UMD_MACHASHSIZE
#define UMD_MACHASHSIZE         256
#endif

#ifndef UMD_IPHASHSIZE
#define UMD_IPHASHSIZE          256
#endif

#ifndef UMD_CONF
#ifdef __PC__
#   define UMD_CONF             "./umd.conf"
#else
#   define UMD_CONF             "/tmp/config/umd.conf"
#endif
#endif

#ifndef UMD_SCRIPT_EVENT
#ifdef __PC__
#   define UMD_SCRIPT_EVENT     "./umevent"
#else
#   define UMD_SCRIPT_EVENT     "/etc/um/umevent"
#endif
#endif

#ifndef UMD_SCRIPT_IP
#ifdef __PC__
#   define UMD_SCRIPT_IP        "./getipbymac"
#else
#   define UMD_SCRIPT_IP        "/usr/sbin/getipbymac"
#endif
#endif

#ifndef UMD_SCRIPT_MAC
#ifdef __PC__
#   define UMD_SCRIPT_MAC       "./getmacbyip"
#else
#   define UMD_SCRIPT_MAC       "/usr/sbin/getmacbyip"
#endif
#endif

#ifndef UMD_TICKS
#ifdef __PC__
#   define UMD_TICKS            5  /* second */
#else
#   define UMD_TICKS            10 /* second */
#endif
#endif

#ifndef UMD_IDLE
#ifdef __PC__
#   define UMD_IDLE             60  /* second */
#else
#   define UMD_IDLE             300 /* second */
#endif
#endif

#ifndef UMD_FAKE
#ifdef __PC__
#   define UMD_FAKE             10  /* second */
#else
#   define UMD_FAKE             30  /* second */
#endif
#endif

#ifndef UMD_GC
#ifdef __PC__
#   define UMD_GC               30  /* second */
#else
#   define UMD_GC               0   /* second */
#endif
#endif

#ifndef UMD_SNIFF_COUNT
#ifdef __PC__
#    define UMD_SNIFF_COUNT     128
#else
#    define UMD_SNIFF_COUNT     32
#endif
#endif

#ifndef UMD_IFNAME_INGRESS
#ifdef __PC__
#    define UMD_IFNAME_INGRESS  "eth0"
#else
#    define UMD_IFNAME_INGRESS  "lan0"
#endif
#endif

#if 1
#define __XLIST_UM_AUTO(_)         \
    _(UM_AUTO_NONE, 0, "none"),    \
    _(UM_AUTO_BIND, 1, "bind"),    \
    _(UM_AUTO_FAKE, 2, "fake"),    \
    /* end */

static inline bool is_good_user_auto(int id);
static inline char *user_auto_string(int id);
static inline int user_auto_idx(char *name);
DECLARE_ENUM(user_auto, __XLIST_UM_AUTO, UM_AUTO_END);

#define UM_AUTO_NONE   UM_AUTO_NONE
#define UM_AUTO_BIND   UM_AUTO_BIND
#define UM_AUTO_FAKE   UM_AUTO_FAKE
#define UM_AUTO_END    UM_AUTO_END
#endif

#if 1
#define __XLIST_UM_STATE(_)         \
    _(UM_STATE_NONE, 0, "none"),    \
    _(UM_STATE_BIND, 1, "bind"),    \
    _(UM_STATE_FAKE, 2, "fake"),    \
    _(UM_STATE_AUTH, 3, "auth"),    \
    /* end */

static inline bool is_good_user_state(int id);
static inline char *user_state_string(int id);
static inline int user_state_idx(char *name);
DECLARE_ENUM(user_state, __XLIST_UM_STATE, UM_STATE_END);

#define UM_STATE_NONE   UM_STATE_NONE
#define UM_STATE_BIND   UM_STATE_BIND
#define UM_STATE_FAKE   UM_STATE_FAKE
#define UM_STATE_AUTH   UM_STATE_AUTH
#define UM_STATE_END    UM_STATE_END
#endif

#define __is_none(_state)   (UM_STATE_NONE==(_state))
#define __is_bind(_state)   (UM_STATE_BIND==(_state))
#define __is_fake(_state)   (UM_STATE_FAKE==(_state))
#define __is_auth(_state)   (UM_STATE_AUTH==(_state))
#define __have_auth(_state) (__is_fake(_state) || __is_auth(_state))
#define __have_bind(_state) (__is_bind(_state) || __have_auth(_state))

#define is_none(_user)      __is_none((_user)->state)
#define is_bind(_user)      __is_bind((_user)->state)
#define is_fake(_user)      __is_fake((_user)->state)
#define is_auth(_user)      __is_auth((_user)->state)
#define is_noused(_user)    (__is_none((_user)->state) && 0==(_user)->flags)
#define have_auth(_user)    __have_auth((_user)->state)
#define have_bind(_user)    __have_bind((_user)->state)

#if 1
#define __XLIST_UM_DEAUTH(_)                    \
    _(UM_DEAUTH_NONE,       0, "none"),         \
    _(UM_DEAUTH_AUTO,       1, "auto"),         \
    _(UM_DEAUTH_ONLINETIME, 2, "onlinetime"),   \
    _(UM_DEAUTH_FLOWLIMIT,  3, "flowlimit"),    \
    _(UM_DEAUTH_ADMIN,      4, "admin"),        \
    _(UM_DEAUTH_AGING,      5, "aging"),        \
    _(UM_DEAUTH_INITIATIVE, 6, "initiative"),   \
    /* end */

static inline bool is_good_deauth_reason(int id);
static inline char *deauth_reason_string(int id);
static inline int deauth_reason_idx(char *name);
DECLARE_ENUM(deauth_reason, __XLIST_UM_DEAUTH, UM_DEAUTH_END);

#define UM_DEAUTH_NONE          UM_DEAUTH_NONE
#define UM_DEAUTH_AUTO          UM_DEAUTH_AUTO
#define UM_DEAUTH_ONLINETIME    UM_DEAUTH_ONLINETIME
#define UM_DEAUTH_FLOWLIMIT     UM_DEAUTH_FLOWLIMIT
#define UM_DEAUTH_ADMIN         UM_DEAUTH_ADMIN
#define UM_DEAUTH_AGING         UM_DEAUTH_AGING
#define UM_DEAUTH_INITIATIVE    UM_DEAUTH_INITIATIVE
#define UM_DEAUTH_END           UM_DEAUTH_END
#endif

static inline bool
is_valid_deauth_reason(int reason)
{
    return IS_GOOD_VALUE(reason, UM_DEAUTH_NONE+1, UM_DEAUTH_END);
}

#if 1
#define __XLIST_UM_FLOW_TYPE(_)     \
    _(um_flow_type_lan, 0, "lan"),  \
    _(um_flow_type_wan, 1, "wan"),  \
    /* end */

static inline bool is_good_flow_type(int id);
static inline char *flow_type_string(int id);
static inline int flow_type_idx(char *name);
DECLARE_ENUM(flow_type, __XLIST_UM_FLOW_TYPE, um_flow_type_end);

#define um_flow_type_lan    um_flow_type_lan
#define um_flow_type_wan    um_flow_type_wan
#define um_flow_type_end    um_flow_type_end
#endif

#if 1
#define __XLIST_UM_FLOW_DIR(_)      \
    _(um_flow_dir_up,   0, "up"),   \
    _(um_flow_dir_down, 1, "down"), \
    _(um_flow_dir_all,  2, "all"),  \
    /* end */

static inline bool is_good_flow_dir(int id);
static inline char *flow_dir_string(int id);
static inline int flow_dir_idx(char *name);
DECLARE_ENUM(flow_dir, __XLIST_UM_FLOW_DIR, um_flow_dir_end);

#define um_flow_dir_up      um_flow_dir_up
#define um_flow_dir_down    um_flow_dir_down
#define um_flow_dir_all     um_flow_dir_all
#define um_flow_dir_end     um_flow_dir_end
#endif

struct um_limit_online {
    uint32 max;         /* config */
    uint32 idle;        /* config */
    uint32 numerator;   /* config */
    uint32 denominator; /* config */

    int aging;          /* run data */
    time_t uptime;      /* run data */
    time_t downtime;    /* run data */
};

struct um_limit_flow {
    uint64 max;         /* config */
    uint64 now;         /* run data */
    
    uint64 numerator;   /* config */
    uint64 denominator; /* config */
};

struct um_limit_rate {
    uint32 max;   /* config */
    uint32 avg;   /* config */
};

struct um_limit {
    struct um_limit_online online;
    struct um_limit_flow flow[um_flow_dir_end];
    struct um_limit_rate rate[um_flow_dir_end];
};

struct um_tag {
    struct list_head tag;

    char *k;
    char *v;
};

#define UM_F_MONITOR    0x01
#define UM_F_SYNC       0x02

struct um_user {
    byte mac[OS_MACSIZE];
    byte bssid[OS_MACSIZE];

    char *ssid;
    
    int state;
    int reason;
    int group;

    uint32 flags;
    uint32 ip; /* network */

    time_t create;
    time_t faketime;
    time_t noused;

    struct um_limit limit[um_flow_type_end];

    h2_node_t node;
    
    struct {
        struct list_head tag;
    } head;
};

static inline struct um_limit *
__limit(struct um_user *user, int type)
{
    return &user->limit[type];
}

static inline struct um_limit_online *
__limit_online(struct um_user *user, int type)
{
    return &__limit(user, type)->online;
}

static inline struct um_limit_flow *
__limit_flow(struct um_user *user, int type, int dir)
{
    return &__limit(user, type)->flow[dir];
}

static inline struct um_limit_rate *
__limit_rate(struct um_user *user, int type, int dir)
{
    return &__limit(user, type)->rate[dir];
}

#define __limit_reauth(_max, _numerator, _denominator) ({ \
    typeof(_max) _m_in___limit_reauth = (_max);                   \
    typeof(_numerator) _n_in___limit_reauth = (_numerator);       \
    typeof(_denominator) _d_in___limit_reauth = (_denominator);   \
                                                \
    (_m_in___limit_reauth && _n_in___limit_reauth)? \
        (_d_in___limit_reauth ? \
            _m_in___limit_reauth*_n_in___limit_reauth/_d_in___limit_reauth: \
            _n_in___limit_reauth ): \
        0; \
})

#define __online_max(_user, _type)          __limit_online(_user, _type)->max
#define __online_idle(_user, _type)         __limit_online(_user, _type)->idle
#define __online_aging(_user, _type)        __limit_online(_user, _type)->aging
#define __online_uptime(_user, _type)       __limit_online(_user, _type)->uptime
#define __online_downtime(_user, _type)     __limit_online(_user, _type)->downtime
#define __online_numerator(_user, _type)    __limit_online(_user, _type)->numerator
#define __online_denominator(_user, _type)  __limit_online(_user, _type)->denominator
#define __online_reauth(_user, _type)       \
    __limit_reauth(__online_max(_user, _type), __online_numerator(_user, _type), __online_denominator(_user, _type))

#define __flow_max(_user, _type, _dir)          __limit_flow(_user, _type, _dir)->max
#define __flow_now(_user, _type, _dir)          __limit_flow(_user, _type, _dir)->now
#define __flow_numerator(_user, _type, _dir)    __limit_flow(_user, _type, _dir)->numerator
#define __flow_denominator(_user, _type, _dir)  __limit_flow(_user, _type, _dir)->denominator
#define __flow_reauth(_user, _type, _dir)       \
    __limit_reauth(__flow_max(_user, _type, _dir), __flow_numerator(_user, _type, _dir), __flow_denominator(_user, _type, _dir))

#define __rate_max(_user, _type, _dir)          __limit_rate(_user, _type, _dir)->max
#define __rate_avg(_user, _type, _dir)          __limit_rate(_user, _type, _dir)->avg

#define limit_online(_user, _TYPE)          __limit_online(_user, um_flow_type_##_TYPE)
#define limit_flow(_user, _TYPE, _DIR)      __limit_flow(_user, um_flow_type_##_TYPE, um_flow_dir_##_DIR)
#define limit_rate(_user, _TYPE, _DIR)      __limit_rate(_user, um_flow_type_##_TYPE, um_flow_dir_##_DIR)

static inline void
__update_aging(struct um_user *user, int type, bool debug)
{
    __online_aging(user, type) = __online_idle(user, type);

    if (debug) {
        debug_aging("update %s aging to %d",
            flow_type_string(type),
            __online_aging(user, type));
    }
}

static inline void
update_aging(struct um_user *user, bool debug)
{
    __update_aging(user, um_flow_type_wan, debug);
    __update_aging(user, um_flow_type_lan, debug);
}

typedef mv_t um_foreach_f(struct um_user *user);
typedef mv_t um_get_f(struct um_user *user);

enum {
    UM_SERVER_TIMER,    /* must first */
    UM_SERVER_CLI,
    
    UM_SERVER_FLOW,
    UM_SERVER_END = UM_SERVER_FLOW
};

#define um_intf_id(_server_id)  (_server_id - UM_SERVER_END)
#define um_server_id(_intf_id)  (_intf_id + UM_SERVER_END)

struct um_intf {
    char name[1+OS_IFNAME_LEN];
    byte mac[OS_MACSIZE], __r0[2];
    uint32 id;
    
    uint32 index;
    uint32 ip;
    uint32 mask;
    uint32 flag;
};

#if 1
#define __XLIST_UM_FORWARD_MODE(_)      \
    _(um_forward_mode_br,   0, "br"),   \
    _(um_forward_mode_rt,   1, "rt"),   \
    /* end */

static inline bool is_good_forward_mode(int id);
static inline char *forward_mode_string(int id);
static inline int forward_mode_idx(char *name);
DECLARE_ENUM(forward_mode, __XLIST_UM_FORWARD_MODE, um_forward_mode_end);

#define um_forward_mode_br  um_forward_mode_br
#define um_forward_mode_rt  um_forward_mode_rt
#define um_forward_mode_end um_forward_mode_end
#endif

struct um_config {
    struct {
        int count;
        struct um_intf *intf;
    } instance;
    
    char *conf;
    char *script_event;
    char *script_getmacbyip;
    char *script_getipbymac;
    
    int autouser;
    uint32 gc;
    uint32 sniff_count;
    uint32 ticks;
    uint32 idle;
    uint32 fake;
    uint32 machashsize;
    uint32 iphashsize;
};

#define UMD_CFG_INITER                  {   \
    .autouser = UMD_USE_AUTOUSER,           \
    .gc = UMD_GC,                           \
    .sniff_count = UMD_SNIFF_COUNT,         \
    .ticks = UMD_TICKS,                     \
    .idle = UMD_IDLE,                       \
    .fake = UMD_FAKE,                       \
    .machashsize = UMD_MACHASHSIZE,         \
    .iphashsize = UMD_IPHASHSIZE,           \
                                            \
    .conf = UMD_CONF,                       \
    .script_event = UMD_SCRIPT_EVENT,       \
    .script_getipbymac = UMD_SCRIPT_IP,     \
    .script_getmacbyip = UMD_SCRIPT_MAC,    \
}   /* end */

#define UM_LAN_COUNT    3

struct um_lan {
    uint32 ip;
    uint32 mask;
    char *ipstring;
    char *maskstring;
};
#define __UMD_LAN_INITER(_ipstring, _maskstring) { \
    .ipstring   = _ipstring,    \
    .maskstring = _maskstring,  \
}   /* end */
#define UMD_LAN_INITER { \
    __UMD_LAN_INITER("192.168.0.0", "255.255.255.0"), \
    __UMD_LAN_INITER("172.16.0.0", "255.240.0.0"),    \
    __UMD_LAN_INITER("10.0.0.0", "255.0.0.0"),        \
}   /* end */

enum {
    UM_USER_NIDX_MAC,
    UM_USER_NIDX_IP,

    UM_USER_NIDX_END
};

struct um_control {
    byte basemac[OS_MACSIZE]; /* local ap's base mac */
    uint16 ether_type_ip;   /* network sort */
    uint16 ether_type_vlan; /* network sort */
    uint16 ether_type_all;  /* network sort */
    
    uint32 ticks;
    bool deinit;

    struct um_lan lan[UM_LAN_COUNT];
    struct um_config cfg;
    cli_server_t **server;
    int server_count;
    h2_table_t table;
};

#define UMD_INITER      {   \
    .lan = UMD_LAN_INITER,  \
    .cfg = UMD_CFG_INITER,  \
}   /* end */

extern struct um_control umd;

static inline cli_server_t *
get_server_by_intf(struct um_intf *intf)
{
    return umd.server[um_server_id(intf->id)];
}

static inline struct um_intf *
get_intf_by_id(int intf_id)
{
    return &umd.cfg.instance.intf[intf_id];
}

static inline struct um_intf *
get_intf_by_server(cli_server_t *server)
{
    return get_intf_by_id(um_intf_id(server->id));
}

struct um_user_filter {
    int state;
    
    byte mac[OS_MACSIZE];
    byte macmask[OS_MACSIZE]; /* zero, not use mac as filter */
    
    uint32 ip; /* network */
    uint32 ipmask;/* network. zero, not use ip as filter */
};

struct vlan_header {
#if __BYTE_ORDER == __BIG_ENDIAN
    uint16 pri:3;
    uint16 cfi:1;
    uint16 vid:12;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
    uint16 vid:12;
    uint16 cfi:1;
    uint16 pri:3;
#endif

    uint16 type;
};

#if 1
#define __XLIST_UM_PKT_TYPE(_)  \
    _(um_pkt_type_eth,  0, "eth"),  \
    _(um_pkt_type_vlan, 1, "vlan"), \
    _(um_pkt_type_ip,   2, "ip"),   \
    /* end */

static inline bool is_good_pkt_type(int id);
static inline char *pkt_type_string(int id);
static inline int pkt_type_idx(char *name);
DECLARE_ENUM(pkt_type, __XLIST_UM_PKT_TYPE, um_pkt_type_end);

#define um_pkt_type_eth     um_pkt_type_eth
#define um_pkt_type_vlan    um_pkt_type_vlan
#define um_pkt_type_ip      um_pkt_type_ip
#define um_pkt_type_end     um_pkt_type_end
#endif

#if 1
#define __XLIST_UM_PKT_CHECK(_)  \
    _(um_pkt_check_good,    0, "good"), \
    _(um_pkt_check_bad,     1, "bad"),  \
    _(um_pkt_check_all,     2, "all"),  \
    /* end */

static inline bool is_good_pkt_check(int id);
static inline char *pkt_check_string(int id);
static inline int pkt_check_idx(char *name);
DECLARE_ENUM(pkt_check, __XLIST_UM_PKT_CHECK, um_pkt_check_end);

#define um_pkt_check_good   um_pkt_check_good
#define um_pkt_check_bad    um_pkt_check_bad
#define um_pkt_check_all    um_pkt_check_all
#define um_pkt_check_end    um_pkt_check_end
#endif

struct um_flowst {
    uint32 packets;
    uint64 bytes;
};

struct um_flow {
    struct um_intf *intf;
    
    byte packet[2048];
    int len;

    struct ether_header *eth;
    struct vlan_header  *vlan;
    struct ip           *iph;

    uint32 userip; /* network sort */
    uint32 sip;
    uint32 dip;
    
    byte *usermac;
    byte *smac;
    byte *dmac;
    
    int type;   /* um_flow_type_lan/um_flow_type_wan */
    int dir;    /* um_flow_dir_up/um_flow_dir_down/um_flow_dir_all */

    struct um_flowst    total[um_pkt_type_end][um_pkt_check_end],
                        dev[um_flow_type_end][um_flow_dir_end];
};

/******************************************************************************/
extern jobj_t
um_juser(struct um_user *user);

extern struct um_user *
um_touser(struct um_user *user, jobj_t obj);

static inline void
um_user_dump(char *tag, struct um_user *user)
{
    jobj_t obj = um_juser(user);

    os_println("\t%s:%s", tag, jobj_json(obj));

    jobj_put(obj);
}

static inline void
um_user_debug(char *tag, struct um_user *user, bool debug)
{
    if (debug) {
        jobj_t obj = um_juser(user);
        
        jdebug("%o", tag, obj);

        jobj_put(obj);
    }
}

extern jobj_t
um_jflow(void);

extern int
user_delete(struct um_user *user);

extern int
user_unbind(struct um_user *user, int reason);

extern int
user_unfake(struct um_user *user, int reason);

extern int
user_reauth(struct um_user *user);

extern int
user_deauth(struct um_user *user, int reason);

extern struct um_tag *
um_tag_get(byte mac[], char *k);

extern struct um_tag *
um_tag_set(byte mac[], char *k, char *v);

extern int
um_user_delete(byte mac[]);

extern struct um_user *
um_user_create(byte mac[]);

extern struct um_user *
um_user_sync(byte mac[], jobj_t obj);

extern struct um_user *
um_user_bind(byte mac[], uint32 ip);

extern int
um_user_unbind(byte mac[]);

extern struct um_user *
um_user_fake(byte mac[], uint32 ip);

extern int
um_user_unfake(byte mac[]);

extern struct um_user *
um_user_auth(byte mac[], int group, jobj_t obj);

extern int
um_user_reauth(byte mac[]);

extern int
um_user_deauth(byte mac[], int reason);

extern int
um_user_foreach(um_foreach_f *foreach, bool safe);

extern struct um_user *
um_user_get(byte mac[]);

extern struct um_user *
um_user_getbyip(uint32 ip);

extern int
um_user_getby(struct um_user_filter *filter, um_get_f *get);

extern int
um_user_delbyip(uint32 ip);

extern int
um_user_delby(struct um_user_filter *filter);

#define UM_TEST_JSON    0x01

#ifndef UM_TEST
#ifdef __PC__
#   define UM_TEST      UM_TEST_JSON
#else
#   define UM_TEST      0
#endif
#endif

extern void
update_limit_test(void);

extern mv_t 
umd_gc(struct um_user *user);

extern mv_t 
umd_gc_auto(struct um_user *user, time_t now);
/******************************************************************************/
#endif /* __UM_H_c4e41de0b2154a2aa5e5b4c8fd42dc23__ */
