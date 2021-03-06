#ifndef __UMD_H_c4e41de0b2154a2aa5e5b4c8fd42dc23__
#define __UMD_H_c4e41de0b2154a2aa5e5b4c8fd42dc23__
/******************************************************************************/
#include "um/um.h"

#ifndef UMD_MACHASHSIZE
#define UMD_MACHASHSIZE         PC_VAL(8*1024, 1*1024)
#endif

#ifndef UMD_IPHASHSIZE
#define UMD_IPHASHSIZE          PC_VAL(8*1024, 1*1024)
#endif

#ifndef UMD_CONNHASHSIZE
#define UMD_CONNHASHSIZE        PC_VAL(32*1024, 4*1024)
#endif

#ifndef UMD_INTFHASHSIZE
#define UMD_INTFHASHSIZE        PC_VAL(256, 32)
#endif

#ifndef UMD_SYNCABLE
#define UMD_SYNCABLE            PC_VAL(true, true)
#endif

#ifndef UMD_REAUTHABLE
#define UMD_REAUTHABLE          PC_VAL(true, true)
#endif

#ifndef UMD_CONNECTABLE
#define UMD_CONNECTABLE         PC_VAL(false, false)
#endif

#ifndef UMD_AUTOUSER
#define UMD_AUTOUSER            PC_VAL(true, true) // todo: default false
#endif

#ifndef UMD_FAKEABLE
#define UMD_FAKEABLE            PC_VAL(true, true)
#endif

#ifndef UMD_CONNPROTOCOL
#define UMD_CONNPROTOCOL        PC_VAL(false, false)
#endif

#ifndef UMD_PROMISC
#define UMD_PROMISC             PC_VAL(true, true)
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

#ifndef UMD_GCUSER
#define UMD_GCUSER              PC_VAL(30, 300)   /* second */
#endif

#ifndef UMD_GCCONN
#define UMD_GCCONN              PC_VAL(30, 300)   /* second */
#endif

#ifndef UMD_IFNAME_INGRESS
#define UMD_IFNAME_INGRESS      PC_VAL("eth0", "lan0")
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

ALWAYS_INLINE bool
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

enum {
    UM_USER_NIDX_MAC,
    UM_USER_NIDX_IP,

    UM_USER_NIDX_END
};

enum {
    UM_F_SYNC   =   0x01,
};

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
    struct {
        dlist_node_t tag;
    } node;
    
    char *k;
    char *v;
} umd_user_tag_t;

typedef struct {
    byte mac[OS_MACSIZE], __r0[2];
    byte bssid_first[OS_MACSIZE], 
         bssid_current[OS_MACSIZE];
    char ath[1+OS_IFNAME_LEN];
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

    struct {
        dlist_t tag;
    } head;

    struct {
        h2_node_t user;
    } node;
}
umd_user_t;

typedef void umd_user_timer_f(umd_user_t *user, time_t now);

extern umd_limit_t *
umd_limit_get(umd_user_t *user, int type);

extern umd_limit_online_t *
umd_limit_onliner(umd_user_t *user, int type);

extern umd_limit_flow_t *
umd_limit_flower(umd_user_t *user, int type, int dir);

extern umd_limit_rate_t *
umd_limit_rater(umd_user_t *user, int type, int dir);

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

extern void
umd_update_aging_helper(umd_user_t *user, int type, bool debug);

extern void
umd_update_aging(umd_user_t *user, bool debug);

enum {
    UMD_SERVER_TIMER,    /* must first */
    UMD_SERVER_CLI,
    
    UMD_SERVER_FLOW,
    UMD_SERVER_END = UMD_SERVER_FLOW
};

ALWAYS_INLINE int
umd_ingress_id(int server_id)
{
    return server_id - UMD_SERVER_END;
}

ALWAYS_INLINE int
umd_server_id(int ingress_id)
{
    return ingress_id + UMD_SERVER_END;
}

typedef struct {
    char name[1+OS_IFNAME_LEN];
    byte mac[OS_MACSIZE], __r0[2];
    uint32  id;
    
    int     auth;   // umd_auth_type_end
    int     mode;   // umd_forward_mode_end

    char    ipaddr[1+OS_IPSTRINGLEN];
    char    ipmask[1+OS_IPSTRINGLEN];
    
    uint32  index;
    uint32  ip;
    uint32  mask;
    uint32  flag;
    
    struct {
        h1_node_t intf;
    } node;
} 
umd_intf_t, umd_ingress_t;

extern umd_intf_t *
umd_intf_get(byte mac[]);

#if 1
#define UMD_AUTH_TYPE_ENUM_MAPPER(_)        \
    _(umd_auth_type_none,   0, "none"),     \
    _(umd_auth_type_mac,    1, "mac"),      \
    _(umd_auth_type_portal, 2, "portal"),   \
    _(umd_auth_type_webcat, 3, "webcat"),   \
    /* end */
DECLARE_ENUM(umd_auth_type, UMD_AUTH_TYPE_ENUM_MAPPER, umd_auth_type_end);

static inline enum_ops_t *umd_auth_type_ops(void);
static inline bool is_good_umd_auth_type(int id);
static inline char *umd_auth_type_getnamebyid(int id);
static inline int umd_auth_type_getidbyname(const char *name);

#define umd_auth_type_none      umd_auth_type_none
#define umd_auth_type_mac       umd_auth_type_mac
#define umd_auth_type_portal    umd_auth_type_portal
#define umd_auth_type_webcat    umd_auth_type_webcat
#define umd_auth_type_end       umd_auth_type_end

#define umd_auth_type_deft      umd_auth_type_portal
#endif

#if 1
#define UMD_FORWARD_MODE_ENUM_MAPPER(_)  \
    _(umd_forward_mode_br,  0, "br"),   \
    _(umd_forward_mode_rt,  1, "rt"),   \
    /* end */
DECLARE_ENUM(umd_forward_mode, UMD_FORWARD_MODE_ENUM_MAPPER, umd_forward_mode_end);

static inline enum_ops_t *umd_forward_mode_ops(void);
static inline bool is_good_umd_forward_mode(int id);
static inline char *umd_forward_mode_getnamebyid(int id);
static inline int umd_forward_mode_getidbyname(const char *name);

#define umd_forward_mode_br     umd_forward_mode_br
#define umd_forward_mode_rt     umd_forward_mode_rt
#define umd_forward_mode_end    umd_forward_mode_end

#define umd_forward_mode_deft   umd_forward_mode_rt
#endif

#if 1
#define UMD_GC_TARGET_ENUM_MAPPER(_)  \
    _(umd_gc_target_user,  0, "user"),  \
    _(umd_gc_target_conn,  1, "conn"),  \
    /* end */
DECLARE_ENUM(umd_gc_target, UMD_GC_TARGET_ENUM_MAPPER, umd_gc_target_end);

static inline enum_ops_t *umd_gc_target_ops(void);
static inline bool is_good_umd_gc_target(int id);
static inline char *umd_gc_target_getnamebyid(int id);
static inline int umd_gc_target_getidbyname(const char *name);

#define umd_gc_target_user      umd_gc_target_user
#define umd_gc_target_conn      umd_gc_target_conn
#define umd_gc_target_end       umd_gc_target_end
#endif

typedef struct {
    struct {
        int count;
        umd_ingress_t *ingress;
    } instance;
    
    char *script_event;
    char *script_getmacbyip;
    char *script_getipbymac;

    bool    syncable;
    bool    reauthable;
    bool    connectable;
    bool    connprotocol;
    bool    promisc;
    bool    autouser;
    bool    fakeable;
    
    uint32  gcuser;
    uint32  gcconn;
    uint32  ticks;
    uint32  idle;
    uint32  fake;
    uint32  machashsize;
    uint32  iphashsize;
    uint32  connhashsize;
    uint32  intfhashsize;
}
umd_config_t;

#define UMD_CFG_JMAPPER(_) \
    _(&umd.cfg, string, script_event,       UMD_SCRIPT_EVENT)   \
    _(&umd.cfg, string, script_getipbymac,  UMD_SCRIPT_IP)      \
    _(&umd.cfg, string, script_getmacbyip,  UMD_SCRIPT_MAC)     \
    _(&umd.cfg, bool,   syncable,           UMD_SYNCABLE)       \
    _(&umd.cfg, bool,   reauthable,         UMD_REAUTHABLE)     \
    _(&umd.cfg, bool,   connectable,        UMD_CONNECTABLE)    \
    _(&umd.cfg, bool,   autouser,           UMD_AUTOUSER)       \
    _(&umd.cfg, bool,   fakeable,           UMD_FAKEABLE)       \
    _(&umd.cfg, bool,   connprotocol,       UMD_CONNPROTOCOL)   \
    _(&umd.cfg, bool,   promisc,            UMD_PROMISC)        \
    _(&umd.cfg, u32,    gcuser,             UMD_GCUSER)         \
    _(&umd.cfg, u32,    gcconn,             UMD_GCCONN)         \
    _(&umd.cfg, u32,    ticks,              UMD_TICKS)          \
    _(&umd.cfg, u32,    idle,               UMD_IDLE)           \
    _(&umd.cfg, u32,    fake,               UMD_FAKE)           \
    _(&umd.cfg, u32,    machashsize,        UMD_MACHASHSIZE)    \
    _(&umd.cfg, u32,    iphashsize,         UMD_IPHASHSIZE)     \
    _(&umd.cfg, u32,    connhashsize,       UMD_CONNHASHSIZE)   \
    _(&umd.cfg, u32,    intfhashsize,       UMD_INTFHASHSIZE)   \
    /* end */

#define UMD_CFG_INITER      JOBJ_MAP_INITER(UMD_CFG_JMAPPER)

#define UM_PLAN_COUNT       3

typedef struct {
    uint32 ip;
    uint32 mask;
    char *ipstring;
    char *maskstring;
} umd_plan_t;

#define __UMD_LAN_INITER(_ipstring, _maskstring) { \
    .ipstring   = _ipstring,    \
    .maskstring = _maskstring,  \
}   /* end */

#define UMD_PLAN_INITER { \
    __UMD_LAN_INITER("192.168.0.0", "255.255.255.0"), \
    __UMD_LAN_INITER("172.16.0.0", "255.240.0.0"),    \
    __UMD_LAN_INITER("10.0.0.0", "255.0.0.0"),        \
}   /* end */

typedef struct {
    sock_server_t **servers;
    int count;
} umd_server_t;

typedef struct {
    uint16 ip;      /* network sort */
    uint16 vlan;    /* network sort */
    uint16 all;     /* network sort */
    uint16 _r0;
} umd_ether_type_t;

typedef struct {
    byte basemac[OS_MACSIZE], _r0[2]; /* local ap's base mac */
    uint32 ticks;
    uint32 fini;
    char *conf;

    umd_ether_type_t ether_type;
    umd_plan_t plan[UM_PLAN_COUNT];
    umd_config_t cfg;
    umd_server_t server;
    loop_t loop;

    struct {
        h2_table_t user;
        h1_table_t conn, intf;
    } head;
} umd_control_t;

#define UMD_INITER          {   \
    .plan   = UMD_PLAN_INITER,  \
    .cfg    = UMD_CFG_INITER,   \
    .conf   = UMD_CONF,         \
    .loop   = LOOP_INITER,      \
}   /* end */

extern umd_control_t umd;

extern umd_ingress_t *
umd_getingress_byid(int ingress_id);

extern sock_server_t *
umd_getserver_byid(int server_id);

extern sock_server_t *
umd_getserver_byingress(umd_ingress_t *ingress);

extern umd_ingress_t *
umd_getingress_byserver(sock_server_t *server);

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
#define UMD_PKT_TYPE_ENUM_MAPPER(_)  \
    _(umd_pkt_type_eth,  0, "eth"),  \
    _(umd_pkt_type_vlan, 1, "vlan"), \
    _(umd_pkt_type_ip,   2, "ip"),   \
    /* end */
DECLARE_ENUM(umd_pkt_type, UMD_PKT_TYPE_ENUM_MAPPER, umd_pkt_type_end);

static inline enum_ops_t *umd_pkt_type_ops(void);
static inline bool is_good_umd_pkt_type(int id);
static inline char *umd_pkt_type_getnamebyid(int id);
static inline int umd_pkt_type_getidbyname(const char *name);

#define umd_pkt_type_eth     umd_pkt_type_eth
#define umd_pkt_type_vlan    umd_pkt_type_vlan
#define umd_pkt_type_ip      umd_pkt_type_ip
#define umd_pkt_type_end     umd_pkt_type_end
#endif

#if 1
#define UMD_PKT_CHECK_ENUM_MAPPER(_)     \
    _(umd_pkt_check_good,    0, "good"), \
    _(umd_pkt_check_bad,     1, "bad"),  \
    _(umd_pkt_check_all,     2, "all"),  \
    /* end */
DECLARE_ENUM(umd_pkt_check, UMD_PKT_CHECK_ENUM_MAPPER, umd_pkt_check_end);

static inline enum_ops_t *umd_pkt_check_ops(void);
static inline bool is_good_umd_pkt_check(int id);
static inline char *umd_pkt_check_getnamebyid(int id);
static inline int umd_pkt_check_getidbyname(const char *name);

#define umd_pkt_check_good   umd_pkt_check_good
#define umd_pkt_check_bad    umd_pkt_check_bad
#define umd_pkt_check_all    umd_pkt_check_all
#define umd_pkt_check_end    umd_pkt_check_end
#endif

typedef struct {
    uint32 packets;
    uint64 bytes;
} umd_flowst_t;

typedef struct {
    byte packet[2048];
    int len;

    struct ether_header *eth;
    struct vlan_header  *vlan;
    struct ip           *iph;

    struct {
        umd_flowst_t    total[umd_pkt_type_end][umd_pkt_check_end];
        umd_flowst_t    dev[umd_flow_type_end][umd_flow_dir_end];
    } st;
}
umd_flow_t;

#if 1
#define UMD_CONN_DIR_ENUM_MAPPER(_)  \
    _(umd_conn_dir_dev2dev,     0, "dev2dev"),      \
    _(umd_conn_dir_dev2user,    1, "dev2user"),     \
    _(umd_conn_dir_dev2lan,     2, "dev2lan"),      \
    _(umd_conn_dir_dev2wan,     3, "dev2wan"),      \
    _(umd_conn_dir_user2dev,    4, "user2dev"),     \
    _(umd_conn_dir_user2user,   5, "user2user"),    \
    _(umd_conn_dir_user2lan,    6, "user2lan"),     \
    _(umd_conn_dir_user2wan,    7, "user2wan"),     \
    _(umd_conn_dir_lan2dev,     8, "lan2dev"),      \
    _(umd_conn_dir_lan2user,    9, "lan2user"),     \
    _(umd_conn_dir_lan2lan,     10,"lan2lan"),      \
    _(umd_conn_dir_lan2wan,     11,"lan2wan"),      \
    _(umd_conn_dir_wan2dev,     12,"wan2dev"),      \
    _(umd_conn_dir_wan2user,    13,"wan2user"),     \
    _(umd_conn_dir_wan2lan,     14,"wan2lan"),      \
    _(umd_conn_dir_wan2wan,     15,"wan2wan"),      \
    /* end */
DECLARE_ENUM(umd_conn_dir, UMD_CONN_DIR_ENUM_MAPPER, umd_conn_dir_end);

static inline enum_ops_t *umd_conn_dir_ops(void);
static inline bool is_good_umd_conn_dir(int id);
static inline char *umd_conn_dir_getnamebyid(int id);
static inline int umd_conn_dir_getidbyname(const char *name);

#define umd_conn_dir_dev2dev    umd_conn_dir_dev2dev
#define umd_conn_dir_dev2user   umd_conn_dir_dev2user
#define umd_conn_dir_dev2lan    umd_conn_dir_dev2lan
#define umd_conn_dir_dev2wan    umd_conn_dir_dev2wan
#define umd_conn_dir_user2dev   umd_conn_dir_user2dev
#define umd_conn_dir_user2user  umd_conn_dir_user2user
#define umd_conn_dir_user2lan   umd_conn_dir_user2lan
#define umd_conn_dir_user2wan   umd_conn_dir_user2wan
#define umd_conn_dir_lan2dev    umd_conn_dir_lan2dev
#define umd_conn_dir_lan2user   umd_conn_dir_lan2user
#define umd_conn_dir_lan2lan    umd_conn_dir_lan2lan
#define umd_conn_dir_lan2wan    umd_conn_dir_lan2wan
#define umd_conn_dir_wan2dev    umd_conn_dir_wan2dev
#define umd_conn_dir_wan2user   umd_conn_dir_wan2user
#define umd_conn_dir_wan2lan    umd_conn_dir_wan2lan
#define umd_conn_dir_wan2wan    umd_conn_dir_wan2wan
#define umd_conn_dir_end        umd_conn_dir_end
#endif

/*
* size = 16 * uint32 = 64 byte
*/
typedef struct {
    byte smac[OS_MACSIZE];
    byte protocol;
    byte ingress_id;
    
    byte dmac[OS_MACSIZE];
    byte suser;             // source is user
    byte conn_dir;          // umd_conn_dir_end
    
    byte flow_type;         // umd_flow_type_end
    byte flow_dir;          // umd_flow_dir_end
    uint16 flag;
    
    uint32 sip;     // network sort
    uint32 dip;     // network sort
    
    time_t hit;
    bkdr_t bkdr;

    struct {
        h1_node_t conn;
    } node;
} 
umd_conn_t;

static inline byte *
umd_conn_usermac(umd_conn_t *cn)
{
    return cn->suser?cn->smac:cn->dmac;
}

static inline uint32
umd_conn_userip(umd_conn_t *cn)
{
    return cn->suser?cn->sip:cn->dip;
}

typedef int  umd_conn_handle_f(umd_conn_t *cn);
typedef void umd_conn_timer_f(umd_conn_t *cn, time_t now);

#if 1
DECLARE_DB_H1(&umd.head.conn, umd_conn, umd_conn_t, node.conn);

static inline umd_conn_t *
umd_conn_hx_entry(hash_node_t *node);

static inline umd_conn_t *
umd_conn_h1_entry(h1_node_t *node);

static inline int
umd_conn_foreach(mv_t (*foreach)(umd_conn_t *entry), bool safe);
#endif

#if 1
DECLARE_DB_H1(&umd.head.intf, umd_intf, umd_intf_t, node.intf);

static inline umd_intf_t *
umd_intf_hx_entry(hash_node_t *node);

static inline umd_intf_t *
umd_intf_h1_entry(h1_node_t *node);

static inline int
umd_intf_foreach(mv_t (*foreach)(umd_intf_t *intf), bool safe);
#endif
/******************************************************************************/
extern jobj_t
umd_juser(umd_user_t *user);

extern umd_user_t *
umd_touser(umd_user_t *user, jobj_t obj);

extern void
umd_user_dump(char *tag, umd_user_t *user);

extern void
umd_user_debug_helper(char *tag, umd_user_t *user, bool debug);

extern jobj_t
umd_jflow(void);

extern int
umduser_delete(umd_user_t *user);

extern int
umduser_unbind(umd_user_t *user, int reason);

extern int
umduser_unfake(umd_user_t *user, int reason);

extern umd_user_t *
umduser_reauth(umd_user_t *user);

extern int
umduser_deauth(umd_user_t *user, int reason);

extern umd_user_tag_t *
umd_user_tag_get(byte mac[], char *k);

extern umd_user_tag_t *
umd_user_tag_set(byte mac[], char *k, char *v);

extern umd_user_t *
umd_user_create(byte mac[]);

extern int
umd_user_delete(byte mac[]);

extern int
umd_user_diassociate(byte mac[]);

//guoshuai-ssid
extern umd_user_t *
umd_user_associate(byte mac[], char *ath, char *ssid);

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
umd_user_foreach(mv_t (*foreach)(umd_user_t *user), bool safe);

extern umd_user_t *
umd_user_get(byte mac[]);

extern umd_user_t *
umd_user_getbyip(uint32 ip);

extern int
umd_user_getbyfilter(umd_user_filter_t *filter, mv_t (*get)(umd_user_t *user));

extern int
umd_user_delbyip(uint32 ip);

extern int
umd_user_delby(umd_user_filter_t *filter);

#define UM_TEST_JSON    0x01

#ifndef UM_TEST
#define UM_TEST         PC_VAL(UM_TEST_JSON, 0)
#endif

//guoshuai-hostapd-data
#ifdef __OPENWRT__
#ifdef __BUSYBOX__
struct ifreq ifr;
struct {
	char brstamac[18];
	unsigned int rx_data,tx_data;
	unsigned int all_data;
	unsigned long long rx_bytes,tx_bytes;
	unsigned long long all_bytes;
} brstaflow;
#endif
#endif
//end

extern void
umd_update_limit_test(void);

extern mv_t 
umd_user_gc(umd_user_t *user);

extern mv_t
umd_conn_gc(umd_conn_t *cn);

extern int
umd_user_timer(loop_watcher_t *watcher, time_t now);

extern int
umd_conn_timer(loop_watcher_t *watcher, time_t now);
/******************************************************************************/
#endif /* __UMD_H_c4e41de0b2154a2aa5e5b4c8fd42dc23__ */
