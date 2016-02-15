static void
libcp_register(duk_context *ctx, duk_idx_t idx)
{
    dukc_pointer_entry_t *p;
    dukc_pointer_entry_t libc_pointer[] = {
        LIB_VALUE(stdin),
        LIB_VALUE(stdout),
        LIB_VALUE(stderr),

        LIB_VALUE_END
    };

    for(p = &libc_pointer[0]; p->key; p++) {
        __set_obj_pointer(ctx, idx, p->key, p->value);
    }
}

