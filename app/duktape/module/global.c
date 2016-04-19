#ifndef __THIS_APP
#define __THIS_APP      js
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     global
#endif

#define __RUNAS_UNKNOW__

#include "utils.h"
#include "dukc.h"

static const dukc_func_entry_t global_func[] = {
    LIB_FUNC_END
};

int global_register(duk_context *ctx)
{
    duk_push_global_object(ctx);
	    duk_put_functions(ctx, -1, global_func);
	duk_pop(ctx);
	
    return 0;
}

