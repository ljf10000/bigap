/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
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
