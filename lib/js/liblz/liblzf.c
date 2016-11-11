static const dukc_func_entry_t liblz_func[] = {

    JS_FUNC_END
};

static void
liblzf_register(duk_context *ctx, duk_idx_t idx)
{
    js_put_functions(ctx, idx, liblz_func);
}
