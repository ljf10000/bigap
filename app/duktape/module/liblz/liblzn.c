static const dukc_number_entry_t liblz_number[] = {

    LIB_VALUE_END
};

static void
liblzn_register(duk_context *ctx, duk_idx_t idx)
{
    duk_put_number_list(ctx, idx, liblz_number);
}

