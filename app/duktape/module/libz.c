#ifndef __THIS_APP
#define __THIS_APP      js
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     "libz"
#endif

#include "utils.h"
#include "dukc.h"
#if duk_LIBZ
#include "zlib.h"
#include "libz.h"

LIB_PARAM(zlibVersion, 0);
static duk_ret_t
duke_zlibVersion(duk_context *ctx)
{
    char *version = zlibVersion();
    if (NULL==version) {
        duk_push_undefined(ctx);
    } else {
        duk_push_string(ctx, version);
    }

    return 1;
}

LIB_PARAM(deflateInit, 1);
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
    
    return duk_push_pointer(ctx, f), 1;
error:
    os_free(f);
    
    return duk_push_undefined(ctx), 1;
}

LIB_PARAM(deflateInit2, 5);
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
    
    return duk_push_pointer(ctx, f), 1;
error:
    os_free(f);
    
    return duk_push_undefined(ctx), 1;
}

LIB_PARAM(deflate, 2);
static duk_ret_t
duke_deflate(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    int flush = duk_require_int(ctx, 1);

    int err = deflate(f, flush);
    
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(deflateEnd, 1);
static duk_ret_t
duke_deflateEnd(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);

    int err = deflateEnd(f);
    
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(inflateInit, 0);
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
    
    return duk_push_pointer(ctx, f), 1;
error:
    os_free(f);
    
    return duk_push_undefined(ctx), 1;
}

LIB_PARAM(inflateInit2, 1);
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
    
    return duk_push_pointer(ctx, f), 1;
error:
    os_free(f);
    
    return duk_push_undefined(ctx), 1;
}

LIB_PARAM(inflate, 2);
static duk_ret_t
duke_inflate(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    int flush = duk_require_int(ctx, 1);

    int err = inflate(f, flush);
    
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(inflateEnd, 1);
static duk_ret_t
duke_inflateEnd(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);

    int err = inflateEnd(f);
    
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(deflateSetDictionary, 3);
static duk_ret_t
duke_deflateSetDictionary(duk_context *ctx)
{
    void *buf = NULL;
    duk_size_t bsize = 0;
    int err = 0;
    
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    err = duk_require_buffer_or_lstring(ctx, 0, &buf, &bsize);
    if (err<0) {
        __seterrno(ctx, err); goto error;
    }
    
    int size = duk_require_int(ctx, 1);
    if (bsize < size) {
        err = -E2SMALL; __seterrno(ctx, err); goto error;
    }

    err = deflateSetDictionary(f, buf, size);
    
error:
    return duk_push_uint(ctx, err), 1;
}

LIB_PARAM(deflateCopy, 2);
static duk_ret_t
duke_deflateCopy(duk_context *ctx)
{
    z_streamp dst = (z_streamp)duk_require_pointer(ctx, 0);
    z_streamp src = (z_streamp)duk_require_pointer(ctx, 1);

    int err = deflateCopy(dst, src);
    
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(deflateReset, 1);
static duk_ret_t
duke_deflateReset(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);

    int err = deflateReset(f);
    
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(deflateParams, 3);
static duk_ret_t
duke_deflateParams(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    int level   = duk_require_int(ctx, 1);
    int strategy= duk_require_int(ctx, 2);

    int err = deflateParams(f, level, strategy);
    
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(deflateTune, 3);
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

LIB_PARAM(deflateBound, 3);
static duk_ret_t
duke_deflateBound(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    duk_uint_t sourceLen = duk_require_uint(ctx, 1);

    int err = deflateBound(f, sourceLen);
    
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(deflatePending, 3);
static duk_ret_t
duke_deflatePending(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    unsigned *pending = (unsigned *)duk_require_pointer(ctx, 1);
    int *bits = (int *)duk_require_pointer(ctx, 2);

    /*
    * todo: how ???
    */
    int err = deflatePending(f, pending, bits);
    
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(deflatePrime, 3);
static duk_ret_t
duke_deflatePrime(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    int bits = duk_require_int(ctx, 1);
    int value = duk_require_int(ctx, 2);

    int err = deflatePrime(f, bits, value);
    
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(deflateSetHeader, 2);
static duk_ret_t
duke_deflateSetHeader(duk_context *ctx)
{
    gz_header header;
    
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    // todo: get header

    int err = deflateSetHeader(f, &header);
    
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(inflateSetDictionary, 3);
static duk_ret_t
duke_inflateSetDictionary(duk_context *ctx)
{
    void *buf = NULL;
    duk_size_t bsize = 0;
    int err = 0;
    
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    err = duk_require_buffer_or_lstring(ctx, 0, &buf, &bsize);
    if (err<0) {
        __seterrno(ctx, err); goto error;
    }
    
    int size = duk_require_int(ctx, 1);
    if (bsize < size) {
        err = -E2SMALL; __seterrno(ctx, err); goto error;
    }

    err = inflateSetDictionary(f, buf, size);
    
error:
    return duk_push_uint(ctx, err), 1;
}

LIB_PARAM(inflateGetDictionary, 3);
static duk_ret_t
duke_inflateGetDictionary(duk_context *ctx)
{
    int err = 0;
    
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);

    // todo
    
    return duk_push_uint(ctx, err), 1;
}

LIB_PARAM(inflateSync, 1);
static duk_ret_t
duke_inflateSync(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    
    int err = inflateSync(f);
    
    return duk_push_uint(ctx, err), 1;
}

LIB_PARAM(inflateCopy, 2);
static duk_ret_t
duke_inflateCopy(duk_context *ctx)
{
    z_streamp dst = (z_streamp)duk_require_pointer(ctx, 0);
    z_streamp src = (z_streamp)duk_require_pointer(ctx, 1);

    int err = inflateCopy(dst, src);
    
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(inflateReset, 1);
static duk_ret_t
duke_inflateReset(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);

    int err = inflateReset(f);
    
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(inflateReset2, 2);
static duk_ret_t
duke_inflateReset2(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    int windowBits = duk_require_int(ctx, 1);
    
    int err = inflateReset2(f, windowBits);
    
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(inflatePrime, 3);
static duk_ret_t
duke_inflatePrime(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);
    int bits = duk_require_int(ctx, 1);
    int value = duk_require_int(ctx, 2);

    int err = inflatePrime(f, bits, value);
    
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(inflateMark, 1);
static duk_ret_t
duke_inflateMark(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);

    int err = inflateMark(f);
    
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(inflateGetHeader, 1);
static duk_ret_t
duke_inflateGetHeader(duk_context *ctx)
{
    gz_header header;
    
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);

    int err = inflateGetHeader(f, &header);
    // todo: push header
    
    return 1;
}

LIB_PARAM(inflateBackInit, 2);
static duk_ret_t
duke_inflateBackInit(duk_context *ctx)
{
    int windowBits  = duk_require_int(ctx, 0);
    int window      = duk_require_int(ctx, 1);
    
    z_streamp f = (z_streamp)os_malloc(sizeof(*f));
    if (NULL==f) {
        goto error;
    }
    
    int err = inflateBackInit(f, windowBits, window);
    if (err<0) {
        goto error;
    }
    
    return duk_push_pointer(ctx, f), 1;
error:
    os_free(f);
    
    return duk_push_undefined(ctx), 1;
}

LIB_PARAM(inflateBackEnd, 1);
static duk_ret_t
duke_inflateBackEnd(duk_context *ctx)
{
    z_streamp f = (z_streamp)duk_require_pointer(ctx, 0);

    int err = inflateBackEnd(f);
    
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(zlibCompileFlags, 0);
static duk_ret_t
duke_zlibCompileFlags(duk_context *ctx)
{
    duk_uint_t flag = zlibCompileFlags();
    
    return duk_push_uint(ctx, flag), 1;
}

LIB_PARAM(compress, 4);
static duk_ret_t
duke_compress(duk_context *ctx)
{
    int err = 0;

    // todo:
    
    return duk_push_uint(ctx, err), 1;
}

LIB_PARAM(compress2, 5);
static duk_ret_t
duke_compress2(duk_context *ctx)
{
    int err = 0;

    // todo:
    
    return duk_push_uint(ctx, err), 1;
}

LIB_PARAM(compressBound, 1);
static duk_ret_t
duke_compress(duk_context *ctx)
{
    duk_uint_t sourceLen = duk_require_uint(ctx, 0);
    
    int err = compressBound(sourceLen);

    // todo:
    
    return duk_push_uint(ctx, err), 1;
}

LIB_PARAM(uncompress, 4);
static duk_ret_t
duke_uncompress(duk_context *ctx)
{
    int err = 0;

    // todo:
    
    return duk_push_uint(ctx, err), 1;
}

LIB_PARAM(gzdopen, 2);
static duk_ret_t
duke_gzdopen(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    char *mode = (char *)duk_require_string(ctx, 1);

    gzFile *f = gzdopen(fd, mode);
    
    return duk_push_pointer(ctx, f), 1;
}

LIB_PARAM(gzbuffer, 2);
static duk_ret_t
duke_gzbuffer(duk_context *ctx)
{
    gzFile f = (gzFile)duk_require_pointer(ctx, 0);
    duk_uint_t size = duk_require_uint(ctx, 1);

    int err = gzbuffer(f, size);
    
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(gzsetparams, 3);
static duk_ret_t
duke_gzsetparams(duk_context *ctx)
{
    gzFile f = (gzFile)duk_require_pointer(ctx, 0);
    int level = duk_require_int(ctx, 1);
    int strategy= duk_require_int(ctx, 2);

    int err = gzsetparams(f, level, strategy);
    
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(gzread, 3);
static duk_ret_t
duke_gzread(duk_context *ctx)
{
    duk_size_t bsize = 0;
    int err     = 0;
    
    gzFile f = (gzFile)duk_require_pointer(ctx, 0);
    void *buf   = duk_require_buffer_data(ctx, 1, &bsize);
    int size = duk_require_int(ctx, 2);

    if (bsize < size) {
        err = -E2SMALL; __seterrno(ctx, err); goto error;
    }

    err = gzread(f, buf, len);
    
error:
	return duk_push_int(ctx, err), 1;
}

LIB_PARAM(gzwrite, 3);
static duk_ret_t
duke_gzwrite(duk_context *ctx)
{
    duk_size_t bsize = 0;
    void *buf;
    
    gzFile f = (gzFile)duk_require_pointer(ctx, 0);
    
    int err = duk_require_buffer_or_lstring(ctx, 1, &buf, &bsize);
    if (err<0) {
        __seterrno(ctx, err); goto error;
    }

    int size = duk_require_int(ctx, 2);
    if (bsize < size) {
        err = -E2SMALL; __seterrno(ctx, err); goto error;
    }

    err = gzwrite(f, buf, len);
    
error:
	return duk_push_int(ctx, err), 1;
}

// todo: gzprintf

LIB_PARAM(gzputs, 2);
static duk_ret_t
duke_gzputs(duk_context *ctx)
{
    gzFile f = (gzFile)duk_require_pointer(ctx, 0);
    char *s = (char *)duk_require_string(ctx, 1);

    int err = gzputs(f, s);
    
    return duk_push_int(ctx, err), 1;
}


LIB_PARAM(gzgets, 1);
static duk_ret_t
duke_gzgets(duk_context *ctx)
{
    gzFile f = (gzFile)duk_require_pointer(ctx, 0);
    char *line = NULL;

    // todo
    
    return duk_push_string(ctx, line), 1;
}

LIB_PARAM(gzputc, 2);
static duk_ret_t
duke_gzputc(duk_context *ctx)
{
    gzFile f = (gzFile)duk_require_pointer(ctx, 0);
    int ch = duk_require_int(ctx, 1);

    int err = gzputc(f, ch);
    
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(gzgetc, 1);
static duk_ret_t
duke_gzgetc(duk_context *ctx)
{
    gzFile f = (gzFile)duk_require_pointer(ctx, 0);

    int ch = gzgetc(f);
    
    return duk_push_int(ctx, ch), 1;
}

LIB_PARAM(gzungetc, 2);
static duk_ret_t
duke_gzungetc(duk_context *ctx)
{
    gzFile f = (gzFile)duk_require_pointer(ctx, 0);
    int ch = duk_require_int(ctx, 1);

    int err = gzungetc(ch, f);
    
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(gzflush, 2);
static duk_ret_t
duke_gzflush(duk_context *ctx)
{
    gzFile f = (gzFile)duk_require_pointer(ctx, 0);
    int flush = duk_require_int(ctx, 1);

    int err = gzflush(f, flush);
    
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(gzrewind, 1);
static duk_ret_t
duke_gzrewind(duk_context *ctx)
{
    gzFile f = (gzFile)duk_require_pointer(ctx, 0);

    int err = gzrewind(f);
    
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(gzeof, 1);
static duk_ret_t
duke_gzeof(duk_context *ctx)
{
    gzFile f = (gzFile)duk_require_pointer(ctx, 0);

    bool is = !!gzeof(f);
    
    return duk_push_bool(ctx, is), 1;
}

LIB_PARAM(gzdirect, 1);
static duk_ret_t
duke_gzdirect(duk_context *ctx)
{
    gzFile f = (gzFile)duk_require_pointer(ctx, 0);

    bool eof = !!gzdirect(f);
    
    return duk_push_bool(ctx, is), 1;
}

LIB_PARAM(gzclose, 1);
static duk_ret_t
duke_gzclose(duk_context *ctx)
{
    gzFile f = (gzFile)duk_require_pointer(ctx, 0);

    int err = gzclose(f);
    
    return duk_push_bool(ctx, err), 1;
}

LIB_PARAM(gzclose_r, 1);
static duk_ret_t
duke_gzclose_r(duk_context *ctx)
{
    gzFile f = (gzFile)duk_require_pointer(ctx, 0);

    int err = gzclose_r(f);
    
    return duk_push_bool(ctx, err), 1;
}

LIB_PARAM(gzclose_w, 1);
static duk_ret_t
duke_gzclose_w(duk_context *ctx)
{
    gzFile f = (gzFile)duk_require_pointer(ctx, 0);

    int err = gzclose_w(f);
    
    return duk_push_bool(ctx, err), 1;
}

LIB_PARAM(gzerror, 1);
static duk_ret_t
duke_gzerror(duk_context *ctx)
{
    gzFile f = (gzFile)duk_require_pointer(ctx, 0);

    int err = 0;
    char *errstring = gzerror(f, &err);

    // push obj
    return 1;
}

LIB_PARAM(gzclearerr, 1);
static duk_ret_t
duke_gzerror(duk_context *ctx)
{
    gzFile f = (gzFile)duk_require_pointer(ctx, 0);

    return gzclearerr(f), 0;
}

LIB_PARAM(adler32, 3);
static duk_ret_t
duke_adler32(duk_context *ctx)
{
    duk_size_t bsize = 0;
    int err     = 0;
    
    duk_uint_t adler = duk_require_uint(ctx, 0);
    void *buf   = duk_require_buffer_data(ctx, 1, &bsize);
    int size = duk_require_int(ctx, 2);

    if (bsize < size) {
        err = -E2SMALL; __seterrno(ctx, err); goto error;
    }

    err = adler32(adler, buf, len);
    
error:
	return duk_push_int(ctx, err), 1;
}

LIB_PARAM(crc32, 3);
static duk_ret_t
duke_crc32(duk_context *ctx)
{
    duk_size_t bsize = 0;
    int err     = 0;
    
    duk_uint_t adler = duk_require_uint(ctx, 0);
    void *buf   = duk_require_buffer_data(ctx, 1, &bsize);
    int size = duk_require_int(ctx, 2);

    if (bsize < size) {
        err = -E2SMALL; __seterrno(ctx, err); goto error;
    }

    err = crc32(adler, buf, len);
    
error:
	return duk_push_int(ctx, err), 1;
}

#include "libz/libzf.c"
#include "libz/libzn.c"

int libz_register(duk_context *ctx)
{
    duk_push_global_object(ctx);
        duk_push_object(ctx);
            libzf_register(ctx, -1);
            libzn_register(ctx, -1);
        duk_put_prop_string(ctx, -2, duk_MOD_LIBZ);
    duk_pop(ctx);
    
    return 0;
}
#endif

