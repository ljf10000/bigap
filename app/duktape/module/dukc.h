#ifndef __DUKC_H_fe1a608115554746877494835643599f__
#define __DUKC_H_fe1a608115554746877494835643599f__
/******************************************************************************/
#include "../src/duktape.h"

#define duk_LIBC_UCLIBC     0
#define duk_LIBC_GLIBC      1
#define duk_LIBC            duk_LIBC_UCLIBC

#ifndef duk_LIGHT_FUNC
#define duk_LIGHT_FUNC      0
#endif

#ifndef duk_LIBC_ERROR
#define duk_LIBC_ERROR      0
#endif

#ifndef duk_LIBC_TTY
#define duk_LIBC_TTY        0
#endif

#ifndef duk_LIBC_MATH
#define duk_LIBC_MATH       1
#endif

#ifndef duk_LIBC_TIME
#define duk_LIBC_TIME       1
#endif

#ifndef duk_LIBC_SIG
#define duk_LIBC_SIG        1
#endif

#ifndef duk_LIBC_LOG
#define duk_LIBC_LOG        1
#endif

#ifndef duk_LIBC_LINUX
#define duk_LIBC_LINUX      1
#endif

#ifndef duk_LIBUTIL
#define duk_LIBUTIL         0
#endif

#ifndef duk_LIBCURL
#define duk_LIBCURL         1
#endif

#ifndef ENV_duk_PATH
#define ENV_duk_PATH        "DUK_PATH"
#endif

#ifndef duk_PATH
#define duk_PATH            "/lib/duk"
#endif

#ifndef duk_DUKTAPE
#define duk_DUKTAPE         "Duktape"
#endif

#ifndef duk_SEARCH
#define duk_SEARCH          "modSearch"
#endif

#ifndef duk_MOD_MY
#define duk_MOD_MY          "__my__"
#endif

#ifndef duk_MOD_LIBC
#define duk_MOD_LIBC        "__libc__"
#endif

#ifndef duk_MOD_LIBCURL
#define duk_MOD_LIBCURL     "__libcurl__"
#endif

typedef duk_number_list_entry   dukc_number_entry_t;

typedef struct {
    /*
    * from duk_function_list_entry
    */
	const char *key;
	duk_c_function value;
	duk_idx_t nargs;
	
	uint32_t version;
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
#if duk_LIGHT_FUNC
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

#define __LIB_PARAM_VERSION_DEFT    1
#define __LIB_PARAM_NUMBER(x)       duke_##x##_param_number
#define __LIB_PARAM_VERSION(x)      duke_##x##_version
#define LIB_PARAM_API(x, n, v)      enum{ __LIB_PARAM_NUMBER(x) = n, __LIB_PARAM_VERSION(x) = v }
#define LIB_PARAM(x, n)             LIB_PARAM_API(x, n, __LIB_PARAM_VERSION_DEFT)
#define LIB_FUNC(x)                 {#x, duke_##x, __LIB_PARAM_NUMBER(x), __LIB_PARAM_VERSION(x)}
#define LIB_FUNC_END                {NULL, NULL, 0, __LIB_PARAM_VERSION_DEFT}
#define LIB_TVAR(x, _type)          {#x, (_type)(x)}
#define LIB_VALUE(x)                {#x, x}
#define LIB_VALUE_END               {NULL, 0}

typedef void *duk_pointer_t;
typedef void *duk_buffer_t;
typedef const char *duk_string_t;
typedef duk_double_t duk_number_t;

typedef struct {
    int type;

    union {
        bool b;
        double n;
        char *s;
        void *p;
    } u;
} duk_val_t;

#define duk_get_argc(_ctx)              duk_get_top(_ctx)
#define duk_get_max_idx(_ctx)           duk_get_top_index(_ctx)

#define duk_get_bool(_ctx, _idx)        duk_get_boolean(_ctx, _idx)
#define duk_push_bool(_ctx, _v)         duk_push_boolean(_ctx, _v)
#define duk_require_bool(_ctx, _idx)    duk_require_boolean(_ctx, _idx)

#define duk_is_int(_ctx, _idx)          duk_is_number(_ctx, _idx)
#define duk_is_uint(_ctx, _idx)         duk_is_number(_ctx, _idx)
#define duk_is_bool(_ctx, _idx)         duk_is_boolean(_ctx, _idx)

static inline int
duk_require_buffer_or_lstring(duk_context *ctx, duk_idx_t idx, void **pbuf, duk_size_t *psize)
{
    void *buf = NULL;
    
    int type = duk_get_type(ctx, idx);
    
    if (DUK_TYPE_BUFFER==type) {
        buf = duk_get_buffer_data(ctx, idx, psize);
    }
    else if (DUK_TYPE_STRING==type) {
        buf = (void *)duk_get_lstring(ctx, idx, psize);
    }
    else {
        return -EFORMAT;
    }

    *pbuf = buf;
    
    return 0;
}

static inline void
__pcall(duk_context *ctx, duk_idx_t idx, int (*push)(void))
{
    idx = duk_normalize_index(ctx, idx);
    
    duk_dup(ctx, idx);
    duk_pcall(ctx, (*push)());
    duk_pop(ctx);
}

static inline int
__get_a_prop_string(
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
__get_v_prop_string(
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
__get_prop_string(duk_context *ctx, bool auto_create, duk_idx_t idx, int *level, ...)
{
    va_list args;
    
    va_start(args, level);
    int err = __get_v_prop_string(ctx, auto_create, idx, level, args);
    va_end(args);

    return err;
}

#define __get_obj_field(_ctx, _idx, _type, _key) ({ \
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

#define __set_obj_field(_ctx, _idx, _type, _key, _v)    do{ \
    duk_idx_t __idx = duk_normalize_index(_ctx, _idx);      \
    duk_push_##_type(_ctx, _v);                             \
    duk_put_prop_string(_ctx, __idx, _key);                 \
}while(0)

static inline bool
__get_obj_bool(duk_context *ctx, duk_idx_t idx, const char *k)
{
    return __get_obj_field(ctx, idx, bool, k);
}

static inline void
__set_obj_bool(duk_context *ctx, duk_idx_t idx, const char *k, duk_bool_t v)
{
    __set_obj_field(ctx, idx, bool, k, v);
}

static inline int
__get_obj_int(duk_context *ctx, duk_idx_t idx, const char *k)
{
    return __get_obj_field(ctx, idx, int, k);
}

static inline void
__set_obj_int(duk_context *ctx, duk_idx_t idx, const char *k, duk_int_t v)
{
    __set_obj_field(ctx, idx, int, k, v);
}

static inline duk_uint_t
__get_obj_uint(duk_context *ctx, duk_idx_t idx, const char *k)
{
    return __get_obj_field(ctx, idx, uint, k);
}

static inline void
__set_obj_uint(duk_context *ctx, duk_idx_t idx, const char *k, duk_uint_t v)
{
    __set_obj_field(ctx, idx, uint, k, v);
}

static inline double
__get_obj_number(duk_context *ctx, duk_idx_t idx, const char *k)
{
    return __get_obj_field(ctx, idx, number, k);
}

static inline void
__set_obj_number(duk_context *ctx, duk_idx_t idx, const char *k, duk_double_t v)
{
    __set_obj_field(ctx, idx, number, k, v);
}

static inline void *
__get_obj_pointer(duk_context *ctx, duk_idx_t idx, const char *k)
{
    return __get_obj_field(ctx, idx, pointer, k);
}

static inline void
__set_obj_pointer(duk_context *ctx, duk_idx_t idx, const char *k, duk_pointer_t v)
{
    __set_obj_field(ctx, idx, pointer, k, v);
}

static inline char *
__get_obj_string(duk_context *ctx, duk_idx_t idx, const char *k, duk_size_t *plen)
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
__copy_obj_string(duk_context *ctx, duk_idx_t idx, const char *k, char *buffer, int size)
{
    duk_size_t len = 0;
    char *v = __get_obj_string(ctx, idx, k, &len);
    
    if (v && (size-1) >= len ) {
        v = os_strmcpy(buffer, v, len);
    } else {
        v = NULL;
    }

    return v;
}

static inline void
__set_obj_string(duk_context *ctx, duk_idx_t idx, const char *k, char * v)
{
    idx = duk_normalize_index(ctx, idx);
    
    duk_push_string(ctx, v);
    duk_put_prop_string(ctx, idx, k);
}

static inline void *
__get_obj_buffer(duk_context *ctx, duk_idx_t idx, const char *k, duk_size_t *plen)
{
    void *v = NULL;
    
    duk_get_prop_string(ctx, idx, k);
    if (duk_is_buffer(ctx, -1)) {
        v = duk_get_buffer_data(ctx, -1, plen);
    }
    duk_pop(ctx);

    return v;
}

static inline char *
__copy_obj_buffer(duk_context *ctx, duk_idx_t idx, const char *k, duk_buffer_t buffer, int size)
{
    duk_size_t len = 0;
    void *v = __get_obj_buffer(ctx, idx, k, &len);
    if (v && size>=len) {
        os_memcpy(buffer, v, len);
        v = buffer;
    } else {
        v = NULL;
    }

    return v;
}

static inline void
__set_obj_buffer(duk_context *ctx, duk_idx_t idx, const char *k, duk_buffer_t v, int size)
{
    idx = duk_normalize_index(ctx, idx);
    
    duk_buffer_t buf = duk_push_dynamic_buffer(ctx, size);
    os_memcpy(buf, v, size);
    duk_put_prop_string(ctx, idx, k);
}

static inline int
__get_array_length(duk_context *ctx, duk_idx_t idx)
{
    return __get_obj_int(ctx, idx, "length");
}

#define __get_array_field(_ctx, _idx, _type, _aidx)  ({ \
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

#define __set_array_field(_ctx, _idx, _type, _aidx, _v)  do{    \
    duk_idx_t __idx = duk_normalize_index(_ctx, _idx);          \
    duk_push_##_type(_ctx, _v);                                 \
    duk_put_prop_index(_ctx, __idx, _aidx);                     \
}while(0)

static inline bool
__get_array_bool(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx)
{
    return __get_array_field(ctx, idx, bool, aidx);
}

static inline void
__set_array_bool(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_bool_t v)
{
    __set_array_field(ctx, idx, bool, aidx, v);
}

static inline int
__get_array_int(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx)
{
    return __get_array_field(ctx, idx, int, aidx);
}

static inline void
__set_array_int(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_int_t v)
{
    __set_array_field(ctx, idx, int, aidx, v);
}

static inline duk_uint_t
__get_array_uint(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx)
{
    return __get_array_field(ctx, idx, uint, aidx);
}

static inline void
__set_array_uint(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_uint_t v)
{
    __set_array_field(ctx, idx, uint, aidx, v);
}

static inline double
__get_array_number(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx)
{
    return __get_array_field(ctx, idx, number, aidx);
}

static inline void
__set_array_number(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_number_t v)
{
    __set_array_field(ctx, idx, number, aidx, v);
}

static inline void *
__get_array_pointer(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx)
{
    return __get_array_field(ctx, idx, pointer, aidx);
}

static inline void
__set_array_pointer(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_pointer_t v)
{
    __set_array_field(ctx, idx, pointer, aidx, v);
}

static inline char *
__get_array_string(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_size_t *plen)
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
__copy_array_string(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, char *buffer, int size)
{
    duk_size_t len = 0;
    char *v = __get_array_string(ctx, idx, aidx, &len);
    if (v && (size-1) >= len) {
        os_strmcpy(buffer, v, len);
        v = buffer;
    } else {
        v = NULL;
    }

    return v;
}

static inline void
__set_array_string(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, char *v)
{
    idx = duk_normalize_index(ctx, idx);
    
    duk_push_string(ctx, (char *)v);
    duk_put_prop_index(ctx, idx, aidx);
}

static inline void *
__get_array_buffer(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_size_t *plen)
{
    void *v = NULL;
    
    duk_get_prop_index(ctx, idx, aidx);
    if (duk_is_buffer(ctx, -1)) {
        v = duk_get_buffer_data(ctx, -1, plen);
    }
    duk_pop(ctx);

    return v;
}

static inline char *
__copy_array_buffer(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_buffer_t buffer, int size)
{
    duk_size_t len = 0;
    void *v = __get_array_buffer(ctx, idx, aidx, &len);
    if (v && size >= len) {
        os_memcpy(buffer, v, len);
        v = buffer;
    } else {
        v = NULL;
    }

    return v;
}

static inline void
__set_array_buffer(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_buffer_t v, int size)
{
    idx = duk_normalize_index(ctx, idx);
    
    duk_buffer_t buf = duk_push_dynamic_buffer(ctx, size);
    os_memcpy(buf, v, size);
    duk_put_prop_index(ctx, idx, aidx);
}

static inline void
__seterrno(duk_context *ctx, int err)
{
    duk_push_global_object(ctx);
        if (duk_get_prop_string(ctx, -1, duk_MOD_LIBC)) {
            __set_obj_field(ctx, -1, int, "errno", err);
        }
    	duk_pop(ctx);
    duk_pop(ctx);
}
#define seterrno(_ctx)  __seterrno(ctx, -errno)

typedef int dukc_obj_op_f(duk_context *ctx, duk_idx_t idx, void *obj);

static inline int
__obj_push(duk_context *ctx, dukc_obj_op_f *set, void *obj)
{
    duk_push_object(ctx);
    
    return (*set)(ctx, -1, obj);
}

static inline int
__obj_op(duk_context *ctx, bool auto_create, duk_idx_t idx, dukc_obj_op_f *op, void *obj, const char *k)
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

#define __obj_get(_ctx, _idx, _get, _obj, _k) \
    __obj_op(_ctx, false, _idx, _get, _obj, _k)
#define __obj_set(_ctx, _idx, _get, _obj, _k) \
    __obj_op(_ctx, true, _idx, _get, _obj, _k)

static inline int
__obj_obj_op(duk_context *ctx, bool auto_create, duk_idx_t idx, dukc_obj_op_f *op, void *obj, ...)
{
    int err = 0, level = 0;
    va_list args;

    va_start(args, obj);
    err = __get_v_prop_string(ctx, auto_create, idx, &level, args);
    va_end(args);

    if (0==err) {
        err = (*op)(ctx, -1, obj);
    }
    duk_pop_n(ctx, level);
    
    return err;
}

#define __obj_obj_get(_ctx, _idx, _get, _obj, _args...) \
    __obj_obj_op(_ctx, false, _idx, _get, _obj, ##_args)
#define __obj_obj_set(_ctx, _idx, _get, _obj, _args...) \
    __obj_obj_op(_ctx, true, _idx, _get, _obj, ##_args)

extern duk_context *__ctx;
extern int __argc;
extern char **__argv;

extern int duktape_register(duk_context *ctx);
extern int global_register(duk_context *ctx);
extern int my_register(duk_context *ctx, int argc, char *argv[]);
extern int libc_register(duk_context *ctx);

#if duk_LIBCURL
extern int libcurl_register(duk_context *ctx);
#endif

#if duk_LIBC_SIG
extern char *libc_sig_name[];
extern void libc_sig_handler(int sig);
#endif

#include "libc.h"   /* mast end */
/******************************************************************************/
#endif /* __DUKC_H_fe1a608115554746877494835643599f__ */
