static const dukc_func_entry_t libcurl_func[] = {
    JS_FUNC(curl_easy_init),
    JS_FUNC(curl_easy_cleanup),
    JS_FUNC(curl_easy_perform),
    JS_FUNC(curl_easy_setopt),
    JS_FUNC(curl_formadd),
    JS_FUNC(curl_formfree),
    JS_FUNC(curl_slist_append),
    JS_FUNC(curl_slist_free_all),

    JS_FUNC_END
};

static void
libcurlf_register(duk_context *ctx, duk_idx_t idx)
{
    js_put_functions(ctx, idx, libcurl_func);
}