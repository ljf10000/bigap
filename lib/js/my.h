#ifndef __MY_H_86ec299bb02d4b26b31952dd31eef107__
#define __MY_H_86ec299bb02d4b26b31952dd31eef107__
/******************************************************************************/
struct pipexec_cache {
    char *outstring;
    char *errstring;
    int error;
};

static inline int
__set_pipexec_cache(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct pipexec_cache *p = (struct pipexec_cache *)obj;
    
    js_set_obj_string(ctx, idx, "stdout", p->outstring);
    js_set_obj_string(ctx, idx, "stderr", p->errstring);
    js_set_obj_int(ctx, idx, "errno", p->error);

    return 0;
}

static inline int
__get_libval_t(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    libval_t *p = (libval_t *)obj;

    p->size = js_get_obj_uint(ctx, idx, "size");
    p->type = js_get_obj_uint(ctx, idx, "type");
    p->dir  = js_get_obj_uint(ctx, idx, "dir");

    switch(p->type) {
        case LIBVAL_F32:
            p->u.f4 = (float32_t)js_get_obj_number(ctx, idx, "f4");
            break;
        case LIBVAL_I32:
            p->u.i4 = js_get_obj_int(ctx, idx, "i4");
            break;
        case LIBVAL_U32:
            p->u.u4 = js_get_obj_uint(ctx, idx, "u4");
            break;
        case LIBVAL_F64:
            p->u.f8 = js_get_obj_number(ctx, idx, "f8");
            break;
        case LIBVAL_I64:
            p->u.i8 = (int64)js_get_obj_number(ctx, idx, "i8");
            break;
        case LIBVAL_U64:
            p->u.u8 = (uint64)js_get_obj_number(ctx, idx, "u8");
            break;
        case LIBVAL_BUF:
            p->u.b = js_get_obj_buffer(ctx, idx, "b", (duk_size_t *)&p->len);
            break;
        case LIBVAL_STR:
            p->u.s = js_get_obj_string(ctx, idx, "s", (duk_size_t *)&p->len);
            break;
        case LIBVAL_PTR:
            p->u.p = js_get_obj_pointer(ctx, idx, "p");
            break;
        case LIBVAL_VOD: /* down */
        default:
            os_do_nothing();
            break;
    }

    return 0;
}

static inline int
__set_libval_t(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    libval_t *p = (libval_t *)obj;
    
    js_set_obj_uint(ctx, idx, "size", p->size);
    js_set_obj_uint(ctx, idx, "type", p->type);
    js_set_obj_uint(ctx, idx, "dir",  p->dir);
    js_set_obj_uint(ctx, idx, "len",  p->len);

    switch(p->type) {
        case LIBVAL_F32:
            js_set_obj_number(ctx, idx, "f4", (float64_t)p->u.f4);
            break;
        case LIBVAL_I32:
            js_set_obj_int(ctx, idx, "i4", p->u.i4);
            break;
        case LIBVAL_U32:
            js_set_obj_uint(ctx, idx, "i4", p->u.i4);
            break;
        case LIBVAL_F64:
            js_set_obj_number(ctx, idx, "f8", p->u.f8);
            break;
        case LIBVAL_I64:
            js_set_obj_number(ctx, idx, "i8", (float64_t)p->u.i8);
            break;
        case LIBVAL_U64:
            js_set_obj_number(ctx, idx, "u8", (float64_t)p->u.u8);
            break;
        case LIBVAL_BUF:
            os_do_nothing();
            break;
        case LIBVAL_STR:
            os_do_nothing();
            break;
        case LIBVAL_PTR:
            js_set_obj_pointer(ctx, idx, "p", p->u.p);
            break;
        case LIBVAL_VOD: /* down */
        default:
            os_do_nothing();
            break;
    }
    
    return 0;
}

static inline int
__set_oem_t(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    (void)obj;

    idx = duk_normalize_index(ctx, idx);
    js_set_obj_string(ctx, idx, "type",   oem_type_string(__oem_type()));
    
#if 0
    duk_push_object(ctx);
    js_set_obj_string(ctx, -1, "user",       oem_rsync_user);
    js_set_obj_string(ctx, -1, "pwdfile",    oem_rsync_pwdfile);
    js_set_obj_string(ctx, -1, "path",       oem_rsync_path);
    js_set_obj_string(ctx, -1, "server",     oem_rsync_server);
    js_set_obj_string(ctx, -1, "port",       oem_rsync_port);
    js_set_obj_string(ctx, -1, "timeout",    oem_rsync_timeout);
    duk_put_prop_string(ctx, idx, "rsync");
    
    duk_push_object(ctx);
    js_set_obj_string(ctx, -1, "user",       oem_lss_user);
    js_set_obj_string(ctx, -1, "password",   oem_lss_password);
    js_set_obj_string(ctx, -1, "server",     oem_lss_server);
    js_set_obj_string(ctx, -1, "port",       oem_lss_port);
    js_set_obj_string(ctx, -1, "ca",         oem_lss_ca);
    js_set_obj_string(ctx, -1, "key",        oem_lss_key);
    js_set_obj_string(ctx, -1, "cert",       oem_lss_cert);
    duk_put_prop_string(ctx, idx, "lss");

    duk_push_object(ctx);
    js_set_obj_string(ctx, -1, "ca",         oem_lms_ca);
    js_set_obj_string(ctx, -1, "key",        oem_lms_key);
    js_set_obj_string(ctx, -1, "cert",       oem_lms_cert);
    duk_put_prop_string(ctx, idx, "lms");

    duk_push_object(ctx);
    js_set_obj_string(ctx, -1, "ca",         oem_ums_ca);
    js_set_obj_string(ctx, -1, "key",        oem_ums_key);
    js_set_obj_string(ctx, -1, "cert",       oem_ums_cert);
    duk_put_prop_string(ctx, idx, "ums");
#endif

    return 0;
}

/******************************************************************************/
#endif /* __MY_H_86ec299bb02d4b26b31952dd31eef107__ */
