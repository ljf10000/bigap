#ifndef __LIBCURL_H_5cb068d46d6644ecb80a359194ae20d2__
#define __LIBCURL_H_5cb068d46d6644ecb80a359194ae20d2__
#if js_LIBCURL
/******************************************************************************/
extern int
libcurl_get_curl_forms(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libcurl_set_curl_forms(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libcurl_get_curl_httppost_obj(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libcurl_set_curl_httppost_obj(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libcurl_get_curl_slist_obj(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libcurl_set_curl_slist_obj(duk_context *ctx, duk_idx_t idx, duk_object_t obj);
/******************************************************************************/
#endif
#endif /* __LIBCURL_H_5cb068d46d6644ecb80a359194ae20d2__ */