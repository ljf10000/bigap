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

static int
__main(int argc, char *argv[])
{
    int err = 0;
    duk_context *ctx = NULL;
    
    ctx = js_init("master", argc, argv);
    if (NULL==ctx) {
        err = -ENOMEM; goto error;
    }

    err = duk_script(ctx, NULL);
    if (err<0) {
        goto error;
    }
    
error:
    js_fini(ctx);

    return err;
}

int allinone_main(int argc, char *argv[])
{
    return os_main(__main, argc, argv);
}

