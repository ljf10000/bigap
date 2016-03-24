static const dukc_number_entry_t libbz_number[] = {
    LIB_VALUE(BZ_RUN),
    LIB_VALUE(BZ_FLUSH),
    LIB_VALUE(BZ_FINISH),

    LIB_VALUE(BZ_OK),
    LIB_VALUE(BZ_RUN_OK),
    LIB_VALUE(BZ_FLUSH_OK),
    LIB_VALUE(BZ_FINISH_OK),
    LIB_VALUE(BZ_STREAM_END),
    LIB_VALUE(BZ_SEQUENCE_ERROR),
    LIB_VALUE(BZ_PARAM_ERROR),
    LIB_VALUE(BZ_MEM_ERROR),
    LIB_VALUE(BZ_DATA_ERROR),
    LIB_VALUE(BZ_DATA_ERROR_MAGIC),
    LIB_VALUE(BZ_IO_ERROR),
    LIB_VALUE(BZ_UNEXPECTED_EOF),
    LIB_VALUE(BZ_OUTBUFF_FULL),
    LIB_VALUE(BZ_CONFIG_ERROR),
    
    LIB_VALUE(BZ_MAX_UNUSED),
    
    LIB_VALUE_END
};

static void
libbzn_register(duk_context *ctx, duk_idx_t idx)
{
    duk_put_number_list(ctx, idx, libbz_number);
}

