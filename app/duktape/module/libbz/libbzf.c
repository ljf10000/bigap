static const dukc_func_entry_t libbz_func[] = {
    LIB_FUNC(bzCompressInit),
    LIB_FUNC(bzCompress),
    LIB_FUNC(bzCompressEnd),
    LIB_FUNC(bzDecompressInit),
    LIB_FUNC(bzDecompress),
    LIB_FUNC(bzDecompressEnd),
    
    LIB_FUNC(bzBuffToBuffCompress),
    LIB_FUNC(bzBuffToBuffCompressEx),
    LIB_FUNC(bzBuffToBuffDecompress),
    LIB_FUNC(bzBuffToBuffDecompressEx),

#ifndef BZ_NO_STDIO
    
#endif

    LIB_FUNC_END
};

static void
libbzf_register(duk_context *ctx, duk_idx_t idx)
{
    duk_put_functions(ctx, idx, libbz_func);
}
