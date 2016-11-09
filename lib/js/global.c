/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      js
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     global
#endif

#define __RUNAS_UNKNOW__

#include "utils.h"
#include "js.h"

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
search_paths(duk_context *ctx, char *file, char *paths)
{
    char *path = NULL;

    os_strtok_foreach(path, paths, ":") {
        if (0==search_path(ctx, file, path)) {
            return 0;
        }
    }

    return -ENOEXIST;
}

static int
search_env(duk_context *ctx, char *file, char *env, char *deft)
{
    char *paths = os_strdup(env_gets(env, deft));
    int err = search_paths(ctx, file, paths);
    os_free(paths);

    return err;
}

static int
search_CURRENT(duk_context *ctx, char *file)
{
    return search_path(ctx, file, "./");
}

static int
search_JCACHE(duk_context *ctx, char *file)
{
    return search_paths(ctx, file, js_priv(ctx)->cache);
}

static int
search_JPATH(duk_context *ctx, char *file)
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
    
    /*
    * 1. try file.js
    * 2. try JCACHE/file.js
    * 3. try JPATH/file.js
    */
    if (0==search_CURRENT(ctx, file) ||
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
	    duk_put_functions(ctx, -1, global_func);
	duk_pop(ctx);

	debug_ok("register global ok.");
	
	duk_push_global_object(ctx);
    	duk_get_prop_string(ctx, -1, js_DUKTAPE);
    	    duk_put_functions(ctx, -1, duktape_func);
    	duk_pop(ctx);
	duk_pop(ctx);

	debug_ok("register mod search ok.");
	
    return 0;
}

