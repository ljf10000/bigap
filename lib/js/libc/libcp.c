static void
libcp_register(duk_context *ctx, duk_idx_t idx)
{
    dukc_pointer_entry_t *p;
    dukc_pointer_entry_t libc_pointer[] = {
        JS_VALUE(stdin),
        JS_VALUE(stdout),
        JS_VALUE(stderr),

        JS_VALUE_END
    };

    for(p = &libc_pointer[0]; p->key; p++) {
        js_set_obj_pointer(ctx, idx, p->key, p->value);
    }
}

