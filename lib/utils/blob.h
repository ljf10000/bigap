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
DECLARE_ENUM(blob_type, BLOB_TYPE_ENUM_MAPPER, BLOB_T_END);

static inline enum_ops_t *blob_type_ops_getter(void);

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
static inline uint32
blob_vsize(const blob_t *blob)
{
    uint32 vlen = blob_vlen(blob);
    
    if (vlen) {
        if (BLOB_T_STRING==blob_type(blob)) {
            /*
            * string length NOT include '\0'
            * string value keep '\0', for string function(strlen/strstr/...)
            */
            return BLOB_ALIGN(1 + vlen);
        } else {
            return BLOB_ALIGN(vlen);
        }
    } else {
        return 0;
    }
}

static inline char *
blob_value(const blob_t *blob)
{
    return blob_vlen(blob)?(blob_data(blob) + blob_ksize(blob)):NULL;
}

static inline uint32
blob_vpad_len(const blob_t *blob)
{
    uint32 vlen = blob_vlen(blob);
    
    if (vlen) {
        if (BLOB_T_STRING==blob_type(blob)) {
            return blob_vsize(blob) - vlen - 1;
        } else {
            return blob_vsize(blob) - vlen;
        }
    } else {
        return 0;
    }
}

static inline void *
blob_vpad(const blob_t *blob)
{
    uint32 vlen = blob_vlen(blob);
    
    if (vlen) {
        if (BLOB_T_STRING==blob_type(blob)) {
            return blob_value(blob) + vlen + 1;
        } else {
            return blob_value(blob) + vlen;
        }
    } else {
        return NULL;
    }
}

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

static inline void
blob_zero_pad(blob_t *blob)
{
    if (blob_klen(blob)) {
        os_memzero(blob_kpad(blob), blob_kpad_len(blob));
    }
    
    if (blob_vlen(blob)) {
        os_memzero(blob_vpad(blob), blob_vpad_len(blob));
    }
}

static inline bool
blob_eq(const blob_t *a, const blob_t *b)
{
	if (!a && !b) {
		return true;
    }
	else if (!a || !b) {
		return false;
    }
    else if (a==b) {
        return true;
    }

    uint32 size = blob_size(a);
	if (size != blob_size(b)) {
		return false;
    }
    
	return os_memeq(a, b, size);
}

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

static inline blob_t *
blob_getbyname(const blob_t *root, char *name)
{
    uint32 i, left;
    blob_t *blob;

    if (NULL==name) {
        return NULL;
    }

    blob_foreach(root, blob, i, left) {
        if (0==os_streq(name, blob_key(blob))) {
            return blob;
        }
    }
    
	return NULL;
}

static inline blob_t *
blob_getbyidx(const blob_t *root, uint32 idx)
{
    uint32 i, left;
    blob_t *blob;

#if USE_BLOB_COUNT
    if (false==IS_GOOD_ENUM(idx, blob_count(root))) {
        return NULL;
    }
#endif

    blob_foreach(root, blob, i, left) {
        if (i==idx) {
            return blob;
        }
    }
    
	return NULL;
}

typedef struct {
    bool array;

    union {
        char *name;
        uint32 idx;
    } u;
} blob_key_t;

static inline blob_t *
__blob_getby_r(blob_t *root, blob_key_t *keys, uint32 count)
{
    blob_key_t *key = &keys[0];
    blob_t *blob = NULL;
    
    if (key->array) {
        blob = blob_getbyidx(root, key->u.idx);
    } else {
        blob = blob_getbyname(root, key->u.name);
    }

    if (NULL==blob) {
        return NULL;
    } else if (1==count) {
        return blob;
    } else {
        return __blob_getby_r(blob, &keys[1], count-1);
    }
}

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

static inline blob_t *
blob_vgetby(blob_t *blob, const char *fmt, va_list args)
{
    uint32 idx;
    char *name;

    char *p = (char *)fmt;
    while(*p) {
        if ('%' != *p++) {
            debug_format("bad format:%s", fmt);
            
            return NULL;
        }

        switch(*p++) {
            case bfmt_array:
                idx = va_arg(args, uint32);
                blob = blob_getbyidx(blob, idx);

                break;
            case bfmt_object:
                name = va_arg(args, char *);
                blob = blob_getbyname(blob, name);

                break;
            default:
                debug_format("bad format:%s", fmt);
                
                return NULL;
        }

        if (NULL==blob) {
            return NULL;
        }
    }

    return blob;
}

static inline blob_t *
blob_sgetby(blob_t *root, const char *fmt, ...)
{
    va_list args;
    
    va_start(args, fmt);
    blob_t *blob = blob_vgetby(root, fmt, args);
    va_end(args);
    
    return blob;
}

static inline void
__blob_dump_header(const blob_t *blob, char *tag)
{
    os_printf("%s "
                "name:%s, "
#if USE_BLOB_COUNT
                "count:%u, "
#endif
                "size:%u, "
                "klen:%u, "
                "ksize:%u, "
                "vlen:%u, "
                "vsize:%u, "
                "%s", 
        tag?tag:__empty,
        blob_key(blob), 
#if USE_BLOB_COUNT
        blob_count(blob),
#endif
        blob_size(blob),
        blob_klen(blob),
        blob_ksize(blob),
        blob_vlen(blob),
        blob_vsize(blob),
        blob_type_ops_getter()->getname(blob_type(blob)));
}

static inline void
__blob_dump(const blob_t *blob, int level)
{    
    if (0==level) {
        __blob_dump_header(blob, "==DUMP BEGIN=="); os_printf(__crlf);
    }

    __printab(level); __blob_dump_header(blob, NULL);

    switch(blob_type(blob)) {
        case BLOB_T_OBJECT:
        case BLOB_T_ARRAY: {
            uint32 i, left;
            blob_t *p;
            os_printf(__crlf);
            
            blob_foreach(blob, p, i, left) {
                __blob_dump(p, level+1);
            }
            
        }   break;
        case BLOB_T_BOOL:
            os_println(":%d",   *(int *)blob_value(blob));
            break;
        case BLOB_T_INT32:
            os_println(":%d",   *(int32 *)blob_value(blob));
            break;
        case BLOB_T_UINT32:
            os_println(":%u",   *(uint32 *)blob_value(blob));
            break;
        case BLOB_T_FLOAT32:
            os_println(":%f",   *(float32_t *)blob_value(blob));
            break;
        case BLOB_T_INT64:
            os_println(":%lld", *(int64 *)blob_value(blob));
            break;
        case BLOB_T_UINT64:
            os_println(":%llu", *(uint64 *)blob_value(blob));
            break;
        case BLOB_T_FLOAT64:
            os_println(":%lf",  *(float64_t *)blob_value(blob));
            break;
        case BLOB_T_STRING:
            os_println(":%s",   (char *)blob_value(blob));
            break;
        case BLOB_T_BINARY:
            os_println(":%p", blob_value(blob));
            break;
        default:
            os_println(":%p", blob_value(blob));
            break;
    }

    if (0==level) {
        __blob_dump_header(blob, "==DUMP END  =="); os_printf(__crlf);
    }
}

static inline void
blob_dump(const blob_t *blob)
{
    if (__is_ak_debug_format) {
        __blob_dump(blob, 0);
    }
}

static inline void
__blob_dump_slice(slice_t *slice, char *tag)
{
    os_printf("%s" __crlf
        __tab "slice(size=%u, used=%u, remain=%u)" __crlf
        __tab "root(offset=%u, key=%s, type=%s, vlen=%u)" __crlf, 
        tag,
        slice_size(slice),
        (uint32)pointer_offsetof(slice_tail(slice), slice_data(slice)),
        slice_remain(slice),
        slice_offset(slice),
        blob_key(blob_root(slice)),
        blob_type_ops_getter()->getname(blob_type(blob_root(slice))),
        blob_vlen(blob_root(slice)));
}

typedef struct blob_rule {
    const char *name;
    uint32 type;
    uint32 flag;
    uint32 minsize; /* min value size */
    uint32 maxsize; /* max value size */
    bool (*validate)(const struct blob_rule *, blob_t *);
} blob_rule_t;

static inline int
blob_rule_find_byname(const char *name, const blob_rule_t rule[], int count)
{
    int i;

    for (i=0; i<count; i++) {
        if (rule[i].name && os_streq(rule[i].name, name)) {
            return i;
        }
    }
    
    return -ENOEXIST;
}

static inline int
blob_rule_find_bytypename(uint32 type, const char *name, const blob_rule_t rule[], int count)
{
    int i;

    for (i=0; i<count; i++) {
        if (type==rule[i].type && os_streq(rule[i].name, name)) {
            return i;
        }
    }
    
    return -ENOEXIST;
}

static inline bool
blob_check(uint32 type, const void *value, uint32 len)
{
    static blob_rule_t rule[BLOB_T_END] = {
        [BLOB_T_OBJECT] = {
            .flag   = BLOB_F_MIN,
            .minsize= sizeof(blob_t),
        },
        [BLOB_T_ARRAY] = {
            .flag   = BLOB_F_MIN,
            .minsize= sizeof(blob_t),
        },
        [BLOB_T_STRING] = {
            .flag   = BLOB_F_MIN | BLOB_F_PADZERO,
            .minsize= 1,
        },
        [BLOB_T_BINARY] = {
            .flag   = BLOB_F_MIN,
            .minsize= 1,
        },
        [BLOB_T_BOOL] = {
            .flag   = BLOB_F_FIXED,
            .minsize= sizeof(uint32),
        },
        [BLOB_T_INT32] = {
            .flag   = BLOB_F_FIXED,
            .minsize= sizeof(int32),
        },
        [BLOB_T_UINT32] = {
            .flag   = BLOB_F_FIXED,
            .minsize= sizeof(uint32),
        },
        [BLOB_T_FLOAT32] = {
            .flag   = BLOB_F_FIXED,
            .minsize= sizeof(float32_t),
        },
        [BLOB_T_INT64] = {
            .flag   = BLOB_F_FIXED,
            .minsize= sizeof(int64),
        },
        [BLOB_T_UINT64] = {
            .flag   = BLOB_F_FIXED,
            .minsize= sizeof(uint64),
        },
        [BLOB_T_FLOAT64] = {
            .flag   = BLOB_F_FIXED,
            .minsize= sizeof(float64_t),
        },
    };
    
    if (false==blob_type_ops_getter()->is_good(type)) {
        return false;
    }
    
    uint32 flag   = rule[type].flag;
    uint32 min    = rule[type].minsize;
    char *data      = (char *)value;
    
    if (os_hasflag(flag, BLOB_F_FIXED)) {
        if (len != min) {
            return false;
        }
    }

    if (os_hasflag(flag, BLOB_F_MIN)) {
        if (len < min) {
            return false;
        }
    }

    if (os_hasflag(flag, BLOB_F_MAX)) {
        if (len > rule[type].maxsize) {
            return false;
        }
    }

    if (os_hasflag(flag, BLOB_F_PADZERO)) {
        if (data[len - 1]) {
            return false;
        }
    }

    return true;
}

static inline int
blob_parse(blob_t *blob, blob_t *cache[], const blob_rule_t rule[], uint32 count)
{

    if (NULL==blob) {
        return os_assertV(-EINVAL1);
    }
    else if (NULL==cache) {
        return os_assertV(-EINVAL2);
    }
    else if (NULL==rule) {
        return os_assertV(-EINVAL3);
    }
    os_memzero(cache, sizeof(blob_t *) * count);

    char *name;
    blob_t *p;
    uint32 i, left, type;
    int found = 0;

    blob_foreach(blob, p, i, left) {
        type = blob_type(p);
        if (false==blob_type_ops_getter()->is_good(type)) {
            continue;
        }

        name = blob_key(p);
        if (NULL==name) {
            continue;
        }

        int idx = blob_rule_find_bytypename(type, name, rule, count);
        if (false==IS_GOOD_ENUM(idx, count)) {
            continue;
        }

        uint32 vlen = blob_vlen(p);
        if (false==blob_check(type, blob_value(p), vlen)) {
            continue;
        }
        else if (rule[idx].minsize && vlen < rule[idx].minsize) {
            continue;
        }
        else if (rule[idx].maxsize && vlen > rule[idx].maxsize) {
            continue;
        }
        else if (rule[idx].validate && !rule[idx].validate(&rule[idx], p)) {
            continue;
        }

        if (cache[idx]) {
            continue;
        } else {
            found++;

            cache[idx] = p;
        }
    }

    for (i=0; i<found; i++) {
        if (NULL==cache[i] && os_noflag(rule[i].flag, BLOB_F_MUST)) {
            return -ENOMUST;
        }
    }

    return found?found:-ENOEXIST;
}

static inline void
__blob_init(
    blob_t *blob, 
    uint32 type, 
    const char *name, 
    uint32 payload
)
{
    blob_type(blob) = type;
    blob_klen(blob) = name?os_strlen(name):0;
    blob_vlen(blob) = payload;
#if USE_BLOB_COUNT
    blob_count(blob)= 0;
#endif
}

static inline void
__blob_save_name(blob_t *blob, const char *name)
{
    if (name) {
        os_strmcpy(blob_key(blob), name, blob_klen(blob));
    }

    blob_zero_pad(blob);
}

static inline blob_t *
__blob_new(slice_t *slice, int type, const char *name, int payload)
{
	blob_t tmp;
    
    __blob_init(&tmp, type, name, payload);
    
    uint32 size = blob_size(&tmp);

    if (__is_ak_debug_trace && __is_ak_debug_format) {
        os_printf("blob_new" __crlf
            __tab "slice(size=%u, used=%u, remain=%u)" __crlf
            __tab "blob(type=%s, name=%s, payload=%u, size=%u)" __crlf, 
            slice_size(slice),
            (uint32)pointer_offsetof(slice_tail(slice), slice_data(slice)),
            slice_remain(slice),
            blob_type_ops_getter()->getname(type), 
            name, 
            payload, 
            size);
	}

    if (slice_remain(slice) < size) {
        if (slice_grow(slice, size) < 0) {
            return NULL;
        }
        else if (slice_remain(slice) < size) {
            return NULL;
        }
    }
    
    blob_t *blob = (blob_t *)slice_tail(slice);
    __blob_init(blob, type, name, payload);
    __blob_save_name(blob, name);

    blob_t *root = blob_root(slice);
    if (root && root!=blob) {
        blob_vlen(root) += size;
#if USE_BLOB_COUNT
        blob_count(root)++;
#endif
    }
    
    slice_put(slice, size);

	return blob;
}

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

static inline void *
__blob_nest_start(slice_t *slice, bool array, const char *name)
{
	int type = array?BLOB_T_ARRAY:BLOB_T_OBJECT;
    
	blob_t *new = __blob_new(slice, type, name, 0);
	if (NULL==new) {
        return NULL;
	}

    if (__is_ak_debug_trace && __is_ak_debug_format) {
        __blob_dump_slice(slice, "blob nest begin");
	}

    uint32 offset = (byte *)new - (byte *)slice_data(slice);
    
    return (void *)(uintptr_t)slice_offset_save(slice, offset);
}

static inline void
__blob_nest_end(slice_t *slice, void *cookie)
{
    uint32 size;
    blob_t *root;
    
    /*
    * when nest start, root have save klen
    */
    size = blob_vsize(blob_root(slice));
	slice_offset(slice) = (uint32)(uintptr_t)cookie;
	root = blob_root(slice);
	blob_vlen(root) += size;

    if (__is_ak_debug_trace && __is_ak_debug_format) {
        os_printf("blob nest end" __crlf
            __tab "slice(size=%u, used=%u, remain=%u)" __crlf
            __tab "root(added=%u, vlen=%u)" __crlf, 
            slice_size(slice),
            (uint32)pointer_offsetof(slice_tail(slice), slice_data(slice)),
            slice_remain(slice),
            size,
            blob_vlen(root));
	}
}

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

static inline blob_t *
blob_put(
    slice_t *slice, 
    uint32 type, 
    const char *name, 
    const void *value, 
    uint32 len
)
{
	blob_t *blob = __blob_new(slice, type, name, len);
	if (NULL==blob) {
		return NULL;
	}
    
    if (value) {
        char *v = blob_value(blob);
        
	    os_memcpy(v, value, len);
	    
        /*
        * string length NOT include '\0'
        * string value keep '\0', for string function(strlen/strstr/...)
        */
	    if (BLOB_T_STRING==type) {
            v[len] = 0;
	    }
	}
	
    if (__is_ak_debug_trace && __is_ak_debug_format) {
        __blob_dump_slice(slice, "blob_put");
	}

	return blob;
}

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

static inline blob_t *
blob_put_vsprintf(slice_t *slice, char *name, char *fmt, va_list args)
{
    va_list copy;

    va_copy(copy, args);
    int vsize = os_vsprintf_size(fmt, copy);
    va_end(copy);
    
    blob_t *blob = __blob_new(slice, BLOB_T_STRING, name, vsize);
    if (blob) {
        os_vsnprintf(blob_value(blob), 1+vsize, fmt, args);
    }

    return blob;
}

static inline blob_t *
blob_put_sprintf(slice_t *slice, char *name, char *fmt, ...)
{
    va_list args;
    
    va_start(args, fmt);
    blob_t *blob = blob_put_vsprintf(slice, name, fmt, args);
    va_end(args);
    
    return blob;
}

#ifdef __APP__
extern void
blob_ntoh(blob_t *blob);

extern void
blob_hton(blob_t *blob);

extern jobj_t
blob_btoj(blob_t *blob);

extern blob_t *
blob_jtob(slice_t *slice, jobj_t obj);
#endif

/******************************************************************************/
#endif /* __BLOB_H_f3a53c1649ab48a884ddae7dad5cdca4__ */
