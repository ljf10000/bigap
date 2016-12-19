/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
/*
 * FIPS 180-2 SHA-224/256/384/512 implementation
 * Last update: 02/02/2007
 * Issue date:  04/30/2005
 *
 * Copyright (C) 2005, 2007 Olivier Gay <olivier.gay@a3.epfl.ch>
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
DECLARE_SHA_TABLE(224);
#endif

#if USE_MOD_SHA256
DECLARE_SHA_TABLE(256);
#endif

#if USE_MOD_SHA384
DECLARE_SHA_TABLE(384);
#endif

#if USE_MOD_SHA512
DECLARE_SHA_TABLE(512);
#endif

DECLARE void
sha2_init(sha2_ctx_t *ctx)
{
    switch(ctx->type) {
#if USE_MOD_SHA224
        case SHA_224:
            sha224_init(&ctx->u.ctx224);
            break;
#endif

#if USE_MOD_SHA256
        case SHA_256:
            sha256_init(&ctx->u.ctx256);
            break;
#endif

#if USE_MOD_SHA384
        case SHA_384:
            sha384_init(&ctx->u.ctx384);
            break;
#endif

#if USE_MOD_SHA512
        case SHA_512:
            sha512_init(&ctx->u.ctx512);
            break;
#endif
        default:
            break;
    }
}

DECLARE void
sha2_update(sha2_ctx_t *ctx, const byte *message, uint32 len)
{
    switch(ctx->type) {
#if USE_MOD_SHA224
        case SHA_224:
            sha224_update(&ctx->u.ctx224, message, len);
            break;
#endif

#if USE_MOD_SHA256
        case SHA_256:
            sha256_update(&ctx->u.ctx256, message, len);
            break;
#endif

#if USE_MOD_SHA384
        case SHA_384:
            sha384_update(&ctx->u.ctx384, message, len);
            break;
#endif

#if USE_MOD_SHA512
        case SHA_512:
            sha512_update(&ctx->u.ctx512, message, len);
            break;
#endif
        default:
            break;
    }
}

DECLARE void
sha2_final(sha2_ctx_t *ctx, byte digest[])
{
    switch(ctx->type) {
#if USE_MOD_SHA224
        case SHA_224:
            sha224_final(&ctx->u.ctx224, digest);
            break;
#endif

#if USE_MOD_SHA256
        case SHA_256:
            sha256_final(&ctx->u.ctx256, digest);
            break;
#endif

#if USE_MOD_SHA384
        case SHA_384:
            sha384_final(&ctx->u.ctx384, digest);
            break;
#endif

#if USE_MOD_SHA512
        case SHA_512:
            sha512_final(&ctx->u.ctx512, digest);
            break;
#endif
        default:
            break;
    }
}

DECLARE void
sha2(int type, const byte *message, uint32 len, byte digest[])
{
    switch(type) {
#if USE_MOD_SHA224
        case SHA_224:
            sha224(message, len, digest);
            break;
#endif

#if USE_MOD_SHA256
        case SHA_256:
            sha256(message, len, digest);
            break;
#endif

#if USE_MOD_SHA384
        case SHA_384:
            sha384(message, len, digest);
            break;
#endif

#if USE_MOD_SHA512
        case SHA_512:
            sha512(message, len, digest);
            break;
#endif
        default:
            break;
    }
}
/******************************************************************************/
