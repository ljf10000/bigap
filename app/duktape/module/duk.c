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


JS_PARAM(modSearch, DUK_VARARGS);
static duk_ret_t
duke_modSearch(duk_context *ctx)
{
    char *id = (char *)duk_require_string(ctx, 0);
    int err = -1;
    
    /*
    * try file.js
    */
    char file[1+OS_LINE_LEN] = {0};
    os_snprintf(file, OS_LINE_LEN, "%s.js", id);
    if (os_file_exist(file)) {
        duk_push_string_file(ctx, file);

        return 1;
    }

    /*
    * try js_PATH/file.js
    */
    char *path = NULL;
    char *env = os_strdup(env_gets(ENV_JPATH, js_PATH));
    
    os_strtok_foreach(path, env, ":") {
        int len = os_strlen(path);

        debug_js("search %s at %s ...", file, path);
        /*
        * path last char is '/'
        */
        if (len>1 && '/'==path[len-1]) {
            os_saprintf(file, "%s%s.js", path, id);
        } else {
            os_saprintf(file, "%s/%s.js", path, id);
        }

        if (os_file_exist(file)) {
            duk_push_string_file(ctx, file);

            debug_js("search %s at %s OK.", file, path);

            err = 1; goto error;
        }
    }

error:
    os_free(env);
    
    return err;
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

