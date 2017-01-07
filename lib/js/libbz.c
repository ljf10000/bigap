/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      libjs
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     libbz
#endif

#ifndef __BUSYBOX__
#define __LIB__
#endif

#define __DEAMON__

#include "utils.h"
#include "js.h"

#if js_LIBBZ
#include <bzlib.h>
#include "libbz.h"
#include "libxz.h"

JS_PARAM(bzCompressInit, 4);
STATIC duk_ret_t
duke_bzCompressInit(duk_context *ctx)
{
    duk_pointer_t f = duk_require_pointer(ctx, 0);
    int blockSize100k = duk_require_int(ctx, 1);
    int verbosity = duk_require_int(ctx, 2);
    int workFactor = duk_require_int(ctx, 3);

    int err = BZ2_bzCompressInit(f, blockSize100k, verbosity, workFactor);

    return duk_push_int(ctx, err), 1;
}

JS_PARAM(bzCompress, 2);
STATIC duk_ret_t
duke_bzCompress(duk_context *ctx)
{
    duk_pointer_t f = duk_require_pointer(ctx, 0);
    int action = duk_require_int(ctx, 1);

    int err = BZ2_bzCompress(f, action);

    return duk_push_int(ctx, err), 1;
}

JS_PARAM(bzCompressEnd, 1);
STATIC duk_ret_t
duke_bzCompressEnd(duk_context *ctx)
{
    duk_pointer_t f = duk_require_pointer(ctx, 0);

    int err = BZ2_bzCompressEnd(f);

    return duk_push_int(ctx, err), 1;
}

JS_PARAM(bzDecompressInit, 3);
STATIC duk_ret_t
duke_bzDecompressInit(duk_context *ctx)
{
    duk_pointer_t f = duk_require_pointer(ctx, 0);
    int verbosity = duk_require_int(ctx, 1);
    int small = duk_require_int(ctx, 2);

    int err = BZ2_bzDecompressInit(f, verbosity, small);

    return duk_push_int(ctx, err), 1;
}

JS_PARAM(bzDecompress, 1);
STATIC duk_ret_t
duke_bzDecompress(duk_context *ctx)
{
    duk_pointer_t f = duk_require_pointer(ctx, 0);

    int err = BZ2_bzDecompress(f);

    return duk_push_int(ctx, err), 1;
}

JS_PARAM(bzDecompressEnd, 1);
STATIC duk_ret_t
duke_bzDecompressEnd(duk_context *ctx)
{
    duk_pointer_t f = duk_require_pointer(ctx, 0);

    int err = BZ2_bzDecompressEnd(f);

    return duk_push_int(ctx, err), 1;
}

JS_PARAM(compress, 5);
STATIC duk_ret_t
duke_compress(duk_context *ctx)
{
    int err = 0;
    duk_size_t src_size = 0;
    duk_size_t dst_size = 0;

    duk_buffer_t dst = duk_require_buffer_data(ctx, 0, &dst_size);
    duk_buffer_t src = duk_require_buffer_data(ctx, 1, &src_size);
    int blockSize100k = duk_require_int(ctx, 2);
    int verbosity = duk_require_int(ctx, 3);
    int workFactor = duk_require_int(ctx, 4);

    ulong_t dst_len = dst_size;

    err = BZ2_bzBuffToBuffCompress(dst, &dst_len, src, src_size, blockSize100k, verbosity, workFactor);
    if (err<0) {
        __js_seterrno(ctx, err); goto error;
    }

    err = dst_len;    
error:
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(compressEx, 4);
STATIC duk_ret_t
duke_compressEx(duk_context *ctx)
{
    int err = 0;
    duk_size_t src_size = 0;

    duk_buffer_t src = duk_require_buffer_data(ctx, 0, &src_size);
    int blockSize100k = duk_require_int(ctx, 1);
    int verbosity = duk_require_int(ctx, 2);
    int workFactor = duk_require_int(ctx, 3);

    ulong_t dst_size = src_size + 600;
    duk_buffer_t dst = duk_push_dynamic_buffer(ctx, dst_size);
    if (NULL==dst) {
        __js_seterrno(ctx, -ENOMEM); goto error;
    }

    err = BZ2_bzBuffToBuffCompress(dst, &dst_size, src, src_size, blockSize100k, verbosity, workFactor);
    if (err<0) {
        __js_seterrno(ctx, err); goto error;
    }

    return duk_resize_buffer(ctx, -1, dst_size), 1;
error:
    return duk_push_null(ctx), 1;
}

JS_PARAM(uncompress, 4);
STATIC duk_ret_t
duke_uncompress(duk_context *ctx)
{
    int err = 0;
    duk_size_t src_size = 0;
    duk_size_t dst_size = 0;

    duk_buffer_t dst = duk_require_buffer_data(ctx, 0, &dst_size);
    duk_buffer_t src = duk_require_buffer_data(ctx, 1, &src_size);
    int small = duk_require_int(ctx, 2);
    int verbosity = duk_require_int(ctx, 3);

    ulong_t dst_len = dst_size;

    err = BZ2_bzBuffToBuffDecompress(dst, &dst_len, src, src_size, small, verbosity);
    if (err<0) {
        __js_seterrno(ctx, err); goto error;
    }

    err = dst_len;    
error:
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(uncompressEx, 3);
STATIC duk_ret_t
duke_uncompressEx(duk_context *ctx)
{
    int err = 0;
    duk_size_t src_size = 0;

    duk_buffer_t src = duk_require_buffer_data(ctx, 0, &src_size);
    int small = duk_require_int(ctx, 1);
    int verbosity = duk_require_int(ctx, 2);

    ulong_t dst_size = src_size + 600;
    duk_buffer_t dst = duk_push_dynamic_buffer(ctx, dst_size);
    if (NULL==dst) {
        __js_seterrno(ctx, -ENOMEM); goto error;
    }

    err = BZ2_bzBuffToBuffDecompress(dst, &dst_size, src, src_size, small, verbosity);
    if (err<0) {
        __js_seterrno(ctx, err); goto error;
    }

    return duk_resize_buffer(ctx, -1, dst_size), 1;
error:
    return duk_push_null(ctx), 1;
}

JS_PARAM(bzlibVersion, 0);
STATIC duk_ret_t
duke_bzlibVersion(duk_context *ctx)
{
    return js_push_string(ctx, BZ2_bzlibVersion()), 1;
}

#ifndef BZ_NO_STDIO
#define HFILE   BZFILE*

JS_PARAM(bzopen, 2);
STATIC duk_ret_t
duke_bzopen(duk_context *ctx)
{
    return xzopen(ctx, BZ2_bzopen);
}

JS_PARAM(bzdopen, 2);
STATIC duk_ret_t
duke_bzdopen(duk_context *ctx)
{
    return xzdopen(ctx, BZ2_bzdopen);
}

JS_PARAM(bzread, 2);
STATIC duk_ret_t
duke_bzread(duk_context *ctx)
{
    return xzread(ctx, BZ2_bzread);
}

JS_PARAM(bzreadEx, 2);
STATIC duk_ret_t
duke_bzreadEx(duk_context *ctx)
{
    return xzreadEx(ctx, BZ2_bzread);
}

JS_PARAM(bzwrite, 2);
STATIC duk_ret_t
duke_bzwrite(duk_context *ctx)
{
    return xzwrite(ctx, BZ2_bzwrite);
}

JS_PARAM(bzflush, 1);
STATIC duk_ret_t
duke_bzflush(duk_context *ctx)
{
    HFILE f = (HFILE)duk_require_pointer(ctx, 0);

    int err = BZ2_bzflush(f);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(bzclose, 1);
STATIC duk_ret_t
duke_bzclose(duk_context *ctx)
{
    HFILE f = (HFILE)duk_require_pointer(ctx, 0);

    BZ2_bzclose(f);
    
    return duk_push_int(ctx, 0), 1;
}

JS_PARAM(bzerror, 1);
STATIC duk_ret_t
duke_bzerror(duk_context *ctx)
{
    return xzerror(ctx, BZ2_bzerror);
}
#endif

#include "libbz/libbzf.c"
#include "libbz/libbzn.c"

int js_libbz_register(duk_context *ctx)
{
    duk_push_global_object(ctx);
        duk_push_object(ctx);
            js_libbzf_register(ctx, -1);
            js_libbzn_register(ctx, -1);
        duk_put_prop_string(ctx, -2, js_MOD_LIBBZ);
    duk_pop(ctx);

    debug_ok("register libbz ok.");

    return 0;
}
#endif /* js_LIBBZ */

