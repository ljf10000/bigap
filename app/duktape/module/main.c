/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
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

duk_context *__js_ctx;
bool __js_shabang;
int __js_argc;
char **__js_argv;

static int
buildin_register(duk_context *ctx)
{
    int err = 0;
    
    err = __load_code(ctx, "buildin", duk_global_CODE);

    debug_ok_error(err, "register buildin");

    return err;
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
    int err = 0;
    
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

    err = os_fscan_dir(path, false, __filter, __handler, NULL);

    debug_ok_error(err, "register auto");
    
    return err;
}

static inline int
__register(duk_context *ctx)
{
    static int (*registers[])(duk_context *) = {
        global_register,
        duktape_register,
        my_register,
        libc_register,
#if duk_LIBZ
        libz_register,
#endif
#if duk_LIBBZ
        libbz_register,
#endif
#if duk_LIBLZ
        liblz_register,
#endif
#if duk_LIBCURL
        libcurl_register,
#endif
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

static char *
readfd(duk_context *ctx, int fd) 
{
	duk_file *f = NULL;
	char *buf = NULL;
	long sz = 0, len = 0, left, ret;  /* ANSI C typing */

	if (fd<0) {
		return NULL;
	}
	f = fdopen(fd, "r");
	if (!f) {
		return NULL;
	}

	while(!feof(f) && !ferror(f)) {
        if (0==sz) {
            sz += 4096;
            buf = (char *)malloc(sz);
        }
        else if (sz - len < 512) {
            sz += 4096;
            buf = (char *)realloc(buf, sz);
        }
        else {
            /*
            * use the buf, do nothing
            */
        }
        
        if (NULL==buf) {
            return NULL;
        }

	    left = sz - len;
        ret = fread(buf + len, 1, left, f);
        if (ret > left || ret < 0) {
            return NULL;
        }
        
        len += ret;
	}
	buf[len++] = 0;
	
    if ('#'==buf[0] && '!'==buf[1]) {
        buf[0] = '/';
        buf[1] = '/';
    }

	return buf;
}

static int
__main(int argc, char *argv[])
{
    duk_context *ctx = NULL;
    char *script = NULL;
    int err = 0;
    
    __js_argc = argc;
    __js_argv = argv;
    __js_shabang = (__js_argc>1);

    ctx = __js_ctx = duk_create_heap_default();
    if (NULL==ctx) {
        return -ENOMEM;
    }

    err = __register(ctx);
    if (err<0) {
        goto error;
    }

    if (__js_shabang) {
        script = argv[1];
        /*
        * argv[1] is script name
        */
        duk_peval_file(ctx, script);
    } else {
        script = readfd(ctx, 0);

        /*
        * cat SCRIPT  | js
        * echo SCRIPT | js
        */
        duk_peval_string(ctx, script);
    }

    duk_pop(ctx);
error:
    debug_js("before destroy duk heap");
    duk_destroy_heap(ctx);
    debug_js("after  destroy duk heap");

    return err;
}

int allinone_main(int argc, char *argv[])
{
    return os_main(__main, argc, argv);
}

