#ifndef __RSH_H_cd4ac08f199c4732a4decf3ae976b791__
#define __RSH_H_cd4ac08f199c4732a4decf3ae976b791__
/******************************************************************************/
#include "utils.h"
/******************************************************************************/
#ifndef RSH_VERSION
#define RSH_VERSION                 1
#endif

#ifndef RSH_TICKS
#define RSH_TICKS                   5000 /* ms */
#endif

#ifndef ENV_RSH_CONFIG
#define ENV_RSH_CONFIG              "__RSH_CONFIG__"
#endif

#ifndef ENV_RSH_CLOUD_CONFIG
#define ENV_RSH_CLOUD_CONFIG        "__RSH_CLOUD_CONFIG__"
#endif

#ifndef ENV_RSH_SLOT_LOCAL
#define ENV_RSH_SLOT_LOCAL          "__RSH_SLOT_LOCAL__"
#endif

#ifndef ENV_RSH_SLOT_MASTER
#define ENV_RSH_SLOT_MASTER         "__RSH_SLOT_MASTER__"
#endif

#ifndef ENV_RSH_REQUEST_SCRIPT
#define ENV_RSH_REQUEST_SCRIPT      "__RSH_REQUEST__"
#endif

#ifndef ENV_RSH_RESPONSE_SCRIPT
#define ENV_RSH_RESPONSE_SCRIPT     "__RSH_RESPONSE__"
#endif

#ifndef RSH_SLOT_COUNT
#define RSH_SLOT_COUNT              2
#endif

#define RSH_SLOT_MULTI              (RSH_SLOT_COUNT>1)

#ifndef RSH_PORT
#define RSH_PORT                    8740
#endif

#define RSH_CFG_PREFIX              PC_VAL(__empty, "/tmp/config/")
#define RSH_BIN_PREFIX              PC_VAL(__empty, "/usr/sbin/")

#ifndef RSH_CONFIG_FILE
#define RSH_CONFIG_FILE             RSH_CFG_PREFIX "rsh.config"
#endif

#ifndef RSH_CLOUD_CONFIG_FILE
#define RSH_CLOUD_CONFIG_FILE       RSH_CFG_PREFIX "rsh.cloud"
#endif

#ifndef RSH_SLOT_FILE
#define RSH_SLOT_FILE               RSH_CFG_PREFIX "rsh.%d.slot"
#endif

#ifndef RSH_REQUEST_SCRIPT
#define RSH_REQUEST_SCRIPT          RSH_BIN_PREFIX "rsh.request"
#endif

#ifndef RSH_RESPONSE_SCRIPT
#define RSH_RESPONSE_SCRIPT         RSH_BIN_PREFIX "rsh.response"
#endif

#define RSH_SLOT_MASTER         0
#define RSH_SLOT_CLOUD          RSH_SLOT_COUNT
#define RSH_SLOT_END            (1+RSH_SLOT_COUNT)

static inline bool
is_local_rsh_slot(int slot)
{
    return IS_GOOD_ENUM(slot, RSH_SLOT_CLOUD);
}

#ifndef RSH_ECHO_INTERVAL
#define RSH_ECHO_INTERVAL       60 /* second */
#endif

#ifndef RSH_ECHO_TIMES
#define RSH_ECHO_TIMES          3
#endif
/******************************************************************************/
#if 1
#define RSH_CMD_ENUM_MAPPER(_)          \
    _(RSH_CMD_ECHO,     0, "echo"),     \
    _(RSH_CMD_COMMAND,  1, "command"),  \
    /* end */
DECLARE_ENUM(rsh_cmd, RSH_CMD_ENUM_MAPPER, RSH_CMD_END);

static inline enum_ops_t *rsh_cmd_ops(void);
static inline bool is_good_rsh_cmd(int id);
static inline char *rsh_cmd_getnamebyid(int id);
static inline int rsh_cmd_getidbyname(const char *name);

#define RSH_CMD_ECHO        RSH_CMD_ECHO
#define RSH_CMD_COMMAND     RSH_CMD_COMMAND
#define RSH_CMD_END         RSH_CMD_END
#endif

#if 1
#define RSH_MODE_ENUM_MAPPER(_)         \
    _(RSH_MODE_SYN,     0, "syn"),      \
    _(RSH_MODE_ACK,     1, "ack"),      \
    _(RSH_MODE_ASYN,    2, "asyn"),     \
    _(RSH_MODE_REFLECT, 3, "reflect"),  \
    /* end */
DECLARE_ENUM(rsh_mode, RSH_MODE_ENUM_MAPPER, RSH_MODE_END);

static inline enum_ops_t *rsh_mode_ops(void);
static inline bool is_good_rsh_mode(int id);
static inline char *rsh_mode_getnamebyid(int id);
static inline int rsh_mode_getidbyname(const char *name);

#define RSH_MODE_SYN        RSH_MODE_SYN
#define RSH_MODE_ACK        RSH_MODE_ACK
#define RSH_MODE_ASYN       RSH_MODE_ASYN
#define RSH_MODE_REFLECT    RSH_MODE_REFLECT
#define RSH_MODE_END        RSH_MODE_END
#endif


enum {
    RSH_HDR_MAGIC   = 0,
    RSH_HDR_VERSION = 1,
    RSH_HDR_CMD     = 2,
    RSH_HDR_TARGET  = 3,
    RSH_HDR_LENGTH  = 4,    // NOT include rsh header
    RSH_HDR_FLAG    = 5,

    RSH_HDR_END
};

#define RSH_MAGIC       0x7765
#define RSH_VERSION     1

typedef struct {
    struct {
        uint16 value;
        byte random;
        byte mac;
    } h[RSH_HDR_END];

    uint32 seq;
    uint32 ack;

    byte body[0];
} rsh_msg_t;

#define RSH_BODYSIZE                1400

#define rsh_msg_random(_msg, _idx)  (_msg)->h[_idx].random
#define rsh_msg_mac(_msg, _idx)     (_msg)->h[_idx].mac
#define rsh_msg_value(_msg, _idx)   (_msg)->h[_idx].value
#define rsh_msg_magic(_msg)         rsh_msg_value(_msg, RSH_HDR_MAGIC)
#define rsh_msg_version(_msg)       rsh_msg_value(_msg, RSH_HDR_VERSION)
#define rsh_msg_cmd(_msg)           rsh_msg_value(_msg, RSH_HDR_CMD)
#define rsh_msg_mode(_msg)          rsh_msg_value(_msg, RSH_HDR_MODE)
#define rsh_msg_length(_msg)        rsh_msg_value(_msg, RSH_HDR_LENGTH)
#define rsh_msg_flag(_msg)          rsh_msg_value(_msg, RSH_HDR_FLAG)
#define rsh_msg_body32(_msg)        ((uint32 *)(_msg)->body)

#define RSH_XOR8(_val1, _val2)      (((uint8)(_val1)) ^ ((uint8)(_val2)))
#define RSH_XOR16(_val1, _val2)     (((uint16)(_val1)) ^ ((uint16)(_val2)))
#define RSH_XOR32(_val1, _val2)     (((uint32)(_val1)) ^ ((uint32)(_val2)))

static inline void
rsh_msg_hton(rsh_msg_t *msg)
{
    int i;

    for (i=0; i<RSH_HDR_END; i++) {
        rsh_msg_value(msg, i) = htons(rsh_msg_value(msg, i));
    }

    msg->seq = htonl(msg->seq);
    msg->ack = htonl(msg->ack);
}
#define rsh_msg_ntoh(_msg)  rsh_msg_hton(_msg)

static inline int
rsh_msg_rand(rsh_msg_t *msg)
{
    int i;
    
    /*
    * rand random
    */
    for (i=0; i<RSH_HDR_END; i++) {
        rsh_msg_random(msg, i) = rand() % 0xff;
    }

    return 0;
}

static inline int
rsh_msg_fill(rsh_msg_t *msg, void *buf, int len)
{
    int i;
    
    if (len > RSH_BODYSIZE) {
        return -ETOOBIG;
    }

    rsh_msg_length(msg) = len;
    os_memcpy(msg->body, buf, len);

    rsh_msg_magic(msg)      = RSH_MAGIC;
    rsh_msg_version(msg)    = RSH_VERSION;

    return 0;
}

static inline void
rsh_msg_mac_get(rsh_msg_t *msg, byte mac[])
{
    int i;

    for (i=0; i<RSH_HDR_END; i++) {
        mac[i] = rsh_msg_mac(msg, i);
    }
}

static inline void
rsh_msg_mac_set(rsh_msg_t *msg, byte mac[])
{
    int i;

    for (i=0; i<RSH_HDR_END; i++) {
        rsh_msg_mac(msg, i) = mac[i];
    }
}

static inline bkdr_t
rsh_msg_mac_bkdr(rsh_msg_t *msg)
{
    int i;
    bkdr_t bkdr = 0;
    
    for (i=0; i<RSH_HDR_END; i++) {
        bkdr = __bkdr_push(bkdr, rsh_msg_mac(msg, i));
    }
}

/*
* msg-raw-mac XOR msg-random ==> msg-xor-mac
* msg-xor-mac XOR msg-random ==> msg-raw-mac
*/
static inline void
rsh_msg_mac_xor(rsh_msg_t *msg)
{
    int i;

    for (i=0; i<RSH_HDR_END; i++) {
        rsh_msg_mac(msg, i) = RSH_XOR8(rsh_msg_mac(msg, i), rsh_msg_random(msg, i));
    }
}

/*
* msg-raw-body XOR msg-xor-mac ==> msg-xor-body
* msg-xor-body XOR msg-xor-mac ==> msg-raw-body
*/
static inline void
rsh_msg_body_xor(rsh_msg_t *msg)
{
    int i, idx;

    for (i=0; i<rsh_msg_length(msg); i++) {
        idx = i % RSH_HDR_END;
        
        msg->body[i] = RSH_XOR8(msg->body[i], rsh_msg_mac(msg, idx));
    }
}

/*
* msg-raw-value XOR msg-xor-mac-bkdr ==> msg-xor-value
* msg-xor-value XOR msg-xor-mac-bkdr ==> msg-raw-value
*/
static inline void
rsh_msg_value_xor(rsh_msg_t *msg)
{
    uint16 bkdr16 = rsh_msg_mac_bkdr(msg) & 0xffff;

    int i;

    for (i=0; i<RSH_HDR_END; i++) {
        rsh_msg_value(msg, i) = RSH_XOR16(rsh_msg_value(msg, i), bkdr16);
    }
}

/*
* 1. msg-raw-seq   XOR raw-mac-bdkr     ==> msg-xor-seq
*    msg-raw-rsv   XOR raw-mac-bdkr     ==> msg-xor-rsv
* 2. msg-raw-mac   XOR msg-random       ==> msg-xor-mac
* 3. msg-raw-body  XOR msg-xor-mac      ==> msg-xor-body
* 4. msg-raw-value XOR msg-xor-mac-bkdr ==> msg-xor-value
*/
static inline void
rsh_msg_encode_xor(rsh_msg_t *msg, byte mac[])
{
    uint32 bkdr32 = os_bin_bkdr(mac, OS_MACSIZE);
    
    msg->seq = RSH_XOR32(msg->seq, bkdr32);
    msg->rsv = RSH_XOR32(msg->rsv, bkdr32);
    
    rsh_msg_mac_xor(msg);
    rsh_msg_body_xor(msg);
    rsh_msg_value_xor(msg);
}

/*
* 1. msg-xor-value XOR msg-xor-mac-bkdr ==> msg-raw-value
* 2. msg-xor-body  XOR msg-xor-mac      ==> msg-raw-body
* 3. msg-xor-mac   XOR msg-random       ==> msg-raw-mac
* 4. msg-xor-seq   XOR raw-mac-bdkr     ==> msg-raw-seq
*    msg-xor-rsv   XOR raw-mac-bdkr     ==> msg-raw-rsv
*/
static inline void
rsh_msg_decode_xor(rsh_msg_t *msg)
{
    rsh_msg_value_xor(msg);
    rsh_msg_body_xor(msg);
    rsh_msg_mac_xor(msg);

    uint32 bkdr32 = rsh_msg_mac_bkdr(msg);

    msg->seq = RSH_XOR32(msg->seq, bkdr32);
    msg->rsv = RSH_XOR32(msg->rsv, bkdr32);
}

static inline int
rsh_msg_encode(rsh_msg_t *msg, byte mac[], void *buf, int len)
{
    rsh_msg_mac_set(msg, mac);
    rsh_msg_fill(msg, buf, len);
    rsh_msg_rand(msg);
    rsh_msg_encode_xor(msg, mac);
    rsh_msg_hton(msg);

    return 0;
}

static inline int
rsh_msg_decode(rsh_msg_t *msg, byte mac[])
{
    rsh_msg_ntoh(msg);
    rsh_msg_decode_xor(msg);
    rsh_msg_mac_get(msg, mac);
    
    return 0;
}

/******************************************************************************/
#endif /* __RSH_H_cd4ac08f199c4732a4decf3ae976b791__ */
