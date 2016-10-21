#ifndef __DUKC_H_fe1a608115554746877494835643599f__
#define __DUKC_H_fe1a608115554746877494835643599f__
/******************************************************************************/
#include "../src/duktape.h"

#define js_UCLIBC           1
#define js_GLIBC            2
#define js_LIBC             js_UCLIBC

#ifndef js_LIGHT_FUNC
#define js_LIGHT_FUNC       0
#endif

#if 1 /* libc */
#ifndef js_LIBC_PRIVATE
#define js_LIBC_PRIVATE     1
#endif

#ifndef js_LIBC_MEMORY
#define js_LIBC_MEMORY      0
#endif

#ifndef js_LIBC_CTYPE
#define js_LIBC_CTYPE       0
#endif

#ifndef js_LIBC_ERROR
#define js_LIBC_ERROR       0
#endif

#ifndef js_LIBC_TTY
#define js_LIBC_TTY         0
#endif

#ifndef js_LIBC_MATH
#define js_LIBC_MATH        1
#endif

#ifndef js_LIBC_TIME
#define js_LIBC_TIME        1
#endif

#ifndef js_LIBC_VTIME
#ifdef __OPENWRT__
#   define js_LIBC_VTIME    0
#else
#   define js_LIBC_VTIME    1
#endif
#endif

#ifndef js_LIBC_SIG
#define js_LIBC_SIG         1
#endif

#ifndef js_LIBC_LOG
#define js_LIBC_LOG         1
#endif

#ifndef js_LIBC_MOUNT
#ifdef __OPENWRT__
#   define js_LIBC_MOUNT    0
#else
#   define js_LIBC_MOUNT    1
#endif
#endif

#ifndef js_LIBC_LINUX
#define js_LIBC_LINUX       1
#endif

#ifndef js_LIBC_UTIL
#define js_LIBC_UTIL        0
#endif

#ifndef js_LIBC_FDSET
#define js_LIBC_FDSET       0
#endif

#ifndef js_LIBC_PWENT
#define js_LIBC_PWENT       0
#endif

#ifndef js_LIBC_GWENT
#define js_LIBC_GWENT       0
#endif

#ifndef js_MOD_LIBC
#define js_MOD_LIBC         "__libc__"
#endif
#endif /* libc */

#ifndef js_LIBCALL
#define js_LIBCALL          0
#endif

#ifndef js_MOD_MY
#define js_MOD_MY           "__my__"
#endif

#ifndef js_LIBCURL
#define js_LIBCURL          0
#endif

#ifndef js_MOD_LIBCURL
#define js_MOD_LIBCURL      "__libcurl__"
#endif

#ifndef js_LIBZ
#define js_LIBZ             0
#endif

#ifndef js_MOD_LIBZ
#define js_MOD_LIBZ         "__libz__"
#endif

#ifndef js_LIBBZ
#define js_LIBBZ            0
#endif

#ifndef js_MOD_LIBBZ
#define js_MOD_LIBBZ        "__libbz__"
#endif

#ifndef js_LIBLZ
#define js_LIBLZ            0
#endif

#ifndef js_MOD_LIBLZ
#define js_MOD_LIBLZ        "__liblz__"
#endif

#ifndef ENV_JPATH
#define ENV_JPATH           "JPATH"
#endif

#ifndef js_PATH
#define js_PATH             "/lib/js"
#endif

#ifndef js_AUTO_PATH
#define js_AUTO_PATH        ".auto"
#endif

#ifndef js_DUKTAPE
#define js_DUKTAPE          "Duktape"
#endif
/******************************************************************************/
typedef duk_number_list_entry   dukc_number_entry_t;

typedef struct {
    /*
    * from duk_function_list_entry
    */
	const char *key;
	duk_c_function value;
	duk_idx_t nargs;
	
	uint32 version;
} dukc_func_entry_t;

typedef struct {
	const char *key;
	char *value;
} dukc_string_entry_t;

typedef struct {
	const char *key;
	void *value;
} dukc_pointer_entry_t;

static void
duk_put_functions(duk_context *ctx, duk_idx_t obj_index, const dukc_func_entry_t *funcs)
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

#define JS_PARAM_VERSION_DEFT       1
#define JS_PARAM_NUMBER(x)          duke_##__THIS_FILE##x##_param_number
#define JS_PARAM_VERSION(x)         duke_##__THIS_FILE##x##_version
#define JS_PARAM_API(x, n, v)       enum{ JS_PARAM_NUMBER(x) = n, JS_PARAM_VERSION(x) = v }
#define JS_PARAM(x, n)              JS_PARAM_API(x, n, JS_PARAM_VERSION_DEFT)
#define JS_FUNC(x)                  {#x, duke_##x, JS_PARAM_NUMBER(x), JS_PARAM_VERSION(x)}
#define JS_FUNC_END                 {NULL, NULL, 0, JS_PARAM_VERSION_DEFT}
#define __JS_VALUE(_name, _value)   {_name, _value}
#define JS_TVAR(x, _type)           __JS_VALUE(#x, (_type)(x))
#define JS_VALUE(x)                 __JS_VALUE(#x, x)
#define JS_VALUE_END                __JS_VALUE(NULL, 0)

typedef void *duk_pointer_t;
typedef void *duk_buffer_t;
typedef void *duk_object_t;
typedef const char *duk_string_t;
typedef duk_double_t duk_number_t;

#define duk_get_argc(_ctx)              duk_get_top(_ctx)
#define duk_get_max_idx(_ctx)           duk_get_top_index(_ctx)

#define duk_get_bool(_ctx, _idx)        duk_get_boolean(_ctx, _idx)
#define duk_push_bool(_ctx, _v)         duk_push_boolean(_ctx, _v)
#define duk_require_bool(_ctx, _idx)    duk_require_boolean(_ctx, _idx)

#define duk_is_int(_ctx, _idx)          duk_is_number(_ctx, _idx)
#define duk_is_uint(_ctx, _idx)         duk_is_number(_ctx, _idx)
#define duk_is_bool(_ctx, _idx)         duk_is_boolean(_ctx, _idx)

static inline int
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

static inline int js_push_error(duk_context *ctx, int err);

static inline duk_buffer_t
js_push_pointer(duk_context *ctx, duk_pointer_t pointer)
{
    if (pointer) {
        duk_push_pointer(ctx, pointer);
    } else {
        duk_push_null(ctx);
    }

    return pointer;
}

static inline duk_string_t
js_push_lstring(duk_context *ctx, duk_string_t s, duk_size_t len)
{
    if (is_good_str(s) && len>0) {
        return duk_push_lstring(ctx, s, len);
    } else {
        return duk_push_null(ctx), NULL;
    }
}

static inline duk_string_t
js_push_string(duk_context *ctx, duk_string_t s)
{
    if (s) {
        return duk_push_string(ctx, s);
    } else {
        return duk_push_null(ctx), NULL;
    }
}

static inline duk_buffer_t
js_push_buffer(duk_context *ctx, duk_size_t size, bool dynamic)
{
    if (size>0) {
        return duk_push_buffer(ctx, size, dynamic);
    } else {
        return duk_push_null(ctx), NULL;
    }
}

static inline duk_buffer_t
js_push_fixed_buffer(duk_context *ctx, duk_size_t size)
{
    if (size>0) {
        return duk_push_fixed_buffer(ctx, size);
    } else {
        return duk_push_null(ctx), NULL;
    }
}

static inline duk_buffer_t
js_push_dynamic_buffer(duk_context *ctx, duk_size_t size)
{
    if (size>0) {
        return duk_push_dynamic_buffer(ctx, size);
    } else {
        return duk_push_null(ctx), NULL;
    }
}

static inline void
js_pcall(duk_context *ctx, duk_idx_t idx, int (*push)(void))
{
    idx = duk_normalize_index(ctx, idx);
    
    duk_dup(ctx, idx);
    duk_pcall(ctx, (*push)());
    duk_pop(ctx);
}

static inline int
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

static inline int
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

static inline int
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

static inline bool
js_get_obj_bool(duk_context *ctx, duk_idx_t idx, duk_string_t k)
{
    return js_get_obj_field(ctx, idx, bool, k);
}

static inline void
js_set_obj_bool(duk_context *ctx, duk_idx_t idx, duk_string_t k, duk_bool_t v)
{
    js_set_obj_field(ctx, idx, bool, k, v);
}

static inline int
js_get_obj_int(duk_context *ctx, duk_idx_t idx, duk_string_t k)
{
    return js_get_obj_field(ctx, idx, int, k);
}

static inline void
js_set_obj_int(duk_context *ctx, duk_idx_t idx, duk_string_t k, duk_int_t v)
{
    js_set_obj_field(ctx, idx, int, k, v);
}

static inline duk_uint_t
js_get_obj_uint(duk_context *ctx, duk_idx_t idx, duk_string_t k)
{
    return js_get_obj_field(ctx, idx, uint, k);
}

static inline void
js_set_obj_uint(duk_context *ctx, duk_idx_t idx, duk_string_t k, duk_uint_t v)
{
    js_set_obj_field(ctx, idx, uint, k, v);
}

static inline duk_number_t
js_get_obj_number(duk_context *ctx, duk_idx_t idx, duk_string_t k)
{
    return js_get_obj_field(ctx, idx, number, k);
}

static inline void
js_set_obj_number(duk_context *ctx, duk_idx_t idx, duk_string_t k, duk_number_t v)
{
    js_set_obj_field(ctx, idx, number, k, v);
}

static inline duk_pointer_t
js_get_obj_pointer(duk_context *ctx, duk_idx_t idx, duk_string_t k)
{
    return js_get_obj_field(ctx, idx, pointer, k);
}

static inline void
js_set_obj_pointer(duk_context *ctx, duk_idx_t idx, duk_string_t k, duk_pointer_t v)
{
    js_set_obj_field(ctx, idx, pointer, k, v);
}

static inline char *
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

static inline char *
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

static inline void
js_set_obj_string(duk_context *ctx, duk_idx_t idx, duk_string_t k, char * v)
{
    idx = duk_normalize_index(ctx, idx);
    
    js_push_string(ctx, v);
    duk_put_prop_string(ctx, idx, k);
}

static inline duk_buffer_t
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

static inline char *
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

static inline void
js_set_obj_buffer(duk_context *ctx, duk_idx_t idx, duk_string_t k, duk_buffer_t v, duk_size_t size)
{
    idx = duk_normalize_index(ctx, idx);
    
    duk_buffer_t buf = js_push_dynamic_buffer(ctx, size);
    os_memcpy(buf, v, size);
    duk_put_prop_string(ctx, idx, k);
}

static inline int
js_get_array_length(duk_context *ctx, duk_idx_t idx)
{
    return js_get_obj_int(ctx, idx, "length");
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

static inline bool
js_get_array_bool(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx)
{
    return js_get_array_field(ctx, idx, bool, aidx);
}

static inline void
js_set_array_bool(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_bool_t v)
{
    js_set_array_field(ctx, idx, bool, aidx, v);
}

static inline int
js_get_array_int(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx)
{
    return js_get_array_field(ctx, idx, int, aidx);
}

static inline void
js_set_array_int(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_int_t v)
{
    js_set_array_field(ctx, idx, int, aidx, v);
}

static inline duk_uint_t
js_get_array_uint(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx)
{
    return js_get_array_field(ctx, idx, uint, aidx);
}

static inline void
js_set_array_uint(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_uint_t v)
{
    js_set_array_field(ctx, idx, uint, aidx, v);
}

static inline duk_number_t
js_get_array_number(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx)
{
    return js_get_array_field(ctx, idx, number, aidx);
}

static inline void
js_set_array_number(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_number_t v)
{
    js_set_array_field(ctx, idx, number, aidx, v);
}

static inline duk_pointer_t
js_get_array_pointer(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx)
{
    return js_get_array_field(ctx, idx, pointer, aidx);
}

static inline void
js_set_array_pointer(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_pointer_t v)
{
    js_set_array_field(ctx, idx, pointer, aidx, v);
}

static inline char *
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

static inline char *
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

static inline void
js_set_array_string(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, char *v)
{
    idx = duk_normalize_index(ctx, idx);
    
    js_push_string(ctx, (char *)v);
    duk_put_prop_index(ctx, idx, aidx);
}

static inline duk_buffer_t
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

static inline char *
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

static inline void
js_set_array_buffer(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_buffer_t v, int size)
{
    idx = duk_normalize_index(ctx, idx);
    
    duk_buffer_t buf = js_push_dynamic_buffer(ctx, size);
    os_memcpy(buf, v, size);
    duk_put_prop_index(ctx, idx, aidx);
}

static inline int
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
#define js_seterrno(_ctx)  __js_seterrno(ctx, -errno)

typedef int dukc_obj_op_f(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

static inline int 
js_push_error(duk_context *ctx, int err)
{
    if (err<0) {
        js_seterrno(ctx);
    }

    duk_push_int(ctx, err);
    
    return err;
}

static inline int
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

static inline int
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

#define js_obj_get(_ctx, _idx, _get, _obj, _k) \
    js_obj_op(_ctx, false, _idx, _get, _obj, _k)
#define js_obj_set(_ctx, _idx, _set, _obj, _k) \
    js_obj_op(_ctx, true, _idx, _set, _obj, _k)

static inline int
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

#define js_obj_obj_get(_ctx, _idx, _get, _obj, _args...) \
    js_obj_obj_op(_ctx, false, _idx, _get, _obj, ##_args)
#define js_obj_obj_set(_ctx, _idx, _get, _obj, _args...) \
    js_obj_obj_op(_ctx, true, _idx, _get, _obj, ##_args)

#if js_LIBC_SIG
extern void js_ctx_save(duk_context *ctx);
extern duk_context *js_ctx(void);
#endif

extern int js_auto_register(duk_context *ctx);
extern int js_buildin_register(duk_context *ctx);
extern int js_global_register(duk_context *ctx);
extern int js_duktape_register(duk_context *ctx);
extern int js_my_register(duk_context *ctx);
extern int js_libc_register(duk_context *ctx);
#if js_LIBZ
extern int js_libz_register(duk_context *ctx);
#endif
#if js_LIBBZ
extern int js_libbz_register(duk_context *ctx);
#endif
#if js_LIBLZ
extern int js_liblz_register(duk_context *ctx);
#endif
#if js_LIBCURL
extern int js_libcurl_register(duk_context *ctx);
#endif

static inline int
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

static inline int
js_load_code(duk_context *ctx, char *tag, char *code)
{
    int err = 0;
    
    debug_js("load %s ...", tag);
    err = duk_peval_string_noresult(ctx, code);
    debug_js("load %s %s.", tag, __ok_string(0==err));

    return err;
}

static inline int
js_register(duk_context *ctx)
{
    static int (*registers[])(duk_context *) = {
        js_global_register,
        js_duktape_register,
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

    for (i=0; i<os_count_of(registers); i++) {
        err = (*registers[i])(ctx);
        if (err<0) {
            return err;
        }
    }

    return 0;
}

enum {
    JS_EXEC_SHABANG,
    JS_EXEC_STRING,
    JS_EXEC_BUILDIN,

    JS_EXEC_END
};

static inline void
js_priv_init(duk_priv_t *priv, char *name, int argc, char **argv)
{
    os_objzero(priv);

    priv->argc = argc;
    priv->argv = argv;
    priv->name = os_strdup(name);
    
    if (argv) {
        if (argc > 1) {
            priv->mode = JS_EXEC_SHABANG;
        } else {
            priv->mode = JS_EXEC_STRING;
        }
    } else {
        priv->mode = JS_EXEC_BUILDIN;
    }
}

static inline void
js_priv_fini(duk_priv_t *priv)
{
    int i;

    os_free(priv->name);
    os_free(priv->atexit_name);
    
    for (i=0; i<NSIG; i++) {
        if (priv->sig[i].is_func) {
            os_free(priv->sig[i].name);
        }
    }
}

static inline char *
js_readfd(duk_context *ctx, int fd) 
{
	FILE *f = NULL;
	char *buf = NULL;
	long sz = 0, len = 0, left, ret;  /* ANSI C typing */

	if (fd<0) {
		goto error;
	}
	
	f = os_fdopen(fd, "r");
	if (!f) {
		goto error;
	}

	while(!os_feof(f) && !os_ferror(f)) {
        if (0==sz) {
            sz += 4096;
            buf = (char *)os_malloc(sz);
        }
        else if (sz - len < 512) {
            sz += 4096;
            buf = (char *)os_realloc(buf, sz);
        }
        else {
            /*
            * use the buf, do nothing
            */
        }
        
        if (NULL==buf) {
            goto error;
        }

	    left = sz - len;
        ret = os_fread(f, buf + len, left);
        if (ret > left || ret < 0) {
            goto error;
        }
        
        len += ret;
	}
	buf[len++] = 0;

    if ('#'==buf[0] && '!'==buf[1]) {
        buf[0] = '/';
        buf[1] = '/';
    }

	return buf;
error:
    os_free(buf);
    os_fclose(f);
    
    return NULL;
}

static inline int
js_run(duk_context *ctx)
{
    char *script = NULL;
    
    switch(priv->mode) {
        case JS_EXEC_SHABANG:
            script = duk_priv(ctx)->argv[1];
            /*
            * argv[1] is script name
            */
            duk_peval_file(ctx, script);
            
            break;
        case JS_EXEC_STRING:
            script = js_readfd(ctx, 0);

            /*
            * cat SCRIPT  | js
            * echo SCRIPT | js
            */
            duk_peval_string(ctx, script); free(script);
            
            break;
        case JS_EXEC_BUILDIN:
            break;
    }
    
    duk_pop(ctx);

    return 0;
}

static inline duk_context *
js_init(char *name, int argc, char **argv)
{
    int err = 0;

    duk_context *ctx = duk_create_heap_default();
    if (NULL==ctx) {
        return NULL;
    }
    js_ctx_save(ctx);
    
    duk_priv_t *priv = duk_priv(ctx);
    js_priv_init(priv, name, argc, argv);

    err = js_register(ctx);
    if (err<0) {
        return NULL;
    }

    return ctx;
}

static inline void
__js_fini(duk_context *ctx)
{
    if (ctx) {
        duk_priv_t *priv = duk_priv(ctx);
        char *name = os_strdup(priv->name);
        js_priv_fini(priv);
        
        debug_js("before destroy duk heap(%s)", name);
        duk_destroy_heap(ctx);
        debug_js("after  destroy duk heap(%s)", name);
        os_free(name);
    }
}

#define js_fini(_ctx) do{ \
    __js_fini(_ctx); \
    _ctx = NULL; \
}while(0)

#include "libc.h"   /* must end */
/******************************************************************************/
#endif /* __DUKC_H_fe1a608115554746877494835643599f__ */
