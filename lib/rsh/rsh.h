#ifndef __RSH_H_cd4ac08f199c4732a4decf3ae976b791__
#define __RSH_H_cd4ac08f199c4732a4decf3ae976b791__
/******************************************************************************/
#include "utils.h"
/******************************************************************************/
#ifndef RSH_VERSION
#define RSH_VERSION                 1
#endif

#ifndef RSH_MAGIC
#define RSH_MAGIC                   0x7765
#endif

#ifndef RSH_BODYSIZE
#define RSH_BODYSIZE                1400
#endif

#ifndef RSH_SEQ_UNKNOW
#define RSH_SEQ_UNKNOW              0
#endif

#ifndef RSH_TICKS
#define RSH_TICKS                   PC_VAL(1000, 5000)  /* ms */
#endif

#ifndef RSH_PORT
#define RSH_PORT                    8740
#endif

#ifndef RSH_ECHO_INTERVAL
#define RSH_ECHO_INTERVAL           PC_VAL(1000, 5000)  /* ms */
#endif

#ifndef RSH_ECHO_TIMES
#define RSH_ECHO_TIMES              3
#endif
/******************************************************************************/
#if 1
#define RSH_CMD_ENUM_MAPPER(_)          \
    _(RSH_CMD_NONE,     0, "none"),     \
    _(RSH_CMD_ECHO,     1, "echo"),     \
    _(RSH_CMD_COMMAND,  2, "command"),  \
    /* end */
DECLARE_ENUM(rsh_cmd, RSH_CMD_ENUM_MAPPER, RSH_CMD_END);

static inline enum_ops_t *rsh_cmd_ops(void);
static inline bool is_good_rsh_cmd(int id);
static inline char *rsh_cmd_getnamebyid(int id);
static inline int rsh_cmd_getidbyname(const char *name);

#define RSH_CMD_NONE        RSH_CMD_NONE
#define RSH_CMD_ECHO        RSH_CMD_ECHO
#define RSH_CMD_COMMAND     RSH_CMD_COMMAND
#define RSH_CMD_END         RSH_CMD_END
#endif

enum {
    RSH_H_MAGIC     = 0,
    RSH_H_VERSION   = 1,
    RSH_H_CMD       = 2,
    RSH_H_LENGTH    = 3,    // NOT include rsh header
    RSH_H_FLAG      = 4,
    RSH_H_RESV      = 5,

    RSH_H_END
};

enum {
    RSH_F_ECHO  = 0x01,
    RSH_F_SYN   = 0x02,
    RSH_F_ACK   = 0x04,
};

typedef struct {
    struct {
        uint16 value;
        byte random;
        byte mac;
    } h[RSH_H_END];

    uint32 seq;
    uint32 ack;

    byte body[0];
} rsh_msg_t;

#define rsh_msg_random(_msg, _idx)  (_msg)->h[_idx].random
#define rsh_msg_mac(_msg, _idx)     (_msg)->h[_idx].mac
#define rsh_msg_value(_msg, _idx)   (_msg)->h[_idx].value
#define rsh_msg_magic(_msg)         rsh_msg_value(_msg, RSH_H_MAGIC)
#define rsh_msg_version(_msg)       rsh_msg_value(_msg, RSH_H_VERSION)
#define rsh_msg_cmd(_msg)           rsh_msg_value(_msg, RSH_H_CMD)
#define rsh_msg_length(_msg)        rsh_msg_value(_msg, RSH_H_LENGTH)
#define rsh_msg_flag(_msg)          rsh_msg_value(_msg, RSH_H_FLAG)
#define rsh_msg_resv(_msg)          rsh_msg_value(_msg, RSH_H_RESV)
#define rsh_msg_body32(_msg)        ((uint32 *)(_msg)->body)

#define RSH_XOR_8( _val1, _val2)    (((uint8) (_val1)) ^ ((uint8) (_val2)))
#define RSH_XOR_16(_val1, _val2)    (((uint16)(_val1)) ^ ((uint16)(_val2)))
#define RSH_XOR_32(_val1, _val2)    (((uint32)(_val1)) ^ ((uint32)(_val2)))

static inline void
rsh_msg_hton(rsh_msg_t *msg)
{
    int i;

    for (i=0; i<RSH_H_END; i++) {
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
    for (i=0; i<RSH_H_END; i++) {
        rsh_msg_random(msg, i) = rand() % 0xff;
    }

    rsh_msg_resv(msg) = rand() % 0xffff;
    
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

    for (i=0; i<RSH_H_END; i++) {
        mac[i] = rsh_msg_mac(msg, i);
    }
}

static inline void
rsh_msg_mac_set(rsh_msg_t *msg, byte mac[])
{
    int i;

    for (i=0; i<RSH_H_END; i++) {
        rsh_msg_mac(msg, i) = mac[i];
    }
}

static inline bkdr_t
rsh_msg_mac_bkdr(rsh_msg_t *msg)
{
    int i;
    bkdr_t bkdr = 0;
    
    for (i=0; i<RSH_H_END; i++) {
        bkdr = __bkdr_push(bkdr, rsh_msg_mac(msg, i));
    }

    return bkdr;
}

/*
* msg-raw-mac XOR msg-random ==> msg-xor-mac
* msg-xor-mac XOR msg-random ==> msg-raw-mac
*/
static inline void
rsh_msg_mac_xor(rsh_msg_t *msg)
{
    int i;

    for (i=0; i<RSH_H_END; i++) {
        rsh_msg_mac(msg, i) = RSH_XOR_8(rsh_msg_mac(msg, i), rsh_msg_random(msg, i));
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
        idx = i % RSH_H_END;
        
        msg->body[i] = RSH_XOR_8(msg->body[i], rsh_msg_mac(msg, idx));
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

    for (i=0; i<RSH_H_END; i++) {
        rsh_msg_value(msg, i) = RSH_XOR_16(rsh_msg_value(msg, i), bkdr16);
    }
}

/*
* 1. msg-raw-seq   XOR raw-mac-bdkr     ==> msg-xor-seq
*    msg-raw-ack   XOR raw-mac-bdkr     ==> msg-xor-ack
* 2. msg-raw-mac   XOR msg-random       ==> msg-xor-mac
* 3. msg-raw-body  XOR msg-xor-mac      ==> msg-xor-body
* 4. msg-raw-value XOR msg-xor-mac-bkdr ==> msg-xor-value
*/
static inline void
rsh_msg_encode_xor(rsh_msg_t *msg, byte mac[])
{
    uint32 bkdr32 = os_bin_bkdr(mac, OS_MACSIZE);
    
    msg->seq = RSH_XOR_32(msg->seq, bkdr32);
    msg->ack = RSH_XOR_32(msg->ack, bkdr32);
    
    rsh_msg_mac_xor(msg);
    rsh_msg_body_xor(msg);
    rsh_msg_value_xor(msg);
}

/*
* 1. msg-xor-value XOR msg-xor-mac-bkdr ==> msg-raw-value
* 2. msg-xor-body  XOR msg-xor-mac      ==> msg-raw-body
* 3. msg-xor-mac   XOR msg-random       ==> msg-raw-mac
* 4. msg-xor-seq   XOR raw-mac-bdkr     ==> msg-raw-seq
*    msg-xor-ack   XOR raw-mac-bdkr     ==> msg-raw-ack
*/
static inline void
rsh_msg_decode_xor(rsh_msg_t *msg)
{
    rsh_msg_value_xor(msg);
    rsh_msg_body_xor(msg);
    rsh_msg_mac_xor(msg);

    uint32 bkdr32 = rsh_msg_mac_bkdr(msg);

    msg->seq = RSH_XOR_32(msg->seq, bkdr32);
    msg->ack = RSH_XOR_32(msg->ack, bkdr32);
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
