#ifndef __THIS_APP
#define __THIS_APP      js
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     liblz
#endif

#define __RUNAS_UNKNOW__

#include "utils.h"
#include "dukc.h"

#if duk_LIBLZ
#include <lzlib.h>
#include "liblz.h"


#include "liblz/liblzf.c"
#include "liblz/liblzn.c"
#endif /* duk_LIBLZ */

int liblz_register(duk_context *ctx)
{
#if duk_LIBLZ
    duk_push_global_object(ctx);
        duk_push_object(ctx);
            liblzf_register(ctx, -1);
            liblzn_register(ctx, -1);
        duk_put_prop_string(ctx, -2, duk_MOD_LIBLZ);
    duk_pop(ctx);
#endif

    return 0;
}

