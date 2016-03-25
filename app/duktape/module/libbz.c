#ifndef __THIS_APP
#define __THIS_APP      js
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     "libbz"
#endif

#include "utils.h"
#include "dukc.h"

#if duk_LIBBZ
#include <bzlib.h>
#include "libbz.h"

LIB_PARAM(bzCompressInit, 4);
static duk_ret_t
duke_bzCompressInit(duk_context *ctx)
{
    duk_pointer_t f = duk_require_pointer(ctx, 0);
    int blockSize100k = duk_require_int(ctx, 1);
    int verbosity = duk_require_int(ctx, 2);
    int workFactor = duk_require_int(ctx, 3);

    int err = BZ2_bzCompressInit(f, blockSize100k, verbosity, workFactor);

    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(bzCompress, 2);
static duk_ret_t
duke_bzCompress(duk_context *ctx)
{
    duk_pointer_t f = duk_require_pointer(ctx, 0);
    int action = duk_require_int(ctx, 1);

    int err = BZ2_bzCompress(f, action);

    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(bzCompressEnd, 1);
static duk_ret_t
duke_bzCompressEnd(duk_context *ctx)
{
    duk_pointer_t f = duk_require_pointer(ctx, 0);

    int err = BZ2_bzCompressEnd(f);

    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(bzDecompressInit, 3);
static duk_ret_t
duke_bzDecompressInit(duk_context *ctx)
{
    duk_pointer_t f = duk_require_pointer(ctx, 0);
    int verbosity = duk_require_int(ctx, 1);
    int small = duk_require_int(ctx, 2);

    int err = BZ2_bzDecompressInit(f, verbosity, small);

    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(bzDecompress, 1);
static duk_ret_t
duke_bzDecompress(duk_context *ctx)
{
    duk_pointer_t f = duk_require_pointer(ctx, 0);

    int err = BZ2_bzDecompress(f);

    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(bzDecompressEnd, 1);
static duk_ret_t
duke_bzDecompressEnd(duk_context *ctx)
{
    duk_pointer_t f = duk_require_pointer(ctx, 0);

    int err = BZ2_bzDecompressEnd(f);

    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(bzBuffToBuffCompress, 5);
static duk_ret_t
duke_bzBuffToBuffCompress(duk_context *ctx)
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
        __seterrno(ctx, err); goto error;
    }

    err = dst_len;    
error:
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(bzBuffToBuffCompressEx, 4);
static duk_ret_t
duke_bzBuffToBuffCompressEx(duk_context *ctx)
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
        __seterrno(ctx, -ENOMEM); goto error;
    }

    err = BZ2_bzBuffToBuffCompress(dst, &dst_size, src, src_size, blockSize100k, verbosity, workFactor);
    if (err<0) {
        __seterrno(ctx, err); goto error;
    }

    return duk_resize_buffer(ctx, -1, dst_size), 1;
error:
    return duk_push_null(ctx), 1;
}

LIB_PARAM(bzBuffToBuffDecompress, 4);
static duk_ret_t
duke_bzBuffToBuffDecompress(duk_context *ctx)
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
        __seterrno(ctx, err); goto error;
    }

    err = dst_len;    
error:
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(bzBuffToBuffDecompressEx, 3);
static duk_ret_t
duke_bzBuffToBuffDecompressEx(duk_context *ctx)
{
    int err = 0;
    duk_size_t src_size = 0;

    duk_buffer_t src = duk_require_buffer_data(ctx, 0, &src_size);
    int small = duk_require_int(ctx, 1);
    int verbosity = duk_require_int(ctx, 2);

    ulong_t dst_size = src_size + 600;
    duk_buffer_t dst = duk_push_dynamic_buffer(ctx, dst_size);
    if (NULL==dst) {
        __seterrno(ctx, -ENOMEM); goto error;
    }

    err = BZ2_bzBuffToBuffDecompress(dst, &dst_size, src, src_size, small, verbosity);
    if (err<0) {
        __seterrno(ctx, err); goto error;
    }

    return duk_resize_buffer(ctx, -1, dst_size), 1;
error:
    return duk_push_null(ctx), 1;
}

#ifndef BZ_NO_STDIO

#endif

#include "libbz/libbzf.c"
#include "libbz/libbzn.c"
#endif /* duk_LIBBZ */

int libbz_register(duk_context *ctx)
{
#if duk_LIBBZ
    duk_push_global_object(ctx);
        duk_push_object(ctx);
            libbzf_register(ctx, -1);
            libbzn_register(ctx, -1);
        duk_put_prop_string(ctx, -2, duk_MOD_LIBBZ);
    duk_pop(ctx);
#endif

    return 0;
}

