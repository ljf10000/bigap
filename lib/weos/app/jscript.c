/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
STATIC bool
__jscript_md5eqf(jscript_t *jsc, char *filename)
{
    byte md5[OS_MD5_SIZE];
    char md5string[1+OS_MD5_SIZE] = {0};

    md5_file(filename, md5);

    md5_bin2hex(md5string, md5);

    return os_streq(md5string, jsc->md5);
}

STATIC bool
__jscript_md5eqc(jscript_t *jsc, char *content)
{
    byte md5[OS_MD5_SIZE];
    char md5string[1+OS_MD5_SIZE] = {0};

    md5_encode(md5, content, os_strlen(content));

    md5_bin2hex(md5string, md5);

    return os_streq(md5string, jsc->md5);
}

STATIC int
__jscript_report(jscript_t *jsc, char *ack, jobj_t jreply)
{
    int err = 0;
    
    if (NULL==jsc->reply) {
        return 0;
    }

    jobj_t jobj = jobj_new_object();
    if (NULL==jobj) {
        return -ENOMEM;
    }
    time_t now = time(NULL);

    jobj_add_string(jobj, "mac",jsc->dev.basemac);
    jobj_add_string(jobj, "recvtime", os_fulltime_string(jsc->recvtime));
    jobj_add_string(jobj, "exectime", os_fulltime_string(now));
    jobj_add_string(jobj, "id", jsc->id);
    jobj_add_string(jobj, "ack", ack);
    jobj_add_u64(jobj, "seq", jsc->seq);

    jobj_add(jobj, "reply", jreply);

    err = os_system("%s '%s'", jsc->reply, jobj_json(jobj));
    jobj_put(jobj);

    return err;
}

STATIC jobj_t 
__jscript_jreply(int error, char *outstring, char *errstring)
{
    jobj_t jobj = jobj_new_object();
    if (NULL==jobj) {
        return NULL;
    }

    jobj_add_int(jobj, "errno", error);
    if (outstring) {
        jobj_add_string(jobj, "stdout", outstring);
    }
    if (errstring) {
        jobj_add_string(jobj, "stderr", errstring);
    }

    return jobj;
}

STATIC int
__jscript_verror(jscript_t *jsc, int error, const char *fmt, va_list args)
{
    char *errstring = NULL;

    int err = os_vasprintf(&errstring, fmt, args);
    if (err>0) {
        __jscript_report(jsc, "error", __jscript_jreply(error, NULL, errstring));
    }
    os_free(errstring);

    return error;
}

STATIC int
__jscript_error(jscript_t *jsc, int error, const char *fmt, ...)
{
    va_list args;
    
    va_start(args, (char *)fmt);
    __jscript_verror(jsc, error, fmt, args);
    va_end(args);
    
    return error;
}

STATIC int
__jscript_ok(jscript_t *jsc)
{
    return __jscript_error(jsc, 0, NULL);
}

STATIC int
__jscript_exec(jscript_t *jsc, char *json)
{
    int err = 0;
    
    time_t now = time(NULL);

    int cb(int error, char *outstring, char *errstring)
    {
        __jscript_report(jsc, "reply", __jscript_jreply(error, outstring, errstring));
    
        return 0;
    }

    os_println("__jscript_exec 1");
    
    if (0==jsc->period || now < (jsc->sendtime + jsc->period)) {
        os_println("__jscript_exec 1.1");
        if (jsc->dev.slot == jsc->slot) {
            // ipc
            os_println("__jscript_exec 1.1.1");
            err = os_pexec_json(json, cb);
            os_println("__jscript_exec 1.1.2");
            if (err<0) {
                os_println("__jscript_exec 1.1.2.1");
                __jscript_error(jsc, err, "jexec error:%s", json);
                os_println("__jscript_exec 1.1.2.2");
            }
            os_println("__jscript_exec 1.1.3");
        } else {
            // rpc
            os_println("__jscript_exec 1.2.1");
            err = os_system(JSCRIPT_REMOTE " %d '%s'", jsc->slot, json);
            os_println("__jscript_exec 1.2.2");
            __jscript_error(jsc, err, 0==err?"jremote ok":"jremote error");
            os_println("__jscript_exec 1.2.3");
        }
        os_println("__jscript_exec 1.3");
    }

    os_println("__jscript_exec 2");
    
    return err;
}

STATIC char * // unsafe
__jscript_dir(jscript_t *jsc)
{
    static char dir[1+OS_LINE_LEN];

    switch(jsc->scope) {
        case JSCRIPT_SCOPE_INSTANCE:
            os_saprintf(dir, "%s/%s", jsc->instance.cache, jsc->instance.name);
            
            break;
        case JSCRIPT_SCOPE_GLOBAL:
        default:
            os_saprintf(dir, "%s", jsc->instance.cache);
            
            break;
    }
    
    return dir;
}

STATIC char *
__jscript_filename(jscript_t *jsc)
{
    static char filename[1+OS_LINE_LEN];

    if ('/'==jsc->filename[0]) {
        os_saprintf(filename, "%s", jsc->filename);
    } else {
        os_saprintf(filename, "%s/%s", __jscript_dir(jsc), jsc->filename);
    }
    
    return filename;
}

STATIC int
__jscript_save_content(jscript_t *jsc)
{
    int err = 0;
    char *content = b64_decode((byte *)jsc->content, os_strlen(jsc->content));
    if (NULL==content) {
        return -ENOMEM;
    }
    
    if (NULL==jsc->md5 || false==__jscript_md5eqc(jsc, content)) {
        err = os_writefile(__jscript_filename(jsc), content, os_strlen(content), false, false);
    }

    os_free(content);
    
    return __jscript_ok(jsc);
}

STATIC int
__jscript_download(jscript_t *jsc, char *file)
{
    if (NULL==jsc->url) {
        return __jscript_error(jsc, 1, "file not exist and no url");
    } else {
        int err = os_system("curl -o %s %s &> /dev/null", file, jsc->url);

        return __jscript_error(jsc, err, "download error: curl -o %s %s &> /dev/null", file, jsc->url);
    }
}

STATIC int
__jscript_save_file(jscript_t *jsc)
{
    int err;
    
    os_println("__jscript_save_file 1");
    if (NULL==jsc->filename) {
        /*
        * no filename, needn't save
        */
        os_println("__jscript_save_file 1.1");
        return __jscript_ok(jsc);
    }
    else if (JSCRIPT_CACHE_NONE==jsc->cache) {
        /*
        * no cache, needn't save
        */
        os_println("__jscript_save_file 1.2");
        return __jscript_ok(jsc);
    }
    else if (jsc->content) {
        os_println("__jscript_save_file 1.3");
        return __jscript_save_content(jsc);
    }

    os_println("__jscript_save_file 2");
    
    char *file = __jscript_filename(jsc);
    os_println("__jscript_save_file 3");
    if (os_file_exist(file) && __jscript_md5eqf(jsc, file)) {
        os_println("__jscript_save_file 3.1.1");
        err = __jscript_ok(jsc);
        os_println("__jscript_save_file 3.1.2");
    } else {
        os_println("__jscript_save_file 3.2.1");
        err = __jscript_download(jsc, file);
        os_println("__jscript_save_file 3.2.2");
    }

    return err;
}

STATIC int
__jscript_save_json(jscript_t *jsc, char *json)
{
    char file[1+OS_LINE_LEN];
    int err;

    // todo: inotify the dir and save file to flash
    os_println("__jscript_save_json 1");
    os_saprintf(file, "/tmp/startup/next/%s.%llu.json",
        jsc->instance.name,
        jsc->seq);

    os_println("__jscript_save_json 2");

    err = os_writefile(file, json, os_strlen(json), false, false);
    os_println("__jscript_save_json 3");
}

STATIC int
__jscript_handle(jscript_t *jsc, char *json)
{
    int err;
    
    os_println("__jscript_handle 1");
    __jscript_save_file(jsc);
    os_println("__jscript_handle 2");
    if (jsc->dev.is_startup || JSCRIPT_RUN_THIS==jsc->run) {
        os_println("__jscript_handle 2.1.1");
        err = __jscript_exec(jsc, json);
        os_println("__jscript_handle 2.1.2");
    } else {
        os_println("__jscript_handle 2.2.1");
        err = __jscript_save_json(jsc, json);
        os_println("__jscript_handle 2.2.2");
    }
}

int
jscript_exec(char *json)
{
    int err;
    jscript_t jscript = {
        .dev = {
            .slot = env_geti("SLOT", 0),
            .basemac = os_getmacby(SCRIPT_GETBASEMAC),
        },
    };
    jscript_t *jsc = &jscript;
    
    jobj_t jobj = jobj_byjson(json);
    if (NULL==jobj) {
        return -EBADJSON;
    }
    
    /*
    * use buildin script
    */
    jobj_t jval = jobj_get(jobj, "script");
    if (jval) {
        if (jtype_string != jobj_type(jval)) {
            return __jscript_error(jsc, -EBADJSON, "bad json:script not string");
        }
        
        char *script = jobj_get_string_ex(jval);
        if (script && os_file_exist(script)) {
            return os_system("%s '%s'", script, json);
        }
    }
    
    /*
    * append recvtime
    */
    if (NULL==jobj_get(jobj, "recvtime")) {
        jsc->recvtime = time(NULL);
        
        char *now = os_fulltime_string(jsc->recvtime);
        jobj_add(jobj, "recvtime", jobj_new_string(now));
    } else {
        jsc->dev.is_startup = true;
    }
    
    err = jrule_j2o(jscript_jrules(), jsc, jobj);
    if (err<0) {
        return __jscript_error(jsc, err, "bad json:%s", jobj_json(jobj));
    }
    else if (NULL==jsc->filename && NULL==jsc->content) {
        return __jscript_error(jsc, 1, "no filename and content");
    }
    
    return __jscript_handle(jsc, jobj_json(jobj));
}

/******************************************************************************/
