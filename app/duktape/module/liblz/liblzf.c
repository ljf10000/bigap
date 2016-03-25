static const dukc_func_entry_t liblz_func[] = {

    LIB_FUNC_END
};

static void
liblzf_register(duk_context *ctx, duk_idx_t idx)
{
    duk_put_functions(ctx, idx, liblz_func);
}
