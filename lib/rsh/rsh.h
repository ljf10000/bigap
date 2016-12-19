#ifndef __RSH_H_cd4ac08f199c4732a4decf3ae976b791__
#define __RSH_H_cd4ac08f199c4732a4decf3ae976b791__
/******************************************************************************/
#include "utils.h"
/******************************************************************************/
#ifndef RSH_VERSION
#define RSH_VERSION                 1
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

#if 1
#define RSH_CRYPT_ENUM_MAPPER(_)    \
    _(RSH_CRYPT_AES,    0, "aes"),  \
    /* end */
DECLARE_ENUM(rsh_crypt, RSH_CRYPT_ENUM_MAPPER, RSH_CRYPT_END);

static inline enum_ops_t *rsh_crypt_ops(void);
static inline bool is_good_rsh_crypt(int id);
static inline char *rsh_crypt_getnamebyid(int id);
static inline int rsh_crypt_getidbyname(const char *name);

#define RSH_CRYPT_AES   RSH_CRYPT_AES
#define RSH_CRYPT_END   RSH_CRYPT_END
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
    _(RSH_CMD_UPDATE,   2, "update"),       \
    _(RSH_CMD_COMMAND,  3, "command"),      \
    /* end */
DECLARE_ENUM(rsh_cmd, RSH_CMD_ENUM_MAPPER, RSH_CMD_END);

static inline enum_ops_t *rsh_cmd_ops(void);
static inline bool is_good_rsh_cmd(int id);
static inline char *rsh_cmd_getnamebyid(int id);
static inline int rsh_cmd_getidbyname(const char *name);

#define RSH_CMD_UNKNOW      RSH_CMD_UNKNOW
#define RSH_CMD_ECHO        RSH_CMD_ECHO
#define RSH_CMD_COMMAND     RSH_CMD_COMMAND
#define RSH_CMD_UPDATE      RSH_CMD_UPDATE
#define RSH_CMD_END         RSH_CMD_END

static inline bool is_valid_rsh_cmd(int id)
{
    return is_good_value(id, RSH_CMD_ECHO, RSH_CMD_END);
}
#endif

#if 1
#define RSH_UPDATE_ENUM_MAPPER(_) \
    _(RSH_UPDATE_KEY,   0, "key"),      \
    /* end */
DECLARE_ENUM(rsh_update, RSH_UPDATE_ENUM_MAPPER, RSH_UPDATE_END);

static inline enum_ops_t *rsh_update_ops(void);
static inline bool is_good_rsh_update(int id);
static inline char *rsh_update_getnamebyid(int id);
static inline int rsh_update_getidbyname(const char *name);

#define RSH_UPDATE_KEY      RSH_UPDATE_KEY
#define RSH_UPDATE_END      RSH_UPDATE_END

static inline bool is_valid_rsh_update(int id)
{
    return is_good_value(id, RSH_UPDATE_ECHO, RSH_UPDATE_END);
}
#endif

enum {
    RSH_F_ACK   = 0x01,
    RSH_F_TLV   = 0x10,
};

enum {
    RSH_E_HMAC,
    RSH_E_CMD,
    RSH_E_LEN,
    RSH_E_FLAG,
    RSH_E_ACK,
    RSH_E_SEQ,
    RSH_E_MAC,
    RSH_E_VERSION,
    RSH_E_SECRET,
    
    RSH_E_END
};

typedef struct {
    uint32 size; // byte count

    union {
        byte    key[  RSH_KEY_SIZE];
        uint32  key32[RSH_KEY32_SIZE];
    } u;
} rsh_key_t;

typedef struct {
    uint32 type;

    union {
        rsh_key_t key;
    } u;
} rsh_update_t;

typedef struct {
    byte    cmd;
    byte    version;
    /*
    * must
    *   28: SHA224_DIGEST_SIZE
    *   32: SHA256_DIGEST_SIZE
    *   48: SHA384_DIGEST_SIZE
    *   64: SHA512_DIGEST_SIZE
    */
    byte    hmacsize;
    /*
    * is  ack: check it
    * not ack: ignore it
    */
    int8    error;

    uint16  flag;
    /*
    * NOT include rsh header
    * NOT include hmac
    */
    uint16  len;    

    byte    _r0[2];
    byte    mac[OS_MACSIZE];

    uint16  _r1[2];
    uint32  seq;
    uint32  ack;
    uint32  _r2;

    byte hmac[0]; // after hton, before encrypt
} 
rsh_msg_t;        // 32 == sizeof(rsh_msg_t)

enum {
    RSH_MSG_BODYSIZE    = 1344, // 1460(udp mtu) - 32(rsh_msg_t) - 2*4(double vlan) - 64(max hmacsize) - 12
    RSH_MSG_ALLSIZE     = RSH_MSG_BODYSIZE + sizeof(rsh_msg_t),
};

#define RSH_MSG_SIZE(_bodysize)     AES_ALIGN((_bodysize) + sizeof(rsh_msg_t))

#define is_rsh_msg_ack(_msg)        os_hasflag((_msg)->flag, RSH_F_ACK)

static inline bool
is_good_rsh_msg_size(int size)
{
    return size > 0 && 0 == (size % AES_BLOCK_SIZE);
}

static inline int // NOT include align padding
rsh_msg_len(rsh_msg_t *msg)
{
    return msg->hmacsize + msg->len + sizeof(rsh_msg_t);
}

static inline int // include align padding
rsh_msg_size(rsh_msg_t *msg)
{
    return RSH_MSG_SIZE(msg->hmacsize + msg->len);
}

static inline byte *
rsh_msg_body(rsh_msg_t *msg)
{
    return msg->hmac + msg->hmacsize;
}

static inline char *
rsh_msg_current_body(rsh_msg_t *msg)
{
    return (char *)rsh_msg_body(msg) + msg->len;
}

static inline rsh_update_t *
rsh_msg_update(rsh_msg_t *msg)
{
    return (rsh_update_t *)rsh_msg_body(msg);
}

static inline int
rsh_msg_update_type(rsh_msg_t *msg)
{
    return (int)rsh_msg_update(msg)->type;
}

static inline rsh_key_t *
rsh_msg_key(rsh_msg_t *msg)
{
    return rsh_msg_update(msg)->u.key;
}

/*
* error msg header(network sort)
* it's body is error info
*/
static inline rsh_msg_t *
rsh_emsg(rsh_msg_t *msg)
{
    return (rsh_msg_t *)rsh_msg_body(msg);
}

static inline char *
rsh_emsg_body(rsh_msg_t *msg)
{
    return rsh_msg_body(rsh_emsg(msg));
}

static inline void
rsh_key_hton(rsh_key_t *key)
{
    key->size = htonl(key->size);
}
#define rsh_key_ntoh(_secret)    rsh_key_hton(_secret)

static inline void
rsh_update_hton(rsh_update_t *update)
{
    update->type = htonl(update->type);
    rsh_key_hton(&update->u.key);
}
#define rsh_update_ntoh(_update)    rsh_update_hton(_update)

static inline void
rsh_body_hton(rsh_msg_t *msg)
{
    switch(msg->cmd) {
        case RSH_CMD_UPDATE:
            rsh_update_hton(rsh_msg_update(msg));
            
            break;
        default:
            break;
    }
}
#define rsh_body_ntoh(_msg)  rsh_body_hton(_msg)

static inline void
rsh_hdr_hton(rsh_msg_t *msg)
{
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
rsh_hmac_begin(rsh_msg_t *msg)
{
    return msg->hmac + OS_MD5_SIZE;
}

static inline void
rsh_msg_hmac(rsh_msg_t *msg, rsh_key_t *key, byte hmac[], int hmacsize)
{
    hmac_sha2_ctx_t ctx = HMAC_SHA2_CTX_INITER(SHA_TYPE(hmacsize));
    
    hmac_sha2_init(&ctx, key->u.key, key->size);
    hmac_sha2_update(&ctx, msg, sizeof(msg));
    hmac_sha2_update(&ctx, rsh_msg_body(msg), msg->len);
    hmac_sha2_final(&ctx, hmac);
}

static inline void
rsh_msg_crypt(rsh_msg_t *msg, int len, rsh_key_t *key, aes_crypt_handle_t *crypt)
{
    byte *in, *out, *buffer = rsh_hmac_begin(msg);
    int i, count = AES_BLOCK_COUNT(len);
    
    for (i=0; i<count; i++) {
        in = out = buffer + i*AES_BLOCK_SIZE;
        
        (*crypt)(in, out, key->u.key32, 8*key->size);
    }
}

static inline void
rsh_msg_encode(rsh_msg_t *msg, int len, rsh_key_t *key, byte hmac[], int hmacsize)
{
    rsh_msg_hton(msg);
    rsh_msg_hmac(msg, key, hmac, hmacsize);
    rsh_msg_crypt(msg, len, key, aes_encrypt);
}

static inline void
rsh_msg_decode(rsh_msg_t *msg, int len, rsh_key_t *key, byte hmac[], int hmacsize)
{
    rsh_msg_crypt(msg, len, key, aes_decrypt);
    rsh_msg_hmac(msg, key, hmac, hmacsize);
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

    os_memcpy(rsh_msg_current_body(msg), buf, len);
    os_memzero(rsh_msg_current_body(msg) + len, len % AES_BLOCK_SIZE);
    msg->len += len;

    return 0;
}

static inline int
rsh_msg_fill(rsh_msg_t *msg, byte mac[], void *buf, int len)
{
    int err;
    
    msg->version    = RSH_VERSION;
    os_maccpy(msg->mac, mac);

    err = rsh_msg_append(msg, buf, len);
    if (err<0) {
        return err;
    }

    return 0;
    
}
/******************************************************************************/
#endif /* __RSH_H_cd4ac08f199c4732a4decf3ae976b791__ */
