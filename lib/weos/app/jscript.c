/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
STATIC bool
__jscript_md5eqf(jscript_t *jscript, char *filename)
{
    byte md5[OS_MD5_SIZE];
    char md5string[1+OS_MD5_SIZE] = {0};

    md5_file(filename, md5);

    md5_bin2hex(md5string, md5);

    return os_streq(md5string, jscript->md5);
}

STATIC bool
__jscript_md5eqc(jscript_t *jscript, char *content)
{
    byte md5[OS_MD5_SIZE];
    char md5string[1+OS_MD5_SIZE] = {0};

    md5_encode(md5, content, os_strlen(content));

    md5_bin2hex(md5string, md5);

    return os_streq(md5string, jscript->md5);
}

STATIC int
__jscript_report(jscript_t *jscript, char *ack, jobj_t jreply)
{
    int err = 0;
    
    if (NULL==jscript->reply) {
        return 0;
    }

    jobj_t jobj = jobj_new_object();
    if (NULL==jobj) {
        return -ENOMEM;
    }
    time_t now = time(NULL);

    jobj_add_string(jobj, "mac",jscript->dev.basemac);
    jobj_add_string(jobj, "recvtime", os_fulltime_string(jscript->recvtime));
    jobj_add_string(jobj, "exectime", os_fulltime_string(now));
    jobj_add_string(jobj, "id", jscript->id);
    jobj_add_string(jobj, "ack", ack);
    jobj_add_u64(jobj, "seq", jscript->seq);

    jobj_add(jobj, "reply", jreply);

    err = os_system("%s '%s'", jscript->reply, jobj_json(jobj));
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
__jscript_error(jscript_t *jscript, int error, char *errstring)
{
    __jscript_report(jscript, "error", __jscript_jreply(error, NULL, errstring));

    return error;
}

STATIC int
__jscript_exec(jscript_t *jscript, char *json)
{
    int err = 0;
    
    time_t now = time(NULL);

    int cb(int error, char *outstring, char *errstring)
    {
        __jscript_report(jscript, "reply", __jscript_jreply(error, outstring, errstring));
    
        return 0;
    }

    if (0==jscript->period || now < (jscript->sendtime + jscript->period)) {
        if (jscript->dev.slot == jscript->slot) {
            // ipc
            err = os_pexec_json(json, cb);
            if (err<0) {
                __jscript_error(jscript, err, "jexec error");
            }
        } else {
            // rpc
            err = os_system(JSCRIPT_REMOTE " %d '%s'", jscript->slot, json);

            __jscript_error(jscript, err, 0==err?"jremote ok":"jremote error");
        }
    }

    return err;
}

STATIC char * // unsafe
__jscript_dir(jscript_t *jscript)
{
    static char dir[1+OS_LINE_LEN];

    switch(jscript->scope) {
        case JSCRIPT_SCOPE_INSTANCE:
            os_saprintf(dir, "%s/%s", jscript->instance.cache, jscript->instance.name);
            
            break;
        case JSCRIPT_SCOPE_GLOBAL:
        default:
            os_saprintf(dir, "%s", jscript->instance.cache);
            
            break;
    }
    
    return dir;
}

STATIC char *
__jscript_filename(jscript_t *jscript)
{
    static char filename[1+OS_LINE_LEN];
    
    if ('/'==jscript->filename[0]) {
        os_saprintf(filename, "%s", jscript->filename);
    } else {
        os_saprintf(filename, "%s/%s", __jscript_dir(jscript), jscript->filename);
    }

    return filename;
}

STATIC int
__jscript_save_content(jscript_t *jscript)
{
    int err = 0;
    char *content = b64_decode((byte *)jscript->content, os_strlen(jscript->content));
    if (NULL==content) {
        return -ENOMEM;
    }
    
    if (NULL==jscript->md5 || false==__jscript_md5eqc(jscript, content)) {
        err = os_writefile(__jscript_filename(jscript), content, os_strlen(content), false, false);
    }

    os_free(content);
    
    return __jscript_error(jscript, 0, NULL);
}

STATIC int
__jscript_download(jscript_t *jscript, char *file)
{
    if (NULL==jscript->url) {
        return __jscript_error(jscript, 1, "file not exist and no url");
    } else {
        int err = os_system("curl -o %s %s &> /dev/null", file, jscript->url);

        return __jscript_error(jscript, err, "download");
    }
}

STATIC int
__jscript_save_file(jscript_t *jscript)
{
    if (NULL==jscript->filename) {
        /*
        * no filename, needn't save
        */
        return __jscript_error(jscript, 0, NULL);
    }
    else if (JSCRIPT_CACHE_NONE==jscript->cache) {
        /*
        * no cache, needn't save
        */
        return __jscript_error(jscript, 0, NULL);
    }
    else if (jscript->content) {
        return __jscript_save_content(jscript);
    }

    char *file = __jscript_filename(jscript);
    if (false==os_file_exist(file)) {
        return __jscript_download(jscript, file);
    }

    if (__jscript_md5eqf(jscript, file)) {
        return __jscript_error(jscript, 0, NULL);
    } else {
        return __jscript_download(jscript, file);
    }
}

STATIC int
__jscript_save_json(jscript_t *jscript, char *json)
{
    char file[1+OS_LINE_LEN];
    
    os_saprintf(file, "/tmp/startup/next/%s.%llu.json",
        jscript->instance.name,
        jscript->seq);

    return os_writefile(file, json, os_strlen(json), false, false);
}

STATIC int
__jscript_handle(jscript_t *jscript, char *json)
{
    __jscript_save_file(jscript);

    if (JSCRIPT_RUN_NEXT==jscript->run) {
        return __jscript_save_json(jscript, json);
    } else {
        return __jscript_exec(jscript, json);
    }
}

STATIC int
__jscript_check(jscript_t *jscript)
{
    if (NULL==jscript->filename && jscript->content) {
        return __jscript_error(jscript, 1, "no filename and content");
    }
    
    return 0;
}

int
jscript_exec(char *json)
{
    int err;
    jscript_t J = {
        .dev = {
            .slot = env_geti("SLOT", 0),
            .basemac = os_getmacby(SCRIPT_GETBASEMAC),
        },
    };
    jscript_t *jscript = &J;
    
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
            return __jscript_error(jscript, -EBADJSON, "bad json");
        }
        
        char *script = jobj_get_string(jval);
        if (os_strneq("/bin/" __THIS_APPNAME, script)) {
            return os_system("%s '%s'", script, json);
        }
    }
    
    /*
    * append recvtime
    */
    if (NULL==jobj_get(jobj, "recvtime")) {
        jscript->recvtime = time(NULL);
        
        char *now = os_fulltime_string(jscript.recvtime);
        jobj_add(jobj, "recvtime", jobj_new_string(now));
    } else {
        jscript->dev.is_startup = true;
    }
    
    err = jrule_j2o(jscript_jrules(), jscript, jobj);
    if (err<0) {
        return __jscript_error(jscript, err, "bad json");
    }
    
    err = __jscript_check(jscript);
    if (err<0) {
        return err;
    }
    
    return __jscript_handle(jscript, jobj_json(jobj));
}

/******************************************************************************/
