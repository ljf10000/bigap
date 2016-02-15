static const dukc_string_entry_t libc_string[] = {
    LIB_VALUE_END
};

static void
libcs_register(duk_context *ctx, duk_idx_t idx)
{
    const dukc_string_entry_t *p;

    for(p = &libc_string[0]; p->key; p++) {
        __set_obj_string(ctx, idx, p->key, p->value);
    }
}

