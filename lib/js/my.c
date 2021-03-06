/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      libjs
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     my
#endif

#ifndef __BUSYBOX__
#define __LIB__
#endif

#define __DEAMON__

#include "utils.h"
#include "global.h"
#include "js.h"
#include "my.h"

JS_PARAM(ak_getidbyname, 2);
STATIC duk_ret_t
duke_ak_getidbyname(duk_context *ctx)
{
    char *app = (char *)duk_require_string(ctx, 0);
    char *key = (char *)duk_require_string(ctx, 1);
    
    return duk_push_uint(ctx, __ak_getidbyname(app, key)), 1;
}

JS_PARAM(ak_getidbynameEx, 2);
STATIC duk_ret_t
duke_ak_getidbynameEx(duk_context *ctx)
{
    char *app = (char *)duk_require_string(ctx, 0);
    char *key = (char *)duk_require_string(ctx, 1);
    
    return duk_push_uint(ctx, __ak_getidbynameEx(app, key)), 1;
}

JS_PARAM(ak_get, 2);
STATIC duk_ret_t
duke_ak_get(duk_context *ctx)
{
    uint32 akid = duk_require_uint(ctx, 0);
    uint32 deft = duk_require_uint(ctx, 1);
    
    return duk_push_uint(ctx, ak_get(akid, deft)), 1;
}

JS_PARAM(ak_set, 2);
STATIC duk_ret_t
duke_ak_set(duk_context *ctx)
{
    uint32 akid = duk_require_uint(ctx, 0);
    uint32 value = duk_require_uint(ctx, 1);
    
    return duk_push_int(ctx, ak_set(akid, value)), 1;
}

JS_PARAM(ak_load, 0);
STATIC duk_ret_t
duke_ak_load(duk_context *ctx)
{
    return duk_push_int(ctx, ak_load()), 1;
}

/*
JS_PARAM(is_debug_mod, 1);
static duk_ret_t
duke_is_debug_mod(duk_context *ctx)
{
    bool is = false;

    static char *env_mod;
    static jobj_t jmod;
    static hash_t table;
    
    if (NULL==env_mod) {
        env_mod = env_gets(ENV_duk_DEBUG_MODE, "[]");
    }

    if (NULL==jmod) {
        jmod = jobj(env_mod);
    }

    if (NULL==table) {
        
    }
    
    if (__ak_debug_js) {
        is = __is_js_debug(duk_require_uint(ctx, 0));
    }

    return duk_push_bool(ctx, is), 1;
}
*/

JS_PARAM(is_debug, 1);
STATIC duk_ret_t
duke_is_debug(duk_context *ctx)
{
    bool is = __is_js_debug(duk_require_uint(ctx, 0));
    
    return duk_push_bool(ctx, is), 1;
}

JS_PARAM(debug, 1);
STATIC duk_ret_t
duke_debug(duk_context *ctx)
{
    const char *string = duk_require_string(ctx, 0);

    debug_js(string);

    return 0;
}

JS_PARAM(pexec, 1);
STATIC duk_ret_t
duke_pexec(duk_context *ctx)
{
    int err;
    char *line = (char *)duk_require_string(ctx, 0);
    
    int cb(int error, char *outstring, char *errstring)
    {
        struct pipexec_cache pc;

        pc.outstring = outstring;
        pc.errstring = errstring;
        pc.error = error;

        return js_obj_push(ctx, __set_pipexec_cache, &pc);
    }
    
    pipinfo_t info = PIPINFO_INITER(NULL, cb);
    
    err = os_pexecline(&info, line);
    if (err<0) {
        return err;
    }
    
	return 1;
}

JS_PARAM(shell, 1);
STATIC duk_ret_t
duke_shell(duk_context *ctx)
{
    char *line = (char *)duk_require_string(ctx, 0);
    
    int err = os_system("%s", line);

	return duk_push_int(ctx, err), 1;
}

JS_PARAM(readtxt, 1);
STATIC duk_ret_t
duke_readtxt(duk_context *ctx)
{
    char *buf = NULL;
    uint32 size = 0;
    char *filename = (char *)duk_require_string(ctx, 0);
    
    int err = os_readfileall(filename, &buf, &size, false);
    js_push_lstring(ctx, buf, size);
    os_free(buf);
    
    return 1;
}

JS_PARAM(readbin, 1);
STATIC duk_ret_t
duke_readbin(duk_context *ctx)
{
    char *filename = (char *)duk_require_string(ctx, 0);

    int size = os_fsize(filename);
    if (size<0) {
        return duk_push_null(ctx), 1;
    }

    char *buf = js_push_dynamic_buffer(ctx, size);

    if (os_readfile(filename, buf, size) < 0) {
        duk_pop(ctx);
        duk_push_null(ctx);
    }
    
    return 1;
}

STATIC duk_ret_t
my_writefile(duk_context *ctx, bool append)
{
    duk_buffer_t buf = NULL;
    duk_size_t bsize = 0;
    bool bin = false;
    
    char *filename = (char *)duk_require_string(ctx, 0);
    int err = duk_require_buffer_or_lstring(ctx, 1, &buf, &bsize);
    if (err<0) {
        __js_seterrno(ctx, err); goto error;
    }

    if (2==duk_get_max_idx(ctx)) {
        bin = duk_require_bool(ctx, 2);
    }

    err = os_writefile(filename, buf, bsize, append, bin);
error:
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(writefile, DUK_VARARGS);
STATIC duk_ret_t
duke_writefile(duk_context *ctx)
{
    return my_writefile(ctx, false);
}

JS_PARAM(appendfile, DUK_VARARGS);
STATIC duk_ret_t
duke_appendfile(duk_context *ctx)
{
    return my_writefile(ctx, true);
}

JS_PARAM(cleanfile, 1);
STATIC duk_ret_t
duke_cleanfile(duk_context *ctx)
{
    char *filename = (char *)duk_require_string(ctx, 0);

    return os_fclean(filename), 0;
}

JS_PARAM(readline, 2);
STATIC duk_ret_t
duke_readline(duk_context *ctx)
{
    int err = 0, len;
    STREAM f = NULL;
    char *line = NULL;
    
    char *filename = (char *)duk_require_string(ctx, 0);
    
    line = (char *)os_malloc(1+OS_FILE_LEN);
    if (NULL==line) {
        err = __js_seterrno(ctx, -ENOMEM); goto error;
    }
    
    f = os_fopen(filename, "r");
    if (NULL==f) {
        err = __js_seterrno(ctx, -ENOEXIST); goto error;
    }
    
    while(!os_feof(f)) {
        line[0] = 0;
        os_freadline(f, line, OS_LINE_LEN);
        /*
        * cut tail
        *   "\n"
        *   "\r"
        *   "\r\n"
        */
        len = os_strlen(line);
        if (len>=1 && '\n'==line[len-1]) {
            line[len-1] = 0; len -= 1;
        }
        else if (len>=2 && '\r'==line[len-2] && '\n'==line[len-1]) {
            line[len-2] = 0; len -= 2;
        }

        if (false==is_good_str(line)) {
            continue;
        }

        duk_dup(ctx, 1);                        // dup callback         , callback
        js_push_lstring(ctx, line, len);         // push line            , callback line
        int exec = duk_pcall(ctx, 1);           // call callback(line)  , result/error
        err = duk_get_int(ctx, -1);             // get callback error
        duk_pop(ctx);                           // pop callback result  , empty
        
        if (DUK_EXEC_ERROR==exec) { // check callback exec
            err = __js_seterrno(ctx, -ESCRIPT); goto error;
        }
        else if (err<0) {             // check callback result
            goto error;
        }
    }

    err = 0;
error:
    os_fclose(f);
    os_free(line);
    
    return duk_push_int(ctx, err), 1;
}

typedef struct {
    char *name;
} loop_obj_t;

#define LOOP_OBJ(_name) { .name = _name }

#define LOOP_HANDLE     "handle"
#define LOOP_PARAM      "param"

#define LOOP_OBJ_INITER { \
    [LOOP_TYPE_INOTIFY] = LOOP_OBJ("inotify"),  \
    [LOOP_TYPE_SIGNAL]  = LOOP_OBJ("signal"),   \
    [LOOP_TYPE_TIMER]   = LOOP_OBJ("timer"),    \
    [LOOP_TYPE_NORMAL]  = LOOP_OBJ("normal"),   \
    [LOOP_TYPE_FATHER]  = LOOP_OBJ("father"),   \
}   /* end */

STATIC void
js_my_loop_pcall(duk_context *ctx, duk_idx_t idx, loop_obj_t *obj, int (*push)(void))
{
    int iobj = duk_get_prop_string(ctx, idx, obj->name);
    duk_push_string(ctx, LOOP_HANDLE);
    duk_pcall_prop(ctx, iobj, (*push)());
    duk_pop_2(ctx);
}

STATIC int
js_my_loop_watcher_initer(duk_context *ctx, int idx, loop_obj_t *obj, int *level)
{
    bool ok;

    // push signal
    (*level)++;
    ok = duk_get_prop_string(ctx, idx, obj->name);
    if (false==ok) {
        debug_trace("no %s loop", obj->name);
        
        return -ENOEXIST;
    }
    int iobj = duk_normalize_index(ctx, -1);
    
    // push handle
    (*level)++;
    ok = duk_get_prop_string(ctx, iobj, LOOP_HANDLE);
    if (false==ok) {
        debug_trace("invalid %s loop handle", obj->name);
        
        return -ENOEXIST;
    }
    
    // push param
    (*level)++;
    ok = duk_get_prop_string(ctx, iobj, LOOP_PARAM);
    if (false==ok) {
        debug_trace("invalid %s loop param", obj->name);
        
        return -ENOEXIST;
    }

    return 0;
}

STATIC int
js_my_loop_inotify_initer(loop_t *loop, loop_obj_t *obj, duk_context *ctx, int idx, loop_inotify_f *cb)
{
    int err = 0, level = 0;

    // push obj
    // push handle
    // push param
    err = js_my_loop_watcher_initer(ctx, idx, obj, &level);
    if (err<0) {
        duk_pop_n(ctx, level); return 0;
    }
    
    if (false==duk_is_array(ctx, -1)) {
        debug_trace("%s loop param isn's array", obj->name);
        
        duk_pop_n(ctx, level); return 0;
    }
    
    int i, count = js_get_array_length(ctx, -1);
    if (count<=0) {
        debug_trace("invalid %s loop param array count", obj->name);
        
        duk_pop_n(ctx, level); return 0;
    }
    
    loop_inotify_t inotify[count];
    
    for (i=0; i<count; i++) {
        duk_get_prop_index(ctx, -1, i); level++;
    
        if (false==duk_is_object(ctx, -1)) {
            debug_trace("%s loop param array item isn't object", obj->name);
            
            duk_pop_n(ctx, level); return 0;
        }
        
        inotify[i].path = js_get_obj_string(ctx, -1, "path", NULL);
        inotify[i].mask = js_get_obj_uint(ctx, -1, "mask");

        duk_pop(ctx); level--;
    }

    err = os_loop_add_inotify(loop, cb, inotify, count);

    duk_pop_n(ctx, level); return err;
}

STATIC int
js_my_loop_signal_initer(loop_t *loop, loop_obj_t *obj, duk_context *ctx, int idx, loop_signal_f *cb)
{
    int err = 0, level = 0;

    // push obj
    // push handle
    // push param
    err = js_my_loop_watcher_initer(ctx, idx, obj, &level);
    if (err<0) {
        duk_pop_n(ctx, level); return 0;
    }
    
    if (false==duk_is_array(ctx, -1)) {
        debug_trace("%s loop param isn's array", obj->name);
        
        duk_pop_n(ctx, level); return 0;
    }
    
    int i, count = js_get_array_length(ctx, -1);
    if (count<=0) {
        debug_trace("invalid %s loop param array count", obj->name);
        
        duk_pop_n(ctx, level); return 0;
    }

    int sigs[count];
    for (i=0; i<count; i++) {
        sigs[i] = js_get_array_int(ctx, -1, i);
    }

    err = os_loop_add_signal(loop, cb, sigs, count);
    
    duk_pop_n(ctx, level); return err;
}

STATIC int
js_my_loop_timer_initer(loop_t *loop, loop_obj_t *obj, duk_context *ctx, int idx, loop_timer_f *cb)
{
    int err = 0, level = 0;

    // push obj
    // push handle
    // push param
    err = js_my_loop_watcher_initer(ctx, idx, obj, &level);
    if (err<0) {
        duk_pop_n(ctx, level); return 0;
    }
    
    if (false==duk_is_object(ctx, -1)) {
        debug_trace("%s loop param isn's object", obj->name);
        
        duk_pop_n(ctx, level); return 0;
    }

    struct itimerspec tm;
    libc_get_itimerspec(ctx, -1, &tm);

    err = os_loop_add_timer(loop, cb, &tm);

    duk_pop_n(ctx, level); return err;
}

STATIC int
js_my_loop_normal_initer(loop_t *loop, loop_obj_t *obj, duk_context *ctx, int idx, loop_normal_f *cb)
{
    int fd, err = 0, level = 0;

    // push obj
    // push handle
    // push param
    err = js_my_loop_watcher_initer(ctx, idx, obj, &level);
    if (err<0) {
        duk_pop_n(ctx, level); return 0;
    }
    
    if (false==duk_is_array(ctx, -1)) {
        debug_trace("%s loop param isn's array", obj->name);
        
        duk_pop_n(ctx, level); return 0;
    }
    
    int i, count = js_get_array_length(ctx, -1);
    if (count<=0) {
        debug_trace("invalid %s loop param array count", obj->name);
        
        duk_pop_n(ctx, level); return 0;
    }

    for (i=0; i<count; i++) {
        fd = js_get_array_int(ctx, -1, i);

        err = os_loop_add_normal(loop, fd, cb, NULL);
        if (err<0) {
            duk_pop_n(ctx, level); return 0;
        }
    }
    
    duk_pop_n(ctx, level); return err;
}

STATIC int
js_my_loop_father_initer(loop_t *loop, loop_obj_t *obj, duk_context *ctx, int idx, loop_son_f *cb)
{
    int fd, err = 0, level = 0;

    // push obj
    // push handle
    // push param
    err = js_my_loop_watcher_initer(ctx, idx, obj, &level);
    if (err<0) {
        duk_pop_n(ctx, level); return 0;
    }
    
    if (false==duk_is_array(ctx, -1)) {
        debug_trace("%s loop param isn's array", obj->name);
        
        duk_pop_n(ctx, level); return 0;
    }
    
    int i, count = js_get_array_length(ctx, -1);
    if (count<=0) {
        debug_trace("invalid %s loop param array count", obj->name);
        
        duk_pop_n(ctx, level); return 0;
    }

    for (i=0; i<count; i++) {
        fd = js_get_array_int(ctx, -1, i);

        err = os_loop_add_father(loop, fd, cb, 0, NULL);
        if (err<0) {
            duk_pop_n(ctx, level); return 0;
        }
    }
    
    duk_pop_n(ctx, level); return err;
}

// TODO: inotify on loop
JS_PARAM(loop, 1);
STATIC duk_ret_t
duke_loop(duk_context *ctx)
{
    int err = 0;

    loop_obj_t objs[LOOP_TYPE_END] = LOOP_OBJ_INITER;
    loop_t loop = LOOP_INITER;

    int idx = duk_normalize_index(ctx, -1);
    if (false==duk_is_object(ctx, idx)) {
        __js_seterrno(ctx, -EFORMAT); goto error;
    }

    int __inotify_handle(loop_watcher_t *watcher, struct inotify_event *ev, time_t now)
    {
        int push_inotify(void) {
            return js_obj_push(ctx, libc_set_inotify_event, ev), 1;
        }
        
        js_my_loop_pcall(ctx, idx, &objs[LOOP_TYPE_INOTIFY], push_inotify);
    
        return 0;
    }

    int __signal_handle(loop_watcher_t *watcher, struct signalfd_siginfo *siginfo, time_t now)
    {
        int push_signal(void) {
            return js_obj_push(ctx, libc_set_signalfd_siginfo, siginfo), 1;
        }
    
        js_my_loop_pcall(ctx, idx, &objs[LOOP_TYPE_SIGNAL], push_signal);
    
        return 0;
    }

    int __timer_handle(loop_watcher_t *watcher, time_t now)
    {
        int push_timer(void) {
            return duk_push_uint(ctx, now), 1;
        }
    
        js_my_loop_pcall(ctx, idx, &objs[LOOP_TYPE_TIMER], push_timer);
    
        return 0;
    }

    int __normal_handle(loop_watcher_t *watcher, time_t now)
    {
        int push_normal(void) {
            return js_obj_push(ctx, libc_set_watcher_event, watcher), 1;
        }
    
        js_my_loop_pcall(ctx, idx, &objs[LOOP_TYPE_NORMAL], push_normal);
    
        return 0;
    }

    int __father_handle(loop_watcher_t *watcher, time_t now)
    {
        int push_father(void) {
            return js_obj_push(ctx, libc_set_watcher_event, watcher), 1;
        }

        js_my_loop_pcall(ctx, idx, &objs[LOOP_TYPE_FATHER], push_father);
    
        return 0;
    }
    
    err = js_my_loop_inotify_initer(&loop, &objs[LOOP_TYPE_INOTIFY], ctx, idx, __inotify_handle);
    if (err<0) {
        __js_seterrno(ctx, err); goto error;
    }
    
    err = js_my_loop_signal_initer(&loop, &objs[LOOP_TYPE_SIGNAL], ctx, idx, __signal_handle);
    if (err<0) {
        __js_seterrno(ctx, err); goto error;
    }
    
    err = js_my_loop_timer_initer(&loop, &objs[LOOP_TYPE_TIMER], ctx, idx, __timer_handle);
    if (err<0) {
        __js_seterrno(ctx, err); goto error;
    }
    
    err = js_my_loop_normal_initer(&loop, &objs[LOOP_TYPE_NORMAL], ctx, idx, __normal_handle);
    if (err<0) {
        __js_seterrno(ctx, err); goto error;
    }
    
    err = js_my_loop_father_initer(&loop, &objs[LOOP_TYPE_FATHER], ctx, idx, __father_handle);
    if (err<0) {
        __js_seterrno(ctx, err); goto error;
    }

    err = os_loop(&loop);

error:
    return duk_push_int(ctx, err), 1;
}

#if js_LIBCALL
STATIC duk_ret_t
js_my_libcall(duk_context *ctx)
{
    int err = 0;
    duk_string_t lib = (duk_string_t)duk_require_string(ctx, 0);
    duk_string_t sym = (duk_string_t)duk_require_string(ctx, 1);
    int count = js_get_array_length(ctx, 2);
    if (count<=0) {
        return -EKEYBAD;
    }
    
    libval_t result;
    __get_libval_t(ctx, 3, &result);

    libval_t *param;
    libval_t params[count];
    libproto_t proto = ____LIBPROTO_INITER(result.size, params, count);

    int i;
    for (i=0; i<count; i++) {
        param = &params[i];
        
        duk_get_prop_index(ctx, 2, i);
        __get_libval_t(ctx, -1, param);
        duk_pop(ctx);
    }

    err = os_libcall(lib, sym, &proto);
    if (err<0) {
        return err;
    }

    for (i=0; i<count; i++) {
        param = &params[i];
        
        if (LIBVAL_OUT & param->dir) {
            duk_get_prop_index(ctx, 2, i);
            __set_libval_t(ctx, -1, param);
            duk_pop(ctx);
        }
    }

    __set_libval_t(ctx, 3, &proto.result);

    return 0;
}

JS_PARAM(libcall, 4);
STATIC duk_ret_t
duke_libcall(duk_context *ctx)
{
    return duk_push_int(ctx, js_my_libcall(ctx)), 1;
}
#endif

STATIC void 
js_my_env_register(duk_context *ctx)
{
    char *env = *environ;
    char *k, *v;
    int i;
    
    duk_push_object(ctx);
    for (i=1; env; i++) {
        k = strdup(env);
        v = strchr(k, '=');
        if (NULL!=v) {
            *v++ = 0;

            js_set_obj_string(ctx, -1, k, v);
        }
        os_free(k);
        
        env = *(environ+i);
    }
    duk_put_prop_string(ctx, -2, "env");
}

STATIC void 
js_my_arg_register(duk_context *ctx)
{
    js_priv_t *priv = js_priv(ctx);

#if 0
    js_set_obj_string(ctx, -1, "priv", priv->name);
    js_set_obj_int(ctx, -1, "mode", priv->mode);
#endif

    if (JS_EVAL_BUILDIN != priv->mode) {
        js_set_obj_string(ctx, -1, "name", priv->argv[0]);
    }
    
    if (JS_EVAL_SHABANG == priv->mode) {
        js_set_obj_string(ctx, -1, "script", priv->argv[1]);
        
        int i, argc = priv->argc - 2;
        char **argv = priv->argv + 2;
        duk_push_array(ctx);
        for (i=0; i<argc; i++) {
            js_set_array_string(ctx, -1, i, argv[i]);
        }
        duk_put_prop_string(ctx, -2, "argv");
    }
}

STATIC const dukc_func_entry_t js_my_func[] = {
    JS_FUNC(ak_getidbyname),
    JS_FUNC(ak_getidbynameEx),
    JS_FUNC(ak_get),
    JS_FUNC(ak_set),
    JS_FUNC(ak_load),
    JS_FUNC(is_debug),
    JS_FUNC(debug),
    JS_FUNC(pexec),
    JS_FUNC(shell),
    JS_FUNC(readtxt),
    JS_FUNC(readbin),
    JS_FUNC(readline),
    JS_FUNC(writefile),
    JS_FUNC(appendfile),
    JS_FUNC(cleanfile),
    JS_FUNC(loop),
#if js_LIBCALL
    JS_FUNC(libcall),
#endif

    JS_FUNC_END
};

STATIC const dukc_number_entry_t js_my_static_number[] = {
    JS_VALUE_END
};

int js_buildin_register(duk_context *ctx)
{
    int err = 0;
    
    err = js_load_code(ctx, "buildin", duk_global_CODE);

    debug_ok_error(err, "register buildin");

    return err;
}

int js_auto_register(duk_context *ctx)
{
    char autopath[1+OS_LINE_LEN] = {0};
    char *env = os_strdup(env_gets(ENV_JPATH, js_PATH));
    char *sub;
    
    bool __filter(const char *path, const char *file)
    {
        /*
        * NOT js file, skip it
        */
        return false==os_str_is_end_by(file, ".js");
    }
    
    mv_t __handler(const char *path, const char *file, os_fscan_line_handle_f *line_handle)
    {
        (void)line_handle;
        
        char filename[1+OS_LINE_LEN] = {0};

        os_saprintf(filename, "%s/%s", path, file);

        int err = js_load_file(ctx, filename);
        if (err<0) {
            return mv2_go(err);
        }
        
        return mv2_ok;
    }

    /*
    * try js_PATH/.auto/xxx.js
    */
    os_strtok_foreach(sub, env, ":") {
        os_saprintf(autopath, "%s/" js_AUTO_PATH, sub);
        
        os_fscan_dir(autopath, false, __filter, __handler, NULL);
    }
    os_free(env);
        
    return 0;
}

int js_my_register(duk_context *ctx)
{
    const dukc_number_entry_t my_dynamic_number[] = {
        __JS_VALUE("SIZEOF_SHORT",     sizeof(short)),
        __JS_VALUE("SIZEOF_INT",       sizeof(int)),
        __JS_VALUE("SIZEOF_LONG",      sizeof(long)),
        __JS_VALUE("SIZEOF_LONGLONG",  sizeof(long long)),
        __JS_VALUE("SIZEOF_POINTER",   sizeof(void *)),

        __JS_VALUE("BIG_ENDIAN",       !!(*(uint16 *)"\x12\x34"==0x1234)),
        __JS_VALUE("LITTLE_ENDIAN",    !!(*(uint16 *)"\x12\x34"==0x3412)),

        JS_VALUE_END
    };
    
    duk_push_global_object(ctx);
        duk_push_object(ctx);
            js_put_functions(ctx, -1, js_my_func);
            duk_put_number_list(ctx, -1, js_my_static_number);
            duk_put_number_list(ctx, -1, my_dynamic_number);
            
            js_my_env_register(ctx);
            js_my_arg_register(ctx);
        duk_put_prop_string(ctx, -2, js_MOD_MY);
    duk_pop(ctx);

    debug_ok("register my ok.");
    
    return 0;
}

