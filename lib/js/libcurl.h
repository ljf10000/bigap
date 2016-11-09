#ifndef __LIBCURL_H_5cb068d46d6644ecb80a359194ae20d2__
#define __LIBCURL_H_5cb068d46d6644ecb80a359194ae20d2__
#if js_LIBCURL
/******************************************************************************/
#define __LIB_VERSION(A, B, C)  ((A)<<16 | (B)<<8 | (C))
#define __LIB_VERSION_CURRENT   __LIB_VERSION(LIBCURL_VERSION_MAJOR, LIBCURL_VERSION_MINOR, LIBCURL_VERSION_PATCH)
#define LIB_VERSION(A, B, C)    (__LIB_VERSION_CURRENT >= __LIB_VERSION(A, B, C))

static inline int
__get_curl_forms(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct curl_forms *p = (struct curl_forms *)obj; os_objzero(p);
    
    p->option   = js_get_obj_int(ctx, idx, "option");
    p->value    = js_get_obj_string(ctx, idx, "value", NULL);

    return 0;
}

static inline int
__set_curl_forms(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct curl_forms *p = (struct curl_forms *)obj;
    
    js_set_obj_int(ctx, idx, "option", p->option);
    js_set_obj_string(ctx, idx, "value", (char *)p->value);

    return 0;
}

struct curl_httppost_obj {
    struct curl_httppost *post;
    struct curl_httppost *last;
};

static inline int
__get_curl_httppost_obj(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct curl_httppost_obj *p = (struct curl_httppost_obj *)obj; os_objzero(p);
    
    p->post = (struct curl_httppost *)js_get_obj_pointer(ctx, idx, "post");
    p->last = (struct curl_httppost *)js_get_obj_pointer(ctx, idx, "last");

    return 0;
}

static inline int
__set_curl_httppost_obj(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct curl_httppost_obj *p = (struct curl_httppost_obj *)obj;
    
    js_set_obj_pointer(ctx, idx, "post", p->post);
    js_set_obj_pointer(ctx, idx, "last", p->last);

    return 0;
}

struct curl_slist_obj {
    struct curl_slist *list;
};

static inline int
__get_curl_slist_obj(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct curl_slist_obj *p = (struct curl_slist_obj *)obj; os_objzero(p);
    
    p->list = (struct curl_slist *)js_get_obj_pointer(ctx, idx, "list");

    return 0;
}

static inline int
__set_curl_slist_obj(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct curl_slist_obj *p = (struct curl_slist_obj *)obj;
    
    js_set_obj_pointer(ctx, idx, "list", p->list);

    return 0;
}
/******************************************************************************/
#endif
#endif /* __LIBCURL_H_5cb068d46d6644ecb80a359194ae20d2__ */