/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
jscript_control_t *
__this_jscript(void)
{
    static jscript_control_t jscript;

    if (NULL==jscript.mac) {
        jscript.slot  = env_geti("SLOT", 0);
        jscript.mac   = os_getmacby(SCRIPT_GETBASEMAC);
    }
    
    return &jscript;
}

STATIC bool
__jscript_md5eqf(char *filename)
{
    byte md5[OS_MD5_SIZE];
    char md5string[1+OS_MD5_SIZE] = {0};

    md5_file(filename, md5);

    md5_bin2hex(md5string, md5);

    return os_streq(md5string, __this_jscript()->J.md5);
}

STATIC bool
__jscript_md5eqc(char *content)
{
    byte md5[OS_MD5_SIZE];
    char md5string[1+OS_MD5_SIZE] = {0};

    md5_encode(md5, content, os_strlen(content));

    md5_bin2hex(md5string, md5);

    return os_streq(md5string, __this_jscript()->J.md5);
}

STATIC int
__jscript_report(char *ack, jobj_t jreply)
{
    int err = 0;
    
    if (NULL==__this_jscript()->J.reply) {
        return 0;
    }

    jobj_t jobj = jobj_new_object();
    if (NULL==jobj) {
        return -ENOMEM;
    }
    time_t now = time(NULL);

    jobj_add_string(jobj, "mac", __this_jscript()->mac);
    jobj_add_string(jobj, "recvtime", os_fulltime_string(__this_jscript()->J.recvtime));
    jobj_add_string(jobj, "exectime", os_fulltime_string(now));
    jobj_add_string(jobj, "id", __this_jscript()->J.id);
    jobj_add_string(jobj, "ack", ack);
    jobj_add_u64(jobj, "seq", __this_jscript()->J.seq);

    jobj_add(jobj, "reply", jreply);

    err = os_system("%s '%s'", __this_jscript()->J.reply, jobj_json(jobj));
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
__jscript_error(int error, char *errstring)
{
    __jscript_report("error", __jscript_jreply(error, NULL, errstring));

    return error;
}

STATIC int 
__jscript_callback(int error, char *outstring, char *errstring)
{
    __jscript_report("reply", __jscript_jreply(error, outstring, errstring));

    return 0;
}

STATIC int
__jscript_exec(void)
{
    int err;
    
    time_t now = time(NULL);

    if (0==__this_jscript()->J.period || now < (__this_jscript()->J.sendtime + __this_jscript()->J.period)) {
        char *json = jobj_json(__this_jscript()->jobj);

        if (__this_jscript()->slot == __this_jscript()->J.slot) {
            // ipc
            err = os_pexec_json(json, __jscript_callback);
            if (err<0) {
                return __jscript_error(err, "jexec error");
            }
        } else {
            // rpc
            err = os_system(JSCRIPT_REMOTE " %d '%s'", __this_jscript()->slot, jobj_json(__this_jscript()->jobj));

            return __jscript_error(err, 0==err?"jremote ok":"jremote error");
        }
    }

    return 0;
}

STATIC char *
__jscript_dir(void)
{
    if (false==is_good_str(__this_jscript()->dir)) {
        switch(__this_jscript()->J.scope) {
            case JSCRIPT_SCOPE_INSTANCE:
                os_saprintf(__this_jscript()->dir, "%s/%s", __this_jscript()->J.instance.cache, __this_jscript()->J.instance.name);
                
                break;
            case JSCRIPT_SCOPE_GLOBAL:
            default:
                os_saprintf(__this_jscript()->dir, "%s", __this_jscript()->J.instance.cache);
                
                break;
        }
    }
    
    return __this_jscript()->dir;
}

STATIC char *
__jscript_filename(void)
{
    if (false==is_good_str(__this_jscript()->filename)) {
        if ('/'==__this_jscript()->J.filename[0]) {
            os_saprintf(__this_jscript()->filename, "%s", __this_jscript()->J.filename);
        } else {
            os_saprintf(__this_jscript()->filename, "%s/%s", __jscript_dir(), __this_jscript()->J.filename);
        }
    }

    return __this_jscript()->filename;
}

STATIC int
__jscript_save_content(void)
{
    int err = 0;
    char *content = b64_decode((byte *)__this_jscript()->J.content, os_strlen(__this_jscript()->J.content));
    if (NULL==content) {
        return -ENOMEM;
    }
    
    if (NULL==__this_jscript()->J.md5 || false==__jscript_md5eqc(content)) {
        err = os_writefile(__jscript_filename(), content, os_strlen(content), false, false);
    }

    os_free(content);
    
    return __jscript_error(0, NULL);
}

STATIC int
__jscript_download(char *file)
{
    if (NULL==__this_jscript()->J.url) {
        return __jscript_error(1, "file not exist and no url");
    } else {
        int err = os_system("curl -o %s %s &> /dev/null", file, __this_jscript()->J.url);

        return __jscript_error(err, "download");
    }
}

STATIC int
__jscript_save_file(void)
{
    if (NULL==__this_jscript()->J.filename) {
        /*
        * no filename, needn't save
        */
        return __jscript_error(0, NULL);
    }
    else if (JSCRIPT_CACHE_NONE==__this_jscript()->J.cache) {
        /*
        * no cache, needn't save
        */
        return __jscript_error(0, NULL);
    }
    else if (__this_jscript()->J.content) {
        return __jscript_save_content();
    }

    char *file = __jscript_filename();
    if (false==os_file_exist(file)) {
        return __jscript_download(file);
    }

    if (__jscript_md5eqf(file)) {
        return __jscript_error(0, NULL);
    } else {
        return __jscript_download(file);
    }
}

STATIC int
__jscript_save_json(void)
{
    char file[1+OS_LINE_LEN];
    char *json = jobj_json(__this_jscript()->jobj);
    
    os_saprintf(file, "/tmp/startup/next/%s.%llu.json",
        __this_jscript()->J.instance.name,
        __this_jscript()->J.seq);

    return os_writefile(file, json, os_strlen(json), false, false);
}

STATIC int
__jscript_handle(void)
{
    __jscript_save_file();

    if (JSCRIPT_RUN_NEXT==__this_jscript()->J.run) {
        return __jscript_save_json();
    } else {
        return __jscript_exec();
    }
}

STATIC int
__jscript_check(void)
{
    if (NULL==__this_jscript()->J.filename && NULL==__this_jscript()->J.content) {
        return __jscript_error(1, "no filename and content");
    }
    
    return 0;
}

int
jscript_exec(char *json)
{
    int err;
    
    __this_jscript()->jobj = jobj_byjson(json);
    if (NULL==__this_jscript()->jobj) {
        return -EBADJSON;
    }

    /*
    * use buildin script
    */
    jobj_t jval = jobj_get(__this_jscript()->jobj, "script");
    if (jval) {
        if (jtype_string != jobj_type(jval)) {
            return __jscript_error(-EBADJSON, "bad json");
        }
        
        char *script = jobj_get_string(jval);
        if (os_strneq("/bin/" __THIS_APPNAME, script)) {
            return os_system("%s '%s'", script, json);
        }
    }
    
    /*
    * append recvtime
    */
    if (NULL==jobj_get(__this_jscript()->jobj, "recvtime")) {
        __this_jscript()->J.recvtime = time(NULL);
        
        char *now = os_fulltime_string(__this_jscript()->J.recvtime);
        jobj_add(__this_jscript()->jobj, "recvtime", jobj_new_string(now));
    } else {
        __this_jscript()->is_startup = true;
    }
    
    err = jrule_j2o(jscript_jrules(), &__this_jscript()->J, __this_jscript()->jobj);
    if (err<0) {
        return __jscript_error(err, "bad json");
    }
    
    err = __jscript_check();
    if (err<0) {
        return err;
    }
    
    return __jscript_handle();
}

/******************************************************************************/
