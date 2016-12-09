/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
DECLARE uint32
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

DECLARE uint32
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

DECLARE void *
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

DECLARE void
blob_zero_pad(blob_t *blob)
{
    if (blob_klen(blob)) {
        os_memzero(blob_kpad(blob), blob_kpad_len(blob));
    }
    
    if (blob_vlen(blob)) {
        os_memzero(blob_vpad(blob), blob_vpad_len(blob));
    }
}

DECLARE bool
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

DECLARE blob_t *
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

DECLARE blob_t *
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

DECLARE blob_t *
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

DECLARE blob_t *
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

DECLARE blob_t *
blob_sgetby(blob_t *root, const char *fmt, ...)
{
    va_list args;
    
    va_start(args, fmt);
    blob_t *blob = blob_vgetby(root, fmt, args);
    va_end(args);
    
    return blob;
}

DECLARE void
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
        blob_type_getnamebyid(blob_type(blob)));
}

DECLARE void
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

DECLARE void
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
        blob_type_getnamebyid(blob_type(blob_root(slice))),
        blob_vlen(blob_root(slice)));
}

DECLARE int
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

DECLARE int
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

DECLARE bool
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
    
    if (false==is_good_blob_type(type)) {
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

DECLARE int
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
        if (false==is_good_blob_type(type)) {
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

DECLARE void
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

DECLARE void
__blob_save_name(blob_t *blob, const char *name)
{
    if (name) {
        os_strmcpy(blob_key(blob), name, blob_klen(blob));
    }

    blob_zero_pad(blob);
}

DECLARE blob_t *
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
            blob_type_getnamebyid(type), 
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

DECLARE void *
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

DECLARE void
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

DECLARE blob_t *
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

DECLARE blob_t *
blob_put_vsprintf(slice_t *slice, char *name, const char *fmt, va_list args)
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

DECLARE blob_t *
blob_put_sprintf(slice_t *slice, char *name, const char *fmt, ...)
{
    va_list args;
    
    va_start(args, fmt);
    blob_t *blob = blob_put_vsprintf(slice, name, fmt, args);
    va_end(args);
    
    return blob;
}

#ifdef __APP__

DECLARE void
__blob_byteorder(blob_t *blob, bool ntoh)
{
    
    if (ntoh) {
        blob_klen(blob) = bswap_16(blob_klen(blob));
        blob_vlen(blob) = bswap_32(blob_vlen(blob));
#if USE_BLOB_COUNT
        blob_count(blob)= bswap_32(blob_count(blob));
#endif
    }
    
    switch(blob_type(blob)) {
        case BLOB_T_OBJECT:
        case BLOB_T_ARRAY: {
            uint32 i, left;
            blob_t *p;
            
            blob_foreach(blob, p, i, left) {
                __blob_byteorder(p, ntoh);
            }
        }   break;
        case BLOB_T_INT64:
        case BLOB_T_UINT64:
        case BLOB_T_FLOAT64:
            *blob_vpointer(uint64, blob) = bswap_64(blob_get_u64(blob));
            break;
        case BLOB_T_BOOL:
        case BLOB_T_INT32:
        case BLOB_T_UINT32:
        case BLOB_T_FLOAT32:
            *blob_vpointer(uint32, blob) = bswap_32(blob_get_u32(blob));
            break;
        case BLOB_T_STRING:
        case BLOB_T_BINARY:
        default:
            /* do nothinig */
            break;
    }
    
    if (false==ntoh) {
        blob_klen(blob) = bswap_16(blob_klen(blob));
        blob_vlen(blob) = bswap_32(blob_vlen(blob));
#if USE_BLOB_COUNT
        blob_count(blob)= bswap_32(blob_count(blob));
#endif
    }
}

DECLARE void
blob_ntoh(blob_t *blob)
{
    __blob_byteorder(blob, true);
}

DECLARE void
blob_hton(blob_t *blob)
{
    __blob_byteorder(blob, false);
}

DECLARE jobj_t
__blob_jobj(blob_t *blob)
{
    switch(blob_type(blob)) {
        case BLOB_T_OBJECT: return jobj_new_object();
        case BLOB_T_ARRAY:  return jobj_new_array();
        default: return NULL;
    }
}

DECLARE jobj_t
__blob_btoj(blob_t *blob, jobj_t root, int level)
{
    char *name      = blob_key(blob);
    uint32 type   = blob_type(blob);
    int err = 0;

    if (NULL==root) {
        return NULL;
    }
    
    switch(type) {
        case BLOB_T_OBJECT:
        case BLOB_T_ARRAY: {
            uint32 i, left;
            blob_t *p = NULL;
            
            blob_foreach(blob, p, i, left) {
                jobj_t obj = __blob_jobj(p);
                if (obj) {
                    char  *k = BLOB_T_OBJECT==type?blob_key(p):NULL;
                    jobj_t v = __blob_btoj(p, obj, level+1);
                    
                    jobj_add(root, k, v);
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
        case BLOB_T_BINARY:
            jobj_add_binary(root, name, blob_get_binary(blob));
            break;
        default:
            debug_error("unknow blob type");
            break;
    }

    return root;
}

DECLARE jobj_t
blob_btoj(blob_t *blob)
{
    return __blob_btoj(blob, __blob_jobj(blob), 0);
}

/*
* blob to json
*/
DECLARE blob_t *
__blob_bobj(slice_t *slice, jobj_t obj)
{
    switch(jobj_type(obj)) {
        case jtype_array:   return blob_root_array(slice);
        case jtype_object:  return blob_root_object(slice);
        default:            return NULL;
    }
}

EXTERN void
__blob_jtob(slice_t *slice, char *name, jobj_t obj, int level);

DECLARE void
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

DECLARE void
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
DECLARE blob_t *
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
