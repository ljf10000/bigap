/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      libjs
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     js
#endif

#define __RUNAS_UNKNOW__

#include "utils.h"
#include "js.h"
/******************************************************************************/

int
duk_require_buffer_or_lstring(duk_context *ctx, duk_idx_t idx, duk_buffer_t *pbuf, duk_size_t *psize)
{
    duk_buffer_t buf = NULL;
    
    int type = duk_get_type(ctx, idx);
    
    if (DUK_TYPE_BUFFER==type) {
        buf = duk_get_buffer_data(ctx, idx, psize);
    }
    else if (DUK_TYPE_STRING==type) {
        buf = (duk_buffer_t)duk_get_lstring(ctx, idx, psize);
    }
    else {
        return -EFORMAT;
    }

    *pbuf = buf;
    
    return 0;
}

void
js_put_functions(duk_context *ctx, duk_idx_t obj_index, const dukc_func_entry_t *funcs)
{
	const dukc_func_entry_t *ent;

	obj_index = duk_require_normalize_index(ctx, obj_index);
	for (ent = funcs; ent && ent->key; ent++) {
#if js_LIGHT_FUNC
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

int 
js_push_error(duk_context *ctx, int err)
{
    if (err<0) {
        js_seterrno(ctx);
    }

    duk_push_int(ctx, err);
    
    return err;
}

duk_buffer_t
js_push_pointer(duk_context *ctx, duk_pointer_t pointer)
{
    if (pointer) {
        duk_push_pointer(ctx, pointer);
    } else {
        duk_push_null(ctx);
    }

    return pointer;
}

duk_string_t
js_push_lstring(duk_context *ctx, duk_string_t s, duk_size_t len)
{
    if (is_good_str(s) && len>0) {
        return duk_push_lstring(ctx, s, len);
    } else {
        return duk_push_null(ctx), NULL;
    }
}

duk_string_t
js_push_string(duk_context *ctx, duk_string_t s)
{
    if (s) {
        return duk_push_string(ctx, s);
    } else {
        return duk_push_null(ctx), NULL;
    }
}

duk_buffer_t
js_push_buffer(duk_context *ctx, duk_size_t size, bool dynamic)
{
    if (size>0) {
        return duk_push_buffer(ctx, size, dynamic);
    } else {
        return duk_push_null(ctx), NULL;
    }
}

duk_buffer_t
js_push_fixed_buffer(duk_context *ctx, duk_size_t size)
{
    if (size>0) {
        return duk_push_fixed_buffer(ctx, size);
    } else {
        return duk_push_null(ctx), NULL;
    }
}

duk_buffer_t
js_push_dynamic_buffer(duk_context *ctx, duk_size_t size)
{
    if (size>0) {
        return duk_push_dynamic_buffer(ctx, size);
    } else {
        return duk_push_null(ctx), NULL;
    }
}

void
js_pcall(duk_context *ctx, duk_idx_t idx, int (*push)(void))
{
    idx = duk_normalize_index(ctx, idx);
    
    duk_dup(ctx, idx);
    duk_pcall(ctx, (*push)());
    duk_pop(ctx);
}

int
js_get_a_prop_string(
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

int
js_get_v_prop_string(
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

int
js_get_prop_string(duk_context *ctx, bool auto_create, duk_idx_t idx, int *level, ...)
{
    va_list args;
    
    va_start(args, level);
    int err = js_get_v_prop_string(ctx, auto_create, idx, level, args);
    va_end(args);

    return err;
}


char *
js_get_obj_string(duk_context *ctx, duk_idx_t idx, duk_string_t k, duk_size_t *plen)
{
    char *v = NULL;
    
    duk_get_prop_string(ctx, idx, k);
    if (duk_is_string(ctx, -1)) {
        v = (char *)duk_get_lstring(ctx, -1, plen);
    }
    duk_pop(ctx);

    return v;
}

char *
js_copy_obj_string(duk_context *ctx, duk_idx_t idx, duk_string_t k, char *buffer, int size)
{
    duk_size_t len = 0;
    char *v = js_get_obj_string(ctx, idx, k, &len);
    
    if (v && (size-1) >= len ) {
        v = os_strmcpy(buffer, v, len);
    } else {
        v = NULL;
    }

    return v;
}

void
js_set_obj_string(duk_context *ctx, duk_idx_t idx, duk_string_t k, char * v)
{
    idx = duk_normalize_index(ctx, idx);
    
    js_push_string(ctx, v);
    duk_put_prop_string(ctx, idx, k);
}

duk_buffer_t
js_get_obj_buffer(duk_context *ctx, duk_idx_t idx, duk_string_t k, duk_size_t *plen)
{
    duk_buffer_t v = NULL;
    
    duk_get_prop_string(ctx, idx, k);
    if (duk_is_buffer(ctx, -1)) {
        v = duk_get_buffer_data(ctx, -1, plen);
    }
    duk_pop(ctx);

    return v;
}

char *
js_copy_obj_buffer(duk_context *ctx, duk_idx_t idx, duk_string_t k, duk_buffer_t buffer, duk_size_t size)
{
    duk_size_t len = 0;
    duk_buffer_t v = js_get_obj_buffer(ctx, idx, k, &len);
    if (v && size>=len) {
        os_memcpy(buffer, v, len);
        v = buffer;
    } else {
        v = NULL;
    }

    return v;
}

void
js_set_obj_buffer(duk_context *ctx, duk_idx_t idx, duk_string_t k, duk_buffer_t v, duk_size_t size)
{
    idx = duk_normalize_index(ctx, idx);
    
    duk_buffer_t buf = js_push_dynamic_buffer(ctx, size);
    os_memcpy(buf, v, size);
    duk_put_prop_string(ctx, idx, k);
}

char *
js_get_array_string(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_size_t *plen)
{
    char *v = NULL;
    
    duk_get_prop_index(ctx, idx, aidx);
    if (duk_is_string(ctx, -1)) {
        v = (char *)duk_get_lstring(ctx, -1, plen);
    }
    duk_pop(ctx);

    return v;
}

char *
js_copy_array_string(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, char *buffer, int size)
{
    duk_size_t len = 0;
    char *v = js_get_array_string(ctx, idx, aidx, &len);
    if (v && (size-1) >= len) {
        os_strmcpy(buffer, v, len);
        v = buffer;
    } else {
        v = NULL;
    }

    return v;
}

void
js_set_array_string(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, char *v)
{
    idx = duk_normalize_index(ctx, idx);
    
    js_push_string(ctx, (char *)v);
    duk_put_prop_index(ctx, idx, aidx);
}

duk_buffer_t
js_get_array_buffer(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_size_t *plen)
{
    duk_buffer_t v = NULL;
    
    duk_get_prop_index(ctx, idx, aidx);
    if (duk_is_buffer(ctx, -1)) {
        v = duk_get_buffer_data(ctx, -1, plen);
    }
    duk_pop(ctx);

    return v;
}

char *
js_copy_array_buffer(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_buffer_t buffer, int size)
{
    duk_size_t len = 0;
    duk_buffer_t v = js_get_array_buffer(ctx, idx, aidx, &len);
    if (v && size >= len) {
        os_memcpy(buffer, v, len);
        v = buffer;
    } else {
        v = NULL;
    }

    return v;
}

void
js_set_array_buffer(duk_context *ctx, duk_idx_t idx, duk_idx_t aidx, duk_buffer_t v, int size)
{
    idx = duk_normalize_index(ctx, idx);
    
    duk_buffer_t buf = js_push_dynamic_buffer(ctx, size);
    os_memcpy(buf, v, size);
    duk_put_prop_index(ctx, idx, aidx);
}

int
__js_seterrno(duk_context *ctx, int err)
{
    duk_push_global_object(ctx);
        if (duk_get_prop_string(ctx, -1, js_MOD_LIBC)) {
            js_set_obj_field(ctx, -1, int, "errno", err);
        }
    	duk_pop(ctx);
    duk_pop(ctx);

    return err;
}

int
js_obj_push(duk_context *ctx, dukc_obj_op_f *set, duk_object_t obj)
{
    if (obj) {
        duk_push_object(ctx);

        return (*set)(ctx, -1, obj);
    } else {
        duk_push_null(ctx);

        return 0;
    }
}

int
js_obj_op(duk_context *ctx, bool auto_create, duk_idx_t idx, dukc_obj_op_f *op, duk_object_t obj, duk_string_t k)
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

int
js_obj_obj_op(duk_context *ctx, bool auto_create, duk_idx_t idx, dukc_obj_op_f *op, duk_object_t obj, ...)
{
    int err = 0, level = 0;
    va_list args;

    va_start(args, obj);
    err = js_get_v_prop_string(ctx, auto_create, idx, &level, args);
    va_end(args);

    if (0==err) {
        err = (*op)(ctx, -1, obj);
    }
    duk_pop_n(ctx, level);
    
    return err;
}

int
js_load_file(duk_context *ctx, char *filename)
{
    int err = -ENOEXIST;
    
    debug_js("load %s ...", filename);
    if (os_file_exist(filename)) {
        err = duk_peval_file_noresult(ctx, filename);
    }
    debug_js("load %s %s.", filename, __ok_string(0==err));

    return err;
}

int
js_load_code(duk_context *ctx, char *tag, char *code)
{
    int err = 0;
    
    debug_js("load %s ...", tag);
    err = duk_peval_string_noresult(ctx, code);
    debug_js("load %s %s.", tag, __ok_string(0==err));

    return err;
}

int
js_register(duk_context *ctx, js_register_f *cb)
{
    static js_register_f *registers[] = {
        js_global_register,
        js_my_register,
        js_libc_register,
#if js_LIBZ
        js_libz_register,
#endif
#if js_LIBBZ
        js_libbz_register,
#endif
#if js_LIBLZ
        js_liblz_register,
#endif
#if js_LIBCURL
        js_libcurl_register,
#endif
        /*keep below last*/
        js_buildin_register,
        js_auto_register,
    };
    int i, err;

    if (NULL==cb) {
        for (i=0; i<os_count_of(registers); i++) {
            err = (*registers[i])(ctx);
            if (err<0) {
                return err;
            }
        }

        return 0;
    } else {
        return (*cb)(ctx);
    }
}

void
__js_priv_fini(js_priv_t *priv)
{
    int i;

    if (priv) {
        os_free(priv->cache);
        os_free(priv->name);
        os_free(priv->atexit_name);
        
        for (i=0; i<NSIG; i++) {
            if (priv->sig[i].is_func) {
                os_free(priv->sig[i].name);
            }
        }
    }
}

js_priv_t *
js_priv_init(char *name, char *cache, int argc, char **argv)
{
    js_priv_t *priv = (js_priv_t *)os_zalloc(sizeof(*priv));
    if (NULL==priv) {
        return NULL;
    }

    priv->argc = argc;
    priv->argv = argv;
    priv->name = os_strdup(name);
    priv->cache= os_strdup(cache);
    
    if (1==argc) {
        priv->mode = JS_EVAL_STREAM;
    } else if (argc > 1) {
        priv->mode = JS_EVAL_SHABANG;
    } else {
        priv->mode = JS_EVAL_BUILDIN;
    }
    
    return priv;
}

void
__js_fini(duk_context *ctx)
{
    if (ctx) {
        js_priv_t *priv = js_priv(ctx);
        
        debug_js("before destroy duk heap(%s)", priv->name);
        duk_destroy_heap(ctx);
        debug_js("after  destroy duk heap(%s)", priv->name);
        
        js_priv_fini(priv);
    }
}

duk_context *
js_init(char *name, char *cache, int argc, char **argv)
{
    duk_context *ctx = NULL;
    int err = 0;

    ctx = duk_create_heap_default();
    if (NULL==ctx) {
        goto error;
    }
    js_ctx_save(ctx);
    
    js_priv_t *priv = js_priv_init(name, cache, argc, argv);
    if (NULL==priv) {
        goto error;
    }
    duk_set_priv(ctx, priv);
    
    err = js_register(ctx, NULL);
    if (err<0) {
        goto error;
    }

    return ctx;
error:
    js_fini(ctx);
    
    return NULL;
}

int
js_eval(duk_context *ctx, char *jsfile)
{
    js_priv_t *priv = js_priv(ctx);
    char *script = NULL;
    
    switch(priv->mode) {
        case JS_EVAL_SHABANG:
            script = priv->argv[1];
            /*
            * argv[1] is script name
            */
            duk_peval_file(ctx, script);
            
            break;
        case JS_EVAL_STREAM:
            script = os_readfd(0, 4096);

            /*
            * cat SCRIPT  | js
            * echo SCRIPT | js
            */
            duk_peval_string(ctx, script); free(script);
            
            break;
        case JS_EVAL_BUILDIN:
            script = jsfile;
            /*
            * jsfile is script name
            */
            duk_peval_file(ctx, script);
            
            break;
        default:
            return -ENOSUPPORT;
    }
    
    duk_pop(ctx);

    return 0;
}

int
libc_get_sockaddr(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    os_sockaddr_t *p = (os_sockaddr_t *)obj; os_objzero(p);
    
    p->c.sa_family = js_get_obj_int(ctx, idx, "family");
    
    switch(p->c.sa_family) {
        case AF_INET:
            p->in.sin_addr.s_addr = js_get_obj_uint(ctx, idx, "ip");
            p->in.sin_port = js_get_obj_int(ctx, idx, "port");

            break;
        case AF_UNIX: {
            bool abstract = js_get_obj_bool(ctx, idx, "abstract");
            
            js_copy_obj_string(ctx, idx, "path",
                p->un.sun_path + !!abstract,
                sizeof(p->un.sun_path) - !!abstract);
        }   break;
        case AF_PACKET:
            p->ll.sll_protocol  = js_get_obj_int(ctx, idx, "protocol");
            p->ll.sll_ifindex   = js_get_obj_int(ctx, idx, "ifindex");
            p->ll.sll_hatype    = js_get_obj_int(ctx, idx, "hatype");
            p->ll.sll_pkttype   = js_get_obj_int(ctx, idx, "pkttype");
            p->ll.sll_halen     = js_get_obj_int(ctx, idx, "halen");

            js_copy_obj_buffer(ctx, idx, "addr", (char *)p->ll.sll_addr, 8);

            break;
        case AF_NETLINK:
            p->nl.nl_pid = js_get_obj_int(ctx, idx, "pid");
            p->nl.nl_groups = js_get_obj_int(ctx, idx, "groups");

            break;
        default:
            return -ENOSUPPORT;
    }

    return 0;
}

int
libc_set_sockaddr(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    os_sockaddr_t *p = (os_sockaddr_t *)obj;
    
    js_set_obj_int(ctx, idx, "family", p->c.sa_family);
    
    switch(p->c.sa_family) {
        case AF_INET:
            js_set_obj_uint(ctx, idx, "ip", p->in.sin_addr.s_addr);
            js_set_obj_uint(ctx, idx, "port", p->in.sin_port);
            
            break;
        case AF_UNIX: {
            bool abstract = is_abstract_sockaddr(&p->un);

            js_set_obj_bool(ctx, idx, "abstract", abstract);
            js_set_obj_string(ctx, idx, "path", p->un.sun_path + !!abstract);
            
        }   break;
        case AF_PACKET:
            js_set_obj_int(ctx, idx, "protocol", p->ll.sll_protocol);
            js_set_obj_uint(ctx, idx, "ifindex", p->ll.sll_ifindex);
            js_set_obj_int(ctx, idx, "hatype", p->ll.sll_hatype);
            js_set_obj_int(ctx, idx, "pkttype", p->ll.sll_pkttype);
            js_set_obj_int(ctx, idx, "halen", p->ll.sll_halen);
            js_set_obj_buffer(ctx, idx, "addr", p->ll.sll_addr, 8);
            
            break;
        case AF_NETLINK:
            js_set_obj_int(ctx, idx, "pid", p->nl.nl_pid);
            js_set_obj_uint(ctx, idx, "groups", p->nl.nl_groups);
            
            break;
        default:
            return -ENOSUPPORT;
    }

    return 0;
}

int
libc_get_linger(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct linger *p = (struct linger *)obj; os_objzero(p);
    
    p->l_onoff   = js_get_obj_bool(ctx, idx, "onoff");
    p->l_linger  = js_get_obj_uint(ctx, idx, "linger");

    return 0;
}

int
libc_set_linger(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct linger *p = (struct linger *)obj;
    
    js_set_obj_bool(ctx, idx, "onoff", p->l_onoff);
    js_set_obj_uint(ctx, idx, "linger", p->l_linger);

    return 0;
}

int
libc_get_dirent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct dirent *p = (struct dirent *)obj; os_objzero(p);
    
    p->d_type   = js_get_obj_int(ctx, idx, "type");
    p->d_ino    = js_get_obj_int(ctx, idx, "ino");
    p->d_off    = js_get_obj_uint(ctx, idx, "off");
    js_copy_obj_string(ctx, idx, "name", p->d_name, sizeof(p->d_name));

    return 0;
}

int
libc_set_dirent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct dirent *p = (struct dirent *)obj;
    
    js_set_obj_int(ctx, idx, "type", p->d_type);
    js_set_obj_int(ctx, idx, "ino", p->d_ino);
    js_set_obj_uint(ctx, idx, "off", p->d_off);
    js_set_obj_string(ctx, idx, "name", p->d_name);

    return 0;
}

int
libc_get_stat(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct stat *p = (struct stat *)obj; os_objzero(p);
    
    p->st_dev   = js_get_obj_uint(ctx, idx, "dev");
    p->st_rdev  = js_get_obj_uint(ctx, idx, "rdev");
    p->st_ino   = js_get_obj_uint(ctx, idx, "ino");
    p->st_mode  = js_get_obj_uint(ctx, idx, "mode");
    p->st_nlink = js_get_obj_uint(ctx, idx, "nlink");
    p->st_uid   = js_get_obj_uint(ctx, idx, "uid");
    p->st_gid   = js_get_obj_uint(ctx, idx, "gid");
    p->st_atime = js_get_obj_uint(ctx, idx, "atime");
    p->st_mtime = js_get_obj_uint(ctx, idx, "mtime");
    p->st_ctime = js_get_obj_uint(ctx, idx, "ctime");
    p->st_size  = js_get_obj_int(ctx, idx, "size");
    p->st_blksize = js_get_obj_int(ctx, idx, "blksize");
    p->st_blocks= js_get_obj_int(ctx, idx, "blocks");

    return 0;
}

int
libc_set_stat(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct stat *p = (struct stat *)obj;
    
    js_set_obj_uint(ctx, idx, "dev", p->st_dev);
    js_set_obj_uint(ctx, idx, "rdev", p->st_rdev);
    js_set_obj_uint(ctx, idx, "ino", p->st_ino);
    js_set_obj_uint(ctx, idx, "mode", p->st_mode);
    js_set_obj_uint(ctx, idx, "nlink", p->st_nlink);
    js_set_obj_uint(ctx, idx, "uid", p->st_uid);
    js_set_obj_uint(ctx, idx, "gid", p->st_gid);
    js_set_obj_uint(ctx, idx, "atime", p->st_atime);
    js_set_obj_uint(ctx, idx, "mtime", p->st_mtime);
    js_set_obj_uint(ctx, idx, "ctime", p->st_ctime);
    js_set_obj_int(ctx, idx, "size", p->st_size);
    js_set_obj_int(ctx, idx, "blksize", p->st_blksize);
    js_set_obj_int(ctx, idx, "blocks", p->st_blocks);

    return 0;
}

int
libc_get_if_nameindex(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct if_nameindex *p = (struct if_nameindex *)obj; os_objzero(p);
    
    p->if_index = js_get_obj_uint(ctx, idx, "index");
    p->if_name  = js_get_obj_string(ctx, idx, "name", NULL);

    return 0;
}

int
libc_set_if_nameindex(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct if_nameindex *p = (struct if_nameindex *)obj;
    
    js_set_obj_uint(ctx, idx, "index", p->if_index);
    js_set_obj_string(ctx, idx, "name", p->if_name);

    return 0;
}

int
libc_set_hostent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct hostent *p = (struct hostent *)obj;
    int i;
    
    js_set_obj_string(ctx, idx, "name", p->h_name);
    js_set_obj_int(ctx, idx, "addrtype", p->h_addrtype);
    js_set_obj_int(ctx, idx, "length", p->h_length);
    js_set_obj_uint(ctx, idx, "addr", *(uint32 *)(p->h_addr));
    
    duk_push_object(ctx);
    for (i=0; p->h_aliases[i]; i++) {
        js_set_array_string(ctx, -1, i, p->h_aliases[i]);
    }
    duk_put_prop_string(ctx, idx, "aliases");
    
    duk_push_object(ctx);
    for (i=0; p->h_addr_list[0]; i++) {
        js_set_array_uint(ctx, -1, i, *(uint32 *)p->h_addr_list[i]);
    }
    duk_put_prop_string(ctx, idx, "addr_list");

    return 0;
}

int
libc_set_servent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct servent *p = (struct servent *)obj;
    char *a;
    int i;
    
    js_set_obj_string(ctx, idx, "name", p->s_name);
    js_set_obj_int(ctx, idx, "port", p->s_port);
    js_set_obj_string(ctx, idx, "proto", p->s_proto);
    
    duk_push_object(ctx);
    for (i=0, a=p->s_aliases[0]; a; i++, a++) {
        js_set_array_string(ctx, -1, i, a);
    }
    duk_put_prop_string(ctx, idx, "aliases");

    return 0;
}

int
libc_set_protoent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct protoent *p = (struct protoent *)obj;
    char *a;
    int i;
    
    js_set_obj_string(ctx, idx, "name", p->p_name);
    js_set_obj_int(ctx, idx, "proto", p->p_proto);
    
    duk_push_object(ctx);
    for (i=0, a=p->p_aliases[0];a; i++, a++) {
        js_set_array_string(ctx, -1, i, a);
    }
    duk_put_prop_string(ctx, idx, "aliases");

    return 0;
}

int
libc_set_netent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct netent *p = (struct netent *)obj;
    int i;
        
    js_set_obj_string(ctx, idx, "n_name", p->n_name);
    js_set_obj_int(ctx, idx, "n_addrtype", p->n_addrtype);
    js_set_obj_int(ctx, idx, "n_net", p->n_net);

    duk_push_object(ctx);
    for (i=0; p->n_aliases[i]; i++) {
        js_set_array_string(ctx, -1, i, p->n_aliases[i]);
    }
    duk_put_prop_string(ctx, idx, "n_aliases");

    return 0;
}

int
libc_get_utimbuf(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct utimbuf *p = (struct utimbuf *)obj; os_objzero(p);
    
    p->actime   = js_get_obj_uint(ctx, idx, "actime");
    p->modtime  = js_get_obj_uint(ctx, idx, "modtime");

    return 0;
}

int
libc_set_utimbuf(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct utimbuf *p = (struct utimbuf *)obj;
    
    js_set_obj_uint(ctx, idx, "actime", p->actime);
    js_set_obj_uint(ctx, idx, "modtime", p->modtime);

    return 0;
}

int
libc_get_timeval(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timeval *p = (struct timeval *)obj; os_objzero(p);
    
    p->tv_sec   = js_get_obj_uint(ctx, idx, "sec");
    p->tv_usec  = js_get_obj_uint(ctx, idx, "usec");

    return 0;
}

int
libc_set_timeval(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timeval *p = (struct timeval *)obj;
    
    js_set_obj_uint(ctx, idx, "sec", p->tv_sec);
    js_set_obj_uint(ctx, idx, "usec", p->tv_usec);

    return 0;
}

int
libc_get_itimerval(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct itimerval *p = (struct itimerval *)obj; os_objzero(p);

    js_obj_get(ctx, idx, libc_get_timeval, &p->it_interval, "interval");
    js_obj_get(ctx, idx, libc_get_timeval, &p->it_value, "value");

    return 0;
}

int
libc_set_itimerval(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct itimerval *p = (struct itimerval *)obj;
    
    js_obj_set(ctx, idx, libc_set_timeval, &p->it_interval, "interval");
    js_obj_set(ctx, idx, libc_set_timeval, &p->it_value, "value");

    return 0;
}

int
libc_get_timespec(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timespec *p = (struct timespec *)obj; os_objzero(p);
    
    p->tv_sec   = js_get_obj_uint(ctx, idx, "sec");
    p->tv_nsec  = js_get_obj_uint(ctx, idx, "nsec");

    return 0;
}

int
libc_set_timespec(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timespec *p = (struct timespec *)obj;
    
    js_set_obj_uint(ctx, idx, "sec", p->tv_sec);
    js_set_obj_uint(ctx, idx, "nsec", p->tv_nsec);

    return 0;
}

int
libc_get_itimerspec(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct itimerspec *p = (struct itimerspec *)obj; os_objzero(p);

    js_obj_get(ctx, idx, libc_get_timespec, &p->it_interval, "interval");
    js_obj_get(ctx, idx, libc_get_timespec, &p->it_value, "value");

    return 0;
}

int
libc_set_itimerspec(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct itimerspec *p = (struct itimerspec *)obj;

    js_obj_set(ctx, idx, libc_set_timespec, &p->it_interval, "interval");
    js_obj_set(ctx, idx, libc_set_timespec, &p->it_value, "value");

    return 0;
}

#if js_LIBC_LINUX
int
libc_get_signalfd_siginfo(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct signalfd_siginfo *p = (struct signalfd_siginfo *)obj; os_objzero(p);
    
    p->ssi_signo    = js_get_obj_uint(ctx, idx, "signo");
    p->ssi_errno    = js_get_obj_int(ctx, idx, "errno");
    p->ssi_code     = js_get_obj_int(ctx, idx, "code");
    p->ssi_pid      = js_get_obj_uint(ctx, idx, "pid");
    p->ssi_uid      = js_get_obj_uint(ctx, idx, "uid");
    p->ssi_fd       = js_get_obj_int(ctx, idx, "fd");
    p->ssi_tid      = js_get_obj_uint(ctx, idx, "tid");
    p->ssi_band     = js_get_obj_uint(ctx, idx, "band");
    p->ssi_overrun  = js_get_obj_uint(ctx, idx, "overrun");
    p->ssi_trapno   = js_get_obj_uint(ctx, idx, "trapno");
    p->ssi_status   = js_get_obj_int(ctx, idx, "status");
    p->ssi_int      = js_get_obj_int(ctx, idx, "int");

    return 0;
}

int
libc_set_signalfd_siginfo(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct signalfd_siginfo *p = (struct signalfd_siginfo *)obj;
    
    js_set_obj_uint(ctx, idx, "signo", p->ssi_signo);
    js_set_obj_int(ctx, idx, "errno", p->ssi_errno);
    js_set_obj_int(ctx, idx, "code", p->ssi_code);
    js_set_obj_uint(ctx, idx, "pid", p->ssi_pid);
    js_set_obj_uint(ctx, idx, "uid", p->ssi_uid);
    js_set_obj_int(ctx, idx, "fd", p->ssi_fd);
    js_set_obj_uint(ctx, idx, "tid", p->ssi_tid);
    js_set_obj_uint(ctx, idx, "band", p->ssi_band);
    js_set_obj_uint(ctx, idx, "overrun", p->ssi_overrun);
    js_set_obj_uint(ctx, idx, "trapno", p->ssi_trapno);
    js_set_obj_int(ctx, idx, "status", p->ssi_status);
    js_set_obj_int(ctx, idx, "int", p->ssi_int);

    return 0;
}

int
libc_get_watcher_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    loop_watcher_t *p = (loop_watcher_t *)obj; os_objzero(p);

    p->fd       = js_get_obj_int(ctx, idx, "fd");
    p->father   = js_get_obj_int(ctx, idx, "father");
    p->type     = js_get_obj_int(ctx, idx, "type");
    p->flag     = js_get_obj_int(ctx, idx, "flag");

    return 0;
}

int
libc_set_watcher_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    loop_watcher_t *p = (loop_watcher_t *)obj; os_objzero(p);

    js_set_obj_int(ctx, idx, "fd",      p->fd);
    js_set_obj_int(ctx, idx, "father",  p->father);
    js_set_obj_int(ctx, idx, "type",    p->type);
    js_set_obj_int(ctx, idx, "flag",    p->flag);

    return 0;
}

int
libc_get_epoll_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct epoll_event *p = (struct epoll_event *)obj; os_objzero(p);
    
    p->events   = js_get_obj_uint(ctx, idx, "events");
    p->data.fd  = js_get_obj_int(ctx, idx, "fd");

    return 0;
}

int
libc_set_epoll_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct epoll_event *p = (struct epoll_event *)obj;
    
    js_set_obj_uint(ctx, idx, "events", p->events);
    js_set_obj_int(ctx, idx, "fd", p->data.fd);

    return 0;
}

int
libc_get_inotify_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct inotify_event *p = (struct inotify_event *)obj; os_objzero(p);
    
    p->wd   = js_get_obj_int(ctx, idx, "wd");
    p->mask = js_get_obj_uint(ctx, idx, "mask");
    p->cookie = js_get_obj_uint(ctx, idx, "cookie");
    js_copy_obj_string(ctx, idx, "name", p->name, 1+NAME_MAX);
    p->len  = os_strlen(p->name);

    return 0;
}

int
libc_set_inotify_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct inotify_event *p = (struct inotify_event *)obj;
    
    js_set_obj_int(ctx, idx, "wd", p->wd);
    js_set_obj_uint(ctx, idx, "mask", p->mask);
    js_set_obj_uint(ctx, idx, "cookie", p->cookie);
    js_set_obj_string(ctx, idx, "name", p->name);

    return 0;
}
#endif

int
libc_get_tms(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct tms *p = (struct tms *)obj; os_objzero(p);
    
    p->tms_utime    = js_get_obj_uint(ctx, idx, "utime");
    p->tms_stime    = js_get_obj_uint(ctx, idx, "stime");
    p->tms_cutime   = js_get_obj_uint(ctx, idx, "cutime");
    p->tms_cstime   = js_get_obj_uint(ctx, idx, "cstime");

    return 0;
}

int
libc_set_tms(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct tms *p = (struct tms *)obj;
    
    js_set_obj_uint(ctx, idx, "utime", p->tms_utime);
    js_set_obj_uint(ctx, idx, "stime", p->tms_stime);
    js_set_obj_uint(ctx, idx, "cutime", p->tms_cutime);
    js_set_obj_uint(ctx, idx, "cstime", p->tms_cstime);

    return 0;
}

int
libc_get_timezone(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timezone *p = (struct timezone *)obj; os_objzero(p);
    
    p->tz_minuteswest   = js_get_obj_uint(ctx, idx, "minuteswest");
    p->tz_dsttime       = js_get_obj_uint(ctx, idx, "dsttime");

    return 0;
}

int
libc_set_timezone(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timezone *p = (struct timezone *)obj;
    
    js_set_obj_uint(ctx, idx, "minuteswest", p->tz_minuteswest);
    js_set_obj_uint(ctx, idx, "dsttime", p->tz_dsttime);

    return 0;
}

int
libc_get_tm(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct tm *p = (struct tm *)obj; os_objzero(p);
    
    p->tm_sec   = js_get_obj_int(ctx, idx, "sec");
    p->tm_min   = js_get_obj_int(ctx, idx, "min");
    p->tm_hour  = js_get_obj_int(ctx, idx, "hour");
    p->tm_mday  = js_get_obj_int(ctx, idx, "mday");
    p->tm_mon   = js_get_obj_int(ctx, idx, "mon");
    p->tm_wday  = js_get_obj_int(ctx, idx, "wday");
    p->tm_yday  = js_get_obj_int(ctx, idx, "yday");
    p->tm_isdst = js_get_obj_int(ctx, idx, "isdst");
    p->tm_gmtoff= js_get_obj_int(ctx, idx, "gmtoff");
    p->tm_zone  = js_get_obj_string(ctx, idx, "zone", NULL);

    return 0;
}

int
libc_set_tm(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct tm *p = (struct tm *)obj;
    
    js_set_obj_int(ctx, idx, "sec", p->tm_sec);
    js_set_obj_int(ctx, idx, "min", p->tm_min);
    js_set_obj_int(ctx, idx, "hour", p->tm_hour);
    js_set_obj_int(ctx, idx, "mday", p->tm_mday);
    js_set_obj_int(ctx, idx, "mon", p->tm_mon);
    js_set_obj_int(ctx, idx, "wday", p->tm_wday);
    js_set_obj_int(ctx, idx, "yday", p->tm_yday);
    js_set_obj_int(ctx, idx, "isdst", p->tm_isdst);
    js_set_obj_int(ctx, idx, "gmtoff", p->tm_gmtoff);
    js_set_obj_string(ctx, idx, "zone", (char *)p->tm_zone);

    return 0;
}

int
libc_get_ntptimeval(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct ntptimeval *p = (struct ntptimeval *)obj; os_objzero(p);
    
    p->maxerror   = js_get_obj_int(ctx, idx, "maxerror");
    p->esterror  = js_get_obj_int(ctx, idx, "esterror");

    os_objzero(&p->time);

    js_obj_get(ctx, idx, libc_get_timeval, &p->time, "time");

    return 0;
}

int
libc_set_ntptimeval(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct ntptimeval *p = (struct ntptimeval *)obj;
    
    js_set_obj_int(ctx, idx, "maxerror", p->maxerror);
    js_set_obj_int(ctx, idx, "esterror", p->esterror);

    js_obj_set(ctx, idx, libc_set_timeval, &p->time, "time");

    return 0;
}

int
libc_get_timex(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timex *p = (struct timex *)obj; os_objzero(p);
    
    p->modes    = js_get_obj_uint(ctx, idx, "modes");
    p->offset   = js_get_obj_int(ctx, idx, "offset");
#if 0
    p->frequency= js_get_obj_int(ctx, idx, "frequency");
#endif
    p->maxerror = js_get_obj_int(ctx, idx, "maxerror");
    p->esterror = js_get_obj_int(ctx, idx, "esterror");
    p->status   = js_get_obj_int(ctx, idx, "status");
    p->constant = js_get_obj_int(ctx, idx, "constant");
    p->precision= js_get_obj_int(ctx, idx, "precision");
    p->tolerance= js_get_obj_int(ctx, idx, "tolerance");
    p->tick     = js_get_obj_int(ctx, idx, "tick");
    p->ppsfreq  = js_get_obj_int(ctx, idx, "ppsfreq");
    p->jitter   = js_get_obj_int(ctx, idx, "jitter");
    p->shift    = js_get_obj_int(ctx, idx, "shift");
    p->stabil   = js_get_obj_int(ctx, idx, "stabil");
    p->jitcnt   = js_get_obj_int(ctx, idx, "jitcnt");
    p->calcnt   = js_get_obj_int(ctx, idx, "calcnt");
    p->errcnt   = js_get_obj_int(ctx, idx, "errcnt");
    p->stbcnt   = js_get_obj_int(ctx, idx, "stbcnt");

    os_objzero(&p->time);

    js_obj_get(ctx, idx, libc_get_timeval, &p->time, "time");

    return 0;
}

int
libc_set_timex(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timex *p = (struct timex *)obj;
    
    js_set_obj_uint(ctx, idx, "modes", p->modes);
    js_set_obj_int(ctx, idx, "offset", p->offset);
#if 0
    js_set_obj_int(ctx, idx, "frequency", p->frequency);
#endif
    js_set_obj_int(ctx, idx, "maxerror", p->maxerror);
    js_set_obj_int(ctx, idx, "esterror", p->esterror);
    js_set_obj_int(ctx, idx, "status", p->status);
    js_set_obj_int(ctx, idx, "constant", p->constant);
    js_set_obj_int(ctx, idx, "precision", p->precision);
    js_set_obj_int(ctx, idx, "tolerance", p->tolerance);
    js_set_obj_int(ctx, idx, "tick", p->tick);
    js_set_obj_int(ctx, idx, "ppsfreq", p->ppsfreq);
    js_set_obj_int(ctx, idx, "jitter", p->jitter);
    js_set_obj_int(ctx, idx, "shift", p->shift);
    js_set_obj_int(ctx, idx, "stabil", p->stabil);
    js_set_obj_int(ctx, idx, "jitcnt", p->jitcnt);
    js_set_obj_int(ctx, idx, "calcnt", p->calcnt);
    js_set_obj_int(ctx, idx, "errcnt", p->errcnt);
    js_set_obj_int(ctx, idx, "stbcnt", p->stbcnt);

    js_obj_set(ctx, idx, libc_set_timeval, &p->time, "time");

    return 0;
}

int
libc_get_rusage(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct rusage *p = (struct rusage *)obj; os_objzero(p);
    
    p->ru_maxrss = js_get_obj_int(ctx, idx, "maxrss");
    p->ru_ixrss  = js_get_obj_int(ctx, idx, "ixrss");
    p->ru_idrss  = js_get_obj_int(ctx, idx, "idrss");
    p->ru_isrss  = js_get_obj_int(ctx, idx, "isrss");
    p->ru_minflt = js_get_obj_int(ctx, idx, "minflt");
    p->ru_majflt = js_get_obj_int(ctx, idx, "majflt");
    p->ru_nswap  = js_get_obj_int(ctx, idx, "nswap");
    p->ru_inblock= js_get_obj_int(ctx, idx, "inblock");
    p->ru_oublock= js_get_obj_int(ctx, idx, "oublock");
    p->ru_msgsnd = js_get_obj_int(ctx, idx, "msgsnd");
    p->ru_msgrcv = js_get_obj_int(ctx, idx, "msgrcv");
    p->ru_nsignals = js_get_obj_int(ctx, idx, "nsignals");
    p->ru_nvcsw  = js_get_obj_int(ctx, idx, "nvcsw");
    p->ru_nivcsw = js_get_obj_int(ctx, idx, "nivcsw");

    os_objzero(&p->ru_utime);
    os_objzero(&p->ru_stime);

    js_obj_get(ctx, idx, libc_get_timeval, &p->ru_utime, "utime");
    js_obj_get(ctx, idx, libc_get_timeval, &p->ru_stime, "stime");

    return 0;
}

int
libc_set_rusage(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct rusage *p = (struct rusage *)obj;
    
    js_set_obj_int(ctx, idx, "maxrss", p->ru_maxrss);
    js_set_obj_int(ctx, idx, "ixrss", p->ru_ixrss);
    js_set_obj_int(ctx, idx, "idrss", p->ru_idrss);
    js_set_obj_int(ctx, idx, "isrss", p->ru_isrss);
    js_set_obj_int(ctx, idx, "minflt", p->ru_minflt);
    js_set_obj_int(ctx, idx, "majflt", p->ru_majflt);
    js_set_obj_int(ctx, idx, "nswap", p->ru_nswap);
    js_set_obj_int(ctx, idx, "inblock", p->ru_inblock);
    js_set_obj_int(ctx, idx, "oublock", p->ru_oublock);
    js_set_obj_int(ctx, idx, "msgsnd", p->ru_msgsnd);
    js_set_obj_int(ctx, idx, "msgrcv", p->ru_msgrcv);
    js_set_obj_int(ctx, idx, "nsignals", p->ru_nsignals);
    js_set_obj_int(ctx, idx, "nvcsw", p->ru_nvcsw);
    js_set_obj_int(ctx, idx, "nivcsw", p->ru_nivcsw);
    
    js_obj_set(ctx, idx, libc_set_timeval, &p->ru_utime, "utime");
    js_obj_set(ctx, idx, libc_set_timeval, &p->ru_stime, "stime");

    return 0;
}








/******************************************************************************/
