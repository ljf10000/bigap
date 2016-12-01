STATIC const dukc_string_entry_t js_libc_string[] = {
    JS_VALUE_END
};

STATIC void
js_libcs_register(duk_context *ctx, duk_idx_t idx)
{
    const dukc_string_entry_t *p;

    for(p = &js_libc_string[0]; p->key; p++) {
        js_set_obj_string(ctx, idx, p->key, p->value);
    }
}

