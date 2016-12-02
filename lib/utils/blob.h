#ifndef __BLOB_H_f3a53c1649ab48a884ddae7dad5cdca4__
#define __BLOB_H_f3a53c1649ab48a884ddae7dad5cdca4__
/******************************************************************************/
enum {
    BLOB_F_FIXED    = 0x01,
    BLOB_F_MIN      = 0x02,
    BLOB_F_MAX      = 0x04,
    BLOB_F_PADZERO  = 0x08, /* last must zero */
    BLOB_F_MUST     = 0x10,
};

#if 1
#define BLOB_TYPE_ENUM_MAPPER(_)        \
    _(BLOB_T_OBJECT,    0, "object"),   \
    _(BLOB_T_ARRAY,     1, "array"),    \
    _(BLOB_T_STRING,    2, "string"),   \
    _(BLOB_T_BINARY,    3, "binary"),   \
    _(BLOB_T_BOOL,      4, "bool"),     \
    _(BLOB_T_INT32,     5, "int32"),    \
    _(BLOB_T_UINT32,    6, "uint32"),   \
    _(BLOB_T_FLOAT32,   7, "float32"),  \
    _(BLOB_T_INT64,     8, "int64"),    \
    _(BLOB_T_UINT64,    9, "uint64"),   \
    _(BLOB_T_FLOAT64,   10,"float64"),  \
    /* end */
EXTERN_ENUMER(blob_type, BLOB_TYPE_ENUM_MAPPER, BLOB_T_END);

EXTERN enum_ops_t *blob_type_ops(void);
EXTERN bool is_good_blob_type(int id);
EXTERN char *blob_type_getnamebyid(int id);
EXTERN int blob_type_getidbyname(const char *name);

#define BLOB_T_OBJECT   BLOB_T_OBJECT
#define BLOB_T_ARRAY    BLOB_T_ARRAY
#define BLOB_T_STRING   BLOB_T_STRING
#define BLOB_T_BINARY   BLOB_T_BINARY
#define BLOB_T_BOOL     BLOB_T_BOOL
#define BLOB_T_INT32    BLOB_T_INT32
#define BLOB_T_UINT32   BLOB_T_UINT32
#define BLOB_T_FLOAT32  BLOB_T_FLOAT32
#define BLOB_T_INT64    BLOB_T_INT64
#define BLOB_T_UINT64   BLOB_T_UINT64
#define BLOB_T_FLOAT64  BLOB_T_FLOAT64
#define BLOB_T_END      BLOB_T_END
#endif

static inline bool
is_blob_type_container(int type)
{
    return BLOB_T_OBJECT==type || BLOB_T_ARRAY==type;
}

#define BLOB_ALIGN(_len)    OS_ALIGN(_len, sizeof(uint32))

#ifndef USE_BLOB_COUNT
#define USE_BLOB_COUNT      1
#endif

typedef struct {
    byte    type;       /* enum blob_type */
    byte    flag;
    uint16  klen;       /* real key len, NOT include '\0' */

    uint32  vlen;       /* real value len, NOT include '\0' if value is string */
#if USE_BLOB_COUNT
    uint32  count;      /* if current node is object/array, the count is node's sub node count */
#endif

/*
    name/value align 4
    ----------------------
    |  name    | value   |
    ---------------------- 
*/
	char data[];            /* include name and value */
} blob_t;

#define blob_type(_blob)    (_blob)->type
#define blob_flag(_blob)    (_blob)->flag
#define blob_klen(_blob)    (_blob)->klen
#define blob_vlen(_blob)    (_blob)->vlen
#define blob_count(_blob)   (_blob)->count

static inline char *
blob_data(const blob_t *blob)
{
    return (char *)blob->data;
}

/*
* key + ['\0'] + [pad]
*/
static inline uint32
blob_ksize(const blob_t *blob)
{
    uint32 klen = blob_klen(blob);
    
    return klen?BLOB_ALIGN(1 + klen):0;
}

static inline uint32
blob_kpad_len(const blob_t *blob)
{
    uint32 klen = blob_klen(blob);
    
    return klen?(blob_ksize(blob) - klen - 1):0;
}

static inline char *
blob_key(const blob_t *blob)
{
    return blob_klen(blob)?blob_data(blob):NULL;
}

static inline void *
blob_kpad(const blob_t *blob)
{
    uint32 klen = blob_klen(blob);
    
    return klen?(blob_key(blob) + klen + 1):NULL;
}

/*
* value + ['\0'] + [pad]
*/
EXTERN uint32
blob_vsize(const blob_t *blob);

static inline char *
blob_value(const blob_t *blob)
{
    return blob_vlen(blob)?(blob_data(blob) + blob_ksize(blob)):NULL;
}

EXTERN uint32
blob_vpad_len(const blob_t *blob);

EXTERN void *
blob_vpad(const blob_t *blob);

static inline uint32
blob_dsize(const blob_t *blob)
{
    return blob_ksize(blob) + blob_vsize(blob);
}

static inline uint32
blob_dlen(const blob_t *blob)
{
    return blob_ksize(blob) + blob_vlen(blob);
}

static inline uint32
blob_size(const blob_t *blob)
{
    return blob_dsize(blob) + sizeof(blob_t);
}

#define blob_vpointer(_type, _blob)     ((_type *)blob_value(_blob))

static inline blob_t *
blob_root(slice_t *slice)
{
    return (blob_t *)slice_cookie(slice);
}

static inline blob_t *
blob_first(const blob_t *blob)
{
	return blob?blob_vpointer(blob_t, blob):NULL;
}

static inline blob_t *
blob_next(const blob_t *blob)
{
	return blob?(blob_t *)((char *)blob + blob_size(blob)):NULL;
}

EXTERN void
blob_zero_pad(blob_t *blob);

EXTERN bool
blob_eq(const blob_t *a, const blob_t *b);

static inline bool
blob_get_bool(const blob_t *blob)
{
	return !!*blob_vpointer(bool, blob);
}

static inline int32
blob_get_i32(const blob_t *blob)
{
    return *blob_vpointer(int32, blob);
}

static inline uint32
blob_get_u32(const blob_t *blob)
{
	return *blob_vpointer(uint32, blob);
}

static inline float32_t
blob_get_f32(const blob_t *blob)
{
	return *blob_vpointer(float32_t, blob);
}

static inline int64
blob_get_i64(const blob_t *blob)
{
	return *blob_vpointer(int64, blob);
}

static inline uint64
blob_get_u64(const blob_t *blob)
{
	return *blob_vpointer(uint64, blob);
}

static inline float64_t
blob_get_f64(const blob_t *blob)
{
	return *blob_vpointer(float64_t, blob);
}

static inline char *
blob_get_string(const blob_t *blob)
{
	return blob_vpointer(char, blob);
}

static inline byte *
blob_get_binary(const blob_t *blob)
{
	return blob_vpointer(byte, blob);
}

#if USE_BLOB_COUNT
#define blob_foreach(_root, _blob, _i, _left) \
	for (_i = 0, \
	        _left = _root?blob_vsize(_root):0, \
	        _blob = blob_first(_root); \
	     _i < blob_count(_root) \
	        && _blob \
	        && _left > 0 \
	        && blob_size(_blob) <= _left \
	        && blob_size(_blob) >= sizeof(blob_t); \
	     _i++, \
	        _left -= blob_size(_blob), \
	        _blob = blob_next(_blob))  \
    /* end */
#else
#define blob_foreach(_root, _blob, _i, _left) \
	for (_left = _root?blob_vsize(_root):0, \
	        _blob = blob_first(_root); \
	     _left > 0 \
	        && blob_size(_blob) <= _left \
	        && blob_size(_blob) >= sizeof(blob_t); \
	     _left -= blob_size(_blob), \
	        _blob = blob_next(_blob)) \
    /* end */
#endif

EXTERN blob_t *
blob_getbyname(const blob_t *root, char *name);

EXTERN blob_t *
blob_getbyidx(const blob_t *root, uint32 idx);

typedef struct {
    bool array;

    union {
        char *name;
        uint32 idx;
    } u;
} blob_key_t;

EXTERN blob_t *
__blob_getby_r(blob_t *root, blob_key_t *keys, uint32 count);

static inline blob_t *
__blob_getby(blob_t *root, blob_key_t *keys, uint32 count)
{
    if (NULL==root || NULL==keys || 0==count) {
        return NULL;
    }

    return __blob_getby_r(root, keys, count);
}
#define blob_getby(_root, _keys)    __blob_getby(_root, _keys, os_count_of(_keys))

enum {
    bfmt_array  = 'a',
    bfmt_object = 'o',
};

EXTERN blob_t *
blob_vgetby(blob_t *blob, const char *fmt, va_list args);

EXTERN blob_t *
blob_sgetby(blob_t *root, const char *fmt, ...);

EXTERN void
__blob_dump_header(const blob_t *blob, char *tag);

EXTERN void
__blob_dump(const blob_t *blob, int level);

static inline void
blob_dump(const blob_t *blob)
{
    if (__is_ak_debug_format) {
        __blob_dump(blob, 0);
    }
}

EXTERN void
__blob_dump_slice(slice_t *slice, char *tag);

typedef struct blob_rule {
    const char *name;
    uint32 type;
    uint32 flag;
    uint32 minsize; /* min value size */
    uint32 maxsize; /* max value size */
    bool (*validate)(const struct blob_rule *, blob_t *);
} blob_rule_t;

EXTERN int
blob_rule_find_byname(const char *name, const blob_rule_t rule[], int count);

EXTERN int
blob_rule_find_bytypename(uint32 type, const char *name, const blob_rule_t rule[], int count);

EXTERN bool
blob_check(uint32 type, const void *value, uint32 len);

EXTERN int
blob_parse(blob_t *blob, blob_t *cache[], const blob_rule_t rule[], uint32 count);

EXTERN void
__blob_init(
    blob_t *blob, 
    uint32 type, 
    const char *name, 
    uint32 payload
);

EXTERN void
__blob_save_name(blob_t *blob, const char *name);

EXTERN blob_t *
__blob_new(slice_t *slice, int type, const char *name, int payload);

static inline blob_t *
blob_root_init(slice_t *slice, int type, const char *name)
{
    slice_clean(slice);
    
    return __blob_new(slice, type, name, 0);
}

static inline blob_t *
blob_root_object(slice_t *slice)
{
    return blob_root_init(slice, BLOB_T_OBJECT, NULL);
}

static inline blob_t *
blob_root_array(slice_t *slice)
{
    return blob_root_init(slice, BLOB_T_ARRAY, NULL);
}

EXTERN void *
__blob_nest_start(slice_t *slice, bool array, const char *name);

EXTERN void
__blob_nest_end(slice_t *slice, void *cookie);

static inline void *
blob_object_start(slice_t *slice, const char *name)
{
    return __blob_nest_start(slice, false, name);
}
#define blob_object_end(_slice, _cookie)    __blob_nest_end(_slice, _cookie)

static inline void *
blob_array_start(slice_t *slice, const char *name)
{
    return __blob_nest_start(slice, true, name);
}
#define blob_array_end(_slice, _cookie)    __blob_nest_end(_slice, _cookie)

EXTERN blob_t *
blob_put(
    slice_t *slice, 
    uint32 type, 
    const char *name, 
    const void *value, 
    uint32 len
);

static inline blob_t *
blob_put_bool(slice_t *slice, const char *name, bool val)
{
    val = !!val;
    
	return blob_put(slice, BLOB_T_BOOL, name, &val, sizeof(val));
}

static inline blob_t *
blob_put_i32(slice_t *slice, const char *name, int32 val)
{
	return blob_put(slice, BLOB_T_INT32, name, &val, sizeof(val));
}

static inline blob_t *
blob_put_u32(slice_t *slice, const char *name, uint32 val)
{
	return blob_put(slice, BLOB_T_UINT32, name, &val, sizeof(val));
}

static inline blob_t *
blob_put_f32(slice_t *slice, const char *name, float32_t val)
{
	return blob_put(slice, BLOB_T_FLOAT32, name, &val, sizeof(val));
}

static inline blob_t *
blob_put_i64(slice_t *slice, const char *name, int64 val)
{
	return blob_put(slice, BLOB_T_INT64, name, &val, sizeof(val));
}

static inline blob_t *
blob_put_u64(slice_t *slice, const char *name, uint64 val)
{
	return blob_put(slice, BLOB_T_UINT64, name, &val, sizeof(val));
}

static inline blob_t *
blob_put_f64(slice_t *slice, const char *name, float64_t val)
{
	return blob_put(slice, BLOB_T_FLOAT64, name, &val, sizeof(val));
}

static inline blob_t *
blob_put_blob(slice_t *slice, blob_t *blob)
{
	return blob_put(slice, blob_type(blob), blob_key(blob), blob_value(blob), blob_vlen(blob));
}

static inline blob_t *
blob_put_string(slice_t *slice, const char *name, const char *str)
{
	return blob_put(slice, BLOB_T_STRING, name, str, os_strlen(str));
}

/*
* convert buffer to hex-string
*/
static inline blob_t *
blob_put_binary(slice_t *slice, const char *name, const void *binary, uint32 len)
{
	return blob_put(slice, BLOB_T_BINARY, name, binary, len);
}

EXTERN blob_t *
blob_put_vsprintf(slice_t *slice, char *name, char *fmt, va_list args);

EXTERN blob_t *
blob_put_sprintf(slice_t *slice, char *name, char *fmt, ...);

#ifdef __APP__
EXTERN void
blob_ntoh(blob_t *blob);

EXTERN void
blob_hton(blob_t *blob);

EXTERN jobj_t
blob_btoj(blob_t *blob);

EXTERN blob_t *
blob_jtob(slice_t *slice, jobj_t obj);
#endif

#ifdef __BOOT__
#include "weos/boot/blob.c"
#endif
/******************************************************************************/
#endif /* __BLOB_H_f3a53c1649ab48a884ddae7dad5cdca4__ */
