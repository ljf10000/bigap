/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
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
#if USE_MOD_SHA224
    DECLARE_HMAC_SHA_TABLE(224);
#endif

#if USE_MOD_SHA256
    DECLARE_HMAC_SHA_TABLE(256);
#endif

#if USE_MOD_SHA384
    DECLARE_HMAC_SHA_TABLE(384);
#endif

#if USE_MOD_SHA512
    DECLARE_HMAC_SHA_TABLE(512);
#endif

DECLARE void
hmac_sha2_init(hmac_sha2_ctx_t *ctx, const byte *key, uint32 key_size)
{
    switch(ctx->type) {
#if USE_MOD_SHA224
        case SHA_224:
            hmac_sha224_init(&ctx->u.ctx224, key, key_size);
            break;
#endif

#if USE_MOD_SHA256
        case SHA_256:
            hmac_sha256_init(&ctx->u.ctx256, key, key_size);
            break;
#endif

#if USE_MOD_SHA384
        case SHA_384:
            hmac_sha384_init(&ctx->u.ctx384, key, key_size);
            break;
#endif

#if USE_MOD_SHA512
        case SHA_512:
            hmac_sha512_init(&ctx->u.ctx512, key, key_size);
            break;
#endif
        default:
            break;
    }
}

DECLARE void
hmac_sha2_reinit(hmac_sha2_ctx_t *ctx)
{
    switch(ctx->type) {
#if USE_MOD_SHA224
        case SHA_224:
            hmac_sha224_reinit(&ctx->u.ctx224);
            break;
#endif

#if USE_MOD_SHA256
        case SHA_256:
            hmac_sha256_reinit(&ctx->u.ctx256);
            break;
#endif

#if USE_MOD_SHA384
        case SHA_384:
            hmac_sha384_reinit(&ctx->u.ctx384);
            break;
#endif

#if USE_MOD_SHA512
        case SHA_512:
            hmac_sha512_reinit(&ctx->u.ctx512);
            break;
#endif
        default:
            break;
    }
}

DECLARE void
hmac_sha2_update(hmac_sha2_ctx_t *ctx, const byte *message, uint32 message_len)
{
    switch(ctx->type) {
#if USE_MOD_SHA224
        case SHA_224:
            hmac_sha224_update(&ctx->u.ctx224, message, message_len);
            break;
#endif

#if USE_MOD_SHA256
        case SHA_256:
            hmac_sha256_update(&ctx->u.ctx256, message, message_len);
            break;
#endif

#if USE_MOD_SHA384
        case SHA_384:
            hmac_sha384_update(&ctx->u.ctx384, message, message_len);
            break;
#endif

#if USE_MOD_SHA512
        case SHA_512:
            hmac_sha512_update(&ctx->u.ctx512, message, message_len);
            break;
#endif
        default:
            break;
    }
}

DECLARE void
hmac_sha2_final(hmac_sha2_ctx_t *ctx, byte mac[])
{
    switch(ctx->type) {
#if USE_MOD_SHA224
        case SHA_224:
            hmac_sha224_final(&ctx->u.ctx224, mac);
            break;
#endif

#if USE_MOD_SHA256
        case SHA_256:
            hmac_sha256_final(&ctx->u.ctx256, mac);
            break;
#endif

#if USE_MOD_SHA384
        case SHA_384:
            hmac_sha384_final(&ctx->u.ctx384, mac);
            break;
#endif

#if USE_MOD_SHA512
        case SHA_512:
            hmac_sha512_final(&ctx->u.ctx512, mac);
            break;
#endif
        default:
            break;
    }
}

DECLARE void
hmac_sha2(int type, const byte *key, uint32 key_size, const byte *message, uint32 message_len, byte mac[])
{
    switch(type) {
#if USE_MOD_SHA224
        case SHA_224:
            hmac_sha224(type, key, key_size, message, message_len, mac);
            break;
#endif

#if USE_MOD_SHA256
        case SHA_256:
            hmac_sha256(type, key, key_size, message, message_len, mac);
            break;
#endif

#if USE_MOD_SHA384
        case SHA_384:
            hmac_sha384(type, key, key_size, message, message_len, mac);
            break;
#endif

#if USE_MOD_SHA512
        case SHA_512:
            hmac_sha512(type, key, key_size, message, message_len, mac);
            break;
#endif
        default:
            break;
    }
}


/******************************************************************************/
