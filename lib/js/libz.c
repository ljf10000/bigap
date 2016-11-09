/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      libjs
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     libz
#endif

#define __RUNAS_UNKNOW__

#include "utils.h"
#include "js.h"

#if js_LIBZ
#include <zlib.h>
#include "libz.h"
#include "libxz.h"

JS_PARAM(zlibVersion, 0);
static duk_ret_t
duke_zlibVersion(duk_context *ctx)
{
    return js_push_string(ctx, zlibVersion()), 1;
}

JS_PARAM(deflateInit, 1);
static duk_ret_t
duke_deflateInit(duk_context *ctx)
{
    int level = duk_require_int(ctx, 0);
    z_streamp f = (z_streamp)os_malloc(sizeof(*f));
    if (NULL==f) {
        goto error;
    }
    
    int err = deflateInit(f, level);
    if (err<0) {
        goto error;
    }
    
    return js_push_pointer(ctx, f), 1;
error:
    os_free(f);
    
    return duk_push_null(ctx), 1;
}

JS_PARAM(deflateInit2, 5);
static duk_ret_t
duke_deflateInit2(duk_context *ctx)
{
    int level       = duk_require_int(ctx, 0);
    int method      = duk_require_int(ctx, 1);
    int windowBits  = duk_require_int(ctx, 2);
    int memLevel    = duk_require_int(ctx, 3);
    int strategy    = duk_require_int(ctx, 4);
    
    z_streamp f = (z_streamp)os_malloc(sizeof(*f));
    if (NULL==f) {
        goto error;
    }
    
    int err = deflateInit2(f, level, method, windowBits, memLevel, strategy);
    if (err<0) {
        goto error;
    }
    
    return js_push_pointer(ctx, f), 1;
error:
    os_free(f);
    
    return duk_push_null(ctx), 1;
}

JS_PARAM(deflate, 2);
static duk_ret_t
duke_deflate(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    int flush = duk_require_int(ctx, 1);

    int err = deflate(f, flush);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(deflateEnd, 1);
static duk_ret_t
duke_deflateEnd(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);

    int err = deflateEnd(f); os_free(f);

    return duk_push_int(ctx, err), 1;
}

JS_PARAM(inflateInit, 0);
static duk_ret_t
duke_inflateInit(duk_context *ctx)
{
    z_streamp f = (z_streamp)os_malloc(sizeof(*f));
    if (NULL==f) {
        goto error;
    }
    
    int err = inflateInit(f);
    if (err<0) {
        goto error;
    }
    
    return js_push_pointer(ctx, f), 1;
error:
    os_free(f);
    
    return duk_push_null(ctx), 1;
}

JS_PARAM(inflateInit2, 1);
static duk_ret_t
duke_inflateInit2(duk_context *ctx)
{
    int windowBits  = duk_require_int(ctx, 0);
    
    z_streamp f = (z_streamp)os_malloc(sizeof(*f));
    if (NULL==f) {
        goto error;
    }
    
    int err = inflateInit2(f, windowBits);
    if (err<0) {
        goto error;
    }
    
    return js_push_pointer(ctx, f), 1;
error:
    os_free(f);
    
    return duk_push_null(ctx), 1;
}

JS_PARAM(inflate, 2);
static duk_ret_t
duke_inflate(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    int flush = duk_require_int(ctx, 1);

    int err = inflate(f, flush);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(inflateEnd, 1);
static duk_ret_t
duke_inflateEnd(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);

    int err = inflateEnd(f); os_free(f);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(deflateSetDictionary, 2);
static duk_ret_t
duke_deflateSetDictionary(duk_context *ctx)
{
    duk_buffer_t buf = NULL;
    duk_size_t bsize = 0;
    int err = 0;
    
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    err = duk_require_buffer_or_lstring(ctx, 1, &buf, &bsize);
    if (err<0) {
        __js_seterrno(ctx, err); goto error;
    }

    err = deflateSetDictionary(f, buf, bsize);
    
error:
    return duk_push_uint(ctx, err), 1;
}

JS_PARAM(deflateCopy, 2);
static duk_ret_t
duke_deflateCopy(duk_context *ctx)
{
    z_streamp dst = (z_streamp)duk_require_pointer(ctx, 0);
    z_streamp src = (z_streamp)duk_require_pointer(ctx, 1);

    int err = deflateCopy(dst, src);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(deflateReset, 1);
static duk_ret_t
duke_deflateReset(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);

    int err = deflateReset(f);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(deflateParams, 3);
static duk_ret_t
duke_deflateParams(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    int level   = duk_require_int(ctx, 1);
    int strategy= duk_require_int(ctx, 2);

    int err = deflateParams(f, level, strategy);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(deflateTune, 3);
static duk_ret_t
duke_deflateTune(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    int good_length = duk_require_int(ctx, 1);
    int max_lazy    = duk_require_int(ctx, 2);
    int nice_length = duk_require_int(ctx, 3);
    int max_chain   = duk_require_int(ctx, 4);

    int err = deflateTune(f, good_length, max_lazy, nice_length, max_chain);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(deflateBound, 3);
static duk_ret_t
duke_deflateBound(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    duk_uint_t sourceLen = duk_require_uint(ctx, 1);

    int err = deflateBound(f, sourceLen);
    
    return duk_push_int(ctx, err), 1;
}


#if ZLIB_VERNUM >= 0x1251
JS_PARAM(deflatePending, 1);
static duk_ret_t
duke_deflatePending(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    unsigned int pending;
    int bits;

    int err = deflatePending(f, &pending, &bits);
    if (err<0) {
        return duk_push_null(ctx, 1);
    }
    
    duk_push_object(ctx);
        js_set_obj_uint(ctx, -1, "pending", pending);
        js_set_obj_int(ctx, -1, "bits", bits);
    
    return 1;
}
#endif

JS_PARAM(deflatePrime, 3);
static duk_ret_t
duke_deflatePrime(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    int bits = duk_require_int(ctx, 1);
    int value = duk_require_int(ctx, 2);

    int err = deflatePrime(f, bits, value);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(deflateSetHeader, 2);
static duk_ret_t
duke_deflateSetHeader(duk_context *ctx)
{
    gz_header header;
    
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    __get_gz_header(ctx, 1, &header);
    
    int err = deflateSetHeader(f, &header);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(inflateSetDictionary, 2);
static duk_ret_t
duke_inflateSetDictionary(duk_context *ctx)
{
    duk_buffer_t buf = NULL;
    duk_size_t bsize = 0;
    int err = 0;
    
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    err = duk_require_buffer_or_lstring(ctx, 1, &buf, &bsize);
    if (err<0) {
        __js_seterrno(ctx, err); goto error;
    }
    
    err = inflateSetDictionary(f, buf, bsize);
    
error:
    return duk_push_uint(ctx, err), 1;
}

#if ZLIB_VERNUM >= 0x1271
JS_PARAM(inflateGetDictionary, 3);
static duk_ret_t
duke_inflateGetDictionary(duk_context *ctx)
{
    int err = 0;
    duk_uint_t size = 0;
    
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    inflateGetDictionary(f, NULL, &size);
    
    duk_buffer_t buf = js_push_dynamic_buffer(ctx, size);
    if (NULL==buf) {
        err = __js_seterrno(ctx, -ENOMEM); goto error;
    }

    err = inflateGetDictionary(f, buf, size);
    if (err<0) {
        duk_pop(ctx); goto error;
    }

    return 1;
error:
    return duk_push_null(ctx), 1;
}
#endif

JS_PARAM(inflateSync, 1);
static duk_ret_t
duke_inflateSync(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    
    int err = inflateSync(f);
    
    return duk_push_uint(ctx, err), 1;
}

JS_PARAM(inflateCopy, 2);
static duk_ret_t
duke_inflateCopy(duk_context *ctx)
{
    z_streamp dst = (z_streamp)duk_require_pointer(ctx, 0);
    z_streamp src = (z_streamp)duk_require_pointer(ctx, 1);

    int err = inflateCopy(dst, src);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(inflateReset, 1);
static duk_ret_t
duke_inflateReset(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);

    int err = inflateReset(f);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(inflateReset2, 2);
static duk_ret_t
duke_inflateReset2(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    int windowBits = duk_require_int(ctx, 1);
    
    int err = inflateReset2(f, windowBits);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(inflatePrime, 3);
static duk_ret_t
duke_inflatePrime(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    int bits = duk_require_int(ctx, 1);
    int value = duk_require_int(ctx, 2);

    int err = inflatePrime(f, bits, value);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(inflateMark, 1);
static duk_ret_t
duke_inflateMark(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);

    int err = inflateMark(f);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(inflateGetHeader, 1);
static duk_ret_t
duke_inflateGetHeader(duk_context *ctx)
{
    gz_header header;
    
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);

    int err = inflateGetHeader(f, &header);
    
    return js_obj_push(ctx, __set_gz_header, &header), 1;
}

JS_PARAM(inflateBackInit, 2);
static duk_ret_t
duke_inflateBackInit(duk_context *ctx)
{
    int windowBits  = duk_require_int(ctx, 0);
    byte *window = (byte *)duk_require_pointer(ctx, 1);
    
    z_streamp f = (z_streamp)os_malloc(sizeof(*f));
    if (NULL==f) {
        goto error;
    }
    
    int err = inflateBackInit(f, windowBits, window);
    if (err<0) {
        goto error;
    }
    
    return js_push_pointer(ctx, f), 1;
error:
    os_free(f);
    
    return duk_push_null(ctx), 1;
}

JS_PARAM(inflateBackEnd, 1);
static duk_ret_t
duke_inflateBackEnd(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);

    int err = inflateBackEnd(f); os_free(f);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(zlibCompileFlags, 0);
static duk_ret_t
duke_zlibCompileFlags(duk_context *ctx)
{
    duk_uint_t flag = zlibCompileFlags();
    
    return duk_push_uint(ctx, flag), 1;
}

JS_PARAM(compress, 2);
static duk_ret_t
duke_compress(duk_context *ctx)
{
    int err = 0;
    duk_size_t src_size = 0;
    duk_size_t dst_size = 0;

    duk_buffer_t dst = duk_require_buffer_data(ctx, 0, &dst_size);
    duk_buffer_t src = duk_require_buffer_data(ctx, 1, &src_size);
    ulong_t dst_len = compressBound(src_size);

    if (dst_len>dst_size) {
        err = __js_seterrno(ctx, -ENOSPACE); goto error;
    }
    
    err = compress(dst, &dst_len, src, src_size);
    if (err<0) {
        __js_seterrno(ctx, err); goto error;
    }

    err = dst_len;    
error:
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(compressEx, 1);
static duk_ret_t
duke_compressEx(duk_context *ctx)
{
    int err = 0;
    duk_size_t src_size = 0;
    duk_buffer_t src = duk_require_buffer_data(ctx, 0, &src_size);

    ulong_t dst_size = compressBound(src_size);
    duk_buffer_t dst = duk_push_dynamic_buffer(ctx, dst_size);
    if (NULL==dst) {
        __js_seterrno(ctx, -ENOMEM); goto error;
    }

    err = compress(dst, &dst_size, src, src_size);
    if (err<0) {
        __js_seterrno(ctx, err); goto error;
    }

    return duk_resize_buffer(ctx, -1, dst_size), 1;
error:
    return duk_push_null(ctx), 1;
}

JS_PARAM(compress2, 3);
static duk_ret_t
duke_compress2(duk_context *ctx)
{
    int err = 0;
    duk_size_t src_size = 0;
    duk_size_t dst_size = 0;

    duk_buffer_t dst = duk_require_buffer_data(ctx, 0, &dst_size);
    duk_buffer_t src = duk_require_buffer_data(ctx, 1, &src_size);
    int level = duk_require_int(ctx, 2);
    ulong_t dst_len = compressBound(src_size);

    if (dst_len>dst_size) {
        err = __js_seterrno(ctx, -ENOSPACE); goto error;
    }
    
    err = compress2(dst, &dst_len, src, src_size, level);
    if (err<0) {
        __js_seterrno(ctx, err); goto error;
    }

    err = dst_len;    
error:
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(compress2Ex, 2);
static duk_ret_t
duke_compress2Ex(duk_context *ctx)
{
    int err = 0;
    duk_size_t src_size = 0;

    duk_buffer_t src = duk_require_buffer_data(ctx, 0, &src_size);
    int level = duk_require_int(ctx, 1);
    ulong_t dst_size = compressBound(src_size);

    duk_buffer_t dst = duk_push_dynamic_buffer(ctx, dst_size);
    if (NULL==dst) {
        __js_seterrno(ctx, -ENOMEM); goto error;
    }

    err = compress2(dst, &dst_size, src, src_size, level);
    if (err<0) {
        __js_seterrno(ctx, err); goto error;
    }

    return duk_resize_buffer(ctx, -1, dst_size), 1;
error:
    return duk_push_null(ctx), 1;
}

JS_PARAM(compressBound, 1);
static duk_ret_t
duke_compressBound(duk_context *ctx)
{
    duk_uint_t sourceLen = duk_require_uint(ctx, 0);
    
    duk_uint_t size = compressBound(sourceLen);

    return duk_push_uint(ctx, size), 1;
}

JS_PARAM(uncompress, 2);
static duk_ret_t
duke_uncompress(duk_context *ctx)
{
    int err = 0;
    duk_size_t src_size = 0;
    duk_size_t dst_size = 0;

    duk_buffer_t dst = duk_require_buffer_data(ctx, 0, &dst_size);
    duk_buffer_t src = duk_require_buffer_data(ctx, 1, &src_size);
    ulong_t dst_len = compressBound(src_size);

    if (dst_len>dst_size) {
        err = __js_seterrno(ctx, -ENOSPACE); goto error;
    }
    
    err = uncompress(dst, &dst_len, src, src_size);
    if (err<0) {
        __js_seterrno(ctx, err); goto error;
    }

    err = dst_len;    
error:
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(uncompressEx, 1);
static duk_ret_t
duke_uncompressEx(duk_context *ctx)
{
    int err = 0;
    duk_size_t src_size = 0;

    duk_buffer_t src = duk_require_buffer_data(ctx, 0, &src_size);
    ulong_t dst_size = compressBound(src_size);

    duk_buffer_t dst = js_push_dynamic_buffer(ctx, dst_size);
    if (NULL==dst) {
        __js_seterrno(ctx, -ENOMEM); goto error;
    }

    err = uncompress(dst, &dst_size, src, src_size);
    if (err<0) {
        __js_seterrno(ctx, err); goto error;
    }
    duk_resize_buffer(ctx, -1, dst_size);
    
    return 1;
error:
    return duk_push_null(ctx), 1;
}

#define HFILE   gzFile

JS_PARAM(gzopen, 2);
static duk_ret_t
duke_gzopen(duk_context *ctx)
{
    return xzopen(ctx, gzopen);
}

JS_PARAM(gzdopen, 2);
static duk_ret_t
duke_gzdopen(duk_context *ctx)
{
    return xzdopen(ctx, gzdopen);
}

JS_PARAM(gzsetparams, 3);
static duk_ret_t
duke_gzsetparams(duk_context *ctx)
{
    HFILE f = (HFILE)duk_require_pointer(ctx, 0);
    int level = duk_require_int(ctx, 1);
    int strategy= duk_require_int(ctx, 2);

    int err = gzsetparams(f, level, strategy);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(gzread, 2);
static duk_ret_t
duke_gzread(duk_context *ctx)
{
    return xzread(ctx, gzread);
}

JS_PARAM(gzreadEx, 2);
static duk_ret_t
duke_gzreadEx(duk_context *ctx)
{
    return xzreadEx(ctx, gzread);
}

JS_PARAM(gzwrite, 2);
static duk_ret_t
duke_gzwrite(duk_context *ctx)
{
    return xzwrite(ctx, gzwrite);
}

JS_PARAM(gzputs, 2);
static duk_ret_t
duke_gzputs(duk_context *ctx)
{
    HFILE f = (HFILE)duk_require_pointer(ctx, 0);
    char *s = (char *)duk_require_string(ctx, 1);

    int err = gzputs(f, s);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(gzgets, 2);
static duk_ret_t
duke_gzgets(duk_context *ctx)
{
    duk_size_t bsize = 0;

    HFILE f = (HFILE)duk_require_pointer(ctx, 0);
    duk_buffer_t buf = duk_require_buffer_data(ctx, 1, &bsize);

    char *s = gzgets(f, buf, bsize);
    int len = os_strlen(s);
    
	return duk_push_int(ctx, len), 1;
}

JS_PARAM(gzputc, 2);
static duk_ret_t
duke_gzputc(duk_context *ctx)
{
    HFILE f = (HFILE)duk_require_pointer(ctx, 0);
    int ch = duk_require_int(ctx, 1);

    int err = gzputc(f, ch);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(gzgetc, 1);
static duk_ret_t
duke_gzgetc(duk_context *ctx)
{
    HFILE f = (HFILE)duk_require_pointer(ctx, 0);

    int ch = gzgetc(f);
    
    return duk_push_int(ctx, ch), 1;
}

JS_PARAM(gzungetc, 2);
static duk_ret_t
duke_gzungetc(duk_context *ctx)
{
    HFILE f = (HFILE)duk_require_pointer(ctx, 0);
    int ch = duk_require_int(ctx, 1);

    int err = gzungetc(ch, f);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(gzflush, 2);
static duk_ret_t
duke_gzflush(duk_context *ctx)
{
    HFILE f = (HFILE)duk_require_pointer(ctx, 0);
    int flush = duk_require_int(ctx, 1);

    int err = gzflush(f, flush);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(gzrewind, 1);
static duk_ret_t
duke_gzrewind(duk_context *ctx)
{
    HFILE f = (HFILE)duk_require_pointer(ctx, 0);

    int err = gzrewind(f);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(gzseek, 3);
static duk_ret_t
duke_gzseek(duk_context *ctx)
{
    HFILE f = (HFILE)duk_require_pointer(ctx, 0);
    int offset = duk_require_int(ctx, 1);
    int where = duk_require_int(ctx, 2);

    int err = gzseek(f, offset, where);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(gztell, 1);
static duk_ret_t
duke_gztell(duk_context *ctx)
{
    HFILE f = (HFILE)duk_require_pointer(ctx, 0);

    int err = gztell(f);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(gzeof, 1);
static duk_ret_t
duke_gzeof(duk_context *ctx)
{
    HFILE f = (HFILE)duk_require_pointer(ctx, 0);

    bool is = !!gzeof(f);
    
    return duk_push_bool(ctx, is), 1;
}

JS_PARAM(gzdirect, 1);
static duk_ret_t
duke_gzdirect(duk_context *ctx)
{
    HFILE f = (HFILE)duk_require_pointer(ctx, 0);

    bool is = !!gzdirect(f);
    
    return duk_push_bool(ctx, is), 1;
}

JS_PARAM(gzclose, 1);
static duk_ret_t
duke_gzclose(duk_context *ctx)
{
    HFILE f = (HFILE)duk_require_pointer(ctx, 0);

    int err = gzclose(f);
    
    return duk_push_bool(ctx, err), 1;
}

#if ZLIB_VERNUM >= 0x1235
JS_PARAM(gzbuffer, 2);
static duk_ret_t
duke_gzbuffer(duk_context *ctx)
{
    HFILE f = (HFILE)duk_require_pointer(ctx, 0);
    duk_uint_t size = duk_require_uint(ctx, 1);

    int err = gzbuffer(f, size);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(gzoffset, 1);
static duk_ret_t
duke_gzoffset(duk_context *ctx)
{
    HFILE f = (HFILE)duk_require_pointer(ctx, 0);

    int err = gzoffset(f);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(gzclose_r, 1);
static duk_ret_t
duke_gzclose_r(duk_context *ctx)
{
    HFILE f = (HFILE)duk_require_pointer(ctx, 0);

    int err = gzclose_r(f);
    
    return duk_push_bool(ctx, err), 1;
}

JS_PARAM(gzclose_w, 1);
static duk_ret_t
duke_gzclose_w(duk_context *ctx)
{
    HFILE f = (HFILE)duk_require_pointer(ctx, 0);

    int err = gzclose_w(f);
    
    return duk_push_bool(ctx, err), 1;
}
#endif

JS_PARAM(gzerror, 1);
static duk_ret_t
duke_gzerror(duk_context *ctx)
{
    return xzerror(ctx, gzerror);
}

JS_PARAM(gzclearerr, 1);
static duk_ret_t
duke_gzclearerr(duk_context *ctx)
{
    HFILE f = (HFILE)duk_require_pointer(ctx, 0);

    return gzclearerr(f), 0;
}

JS_PARAM(adler32, 3);
static duk_ret_t
duke_adler32(duk_context *ctx)
{
    duk_size_t bsize = 0;
    int err     = 0;
    
    duk_uint_t adler = duk_require_uint(ctx, 0);
    duk_buffer_t buf = duk_require_buffer_data(ctx, 1, &bsize);
    int size = duk_require_int(ctx, 2);

    if (bsize < size) {
        err = __js_seterrno(ctx, -E2SMALL); goto error;
    }

    err = adler32(adler, buf, size);
    
error:
	return duk_push_int(ctx, err), 1;
}

JS_PARAM(crc32, 3);
static duk_ret_t
duke_crc32(duk_context *ctx)
{
    duk_size_t bsize = 0;
    int err     = 0;
    
    duk_uint_t adler = duk_require_uint(ctx, 0);
    duk_buffer_t buf = duk_require_buffer_data(ctx, 1, &bsize);
    int size = duk_require_int(ctx, 2);

    if (bsize < size) {
        err = __js_seterrno(ctx, -E2SMALL); goto error;
    }

    err = crc32(adler, buf, size);
    
error:
	return duk_push_int(ctx, err), 1;
}

#include "libz/libzf.c"
#include "libz/libzn.c"

int js_libz_register(duk_context *ctx)
{
    duk_push_global_object(ctx);
        duk_push_object(ctx);
            libzf_register(ctx, -1);
            libzn_register(ctx, -1);
        duk_put_prop_string(ctx, -2, js_MOD_LIBZ);
    duk_pop(ctx);

    debug_ok("register libz ok.");

    return 0;
}
#endif /* js_LIBZ */

