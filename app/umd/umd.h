#ifndef __UM_H_c4e41de0b2154a2aa5e5b4c8fd42dc23__
#define __UM_H_c4e41de0b2154a2aa5e5b4c8fd42dc23__
/******************************************************************************/
#include "um/um.h"

#ifndef UMD_MACHASHSIZE
#define UMD_MACHASHSIZE         256
#endif

#ifndef UMD_IPHASHSIZE
#define UMD_IPHASHSIZE          256
#endif

#ifndef UMD_SYNCABLE
#define UMD_SYNCABLE            true
#endif

#ifndef UMD_REAUTHABLE
#define UMD_REAUTHABLE          true
#endif

#ifndef UMD_FAKEABLE
#define UMD_FAKEABLE            true
#endif

#ifndef UMD_CONF
#define UMD_CONF                PC_FILE("/tmp/config", "umd.conf")
#endif

#ifndef UMD_SCRIPT_EVENT
#define UMD_SCRIPT_EVENT        PC_FILE("/etc/um", "umevent")
#endif

#ifndef UMD_SCRIPT_IP
#define UMD_SCRIPT_IP           PC_FILE("/usr/sbin", "getipbymac")
#endif

#ifndef UMD_SCRIPT_MAC
#define UMD_SCRIPT_MAC          PC_FILE("/usr/sbin", "getmacbyip")
#endif

#ifndef UMD_TICKS
#define UMD_TICKS               PC_VAL(5, 10)   /* second */
#endif

#ifndef UMD_IDLE
#define UMD_IDLE                PC_VAL(60, 300) /* second */
#endif

#ifndef UMD_FAKE
#define UMD_FAKE                PC_VAL(10, 30)  /* second */
#endif

#ifndef UMD_GC
#define UMD_GC                  PC_VAL(30, 0)   /* second */
#endif

#ifndef UMD_SNIFF_COUNT
#define UMD_SNIFF_COUNT         PC_VAL(128, 32)
#endif

#ifndef UMD_IFNAME_INGRESS
#define UMD_IFNAME_INGRESS      PC_VAL("eth0", "lan0")
#endif

#if 1
#define UMD_AUTO_ENUM_MAPPER(_)     \
    _(UMD_AUTO_NONE, 0, "none"),    \
    _(UMD_AUTO_BIND, 1, "bind"),    \
    _(UMD_AUTO_FAKE, 2, "fake"),    \
    /* end */
DECLARE_ENUM(umd_user_auto, UMD_AUTO_ENUM_MAPPER, UMD_AUTO_END);

static inline enum_ops_t *umd_user_auto_ops(void);
static inline bool is_good_umd_user_auto(int id);
static inline char *umd_user_auto_getnamebyid(int id);
static inline int umd_user_auto_getidbyname(const char *name);

#define UMD_AUTO_NONE   UMD_AUTO_NONE
#define UMD_AUTO_BIND   UMD_AUTO_BIND
#define UMD_AUTO_FAKE   UMD_AUTO_FAKE
#define UMD_AUTO_END    UMD_AUTO_END
#endif

#ifndef UMD_AUTOUSER
#define UMD_AUTOUSER    UMD_AUTO_FAKE
#endif

#if 1
#define UMD_STATE_ENUM_MAPPER(_)    \
    _(UMD_STATE_NONE, 0, "none"),   \
    _(UMD_STATE_BIND, 1, "bind"),   \
    _(UMD_STATE_FAKE, 2, "fake"),   \
    _(UMD_STATE_AUTH, 3, "auth"),   \
    _(UMD_STATE_BLOCK,4, "block"),  \
    /* end */
DECLARE_ENUM(umd_user_state, UMD_STATE_ENUM_MAPPER, UMD_STATE_END);

static inline enum_ops_t *umd_user_state_ops(void);
static inline bool is_good_umd_user_state(int id);
static inline char *umd_user_state_getnamebyid(int id);
static inline int umd_user_state_getidbyname(const char *name);

#define UMD_STATE_NONE  UMD_STATE_NONE
#define UMD_STATE_BIND  UMD_STATE_BIND
#define UMD_STATE_FAKE  UMD_STATE_FAKE
#define UMD_STATE_AUTH  UMD_STATE_AUTH
#define UMD_STATE_BLOCK UMD_STATE_BLOCK
#define UMD_STATE_END   UMD_STATE_END
#endif

#define __is_user_none(_state)      (UMD_STATE_NONE==(_state))
#define __is_user_bind(_state)      (UMD_STATE_BIND==(_state))
#define __is_user_fake(_state)      (UMD_STATE_FAKE==(_state))
#define __is_user_auth(_state)      (UMD_STATE_AUTH==(_state))
#define __is_user_block(_state)     (UMD_STATE_BLOCK==(_state))
#define __is_user_have_auth(_state) (__is_user_fake(_state) || __is_user_auth(_state))
#define __is_user_have_bind(_state) (__is_user_bind(_state) || __is_user_have_auth(_state))

#define is_user_none(_user)         __is_user_none((_user)->state)
#define is_user_bind(_user)         __is_user_bind((_user)->state)
#define is_user_fake(_user)         __is_user_fake((_user)->state)
#define is_user_auth(_user)         __is_user_auth((_user)->state)
#define is_user_block(_user)        __is_user_block((_user)->state)
#define is_user_noused(_user)       (__is_user_none((_user)->state) && 0==(_user)->flags)
#define is_user_have_auth(_user)    __is_user_have_auth((_user)->state)
#define is_user_have_bind(_user)    __is_user_have_bind((_user)->state)

#if 1
#define UMD_DEAUTH_ENUM_MAPPER(_)                \
    _(UMD_DEAUTH_NONE,       0, "none"),         \
    _(UMD_DEAUTH_AUTO,       1, "auto"),         \
    _(UMD_DEAUTH_ONLINETIME, 2, "onlinetime"),   \
    _(UMD_DEAUTH_FLOWLIMIT,  3, "flowlimit"),    \
    _(UMD_DEAUTH_ADMIN,      4, "admin"),        \
    _(UMD_DEAUTH_AGING,      5, "aging"),        \
    _(UMD_DEAUTH_INITIATIVE, 6, "initiative"),   \
    _(UMD_DEAUTH_BLOCK,      7, "block"),        \
    /* end */
DECLARE_ENUM(umd_deauth_reason, UMD_DEAUTH_ENUM_MAPPER, UMD_DEAUTH_END);

static inline enum_ops_t *umd_deauth_reason_ops(void);
static inline bool is_good_umd_deauth_reason(int id);
static inline char *umd_deauth_reason_getnamebyid(int id);
static inline int umd_deauth_reason_getidbyname(const char *name);

#define UMD_DEAUTH_NONE         UMD_DEAUTH_NONE
#define UMD_DEAUTH_AUTO         UMD_DEAUTH_AUTO
#define UMD_DEAUTH_ONLINETIME   UMD_DEAUTH_ONLINETIME
#define UMD_DEAUTH_FLOWLIMIT    UMD_DEAUTH_FLOWLIMIT
#define UMD_DEAUTH_ADMIN        UMD_DEAUTH_ADMIN
#define UMD_DEAUTH_AGING        UMD_DEAUTH_AGING
#define UMD_DEAUTH_INITIATIVE   UMD_DEAUTH_INITIATIVE
#define UMD_DEAUTH_BLOCK        UMD_DEAUTH_BLOCK
#define UMD_DEAUTH_END          UMD_DEAUTH_END
#endif

static inline bool
is_valid_umd_deauth_reason(int reason)
{
    return IS_GOOD_VALUE(reason, UMD_DEAUTH_NONE+1, UMD_DEAUTH_END);
}

#if 1
#define UMD_FLOW_TYPE_ENUM_MAPPER(_) \
    _(umd_flow_type_lan, 0, "lan"), \
    _(umd_flow_type_wan, 1, "wan"), \
    /* end */
DECLARE_ENUM(umd_flow_type, UMD_FLOW_TYPE_ENUM_MAPPER, umd_flow_type_end);

static inline enum_ops_t *umd_flow_type_ops(void);
static inline bool is_good_umd_flow_type(int id);
static inline char *umd_flow_type_getnamebyid(int id);
static inline int umd_flow_type_getidbyname(const char *name);

#define umd_flow_type_lan   umd_flow_type_lan
#define umd_flow_type_wan   umd_flow_type_wan
#define umd_flow_type_end   umd_flow_type_end
#endif

#if 1
#define UMD_FLOW_DIR_ENUM_MAPPER(_) \
    _(umd_flow_dir_up,   0, "up"),  \
    _(umd_flow_dir_down, 1, "down"),\
    _(umd_flow_dir_all,  2, "all"), \
    /* end */
DECLARE_ENUM(umd_flow_dir, UMD_FLOW_DIR_ENUM_MAPPER, umd_flow_dir_end);

static inline enum_ops_t *umd_flow_dir_ops(void);
static inline bool is_good_umd_flow_dir(int id);
static inline char *umd_flow_dir_getnamebyid(int id);
static inline int umd_flow_dir_getidbyname(const char *name);

#define umd_flow_dir_up     umd_flow_dir_up
#define umd_flow_dir_down   umd_flow_dir_down
#define umd_flow_dir_all    umd_flow_dir_all
#define umd_flow_dir_end    umd_flow_dir_end
#endif

typedef struct {
    uint32 max;         /* config */
    uint32 idle;        /* config */
    uint32 numerator;   /* config */
    uint32 denominator; /* config */

    int aging;          /* run data */
    time_t uptime;      /* run data */
    time_t downtime;    /* run data */
} umd_limit_online_t;

typedef struct {
    uint64 max;         /* config */
    uint64 now;         /* run data */
    
    uint64 numerator;   /* config */
    uint64 denominator; /* config */
} umd_limit_flow_t;

typedef struct {
    uint32 max;   /* config */
    uint32 avg;   /* config */
} umd_limit_rate_t;

typedef struct {
    umd_limit_online_t online;
    umd_limit_flow_t flow[umd_flow_dir_end];
    umd_limit_rate_t rate[umd_flow_dir_end];
} umd_limit_t;

typedef struct {
    struct list_head tag;

    char *k;
    char *v;
} umd_tag_t;

#define UM_F_SYNC       0x01

typedef struct {
    byte mac[OS_MACSIZE], __r0[2];
    byte bssid_first[OS_MACSIZE], 
         bssid_current[OS_MACSIZE];
    
    char *ssid;
    
    int state;
    int reason;
    int group;

    uint32 flags;
    uint32 ip; /* network */

    time_t create;
    time_t faketime;
    time_t noused;
    time_t hitime;
    
    umd_limit_t limit[umd_flow_type_end];

    h2_node_t node;
    
    struct {
        struct list_head tag;
    } head;
}
umd_user_t;

static inline umd_limit_t *
umd_limit_get(umd_user_t *user, int type)
{
    return &user->limit[type];
}

static inline umd_limit_online_t *
umd_limit_onliner(umd_user_t *user, int type)
{
    return &umd_limit_get(user, type)->online;
}

static inline umd_limit_flow_t *
umd_limit_flower(umd_user_t *user, int type, int dir)
{
    return &umd_limit_get(user, type)->flow[dir];
}

static inline umd_limit_rate_t *
umd_limit_rater(umd_user_t *user, int type, int dir)
{
    return &umd_limit_get(user, type)->rate[dir];
}

#define umd_limit_reauthor(_max, _numerator, _denominator) ({ \
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

#define umd_online_max(_user, _type)            umd_limit_onliner(_user, _type)->max
#define umd_online_idle(_user, _type)           umd_limit_onliner(_user, _type)->idle
#define umd_online_aging(_user, _type)          umd_limit_onliner(_user, _type)->aging
#define umd_online_uptime(_user, _type)         umd_limit_onliner(_user, _type)->uptime
#define umd_online_downtime(_user, _type)       umd_limit_onliner(_user, _type)->downtime
#define umd_online_numerator(_user, _type)      umd_limit_onliner(_user, _type)->numerator
#define umd_online_denominator(_user, _type)    umd_limit_onliner(_user, _type)->denominator
#define umd_online_reauthor(_user, _type)       \
    umd_limit_reauthor(umd_online_max(_user, _type), umd_online_numerator(_user, _type), umd_online_denominator(_user, _type))

#define umd_flow_max(_user, _type, _dir)            umd_limit_flower(_user, _type, _dir)->max
#define umd_flow_now(_user, _type, _dir)            umd_limit_flower(_user, _type, _dir)->now
#define umd_flow_numerator(_user, _type, _dir)      umd_limit_flower(_user, _type, _dir)->numerator
#define umd_flow_denominator(_user, _type, _dir)    umd_limit_flower(_user, _type, _dir)->denominator
#define umd_flow_reauthor(_user, _type, _dir)   \
    umd_limit_reauthor(umd_flow_max(_user, _type, _dir), umd_flow_numerator(_user, _type, _dir), umd_flow_denominator(_user, _type, _dir))

#define umd_rate_max(_user, _type, _dir)        umd_limit_rater(_user, _type, _dir)->max
#define umd_rate_avg(_user, _type, _dir)        umd_limit_rater(_user, _type, _dir)->avg

#define umd_limit_online(_user, _TYPE)          umd_limit_onliner(_user, umd_flow_type_##_TYPE)
#define umd_limit_flow(_user, _TYPE, _DIR)      umd_limit_flower(_user, umd_flow_type_##_TYPE, umd_flow_dir_##_DIR)
#define umd_limit_rate(_user, _TYPE, _DIR)      umd_limit_rater(_user, umd_flow_type_##_TYPE, umd_flow_dir_##_DIR)

static inline void
umd_update_aging_helper(umd_user_t *user, int type, bool debug)
{
    umd_online_aging(user, type) = umd_online_idle(user, type);

    if (debug) {
        debug_aging("update %s aging to %d",
            umd_flow_type_getnamebyid(type),
            umd_online_aging(user, type));
    }
}

static inline void
umd_update_aging(umd_user_t *user, bool debug)
{
    umd_update_aging_helper(user, umd_flow_type_wan, debug);
    umd_update_aging_helper(user, umd_flow_type_lan, debug);
}

typedef mv_t um_foreach_f(umd_user_t *user);
typedef mv_t um_get_f(umd_user_t *user);

enum {
    UMD_SERVER_TIMER,    /* must first */
    UMD_SERVER_CLI,
    
    UMD_SERVER_FLOW,
    UMD_SERVER_END = UMD_SERVER_FLOW
};

#define umd_intf_id(_server_id)  (_server_id - UMD_SERVER_END)
#define umd_server_id(_intf_id)  (_intf_id + UMD_SERVER_END)

typedef struct {
    char name[1+OS_IFNAME_LEN];
    byte mac[OS_MACSIZE], __r0[2];
    uint32 id;
    
    uint32 index;
    uint32 ip;
    uint32 mask;
    uint32 flag;
} umd_intf_t;

#if 1
#define UM_FORWARD_MODE_ENUM_MAPPER(_)  \
    _(um_forward_mode_br,   0, "br"),   \
    _(um_forward_mode_rt,   1, "rt"),   \
    /* end */
DECLARE_ENUM(forward_mode, UM_FORWARD_MODE_ENUM_MAPPER, um_forward_mode_end);

static inline enum_ops_t *forward_mode_ops(void);
static inline bool is_good_flow_mode(int id);
static inline char *flow_mode_getnamebyid(int id);
static inline int flow_mode_getidbyname(const char *name);

#define um_forward_mode_br  um_forward_mode_br
#define um_forward_mode_rt  um_forward_mode_rt
#define um_forward_mode_end um_forward_mode_end
#endif

typedef struct {
    struct {
        int count;
        umd_intf_t *intf;
    } instance;
    
    char *script_event;
    char *script_getmacbyip;
    char *script_getipbymac;

    bool   syncable;
    bool   reauthable;
    uint32 autouser;
    uint32 gc;
    uint32 sniff_count;
    uint32 ticks;
    uint32 idle;
    uint32 fake;
    uint32 machashsize;
    uint32 iphashsize;
}
umd_config_t;

#define UMD_CFG_INITER                  {   \
    .syncable = UMD_SYNCABLE,               \
    .reauthable = UMD_REAUTHABLE,           \
    .autouser = UMD_AUTOUSER,               \
    .gc = UMD_GC,                           \
    .sniff_count = UMD_SNIFF_COUNT,         \
    .ticks = UMD_TICKS,                     \
    .idle = UMD_IDLE,                       \
    .fake = UMD_FAKE,                       \
    .machashsize = UMD_MACHASHSIZE,         \
    .iphashsize = UMD_IPHASHSIZE,           \
                                            \
    .script_event = UMD_SCRIPT_EVENT,       \
    .script_getipbymac = UMD_SCRIPT_IP,     \
    .script_getmacbyip = UMD_SCRIPT_MAC,    \
}   /* end */

#define UM_LAN_COUNT    3

typedef struct {
    uint32 ip;
    uint32 mask;
    char *ipstring;
    char *maskstring;
} umd_lan_t;
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

typedef struct {
    byte basemac[OS_MACSIZE]; /* local ap's base mac */
    uint16 ether_type_ip;   /* network sort */
    uint16 ether_type_vlan; /* network sort */
    uint16 ether_type_all;  /* network sort */
    
    uint32 ticks;
    bool deinit;

    umd_lan_t lan[UM_LAN_COUNT];
    umd_config_t cfg;
    char *conf;
    sock_server_t **server;
    int server_count;
    h2_table_t table;
} umd_control_t;

#define UMD_INITER          {   \
    .lan    = UMD_LAN_INITER,   \
    .cfg    = UMD_CFG_INITER,   \
    .conf   = UMD_CONF,         \
}   /* end */

extern umd_control_t umd;

static inline sock_server_t *
umd_get_server_by_intf(umd_intf_t *intf)
{
    return umd.server[umd_server_id(intf->id)];
}

static inline umd_intf_t *
umd_get_intf_by_id(int intf_id)
{
    return &umd.cfg.instance.intf[intf_id];
}

static inline umd_intf_t *
umd_get_intf_by_server(sock_server_t *server)
{
    return umd_get_intf_by_id(umd_intf_id(server->id));
}

typedef struct {
    int state;
    
    byte mac[OS_MACSIZE];
    byte macmask[OS_MACSIZE]; /* zero, not use mac as filter */
    
    uint32 ip; /* network */
    uint32 ipmask;/* network. zero, not use ip as filter */
} umd_user_filter_t;

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
#define UM_PKT_TYPE_ENUM_MAPPER(_)  \
    _(um_pkt_type_eth,  0, "eth"),  \
    _(um_pkt_type_vlan, 1, "vlan"), \
    _(um_pkt_type_ip,   2, "ip"),   \
    /* end */
DECLARE_ENUM(pkt_type, UM_PKT_TYPE_ENUM_MAPPER, um_pkt_type_end);

static inline enum_ops_t *pkt_type_ops(void);
static inline bool is_good_pkt_type(int id);
static inline char *pkt_type_getnamebyid(int id);
static inline int pkt_type_getidbyname(const char *name);

#define um_pkt_type_eth     um_pkt_type_eth
#define um_pkt_type_vlan    um_pkt_type_vlan
#define um_pkt_type_ip      um_pkt_type_ip
#define um_pkt_type_end     um_pkt_type_end
#endif

#if 1
#define UM_PKT_CHECK_ENUM_MAPPER(_)     \
    _(um_pkt_check_good,    0, "good"), \
    _(um_pkt_check_bad,     1, "bad"),  \
    _(um_pkt_check_all,     2, "all"),  \
    /* end */
DECLARE_ENUM(pkt_check, UM_PKT_CHECK_ENUM_MAPPER, um_pkt_check_end);

static inline enum_ops_t *pkt_check_ops(void);
static inline bool is_good_pkt_check(int id);
static inline char *pkt_check_getnamebyid(int id);
static inline int pkt_check_getidbyname(const char *name);

#define um_pkt_check_good   um_pkt_check_good
#define um_pkt_check_bad    um_pkt_check_bad
#define um_pkt_check_all    um_pkt_check_all
#define um_pkt_check_end    um_pkt_check_end
#endif

typedef struct {
    uint32 packets;
    uint64 bytes;
} umd_flowst_t;

typedef struct {
    umd_intf_t *intf;
    
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
    
    int type;   /* umd_flow_type_lan/umd_flow_type_wan */
    int dir;    /* umd_flow_dir_up/umd_flow_dir_down/umd_flow_dir_all */

    umd_flowst_t    total[um_pkt_type_end][um_pkt_check_end],
                    dev[umd_flow_type_end][umd_flow_dir_end];
}
umd_flow_t;

/******************************************************************************/
extern jobj_t
umd_juser(umd_user_t *user);

extern umd_user_t *
umd_touser(umd_user_t *user, jobj_t obj);

static inline void
umd_user_dump(char *tag, umd_user_t *user)
{
    jobj_t obj = umd_juser(user);

    os_println("\t%s:%s", tag, jobj_json(obj));

    jobj_put(obj);
}

static inline void
umd_user_debug(char *tag, umd_user_t *user, bool debug)
{
    if (debug) {
        jobj_t obj = umd_juser(user);
        
        jdebug("%o", tag, obj);

        jobj_put(obj);
    }
}

extern jobj_t
umd_jflow(void);

extern int
user_delete(umd_user_t *user);

extern int
user_unbind(umd_user_t *user, int reason);

extern int
user_unfake(umd_user_t *user, int reason);

extern umd_user_t *
user_reauth(umd_user_t *user);

extern int
user_deauth(umd_user_t *user, int reason);

extern umd_tag_t *
umd_user_tag_get(byte mac[], char *k);

extern umd_tag_t *
umd_user_tag_set(byte mac[], char *k, char *v);

extern umd_user_t *
umd_user_create(byte mac[]);

extern int
umd_user_delete(byte mac[]);

extern umd_user_t *
umd_user_block(byte mac[]);

extern int
umd_user_unblock(byte mac[]);

extern umd_user_t *
umd_user_bind(byte mac[], uint32 ip);

extern int
umd_user_unbind(byte mac[]);

extern umd_user_t *
umd_user_fake(byte mac[], uint32 ip);

extern int
umd_user_unfake(byte mac[]);

extern umd_user_t *
umd_user_auth(byte mac[], int group, jobj_t obj);

extern int
umd_user_deauth(byte mac[], int reason);

extern int
umd_user_reauth(byte mac[]);

extern umd_user_t *
umd_user_sync(byte mac[], jobj_t obj);

extern int
umd_user_foreach(um_foreach_f *foreach, bool safe);

extern umd_user_t *
umd_user_get(byte mac[]);

extern umd_user_t *
umd_user_getbyip(uint32 ip);

extern int
umd_user_getby(umd_user_filter_t *filter, um_get_f *get);

extern int
umd_user_delbyip(uint32 ip);

extern int
umd_user_delby(umd_user_filter_t *filter);

#define UM_TEST_JSON    0x01

#ifndef UM_TEST
#define UM_TEST         PC_VAL(UM_TEST_JSON, 0)
#endif

extern void
umd_update_limit_test(void);

extern int 
umd_gc(umd_user_t *user);
/******************************************************************************/
#endif /* __UM_H_c4e41de0b2154a2aa5e5b4c8fd42dc23__ */
