#ifndef __THIS_APP
#define __THIS_APP      js
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     "main"
#endif

#include "utils.h"
#include "dukc.h"

OS_INITER;

duk_context *__ctx;
int __argc;
char **__argv;

static int
__eval(duk_context *ctx, char *filename)
{
    int err = 0;
    uint32_t size = 0;

    debug_js("eval %s ...", filename);
    if (os_file_exist(filename)) {
        char *buf = __readfileall(filename, &size, false);
        if (buf) {
            duk_eval_lstring_noresult(ctx, buf, size);
            debug_js("eval %s OK.", filename);
            
            os_free(buf);
        } else {
            err = -EIO;
        }
    }
    
    return err;
}

static int
__auto_eval(duk_context * ctx)
{
    int err = 0;
    char path[1+OS_LINE_LEN] = {0};
    
    /*
    * try eval duk_PATH/auto/xxx.js
    */
    char *env = env_gets(ENV_duk_PATH, duk_PATH);
    os_snprintf(path, OS_LINE_LEN, "%s/auto", env);

    bool filter(char *path, char *filename)
    {
        int len = os_strlen(filename);

        /*
        * filename as xxx.js
        */
        return  '.'==filename[len-3] &&
                'j'==filename[len-2] &&
                's'==filename[len-1];
    }

    mv_t handle(char *path, char *filename, os_fscan_line_handle_f *line_handle)
    {
        char file[1+OS_LINE_LEN] = {0};

        os_snprintf(file, OS_LINE_LEN, "%s/%s", path, filename);

        __eval(ctx, file);
    }
    
    os_fscan_dir(path, true, filter, handle, NULL);
    
    return err;
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
    
    global_register(ctx);
    duktape_register(ctx);
    my_register(ctx);
    libc_register(ctx);
    libcurl_register(ctx);

    __eval(ctx, duk_code_pre("global"));
    __eval(ctx, duk_code_pre("duk"));
    __eval(ctx, duk_code_pre("my"));
    __eval(ctx, duk_code_pre("libc"));
    __eval(ctx, duk_code_pre("libcurl"));
    
    __auto_eval(ctx);
    
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

