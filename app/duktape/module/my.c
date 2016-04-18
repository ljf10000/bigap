#ifndef __THIS_APP
#define __THIS_APP      js
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     my
#endif

#include "utils.h"
#include "utils/pipe.h"
#include "dukc.h"
#include "my.h"

#if 0
LIB_PARAM(ak_getbyname, 2);
static duk_ret_t
duke_ak_getbyname(duk_context *ctx)
{
    char *app = (char *)duk_require_string(ctx, 0);
    char *key = (char *)duk_require_string(ctx, 1);
    
    return duk_push_uint(ctx, __ak_getbyname(app, key)), 1;
}

LIB_PARAM(ak_get, 2);
static duk_ret_t
duke_ak_get(duk_context *ctx)
{
    uint32_t akid = duk_require_uint(ctx, 0);
    uint32_t deft = duk_require_uint(ctx, 1);
    
    return duk_push_uint(ctx, ak_get(akid, deft)), 1;
}

LIB_PARAM(ak_set, 2);
static duk_ret_t
duke_ak_set(duk_context *ctx)
{
    uint32_t akid = duk_require_uint(ctx, 0);
    uint32_t value = duk_require_uint(ctx, 1);
    
    return duk_push_int(ctx, ak_set(akid, value)), 1;
}

LIB_PARAM(ak_reload, 0);
static duk_ret_t
duke_ak_reload(duk_context *ctx)
{
    return duk_push_int(ctx, ak_reload()), 1;
}
#endif

/*
LIB_PARAM(is_debug_mod, 1);
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

LIB_PARAM(is_debug, 1);
static duk_ret_t
duke_is_debug(duk_context *ctx)
{
    bool is = __is_js_debug(duk_require_uint(ctx, 0));
    
    return duk_push_bool(ctx, is), 1;
}

LIB_PARAM(debug, 1);
static duk_ret_t
duke_debug(duk_context *ctx)
{
    const char *string = duk_require_string(ctx, 0);

    return debug_js(string), 0;
}

LIB_PARAM(pipe, 1);
static duk_ret_t
duke_pipe(duk_context *ctx)
{
    int err;
    char *line = (char *)duk_require_string(ctx, 0);
    
    int cb(pipe_info_t *info)
    {
        return __obj_push(ctx, __set___info_t, info);
    }

    err = os_pipe_system(cb, line);
    if (err<0) {
        return err;
    }
    
	return 1;
}

LIB_PARAM(shell, 1);
static duk_ret_t
duke_shell(duk_context *ctx)
{
    char *line = (char *)duk_require_string(ctx, 0);
    
    int err = os_system("%s", line);

	return duk_push_int(ctx, err), 1;
}

LIB_PARAM(readtxt, 1);
static duk_ret_t
duke_readtxt(duk_context *ctx)
{
    char *buf = NULL;
    uint32_t size = 0;
    char *filename = (char *)duk_require_string(ctx, 0);
    
    int err = os_readfileall(filename, &buf, &size, false);
    __push_lstring(ctx, buf, size);
    os_free(buf);
    
    return 1;
}

LIB_PARAM(readbin, 1);
static duk_ret_t
duke_readbin(duk_context *ctx)
{
    char *filename = (char *)duk_require_string(ctx, 0);

    int size = os_fsize(filename);
    if (size<0) {
        return duk_push_null(ctx), 1;
    }

    char *buf = __push_dynamic_buffer(ctx, size);

    if (os_readfile(filename, buf, size) < 0) {
        duk_pop(ctx);
        duk_push_null(ctx);
    }
    
    return 1;
}

static duk_ret_t
__writefile(duk_context *ctx, bool append)
{
    duk_buffer_t buf = NULL;
    duk_size_t bsize = 0;
    bool bin = false;
    
    char *filename = (char *)duk_require_string(ctx, 0);
    int err = duk_require_buffer_or_lstring(ctx, 1, &buf, &bsize);
    if (err<0) {
        __seterrno(ctx, err); goto error;
    }

    if (2==duk_get_max_idx(ctx)) {
        bin = duk_require_bool(ctx, 2);
    }

    err = os_writefile(filename, buf, bsize, append, bin);
error:
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(writefile, 2);
static duk_ret_t
duke_writefile(duk_context *ctx)
{
    return __writefile(ctx, false);
}

LIB_PARAM(appendfile, DUK_VARARGS);
static duk_ret_t
duke_appendfile(duk_context *ctx)
{
    return __writefile(ctx, true);
}

LIB_PARAM(readline, 2);
static duk_ret_t
duke_readline(duk_context *ctx)
{
    int err = 0, len;
    FILE *f = NULL;
    char *line = NULL;
    
    char *filename = (char *)duk_require_string(ctx, 0);

    line = (char *)os_malloc(1+OS_FILE_LEN);
    if (NULL==line) {
        err = __seterrno(ctx, -ENOMEM); goto error;
    }
    
    f = os_fopen(filename, "r");
    if (NULL==f) {
        err = __seterrno(ctx, -ENOEXIST); goto error;
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
        if (len>=1 && ('\n'==line[len-1] || '\r'==line[len-1])) {
            line[len-1] = 0; len -= 1;
        }
        else if (len>=2 && '\r'==line[len-2] && '\n'==line[len-1]) {
            line[len-2] = 0; len -= 2;
        }

        duk_dup(ctx, 1);                        // dup callback         , callback
        __push_lstring(ctx, line, len+1);       // push line            , callback line
        int exec = duk_pcall(ctx, 1);           // call callback(line)  , result/error
        if (DUK_EXEC_SUCCESS==exec) {
            err = duk_get_int(ctx, -1);         // get callback error
        }
        duk_pop(ctx);                           // pop callback result  , empty
        
        if (DUK_EXEC_ERROR==exec) { // check callback exec
            err = __seterrno(ctx, -ESCRIPT); goto error;
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
    char *obj;
    char *param;
    
    int fd;
    int func;   /* func index */
    bool used;
} loop_info_t;

#define LOOP_INFO(_obj, _param) {   \
    .fd     = INVALID_FD,   \
    .func   = 0,            \
    .used   = false,        \
    .obj    = _obj,         \
    .param  = _param,       \
}

#define LOOP_HANDLE     "handle"

static loop_info_t __loop_signal    = LOOP_INFO("signal",   "sigs");
static loop_info_t __loop_timer     = LOOP_INFO("timer",    "spec");
static loop_info_t __loop_epoll     = LOOP_INFO("epoll",    "fds");
static loop_info_t __loop_inotify   = LOOP_INFO("inotify",  "paths");

static loop_info_t *LOOP[] = {
    &__loop_signal,
    &__loop_timer,
    &__loop_epoll,
    &__loop_inotify,
};

static int loop_param_idx;

static int
loop_add(int fd)
{
    struct epoll_event ev;
    
    ev.events   = EPOLLIN;
    ev.data.fd  = fd;
    
    return epoll_ctl(__loop_epoll.fd, EPOLL_CTL_ADD, fd, &ev);
}

static int
loop_signal_init(duk_context *ctx)
{
    int err = 0, pop = 0;

    if (++pop, false==duk_get_prop_string(ctx, loop_param_idx, __loop_signal.obj)) {
        goto error;
    }

    if (++pop, false==duk_get_prop_string(ctx, -1, LOOP_HANDLE)) {
        goto error;
    }
    --pop; duk_pop(ctx);
    
    if (++pop, false==duk_get_prop_string(ctx, -1, __loop_signal.param)) {
        goto error;
    }
    
    if (false==duk_is_array(ctx, -1)) {
        goto error;
    }
    
    int i, count = __get_array_length(ctx, -1);
    if (count<=0) {
        goto error;
    }
    
    sigset_t set;
    sigemptyset(&set);
    for (i=0; i<count; i++) {
        sigaddset(&set, __get_array_int(ctx, -1, i));
    }
    sigprocmask(SIG_SETMASK, &set, NULL);

    __loop_signal.fd = signalfd(-1, &set, 0/*EFD_CLOEXEC*/);
    if (__loop_signal.fd<0) {
        err = -errno; goto error;
    }

    err = loop_add(__loop_signal.fd);
    if (err<0) {
        goto error;
    }

    __loop_signal.used = true;
error:
    duk_pop_n(ctx, pop);
    
    return err;
}

static int
loop_timer_init(duk_context *ctx)
{
    int err = 0, pop = 0;

    if (++pop, false==duk_get_prop_string(ctx, loop_param_idx, __loop_timer.obj)) {
        goto error;
    }

    if (++pop, false==duk_get_prop_string(ctx, -1, LOOP_HANDLE)) {
        goto error;
    }
    --pop; duk_pop(ctx);

    if (++pop, false==duk_get_prop_string(ctx, -1, __loop_timer.param)) {
        goto error;
    }

    if (false==duk_is_object(ctx, -1)) {
        goto error;
    }

    struct itimerspec new;
    __get_itimerspec(ctx, -1, &new);
    if (0==new.it_value.tv_sec && 0==new.it_value.tv_nsec) {
        new.it_value.tv_nsec = 1;
    }

    __loop_timer.fd = timerfd_create(CLOCK_MONOTONIC, EFD_CLOEXEC);
    if (__loop_timer.fd<0) {
        err = -errno; goto error;
    }

    err = timerfd_settime(__loop_timer.fd, 0, &new, NULL);
    if (err<0) {
        err = -errno; goto error;
    }

    err = loop_add(__loop_timer.fd);
    if (err<0) {
        goto error;
    }

    __loop_timer.used = true;
error:
    duk_pop_n(ctx, pop);

    return err;
}

static int
loop_epoll_init(duk_context *ctx)
{
    int err = 0, pop = 0;

    if (++pop, false==duk_get_prop_string(ctx, loop_param_idx, __loop_epoll.obj)) {
        goto error;
    }
    
    if (++pop, false==duk_get_prop_string(ctx, -1, LOOP_HANDLE)) {
        goto error;
    }
    --pop; duk_pop(ctx);
    
    if (++pop, false==duk_get_prop_string(ctx, -1, __loop_epoll.param)) {
        goto error;
    }
    
    if (false==duk_is_array(ctx, -1)) {
        goto error;
    }
    
    int i, count = __get_array_length(ctx, -1);
    if (count<=0) {
        goto error;
    }

    for (i=0; i<count; i++) {
        err = loop_add(__get_array_int(ctx, -1, i));
        if (err<0) {
            goto error;
        }
    }

    __loop_epoll.used = true;
error:
    duk_pop_n(ctx, pop);
    
    return err;
}

static int
loop_inotify_init(duk_context *ctx)
{
    int err = 0, pop = 0;

    if (++pop, false==duk_get_prop_string(ctx, loop_param_idx, __loop_inotify.obj)) {
        goto error;
    }
    
    if (++pop, false==duk_get_prop_string(ctx, -1, LOOP_HANDLE)) {
        goto error;
    }
    --pop; duk_pop(ctx);
    
    __loop_inotify.fd = inotify_init1(IN_CLOEXEC);
    if (__loop_inotify.fd<0) {
        err = -errno; goto error;
    }
    
    err = loop_add(__loop_inotify.fd);
    if (err<0) {
        goto error;
    }
    __loop_signal.used = true;
    
    if (++pop, false==duk_get_prop_string(ctx, -1, __loop_inotify.param)) {
        goto error;
    }
    
    if (false==duk_is_array(ctx, -1)) {
        goto error;
    }
    
    int i, count = __get_array_length(ctx, -1);
    if (count<=0) {
        goto error;
    }
    
    for (i=0; i<count; i++) {
        char *path = __get_array_string(ctx, -1, i, NULL);
        uint32_t mask = __get_array_uint(ctx, -1, i);

        err = inotify_add_watch(__loop_inotify.fd, path, mask);
        if (err<0) {
            goto error;
        }
    }

error:
    duk_pop_n(ctx, pop);
    
    return err;
}

static void
loop_save(duk_context *ctx)
{
    int i;

    for (i=0; i<os_count_of(LOOP); i++) {
        if (LOOP[i]->used) {
            duk_get_prop_string(ctx, loop_param_idx, LOOP[i]->obj);
            __set_obj_int(ctx, -1, "fd", LOOP[i]->fd);
            duk_pop(ctx);
        }
    }
}

static void
loop_push_func(duk_context *ctx)
{
    int i;

    for (i=0; i<os_count_of(LOOP); i++) {
        if (LOOP[i]->used) {
            duk_get_prop_string(ctx, loop_param_idx, LOOP[i]->obj);
            duk_get_prop_string(ctx, -1, LOOP_HANDLE);
            duk_swap(ctx, -1, -2);
            duk_pop(ctx);
            
            LOOP[i]->func = duk_normalize_index(ctx, -1);
        }
    }
}

static void
loop_pop_func(duk_context *ctx)
{
    int i;

    for (i=0; i<os_count_of(LOOP); i++) {
        if (LOOP[i]->used) {
            duk_pop(ctx);
        }
    }
}

static int
loop_init(duk_context *ctx)
{
    int i, err = 0;
    
    static int (*init[])(duk_context *) = {
        loop_signal_init,
        loop_timer_init,
        loop_inotify_init,
        loop_epoll_init,
    };

    if (false==duk_is_object(ctx, loop_param_idx)) {
        return -EFORMAT;
    }

    __loop_epoll.fd = epoll_create1(EPOLL_CLOEXEC);
    if (__loop_epoll.fd<0) {
        return -errno;
    }

    for (i=0; i<os_count_of(init); i++) {
        err = (*init[i])(ctx);
        if (err<0) {
            return err;
        }
    }
    
    return 0;
}

static void
loop_fini(void)
{
    os_close(__loop_epoll.fd);
    os_close(__loop_timer.fd);
    os_close(__loop_signal.fd);
    os_close(__loop_inotify.fd);
}

static void
loop_signal_handle(duk_context *ctx)
{
    struct signalfd_siginfo siginfo;

    int push_signal(void) {
        return __obj_push(ctx, __set_signalfd_siginfo, &siginfo), 1;
    }

    int len = read(__loop_signal.fd, &siginfo, sizeof(siginfo));
    if (len==sizeof(siginfo)) {
        __pcall(ctx, __loop_signal.func, push_signal);
    }
}

static void
loop_timer_handle(duk_context *ctx)
{
    uint64_t val = 0;
    
    int push_timer(void) {
        return duk_push_int(ctx, (int)val), 1;
    }
    
    int len = read(__loop_timer.fd, &val, sizeof(val));
    if (len==sizeof(val)) {
        __pcall(ctx, __loop_timer.func, push_timer);
    }
}

static void
loop_epoll_handle(duk_context *ctx, struct epoll_event *ev)
{
    int push_epoll(void) {
        return __obj_push(ctx, __set_epoll_event, ev), 1;
    }

    __pcall(ctx, __loop_epoll.func, push_epoll);
}

static void
loop_inotify_handle(duk_context *ctx)
{
    struct inotify_event val;
    
    int push_inotify(void) {
        return __obj_push(ctx, __set_inotify_event, &val), 1;
    }
    
    int len = read(__loop_inotify.fd, &val, sizeof(val));
    if (len==sizeof(val)) {
        __pcall(ctx, __loop_inotify.func, push_inotify);
    }
}

static int
loop_handle(duk_context *ctx)
{
    struct epoll_event evs[32];
    int i;
    
    int nfds = epoll_wait(__loop_epoll.fd, evs, os_count_of(evs), -1);
    if (nfds<0) {
        if (EINTR==errno) {
            return 0;
        } else {
            return -errno;
        }
    }

    for (i=0; i<nfds; i++) {
        struct epoll_event *ev = &evs[i];
        
        if (__loop_signal.used && ev->data.fd==__loop_signal.fd) {
            loop_signal_handle(ctx);
        }
        else if (__loop_timer.used && ev->data.fd==__loop_timer.fd) {
            loop_timer_handle(ctx);
        }
        else if (__loop_inotify.used && ev->data.fd==__loop_inotify.fd) {
            loop_inotify_handle(ctx);
        }
        else if (__loop_epoll.used) {
            loop_epoll_handle(ctx, ev);
        }
        else {
            os_do_nothing(); // TODO: log it
        }
    }

    return 0;
}

// TODO: inotify on loop
LIB_PARAM(loop, 1);
static duk_ret_t
duke_loop(duk_context *ctx)
{
    int err = 0;

    loop_param_idx = duk_normalize_index(ctx, -1);

    err = loop_init(ctx);
    if (err<0) {
        __seterrno(ctx, err); goto error;
    }
    
    loop_save(ctx);
    loop_push_func(ctx);
    
    while(1) {
        err = loop_handle(ctx);
        if (err<0) {
            __seterrno(ctx, err); goto error;
        }
    }

error:
    loop_pop_func(ctx);
    loop_fini();

    return duk_push_int(ctx, err), 1;
}

#if duk_LIBCALL
static duk_ret_t
libcall(duk_context *ctx)
{
    int err = 0;
    duk_string_t lib = (duk_string_t)duk_require_string(ctx, 0);
    duk_string_t sym = (duk_string_t)duk_require_string(ctx, 1);
    int count = __get_array_length(ctx, 2);
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

LIB_PARAM(libcall, 4);
static duk_ret_t
duke_libcall(duk_context *ctx)
{
    return duk_push_int(ctx, libcall(ctx)), 1;
}
#endif

static void 
env_register(duk_context *ctx)
{
    extern char **environ;
    char *env = *environ;
    char *k, *v;
    int i;
    
    duk_push_object(ctx);
    for (i=1; env; i++) {
        k = strdup(env);
        v = strchr(k, '=');
        if (NULL!=v) {
            *v++ = 0;

            __set_obj_string(ctx, -1, k, v);
        }
        os_free(k);
        
        env = *(environ+i);
    }
    duk_put_prop_string(ctx, -2, "env");
}

static void 
arg_register(duk_context *ctx)
{
    int i, argc = __argc - 2;
    char **argv = __argv + 2;
    
    __set_obj_string(ctx, -1, "name", __argv[0]);
    __set_obj_string(ctx, -1, "script", __argv[1]);

    duk_push_array(ctx);
    for (i=0; i<argc; i++) {
        __set_array_string(ctx, -1, i, argv[i]);
    }
    duk_put_prop_string(ctx, -2, "argv");
}

static const dukc_func_entry_t my_func[] = {
#if 0
    LIB_FUNC(ak_getbyname),
    LIB_FUNC(ak_get),
    LIB_FUNC(ak_set),
    LIB_FUNC(ak_reload),
#endif
    LIB_FUNC(is_debug),
    LIB_FUNC(debug),
    LIB_FUNC(pipe),
    LIB_FUNC(shell),
    LIB_FUNC(readtxt),
    LIB_FUNC(readbin),
    LIB_FUNC(readline),
    LIB_FUNC(writefile),
    LIB_FUNC(appendfile),
    LIB_FUNC(loop),
#if duk_LIBCALL
    LIB_FUNC(libcall),
#endif

    LIB_FUNC_END
};

static const dukc_number_entry_t my_static_number[] = {
    LIB_VALUE_END
};

int my_register(duk_context *ctx)
{
    const dukc_number_entry_t my_dynamic_number[] = {
        __LIB_VALUE("SIZEOF_SHORT",     sizeof(short)),
        __LIB_VALUE("SIZEOF_INT",       sizeof(int)),
        __LIB_VALUE("SIZEOF_LONG",      sizeof(long)),
        __LIB_VALUE("SIZEOF_LONGLONG",  sizeof(long long)),
        __LIB_VALUE("SIZEOF_POINTER",   sizeof(void *)),

        __LIB_VALUE("BIG_ENDIAN",       !!(*(uint16 *)"\x12\x34"==0x1234)),
        __LIB_VALUE("LITTLE_ENDIAN",    !!(*(uint16 *)"\x12\x34"==0x3412)),

        LIB_VALUE_END
    };
    
    duk_push_global_object(ctx);
        duk_push_object(ctx);
            duk_put_functions(ctx, -1, my_func);
            duk_put_number_list(ctx, -1, my_static_number);
            duk_put_number_list(ctx, -1, my_dynamic_number);
            
            env_register(ctx);
            arg_register(ctx);
        duk_put_prop_string(ctx, -2, duk_MOD_MY);
    duk_pop(ctx);
    
    return 0;
}

