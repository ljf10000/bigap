#ifndef __HMAC_H_4ae0798d1e3b4690b417a81f3e92db0b__
#define __HMAC_H_4ae0798d1e3b4690b417a81f3e92db0b__
/******************************************************************************/
// https://github.com/ogay/hmac
/*
 * HMAC-SHA-224/256/384/512 implementation
 * Last update: 06/15/2005
 * Issue date:  06/15/2005
 *
 * Copyright (C) 2005 Olivier Gay <olivier.gay@a3.epfl.ch>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

typedef struct {
    sha224_ctx_t ctx_inside;
    sha224_ctx_t ctx_outside;

    /* for hmac_reinit */
    sha224_ctx_t ctx_inside_reinit;
    sha224_ctx_t ctx_outside_reinit;

    byte block_ipad[SHA224_BLOCK_SIZE];
    byte block_opad[SHA224_BLOCK_SIZE];
} hmac_sha224_ctx_t;

typedef struct {
    sha256_ctx_t ctx_inside;
    sha256_ctx_t ctx_outside;

    /* for hmac_reinit */
    sha256_ctx_t ctx_inside_reinit;
    sha256_ctx_t ctx_outside_reinit;

    byte block_ipad[SHA256_BLOCK_SIZE];
    byte block_opad[SHA256_BLOCK_SIZE];
} hmac_sha256_ctx_t;

typedef struct {
    sha384_ctx_t ctx_inside;
    sha384_ctx_t ctx_outside;

    /* for hmac_reinit */
    sha384_ctx_t ctx_inside_reinit;
    sha384_ctx_t ctx_outside_reinit;

    byte block_ipad[SHA384_BLOCK_SIZE];
    byte block_opad[SHA384_BLOCK_SIZE];
} hmac_sha384_ctx_t;

typedef struct {
    sha512_ctx_t ctx_inside;
    sha512_ctx_t ctx_outside;

    /* for hmac_reinit */
    sha512_ctx_t ctx_inside_reinit;
    sha512_ctx_t ctx_outside_reinit;

    byte block_ipad[SHA512_BLOCK_SIZE];
    byte block_opad[SHA512_BLOCK_SIZE];
} hmac_sha512_ctx_t;

#define DECLARE_HMAC_SHA_TABLE(_number) \
DECLARE void \
hmac_sha##_number##_init(hmac_sha##_number##_ctx_t *ctx, const byte *key, uint32 key_size) \
{                                                           \
    uint32 fill;                                      \
    uint32 num;                                       \
                                                            \
    const byte *key_used;                          \
    byte key_temp[SHA##_number##_DIGEST_SIZE];     \
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
    memcpy(&ctx->ctx_inside_reinit, &ctx->ctx_inside, sizeof(sha##_number##_ctx_t)); \
    memcpy(&ctx->ctx_outside_reinit, &ctx->ctx_outside, sizeof(sha##_number##_ctx_t)); \
}                                                           \
DECLARE void                                                \
hmac_sha##_number##_reinit(hmac_sha##_number##_ctx_t *ctx)    \
{                                                           \
    memcpy(&ctx->ctx_inside, &ctx->ctx_inside_reinit, sizeof(sha##_number##_ctx_t)); \
    memcpy(&ctx->ctx_outside, &ctx->ctx_outside_reinit, sizeof(sha##_number##_ctx_t)); \
}                                                           \
DECLARE void                                                \
hmac_sha##_number##_update(hmac_sha##_number##_ctx_t *ctx, const byte *message, uint32 message_len) \
{                                                           \
    sha##_number##_update(&ctx->ctx_inside, message, message_len); \
}                                                           \
DECLARE void                                                \
hmac_sha##_number##_final(hmac_sha##_number##_ctx_t *ctx, byte *mac, uint32 mac_size) \
{                                                           \
    byte digest_inside[SHA##_number##_DIGEST_SIZE];\
    byte mac_temp[SHA##_number##_DIGEST_SIZE];     \
                                                            \
    sha##_number##_final(&ctx->ctx_inside, digest_inside);  \
    sha##_number##_update(&ctx->ctx_outside, digest_inside, SHA##_number##_DIGEST_SIZE); \
    sha##_number##_final(&ctx->ctx_outside, mac_temp);      \
    memcpy(mac, mac_temp, mac_size);                        \
}                                                           \
DECLARE void                                                \
hmac_sha##_number(const byte *key, uint32 key_size, const byte *message, uint32 message_len, byte *mac, uint32 mac_size) \
{                                                           \
    hmac_sha##_number##_ctx_t ctx;                          \
                                                            \
    hmac_sha##_number##_init(&ctx, key, key_size);          \
    hmac_sha##_number##_update(&ctx, message, message_len); \
    hmac_sha##_number##_final(&ctx, mac, mac_size);         \
}                                                           \
os_fake_declare                                             \
/* end */

#if USE_MOD_SHA224
EXTERN void
hmac_sha224_init(hmac_sha224_ctx_t *ctx, const byte *key, uint32 key_size);
EXTERN void
hmac_sha224_reinit(hmac_sha224_ctx_t *ctx);
EXTERN void
hmac_sha224_update(hmac_sha224_ctx_t *ctx, const byte *message, uint32 message_len);
EXTERN void
hmac_sha224_final(hmac_sha224_ctx_t *ctx, byte *mac, uint32 mac_size);
EXTERN void
hmac_sha224(const byte *key, uint32 key_size, const byte *message, uint32 message_len, byte *mac, uint32 mac_size);
#endif

#if USE_MOD_SHA256
EXTERN void
hmac_sha256_init(hmac_sha256_ctx_t *ctx, const byte *key, uint32 key_size);
EXTERN void
hmac_sha256_reinit(hmac_sha256_ctx_t *ctx);
EXTERN void
hmac_sha256_update(hmac_sha256_ctx_t *ctx, const byte *message, uint32 message_len);
EXTERN void
hmac_sha256_final(hmac_sha256_ctx_t *ctx, byte *mac, uint32 mac_size);
EXTERN void
hmac_sha256(const byte *key, uint32 key_size, const byte *message, uint32 message_len, byte *mac, uint32 mac_size);
#endif

#if USE_MOD_SHA384
EXTERN void
hmac_sha384_init(hmac_sha384_ctx_t *ctx, const byte *key, uint32 key_size);
EXTERN void
hmac_sha384_reinit(hmac_sha384_ctx_t *ctx);
EXTERN void
hmac_sha384_update(hmac_sha384_ctx_t *ctx, const byte *message, uint32 message_len);
EXTERN void
hmac_sha384_final(hmac_sha384_ctx_t *ctx, byte *mac, uint32 mac_size);
EXTERN void
hmac_sha384(const byte *key, uint32 key_size, const byte *message, uint32 message_len, byte *mac, uint32 mac_size);
#endif

#if USE_MOD_SHA512
EXTERN void
hmac_sha512_init(hmac_sha512_ctx_t *ctx, const byte *key, uint32 key_size);
EXTERN void
hmac_sha512_reinit(hmac_sha512_ctx_t *ctx);
EXTERN void
hmac_sha512_update(hmac_sha512_ctx_t *ctx, const byte *message, uint32 message_len);
EXTERN void
hmac_sha512_final(hmac_sha512_ctx_t *ctx, byte *mac, uint32 mac_size);
EXTERN void
hmac_sha512(const byte *key, uint32 key_size, const byte *message, uint32 message_len, byte *mac, uint32 mac_size);
#endif

#ifdef __BOOT__
#include "weos/boot/hmac.c"
#endif
/******************************************************************************/
#endif /* __HMAC_H_4ae0798d1e3b4690b417a81f3e92db0b__ */
