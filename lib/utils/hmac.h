#ifndef __HMAC_H_4ae0798d1e3b4690b417a81f3e92db0b__
#define __HMAC_H_4ae0798d1e3b4690b417a81f3e92db0b__
/******************************************************************************/
// https://github.com/ogay/hmac

typedef struct {
    sha224_t ctx_inside;
    sha224_t ctx_outside;

    /* for hmac_reinit */
    sha224_t ctx_inside_reinit;
    sha224_t ctx_outside_reinit;

    byte block_ipad[SHA224_BLOCK_SIZE];
    byte block_opad[SHA224_BLOCK_SIZE];
} hmac_sha224_t;

typedef struct {
    sha256_t ctx_inside;
    sha256_t ctx_outside;

    /* for hmac_reinit */
    sha256_t ctx_inside_reinit;
    sha256_t ctx_outside_reinit;

    byte block_ipad[SHA256_BLOCK_SIZE];
    byte block_opad[SHA256_BLOCK_SIZE];
} hmac_sha256_t;

typedef struct {
    sha384_t ctx_inside;
    sha384_t ctx_outside;

    /* for hmac_reinit */
    sha384_t ctx_inside_reinit;
    sha384_t ctx_outside_reinit;

    byte block_ipad[SHA384_BLOCK_SIZE];
    byte block_opad[SHA384_BLOCK_SIZE];
} hmac_sha384_t;

typedef struct {
    sha512_t ctx_inside;
    sha512_t ctx_outside;

    /* for hmac_reinit */
    sha512_t ctx_inside_reinit;
    sha512_t ctx_outside_reinit;

    byte block_ipad[SHA512_BLOCK_SIZE];
    byte block_opad[SHA512_BLOCK_SIZE];
} hmac_sha512_t;

#define DECLARE_HMAC_SHA_TABLE(_number) \
DECLARE void \
hmac_sha##_number##_init(hmac_sha##_number##_t *ctx, const byte *key, uint32 key_size) \
{                                                           \
    uint32 fill;                                            \
    uint32 num;                                             \
                                                            \
    const byte *key_used;                                   \
    byte key_temp[SHA##_number##_DIGEST_SIZE];              \
    int i;                                                  \
                                                            \
    if (key_size == SHA##_number##_BLOCK_SIZE) {            \
        key_used = key;                                     \
        num = SHA##_number##_BLOCK_SIZE;                    \
    } else {                                                \
        if (key_size > SHA##_number##_BLOCK_SIZE){          \
            num = SHA##_number##_DIGEST_SIZE;               \
            sha##_number(key, key_size, key_temp);          \
            key_used = key_temp;                            \
        } else {                                            \
            key_used = key;                                 \
            num = key_size;                                 \
        }                                                   \
        fill = SHA##_number##_BLOCK_SIZE - num;             \
                                                            \
        memset(ctx->block_ipad + num, 0x36, fill);          \
        memset(ctx->block_opad + num, 0x5c, fill);          \
    }                                                       \
                                                            \
    for (i = 0; i < (int) num; i++) {                       \
        ctx->block_ipad[i] = key_used[i] ^ 0x36;            \
        ctx->block_opad[i] = key_used[i] ^ 0x5c;            \
    }                                                       \
                                                            \
    sha##_number##_init(&ctx->ctx_inside);                  \
    sha##_number##_update(&ctx->ctx_inside, ctx->block_ipad, SHA##_number##_BLOCK_SIZE); \
                                                            \
    sha##_number##_init(&ctx->ctx_outside);                 \
    sha##_number##_update(&ctx->ctx_outside, ctx->block_opad, SHA##_number##_BLOCK_SIZE); \
                                                            \
    /* for hmac_reinit */                                   \
    memcpy(&ctx->ctx_inside_reinit, &ctx->ctx_inside, sizeof(sha##_number##_t)); \
    memcpy(&ctx->ctx_outside_reinit, &ctx->ctx_outside, sizeof(sha##_number##_t)); \
}                                                           \
DECLARE void                                                \
hmac_sha##_number##_reinit(hmac_sha##_number##_t *ctx)      \
{                                                           \
    memcpy(&ctx->ctx_inside, &ctx->ctx_inside_reinit, sizeof(sha##_number##_t)); \
    memcpy(&ctx->ctx_outside, &ctx->ctx_outside_reinit, sizeof(sha##_number##_t)); \
}                                                           \
DECLARE void                                                \
hmac_sha##_number##_update(hmac_sha##_number##_t *ctx, const byte *message, uint32 message_len) \
{                                                           \
    sha##_number##_update(&ctx->ctx_inside, message, message_len); \
}                                                           \
DECLARE void                                                \
hmac_sha##_number##_final(hmac_sha##_number##_t *ctx, byte mac[]) \
{                                                           \
    byte digest_inside[SHA##_number##_DIGEST_SIZE];         \
    byte mac_temp[SHA##_number##_DIGEST_SIZE];              \
                                                            \
    sha##_number##_final(&ctx->ctx_inside, digest_inside);  \
    sha##_number##_update(&ctx->ctx_outside, digest_inside, SHA##_number##_DIGEST_SIZE); \
    sha##_number##_final(&ctx->ctx_outside, mac_temp);      \
    memcpy(mac, mac_temp, SHA##_number##_DIGEST_SIZE);      \
}                                                           \
DECLARE void                                                \
hmac_sha##_number(const byte *key, uint32 key_size, const byte *message, uint32 message_len, byte mac[]) \
{                                                           \
    hmac_sha##_number##_t ctx;                              \
                                                            \
    hmac_sha##_number##_init(&ctx, key, key_size);          \
    hmac_sha##_number##_update(&ctx, message, message_len); \
    hmac_sha##_number##_final(&ctx, mac);                   \
}                                                           \
os_fake_declare                                             \
/* end */

#if USE_MOD_HMAC_SHA224
EXTERN void
hmac_sha224_init(hmac_sha224_t *ctx, const byte *key, uint32 key_size);
EXTERN void
hmac_sha224_reinit(hmac_sha224_t *ctx);
EXTERN void
hmac_sha224_update(hmac_sha224_t *ctx, const byte *message, uint32 message_len);
EXTERN void
hmac_sha224_final(hmac_sha224_t *ctx, byte mac[SHA224_DIGEST_SIZE]);
EXTERN void
hmac_sha224(const byte *key, uint32 key_size, const byte *message, uint32 message_len, byte mac[SHA224_DIGEST_SIZE]);
#endif

#if USE_MOD_HMAC_SHA256
EXTERN void
hmac_sha256_init(hmac_sha256_t *ctx, const byte *key, uint32 key_size);
EXTERN void
hmac_sha256_reinit(hmac_sha256_t *ctx);
EXTERN void
hmac_sha256_update(hmac_sha256_t *ctx, const byte *message, uint32 message_len);
EXTERN void
hmac_sha256_final(hmac_sha256_t *ctx, byte mac[SHA256_DIGEST_SIZE]);
EXTERN void
hmac_sha256(const byte *key, uint32 key_size, const byte *message, uint32 message_len, byte mac[SHA256_DIGEST_SIZE]);
#endif

#if USE_MOD_HMAC_SHA384
EXTERN void
hmac_sha384_init(hmac_sha384_t *ctx, const byte *key, uint32 key_size);
EXTERN void
hmac_sha384_reinit(hmac_sha384_t *ctx);
EXTERN void
hmac_sha384_update(hmac_sha384_t *ctx, const byte *message, uint32 message_len);
EXTERN void
hmac_sha384_final(hmac_sha384_t *ctx, byte mac[SHA384_DIGEST_SIZE]);
EXTERN void
hmac_sha384(const byte *key, uint32 key_size, const byte *message, uint32 message_len, byte mac[SHA384_DIGEST_SIZE]);
#endif

#if USE_MOD_HMAC_SHA512
EXTERN void
hmac_sha512_init(hmac_sha512_t *ctx, const byte *key, uint32 key_size);
EXTERN void
hmac_sha512_reinit(hmac_sha512_t *ctx);
EXTERN void
hmac_sha512_update(hmac_sha512_t *ctx, const byte *message, uint32 message_len);
EXTERN void
hmac_sha512_final(hmac_sha512_t *ctx, byte mac[SHA512_DIGEST_SIZE]);
EXTERN void
hmac_sha512(const byte *key, uint32 key_size, const byte *message, uint32 message_len, byte mac[SHA512_DIGEST_SIZE]);
#endif

typedef struct {
    int type;   // SHA_END

    union {
        hmac_sha224_t ctx224;
        hmac_sha256_t ctx256;
        hmac_sha384_t ctx384;
        hmac_sha512_t ctx512;
    } u;
} hmac_sha2_ctx_t;

#define HMAC_SHA2_CTX_INITER(_type) { .type = _type}

EXTERN void
hmac_sha2_init(hmac_sha2_ctx_t *ctx, const byte *key, uint32 key_size);

EXTERN void
hmac_sha2_reinit(hmac_sha2_ctx_t *ctx);

EXTERN void
hmac_sha2_update(hmac_sha2_ctx_t *ctx, const void *message, uint32 message_len);

EXTERN void
hmac_sha2_final(hmac_sha2_ctx_t *ctx, byte mac[]);

EXTERN void
hmac_sha2(int type, const byte *key, uint32 key_size, const void *message, uint32 message_len, byte mac[]);

#ifdef __BOOT__
#include "weos/boot/hmac.c"
#endif
/******************************************************************************/
#endif /* __HMAC_H_4ae0798d1e3b4690b417a81f3e92db0b__ */
