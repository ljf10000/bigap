#ifndef __DUKC_H_fe1a608115554746877494835643599f__
#define __DUKC_H_fe1a608115554746877494835643599f__
/******************************************************************************/
#include "duktape.h"

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

extern int
duk_require_buffer_or_lstring(duk_context *ctx, duk_idx_t idx, duk_buffer_t *pbuf, duk_size_t *psize);

extern void
js_put_functions(duk_context *ctx, duk_idx_t obj_index, const dukc_func_entry_t *funcs);

extern int
js_push_error(duk_context *ctx, int err);

extern duk_buffer_t
js_push_pointer(duk_context *ctx, duk_pointer_t pointer);

extern duk_string_t
js_push_lstring(duk_context *ctx, duk_string_t s, duk_size_t len);

extern duk_string_t
js_push_string(duk_context *ctx, duk_string_t s);

extern duk_buffer_t
js_push_buffer(duk_context *ctx, duk_size_t size, bool dynamic);

extern duk_buffer_t
js_push_fixed_buffer(duk_context *ctx, duk_size_t size);

extern duk_buffer_t
js_push_dynamic_buffer(duk_context *ctx, duk_size_t size);

extern void
js_pcall(duk_context *ctx, duk_idx_t idx, int (*push)(void));

extern int
js_get_a_prop_string(
    duk_context *ctx, 
    bool auto_create, 
    duk_idx_t idx, 
    int *level, 
    char **keys,
    int count
);

extern int
js_get_v_prop_string(
    duk_context *ctx, 
    bool auto_create, 
    duk_idx_t idx, 
    int *level, 
    va_list args
);

extern int
js_get_prop_string(duk_context *ctx, bool auto_create, duk_idx_t idx, int *level, ...);

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

extern char *
js_get_obj_string(duk_context *ctx, duk_idx_t idx, duk_string_t k, duk_size_t *plen);

extern char *
js_copy_obj_string(duk_context *ctx, duk_idx_t idx, duk_string_t k, char *buffer, int size);

extern void
js_set_obj_string(duk_context *ctx, duk_idx_t idx, duk_string_t k, char * v);

extern duk_buffer_t
js_get_obj_buffer(duk_context *ctx, duk_idx_t idx, duk_string_t k, duk_size_t *plen);

extern char *
js_copy_obj_buffer(duk_context *ctx, duk_idx_t idx, duk_string_t k, duk_buffer_t buffer, duk_size_t size);

extern void
js_set_obj_buffer(duk_context *ctx, duk_idx_t idx, duk_string_t k, duk_buffer_t v, duk_size_t size);

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

extern char *
js_get_array_string(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_size_t *plen);

extern char *
js_copy_array_string(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, char *buffer, int size);

extern void
js_set_array_string(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, char *v);

extern duk_buffer_t
js_get_array_buffer(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_size_t *plen);

extern char *
js_copy_array_buffer(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_buffer_t buffer, int size);

extern void
js_set_array_buffer(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_buffer_t v, int size);

extern int
__js_seterrno(duk_context *ctx, int err);

#define js_seterrno(_ctx)  __js_seterrno(ctx, -errno)

typedef int dukc_obj_op_f(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
js_obj_push(duk_context *ctx, dukc_obj_op_f *set, duk_object_t obj);

extern int
js_obj_op(duk_context *ctx, bool auto_create, duk_idx_t idx, dukc_obj_op_f *op, duk_object_t obj, duk_string_t k);

#define js_obj_get(_ctx, _idx, _get, _obj, _k) \
    js_obj_op(_ctx, false, _idx, _get, _obj, _k)
#define js_obj_set(_ctx, _idx, _set, _obj, _k) \
    js_obj_op(_ctx, true, _idx, _set, _obj, _k)

extern int
js_obj_obj_op(duk_context *ctx, bool auto_create, duk_idx_t idx, dukc_obj_op_f *op, duk_object_t obj, ...);

#define js_obj_obj_get(_ctx, _idx, _get, _obj, _args...) \
    js_obj_obj_op(_ctx, false, _idx, _get, _obj, ##_args)
#define js_obj_obj_set(_ctx, _idx, _get, _obj, _args...) \
    js_obj_obj_op(_ctx, true, _idx, _get, _obj, ##_args)

#if js_LIBC_SIG
extern void js_ctx_save(duk_context *ctx);
extern duk_context *js_ctx_get(void);
#endif

extern int js_auto_register(duk_context *ctx);
extern int js_buildin_register(duk_context *ctx);
extern int js_global_register(duk_context *ctx);
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

extern int
js_load_file(duk_context *ctx, char *filename);

extern int
js_load_code(duk_context *ctx, char *tag, char *code);

typedef int js_register_f(duk_context *);

extern int
js_register(duk_context *ctx, js_register_f *cb);

enum {
    JS_EVAL_SHABANG,
    JS_EVAL_STREAM,
    JS_EVAL_BUILDIN,

    JS_EVAL_END
};

typedef struct {
    int argc;
    char **argv;
    int mode;
    
    char *atexit_name;
    char *name;
    char *cache;
    
    struct {
        char *name;
        int is_func;
    } sig[NSIG];

    dlist_t list;
} js_priv_t;

extern void *duk_get_priv(duk_context *ctx);
extern void  duk_set_priv(duk_context *ctx, void *priv);

static inline js_priv_t *
js_priv(duk_context *ctx)
{
    return (js_priv_t *)duk_get_priv(ctx);
}

extern void
__js_priv_fini(js_priv_t *priv);

#define js_priv_fini(_priv) do{ \
    __js_priv_fini(_priv);      \
    _priv = NULL;               \
}while(0)

extern js_priv_t *
js_priv_init(char *name, char *cache, int argc, char **argv);

extern void
__js_fini(duk_context *ctx);

#define js_fini(_ctx) do{ \
    __js_fini(_ctx); \
    _ctx = NULL; \
}while(0)

extern duk_context *
js_init(char *name, char *cache, int argc, char **argv);

extern int
js_eval(duk_context *ctx, char *jsfile);

#include "libc.h"   /* must end */
/******************************************************************************/
#endif /* __DUKC_H_fe1a608115554746877494835643599f__ */
