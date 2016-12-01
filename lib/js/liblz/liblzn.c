STATIC const dukc_number_entry_t js_liblz_number[] = {
    JS_VALUE_END
};

STATIC void
js_liblzn_register(duk_context *ctx, duk_idx_t idx)
{
    duk_put_number_list(ctx, idx, js_liblz_number);
}

