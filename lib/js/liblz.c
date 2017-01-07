/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      libjs
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     liblz
#endif

#ifndef __BUSYBOX__
#define __LIB__
#endif

#define __DEAMON__

#include "utils.h"
#include "js.h"

#if js_LIBLZ
#include <lzlib.h>
#include "liblz.h"


#include "liblz/liblzf.c"
#include "liblz/liblzn.c"

int js_liblz_register(duk_context *ctx)
{
    duk_push_global_object(ctx);
        duk_push_object(ctx);
            js_liblzf_register(ctx, -1);
            js_liblzn_register(ctx, -1);
        duk_put_prop_string(ctx, -2, js_MOD_LIBLZ);
    duk_pop(ctx);

    debug_ok("register liblz ok.");

    return 0;
}
#endif /* js_LIBLZ */

