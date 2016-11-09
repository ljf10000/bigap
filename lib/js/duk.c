/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      js
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     duk
#endif

#define __RUNAS_UNKNOW__

#include "utils.h"
#include "dukc.h"

static int
search_path(duk_context *ctx, char *file, char *path)
{
    char fullname[1+OS_LINE_LEN] = {0};

    if (NULL==path) {
        return -ENOEXIST;
    }
    
    debug_js("search %s at %s ...", file, path);
    
    /*
    * path last char is '/'
    */
    int len = os_strlen(path);
    if (len>1 && '/'==path[len-1]) {
        os_saprintf(fullname, "%s%s", path, file);
    } else {
        os_saprintf(fullname, "%s/%s", path, file);
    }

    if (os_file_exist(fullname)) {
        duk_push_string_file(ctx, fullname);

        debug_js("search %s at %s OK.", file, path);

        return 0;
    }

    return -ENOEXIST;
}

static int
search_env(duk_context *ctx, char *file, char *env, char *deft)
{
    char *ENV = os_strdup(env_gets(env, deft));
    char *path = NULL;
    int err = -ENOEXIST;
    
    os_strtok_foreach(path, ENV, ":") {
        if (0==search_path(ctx, file, path)) {
            err = 1; goto ok;
        }
    }
    
ok:
    os_free(ENV);
    
    return err;
}

static int
search_CURRENT(duk_context *ctx, char *file)
{
    return search_path(ctx, file, "./");
}

static int
search_JPATH(duk_context *ctx, char *file)
{
    return search_env(ctx, file, ENV_JPATH, js_PATH);
}

static int
search_JPRIV(duk_context *ctx, char *file)
{
    return search_path(ctx, file, js_priv(ctx)->cache);
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
    
    /*
    * try ./file.js
    * try __js_PATH/file.js
    * try __js_CACHE/file.js
    */
    if (0==search_CURRENT(ctx, file) ||
        0==search_JPATH(ctx, file) ||
        0==search_JPRIV(ctx, file)) {
        return 1;
    }

    return -1;
}

static const dukc_func_entry_t duktape_func[] = {
    JS_FUNC(modSearch),

    JS_FUNC_END
};

int js_duktape_register(duk_context *ctx)
{
    duk_push_global_object(ctx);
    	duk_get_prop_string(ctx, -1, js_DUKTAPE);
    	    duk_put_functions(ctx, -1, duktape_func);
    	duk_pop(ctx);
	duk_pop(ctx);

	debug_ok("register mod ok.");
	
    return 0;
}

