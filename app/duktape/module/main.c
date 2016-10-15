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
#include "dukc.h"

OS_INITER;

duk_context *__js_ctx;
bool __js_shabang;
int __js_argc;
char **__js_argv;

static char *
readfd(duk_context *ctx, int fd) 
{
	FILE *f = NULL;
	char *buf = NULL;
	long sz = 0, len = 0, left, ret;  /* ANSI C typing */

	if (fd<0) {
		goto error;
	}
	
	f = os_fdopen(fd, "r");
	if (!f) {
		goto error;
	}

	while(!os_feof(f) && !os_ferror(f)) {
        if (0==sz) {
            sz += 4096;
            buf = (char *)os_malloc(sz);
        }
        else if (sz - len < 512) {
            sz += 4096;
            buf = (char *)os_realloc(buf, sz);
        }
        else {
            /*
            * use the buf, do nothing
            */
        }
        
        if (NULL==buf) {
            goto error;
        }

	    left = sz - len;
        ret = os_fread(f, buf + len, left);
        if (ret > left || ret < 0) {
            goto error;
        }
        
        len += ret;
	}
	buf[len++] = 0;

    if ('#'==buf[0] && '!'==buf[1]) {
        buf[0] = '/';
        buf[1] = '/';
    }

	return buf;
error:
    os_free(buf);
    os_fclose(f);

    return NULL;
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

    err = js_register(ctx);
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
        duk_peval_string(ctx, script); os_free(script);
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

