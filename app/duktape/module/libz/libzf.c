static const dukc_func_entry_t libz_func[] = {
    JS_FUNC(zlibVersion),
    JS_FUNC(deflateInit),
    JS_FUNC(deflateInit2),
    JS_FUNC(deflate),
    JS_FUNC(deflateEnd),
    JS_FUNC(inflateInit),
    JS_FUNC(inflateInit2),
    JS_FUNC(inflate),
    JS_FUNC(inflateEnd),
    JS_FUNC(deflateSetDictionary),
    JS_FUNC(deflateCopy),
    JS_FUNC(deflateReset),
    JS_FUNC(deflateParams),
    JS_FUNC(deflateTune),
    JS_FUNC(deflateBound),
#if ZLIB_VERNUM >= 0x1251
    JS_FUNC(deflatePending),
#endif
    JS_FUNC(deflatePrime),
    JS_FUNC(deflateSetHeader),
    JS_FUNC(inflateSetDictionary),
#if ZLIB_VERNUM >= 0x1271
    JS_FUNC(inflateGetDictionary),
#endif
    JS_FUNC(inflateSync),
    JS_FUNC(inflateCopy),
    JS_FUNC(inflateReset),
    JS_FUNC(inflateReset2),
    JS_FUNC(inflatePrime),
    JS_FUNC(inflateMark),
    JS_FUNC(inflateGetHeader),
    JS_FUNC(inflateBackInit),
    JS_FUNC(inflateBackEnd),
    JS_FUNC(zlibCompileFlags),
    JS_FUNC(compress),
    JS_FUNC(compressEx),
    JS_FUNC(compress2),
    JS_FUNC(compress2Ex),
    JS_FUNC(compressBound),
    JS_FUNC(uncompress),
    JS_FUNC(uncompressEx),
    JS_FUNC(gzopen),
    JS_FUNC(gzdopen),
    JS_FUNC(gzsetparams),
    JS_FUNC(gzread),
    JS_FUNC(gzreadEx),
    JS_FUNC(gzwrite),
    JS_FUNC(gzputs),
    JS_FUNC(gzgets),
    JS_FUNC(gzputc),
    JS_FUNC(gzgetc),
    JS_FUNC(gzungetc),
    JS_FUNC(gzflush),
    JS_FUNC(gzrewind),
    JS_FUNC(gzseek),
    JS_FUNC(gztell),
    JS_FUNC(gzeof),
    JS_FUNC(gzdirect),
    JS_FUNC(gzclose),
#if ZLIB_VERNUM >= 0x1235
    JS_FUNC(gzbuffer),
    JS_FUNC(gzoffset),
    JS_FUNC(gzclose_r),
    JS_FUNC(gzclose_w),
#endif
    JS_FUNC(gzerror),
    JS_FUNC(gzclearerr),
    JS_FUNC(adler32),
    JS_FUNC(crc32),

    JS_FUNC_END
};

static void
libzf_register(duk_context *ctx, duk_idx_t idx)
{
    duk_put_functions(ctx, idx, libz_func);
}
