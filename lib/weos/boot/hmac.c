/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#if USE_MOD_HMAC_SHA224
DECLARE_HMAC_SHA_TABLE(224);
#endif

#if USE_MOD_HMAC_SHA256
DECLARE_HMAC_SHA_TABLE(256);
#endif

#if USE_MOD_HMAC_SHA384
DECLARE_HMAC_SHA_TABLE(384);
#endif

#if USE_MOD_HMAC_SHA512
DECLARE_HMAC_SHA_TABLE(512);
#endif

DECLARE void
hmac_sha2_init(hmac_sha2_ctx_t *ctx, const byte *key, uint32 key_size)
{
    switch(ctx->type) {
#if USE_MOD_HMAC_SHA224
        case SHA_224:
            hmac_sha224_init(&ctx->u.ctx224, key, key_size);
            break;
#endif

#if USE_MOD_HMAC_SHA256
        case SHA_256:
            hmac_sha256_init(&ctx->u.ctx256, key, key_size);
            break;
#endif

#if USE_MOD_HMAC_SHA384
        case SHA_384:
            hmac_sha384_init(&ctx->u.ctx384, key, key_size);
            break;
#endif

#if USE_MOD_HMAC_SHA512
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
#if USE_MOD_HMAC_SHA224
        case SHA_224:
            hmac_sha224_reinit(&ctx->u.ctx224);
            break;
#endif

#if USE_MOD_HMAC_SHA256
        case SHA_256:
            hmac_sha256_reinit(&ctx->u.ctx256);
            break;
#endif

#if USE_MOD_HMAC_SHA384
        case SHA_384:
            hmac_sha384_reinit(&ctx->u.ctx384);
            break;
#endif

#if USE_MOD_HMAC_SHA512
        case SHA_512:
            hmac_sha512_reinit(&ctx->u.ctx512);
            break;
#endif
        default:
            break;
    }
}

DECLARE void
hmac_sha2_update(hmac_sha2_ctx_t *ctx, const void *message, uint32 message_len)
{
    switch(ctx->type) {
#if USE_MOD_HMAC_SHA224
        case SHA_224:
            hmac_sha224_update(&ctx->u.ctx224, (const byte *)message, message_len);
            break;
#endif

#if USE_MOD_HMAC_SHA256
        case SHA_256:
            hmac_sha256_update(&ctx->u.ctx256, (const byte *)message, message_len);
            break;
#endif

#if USE_MOD_HMAC_SHA384
        case SHA_384:
            hmac_sha384_update(&ctx->u.ctx384, (const byte *)message, message_len);
            break;
#endif

#if USE_MOD_HMAC_SHA512
        case SHA_512:
            hmac_sha512_update(&ctx->u.ctx512, (const byte *)message, message_len);
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
#if USE_MOD_HMAC_SHA224
        case SHA_224:
            hmac_sha224_final(&ctx->u.ctx224, mac);
            break;
#endif

#if USE_MOD_HMAC_SHA256
        case SHA_256:
            hmac_sha256_final(&ctx->u.ctx256, mac);
            break;
#endif

#if USE_MOD_HMAC_SHA384
        case SHA_384:
            hmac_sha384_final(&ctx->u.ctx384, mac);
            break;
#endif

#if USE_MOD_HMAC_SHA512
        case SHA_512:
            hmac_sha512_final(&ctx->u.ctx512, mac);
            break;
#endif
        default:
            break;
    }
}

DECLARE void
hmac_sha2(int type, const byte *key, uint32 key_size, const void *message, uint32 message_len, byte mac[])
{
    switch(type) {
#if USE_MOD_HMAC_SHA224
        case SHA_224:
            hmac_sha224(key, key_size, (const byte *)message, message_len, mac);
            break;
#endif

#if USE_MOD_HMAC_SHA256
        case SHA_256:
            hmac_sha256(key, key_size, (const byte *)message, message_len, mac);
            break;
#endif

#if USE_MOD_HMAC_SHA384
        case SHA_384:
            hmac_sha384(key, key_size, (const byte *)message, message_len, mac);
            break;
#endif

#if USE_MOD_HMAC_SHA512
        case SHA_512:
            hmac_sha512(key, key_size, (const byte *)message, message_len, mac);
            break;
#endif
        default:
            break;
    }
}


/******************************************************************************/
