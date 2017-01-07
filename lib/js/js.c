/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      libjs
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     js
#endif

#ifndef __BUSYBOX__
#define __LIB__
#endif

#define __DEAMON__

#include "utils.h"
#include "js.h"

akid_t  __THIS_DEBUG;
akid_t  __THIS_JDEBUG;

LIB_INITER;
/******************************************************************************/
int
duk_require_buffer_or_lstring(duk_context *ctx, duk_idx_t idx, duk_buffer_t *pbuf, duk_size_t *psize)
{
    duk_buffer_t buf = NULL;
    
    int type = duk_get_type(ctx, idx);
    
    if (DUK_TYPE_BUFFER==type) {
        buf = duk_get_buffer_data(ctx, idx, psize);
    }
    else if (DUK_TYPE_STRING==type) {
        buf = (duk_buffer_t)duk_get_lstring(ctx, idx, psize);
    }
    else {
        return -EFORMAT;
    }

    *pbuf = buf;
    
    return 0;
}

void
js_put_functions(duk_context *ctx, duk_idx_t obj_index, const dukc_func_entry_t *funcs)
{
	const dukc_func_entry_t *ent;

	obj_index = duk_require_normalize_index(ctx, obj_index);
	for (ent = funcs; ent && ent->key; ent++) {
#if js_LIGHT_FUNC
        duk_push_c_lightfunc(ctx, ent->value, ent->nargs, ent->nargs, ent->version);
		duk_put_prop_string(ctx, obj_index, ent->key);
#else
		duk_push_c_function(ctx, ent->value, ent->nargs);
		duk_put_prop_string(ctx, obj_index, ent->key);

        /*
        * save function's api version
        */
		duk_get_prop_string(ctx, obj_index, ent->key);
		duk_idx_t idx = duk_require_normalize_index(ctx, -1);
		{
			duk_push_uint(ctx, ent->version);
			duk_put_prop_string(ctx, idx, "version");
		}
		duk_pop(ctx);
#endif
	}
}

int 
js_push_error(duk_context *ctx, int err)
{
    if (err<0) {
        js_seterrno(ctx);
    }

    duk_push_int(ctx, err);
    
    return err;
}

duk_buffer_t
js_push_pointer(duk_context *ctx, duk_pointer_t pointer)
{
    if (pointer) {
        duk_push_pointer(ctx, pointer);
    } else {
        duk_push_null(ctx);
    }

    return pointer;
}

duk_string_t
js_push_lstring(duk_context *ctx, duk_string_t s, duk_size_t len)
{
    if (is_good_str(s) && len>0) {
        return duk_push_lstring(ctx, s, len);
    } else {
        return duk_push_null(ctx), NULL;
    }
}

duk_string_t
js_push_string(duk_context *ctx, duk_string_t s)
{
    if (s) {
        return duk_push_string(ctx, s);
    } else {
        return duk_push_null(ctx), NULL;
    }
}

duk_buffer_t
js_push_buffer(duk_context *ctx, duk_size_t size, bool dynamic)
{
    if (size>0) {
        return duk_push_buffer(ctx, size, dynamic);
    } else {
        return duk_push_null(ctx), NULL;
    }
}

duk_buffer_t
js_push_fixed_buffer(duk_context *ctx, duk_size_t size)
{
    if (size>0) {
        return duk_push_fixed_buffer(ctx, size);
    } else {
        return duk_push_null(ctx), NULL;
    }
}

duk_buffer_t
js_push_dynamic_buffer(duk_context *ctx, duk_size_t size)
{
    if (size>0) {
        return duk_push_dynamic_buffer(ctx, size);
    } else {
        return duk_push_null(ctx), NULL;
    }
}

void
js_pcall(duk_context *ctx, duk_idx_t idx, int (*push)(void))
{
    idx = duk_normalize_index(ctx, idx);
    
    duk_dup(ctx, idx);
    duk_pcall(ctx, (*push)());
    duk_pop(ctx);
}

int
js_get_a_prop_string(
    duk_context *ctx, 
    bool auto_create, 
    duk_idx_t idx, 
    int *level, 
    char **keys,
    int count
)
{
    int i;

    if (DUK_INVALID_INDEX==idx) { // global
        duk_push_global_object(ctx); (*level)++;
        idx = -1;
    }
    
    idx = duk_normalize_index(ctx, idx);

    for (i=0; i<count; i++, idx = -1) {
        char *k = keys[i];
        bool exist = duk_get_prop_string(ctx, idx, k); (*level)++;
        if (false==exist) {
            if (false==auto_create) {
                return -EFORMAT;
            }

            duk_pop(ctx);                       // pop undefined,   empty
            duk_push_object(ctx);               // push object      object
            duk_put_prop_string(ctx, idx, k);   // set name         empty
            duk_get_prop_string(ctx, idx, k);   // get name         object
        }
    }
    
    return 0;
}

int
js_get_v_prop_string(
    duk_context *ctx, 
    bool auto_create, 
    duk_idx_t idx, 
    int *level, 
    va_list args
)
{
    int i, count = 0;
    char *k;

    if (DUK_INVALID_INDEX==idx) { // global
        duk_push_global_object(ctx); (*level)++;
        idx = -1;
    }

    idx = duk_normalize_index(ctx, idx);
    
    while((k = va_arg(args, char *))) {
        bool exist = duk_get_prop_string(ctx, idx, k); (*level)++;
        if (false==exist) {
            if (false==auto_create) {
                return -EFORMAT;
            }

            duk_pop(ctx);                       // pop undefined,   empty
            duk_push_object(ctx);               // push object      object
            duk_put_prop_string(ctx, idx, k);   // set name         empty
            duk_get_prop_string(ctx, idx, k);   // get name         object
        }

        idx = -1;
    }

    return 0;
}

int
js_get_prop_string(duk_context *ctx, bool auto_create, duk_idx_t idx, int *level, ...)
{
    va_list args;
    
    va_start(args, level);
    int err = js_get_v_prop_string(ctx, auto_create, idx, level, args);
    va_end(args);

    return err;
}

#define js_get_obj_field(_ctx, _idx, _type, _key) ({ \
    duk_##_type##_t v = (duk_##_type##_t)0;         \
                                                    \
    duk_get_prop_string(_ctx, _idx, _key);          \
    if (duk_is_##_type(_ctx, -1)) {                 \
        v = duk_get_##_type(_ctx, -1);              \
    }                                               \
    duk_pop(ctx);                                   \
                                                    \
    v;                                              \
})

#define js_set_obj_field(_ctx, _idx, _type, _key, _v)   do{ \
    duk_idx_t __idx = duk_normalize_index(_ctx, _idx);      \
    duk_push_##_type(_ctx, _v);                             \
    duk_put_prop_string(_ctx, __idx, _key);                 \
}while(0)

bool
js_get_obj_bool(duk_context *ctx, duk_idx_t idx, duk_string_t k)
{
    return js_get_obj_field(ctx, idx, bool, k);
}

void
js_set_obj_bool(duk_context *ctx, duk_idx_t idx, duk_string_t k, duk_bool_t v)
{
    js_set_obj_field(ctx, idx, bool, k, v);
}

int
js_get_obj_int(duk_context *ctx, duk_idx_t idx, duk_string_t k)
{
    return js_get_obj_field(ctx, idx, int, k);
}

void
js_set_obj_int(duk_context *ctx, duk_idx_t idx, duk_string_t k, duk_int_t v)
{
    js_set_obj_field(ctx, idx, int, k, v);
}

duk_uint_t
js_get_obj_uint(duk_context *ctx, duk_idx_t idx, duk_string_t k)
{
    return js_get_obj_field(ctx, idx, uint, k);
}

void
js_set_obj_uint(duk_context *ctx, duk_idx_t idx, duk_string_t k, duk_uint_t v)
{
    js_set_obj_field(ctx, idx, uint, k, v);
}

duk_number_t
js_get_obj_number(duk_context *ctx, duk_idx_t idx, duk_string_t k)
{
    return js_get_obj_field(ctx, idx, number, k);
}

void
js_set_obj_number(duk_context *ctx, duk_idx_t idx, duk_string_t k, duk_number_t v)
{
    js_set_obj_field(ctx, idx, number, k, v);
}

duk_pointer_t
js_get_obj_pointer(duk_context *ctx, duk_idx_t idx, duk_string_t k)
{
    return js_get_obj_field(ctx, idx, pointer, k);
}

void
js_set_obj_pointer(duk_context *ctx, duk_idx_t idx, duk_string_t k, duk_pointer_t v)
{
    js_set_obj_field(ctx, idx, pointer, k, v);
}

char *
js_get_obj_string(duk_context *ctx, duk_idx_t idx, duk_string_t k, duk_size_t *plen)
{
    char *v = NULL;
    
    duk_get_prop_string(ctx, idx, k);
    if (duk_is_string(ctx, -1)) {
        v = (char *)duk_get_lstring(ctx, -1, plen);
    }
    duk_pop(ctx);

    return v;
}

char *
js_copy_obj_string(duk_context *ctx, duk_idx_t idx, duk_string_t k, char *buffer, int size)
{
    duk_size_t len = 0;
    char *v = js_get_obj_string(ctx, idx, k, &len);
    
    if (v && (size-1) >= len ) {
        v = os_strmcpy(buffer, v, len);
    } else {
        v = NULL;
    }

    return v;
}

void
js_set_obj_string(duk_context *ctx, duk_idx_t idx, duk_string_t k, char * v)
{
    idx = duk_normalize_index(ctx, idx);
    
    js_push_string(ctx, v);
    duk_put_prop_string(ctx, idx, k);
}

duk_buffer_t
js_get_obj_buffer(duk_context *ctx, duk_idx_t idx, duk_string_t k, duk_size_t *plen)
{
    duk_buffer_t v = NULL;
    
    duk_get_prop_string(ctx, idx, k);
    if (duk_is_buffer(ctx, -1)) {
        v = duk_get_buffer_data(ctx, -1, plen);
    }
    duk_pop(ctx);

    return v;
}

char *
js_copy_obj_buffer(duk_context *ctx, duk_idx_t idx, duk_string_t k, duk_buffer_t buffer, duk_size_t size)
{
    duk_size_t len = 0;
    duk_buffer_t v = js_get_obj_buffer(ctx, idx, k, &len);
    if (v && size>=len) {
        os_memcpy(buffer, v, len);
        v = buffer;
    } else {
        v = NULL;
    }

    return v;
}

void
js_set_obj_buffer(duk_context *ctx, duk_idx_t idx, duk_string_t k, duk_buffer_t v, duk_size_t size)
{
    idx = duk_normalize_index(ctx, idx);
    
    duk_buffer_t buf = js_push_dynamic_buffer(ctx, size);
    os_memcpy(buf, v, size);
    duk_put_prop_string(ctx, idx, k);
}

#define js_get_array_field(_ctx, _idx, _type, _aidx) ({ \
    duk_##_type##_t v = (duk_##_type##_t)0;             \
                                                        \
    duk_get_prop_index(_ctx, _idx, _aidx);              \
    if (duk_is_##_type(_ctx, -1)) {                     \
        v = duk_get_##_type(_ctx, -1);                  \
    }                                                   \
    duk_pop(_ctx);                                      \
                                                        \
    v;                                                  \
})

#define js_set_array_field(_ctx, _idx, _type, _aidx, _v)  do{   \
    duk_idx_t __idx = duk_normalize_index(_ctx, _idx);          \
    duk_push_##_type(_ctx, _v);                                 \
    duk_put_prop_index(_ctx, __idx, _aidx);                     \
}while(0)

bool
js_get_array_bool(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx)
{
    return js_get_array_field(ctx, idx, bool, aidx);
}

void
js_set_array_bool(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_bool_t v)
{
    js_set_array_field(ctx, idx, bool, aidx, v);
}

int
js_get_array_int(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx)
{
    return js_get_array_field(ctx, idx, int, aidx);
}

void
js_set_array_int(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_int_t v)
{
    js_set_array_field(ctx, idx, int, aidx, v);
}

duk_uint_t
js_get_array_uint(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx)
{
    return js_get_array_field(ctx, idx, uint, aidx);
}

void
js_set_array_uint(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_uint_t v)
{
    js_set_array_field(ctx, idx, uint, aidx, v);
}

duk_number_t
js_get_array_number(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx)
{
    return js_get_array_field(ctx, idx, number, aidx);
}

void
js_set_array_number(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_number_t v)
{
    js_set_array_field(ctx, idx, number, aidx, v);
}

duk_pointer_t
js_get_array_pointer(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx)
{
    return js_get_array_field(ctx, idx, pointer, aidx);
}

void
js_set_array_pointer(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_pointer_t v)
{
    js_set_array_field(ctx, idx, pointer, aidx, v);
}

char *
js_get_array_string(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_size_t *plen)
{
    char *v = NULL;
    
    duk_get_prop_index(ctx, idx, aidx);
    if (duk_is_string(ctx, -1)) {
        v = (char *)duk_get_lstring(ctx, -1, plen);
    }
    duk_pop(ctx);

    return v;
}

char *
js_copy_array_string(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, char *buffer, int size)
{
    duk_size_t len = 0;
    char *v = js_get_array_string(ctx, idx, aidx, &len);
    if (v && (size-1) >= len) {
        os_strmcpy(buffer, v, len);
        v = buffer;
    } else {
        v = NULL;
    }

    return v;
}

void
js_set_array_string(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, char *v)
{
    idx = duk_normalize_index(ctx, idx);
    
    js_push_string(ctx, (char *)v);
    duk_put_prop_index(ctx, idx, aidx);
}

duk_buffer_t
js_get_array_buffer(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_size_t *plen)
{
    duk_buffer_t v = NULL;
    
    duk_get_prop_index(ctx, idx, aidx);
    if (duk_is_buffer(ctx, -1)) {
        v = duk_get_buffer_data(ctx, -1, plen);
    }
    duk_pop(ctx);

    return v;
}

char *
js_copy_array_buffer(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_buffer_t buffer, int size)
{
    duk_size_t len = 0;
    duk_buffer_t v = js_get_array_buffer(ctx, idx, aidx, &len);
    if (v && size >= len) {
        os_memcpy(buffer, v, len);
        v = buffer;
    } else {
        v = NULL;
    }

    return v;
}

void
js_set_array_buffer(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_buffer_t v, int size)
{
    idx = duk_normalize_index(ctx, idx);
    
    duk_buffer_t buf = js_push_dynamic_buffer(ctx, size);
    os_memcpy(buf, v, size);
    duk_put_prop_index(ctx, idx, aidx);
}

int
__js_seterrno(duk_context *ctx, int err)
{
    duk_push_global_object(ctx);
        if (duk_get_prop_string(ctx, -1, js_MOD_LIBC)) {
            js_set_obj_field(ctx, -1, int, "errno", err);
        }
    	duk_pop(ctx);
    duk_pop(ctx);

    return err;
}

int
js_obj_push(duk_context *ctx, dukc_obj_op_f *set, duk_object_t obj)
{
    if (obj) {
        duk_push_object(ctx);

        return (*set)(ctx, -1, obj);
    } else {
        duk_push_null(ctx);

        return 0;
    }
}

int
js_obj_op(duk_context *ctx, bool auto_create, duk_idx_t idx, dukc_obj_op_f *op, duk_object_t obj, duk_string_t k)
{
    int err = 0;

    idx = duk_normalize_index(ctx, idx);
    
    duk_get_prop_string(ctx, idx, k);
    if (duk_is_object(ctx, -1)) {
        err = (*op)(ctx, -1, obj);
    }
    else if (duk_is_undefined(ctx, -1) && auto_create) {
        duk_pop(ctx);
        duk_push_object(ctx);
        duk_put_prop_string(ctx, idx, k);
        duk_get_prop_string(ctx, idx, k);
        err = (*op)(ctx, -1, obj);
    }
    else {
        err = -EFORMAT;
    }
    duk_pop(ctx);
    
    return err;
}

int
js_obj_obj_op(duk_context *ctx, bool auto_create, duk_idx_t idx, dukc_obj_op_f *op, duk_object_t obj, ...)
{
    int err = 0, level = 0;
    va_list args;

    va_start(args, obj);
    err = js_get_v_prop_string(ctx, auto_create, idx, &level, args);
    va_end(args);

    if (0==err) {
        err = (*op)(ctx, -1, obj);
    }
    duk_pop_n(ctx, level);
    
    return err;
}

int
js_load_file(duk_context *ctx, char *filename)
{
    int err = -ENOEXIST;
    
    debug_js("load %s ...", filename);
    if (os_file_exist(filename)) {
        err = duk_peval_file_noresult(ctx, filename);
    }
    debug_js("load %s %s.", filename, __ok_string(0==err));

    return err;
}

int
js_load_code(duk_context *ctx, char *tag, char *code)
{
    int err = 0;
    
    debug_js("load %s ...", tag);
    err = duk_peval_string_noresult(ctx, code);
    debug_js("load %s %s.", tag, __ok_string(0==err));

    return err;
}

int
js_register(duk_context *ctx, js_register_f *cb)
{
    static js_register_f *registers[] = {
        js_global_register,
        js_my_register,
        js_libc_register,
#if js_LIBZ
        js_libz_register,
#endif
#if js_LIBBZ
        js_libbz_register,
#endif
#if js_LIBLZ
        js_liblz_register,
#endif
#if js_LIBCURL
        js_libcurl_register,
#endif
        /*keep below last*/
        js_buildin_register,
        js_auto_register,
    };
    int i, err;

    if (NULL==cb) {
        for (i=0; i<os_count_of(registers); i++) {
            err = (*registers[i])(ctx);
            if (err<0) {
                return err;
            }
        }

        return 0;
    } else {
        return (*cb)(ctx);
    }
}

void
__js_priv_fini(js_priv_t *priv)
{
    int i;

    if (priv) {
        os_free(priv->cache);
        os_free(priv->name);
        os_free(priv->atexit_name);
        
        for (i=0; i<NSIG; i++) {
            if (priv->sig[i].is_func) {
                os_free(priv->sig[i].name);
            }
        }
    }
}

js_priv_t *
js_priv_init(char *name, char *cache, int argc, char **argv)
{
    js_priv_t *priv = (js_priv_t *)os_zalloc(sizeof(*priv));
    if (NULL==priv) {
        return NULL;
    }

    priv->argc = argc;
    priv->argv = argv;
    priv->name = os_strdup(name);
    priv->cache= os_strdup(cache);
    
    if (1==argc) {
        priv->mode = JS_EVAL_STREAM;
    } else if (argc > 1) {
        if (os_streq("-c", argv[1])) {
            priv->mode = JS_EVAL_CONTENT;
        } else {
            priv->mode = JS_EVAL_SHABANG;
        }
    } else {
        priv->mode = JS_EVAL_BUILDIN;
    }
    
    return priv;
}

void
__js_fini(duk_context *ctx)
{
    if (ctx) {
        js_priv_t *priv = js_priv(ctx);
        
        debug_js("before destroy duk heap(%s)", priv->name);
        duk_destroy_heap(ctx);
        debug_js("after  destroy duk heap(%s)", priv->name);
        
        js_priv_fini(priv);
    }
}

duk_context *
js_init(char *name, char *cache, int argc, char **argv)
{
    duk_context *ctx = NULL;
    int err = 0;

    ctx = duk_create_heap_default();
    if (NULL==ctx) {
        goto error;
    }
    js_ctx_save(ctx);
    
    js_priv_t *priv = js_priv_init(name, cache, argc, argv);
    if (NULL==priv) {
        goto error;
    }
    duk_set_priv(ctx, priv);
    
    err = js_register(ctx, NULL);
    if (err<0) {
        goto error;
    }

    return ctx;
error:
    js_fini(ctx);
    
    return NULL;
}

static char *
js_content(int argc, char *argv[])
{
    int i, len, sum;
    char *content = NULL;
    
    /*
    * skip argv[0] and argv[1]
    *   argv[0] is appname
    *   argv[1] is "-c"
    */
    sum = 0;
    for (i=2; i<argc; i++) {
        sum += 1 + os_strlen(argv[i]);
    }

    content = (char *)os_malloc(1+sum);
    if (NULL==content) {
        return NULL;
    }

    sum = 0;
    for (i=2; i<argc; i++) {
        len = os_strlen(argv[i]);
        os_memcpy(content + sum, argv[i], len);
        sum += len;
        content[sum++] = ' ';
    }
    content[sum] = 0;
    
    return content;
}

int
js_eval(duk_context *ctx, char *jsfile)
{
    js_priv_t *priv = js_priv(ctx);
    char *script = NULL;
    
    switch(priv->mode) {
        case JS_EVAL_SHABANG:
            script = priv->argv[1];
            /*
            * argv[1] is script name
            */
            duk_peval_file(ctx, script);
            
            break;
        case JS_EVAL_CONTENT:
            /*
            * js -c CONTENT
            */
            script = js_content(priv->argc, priv->argv);
            duk_peval_string(ctx, script);
            free(script);
            
            break;
        case JS_EVAL_STREAM:
            /*
            * cat SCRIPT  | js
            * echo SCRIPT | js
            */
            script = os_readfd(0);
            duk_peval_string(ctx, script);
            free(script);
            
            break;
        case JS_EVAL_BUILDIN:
            script = jsfile;
            /*
            * jsfile is script name
            */
            duk_peval_file(ctx, script);
            
            break;
        default:
            return -ENOSUPPORT;
    }
    
    duk_pop(ctx);

    return 0;
}
/******************************************************************************/
