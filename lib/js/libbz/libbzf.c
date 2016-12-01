STATIC const dukc_func_entry_t js_libbz_func[] = {
    JS_FUNC(bzCompressInit),
    JS_FUNC(bzCompress),
    JS_FUNC(bzCompressEnd),
    JS_FUNC(bzDecompressInit),
    JS_FUNC(bzDecompress),
    JS_FUNC(bzDecompressEnd),
    
    JS_FUNC(compress),
    JS_FUNC(compressEx),
    JS_FUNC(uncompress),
    JS_FUNC(uncompressEx),

#ifndef BZ_NO_STDIO
    JS_FUNC(bzopen),
    JS_FUNC(bzdopen),
    JS_FUNC(bzread),
    JS_FUNC(bzreadEx),
    JS_FUNC(bzwrite),
    JS_FUNC(bzflush),
    JS_FUNC(bzclose),
    JS_FUNC(bzerror),    
#endif

    JS_FUNC_END
};

STATIC void
js_libbzf_register(duk_context *ctx, duk_idx_t idx)
{
    js_put_functions(ctx, idx, js_libbz_func);
}
