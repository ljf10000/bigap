static const dukc_number_entry_t libz_number[] = {
    JS_VALUE(Z_NO_FLUSH),
    JS_VALUE(Z_PARTIAL_FLUSH),
    JS_VALUE(Z_SYNC_FLUSH),
    JS_VALUE(Z_FULL_FLUSH),
    JS_VALUE(Z_FINISH),
    JS_VALUE(Z_BLOCK),
    JS_VALUE(Z_TREES),
    
    JS_VALUE(Z_OK),
    JS_VALUE(Z_STREAM_END),
    JS_VALUE(Z_NEED_DICT),
    JS_VALUE(Z_ERRNO),
    JS_VALUE(Z_STREAM_ERROR),
    JS_VALUE(Z_DATA_ERROR),
    JS_VALUE(Z_MEM_ERROR),
    JS_VALUE(Z_BUF_ERROR),
    JS_VALUE(Z_VERSION_ERROR),
    
    JS_VALUE(Z_NO_COMPRESSION),
    JS_VALUE(Z_BEST_SPEED),
    JS_VALUE(Z_BEST_COMPRESSION),
    JS_VALUE(Z_DEFAULT_COMPRESSION),
    
    JS_VALUE(Z_FILTERED),
    JS_VALUE(Z_HUFFMAN_ONLY),
    JS_VALUE(Z_RLE),
    JS_VALUE(Z_FIXED),
    JS_VALUE(Z_DEFAULT_STRATEGY),
    
    JS_VALUE(Z_BINARY),
    JS_VALUE(Z_TEXT),
    JS_VALUE(Z_ASCII),
    JS_VALUE(Z_UNKNOWN),
    
    JS_VALUE(Z_DEFLATED),
    JS_VALUE(Z_NULL),
    
    JS_VALUE_END
};

static void
libzn_register(duk_context *ctx, duk_idx_t idx)
{
    duk_put_number_list(ctx, idx, libz_number);
}

