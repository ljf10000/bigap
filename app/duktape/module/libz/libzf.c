static const dukc_func_entry_t libz_func[] = {
    LIB_FUNC(zlibVersion),
    LIB_FUNC(deflateInit),
    LIB_FUNC(deflateInit2),
    LIB_FUNC(deflate),
    LIB_FUNC(deflateEnd),
    LIB_FUNC(inflateInit),
    LIB_FUNC(inflateInit2),
    LIB_FUNC(inflate),
    LIB_FUNC(inflateEnd),
    LIB_FUNC(deflateSetDictionary),
    LIB_FUNC(deflateCopy),
    LIB_FUNC(deflateReset),
    LIB_FUNC(deflateParams),
    LIB_FUNC(deflateTune),
    LIB_FUNC(deflateBound),
    LIB_FUNC(deflatePending),
    LIB_FUNC(deflatePrime),
    LIB_FUNC(deflateSetHeader),
    LIB_FUNC(inflateSetDictionary),
    LIB_FUNC(inflateGetDictionary),
    LIB_FUNC(inflateSync),
    LIB_FUNC(inflateCopy),
    LIB_FUNC(inflateReset),
    LIB_FUNC(inflateReset2),
    LIB_FUNC(inflatePrime),
    LIB_FUNC(inflateMark),
    LIB_FUNC(inflateGetHeader),
    LIB_FUNC(inflateBackInit),
    LIB_FUNC(inflateBackEnd),
    LIB_FUNC(zlibCompileFlags),
    LIB_FUNC(compress),
    LIB_FUNC(compress2),
    LIB_FUNC(compressBound),
    LIB_FUNC(uncompress),
    LIB_FUNC(gzdopen),
    LIB_FUNC(gzbuffer),
    LIB_FUNC(gzsetparams),
    LIB_FUNC(gzread),
    LIB_FUNC(gzwrite),
    LIB_FUNC(gzputs),
    LIB_FUNC(gzgets),
    LIB_FUNC(gzputc),
    LIB_FUNC(gzgetc),
    LIB_FUNC(gzungetc),
    LIB_FUNC(gzflush),
    LIB_FUNC(gzrewind),
    LIB_FUNC(gzeof),
    LIB_FUNC(gzdirect),
    LIB_FUNC(gzclose),
    LIB_FUNC(gzclose_r),
    LIB_FUNC(gzclose_w),
    LIB_FUNC(gzerror),
    LIB_FUNC(gzclearerr),
    LIB_FUNC(adler32),
    LIB_FUNC(crc32),

    LIB_FUNC_END
};

static void
libzf_register(duk_context *ctx, duk_idx_t idx)
{
    duk_put_functions(ctx, idx, libz_func);
}
