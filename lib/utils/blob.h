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
    _(BLOB_T_INT64,     7, "int64"),    \
    _(BLOB_T_DOUBLE,    8, "double"),   \
    /* end */

#define BLOB_T_EMPTY    BLOB_T_EMPTY
#define BLOB_T_OBJECT   BLOB_T_OBJECT
#define BLOB_T_ARRAY    BLOB_T_ARRAY
#define BLOB_T_STRING   BLOB_T_STRING
#define BLOB_T_BINARY   BLOB_T_BINARY
#define BLOB_T_BOOL     BLOB_T_BOOL
#define BLOB_T_INT32    BLOB_T_INT32
#define BLOB_T_INT64    BLOB_T_INT64
#define BLOB_T_DOUBLE   BLOB_T_DOUBLE

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
    uint16_t    count;      /* if current node is object/array, the count is node's sub node count */
    uint32_t    vlen;       /* real value len, NOT include '\0' if value is string */
/*
    name/value align 4
    ----------------------
    |  name    | value   |
    ---------------------- 
*/
	char data[];            /* include name and value */
} blob_t;

static inline char *
blob_data(const blob_t *blob)
{
    return (char *)blob->data;
}

/*
* key + ['\0'] + [pad]
*/
static inline uint32_t
blob_ksize(const blob_t *blob)
{
    return blob->klen?BLOB_ALIGN(1 + blob->klen):0;
}

static inline uint32_t
blob_kpad_len(const blob_t *blob)
{
    return blob->klen?(blob_ksize(blob) - blob->klen - 1):0;
}

static inline char *
blob_key(const blob_t *blob)
{
    return blob->klen?blob_data(blob):NULL;
}

static inline void *
blob_kpad(const blob_t *blob)
{
    return blob->klen?(blob_key(blob) + blob->klen + 1):NULL;
}

/*
* value + ['\0'] + [pad]
*/
static inline uint32_t
blob_vsize(const blob_t *blob)
{
    if (blob->vlen) {
        if (BLOB_T_STRING==blob->type) {
            return BLOB_ALIGN(1 + blob->vlen);
        } else {
            return BLOB_ALIGN(blob->vlen);
        }
    } else {
        return 0;
    }
}

static inline char *
blob_value(const blob_t *blob)
{
    return blob->vlen?(blob_data(blob) + blob_ksize(blob)):NULL;
}

static inline uint32_t
blob_vpad_len(const blob_t *blob)
{
    if (blob->vlen) {
        if (BLOB_T_STRING==blob->type) {
            return blob_vsize(blob) - blob->vlen - 1;
        } else {
            return blob_vsize(blob) - blob->vlen;
        }
    } else {
        return 0;
    }
}

static inline void *
blob_vpad(const blob_t *blob)
{
    if (blob->vlen) {
        if (BLOB_T_STRING==blob->type) {
            return blob_value(blob) + blob->vlen + 1;
        } else {
            return blob_value(blob) + blob->vlen;
        }
    } else {
        return NULL;
    }
}

#if 0
static inline void
blob_sub_vlen(blob_t *blob, uint32_t len)
{
    if (len <= blob->vlen) {
        blob->vlen -= len;
    } else {
        trace_assert(0, "SUB: len(%d) < vlen(%d)", len, blob->vlen);
    }
}
#endif

static inline uint32_t
blob_dsize(const blob_t *blob)
{
    return blob_ksize(blob) + blob_vsize(blob);
}

static inline uint32_t
blob_dlen(const blob_t *blob)
{
    return blob_ksize(blob) + blob->vlen;
}

static inline uint32_t
blob_size(const blob_t *blob)
{
    return blob_dsize(blob) + sizeof(blob_t);
}

#define blob_vpointer(_type, _blob)     ((_type *)blob_value(_blob))

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

static inline double
blob_get_double(const blob_t *blob)
{
	return *blob_vpointer(double, blob);
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
    if (blob->klen) {
        os_memzero(blob_kpad(blob), blob_kpad_len(blob));
    }
    
    if (blob->vlen) {
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

#if 1
#define blob_foreach(_root, _blob, _i, _left) \
	for (_left = _root?blob_vsize(_root):0, _blob = blob_first(_root); \
	     _left > 0 && blob_size(_blob) <= _left && blob_size(_blob) >= sizeof(blob_t); \
	     _left -= blob_size(_blob), _blob = blob_next(_blob)) \
    /* end */
#else
#define blob_foreach(_root, _blob, _i, _left) \
	for (_i = 0, _left = _root?blob_vsize(_root):0, _blob = blob_first(_root); \
	     _i < (_root)->count && _blob && _left > 0 && blob_size(_blob) <= _left && blob_size(_blob) >= sizeof(blob_t); \
	     _i++, _left -= blob_size(_blob), _blob = blob_next(_blob))  \
    /* end */
#endif

#define BLOB_DUMP_ATOMIC    0

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
        blob->count,
        blob_size(blob),
        blob->klen,
        blob_ksize(blob),
        blob->vlen,
        blob_vsize(blob),
        blob_type_string(blob->type));
}

static inline void
__blob_dump(const blob_t *blob, int level)
{
    int i, left;
    blob_t *p;
    
    if (0==level) {
        __blob_dump_header(blob, "==DUMP BEGIN==");
        os_printf(__crlf);
    }
    
#if !BLOB_DUMP_ATOMIC
    if (is_blob_type_container(blob->type)) {
#endif
        __printab(level);
        __blob_dump_header(blob, NULL);
#if !BLOB_DUMP_ATOMIC
        os_printf(__crlf);
    }
#endif

    switch(blob->type) {
        case BLOB_T_OBJECT:
        case BLOB_T_ARRAY:
#if BLOB_DUMP_ATOMIC
            os_printf(__crlf);
#endif
            blob_foreach(blob, p, i, left) {
                __blob_dump(p, level+1);
            }
            
            break;
#if BLOB_DUMP_ATOMIC
        case BLOB_T_BOOL:
            os_println(":%d", *(int *)blob_value(blob));
            break;
        case BLOB_T_INT32:
            os_println(":%d", *(int32_t *)blob_value(blob));
            break;
        case BLOB_T_INT64:
            os_println(":%lld", *(int64_t *)blob_value(blob));
            break;
        case BLOB_T_DOUBLE:
            os_println(":%lf", *(double *)blob_value(blob));
            break;
        case BLOB_T_STRING:
            os_println(":%s", (char *)blob_value(blob));
            break;
        case BLOB_T_BINARY:
            os_println(":%p", blob_value(blob));
            break;
        case BLOB_T_EMPTY:
            break;
#endif
        default:
#if BLOB_DUMP_ATOMIC
            os_println(":%p", blob_value(blob));
#endif
            break;
    }

    if (0==level) {
        __blob_dump_header(blob, "==DUMP END  ==");
        os_printf(__crlf);
    }
}

static inline void
blob_dump(const blob_t *blob)
{
    __blob_dump(blob, 0);
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
        blob_type_string(blob_root(slice)->type),
        blob_root(slice)->vlen);
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
            .minsize= sizeof(uint32_t),
        },
        [BLOB_T_INT64] = {
            .flag   = BLOB_F_FIXED,
            .minsize= sizeof(uint64_t),
        },
        [BLOB_T_DOUBLE] = {
            .flag   = BLOB_F_FIXED,
            .minsize= sizeof(double),
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
    uint32_t left, type;
    int i, found = 0;

    blob_foreach(blob, p, i, left) {
        type = p->type;
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

        uint32_t vlen = p->vlen;
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
    blob->type  = type;
    blob->klen  = name?os_strlen(name):0;
    blob->vlen  = payload;
    blob->count = 0;
}

static inline void
__blob_save_name(blob_t *blob, const char *name)
{
    if (name) {
        os_strmcpy(blob_key(blob), name, blob->klen);
    }

    blob_zero_pad(blob);
}

static inline blob_t *
__blob_new(slice_t *slice, int type, const char *name, int payload)
{
	blob_t tmp;
    
    __blob_init(&tmp, type, name, payload);
    
    int size = blob_size(&tmp);

    if (__is_ak_debug_trace | __is_ak_debug_blob) {
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
        root->vlen += size;
        root->count++;
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

    if (__is_ak_debug_trace | __is_ak_debug_blob) {
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
	root->vlen += size;

    if (__is_ak_debug_trace | __is_ak_debug_blob) {
        os_printf("blob nest end" __crlf
            __tab "slice(size=%d, used=%d, remain=%d)" __crlf
            __tab "root(added=%d, vlen=%d)" __crlf, 
            slice_size(slice),
            slice_tail(slice) - slice_data(slice),
            slice_remain(slice),
            size,
            root->vlen);
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

#if 0 /* todo: debug it */
#define blob_sprintf(_slice, _id, _name, _fmt, _args...) ({ \
    __label__ error;                                \
    __label__ ok;                                   \
    int len, space, size;                           \
    int payload = 1 + os_strlen(_fmt);              \
    blob_t *blob = __blob_new(_slice, false, _id,   \
        BLOB_T_STRING, _name, payload);             \
                                                    \
    /* pub blob header/_name */                     \
    size = sizeof(*blob) + blob_ksize(blob);    \
    if (NULL==slice_put(_slice, size)) {           \
        blob = NULL; goto error;                    \
    }                                               \
                                                    \
    space = slice_remain(_slice);                   \
    debug_ok("blob_sprintf: remain %d", space);     \
                                                    \
    len = slice_sprintf(_slice,                     \
        SLICE_F_GROW, _fmt, ##_args);               \
    if (os_snprintf_is_full(space, len)) {          \
        debug_error("blob_sprintf: full");         \
        blob = NULL; goto error;                    \
    }                                               \
                                                    \
    /* pad blob value */                            \
    len++; /* '\0' */                               \
    len = BLOB_ALIGN(len);                          \
    if (os_snprintf_is_full(space, len)             \
        && slice_grow(_slice, len - space)) {       \
        blob = NULL; goto error;                    \
    }                                               \
    /* put blob value */                            \
    slice_put(_slice, len);                         \
    blob_add_vlen(blob, len);                       \
    blob_add_vlen(blob_root(_slice),blob_size(blob)); \
                                                    \
    goto ok;                                        \
error:                                              \
    slice_trim(_slice, size);                       \
ok:                                                 \
    blob;                                           \
})  /* end */
#else
#define blob_sprintf(_slice, _id, _name, _fmt, _args...)    NULL
#endif

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
        char *bv = blob_value(blob);
        
	    os_memcpy(bv, value, len);
	    if (BLOB_T_STRING==type) {
            bv[len] = 0;
	    }
	}
	
    if (__is_ak_debug_trace | __is_ak_debug_blob) {
        __blob_dump_slice(slice, "blob_put");
	}

	return blob;
}

static inline blob_t *
blob_put_string(slice_t *slice, const char *name, const char *str)
{
	return blob_put(slice, BLOB_T_STRING, name, str, os_strlen(str));
}

static inline blob_t *
blob_put_binary(slice_t *slice, const char *name, const void *binary, uint32_t len)
{
	return blob_put(slice, BLOB_T_BINARY, name, binary, len);
}

static inline blob_t *
blob_put_bool(slice_t *slice, const char *name, bool val)
{
    val = !!val;
    
	return blob_put(slice, BLOB_T_BOOL, name, &val, sizeof(val));
}

static inline blob_t *
blob_put_u32(slice_t *slice, const char *name, uint32_t val)
{
	return blob_put(slice, BLOB_T_INT32, name, &val, sizeof(val));
}

static inline blob_t *
blob_put_u64(slice_t *slice, const char *name, uint64_t val)
{
	return blob_put(slice, BLOB_T_INT64, name, &val, sizeof(val));
}

static inline blob_t *
blob_put_i32(slice_t *slice, const char *name, int32_t val)
{
	return blob_put(slice, BLOB_T_INT32, name, &val, sizeof(val));
}

static inline blob_t *
blob_put_i64(slice_t *slice, const char *name, int64_t val)
{
	return blob_put(slice, BLOB_T_INT64, name, &val, sizeof(val));
}

static inline blob_t *
blob_put_double(slice_t *slice, const char *name, double val)
{
	return blob_put(slice, BLOB_T_DOUBLE, name, &val, sizeof(val));
}

static inline blob_t *
blob_put_blob(slice_t *slice, blob_t *blob)
{
	return blob_put(slice, blob->type, blob_key(blob), blob_value(blob), blob->vlen);
}

#ifdef __APP__
static inline void
__blob_byteorder(blob_t *blob, bool ntoh)
{
    uint32_t left;
    uint32_t type = blob->type;
    int i;
    
    if (ntoh) {
        blob->count= bswap_16(blob->count);
        blob->vlen = bswap_32(blob->vlen);
    }
    
    switch(type) {
        case BLOB_T_OBJECT:
        case BLOB_T_ARRAY: {
            blob_t *p;
            
            blob_foreach(blob, p, i, left) {
                __blob_byteorder(p, ntoh);
            }
        }   break;
        case BLOB_T_INT64:
        case BLOB_T_DOUBLE:
            *blob_vpointer(int64_t, blob) = bswap_64(blob_get_u64(blob));
            break;
        case BLOB_T_BOOL:
        case BLOB_T_INT32:
            *blob_vpointer(int32_t, blob) = bswap_32(blob_get_u32(blob));
            break;
        case BLOB_T_STRING:
        case BLOB_T_BINARY:
        case BLOB_T_EMPTY:
        default:
            /* do nothinig */
            break;
    }
    
    if (false==ntoh) {
        blob->count= bswap_16(blob->count);
        blob->vlen = bswap_32(blob->vlen);
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
    switch(blob->type) {
        case BLOB_T_OBJECT: return jobj_new_object();
        case BLOB_T_ARRAY:  return jobj_new_array();
        default: return NULL;
    }
}

static inline jobj_t
__blob_btoj(blob_t *blob, jobj_t root, int level)
{
    char *name  = blob_key(blob);
    int err = 0;

    if (NULL==root) {
        return NULL;
    }
    
    switch(blob->type) {
        case BLOB_T_OBJECT:
        case BLOB_T_ARRAY: {
            blob_t *p   = NULL;
            uint32_t left;
            int i = 0;
            
            blob_foreach(blob, p, i, left) {
                jobj_t obj = __blob_jobj(p);
                if (obj) {
                    jobj_add(root, blob->type==BLOB_T_OBJECT?blob_key(p):NULL, __blob_btoj(p, obj, level+1));
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
            jobj_add_int(root, name, blob_get_i32(blob));
            break;
        case BLOB_T_INT64:
            jobj_add_int64(root, name, blob_get_i64(blob));
            break;
        case BLOB_T_DOUBLE:
            jobj_add_double(root, name, blob_get_double(blob));
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
            blob_put_i32(slice, name, jobj_get_int(obj));
            break;
        case jtype_double:
            blob_put_double(slice, name, jobj_get_double(obj));
            break;
        case jtype_string:
            blob_put_string(slice, name, jobj_get_string(obj));
            break;
        case jtype_null:
        default:
            break;
    }
}

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
