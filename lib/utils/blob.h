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

#define BLOB_TYPE_LIST(_) \
    _(BLOB_T_EMPTY,     0, "empty"),    \
    _(BLOB_T_OBJECT,    1, "object"),   \
    _(BLOB_T_ARRAY,     2, "array"),    \
    _(BLOB_T_STRING,    3, "string"),   \
    _(BLOB_T_BINARY,    4, "binary"),   \
    _(BLOB_T_BOOL,      5, "bool"),     \
    _(BLOB_T_INT32,     6, "int32"),    \
    _(BLOB_T_UINT32,    7, "uint32"),   \
    _(BLOB_T_FLOAT32,   8, "float32"),  \
    _(BLOB_T_INT64,     9, "int64"),    \
    _(BLOB_T_UINT64,    10,"uint64"),   \
    _(BLOB_T_FLOAT64,   11,"float64"),  \
    /* end */

#if 1 /* just for sourceinsight */
#define BLOB_T_EMPTY    BLOB_T_EMPTY
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
#endif /* just for sourceinsight */

static inline bool is_good_blob_type(int type);
static inline char *blob_type_string(int type);
static inline int blob_type_idx(char *type_string);
DECLARE_ENUM(blob_type, BLOB_TYPE_LIST, BLOB_T_END);

static inline bool
is_blob_type_container(int type)
{
    return BLOB_T_OBJECT==type || BLOB_T_ARRAY==type;
}

#define BLOB_ALIGN(_len)    OS_ALIGN(_len, sizeof(uint32_t))

typedef struct {
    uint8_t     type;       /* enum blob_type */
    uint8_t     klen;       /* real key len, NOT include '\0' */
    uint16_t    flag;
    
    uint32_t    vlen;       /* real value len, NOT include '\0' if value is string */
    uint32_t    count;      /* if current node is object/array, the count is node's sub node count */

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

/*
* key + ['\0'] + [pad]
*/
static inline uint32_t
blob_ksize(const blob_t *blob)
{
    uint32_t klen = blob_klen(blob);
    
    return klen?BLOB_ALIGN(1 + klen):0;
}

static inline uint32_t
blob_kpad_len(const blob_t *blob)
{
    uint32_t klen = blob_klen(blob);
    
    return klen?(blob_ksize(blob) - klen - 1):0;
}

static inline char *
blob_key(const blob_t *blob)
{
    return blob_klen(blob)?(char *)blob->data:NULL;
}

static inline void *
blob_kpad(const blob_t *blob)
{
    uint32_t klen = blob_klen(blob);
    
    return klen?(blob_key(blob) + klen + 1):NULL;
}

/*
* value + ['\0'] + [pad]
*/
static inline uint32_t
blob_vsize(const blob_t *blob)
{
    uint32_t vlen = blob_vlen(blob);
    
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
    return blob_vlen(blob)?((char *)blob->data + blob_ksize(blob)):NULL;
}

static inline uint32_t
blob_vpad_len(const blob_t *blob)
{
    uint32_t vlen = blob_vlen(blob);
    
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
    uint32_t vlen = blob_vlen(blob);
    
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

static inline uint32_t
blob_dsize(const blob_t *blob)
{
    return blob_ksize(blob) + blob_vsize(blob);
}

static inline uint32_t
blob_dlen(const blob_t *blob)
{
    return blob_ksize(blob) + blob_vlen(blob);
}

static inline uint32_t
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

    uint32_t size = blob_size(a);
	if (size != blob_size(b)) {
		return false;
    }
    
	return os_memeq(a, b, size);
}

static inline int32_t
blob_get_i32(const blob_t *blob)
{
    return *blob_vpointer(int32_t, blob);
}

static inline uint32_t
blob_get_u32(const blob_t *blob)
{
	return *blob_vpointer(uint32_t, blob);
}

static inline float32_t
blob_get_f32(const blob_t *blob)
{
	return *blob_vpointer(float32_t, blob);
}

static inline int64_t
blob_get_i64(const blob_t *blob)
{
	return *blob_vpointer(int64_t, blob);
}

static inline uint64_t
blob_get_u64(const blob_t *blob)
{
	return *blob_vpointer(uint64_t, blob);
}

static inline float64_t
blob_get_f64(const blob_t *blob)
{
	return *blob_vpointer(float64_t, blob);
}

static inline bool
blob_get_bool(const blob_t *blob)
{
	return !!blob_get_u32(blob);
}

static inline const char *
blob_get_string(const blob_t *blob)
{
	return blob_value(blob);
}

#if 1
#define blob_foreach(_root, _blob, _i, _left) \
	for (_i = 0, _left = _root?blob_vsize(_root):0, _blob = blob_first(_root); \
	     _i < blob_count(_root) && _blob && _left > 0 && blob_size(_blob) <= _left && blob_size(_blob) >= sizeof(blob_t); \
	     _i++, _left -= blob_size(_blob), _blob = blob_next(_blob))  \
    /* end */
#else
#define blob_foreach(_root, _blob, _i, _left) \
	for (_left = _root?blob_vsize(_root):0, _blob = blob_first(_root); \
	     _left > 0 && blob_size(_blob) <= _left && blob_size(_blob) >= sizeof(blob_t); \
	     _left -= blob_size(_blob), _blob = blob_next(_blob)) \
    /* end */
#endif

static inline blob_t *
blob_getbyname(const blob_t *root, char *name)
{
    uint32_t i, left;
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
blob_getbyidx(const blob_t *root, uint32_t idx)
{
    uint32_t i, left, count = blob_count(root);
    blob_t *blob;

    if (false==is_good_enum(idx, count)) {
        return NULL;
    }

    blob_foreach(root, blob, i, left) {
        if (i==idx) {
            return blob;
        }
    }
    
	return NULL;
}

static inline void
__blob_dump_header(const blob_t *blob, char *tag)
{
    os_printf("%s "
                "name:%s, "
                "count:%u, "
                "size:%u, "
                "klen:%u, "
                "ksize:%u, "
                "vlen:%u, "
                "vsize:%u, "
                "%s", 
        tag?tag:__empty,
        blob_key(blob), 
        blob_count(blob),
        blob_size(blob),
        blob_klen(blob),
        blob_ksize(blob),
        blob_vlen(blob),
        blob_vsize(blob),
        blob_type_string(blob_type(blob)));
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
            uint32_t i, left;
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
            os_println(":%d",   *(int32_t *)blob_value(blob));
            break;
        case BLOB_T_UINT32:
            os_println(":%u",   *(uint32_t *)blob_value(blob));
            break;
        case BLOB_T_FLOAT32:
            os_println(":%f",   *(float32_t *)blob_value(blob));
            break;
        case BLOB_T_INT64:
            os_println(":%lld", *(int64_t *)blob_value(blob));
            break;
        case BLOB_T_UINT64:
            os_println(":%llu", *(uint64_t *)blob_value(blob));
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
        case BLOB_T_EMPTY:
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
    if (__is_ak_debug_blob) {
        __blob_dump(blob, 0);
    }
}

static inline void
__blob_dump_slice(slice_t *slice, char *tag)
{
    os_printf("%s" __crlf
        __tab "slice(size=%d, used=%d, remain=%d)" __crlf
        __tab "root(offset=%d, key=%s, type=%s, vlen=%d)" __crlf, 
        tag,
        slice_size(slice),
        slice_tail(slice) - slice_data(slice),
        slice_remain(slice),
        slice_offset(slice),
        blob_key(blob_root(slice)),
        blob_type_string(blob_type(blob_root(slice))),
        blob_vlen(blob_root(slice)));
}

typedef struct blob_rule {
    const char *name;
    uint32_t type;
    uint32_t flag;
    uint32_t minsize; /* min value size */
    uint32_t maxsize; /* max value size */
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
blob_rule_find_bytypename(uint32_t type, const char *name, const blob_rule_t rule[], int count)
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
blob_check(uint32_t type, const void *value, uint32_t len)
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
        [BLOB_T_EMPTY] = {
            .flag   = BLOB_F_FIXED,
        },
        [BLOB_T_BOOL] = {
            .flag   = BLOB_F_FIXED,
            .minsize= sizeof(uint32_t),
        },
        [BLOB_T_INT32] = {
            .flag   = BLOB_F_FIXED,
            .minsize= sizeof(int32_t),
        },
        [BLOB_T_UINT32] = {
            .flag   = BLOB_F_FIXED,
            .minsize= sizeof(uint32_t),
        },
        [BLOB_T_FLOAT32] = {
            .flag   = BLOB_F_FIXED,
            .minsize= sizeof(float32_t),
        },
        [BLOB_T_INT64] = {
            .flag   = BLOB_F_FIXED,
            .minsize= sizeof(int64_t),
        },
        [BLOB_T_UINT64] = {
            .flag   = BLOB_F_FIXED,
            .minsize= sizeof(uint64_t),
        },
        [BLOB_T_FLOAT64] = {
            .flag   = BLOB_F_FIXED,
            .minsize= sizeof(float64_t),
        },
    };
    
    if (false==is_good_blob_type(type)) {
        return false;
    }
    
    uint32_t flag   = rule[type].flag;
    uint32_t min    = rule[type].minsize;
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
blob_parse(blob_t *blob, blob_t *cache[], const blob_rule_t rule[], uint32_t count)
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
    uint32_t i, left, type;
    int found = 0;

    blob_foreach(blob, p, i, left) {
        type = blob_type(p);
        if (false==is_good_blob_type(type)) {
            continue;
        }

        name = blob_key(p);
        if (NULL==name) {
            continue;
        }

        int idx = blob_rule_find_bytypename(type, name, rule, count);
        if (false==is_good_enum(idx, count)) {
            continue;
        }

        uint32_t vlen = blob_vlen(p);
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
    uint32_t type, 
    const char *name, 
    uint32_t payload
)
{
    blob_type(blob) = type;
    blob_klen(blob) = name?os_strlen(name):0;
    blob_vlen(blob) = payload;
    blob_count(blob)= 0;
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
    
    int size = blob_size(&tmp);

    if (__is_ak_debug_trace && __is_ak_debug_blob) {
        os_printf("blob_new" __crlf
            __tab "slice(size=%d, used=%d, remain=%d)" __crlf
            __tab "blob(type=%s, name=%s, payload=%d, size=%d)" __crlf, 
            slice_size(slice),
            slice_tail(slice) - slice_data(slice),
            slice_remain(slice),
            blob_type_string(type), 
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
        blob_count(root)++;
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

    if (__is_ak_debug_trace && __is_ak_debug_blob) {
        __blob_dump_slice(slice, "blob nest begin");
	}

    uint32_t offset = (byte *)new - (byte *)slice_data(slice);
    
    return (void *)slice_offset_save(slice, offset);
}

static inline void
__blob_nest_end(slice_t *slice, void *cookie)
{
    uint32_t size;
    blob_t *root;
    
    /*
    * when nest start, root have save klen
    */
    size = blob_vsize(blob_root(slice));
	slice_offset(slice) = (uint32_t)cookie;
	root = blob_root(slice);
	blob_vlen(root) += size;

    if (__is_ak_debug_trace && __is_ak_debug_blob) {
        os_printf("blob nest end" __crlf
            __tab "slice(size=%d, used=%d, remain=%d)" __crlf
            __tab "root(added=%d, vlen=%d)" __crlf, 
            slice_size(slice),
            slice_tail(slice) - slice_data(slice),
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
    uint32_t type, 
    const char *name, 
    const void *value, 
    uint32_t len
)
{
	blob_t *blob = __blob_new(slice, type, name, len);
	if (NULL==blob) {
		return NULL;
	}
    
    if (value) {
        char *value = blob_value(blob);
        
	    os_memcpy(value, value, len);
	    
        /*
        * string length NOT include '\0'
        * string value keep '\0', for string function(strlen/strstr/...)
        */
	    if (BLOB_T_STRING==type) {
            value[len] = 0;
	    }
	}
	
    if (__is_ak_debug_trace && __is_ak_debug_blob) {
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
blob_put_i32(slice_t *slice, const char *name, int32_t val)
{
	return blob_put(slice, BLOB_T_INT32, name, &val, sizeof(val));
}

static inline blob_t *
blob_put_u32(slice_t *slice, const char *name, uint32_t val)
{
	return blob_put(slice, BLOB_T_UINT32, name, &val, sizeof(val));
}

static inline blob_t *
blob_put_f32(slice_t *slice, const char *name, float32_t val)
{
	return blob_put(slice, BLOB_T_FLOAT32, name, &val, sizeof(val));
}

static inline blob_t *
blob_put_i64(slice_t *slice, const char *name, int64_t val)
{
	return blob_put(slice, BLOB_T_INT64, name, &val, sizeof(val));
}

static inline blob_t *
blob_put_u64(slice_t *slice, const char *name, uint64_t val)
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
blob_put_binary(slice_t *slice, const char *name, const void *binary, uint32_t len)
{
	return blob_put(slice, BLOB_T_BINARY, name, binary, len);
}

static inline int
blob_put_vsprintf(slice_t *slice, char *name, char *fmt, va_list args)
{
    int vsize = os_vsprintf_size(fmt, args);
    blob_t *blob = __blob_new(slice, BLOB_T_STRING, name, vsize);
    if (blob) {
        os_vsnprintf(blob_value(blob), vsize, fmt, args);
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
static inline void
__blob_byteorder(blob_t *blob, bool ntoh)
{
    
    if (ntoh) {
        blob_flag(blob) = bswap_16(blob_flag(blob));
        blob_vlen(blob) = bswap_32(blob_vlen(blob));
        blob_count(blob)= bswap_32(blob_count(blob));
    }
    
    switch(blob_type(blob)) {
        case BLOB_T_OBJECT:
        case BLOB_T_ARRAY: {
            uint32_t i, left;
            blob_t *p;
            
            blob_foreach(blob, p, i, left) {
                __blob_byteorder(p, ntoh);
            }
        }   break;
        case BLOB_T_INT64:
        case BLOB_T_UINT64:
        case BLOB_T_FLOAT64:
            *blob_vpointer(uint64_t, blob) = bswap_64(blob_get_u64(blob));
            break;
        case BLOB_T_BOOL:
        case BLOB_T_INT32:
        case BLOB_T_UINT32:
        case BLOB_T_FLOAT32:
            *blob_vpointer(uint32_t, blob) = bswap_32(blob_get_u32(blob));
            break;
        case BLOB_T_STRING:
        case BLOB_T_BINARY:
        case BLOB_T_EMPTY:
        default:
            /* do nothinig */
            break;
    }
    
    if (false==ntoh) {
        blob_flag(blob) = bswap_16(blob_flag(blob));
        blob_vlen(blob) = bswap_32(blob_vlen(blob));
        blob_count(blob)= bswap_32(blob_count(blob));
    }
}

static inline void
blob_ntoh(blob_t *blob)
{
    __blob_byteorder(blob, true);
}

static inline void
blob_hton(blob_t *blob)
{
    __blob_byteorder(blob, false);
}

static inline jobj_t
__blob_jobj(blob_t *blob)
{
    switch(blob_type(blob)) {
        case BLOB_T_OBJECT: return jobj_new_object();
        case BLOB_T_ARRAY:  return jobj_new_array();
        default: return NULL;
    }
}

static inline jobj_t
__blob_btoj(blob_t *blob, jobj_t root, int level)
{
    char *name      = blob_key(blob);
    uint32_t type   = blob_type(blob);
    int err = 0;

    if (NULL==root) {
        return NULL;
    }
    
    switch(type) {
        case BLOB_T_OBJECT:
        case BLOB_T_ARRAY: {
            uint32_t i, left;
            blob_t *p = NULL;
            
            blob_foreach(blob, p, i, left) {
                jobj_t obj = __blob_jobj(p);
                if (obj) {
                    jobj_add(root, BLOB_T_OBJECT==type?blob_key(p):NULL, __blob_btoj(p, obj, level+1));
                } else {
                    __blob_btoj(p, root, level+1);
                }
            }
        }   break;
        case BLOB_T_STRING:
            jobj_add_string(root, name, blob_get_string(blob));
            break;
        case BLOB_T_BOOL:
            jobj_add_bool(root, name, blob_get_bool(blob));
            break;
        case BLOB_T_INT32:
            jobj_add_i32(root, name, blob_get_i32(blob));
            break;
        case BLOB_T_UINT32:
            jobj_add_u32(root, name, blob_get_u32(blob));
            break;
        case BLOB_T_FLOAT32:
            jobj_add_f32(root, name, blob_get_f32(blob));
            break;
        case BLOB_T_INT64:
            jobj_add_i64(root, name, blob_get_i64(blob));
            break;
        case BLOB_T_UINT64:
            jobj_add_u64(root, name, blob_get_u64(blob));
            break;
        case BLOB_T_FLOAT64:
            jobj_add_f64(root, name, blob_get_f64(blob));
            break;
        case BLOB_T_EMPTY:
            debug_error("no support empty");
            break;
        case BLOB_T_BINARY:
            jobj_add_string(root, name, blob_get_string(blob));
            break;
        default:
            debug_error("unknow blob type");
            break;
    }

    return root;
}

static inline jobj_t
blob_btoj(blob_t *blob)
{
    return __blob_btoj(blob, __blob_jobj(blob), 0);
}

/*
* blob to json
*/
static inline blob_t *
__blob_bobj(slice_t *slice, jobj_t obj)
{
    switch(jobj_type(obj)) {
        case jtype_array:   return blob_root_array(slice);
        case jtype_object:  return blob_root_object(slice);
        default: return NULL;
    }
}

static inline void
__blob_jtob(slice_t *slice, char *name, jobj_t obj, int level);

static inline void
____blob_jtob(slice_t *slice, char *name, jobj_t obj, int level)
{
    int type = jobj_type(obj);
    bool is_container = jtype_is_container(type);
    void *cookie = NULL;
    
    if (is_container) {
        cookie = __blob_nest_start(slice, jtype_array==type, name);
    }
    
    __blob_jtob(slice, name, obj, 1+level);
    
    if (is_container) {
        __blob_nest_end(slice, cookie);
    }
}

static inline void
__blob_jtob(slice_t *slice, char *name, jobj_t obj, int level)
{
    int type = jobj_type(obj);

    switch(type) {
        case jtype_array:{
            int i, count = jarray_length(obj);
            
            for (i=0; i<count; i++) {
                jobj_t sub = jarray_get(obj, i);
                if (NULL==sub) {
                    continue;
                }

                ____blob_jtob(slice, NULL, sub, level);
            }
            
        }   break;
        case jtype_object: {
            jobj_foreach(obj, k, sub) {
                ____blob_jtob(slice, k, sub, level);
            }
            
        }   break;
        case jtype_bool:
            blob_put_bool(slice, name, jobj_get_bool(obj));
            break;
        case jtype_int:
            blob_put_i32(slice, name, jobj_get_i32(obj));
            break;
        case jtype_double:
            blob_put_f64(slice, name, jobj_get_f64(obj));
            break;
        case jtype_string:
            blob_put_string(slice, name, jobj_get_string(obj));
            break;
        case jtype_null:
        default:
            break;
    }
}

/*
* json to blob
*/
static inline blob_t *
blob_jtob(slice_t *slice, jobj_t obj)
{
    if (NULL==__blob_bobj(slice, obj)) {
        return NULL;
    }
    
    __blob_jtob(slice, NULL, obj, 0);

    return blob_root(slice);
}

#endif

/******************************************************************************/
#endif /* __BLOB_H_f3a53c1649ab48a884ddae7dad5cdca4__ */
