#ifndef __RSH_H_cd4ac08f199c4732a4decf3ae976b791__
#define __RSH_H_cd4ac08f199c4732a4decf3ae976b791__
/******************************************************************************/
#include "utils.h"
/******************************************************************************/
#ifndef RSH_VERSION
#define RSH_VERSION                 1
#endif

#ifndef RSH_SEQ_OFFSET
#define RSH_SEQ_OFFSET              1024
#endif

#ifndef RSH_KEY_BITS
#define RSH_KEY_BITS                256
#endif

#define RSH_KEY_SIZE                (RSH_KEY_BITS/sizeof(uint8))
#define RSH_KEY32_COUNT             AES_KEY32_COUNT

#ifndef RSH_PRINT
#define RSH_PRINT                   0
#endif

#if RSH_PRINT
#define rsh_println(_fmt, _args...)     os_println(_fmt, ##_args)
#else
#define rsh_println(_fmt, _args...)     os_do_nothing()
#endif
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

static inline bool
is_valid_rsh_cmd(int id)
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
        debug_entry("bad key:%s size:%s", keystring, size);
        
        err = -EBADKEY; goto error;
    }

    err = os_hex2bin(keystring, key->key, size);
    if (err<0) {
        debug_entry("bad hex key:%s", keystring);
        
        err = -EBADKEY; goto error;
    }
    key->size = size;
    aes_key_setup(key->key, key->key32, 8*key->size);
    
    return 0;
error:

    return err;
}

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
    byte    r0;
    
    uint16  flag;
    /*
    * NOT include rsh header
    * NOT include hmac
    */
    uint16  len;

    /*
    * is  ack: check it
    * not ack: ignore it
    */
    int16   error;
    byte    mac[OS_MACSIZE];

    uint32  seq;
    uint32  ack;
    uint32  r1;
    uint32  r2;

    byte hmac[0]; // after hton, before encrypt
} 
rsh_msg_t;        // 32 == sizeof(rsh_msg_t)
#define RSH_CRYPT_HDR_SIZE     (sizeof(rsh_msg_t)/2)

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

static inline int
rsh_msg_padlen(rsh_msg_t *msg)
{
    return rsh_msg_size(msg) - rsh_msg_len(msg);
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

static inline void
rsh_msg_padzero(rsh_msg_t *msg)
{
    os_memzero(rsh_msg_current(msg), rsh_msg_padlen(msg));
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
    msg->r1     = htonl(msg->r1);
    msg->r2     = htonl(msg->r2);

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
    uint32 bsize = len - sizeof(rsh_msg_t) - hmacsize;
    
    hmac_sha2_init(&ctx, key->key, key->size);
    
    // calc msg header
    hmac_sha2_update(&ctx, (const byte *)msg, sizeof(rsh_msg_t));
    if (__is_ak_debug_crypt) {
        os_println("hmac calc msg hdr:");
        os_dump_buffer(msg, sizeof(rsh_msg_t));
    }
    
    // calc msg body
    if (bsize > 0) {
        hmac_sha2_update(&ctx, (const byte *)rsh_msg_body(msg), bsize);
        if (__is_ak_debug_crypt) {
            os_println("hmac calc msg body:");
            os_dump_buffer(rsh_msg_body(msg), bsize);
        }
    }
    
    hmac_sha2_final(&ctx, hmac);
}

static inline void
rsh_msg_crypt(byte *buffer, int len, rsh_key_t *key, aes_crypt_handle_t *crypt)
{
    byte *in, *out;
    int i, count = AES_BLOCK_COUNT(len);

    for (i=0; i<count; i++) {
        in = out = buffer + i*AES_BLOCK_SIZE;
        
        (*crypt)(in, out, key->key32, 8*key->size);
    }
}

static inline void
rsh_msg_encrypt(rsh_msg_t *msg, int len, rsh_key_t *pmk, rsh_key_t *key)
{
    /*
    * 1. encrypt msg (not include first 16 bytes) by key
    * 2. encrypt msg by pmk
    */
    if (__is_ak_debug_crypt) {
        os_println("msg before key encrypt:");
        os_dump_buffer(msg, len);
    }
    
    rsh_msg_crypt((byte *)msg + RSH_CRYPT_HDR_SIZE, len - RSH_CRYPT_HDR_SIZE, key, aes_encrypt);

    if (__is_ak_debug_crypt) {
        os_println("msg after key encrypt:");
        os_dump_buffer(msg, len);
    }
    
    rsh_msg_crypt((byte *)msg, len, pmk, aes_encrypt);

    if (__is_ak_debug_crypt) {
        os_println("msg after pmk encrypt:");
        os_dump_buffer(msg, len);
    }
}

static inline void
rsh_msg_decrypt(rsh_msg_t *msg, int len, rsh_key_t *pmk, rsh_key_t *key)
{
    /*
    * 2. decrypt msg by pmk
    * 1. decrypt msg (not include first 16 bytes) by key
    */
    if (__is_ak_debug_crypt) {
        os_println("msg before pmk decrypt:");
        os_dump_buffer(msg, len);
    }
    
    rsh_msg_crypt((byte *)msg, len, pmk, aes_decrypt);

    if (__is_ak_debug_crypt) {
        os_println("msg after pmk decrypt:");
        os_dump_buffer(msg, len);
    }
    
    rsh_msg_crypt((byte *)msg + RSH_CRYPT_HDR_SIZE, len - RSH_CRYPT_HDR_SIZE, key, aes_decrypt);
    
    if (__is_ak_debug_crypt) {
        os_println("msg after key decrypt:");
        os_dump_buffer(msg, len);
    }
}

static inline void
rsh_msg_encode(rsh_msg_t *msg, int len, rsh_key_t *pmk, rsh_key_t *key, byte hmac[], int hmacsize)
{
    /*
    * 1. host==>network
    * 2. calc hmac and save hmac to msg
    * 3. encrypt msg
    */
    rsh_msg_hton(msg);
    rsh_msg_hmac(msg, len, key, hmac, hmacsize);
    rsh_msg_encrypt(msg, len, pmk, key);
}

static inline void
rsh_msg_decode(rsh_msg_t *msg, int len, rsh_key_t *pmk, rsh_key_t *key, byte hmac[], int hmacsize)
{
    /*
    * 1. decrypt msg
    * 2. calc hmac and save hmac to instance
    * 3. network==>host
    */
    rsh_msg_decrypt(msg, len, pmk, key);
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
