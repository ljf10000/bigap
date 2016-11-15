/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      nsqa
#endif
        
#ifndef __THIS_FILE
#define __THIS_FILE     script
#endif

#define __DEAMON__
#include "nsqa.h"
/******************************************************************************/
int
nsq_script_fini(nsq_script_t *script)
{
    os_free(script->content);
    os_free(script->argument);
    
    jobj_put(script->jscript);
    os_objzero(script);

    return 0;
}
    
static int 
file_md5_check_helper(char *filename, void *user)
{
    nsq_script_t *script = (nsq_script_t *)user;
    byte md5[OS_MD5_SIZE];
    
    md5_file(filename, md5);

    if (md5_eq(md5, script->md5bin)) {
        return 0;
    } else {
        char md5_string[1+OS_MD5_STRING_LEN];
        
        md5_bin2hex(md5_string, md5);
        
        debug_proto("file[%s] md5[%s] != script md5[%s]", 
            filename, 
            md5_string, 
            script->md5);
        
        return -EBADMD5;
    }
}

static int
file_md5_check(nsq_script_t *script)
{
    int err = 0;

    if (os_file_absolute(script->filename)) {
        if (os_file_exist(script->filename)) {
            err = file_md5_check_helper(script->filename, script);
        } else {
            debug_proto("no found absolute file:%s", script->filename);

            err = -ENOEXIST;
        }
    }
    else if (NSQ_SCRIPT_SCOPE_GLOBAL==script->scope) {
        err = os_fsearch_paths(script->filename, nsqa.env.cache, file_md5_check_helper, script);
        if (err<0 && err != -EBADMD5) {
            debug_proto("no found file:%s at %s", script->filename, nsqa.env.cache);
        }
    }
    else {
        nsq_instance_t *instance = container_of(script, nsq_instance_t, script);
        
        err = os_fsearch_paths(script->filename, instance->cache, file_md5_check_helper, script);
        if (err<0 && err != -EBADMD5) {
            debug_proto("no found file:%s at %s", script->filename, instance->cache);
        }
    }

    return err;
}

static int
content_md5_check(nsq_script_t *script)
{
    byte md5[OS_MD5_SIZE];

    md5_encode(md5, script->content, script->content_len);

    if (md5_eq(md5, script->md5bin)) {
        return 0;
    } else {
        char md5_string[1+OS_MD5_STRING_LEN];

        md5_bin2hex(md5_string, md5);
        
        debug_proto("content md5[%s] != script md5[%s]", md5_string, script->md5);
        
        return -EBADMD5;
    }
}

static int
script_md5_check(nsq_script_t *script)
{
    int err = 0, len;

    len = os_strlen(script->md5);
    if (OS_MD5_STRING_LEN != len) {
        debug_proto("invalid md5 length %d", len);
        
        return -EBADMD5;
    }
    md5_hex2bin(script->md5, script->md5bin);
    
    if (script->content) {
        err = content_md5_check(script);
    }
    else if (script->filename) {
        err = file_md5_check(script);
    }

    return err;
}

static int
script_init(nsq_script_t *script, jobj_t jscript)
{
    jobj_t jobj;
    int err = 0;

    err = nsq_script_jcheck(jscript);
    if (err<0) {
        return err;
    }

    jj_string_unsafe(script, jscript, filename);
    jj_string_unsafe(script, jscript, url);
    jj_string_unsafe(script, jscript, md5);
    jj_string_unsafe(script, jscript, content);
    jj_string_unsafe(script, jscript, argument);
    jj_string_unsafe(script, jscript, id);
    jj_string_unsafe(script, jscript, reply);
    
    jobj = jobj_get(jscript, "type");
    script->type = nsq_script_type_idx(jobj_get_string(jobj));
    if (false==is_good_nsq_script_type(script->type)) {
        return -EBADJSON;
    }
    
    jobj = jobj_get(jscript, "cache");
    script->cache = nsq_script_cache_idx(jobj_get_string(jobj));
    if (false==is_good_nsq_script_cache(script->cache)) {
        return -EBADJSON;
    }

    jobj = jobj_get(jscript, "scope");
    script->cache = nsq_script_scope_idx(jobj_get_string(jobj));
    if (false==is_good_nsq_script_scope(script->cache)) {
        return -EBADJSON;
    }

    jobj = jobj_get(jscript, "exec");
    script->cache = nsq_script_exec_idx(jobj_get_string(jobj));
    if (false==is_good_nsq_script_exec(script->cache)) {
        return -EBADJSON;
    }
    
    if (script->content) {
        size_t len;
        
        script->content = b64_decode_ex(script->content, os_strlen(script->content), &len);
        if (NULL==script->content) {
            return -EBASE64;
        }
        script->content_len = len;
    }

    if (script->argument) {
        script->argument = b64_decode_ex(script->argument, os_strlen(script->argument), NULL);
        if (NULL==script->argument) {
            return -EBASE64;
        }
    }
    
    if (script->filename) {
        if (os_file_relative(script->filename)) {
            debug_proto("not support relative file:%s", script->filename);
            
            return -ENOSUPPORT;
        }
    }
    
    if (script->md5) {
        err = script_md5_check(script);
        if (err<0) {
            return err;
        }
    }
    
    if (script->content &&
        NULL==script->filename &&
        NSQ_SCRIPT_CACHE_NONE != script->cache) {
        
        debug_proto("cache[%s] without filename", 
            nsq_script_cache_string(script->cache));
        
        return -EPROTO;
    }
    
    script->jscript = jscript;
    
    return 0;
}

int
nsq_script_init(nsq_script_t *script, char *json)
{
    jobj_t jscript = NULL;
    int err = 0;

    nsq_script_fini(script);
    
    jscript = jobj_byjson(json);
    if (NULL==jscript) {
        return -EBADJSON;
    }
    
    err = script_init(script, jscript);
    if (err<0) {
        nsq_script_fini(script);

        return err;
    }

    return 0;
}

static int
exec_file_shell(nsq_script_t *script)
{
    return 0;
}

static int
exec_file_embed(nsq_script_t *script)
{
    return 0;
}

static int
exec_file(nsq_script_t *script)
{
    if (NSQ_SCRIPT_EXEC_SHELL==script->exec) {
        return exec_file_shell(script);
    } else {
        return exec_file_embed(script);
    }
}

static int
exec_content_shell(nsq_script_t *script)
{
    return 0;
}

static int
exec_content_embed(nsq_script_t *script)
{
    return 0;
}

static int
exec_content(nsq_script_t *script)
{
    if (NSQ_SCRIPT_EXEC_SHELL==script->exec) {
        return exec_content_shell(script);
    } else {
        return exec_content_embed(script);
    }
}

static int
save_file(nsq_script_t *script)
{
    char filename[1+OS_LINE_LEN];
    char *file = filename;
    int err;
    
    if (os_file_absolute(script->filename)) {
        file = script->filename;
    }
    else if (NSQ_SCRIPT_SCOPE_GLOBAL==script->scope) {
        os_saprintf(filename, "%s/%s", nsqa.env.cache, script->filename);
    }
    else {
        nsq_instance_t *instance = container_of(script, nsq_instance_t, script);

        os_saprintf(filename, "%s/%s", instance->cache, script->filename);
    }

    return os_writefile(script->filename, script->content, script->content_len, false, false);
}

int
nsq_script_exec(nsq_script_t *script)
{
    int err;
    
    if (script->content) {
        err = exec_content(script);
        if (err<0) {
            return err;
        }
        
        if (script->filename) {
            save_file(script);
        }

        return 0;
    } else {
        return exec_file(script);
    }
}


/******************************************************************************/
