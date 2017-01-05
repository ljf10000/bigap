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
    int     d;
    int64   l;
    float64 f;
    char    *s;
    char    *j;
    jobj_t  a;
    jobj_t  o;
    void    *v;
} jvar_t;

#ifdef __APP__

#define json_type_end   (1+json_type_string)    // 7
#define jtype_bits      8

#if 1
/*
* ENUM: c enum macro
*
*/
#define JTYPE_ENUM_MAPPER(_) \
    _(jtype_null,   json_type_null,     "null"),    \
    _(jtype_bool,   json_type_boolean,  "bool"),    \
    _(jtype_double, json_type_double,   "double"),  \
    _(jtype_int,    json_type_int,      "int"),     \
    _(jtype_object, json_type_object,   "object"),  \
    _(jtype_array,  json_type_array,    "array"),   \
    _(jtype_string, json_type_string,   "string"),  \
    _(jtype_i32,    json_type_end+0,    "int32"),   \
    _(jtype_u32,    json_type_end+1,    "uint32"),  \
    _(jtype_f32,    json_type_end+2,    "float32"), \
    _(jtype_i64,    json_type_end+3,    "int64"),   \
    _(jtype_u64,    json_type_end+4,    "uint64"),  \
    _(jtype_f64,    json_type_end+5,    "float64"), \
    _(jtype_enum,   json_type_end+6,    "enum"),    \
    _(jtype_time,   json_type_end+7,    "time"),    \
    _(jtype_ip,     json_type_end+8,    "ip"),      \
    _(jtype_mac,    json_type_end+9,    "mac"),     \
    /* end */
DECLARE_ENUM(jtype, JTYPE_ENUM_MAPPER, jtype_end);

static inline enum_ops_t *jtype_ops(void);
static inline bool is_good_jtype(int id);
static inline char *jtype_getnamebyid(int id);
static inline int jtype_getidbyname(const char *name);

#define jtype_null      jtype_null
#define jtype_bool      jtype_bool
#define jtype_double    jtype_double
#define jtype_int       jtype_int
#define jtype_object    jtype_object
#define jtype_array     jtype_array
#define jtype_string    jtype_string
#define jtype_i32       jtype_i32
#define jtype_u32       jtype_u32
#define jtype_f32       jtype_f32
#define jtype_i64       jtype_i64
#define jtype_u64       jtype_u64
#define jtype_f64       jtype_f64
#define jtype_enum      jtype_enum
#define jtype_time      jtype_time
#define jtype_ip        jtype_ip
#define jtype_mac       jtype_mac
#define jtype_end       jtype_end
#endif

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
#define jobj_new_double(_v)     json_object_new_double(_v)

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

extern char *
jobj_string_ex(jobj_t obj, int flag);

extern char *
jobj_json(jobj_t obj);

extern int
jobj_string_len(jobj_t obj);

extern void
jobj_del(jobj_t obj, char *k);

extern void
jobj_add(jobj_t obj, char *k, jobj_t v);

#define jobj_get_bool(_obj)             json_object_get_boolean(_obj)
#define jobj_get_int(_obj)              json_object_get_int(_obj)
#define jobj_get_double(_obj)           json_object_get_double(_obj)

#define jobj_get_i32(_obj)              ((int32)json_object_get_int(_obj))
#define jobj_get_u32(_obj)              ((uint32)json_object_get_int(_obj))
#define jobj_get_f32(_obj)              ((float32)json_object_get_double(_obj))

#define jobj_get_i64(_obj)              ((int64)json_object_get_int64(_obj))
#define jobj_get_u64(_obj)              ((uint64)json_object_get_int64(_obj))
#define jobj_get_f64(_obj)              ((float64)json_object_get_double(_obj))

#define jobj_get_string(_obj)           ((char *)json_object_get_string(_obj))
#define jobj_get_string_len(_obj)       json_object_get_string_len(_obj)
#define jobj_get_binary(_obj)           ((byte *)json_object_get_string(_obj))
#define jobj_get_binary_len(_obj)       json_object_get_string_len(_obj)

extern jobj_t
jobj_get(jobj_t obj, char *key);

#define jarray_length(_obj)             json_object_array_length(_obj)

extern jobj_t
jarray_get(jobj_t array, int idx);

#define jarray_set(_jarray, _idx, _jval)    json_object_array_put_idx(_jarray, _idx, _jval)
#define jarray_add(_jarray, _jval)          json_object_array_add(_jarray, _jval)

#define jobj_foreach(_root, _k, _v)     json_object_object_foreach(_root, _k, _v)

#define jtok_new()                      json_tokener_new()
#define jtok_free(_tok)                 do{ json_tokener_free(_tok); _tok = NULL; }while(0)
#define jtok_set_flags(_tok, _flag)     json_tokener_set_flags(_tok, _flag)
#define jtok_parse_ex(_tok, _str, _len) json_tokener_parse_ex(_tok, _str, _len)

extern int
__jobj_add_bool(jobj_t obj, char *key, bool value);
#define jobj_add_bool(_obj, _key, _value)       __jobj_add_bool(_obj, (char *)_key, (bool)_value)

extern int
__jobj_add_int(jobj_t obj, char *key, int value);
#define jobj_add_int(_obj, _key, _value)        __jobj_add_int(_obj, (char *)_key, (int)_value)

extern int
__jobj_add_double(jobj_t obj, char *key, double value);
#define jobj_add_double(_obj, _key, _value)     __jobj_add_double(_obj, (char *)_key, (double)_value)

extern int
__jobj_add_i32(jobj_t obj, char *key, int32 value);
#define jobj_add_i32(_obj, _key, _value)        __jobj_add_i32(_obj, (char *)_key, (int32)_value)

extern int
__jobj_add_u32(jobj_t obj, char *key, uint32 value);
#define jobj_add_u32(_obj, _key, _value)        __jobj_add_u32(_obj, (char *)_key, (uint32)_value)

extern int
__jobj_add_f32(jobj_t obj, char *key, float32 value);
#define jobj_add_f32(_obj, _key, _value)        __jobj_add_f32(_obj, (char *)_key, (float32)_value)

extern int
__jobj_add_i64(jobj_t obj, char *key, int64 value);
#define jobj_add_i64(_obj, _key, _value)        __jobj_add_i64(_obj, (char *)_key, (int64)_value)

extern int
__jobj_add_u64(jobj_t obj, char *key, uint64 value);
#define jobj_add_u64(_obj, _key, _value)        __jobj_add_u64(_obj, (char *)_key, (uint64)_value)

extern int
__jobj_add_f64(jobj_t obj, char *key, float64 value);
#define jobj_add_f64(_obj, _key, _value)        __jobj_add_f64(_obj, (char *)_key, (float64)_value)

extern int
__jobj_add_string(jobj_t obj, char *key, char *value);
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
#define __jj_byvar(_obj, _jobj, _jname, _member, _type, _format) ({ \
    jobj_t __tmp = jobj_get(_jobj, #_jname); \
    if (__tmp) { \
        (_obj)->_member = jobj_get_##_type(__tmp); \
        debug_format(#_jname "=" _format, (_obj)->_member); \
    } \
    0; \
})  /* end */

#define __jj_bymapper(_obj, _jobj, _jname, _member, _mapper, _op) ({ \
    jobj_t __tmp = jobj_get(_jobj, #_jname); \
    if (__tmp) { \
        char *string = jobj_get_string(__tmp); \
        _op((_obj)->_member, _mapper(string)); \
        debug_format(#_jname "=%s", string); \
    } \
    0; \
})  /* end */

#define __jj_byeq(_obj, _jobj, _jname, _member, _mapper) \
        __jj_bymapper(_obj, _jobj, _jname, _member, _mapper, os_eq_buildin)

#define __jj_ip(_obj, _jobj, _jname, _member)       __jj_byeq(_obj, _jobj, _jname, _member, os_ipaddr)
#define __jj_time(_obj, _jobj, _jname, _member)     __jj_byeq(_obj, _jobj, _jname, _member, os_fulltime)
#define __jj_string(_obj, _jobj, _jname, _member)   __jj_byeq(_obj, _jobj, _jname, _member, os_strdup)
#define __jj_string_unsafe(_obj, _jobj, _jname, _member)    __jj_byeq(_obj, _jobj, _jname, _member, os_map_nothing)

#define __jj_mac(_obj, _jobj, _jname, _member)      __jj_bymapper(_obj, _jobj, _jname, _member, os_mac, os_maccpy)
#define __jj_strcpy(_obj, _jobj, _jname, _member)   __jj_bymapper(_obj, _jobj, _jname, _member, os_map_nothing, os_strcpy)

#define __jj_bool(_obj, _jobj, _jname, _member)     __jj_byvar(_obj, _jobj, _jname, _member, bool, "%d")
#define __jj_int(_obj, _jobj, _jname, _member)      __jj_byvar(_obj, _jobj, _jname, _member, int, "%d")
#define __jj_double(_obj, _jobj, _jname, _member)   __jj_byvar(_obj, _jobj, _jname, _member, double, "%f")
#define __jj_i32(_obj, _jobj, _jname, _member)      __jj_byvar(_obj, _jobj, _jname, _member, i32, "%d")
#define __jj_u32(_obj, _jobj, _jname, _member)      __jj_byvar(_obj, _jobj, _jname, _member, u32, "%u")
#define __jj_f32(_obj, _jobj, _jname, _member)      __jj_byvar(_obj, _jobj, _jname, _member, f32, "%f")
#define __jj_i64(_obj, _jobj, _jname, _member)      __jj_byvar(_obj, _jobj, _jname, _member, i64, "%lld")
#define __jj_u64(_obj, _jobj, _jname, _member)      __jj_byvar(_obj, _jobj, _jname, _member, u64, "%llu")
#define __jj_f64(_obj, _jobj, _jname, _member)      __jj_byvar(_obj, _jobj, _jname, _member, f64, "%llu")
/******************************************************************************/
#define jj_byvar(_obj, _jobj, _member, _type, _format) \
    __jj_byvar(_obj, _jobj, _member, _member, _type, _format)

#define jj_bymapper(_obj, _jobj, _member, _mapper, _op) \
    __jj_bymapper(_obj, _jobj, _member, _member, _mapper, _op)

#define jj_byeq(_obj, _jobj, _member, _mapper) \
    __jj_byeq(_obj, _jobj, _member, _member, _mapper)

#define jj_ip(_obj, _jobj, _member)             __jj_ip(_obj, _jobj, _member, _member)
#define jj_time(_obj, _jobj, _member)           __jj_time(_obj, _jobj, _member, _member)
#define jj_string(_obj, _jobj, _member)         __jj_string(_obj, _jobj, _member, _member)
#define jj_string_unsafe(_obj, _jobj, _member)  __jj_string_unsafe(_obj, _jobj, _member, _member)

#define jj_mac(_obj, _jobj, _member)        __jj_mac(_obj, _jobj, _member, _member)
#define jj_strcpy(_obj, _jobj, _member)     __jj_strcpy(_obj, _jobj, _member, _member)

#define jj_bool(_obj, _jobj, _member)       __jj_bool(_obj, _jobj, _member, _member)
#define jj_int(_obj, _jobj, _member)        __jj_int(_obj, _jobj, _member, _member)
#define jj_double(_obj, _jobj, _member)     __jj_double(_obj, _jobj, _member, _member)
#define jj_i32(_obj, _jobj, _member)        __jj_i32(_obj, _jobj, _member, _member)
#define jj_u32(_obj, _jobj, _member)        __jj_u32(_obj, _jobj, _member, _member)
#define jj_f32(_obj, _jobj, _member)        __jj_f32(_obj, _jobj, _member, _member)
#define jj_i64(_obj, _jobj, _member)        __jj_i64(_obj, _jobj, _member, _member)
#define jj_u64(_obj, _jobj, _member)        __jj_u64(_obj, _jobj, _member, _member)
#define jj_f64(_obj, _jobj, _member)        __jj_f64(_obj, _jobj, _member, _member)
/******************************************************************************/
typedef int jobj_mapper_f(jobj_t jobj);

extern jobj_t 
__jobj_map(jobj_t jobj, jobj_mapper_f *map[], int count);
#define jobj_map(_jobj, _map)               __jobj_map(_jobj, _map, os_count_of(_map))

#define __jobj_mapfile(_file, _map, _count) __jobj_map(jobj_byfile(_file), _map, _count)
#define jobj_mapfile(_file, _map)           __jobj_mapfile(_file, _map, os_count_of(_map))


#define JOBJ_FUNC_NAME_MEMBER_MAPPER(_obj, _type, _jname, _member) \
int __jobj_map_##_jname##_##_member##_helper(jobj_t jobj)   \
{                                                           \
    return __jj_##_type(_obj, jobj, _jname, _member);       \
}   /* end */
#define JOBJ_FUNC_NAME_MEMBER_HELPER(_obj, _type, _jname, _member)  __jobj_map_##_jname##_##_member##_helper,


#define JOBJ_FUNC_MEMBER_MAPPER(_obj, _type, _member, _default) \
int __jobj_map_##_member##_helper(jobj_t jobj)  \
{                                               \
    return jj_##_type(_obj, jobj, _member);     \
}   /* end */
#define JOBJ_FUNC_MEMBER_HELPER(_obj, _type, _member, _default)     __jobj_map_##_member##_helper,


#define JOBJ_MAP_BY(_jobj, _mapper, _func_mapper, _func_helper) ({ \
    _mapper(_func_mapper) os_fake_declare;                  \
    jobj_mapper_f *__map[] = { _mapper(_func_helper) };     \
    jobj_map(_jobj, __map);                                 \
})
#define JOBJ_MAPFILE_BY(_file, _mapper, _func_mapper, _func_helper)  \
    JOBJ_MAP_BY(jobj_byfile(_file), _mapper, _func_mapper, _func_helper)


#define JOBJ_MAP(_jobj, _mapper) ({ \
    _mapper(JOBJ_FUNC_MEMBER_MAPPER) os_fake_declare; \
    jobj_mapper_f *__map[] = { _mapper(JOBJ_FUNC_MEMBER_HELPER) }; \
    jobj_map(_jobj, __map);         \
})
#define JOBJ_MAPFILE(_file, _mapper)        JOBJ_MAP(jobj_byfile(_file), _mapper)


#define __JOBJ_MAP_INIT_HELPER(_obj, _type, _member, _default)  ._member = _default,
#define JOBJ_MAP_INITER(_mapper)            { _mapper(__JOBJ_MAP_INIT_HELPER) }


#define JOBJ_MAP_NAME_MEMBER(_jobj, _mapper) \
    JOBJ_MAP_BY(_jobj, _mapper, JOBJ_FUNC_NAME_MEMBER_MAPPER, JOBJ_FUNC_NAME_MEMBER_HELPER)
#define JOBJ_MAPFILE_NAME_MEMBER(_file, _mapper) \
    JOBJ_MAPFILE_BY(_file, _mapper, JOBJ_FUNC_NAME_MEMBER_MAPPER, JOBJ_FUNC_NAME_MEMBER_HELPER)


#define JOBJ_MAP_MEMBER(_jobj, _mapper) \
    JOBJ_MAP_BY(_jobj, _mapper, JOBJ_FUNC_MEMBER_MAPPER, JOBJ_FUNC_MEMBER_HELPER)
#define JOBJ_MAPFILE_MEMBER(_file, _mapper) \
    JOBJ_MAPFILE_BY(_file, _mapper, JOBJ_FUNC_MEMBER_MAPPER, JOBJ_FUNC_MEMBER_HELPER)
/******************************************************************************/
enum {
    JRULE_F_MUST    = 0x0001,
    JRULE_F_DROP    = 0x0002,
    
    JRULE_F_CHECKER = 0x0010,
    JRULE_F_BORDER  = 0x0020,
};

#define JURLE_F_VALID_LIST          \
    JRULE_F_MUST,                   \
    JRULE_F_DROP,                   \
                                    \
    JRULE_F_CHECKER,                \
    JRULE_F_CHECKER | JRULE_F_MUST, \
                                    \
    JRULE_F_BORDER,                 \
    JRULE_F_BORDER | JRULE_F_MUST,  \
    /* end */

extern bool
is_good_jrule_flag(int flag);

typedef struct jrule_s jrule_t;
typedef union jrule_var_u jrule_var_t;

typedef int jrule_o2j_f(const jrule_t *rule, void *obj, jobj_t jobj);
typedef int jrule_j2o_f(const jrule_t *rule, void *obj, jobj_t jobj);
typedef int jrule_check_f(const jrule_t *rule, jobj_t jobj);

typedef jrule_t *jrule_get_rules_f(void);
typedef enum_ops_t *jrule_get_enum_ops_f(void);

union jrule_var_u {
    void    *v;
    char    *s;
    
    bool    b;
    int     i;
    double  d;
    
    int32   i32;
    uint32  u32;
    float32 f32;

    int64   i64;
    uint64  u64;
    float64 f64;

    jrule_o2j_f *o2j;
    jrule_j2o_f *j2o;
    jrule_check_f *check;

    jrule_get_rules_f *get_rules;
    jrule_get_enum_ops_f *get_enum_ops;
};

#define JRULE_VAR_INITER(_member, _value)   {._member = _value }
#define JRULE_VAR_BOOL(_value)              JRULE_VAR_INITER(b, _value)
#define JRULE_VAR_INT(_value)               JRULE_VAR_INITER(i, _value)
#define JRULE_VAR_DOUBLE(_value)            JRULE_VAR_INITER(d, _value)
#define JRULE_VAR_INT32(_value)             JRULE_VAR_INITER(i32, _value)
#define JRULE_VAR_INT64(_value)             JRULE_VAR_INITER(i64, _value)
#define JRULE_VAR_POINTER(_value)           JRULE_VAR_INITER(v, _value)

#define JRULE_VAR_STRING(_value)            JRULE_VAR_POINTER(_value)
#define JRULE_VAR_ENUM(_value)              JRULE_VAR_POINTER(_value)
#define JRULE_VAR_RULES(_value)             JRULE_VAR_POINTER(_value)
#define JRULE_VAR_CHECKER(_value)           JRULE_VAR_POINTER(_value)
#define JRULE_VAR_O2J(_value)               JRULE_VAR_POINTER(_value)
#define JRULE_VAR_J2O(_value)               JRULE_VAR_POINTER(_value)

#define JRULE_VAR_STRDUP                    JRULE_VAR_POINTER(jrule_strdup)
#define JRULE_VAR_STRCPY                    JRULE_VAR_POINTER(jrule_strcpy)
#define JRULE_VAR_STRASSIGN                 JRULE_VAR_POINTER(jrule_strassign)
#define JRULE_VAR_NULL                      JRULE_VAR_POINTER(NULL)

struct jrule_s {
    char *name;
    
    int offset;
    int size;
    
    int type;
    uint32 flag;

    /*
    * serializer:   obj==>jobj
    * unserializer: obj<==jobj
    *
    *
    * 1. type is atomic (bool/int/double/...)
    *       deft as default value
    *   if JRULE_F_CHECKER
    *       serialize as checker
    *       unserialize not-used
    *   else if JRULE_F_BORDER
    *       serialize/unserialize as min/max
    *   else
    *       serialize/unserialize not-used
    * 2. type is ip/mac/time
    *       not support JRULE_F_CHECKER/JRULE_F_BORDER
    *       serialize/unserialize not-used
    *       deft as default value
    * 3. type is enum
    *       not support JRULE_F_CHECKER/JRULE_F_BORDER
    *       serialize not-used
    *       unserialize as get_enum_ops
    *       deft as default value
    * 4. type is string
    *       serialize not-used
    *       unserialize as j2o
    *       deft as default value
    * 5. type is object
    *       only support JRULE_F_MUST
    *       serialize/unserialize not-used
    *       deft as rules
    * 6. type is array
    *       only support JRULE_F_MUST
    *       serialize as o2j
    *       unserialize as j2o
    *       deft as rules
    */
    jrule_var_t serialize, unserialize, deft;
};

#define jmethod_min(_rule, _member)     (_rule)->serialize._member
#define jmethod_max(_rule, _member)     (_rule)->unserialize._member
#define jmethod_deft(_rule, _member)    (_rule)->deft._member

static inline jrule_o2j_f *
jmethod_o2j(const jrule_t *rule)
{
    return rule->serialize.o2j;
}

static inline jrule_j2o_f *
jmethod_j2o(const jrule_t *rule)
{
    return rule->unserialize.j2o;
}

static inline jrule_check_f *
jmethod_check(const jrule_t *rule)
{
    return rule->serialize.check;
}

static inline jrule_get_rules_f *
jmethod_get_rules(const jrule_t *rule)
{
    return rule->deft.get_rules;
}

static inline jrule_get_enum_ops_f *
jmethod_get_enum_ops(const jrule_t *rule)
{
    return rule->unserialize.get_enum_ops;
}

#define JRULER(_offset, _member, _name, \
            _type, _size, _flag,\
            _serialize_initer,  \
            _unserialize_initer,\
            _deft_initer) {     \
    .name   = _name,            \
    .offset = _offset,          \
    .size   = _size,            \
    .type   = jtype_##_type,    \
    .flag   = _flag,            \
    .serialize  = _serialize_initer,    \
    .unserialize= _unserialize_initer,  \
    .deft       = _deft_initer,         \
}   /* end */

#define JRULER_END  { .offset = -1 }

#define JRULE_FOREACH(_rule, _rules) \
    for (_rule=(const jrule_t *)&_rules[0]; _rule->offset >= 0; _rule++)

#define JRULE_OBJ_MEMBER_ADDRESS(_rule, _obj) \
    ((char *)(_obj) + (_rule)->offset)

extern int
jrule_strassign(const jrule_t *rule, void *obj, jobj_t jobj);

extern int
jrule_strdup(const jrule_t *rule, void *obj, jobj_t jobj);

extern int
jrule_strcpy(const jrule_t *rule, void *obj, jobj_t jobj);

extern int
jrule_time_o2j(const jrule_t *rule, void *obj, jobj_t jobj);

extern int
jrule_time_j2o(const jrule_t *rule, void *obj, jobj_t jobj);

extern int
jrule_ip_o2j(const jrule_t *rule, void *obj, jobj_t jobj);

extern int
jrule_ip_j2o(const jrule_t *rule, void *obj, jobj_t jobj);

extern int
jrule_mac_o2j(const jrule_t *rule, void *obj, jobj_t jobj);

extern int
jrule_mac_j2o(const jrule_t *rule, void *obj, jobj_t jobj);

extern int
jrule_selfcheck(const jrule_t *rules);

/*
* obj maybe NULL: just do nothing
*/
extern jobj_t
jrule_o2j_ex(const jrule_t *rules, void *obj);

/*
* obj maybe NULL: just do nothing
*/
extern int
jrule_o2j(const jrule_t *rules, void *obj, jobj_t jobj);

/*
* obj maybe NULL: just for check
* jobj maybe NULL: just do nothing
*/
extern int
jrule_j2o(const jrule_t *rules, void *obj, jobj_t jobj);

extern const jrule_t *
jrule_getbyname(const jrule_t *rules, char *name);

#if 0
/*
* JRULE: json rule macro
*/
#define XXX_JRULE_MAPPER(_) \
    _(OFFSET_A, MEMVER_A, "NAME_A", TYPE_A, SIZE_A, FLAG_A, \
        SERIALIZE_INITER_A, UNSERIALIZE_INITER_A, DEFT_INITER_A),
    _(OFFSET_B, MEMVER_B, "NAME_B", TYPE_B, SIZE_B, FLAG_B, \
        SERIALIZE_INITER_B, UNSERIALIZE_INITER_B, DEFT_INITER_B),
    _(OFFSET_C, MEMVER_C, "NAME_C", TYPE_C, SIZE_C, FLAG_C, \
        SERIALIZE_INITER_C, UNSERIALIZE_INITER_C, DEFT_INITER_C),
    /* end */
DECLARE_JRULER(xxx, XXX_JRULE_MAPPER);

static inline jrule_t *xxx_jrules(void);
#endif

#define DECLARE_JRULER(_mod, _mapper) \
    static inline jrule_t *         \
    _mod##_jrules(void)             \
    {                               \
        static jrule_t rule[] = {   \
            _mapper(JRULER)         \
            JRULER_END,             \
        };                          \
                                    \
        return rule;                \
    }                               \
                                    \
    os_fake_declare                 \
    /* end */
#endif /* __APP__ */
/******************************************************************************/
#endif /* __JAPI_H_82b58c7daf6248b381aac0f6971b0d3d__ */
