#ifndef __THIS_APP
#define __THIS_APP      js
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     "main"
#endif

#include "utils.h"
#include "global.h"
#include "dukc.h"

OS_INITER;

duk_context *__ctx;
int __argc;
char **__argv;

static int
__buildin_eval(duk_context *ctx)
{
    return __ceval(ctx, "buildin", duk_global_CODE);
}

static bool 
__filter(char *path, char *filename)
{
    /*
    * skip NOT-js file
    */
    return false==os_str_is_end_by(filename, ".js");
}

static int
__auto_eval(duk_context *ctx)
{
    char path[1+OS_LINE_LEN] = {0};
    
    /*
    * try eval duk_PATH/auto/xxx.js
    */
    char *env = env_gets(ENV_duk_PATH, duk_PATH);
    os_snprintf(path, OS_LINE_LEN, "%s/auto", env);
    
    mv_t __handle(char *path, char *filename, os_fscan_line_handle_f *line_handle)
    {
        (void)line_handle;
        
        char file[1+OS_LINE_LEN] = {0};
    
        os_snprintf(file, OS_LINE_LEN, "%s/%s", path, filename);

        int err = __feval(ctx, file);
        if (err<0) {
            return mv2_go(err);
        }
        
        return mv2_ok;
    }

    return os_fscan_dir(path, false, __filter, __handle, NULL);
}

static inline int
__register(duk_context *ctx)
{
    static duk_register_f *array[] = {   \
        global_register,
        duktape_register,
        my_register,
        libc_register,
        libz_register,
        libcurl_register,

        /*keep below last*/
        __buildin_eval,
        __auto_eval,
    };
    int i, err;

    for (i=0; i<os_count_of(array); i++) {
        err = (*array[i](ctx);
        if (err<0) {
            return err;
        }
    }

    return 0;
}

static int
__main(int argc, char *argv[])
{
    __argc = argc;
    __argv = argv;
    
    duk_context *ctx = __ctx = duk_create_heap_default();
    if (NULL==ctx) {
        return -ENOMEM;
    }

    __register(ctx);
    
    duk_peval_file(ctx, argv[1]);
    duk_pop(ctx);
    
    debug_shell("before destroy duk heap");
    duk_destroy_heap(ctx);
    debug_shell("after  destroy duk heap");

    return 0;
}

#ifndef __BUSYBOX__
#define duk_main  main
#endif

int duk_main(int argc, char *argv[])
{
    return os_main(__main, argc, argv);
}

