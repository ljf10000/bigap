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
    jfmt_double  = 'f', /* float64 */
    jfmt_string  = 's',
    jfmt_array   = 'a',
    jfmt_object  = 'o',
    jfmt_json    = 'j',
};

typedef union {
    bool    b;
    int32   d;
    int64   l;
    float64 f;
    char    *s;
    char    *j;
    jobj_t  a;
    jobj_t  o;
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
#define jobj_new_i32(_v)        json_object_new_int(_v)
#define jobj_new_u32(_v)        json_object_new_int(_v)
#define jobj_new_f32(_v)        json_object_new_double(_v)
#define jobj_new_i64(_v)        json_object_new_int64(_v)
#define jobj_new_u64(_v)        json_object_new_int64(_v)
#define jobj_new_f64(_v)        json_object_new_double(_v)
#define jobj_new_string(_v)     json_object_new_string(_v)
#define jobj_new_object()       json_object_new_object()
#define jobj_new_array()        json_object_new_array()

#define jobj_type(_obj)         ((int)json_object_get_type(_obj))

static inline bool
jtype_is_container(int type) 
{
    return jtype_object==type || jtype_array==type;
}

static inline bool
jobj_is_container(jobj_t obj) 
{
    return jtype_is_container(jobj_type(obj));
}

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

static inline char *
jobj_string_ex(jobj_t obj, int flag)
{
    if (obj) {
        return (char *)json_object_to_json_string_ext(obj, flag);
    } else {
        return NULL;
    }
}

static inline char *
jobj_json(jobj_t obj)
{
    return jobj_string_ex(obj, JSON_C_TO_STRING_PLAIN);
}

static inline int
jobj_string_len(jobj_t obj)
{
    if (obj) {
        return json_object_get_string_len(obj);
    } else {
        return 0;
    }
}

#define jobj_del(_jobj, _k)     json_object_object_del(_jobj, _k)

extern void
jobj_add(jobj_t obj, char *k, jobj_t v);

#define jobj_get_bool(_obj)             json_object_get_boolean(_obj)
#define jobj_get_i32(_obj)              json_object_get_int(_obj)
#define jobj_get_u32(_obj)              ((uint32)json_object_get_int(_obj))
#define jobj_get_f32(_obj)              ((float32)json_object_get_double(_obj))
#define jobj_get_i64(_obj)              json_object_get_int64(_obj)
#define jobj_get_u64(_obj)              ((uint64)json_object_get_int64(_obj))
#define jobj_get_f64(_obj)              json_object_get_double(_obj)
#define jobj_get_string(_obj)           ((char *)json_object_get_string(_obj))
#define jobj_get_string_len(_obj)       json_object_get_string_len(_obj)
#define jobj_get_binary(_obj)           ((byte *)json_object_get_string(_obj))
#define jobj_get_binary_len(_obj)       json_object_get_string_len(_obj)

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
    jobj_t obj = NULL;
    
    if (array && IS_GOOD_ENUM(idx, jarray_length(array))) {
    	obj = json_object_array_get_idx(array, idx);
    }

    return obj;
}

#define jarray_set(_jarray, _idx, _jval)    json_object_array_put_idx(_jarray, _idx, _jval)
#define jarray_add(_jarray, _jval)          json_object_array_add(_jarray, _jval)

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
#define jobj_add_bool(_obj, _key, _value)   __jobj_add_bool(_obj, (char *)_key, (bool)_value)

static inline int
__jobj_add_i32(jobj_t obj, char *key, int32 value)
{
    return jobj_add_value(obj, key, value, jobj_new_i32);
}
#define jobj_add_i32(_obj, _key, _value)    __jobj_add_i32(_obj, (char *)_key, (int32)_value)

static inline int
__jobj_add_u32(jobj_t obj, char *key, uint32 value)
{
    return jobj_add_value(obj, key, value, jobj_new_u32);
}
#define jobj_add_u32(_obj, _key, _value)    __jobj_add_u32(_obj, (char *)_key, (uint32)_value)

static inline int
__jobj_add_f32(jobj_t obj, char *key, float32 value)
{
    return jobj_add_value(obj, key, value, jobj_new_f32);
}
#define jobj_add_f32(_obj, _key, _value) __jobj_add_f32(_obj, (char *)_key, (float32)_value)

static inline int
__jobj_add_i64(jobj_t obj, char *key, int64 value)
{
    return jobj_add_value(obj, key, value, jobj_new_i64);
}
#define jobj_add_i64(_obj, _key, _value)  __jobj_add_i64(_obj, (char *)_key, (int64)_value)

static inline int
__jobj_add_u64(jobj_t obj, char *key, uint64 value)
{
    return jobj_add_value(obj, key, value, jobj_new_u64);
}
#define jobj_add_u64(_obj, _key, _value)  __jobj_add_u64(_obj, (char *)_key, (uint64)_value)

static inline int
__jobj_add_f64(jobj_t obj, char *key, float64 value)
{
    return jobj_add_value(obj, key, value, jobj_new_f64);
}
#define jobj_add_f64(_obj, _key, _value) __jobj_add_f64(_obj, (char *)_key, (float64)_value)

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
#define jobj_add_binary(_obj, _key, _value) __jobj_add_string(_obj, (char *)_key, (char *)_value)

extern int
jobj_vsprintf(jobj_t obj, const char *key, const char *fmt, va_list args);

extern int
jobj_sprintf(jobj_t obj, const char *key, const char *fmt, ...);

static inline jobj_t
jobj_byfile(char *file)
{
    jobj_t obj = json_object_from_file(file);
    if (NULL==obj) {
        japi_println("read json file %s failed", file);
    }
    
    return obj;
}

static inline jobj_t
jobj_byfd(int fd)
{
    jobj_t obj = json_object_from_fd(fd);
    if (NULL==obj) {
        japi_println("read fd %d failed", fd);
    }
    
    return obj;
}

extern jobj_t
jobj_byjson(char *json);

extern int
jobj_add_json(jobj_t obj, char *key, char *value);

extern jobj_t
jarray_clone(jobj_t jobj, bool keepsort);

extern jobj_t
jobj_clone(jobj_t jobj, bool keepsort);

extern bool
jobj_format_is_valid(int c, int valid[], int count);

extern int
jobj_exec(jobj_t obj, const char *fmt, int argc, char *argv[]);

extern int
jobj_vprintf(jobj_t obj, const char *fmt, va_list args);

extern int
jobj_printf(jobj_t obj, const char *fmt, ...);

extern jobj_t
jobj_voprintf(const char *fmt, va_list args);

extern jobj_t
jobj_oprintf(const char *fmt, ...);

extern bool
is_good_json(char *json);

extern jobj_t
jobj_get_vleaf(jobj_t obj, va_list args);

extern jobj_t
jobj_get_leaf(jobj_t obj, ...);
/******************************************************************************/
#define jj_byvar(_obj, _jobj, _member, _type, _format) ({ \
    jobj_t __tmp = jobj_get(_jobj, #_member); \
    if (__tmp) { \
        (_obj)->_member = jobj_get_##_type(__tmp); \
        debug_format(#_member "=" _format, (_obj)->_member); \
    } \
    0; \
})  /* end */

#define jj_bymap(_obj, _jobj, _member, _mapper, _op) ({ \
    jobj_t __tmp = jobj_get(_jobj, #_member); \
    if (__tmp) { \
        char *string = jobj_get_string(__tmp); \
        _op((_obj)->_member, _mapper(string)); \
        debug_format(#_member "=%s", string); \
    } \
    0; \
})  /* end */

#define jj_byeq(_obj, _jobj, _member, _mapper) \
        jj_bymap(_obj, _jobj, _member, _mapper, os_eq_buildin)
        
#define jj_ip(_obj, _jobj, _member)         jj_byeq(_obj, _jobj, _member, inet_addr)
#define jj_time(_obj, _jobj, _member)       jj_byeq(_obj, _jobj, _member, os_fulltime)
#define jj_string(_obj, _jobj, _member)     jj_byeq(_obj, _jobj, _member, os_strdup)
#define jj_string_unsafe(_obj, _jobj, _member)  jj_byeq(_obj, _jobj, _member, os_map_nothing)

#define jj_mac(_obj, _jobj, _member)        jj_bymap(_obj, _jobj, _member, os_mac, os_maccpy)
#define jj_strcpy(_obj, _jobj, _member)     jj_bymap(_obj, _jobj, _member, os_map_nothing, os_strcpy)

#define jj_u64(_obj, _jobj, _member)        jj_byvar(_obj, _jobj, _member, u64, "%llu")
#define jj_i64(_obj, _jobj, _member)        jj_byvar(_obj, _jobj, _member, i64, "%lld")
#define jj_u32(_obj, _jobj, _member)        jj_byvar(_obj, _jobj, _member, u32, "%u")
#define jj_i32(_obj, _jobj, _member)        jj_byvar(_obj, _jobj, _member, i32, "%d")
#define jj_bool(_obj, _jobj, _member)       jj_byvar(_obj, _jobj, _member, bool, "%d")

typedef int jobj_mapper_f(jobj_t jobj);

#define __JOBJ_MAPPER(_obj, _type, _member)     \
int __jobj_map_##_member(jobj_t jobj)           \
{                                               \
    return jj_##_type(_obj, jobj, _member);     \
}   /* end */

#define DECLARE_JOBJ_MAPPER(_mapper)            _mapper(__JOBJ_MAPPER) os_fake_declare

#define __JOBJ_MAP_ENTRY(_obj, _type, _member)  __jobj_map_##_member,
#define JOBJ_MAP_INITER(_mapper)                { _mapper(__JOBJ_MAP_ENTRY) }

extern jobj_t 
__jobj_map(jobj_t jobj, jobj_mapper_f *map[], int count);

#define jobj_map(_jobj, _map)               __jobj_map(_jobj, _map, os_count_of(_map))

#define __jobj_mapf(_file, _map, _count)    __jobj_map(jobj_byfile(_file), _map, _count)
#define jobj_mapf(_file, _map)              __jobj_mapf(_file, _map, os_count_of(_map))

#define JOBJ_MAP(_jobj, _mapper) ({ \
    DECLARE_JOBJ_MAPPER(_mapper);   \
    jobj_mapper_f *__map[] = JOBJ_MAP_INITER(_mapper);\
    jobj_map(_jobj, __map);         \
})

#define JOBJ_MAPF(_file, _mapper)           JOBJ_MAP(jobj_byfile(_file), _mapper)
/******************************************************************************/
typedef struct {
    int type;
    
    uint32 flag;
    
    union {
        char *s;
        void *v;
        bool  b;
        int   i;
        
        char   *(*fs)(void);
        void   *(*fv)(void);
        bool    (*fb)(void);
        int     (*fi)(void);
        int     (*fo)(jobj_t jobj);
    } deft;
} jrule_t;

enum {
    JRULE_MUST      = 0x01,
    JRULE_DYNAMIC   = 0x02,
    JRULE_CONST     = 0x04,
};

#define J_RULE(_id, _type, _flag, _deft) [_id] = { \
    .type   = _type,            \
    .flag   = _flag,            \
    .deft   = {                 \
        .v  = (void *)(_deft),  \
    },                          \
}   /* end */
  
typedef struct {
    int count;
    jrule_t *rule;

    is_good_enum_f  *is_good_id;
    getnamebyid_f   *getnamebyid;
    getidbyname_f   *getidbyname;
} jrule_ops_t;

#define JRULE_OPS_INITER(_rule, _is_good_id, _getnamebyid, _getidbyname) { \
    .rule = _rule,                  \
    .count = os_count_of(_rule),    \
                                    \
    .is_good_id     = _is_good_id,  \
    .getnamebyid    = _getnamebyid, \
    .getidbyname    = _getidbyname, \
}   /* end */

typedef int jrule_apply_f(jobj_t jobj, jrule_t *rule, char *name, jobj_t jval);

extern int
jrule_repair(jobj_t jobj, jrule_t *rule, char *name, jobj_t jval);

extern int
jrule_apply(jobj_t jobj, jrule_ops_t *ops, jrule_apply_f *apply);

#endif /* __APP__ */
/******************************************************************************/
#endif /* __JAPI_H_82b58c7daf6248b381aac0f6971b0d3d__ */
