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

static int
eval_file(const char *file, void *user)
{
    duk_context *ctx = (duk_context *)user;

    duk_push_string_file(ctx, file);

    debug_js("eval %s OK.", file);

    return 0;
}

static int
search_env(duk_context *ctx, const char *file, char *env, char *deft)
{
    return os_fsearch_paths(file, env_gets(env, deft), eval_file, ctx);
}

static int
search_CURRENT(duk_context *ctx, const char *file)
{
    char current[1+OS_LINE_LEN] = {0};
    
    getcwd(current, OS_LINE_LEN);
    
    return os_fsearch_path(file, current, eval_file, ctx);
}

static int
search_JCACHE(duk_context *ctx, const char *file)
{
    return os_fsearch_paths(file, js_priv(ctx)->cache, eval_file, ctx);
}

static int
search_JPATH(duk_context *ctx, const char *file)
{
    return search_env(ctx, file, ENV_JPATH, js_PATH);
}

JS_PARAM(modSearch, DUK_VARARGS);
static duk_ret_t
duke_modSearch(duk_context *ctx)
{
    char *id = (char *)duk_require_string(ctx, 0);
    char *path;
    int err = -1;
    
    char file[1+OS_LINE_LEN] = {0};
    os_snprintf(file, OS_LINE_LEN, "%s.js", id);

    if (os_file_absolute(file)) {
        if (os_file_exist(file)) {
            return eval_file(file, ctx);
        } else {
            return -1;
        }
    }
    else if (os_file_relative(file)) {
        return -ENOSUPPORT;
    }
    
    /*
    * 1. try file.js
    * 2. try JCACHE/file.js
    * 3. try JPATH/file.js
    */
    if (0==search_CURRENT(ctx, file)||
        0==search_JCACHE(ctx, file) ||
        0==search_JPATH(ctx, file)) {
        return 1;
    }

    return -1;
}

static const dukc_func_entry_t duktape_func[] = {
    JS_FUNC(modSearch),

    JS_FUNC_END
};

static const dukc_func_entry_t global_func[] = {
    JS_FUNC_END
};

int js_global_register(duk_context *ctx)
{
    duk_push_global_object(ctx);
	    js_put_functions(ctx, -1, global_func);
	duk_pop(ctx);

	debug_ok("register global ok.");
	
	duk_push_global_object(ctx);
    	duk_get_prop_string(ctx, -1, js_DUKTAPE);
    	    js_put_functions(ctx, -1, duktape_func);
    	duk_pop(ctx);
	duk_pop(ctx);

	debug_ok("register mod search ok.");
	
    return 0;
}

