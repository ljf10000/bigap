/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#if js_LIBLZ
#include <lzlib.h>
#include "liblz.h"


#include "liblz/liblzf.c"
#include "liblz/liblzn.c"

int js_liblz_register(duk_context *ctx)
{
    duk_push_global_object(ctx);
        duk_push_object(ctx);
            liblzf_register(ctx, -1);
            liblzn_register(ctx, -1);
        duk_put_prop_string(ctx, -2, js_MOD_LIBLZ);
    duk_pop(ctx);

    debug_ok("register liblz ok.");

    return 0;
}
#endif /* js_LIBLZ */

