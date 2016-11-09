/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      libjs
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     global
#endif

#define __RUNAS_UNKNOW__

#include "utils.h"
#include "js.h"

static const dukc_func_entry_t global_func[] = {
    JS_FUNC_END
};

int js_global_register(duk_context *ctx)
{
    duk_push_global_object(ctx);
	    duk_put_functions(ctx, -1, global_func);
	duk_pop(ctx);

	debug_ok("register global ok.");
	
    return 0;
}

