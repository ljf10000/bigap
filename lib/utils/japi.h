#ifndef __JAPI_H_82b58c7daf6248b381aac0f6971b0d3d__
#define __JAPI_H_82b58c7daf6248b381aac0f6971b0d3d__
#ifdef __APP__
#include "json-c/json.h"
#endif
/******************************************************************************/
#ifndef JAPI_PRINT
#define JAPI_PRINT   0
#endif

#if JAPI_PRINT
#define japi_println(_fmt, _args...)     os_println(_fmt, ##_args)
#else
#define japi_println(_fmt, _args...)     os_do_nothing()
#endif

typedef struct json_object*     jobj_t;
typedef struct json_tokener*    jtok_t;

enum {
    jfmt_bool    = 'b', /* bool */
    jfmt_long    = 'l', /* int64 */
    jfmt_int     = 'd', /* int32 */
    jfmt_double  = 'f', /* double */
    jfmt_string  = 's',
    jfmt_array   = 'a',
    jfmt_object  = 'o',
    jfmt_json    = 'j',
};

typedef union {
    bool        b;
    double      f;
    int32_t     d;
    int64_t     l;
    char *      s;
    char *      j;
    jobj_t      a;
    jobj_t      o;
} jvar_t;

#ifdef __APP__
enum {
    jtype_null    = json_type_null,
    jtype_bool    = json_type_boolean,
    jtype_double  = json_type_double,
    jtype_int     = json_type_int,
    jtype_object  = json_type_object,
    jtype_array   = json_type_array,
    jtype_string  = json_type_string,
};

typedef struct {
    char **node;
    int count;
    int type;
    
    jobj_t obj;
    jvar_t var;
} jnode_t;

#define JNODE_INITER(_type, _node) { \
    .type = _type, \
    .node = _node, \
    .count = os_count_of(_node), \
}   /* end */

#define jobj_new_bool(_v)       json_object_new_boolean(_v)
#define jobj_new_int(_v)        json_object_new_int(_v)
#define jobj_new_int64(_v)      json_object_new_int64(_v)
#define jobj_new_double(_v)     json_object_new_double(_v)
#define jobj_new_string(_v)     json_object_new_string(_v)
#define jobj_new_object()       json_object_new_object()
#define jobj_new_array()        json_object_new_array()

#define jobj_type(_obj)         json_object_get_type(_obj)

static inline void
__jobj_put(jobj_t obj)
{
    if (obj) {
        json_object_put(obj);
    }
}

#define jobj_put(_obj)  do{ \
    __jobj_put(_obj);       \
    _obj = NULL;            \
}while(0)

#define jobj_set(_obj, _new) do{ \
    __jobj_put(_obj);       \
    _obj = _new;            \
}while(0)

static inline char *
jobj_string_ex(jobj_t obj, int flag)
{
    if (obj) {
        return (char *)json_object_to_json_string_ext(obj, flag);
    } else {
        return NULL;
    }
}

#define jobj_string(_obj)           jobj_string_ex(_obj, JSON_C_TO_STRING_PLAIN)

static inline int
jobj_string_len(jobj_t obj)
{
    if (obj) {
        return json_object_get_string_len(obj);
    } else {
        return 0;
    }
}

static inline void
jobj_add(jobj_t obj, char *k, jobj_t v)
{
    if (obj && v) {
        switch(jobj_type(obj)) {
            case jtype_object:
                if (k) {
                    json_object_object_add(obj, k, v);
                }

                break;
            case jtype_array:
                if (NULL==k) {
                    json_object_array_add(obj, v);
                } else {
                    json_object_array_put_idx(obj, (int)k, v);
                }
                
                break;
        }       
    }
}

#define jobj_get_bool(_obj)             json_object_get_boolean(_obj)
#define jobj_get_int(_obj)              json_object_get_int(_obj)
#define jobj_get_int64(_obj)            json_object_get_int64(_obj)
#define jobj_get_double(_obj)           json_object_get_double(_obj)
#define jobj_get_string(_obj)           ((char *)json_object_get_string(_obj))
#define jobj_get_string_len(_obj)       json_object_get_string_len(_obj)

static inline jobj_t
jobj_get(jobj_t obj, char *key)
{
    struct json_object *new = NULL;
    
    if (obj && key) {
    	json_object_object_get_ex(obj, key, &new);
    }

    return new;
}

#define jarray_length(_obj)             json_object_array_length(_obj)
static inline jobj_t
jarray_get(jobj_t array, int idx)
{
    struct json_object *new = NULL;
    
    if (array && is_good_enum(idx, jarray_length(array))) {
    	new = json_object_array_get_idx(array, idx);
    }

    return new;
}


#define jobj_foreach(_root, _k, _v)     json_object_object_foreach(_root, _k, _v)

#define jtok_new()                      json_tokener_new()
#define jtok_free(_tok)                 do{ json_tokener_free(_tok); _tok = NULL; }while(0)
#define jtok_set_flags(_tok, _flag)     json_tokener_set_flags(_tok, _flag)
#define jtok_parse_ex(_tok, _str, _len) json_tokener_parse_ex(_tok, _str, _len)

#define jobj_add_value(_obj, _key, _value, _create) ({ \
    __label__ error;                            \
    int err = 0;                                \
                                                \
    jobj_t __new = _create(_value);             \
    if (NULL==__new) {                          \
        japi_println(#_create "failed");        \
        err = -ENOMEM; goto error;              \
    }                                           \
                                                \
    jobj_add(_obj, (char *)_key, __new);        \
error:                                          \
    err;                                        \
})

static inline int
__jobj_add_bool(jobj_t obj, char *key, bool value)
{
    return jobj_add_value(obj, key, value, jobj_new_bool);
}
#define jobj_add_bool(_obj, _key, _value)   __jobj_add_bool(_obj, (char *)_key, _value)

static inline int
__jobj_add_int(jobj_t obj, char *key, int value)
{
    return jobj_add_value(obj, key, value, jobj_new_int);
}
#define jobj_add_int(_obj, _key, _value)    __jobj_add_int(_obj, (char *)_key, _value)

static inline int
__jobj_add_int64(jobj_t obj, char *key, int64_t value)
{
    return jobj_add_value(obj, key, value, jobj_new_int64);
}
#define jobj_add_int64(_obj, _key, _value)  __jobj_add_int64(_obj, (char *)_key, _value)

static inline int
__jobj_add_double(jobj_t obj, char *key, double value)
{
    return jobj_add_value(obj, key, value, jobj_new_double);
}
#define jobj_add_double(_obj, _key, _value) __jobj_add_double(_obj, (char *)_key, _value)

static inline int
__jobj_add_string(jobj_t obj, char *key, char *value)
{
    if (is_good_str(value)) {
        return jobj_add_value(obj, key, value, jobj_new_string);
    } else {
        return 0;
    }
}
#define jobj_add_string(_obj, _key, _value) __jobj_add_string(_obj, (char *)_key, (char *)_value)

static inline int
jobj_vsprintf(jobj_t obj, char *key, const char *fmt, va_list args)
{
    int len, err = 0;
    char *line = (char *)os_zalloc(1+OS_BIG_LEN);
    if (NULL==line) {
        err = -ENOMEM; goto error;
    }

    len = os_vsnprintf(line, OS_BIG_LEN, fmt, args);
    if (os_snprintf_is_full(OS_BIG_LEN, len)) {
        err = -ENOSPACE;
    } else {
        err = jobj_add_string(obj, key, line);
	}

error:
    if (line) {
        os_free(line);
    }

    return err;
}

static inline int
jobj_sprintf(jobj_t obj, char *key, const char *fmt, ...)
{
    va_list args;
    int err;
    
    va_start(args, fmt);
    err = jobj_vsprintf(obj, key, fmt, args);
    va_end(args);
    
    return err;    
}

static inline jobj_t
jobj_file(char *file)
{
    jobj_t obj = json_object_from_file(file);
    if (NULL==obj) {
        japi_println("read json file %s failed", file);
    }
    
    return obj;
}

static inline jobj_t
jobj_fd(int fd)
{
    jobj_t obj = json_object_from_fd(fd);
    if (NULL==obj) {
        japi_println("read fd %d failed", fd);
    }
    
    return obj;
}

static inline jobj_t
jobj(char *json)
{
    int err = 0;
    jtok_t tok = NULL;
    jobj_t obj = NULL;

    if (false==is_good_str(json)) {
        return NULL;
    }
    
    tok = jtok_new();
    if (NULL==tok) {
        japi_println("create tok error");
        
        err = -ENOMEM; goto error;
    }
    
    jtok_set_flags(tok, JSON_TOKENER_STRICT);
    
    obj = jtok_parse_ex(tok, json, os_strlen(json));
    if (NULL==tok) {
        japi_println("tok parase error");
        
        err = -ENOMEM; goto error;
    }
    
error:
    jtok_free(tok);

    return obj;
}

static inline int
jobj_add_json(jobj_t obj, char *key, char *value)
{
    int err = 0;
    jobj_t new = NULL;

    if (NULL==key) {
        return -EKEYNULL;
    }
    else if (false==is_good_str(value)) {
        return 0;
    }

    new = jobj(value);
    if (NULL==new) {
        return -EFORMAT;
    }

    /*
    * new is add to obj,
    *   needn't to free new
    */
    jobj_add(obj, key, new);

    return 0;
}

static inline int
jobj_vprintf(jobj_t obj, const char *fmt, va_list args)
{
    static jobj_t empty = NULL;
    
    char *key, *p;
    jvar_t var;
    int err = 0;

    if (NULL==empty) {
        empty = jobj_new_object();
    }
    
    p = (char *)fmt;
    while(*p) {
        if ('%' == *p++) {
            key = va_arg(args, char *);
        } else {
            japi_println("bad format:%s", fmt);
            
            return -EFORMAT;
        }

        switch(*p++) {
            case jfmt_bool:
                var.b = va_arg(args, bool);
                japi_println("bool=%d", var.b);
                err = jobj_add_bool(obj, key, var.b);
                
                break;
            case jfmt_int:
                var.d = va_arg(args, int32_t);
                japi_println("int=%d", var.d);
                err = jobj_add_int(obj, key, var.d);

                break;
            case jfmt_long:
                var.l = va_arg(args, int64_t);
                japi_println("int64=%lld", var.l);
                err = jobj_add_int64(obj, key, var.l);
                
                break;
            case jfmt_double:
                var.f = va_arg(args, double);
                japi_println("double=%lf", var.f);
                err = jobj_add_double(obj, key, var.f);
                
                break;
            case jfmt_string:
                var.s = va_arg(args, char *);
                if (NULL==var.s) {
                    var.s = __empty;
                }
                japi_println("string=%s", var.s);
                err = jobj_add_string(obj, key, var.s);
                
                break;
            case jfmt_array:
                var.a = va_arg(args, jobj_t);
                if (NULL==var.a) {
                    var.a = empty;
                }
                japi_println("array=%s", jobj_string(var.a));
                jobj_add(obj, key, var.a);

                break;
            case jfmt_object:
                var.o = va_arg(args, jobj_t);
                if (NULL==var.o) {
                    var.o = empty;
                }
                japi_println("object=%s", jobj_string(var.o));
                jobj_add(obj, key, var.o);

                break;
            case jfmt_json:
                var.j = va_arg(args, char *);
                if (NULL==var.j) {
                    var.j = "{}";
                }
                japi_println("json=%s", var.j);
                jobj_add_json(obj, key, var.j);

                break;
            default:
                japi_println("bad format:%s", fmt);
                
                return -EFORMAT;
        }

        if (err<0) {
            return err;
        }
    }

    return 0;
}

static inline int
jobj_printf(jobj_t obj, const char *fmt, ...)
{
    int err = 0;
    va_list args;
    
    va_start(args, (char *)fmt);
    err = jobj_vprintf(obj, fmt, args);
    va_end(args);
    
    return err;
}

static inline jobj_t
jobj_voprintf(const char *fmt, va_list args)
{
    jobj_t obj = jobj_new_object();
    if (NULL==obj) {
        return NULL;
    }

    int err = jobj_vprintf(obj, fmt, args);
    if (err<0) {
        return obj;
    }
    
    return obj;
}

static inline jobj_t
jobj_oprintf(const char *fmt, ...)
{
    jobj_t obj;
    va_list args;
    
    va_start(args, (char *)fmt);
    obj = jobj_voprintf(fmt, args);
    va_end(args);
    
    return obj;
}

static inline bool
is_good_json(char *json)
{
    jobj_t obj = jobj(json);
    if (obj) {
        jobj_put(obj);

        return true;
    } else {
        return false;
    }
}

static inline jobj_t
jobj_get_vleaf(jobj_t obj, va_list args)
{
    jobj_t root = obj, leaf = NULL;
    char *key = va_arg(args, char *);
    
    while(key) {
        root = leaf = jobj_get(root, key);

        key = va_arg(args, char *);
    }
    
    return leaf;
}

static inline jobj_t
jobj_get_leaf(jobj_t obj, ...)
{
    jobj_t leaf;
    va_list args;
    
    va_start(args, (char *)obj);
    leaf = jobj_get_vleaf(obj, args);
    va_end(args);
    
    return leaf;
}

#endif /* __APP__ */
/******************************************************************************/
#endif /* __JAPI_H_82b58c7daf6248b381aac0f6971b0d3d__ */
