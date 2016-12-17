#ifndef __RSH_H_cd4ac08f199c4732a4decf3ae976b791__
#define __RSH_H_cd4ac08f199c4732a4decf3ae976b791__
/******************************************************************************/
#include "utils.h"
/******************************************************************************/
#ifndef RSH_VERSION
#define RSH_VERSION                 1
#endif

#ifndef RSH_MAGIC
#define RSH_MAGIC                   0x8741
#endif

#ifndef RSH_SEQ_UNKNOW
#define RSH_SEQ_UNKNOW              0
#endif

#ifndef RSH_KEY_BITS
#define RSH_KEY_BITS                256
#endif

#define RSH_KEY_SIZE                (RSH_KEY_BITS/sizeof(uint8))
#define RSH_KEY32_SIZE              (RSH_KEY_BITS/sizeof(uint32))
/******************************************************************************/
#if 1
#define RSH_TLV_ENUM_MAPPER(_) \
    _(RSH_TLV_UNKNOW,   0, "unknow"),   \
    /* end */
DECLARE_ENUM(rsh_tlv, RSH_TLV_ENUM_MAPPER, RSH_TLV_END);

static inline enum_ops_t *rsh_tlv_ops(void);
static inline bool is_good_rsh_tlv(int id);
static inline char *rsh_tlv_getnamebyid(int id);
static inline int rsh_tlv_getidbyname(const char *name);

#define RSH_TLV_UNKNOW  RSH_TLV_UNKNOW
#define RSH_TLV_END     RSH_TLV_END
#endif

/*
    1. ping
        agent==>proxy
        cmd(echo) flag(0)

    2. pong
        agent<==proxy
        cmd(echo) flag(ack)

    3. command-request
        agent<==proxy
        cmd(command) flag(0)

    4. command-ack
        agent==>proxy
        cmd(command) flag(ack)
*/
#if 1
#define RSH_CMD_ENUM_MAPPER(_) \
    _(RSH_CMD_UNKNOW,   0, "unknow"),       \
    _(RSH_CMD_ECHO,     1, "echo"),         \
    _(RSH_CMD_COMMAND,  2, "command"),      \
    _(RSH_CMD_UPDATEKEY,3, "updatekey"),    \
    /* end */
DECLARE_ENUM(rsh_cmd, RSH_CMD_ENUM_MAPPER, RSH_CMD_END);

static inline enum_ops_t *rsh_cmd_ops(void);
static inline bool is_good_rsh_cmd(int id);
static inline char *rsh_cmd_getnamebyid(int id);
static inline int rsh_cmd_getidbyname(const char *name);

#define RSH_CMD_UNKNOW      RSH_CMD_UNKNOW
#define RSH_CMD_ECHO        RSH_CMD_ECHO
#define RSH_CMD_COMMAND     RSH_CMD_COMMAND
#define RSH_CMD_UPDATEKEY   RSH_CMD_UPDATEKEY
#define RSH_CMD_END         RSH_CMD_END

static inline bool is_valid_rsh_cmd(int id)
{
    return is_good_value(id, RSH_CMD_ECHO, RSH_CMD_END);
}
#endif

enum {
    RSH_F_ACK   = 0x01,
    RSH_F_TLV   = 0x10,
};

typedef struct {
    uint32 size; // byte count

    union {
        byte    key[  RSH_KEY_SIZE];
        uint32  key32[RSH_KEY32_SIZE];
    } u;
} rsh_secret_t;

typedef struct {
    byte    md5[OS_MD5_SIZE]; // after hton, before encrypt
    
    uint16  magic;
    byte    cmd;
    byte    version;

    uint16  flag;
    uint16  len;     // NOT include rsh header
    
    byte    mac[OS_MACSIZE];
    int16   error;

    uint32  seq;
    uint32  ack;

    char    body[0];

    union {
        char body[0];

        /*
        * error msg header(network sort)
        * it's body is error info
        */
        char emsg[0];
        
        rsh_secret_t secret;
    } u[0];
} 
rsh_msg_t;        // 40 == sizeof(rsh_msg_t)

enum {
    RSH_MSG_BODYSIZE    = 1412, // 1460(udp mtu) - 40(rsh_msg_t) - 2*4(double vlan)
    RSH_MSG_ALLSIZE     = RSH_MSG_BODYSIZE + sizeof(rsh_msg_t),
};

#define RSH_MSG_SIZE(_bodysize)     AES_ALIGN((_bodysize) + sizeof(rsh_msg_t))

static inline bool
is_good_rsh_msg_size(int size)
{
    return size > 0 && 0 == (size % AES_BLOCK_SIZE);
}

static inline int // include align padding
rsh_msg_size(rsh_msg_t *msg)
{
    return RSH_MSG_SIZE(msg->len);
}

static inline char *
rsh_msg_body(rsh_msg_t *msg)
{
    return msg->u[0].body + msg->len;
}

static inline rsh_secret_t *
rsh_msg_secret(rsh_msg_t *msg)
{
    return &msg->u[0].secret;
}

static inline rsh_msg_t *
rsh_emsg(rsh_msg_t *msg)
{
    return (rsh_msg_t *)msg->u[0].emsg;
}

static inline char *
rsh_emsg_body(rsh_msg_t *msg)
{
    return rsh_emsg(msg)->u[0].body;
}

static inline void
rsh_secret_hton(rsh_secret_t *secret)
{
    secret->size = htonl(secret->size);
}
#define rsh_secret_ntoh(_secret)    rsh_secret_hton(_secret)

static inline void
rsh_body_hton(rsh_msg_t *msg)
{
    if (RSH_CMD_UPDATEKEY==msg->cmd &&
        0==msg->error &&
        os_hasflag(msg->flag, RSH_F_ACK)) {

        rsh_secret_hton(rsh_msg_secret(msg));
    }
}
#define rsh_body_ntoh(_msg)  rsh_body_hton(_msg)

static inline void
rsh_hdr_hton(rsh_msg_t *msg)
{
    msg->magic  = htons(msg->magic);
    msg->flag   = htons(msg->flag);
    msg->len    = htons(msg->len);
    msg->error  = htons(msg->error);
    
    msg->seq    = htonl(msg->seq);
    msg->ack    = htonl(msg->ack);
}
#define rsh_hdr_ntoh(_msg)  rsh_hdr_hton(_msg)

static inline void
rsh_msg_hton(rsh_msg_t *msg)
{
    rsh_body_hton(msg);
    rsh_hdr_hton(msg);
}

static inline void
rsh_msg_ntoh(rsh_msg_t *msg)
{
    rsh_hdr_ntoh(msg);
    rsh_body_ntoh(msg);
}

static inline byte *
rsh_md5_begin(rsh_msg_t *msg)
{
    return msg->md5 + OS_MD5_SIZE;
}

static inline int // include align padding
rsh_md5_size(rsh_msg_t *msg)
{
    return rsh_msg_size(msg) - OS_MD5_SIZE;
}

static inline void
rsh_msg_md5(rsh_msg_t *msg, byte md5[])
{
    md5_encode(md5, rsh_md5_begin(msg), rsh_md5_size(msg));
}

static inline void
rsh_msg_crypt(rsh_msg_t *msg, uint32 *key, int size, aes_crypt_handle_t *crypt)
{
    byte *in, *out, *buffer = rsh_md5_begin(msg);
    int i, count = AES_BLOCK_COUNT(rsh_md5_size(msg));
    
    for (i=0; i<count; i++) {
        in = out = buffer + i*AES_BLOCK_SIZE;
        
        (*crypt)(in, out, key, 8*size);
    }
}

static inline void
rsh_msg_encode(rsh_msg_t *msg, uint32 *key, int size)
{
    rsh_msg_hton(msg);
    rsh_msg_md5(msg, msg->md5);
    rsh_msg_crypt(msg, key, size, aes_encrypt);
}

static inline void
rsh_msg_decode(rsh_msg_t *msg, byte md5[], uint32 *key, int keysize)
{
    rsh_msg_crypt(msg, key, keysize, aes_decrypt);
    rsh_msg_md5(msg, md5);
    rsh_msg_ntoh(msg);
}

static inline int
rsh_msg_append(rsh_msg_t *msg, void *buf, int len)
{
    len = buf?len:0;
    
    int size = AES_ALIGN(len);
    if ((msg->len + size) > RSH_MSG_BODYSIZE) {
        return -ETOOBIG;
    }

    os_memcpy(rsh_msg_body(msg), buf, len);
    os_memzero(rsh_msg_body(msg) + len, len % AES_BLOCK_SIZE);
    msg->len += len;

    return 0;
}

static inline int
rsh_msg_fill(rsh_msg_t *msg, byte mac[], void *buf, int len)
{
    int err;
    
    msg->magic      = RSH_MAGIC;
    msg->version    = RSH_VERSION;
    os_maccpy(msg->mac, mac);

    err = rsh_msg_append(msg, buf, len);
    if (err<0) {
        return err;
    }

    return 0;
    
}

enum {
    RSH_E_PACKET,
    RSH_E_SERVER,
    
    RSH_E_MD5,
    RSH_E_CMD,
    RSH_E_LEN,
    RSH_E_FLAG,
    RSH_E_ACK,
    RSH_E_MAC,
    RSH_E_MAGIC,
    RSH_E_VERSION,

    RSH_E_END
};
/******************************************************************************/
#endif /* __RSH_H_cd4ac08f199c4732a4decf3ae976b791__ */
