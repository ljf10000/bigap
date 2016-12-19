/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
/*
 * RC4 implementation
 * Issue date: 03/18/2006
 *
 * Copyright (C) 2006 Olivier Gay <olivier.gay@a3.epfl.ch>
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
#include <assert.h>
#include <string.h>

#include "rc4.h"

#define __RC4_CRYPT()           do {    \
    t += s[(byte) i];                   \
    os_swap_value(s[(byte) i], s[t]);   \
    new_dst[i] = new_src[i] ^ s[(byte) (s[(byte) i] + s[t])]; \
}while(0)

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
/******************************************************************************/
