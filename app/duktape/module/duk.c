#ifndef __THIS_APP
#define __THIS_APP      js
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     "duk"
#endif

#include "utils.h"
#include "dukc.h"


LIB_PARAM(modSearch, DUK_VARARGS);
static duk_ret_t
duke_modSearch(duk_context * ctx)
{
    int ret = 1;
    char *path  = NULL;
    char *env   = NULL;

    char *id = (char *)duk_require_string(ctx, 0);

    char file[1+OS_LINE_LEN] = {0};

    /*
    * try file.js
    */
    os_snprintf(file, OS_LINE_LEN, "%s.js", id);
    if (os_file_exist(file)) {
        duk_push_string_file(ctx, file);
    
        ret = 1; goto error;
    }

    /*
    * try duk_PATH/file.js
    */
    env = strdup(env_gets(ENV_duk_PATH, duk_PATH));
    if (NULL==env) {
        ret = -1; goto error;
    }
    
    os_strtok_foreach(path, env, ":") {
        int len = os_strlen(path);

        debug_js("search %s at %s ...", file, path);
        /*
        * path last char is '/'
        */
        if (len>1 && '/'==path[len-1]) {
            os_snprintf(file, OS_LINE_LEN, "%s%s.js", path, id);
        } else {
            os_snprintf(file, OS_LINE_LEN, "%s/%s.js", path, id);
        }

        if (os_file_exist(file)) {
            duk_push_string_file(ctx, file);

            debug_js("search %s at %s OK.", file, path);
            ret = 1; goto error;
        }
    }

    /*
    * no found
    */
    ret = -1;
error:
    os_free(env);
    
    return ret;
}

static const dukc_func_entry_t duktape_func[] = {
    LIB_FUNC(modSearch),

    LIB_FUNC_END
};

int duktape_register(duk_context *ctx)
{
    duk_push_global_object(ctx);
    	duk_get_prop_string(ctx, -1, duk_DUKTAPE);
    	    duk_put_functions(ctx, -1, duktape_func);
    	duk_pop(ctx);
	duk_pop(ctx);
	
    return 0;
}

