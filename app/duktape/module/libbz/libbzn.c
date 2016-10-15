static const dukc_number_entry_t libbz_number[] = {
    JS_VALUE(BZ_RUN),
    JS_VALUE(BZ_FLUSH),
    JS_VALUE(BZ_FINISH),

    JS_VALUE(BZ_OK),
    JS_VALUE(BZ_RUN_OK),
    JS_VALUE(BZ_FLUSH_OK),
    JS_VALUE(BZ_FINISH_OK),
    JS_VALUE(BZ_STREAM_END),
    JS_VALUE(BZ_SEQUENCE_ERROR),
    JS_VALUE(BZ_PARAM_ERROR),
    JS_VALUE(BZ_MEM_ERROR),
    JS_VALUE(BZ_DATA_ERROR),
    JS_VALUE(BZ_DATA_ERROR_MAGIC),
    JS_VALUE(BZ_IO_ERROR),
    JS_VALUE(BZ_UNEXPECTED_EOF),
    JS_VALUE(BZ_OUTBUFF_FULL),
    JS_VALUE(BZ_CONFIG_ERROR),
    
    JS_VALUE(BZ_MAX_UNUSED),
    
    JS_VALUE_END
};

static void
libbzn_register(duk_context *ctx, duk_idx_t idx)
{
    duk_put_number_list(ctx, idx, libbz_number);
}

