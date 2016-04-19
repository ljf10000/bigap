#ifndef __THIS_APP
#define __THIS_APP      js
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     main
#endif

#define __RUNAS_UNKNOW__

#include "utils.h"
#include "global.h"
#include "dukc.h"

OS_INITER;

duk_context *__ctx;
int __argc;
char **__argv;

static int
buildin_register(duk_context *ctx)
{
    return __load_code(ctx, "buildin", duk_global_CODE);
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
auto_register(duk_context *ctx)
{
    char path[1+OS_LINE_LEN] = {0};
    
    /*
    * try eval duk_PATH/auto/xxx.js
    */
    char *env = env_gets(ENV_duk_PATH, duk_PATH);
    os_snprintf(path, OS_LINE_LEN, "%s/" duk_auto_PATH, env);
    
    mv_t __handler(char *path, char *filename, os_fscan_line_handle_f *line_handle)
    {
        (void)line_handle;
        
        char file[1+OS_LINE_LEN] = {0};

        os_snprintf(file, OS_LINE_LEN, "%s/%s", path, filename);

        int err = __load_file(ctx, file);
        if (err<0) {
            return mv2_go(err);
        }
        
        return mv2_ok;
    }

    return os_fscan_dir(path, false, __filter, __handler, NULL);
}

static inline int
__register(duk_context *ctx)
{
    static int (*registers[])(duk_context *) = {
        global_register,
        duktape_register,
        my_register,
        libc_register,
        libz_register,
        libbz_register,
        liblz_register,
        libcurl_register,

        /*keep below last*/
        buildin_register,
        auto_register,
    };
    int i, err;

    for (i=0; i<os_count_of(registers); i++) {
        err = (*registers[i])(ctx);
        if (err<0) {
            return err;
        }
    }

    return 0;
}

static int
__main(int argc, char *argv[])
{
    duk_context *ctx = NULL;
    int err = 0;
    
    __argc = argc;
    __argv = argv;
    
    ctx = __ctx = duk_create_heap_default();
    if (NULL==ctx) {
        return -ENOMEM;
    }

    err = __register(ctx);
    if (err<0) {
        goto error;
    }
    
    duk_peval_file(ctx, argv[1]);
    duk_pop(ctx);

error:
    debug_js("before destroy duk heap");
    duk_destroy_heap(ctx);
    debug_js("after  destroy duk heap");

    return err;
}

#ifndef __BUSYBOX__
#define duk_main  main
#endif

int duk_main(int argc, char *argv[])
{
    return os_main(__main, argc, argv);
}

