static const dukc_func_entry_t libcurl_func[] = {
    LIB_FUNC(curl_easy_init),
    LIB_FUNC(curl_easy_cleanup),
    LIB_FUNC(curl_easy_perform),
    LIB_FUNC(curl_easy_setopt),
    LIB_FUNC(curl_formadd),
    LIB_FUNC(curl_formfree),
    LIB_FUNC(curl_slist_append),
    LIB_FUNC(curl_slist_free_all),

    LIB_FUNC_END
};

static void
libcurlf_register(duk_context *ctx, duk_idx_t idx)
{
    duk_put_functions(ctx, idx, libcurl_func);
}