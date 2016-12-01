STATIC const dukc_func_entry_t js_liblz_func[] = {
    JS_FUNC_END
};

STATIC void
js_liblzf_register(duk_context *ctx, duk_idx_t idx)
{
    js_put_functions(ctx, idx, js_liblz_func);
}
