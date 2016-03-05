#ifndef __UM_H_c4e41de0b2154a2aa5e5b4c8fd42dc23__
#define __UM_H_c4e41de0b2154a2aa5e5b4c8fd42dc23__
/******************************************************************************/
#include "um/um.h"

#ifndef UMD_USE_AUTOUSER
#define UMD_USE_AUTOUSER        1
#endif

#ifndef UMD_USE_PROMISC
#define UMD_USE_PROMISC         0
#endif

#ifndef UMD_USE_BINDIF
#define UMD_USE_BINDIF          1
#endif

#ifndef UMD_HASHSIZE
#define UMD_HASHSIZE            256
#endif

#define UMD_HASHMASK            (UMD_HASHSIZE-1)

#ifndef UMD_CONFIG_FILE
#ifdef __PC__
#   define UMD_CONFIG_FILE      "./umd.conf"
#else
#   define UMD_CONFIG_FILE      "/tmp/config/umd.conf"
#endif
#endif

#ifdef __PC__
#   define UMD_SCRIPT_PREFIX    "./"
#else
#   define UMD_SCRIPT_PREFIX    "/etc/um/"
#endif

#ifndef UMD_SCRIPT_EVENT
#define UMD_SCRIPT_EVENT        UMD_SCRIPT_PREFIX "umevent"
#endif

#ifndef UMD_SCRIPT_IP
#define UMD_SCRIPT_IP           UMD_SCRIPT_PREFIX "umip"
#endif

#ifndef UMD_SCRIPT_MAC
#define UMD_SCRIPT_MAC          UMD_SCRIPT_PREFIX "ummac"
#endif

#ifndef UMD_TICKS
#define UMD_TICKS               10  /* second */
#endif

#ifndef UMD_IDLE
#ifdef __PC__
#   define UMD_IDLE             60  /* second */
#else
#   define UMD_IDLE             300 /* second */
#endif
#endif

#ifndef UMD_SNIFF_COUNT
#define UMD_SNIFF_COUNT         32
#endif

#ifndef UMD_INTF_FLOW_DEFT
#define UMD_INTF_FLOW_DEFT      "eth0"
#endif

#ifndef UMD_INTF_FLOW_ETHERTYPE
#define UMD_INTF_FLOW_ETHERTYPE ETHERTYPE_IP
#endif

#ifdef __PC__
#   ifndef UMD_INTF_TC_DEFT
#       define UMD_INTF_TC_DEFT         UMD_INTF_FLOW_DEFT
#   endif
#   ifndef UMD_INTF_TC_ETHERTYPE
#       define UMD_INTF_TC_ETHERTYPE    ETHERTYPE_IP
#   endif
#else
#   ifndef UMD_INTF_TC_DEFT
#       define UMD_INTF_TC_DEFT         "eth0.1"
#   endif
#   ifndef UMD_INTF_TC_ETHERTYPE
#       define UMD_INTF_TC_ETHERTYPE    ETHERTYPE_VLAN
#   endif
#endif

#define __UM_STATE_LIST(_)          \
    _(UM_STATE_NONE, 0, "none"),    \
    _(UM_STATE_BIND, 1, "bind"),    \
    _(UM_STATE_AUTH, 2, "auth"),    \
    /* end */

static inline bool is_good_user_state(int id);
static inline char *user_state_string(int id);
static inline int user_state_idx(char *name);
DECLARE_ENUM(user_state, __UM_STATE_LIST, UM_STATE_END);

#if 1 /* just for sourceinsight */
#define UM_STATE_NONE   UM_STATE_NONE
#define UM_STATE_BIND   UM_STATE_BIND
#define UM_STATE_AUTH   UM_STATE_AUTH
#define UM_STATE_END    UM_STATE_END
#endif /* just for sourceinsight */

static inline bool
__is_none(int state)
{
    return UM_STATE_NONE==state;
}

static inline bool
__is_auth(int state)
{
    return UM_STATE_AUTH==state;
}

static inline bool
__is_bind(int state)
{
    return UM_STATE_BIND==state;
}

static inline bool
__have_bind(int state)
{
    return __is_bind(state) || __is_auth(state);
}

#if UM_USE_MONITOR
#define is_monitor(_user)   os_hasflag((_user)->state, UM_F_MONITOR)
#else
#define is_monitor(_user)   false
#endif

#define is_auth(_user)      __is_auth((_user)->state)
#define is_bind(_user)      __is_bind((_user)->state)
#define is_noused(_user)    (__is_none((_user)->state) && 0==(_user)->flags)
#define have_bind(_user)    __have_bind((_user)->state)

#define __UM_DEAUTH_LIST(_)                     \
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
DECLARE_ENUM(deauth_reason, __UM_DEAUTH_LIST, UM_DEAUTH_END);

#if 1 /* just for sourceinsight */
#define UM_DEAUTH_NONE          UM_DEAUTH_NONE
#define UM_DEAUTH_AUTO          UM_DEAUTH_AUTO
#define UM_DEAUTH_ONLINETIME    UM_DEAUTH_ONLINETIME
#define UM_DEAUTH_FLOWLIMIT     UM_DEAUTH_FLOWLIMIT
#define UM_DEAUTH_ADMIN         UM_DEAUTH_ADMIN
#define UM_DEAUTH_AGING         UM_DEAUTH_AGING
#define UM_DEAUTH_INITIATIVE    UM_DEAUTH_INITIATIVE
#define UM_DEAUTH_END           UM_DEAUTH_END
#endif /* just for sourceinsight */

static inline bool
is_valid_deauth_reason(int reason)
{
    return IS_GOOD_VALUE(reason, UM_DEAUTH_NONE+1, UM_DEAUTH_END);
}

#define __UM_FLOW_TYPE_LIST(_)      \
    _(um_flow_type_lan, 0, "lan"),  \
    _(um_flow_type_wan, 1, "wan"),  \
    /* end */

static inline bool is_good_flow_type(int id);
static inline char *flow_type_string(int id);
static inline int flow_type_idx(char *name);
DECLARE_ENUM(flow_type, __UM_FLOW_TYPE_LIST, um_flow_type_end);

#if 1 /* just for sourceinsight */
#define um_flow_type_lan    um_flow_type_lan
#define um_flow_type_wan    um_flow_type_wan
#define um_flow_type_end    um_flow_type_end
#endif /* just for sourceinsight */

enum {um_flow_type_local = um_flow_type_end};

#define __UM_FLOW_DIR_LIST(_)       \
    _(um_flow_dir_up,   0, "up"),   \
    _(um_flow_dir_down, 1, "down"), \
    _(um_flow_dir_all,  2, "all"),  \
    /* end */

static inline bool is_good_flow_dir(int id);
static inline char *flow_dir_string(int id);
static inline int flow_dir_idx(char *name);
DECLARE_ENUM(flow_dir, __UM_FLOW_DIR_LIST, um_flow_dir_end);

#if 1 /* just for sourceinsight */
#define um_flow_dir_up      um_flow_dir_up
#define um_flow_dir_down    um_flow_dir_down
#define um_flow_dir_all     um_flow_dir_all
#define um_flow_dir_end     um_flow_dir_end
#endif /* just for sourceinsight */

struct um_limit_online {
    uint32_t max;   /* config */
    uint32_t idle;  /* config */
    uint32_t numerator;
    uint32_t denominator;

    int aging; /* run data */
    time_t uptime;  /* run data */
    time_t downtime;/* run data */
};

struct um_limit_flow {
    uint64_t max;   /* config */
    uint64_t now;   /* run data */
    
    uint64_t numerator;
    uint64_t denominator;
};

struct um_limit_rate {
    uint32_t max;   /* config */
    uint32_t avg;   /* config */
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

#ifndef UM_SSID_MAX
#define UM_SSID_MAX     32
#endif

struct um_monitor {
    char ssid[OS_ALIGN(1+UM_SSID_MAX, 4)];
};

#define UM_F_MONITOR    0x01

enum {
    UM_USER_NIDX_MAC,
    UM_USER_NIDX_IP,

    UM_USER_NIDX_END
};

struct um_user {
    byte mac[OS_MACSIZE];
    byte flags;
    byte __r0;
    uint32_t ip; /* network */
    
    int state;
    int reason;
    int group;

    time_t create;
    time_t noused;

    struct um_limit limit[um_flow_type_end];
    
#if UM_USE_MONITOR
    struct um_monitor monitor;
#endif

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
    typeof(_max) _m = (_max);                   \
    typeof(_numerator) _n = (_numerator);       \
    typeof(_denominator) _d = (_denominator);   \
                                                \
    (_m && _n)?(_d?(_m/_d)*_n:_n):0;            \
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
    UM_SERVER_FLOW,     /* must last */
    
    UM_SERVER_END
};

struct um_intf {
    char name[1+OS_IFNAME_LEN];
    
    uint32_t index;
    uint32_t ip;
    uint32_t mask;
    uint32_t flag;
    byte mac[OS_MACSIZE], __r0[2];
};

enum {
    UM_INTF_FLOW    = 0,
#ifdef __PC__
    UM_INTF_TC      = 0,
#else
    UM_INTF_TC      = 1,
#endif

    UM_INTF_END
};

struct um_control {
    byte resv[2], basemac[OS_MACSIZE]; /* local ap's base mac */
    uint32_t ticks;
    bool deinit;

    cli_server_t *server[UM_SERVER_END];
    struct um_intf intf[UM_INTF_END];
    uint32_t gc;
    
    h2_table_t table;
    int hash_size[UM_USER_NIDX_END];
};

extern struct um_control umd;

struct um_user_filter {
    int state;
    
    byte mac[OS_MACSIZE];
    byte macmask[OS_MACSIZE]; /* zero, not use mac as filter */
    
    uint32_t ip; /* network */
    uint32_t ipmask;/* network. zero, not use ip as filter */
};

struct vlan_header {
#if __BYTE_ORDER == __BIG_ENDIAN
    uint16_t pri:3;
    uint16_t cfi:1;
    uint16_t vid:12;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
    uint16_t vid:12;
    uint16_t cfi:1;
    uint16_t pri:3;
#endif

    uint16_t type;
};

struct um_flow {
    byte packet[2048];
    int len;

    struct ether_header *eth;
    struct vlan_header  *vlan;
    struct ip           *iph;

    uint16_t ether_type_ip;   /* network sort */
    uint16_t ether_type_vlan; /* network sort */
    uint16_t ether_type_all;  /* network sort */
    uint16_t __r0;
    
    uint32_t userip; /* network sort */
    byte *usermac;
    
    int type;   /* um_flow_type_lan/um_flow_type_wan */
    int dir;    /* um_flow_dir_up/um_flow_dir_down/um_flow_dir_all */
    
    uint32_t eth_packets;
    uint64_t eth_bytes;
    
    uint32_t vlan_packets;
    uint64_t vlan_bytes;
    
    uint32_t ip_packets;
    uint64_t ip_bytes;
};
/******************************************************************************/
extern jobj_t
um_juser(struct um_user *user);

static inline void
um_user_dump(char *tag, struct um_user *user)
{
    jobj_t obj = um_juser(user);

    os_println("\t%s:%s", tag, jobj_string(obj));

    jobj_put(obj);
}

static inline void
um_user_debug(char *tag, struct um_user *user, bool debug)
{
    if (debug) {
        jobj_t obj = um_juser(user);

        jdebug("%o", tag, jobj_string(obj));

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

#if UM_USE_MONITOR
extern struct um_user *
um_user_enter(byte mac[], jobj_t obj);

extern int
um_user_leave(byte mac[]);
#endif

extern struct um_user *
um_user_bind(byte mac[], uint32_t ip);

extern int
um_user_unbind(byte mac[]);

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
um_user_getbyip(uint32_t ip);

extern int
um_user_getby(struct um_user_filter *filter, um_get_f *get);

extern int
um_user_delbyip(uint32_t ip);

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
