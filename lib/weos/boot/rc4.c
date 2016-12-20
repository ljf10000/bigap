/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#define __RC4_CRYPT()           do {    \
    t += s[(byte) i];                   \
    os_swap_value(s[(byte) i], s[t]);   \
    new_dst[i] = new_src[i] ^ s[(byte) (s[(byte) i] + s[t])]; \
}while(0)

#if USE_MOD_RC4
DECLARE void
rc4_ks(rc4_ctx_t *ctx, const byte *key, uint32 key_len)
{
    static const byte rc4_table[256] = RC4_TABLE_INITER;
    
    uint32 i;
    byte *s;
    byte t, tmp;

    t = 0;
    s = ctx->se;

    assert(key_len > 0 && key_len <= 256);

    ctx->pose = 1;
    ctx->posd = 1;
    ctx->te = 0;
    ctx->td = 0;

    memcpy(s, rc4_table, 256);

    for (i = 0; i < 256; i++) {
        t += s[i] + key[i % key_len];
        os_swap_value(s[i], s[t]);
    }

    memcpy(ctx->sd, s, 256);
}

DECLARE void
rc4_encrypt(rc4_ctx_t *ctx, const byte *src, byte *dst, uint32 len)
{
    uint32 i;
    uint32 pos;
    const byte *new_src;
    byte *s, *new_dst;
    byte t, tmp;

    pos = ctx->pose;
    s = ctx->se;
    t = ctx->te;

    new_src = src - pos;
    new_dst = dst - pos;

    for (i = pos; i < len + pos; i++) {
        __RC4_CRYPT();
    }

    ctx->pose = i;
    ctx->te = t;
}

DECLARE void
rc4_decrypt(rc4_ctx_t *ctx, const byte *src, byte *dst, uint32 len)
{
    uint32 i;
    uint32 pos;
    const byte *new_src;
    byte *s, *new_dst;
    byte t, tmp;

    pos = ctx->posd;
    s = ctx->sd;
    t = ctx->td;

    new_src = src - pos;
    new_dst = dst - pos;

    for (i = pos; i < len + pos; i++) {
        __RC4_CRYPT();
    }

    ctx->posd = i;
    ctx->td = t;
}

#undef __RC4_CRYPT
#endif /* USE_MOD_RC4 */
/******************************************************************************/
