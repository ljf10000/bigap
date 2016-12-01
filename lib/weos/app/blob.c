/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifdef __APP__

STATIC void
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

void
blob_ntoh(blob_t *blob)
{
    __blob_byteorder(blob, true);
}

void
blob_hton(blob_t *blob)
{
    __blob_byteorder(blob, false);
}

STATIC jobj_t
__blob_jobj(blob_t *blob)
{
    switch(blob_type(blob)) {
        case BLOB_T_OBJECT: return jobj_new_object();
        case BLOB_T_ARRAY:  return jobj_new_array();
        default: return NULL;
    }
}

STATIC jobj_t
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

jobj_t
blob_btoj(blob_t *blob)
{
    return __blob_btoj(blob, __blob_jobj(blob), 0);
}

/*
* blob to json
*/
STATIC blob_t *
__blob_bobj(slice_t *slice, jobj_t obj)
{
    switch(jobj_type(obj)) {
        case jtype_array:   return blob_root_array(slice);
        case jtype_object:  return blob_root_object(slice);
        default:            return NULL;
    }
}

static void
__blob_jtob(slice_t *slice, char *name, jobj_t obj, int level);

STATIC void
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

STATIC void
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
blob_t *
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
