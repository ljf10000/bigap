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
#define RSH_KEY32_COUNT             AES_KEY32_COUNT
/******************************************************************************/
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
    _(RSH_CMD_HANDSHAKE,1, "handshake"),    \
    _(RSH_CMD_ECHO,     2, "echo"),         \
    _(RSH_CMD_COMMAND,  3, "command"),      \
    /* end */
DECLARE_ENUM(rsh_cmd, RSH_CMD_ENUM_MAPPER, RSH_CMD_END);

static inline enum_ops_t *rsh_cmd_ops(void);
static inline bool is_good_rsh_cmd(int id);
static inline char *rsh_cmd_getnamebyid(int id);
static inline int rsh_cmd_getidbyname(const char *name);

#define RSH_CMD_UNKNOW      RSH_CMD_UNKNOW
#define RSH_CMD_HANDSHAKE   RSH_CMD_HANDSHAKE
#define RSH_CMD_ECHO        RSH_CMD_ECHO
#define RSH_CMD_COMMAND     RSH_CMD_COMMAND
#define RSH_CMD_END         RSH_CMD_END

static inline bool is_valid_rsh_cmd(int id)
{
    return is_good_value(id, RSH_CMD_HANDSHAKE, RSH_CMD_END);
}
#endif

enum {
    RSH_F_ACK   = 0x01,
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
    uint32  size; // byte count
    byte    key[  RSH_KEY_SIZE];
    
    uint32  key32[RSH_KEY32_COUNT];
} rsh_key_t;

static inline int
rsh_key_hex2bin(rsh_key_t *key, char *keystring)
{
    int err, len = os_strlen(keystring), size = len/2;
    
    if (len%2 || false==is_good_aes_key_size(size)) {
        err = -EBADKEY; goto error;
    }

    err = os_hex2bin(keystring, key->key, size);
    if (err<0) {
        err = -EBADKEY; goto error;
    }
    key->size = size;
    
    return 0;
error:

    return err;
}

static inline rsh_key_t *
rsh_key_setup(rsh_key_t *dst, rsh_key_t *src)
{
    dst->size = src->size;
    aes_key_setup(src->key, dst->key32, 8*src->size);

    return dst;
}

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

    byte    _r1[2];
    byte    mac[OS_MACSIZE];

    uint16  _r2[2];
    uint32  seq;
    uint32  ack;
    uint32  _r3;

    byte hmac[0]; // after hton, before encrypt
} 
rsh_msg_t;        // 32 == sizeof(rsh_msg_t)

typedef struct {
    int cmd;
} rsh_over_t;

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
rsh_msg_current(rsh_msg_t *msg)
{
    return (char *)rsh_msg_body(msg) + msg->len;
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

static inline byte *
rsh_emsg_body(rsh_msg_t *msg)
{
    return rsh_msg_body(rsh_emsg(msg));
}

static inline void
rsh_body_hton(rsh_msg_t *msg)
{
    os_do_nothing();
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
rsh_msg_hmac(rsh_msg_t *msg, int len, rsh_key_t *key, byte hmac[], int hmacsize)
{
    hmac_sha2_ctx_t ctx = HMAC_SHA2_CTX_INITER(SHA_TYPE(hmacsize));
    
    hmac_sha2_init(&ctx, key->key, key->size);
    hmac_sha2_update(&ctx, (const byte *)msg, sizeof(rsh_msg_t));
    hmac_sha2_update(&ctx, (const byte *)rsh_msg_body(msg), len - sizeof(rsh_msg_t) - hmacsize);
    hmac_sha2_final(&ctx, hmac);
}

static inline void
__rsh_msg_crypt(byte *buffer, int len, rsh_key_t *key, aes_crypt_handle_t *crypt)
{
    byte *in, *out;
    int i, count = AES_BLOCK_COUNT(len);

    for (i=0; i<count; i++) {
        in = out = buffer + i*AES_BLOCK_SIZE;
        
        (*crypt)(in, out, key->key32, 8*key->size);
    }
}

static inline void
rsh_msg_hdr_crypt(rsh_msg_t *msg, rsh_key_t *pmk, aes_crypt_handle_t *crypt)
{
    __rsh_msg_crypt((byte *)msg, sizeof(rsh_msg_t), pmk, crypt);
}

static inline void // include hmac
rsh_msg_body_crypt(byte *body, int body_len, rsh_key_t *key, aes_crypt_handle_t *crypt)
{
    __rsh_msg_crypt(body, body_len, key, crypt);
}

static inline void
rsh_msg_crypt(rsh_msg_t *msg, int len, rsh_key_t *pmk, rsh_key_t *key, aes_crypt_handle_t *crypt)
{
    /*
    * 1. encrypt/decrypt msg header by pmk
    * 2. encrypt/decrypt msg body(include hmac) by key
    */
    rsh_msg_hdr_crypt(msg, pmk, crypt);
    rsh_msg_body_crypt(msg->hmac, len - sizeof(rsh_msg_t), key, crypt);
}

static inline void
rsh_msg_encode(rsh_msg_t *msg, int len, rsh_key_t *pmk, rsh_key_t *key, byte hmac[], int hmacsize)
{
    /*
    * 1. network==>host
    * 2. calc hmac and save hmac to msg
    * 3. encrypt msg
    */
    rsh_msg_hton(msg);
    rsh_msg_hmac(msg, len, key, hmac, hmacsize);
    rsh_msg_crypt(msg, len, pmk, key, aes_encrypt);
}

static inline void
rsh_msg_decode(rsh_msg_t *msg, int len, rsh_key_t *pmk, rsh_key_t *key, byte hmac[], int hmacsize)
{
    /*
    * 1. decrypt msg
    * 2. calc hmac and save hmac to instance
    * 3. network==>host
    */
    rsh_msg_crypt(msg, len, pmk, key, aes_decrypt);
    rsh_msg_hmac(msg, len, key, hmac, hmacsize);
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

    os_memcpy(rsh_msg_current(msg), buf, len);
    os_memzero(rsh_msg_current(msg) + len, len % AES_BLOCK_SIZE);
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
