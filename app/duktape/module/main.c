/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      js
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     main
#endif

#include "utils.h"
#include "js/js.h"

OS_INITER;

STATIC int
js_main_helper(int argc, char *argv[])
{
    int err = 0;
    duk_context *ctx = NULL;
    char *cache = env_gets(ENV_JCACHE, js_CACHE);

    ctx = js_init("main", cache, argc, argv);
    if (NULL==ctx) {
        err = -ENOMEM; goto error;
    }

    err = js_eval(ctx, NULL);
    if (err<0) {
        goto error;
    }
    
error:
    js_fini(ctx);

    return err;
}

int allinone_main(int argc, char *argv[])
{
    return os_main(js_main_helper, argc, argv);
}

