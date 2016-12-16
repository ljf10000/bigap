/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      jscript
#endif

#include "utils.h"

OS_INITER;
/******************************************************************************/
#ifndef JSCRIPT_BUILDIN
#define JSCRIPT_BUILDIN     1
#endif

#ifndef JSCRIPT_REMOTE
#define JSCRIPT_REMOTE      PC_FILE("tmp/script", "jremote.script")
#endif

#ifndef JSCRIPT_EXEC
#define JSCRIPT_EXEC        PC_VAL("../jexec/jexec", "/bin/jexec")
#endif

#ifndef JSCRIPT_STARTUP
#define JSCRIPT_STARTUP     PC_VAL("./startup", "/tmp/startup")
#endif

#ifndef JSCRIPT_CACHE
#define JSCRIPT_CACHE       PC_VAL("./cache", "/tmp/cache")
#endif

/*
if content:exist, filename:exist, then
        cache must cache/flash
{
    "type": "sh/js",                    #default: sh, just for content
    "content": "content",               #must base64 encode
    "argument": [                       #option
        "arg1",
        "arg2",
        "arg3",
        ...
    ],
    
    "filename": "filename",
    "url": "file url for get",          #option with filename
    "md5": "md5 string",                #must with filename

    "sendtime": "1900-01-01 00:00:00",  #must, the message time, utc format
    "recvtime": "1900-01-01 00:00:00",  #if run:next, append and save it
    "period": SECOND,                   #option, just for run:this
    "run": "this/next",                 #default: this
                                        #   this: run at this
                                        #   next: run at next
    "cache": "none/cache",              #default: none
    "scope": "global/instance",         #default: instance

    "board": "BOARD",                   #option
    "slot": SLOT-NUMBER,                #default: 0
    "seq": SEQ-NUMBER,                  #must exist
    "id": "GUID",                       #must exist
    "reply": "COMMAND",                 #option
    "script": "script file"             #default: /bin/jscript

    "instance": {
        "name": "instance name",        #must exist
        "topic": "topic",               #option
        "channel": "channel",           #option
        "cache": "global cache path"    #must exist
    }
}

{
    "mac": "MAC",
    "recvtime": "1900-01-01 00:00:00",
    "exectime": "1900-01-01 00:00:00",
    "seq": SEQ-NUMBER,
    "id": "GUID",
    "ack": "error/reply",
    "reply": {
        "stdout": "STDOUT",
        "stderr": "STDERR",
        "errno": error-number
    }
}
*/
#if 1
#define SCRIPT_TYPE_ENUM_MAPPER(_)  \
    _(SCRIPT_TYPE_JS,   0,  "js"),  \
    _(SCRIPT_TYPE_SH,   1,  "sh"),  \
    /* end */
DECLARE_ENUM(script_type, SCRIPT_TYPE_ENUM_MAPPER, SCRIPT_TYPE_END);

#define SCRIPT_TYPE_JS      SCRIPT_TYPE_JS
#define SCRIPT_TYPE_SH      SCRIPT_TYPE_SH
#define SCRIPT_TYPE_END     SCRIPT_TYPE_END

static inline enum_ops_t *script_type_ops(void);
static inline bool is_good_script_type(int id);
static inline char *script_type_getnamebyid(int id);
static inline int script_type_getidbyname(const char *name);
#endif
    
#if 1
#define SCRIPT_RUN_ENUM_MAPPER(_)       \
    _(SCRIPT_RUN_THIS,  0,  "this"),    \
    _(SCRIPT_RUN_NEXT,  1,  "next"),    \
    /* end */
DECLARE_ENUM(script_run, SCRIPT_RUN_ENUM_MAPPER, SCRIPT_RUN_END);

#define SCRIPT_RUN_THIS     SCRIPT_RUN_THIS
#define SCRIPT_RUN_NEXT     SCRIPT_RUN_NEXT
#define SCRIPT_RUN_END      SCRIPT_RUN_END

static inline enum_ops_t *script_run_ops(void);
static inline bool is_good_script_run(int id);
static inline char *script_run_getnamebyid(int id);
static inline int script_run_getidbyname(const char *name);
#endif

#if 1
#define SCRIPT_CACHE_ENUM_MAPPER(_)         \
    _(SCRIPT_CACHE_NONE,    0,  "none"),    \
    _(SCRIPT_CACHE_CACHE,   1,  "cache"),   \
    /* end */
DECLARE_ENUM(script_cache, SCRIPT_CACHE_ENUM_MAPPER, SCRIPT_CACHE_END);

#define SCRIPT_CACHE_NONE       SCRIPT_CACHE_NONE
#define SCRIPT_CACHE_CACHE      SCRIPT_CACHE_CACHE
#define SCRIPT_CACHE_END        SCRIPT_CACHE_END

static inline enum_ops_t *script_cache_ops(void);
static inline bool is_good_script_cache(int id);
static inline char *script_cache_getnamebyid(int id);
static inline int script_cache_getidbyname(const char *name);
#endif
    
#if 1
#define SCRIPT_SCOPE_ENUM_MAPPER(_)                 \
    _(SCRIPT_SCOPE_GLOBAL,      0,  "global"),      \
    _(SCRIPT_SCOPE_INSTANCE,    1,  "instance"),    \
    /* end */
DECLARE_ENUM(script_scope, SCRIPT_SCOPE_ENUM_MAPPER, SCRIPT_SCOPE_END);

#define SCRIPT_SCOPE_GLOBAL     SCRIPT_SCOPE_GLOBAL
#define SCRIPT_SCOPE_INSTANCE   SCRIPT_SCOPE_INSTANCE
#define SCRIPT_SCOPE_END        SCRIPT_SCOPE_END

static inline enum_ops_t *script_scope_ops(void);
static inline bool is_good_script_scope(int id);
static inline char *script_scope_getnamebyid(int id);
static inline int script_scope_getidbyname(const char *name);
#endif

typedef struct {
    char *name;
    char *topic;
    char *channel;
    char *cache;
} jinstance_t;

#if 1
#define JINSTANCE_JRULE_MAPPER(_) \
    _(offsetof(jinstance_t, name), name, "name",    \
            string, sizeof(char *), JRULE_MUST,     \
            JRULE_VAR_STRDUP,                       \
            JRULE_VAR_NULL,                         \
            JRULE_VAR_NULL),                        \
    _(offsetof(jinstance_t, topic), topic, "topic", \
            string, sizeof(char *), 0,              \
            JRULE_VAR_STRDUP,                       \
            JRULE_VAR_NULL,                         \
            JRULE_VAR_NULL),                        \
    _(offsetof(jinstance_t, channel), channel, "channel",  \
            string, sizeof(char *), 0,              \
            JRULE_VAR_STRDUP,                       \
            JRULE_VAR_NULL,                         \
            JRULE_VAR_NULL),                        \
    _(offsetof(jinstance_t, cache), cache, "cache", \
            string, sizeof(char *), JRULE_MUST,     \
            JRULE_VAR_STRDUP,                       \
            JRULE_VAR_NULL,                         \
            JRULE_VAR_NULL),                        \
    /* end */
DECLARE_JRULER(jinstance, JINSTANCE_JRULE_MAPPER);

static inline jrule_t *jinstance_jrules(void);
#endif

#define JSCRIPT_REMOTE_DEFAULT          "/bin/jscript"

typedef struct {
    int type;
    int run;
    int cache;
    int scope;
    int slot;
    uint64 seq;
    
    char **argument;
    char *filename;
    char *content;
    char *url;
    char *md5;
    char *board;
    char *id;
    char *reply;
    char *script;
    
    time_t sendtime;
    time_t recvtime;
    uint32 period;
    
    jinstance_t instance;
}
jscript_t;

#if 1
#define JSCRIPT_JRULE_MAPPER(_) \
    _(offsetof(jscript_t, type), type, "type",          \
            enum, sizeof(int), 0,                       \
            JRULE_VAR_ENUM(script_type_ops),            \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_INT(SCRIPT_TYPE_SH)),             \
    _(offsetof(jscript_t, run), run, "run",             \
            enum, sizeof(int), 0,                       \
            JRULE_VAR_ENUM(script_run_ops),             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_INT(SCRIPT_RUN_THIS)),            \
    _(offsetof(jscript_t, cache), cache, "cache",       \
            enum, sizeof(int), 0,                       \
            JRULE_VAR_ENUM(script_cache_ops),           \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_INT(SCRIPT_CACHE_NONE)),          \
    _(offsetof(jscript_t, scope), scope, "scope",       \
            enum, sizeof(int), 0,                       \
            JRULE_VAR_ENUM(script_scope_ops),           \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_INT(SCRIPT_SCOPE_GLOBAL)),        \
    _(offsetof(jscript_t, slot), slot, "slot",          \
            int, sizeof(int), JRULE_BORDER,             \
            JRULE_VAR_INT(0),                           \
            JRULE_VAR_INT(1),                           \
            JRULE_VAR_INT(0)),                          \
    _(offsetof(jscript_t, seq), seq, "seq",             \
            u64, sizeof(uint64), JRULE_MUST,            \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(jscript_t, filename), filename, "filename", \
            string, sizeof(char *), 0,                  \
            JRULE_VAR_STRDUP,                           \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(jscript_t, content), content, "content", \
            string, sizeof(char *), 0,                  \
            JRULE_VAR_STRDUP,                           \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(jscript_t, url), url, "url",             \
            string, sizeof(char *), 0,                  \
            JRULE_VAR_STRDUP,                           \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(jscript_t, md5), md5, "md5",             \
            string, sizeof(char *), 0,                  \
            JRULE_VAR_STRDUP,                           \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(jscript_t, board), board, "board",       \
            string, sizeof(char *), 0,                  \
            JRULE_VAR_STRDUP,                           \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(jscript_t, id), id, "id",                \
            string, sizeof(char *), JRULE_MUST,         \
            JRULE_VAR_STRDUP,                           \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(jscript_t, reply), reply, "reply",       \
            string, sizeof(char *), 0,                  \
            JRULE_VAR_STRDUP,                           \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(jscript_t, script), script, "script",    \
            string, sizeof(char *), 0,                  \
            JRULE_VAR_STRDUP,                           \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_STRING(JSCRIPT_REMOTE_DEFAULT)),  \
    _(offsetof(jscript_t, sendtime), sendtime, "sendtime", \
            time, sizeof(time_t), JRULE_MUST,           \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(jscript_t, recvtime), recvtime, "recvtime", \
            time, sizeof(time_t), 0,                    \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(jscript_t, period), period, "period",    \
            u32, sizeof(uint32), JRULE_MUST,            \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(jscript_t, instance), instance, "instance", \
            object, sizeof(jinstance_t), JRULE_MUST,    \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_RULES(jinstance_jrules)),         \
    /* end */
DECLARE_JRULER(jscript, JSCRIPT_JRULE_MAPPER);

static inline jrule_t *jscript_jrules(void);
#endif

static jscript_t J;

static struct {
    int slot;
    char *mac;
    
    jobj_t jobj;
    jobj_t jstd;
    jobj_t jreply;
    
    bool is_startup;

    char dir[1+OS_LINE_LEN];
    char filename[1+OS_LINE_LEN];
} G;

STATIC int
jscript_usage(int argc, char *argv[])
{
    os_eprintln(__THIS_APPNAME);
    os_eprintln(__tab __THIS_APPNAME " json [args1 args2 args3]");

    return -EINVAL;
}

STATIC bool
jscript_md5eqf(char *filename)
{
    byte md5[OS_MD5_SIZE];
    char md5string[1+OS_MD5_SIZE] = {0};

    md5_file(filename, md5);

    md5_bin2hex(md5string, md5);

    return os_streq(md5string, J.md5);
}

STATIC bool
jscript_md5eqc(char *content)
{
    byte md5[OS_MD5_SIZE];
    char md5string[1+OS_MD5_SIZE] = {0};

    md5_encode(md5, content, os_strlen(content));

    md5_bin2hex(md5string, md5);

    return os_streq(md5string, J.md5);
}

STATIC int
jscript_report(char *ack, jobj_t jreply)
{
    int err = 0;
    
    if (NULL==J.reply) {
        return 0;
    }

    jobj_t jobj = jobj_new_object();
    if (NULL==jobj) {
        return -ENOMEM;
    }
    time_t now = time(NULL);

    jobj_add_string(jobj, "mac", G.mac);
    jobj_add_string(jobj, "recvtime", os_fulltime_string(J.recvtime));
    jobj_add_string(jobj, "exectime", os_fulltime_string(now));
    jobj_add_string(jobj, "id", J.id);
    jobj_add_string(jobj, "ack", ack);
    jobj_add_u64(jobj, "seq", J.seq);

    jobj_add(jobj, "reply", jreply);

    err = os_system("%s '%s'", J.reply, jobj_json(jobj));
    jobj_put(jobj);

    return err;
}

STATIC jobj_t 
jscript_jreply(int error, char *outstring, char *errstring)
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
jscript_error(int error, char *errstring)
{
    jscript_report("error", jscript_jreply(error, NULL, errstring));

    return error;
}

STATIC int 
jscript_callback(int error, char *outstring, char *errstring)
{
    jscript_report("reply", jscript_jreply(error, outstring, errstring));

    return 0;
}

STATIC int
jscript_exec(void)
{
    int err;
    
    time_t now = time(NULL);

    if (0==J.period || now < (J.sendtime + J.period)) {
        char *json = jobj_json(G.jobj);

        if (G.slot == J.slot) {
            // ipc
            err = os_pexec_json(json, jscript_callback);
            if (err<0) {
                return jscript_error(err, "jexec error");
            }
        } else {
            // rpc
            err = os_system(JSCRIPT_REMOTE " %d '%s'", G.slot, jobj_json(G.jobj));

            return jscript_error(err, 0==err?"jremote ok":"jremote error");
        }
    }

    return 0;
}

STATIC char *
jscript_dir(void)
{
    if (false==is_good_str(G.dir)) {
        switch(J.scope) {
            case SCRIPT_SCOPE_INSTANCE:
                os_saprintf(G.dir, "%s/%s", J.instance.cache, J.instance.name);
                
                break;
            case SCRIPT_SCOPE_GLOBAL:
            default:
                os_saprintf(G.dir, "%s", J.instance.cache);
                
                break;
        }
    }
    
    return G.dir;
}

STATIC char *
jscript_filename(void)
{
    if (false==is_good_str(G.filename)) {
        if ('/'==J.filename[0]) {
            os_saprintf(G.filename, "%s", J.filename);
        } else {
            os_saprintf(G.filename, "%s/%s", jscript_dir(), J.filename);
        }
    }

    return G.filename;
}

STATIC int
jscript_save_content(void)
{
    int err = 0;
    char *content = b64_decode((byte *)J.content, os_strlen(J.content));
    if (NULL==content) {
        return -ENOMEM;
    }
    
    if (NULL==J.md5 || false==jscript_md5eqc(content)) {
        err = os_writefile(jscript_filename(), content, os_strlen(content), false, false);
    }

    os_free(content);
    
    return jscript_error(0, NULL);
}

STATIC int
jscript_download(char *file)
{
    if (NULL==J.url) {
        return jscript_error(1, "file not exist and no url");
    } else {
        int err = os_system("curl -o %s %s &> /dev/null", file, J.url);

        return jscript_error(err, "download");
    }
}

STATIC int
jscript_save_file(void)
{
    if (NULL==J.filename) {
        /*
        * no filename, needn't save
        */
        return jscript_error(0, NULL);
    }
    else if (SCRIPT_CACHE_NONE==J.cache) {
        /*
        * no cache, needn't save
        */
        return jscript_error(0, NULL);
    }
    else if (J.content) {
        return jscript_save_content();
    }

    char *file = jscript_filename();
    if (false==os_file_exist(file)) {
        return jscript_download(file);
    }

    if (jscript_md5eqf(file)) {
        return jscript_error(0, NULL);
    } else {
        return jscript_download(file);
    }
}

STATIC int
jscript_save_json(void)
{
    char file[1+OS_LINE_LEN];
    char *json = jobj_json(G.jobj);
    
    os_saprintf(file, "/tmp/startup/next/%s.%llu.json",
        J.instance.name,
        J.seq);

    return os_writefile(file, json, os_strlen(json), false, false);
}

STATIC int
jscript_handle(void)
{
    jscript_save_file();

    if (SCRIPT_RUN_NEXT==J.run) {
        return jscript_save_json();
    } else {
        return jscript_exec();
    }
}

STATIC int
jscript_check(void)
{
    if (NULL==J.filename && NULL==J.content) {
        return jscript_error(1, "no filename and content");
    }
    
    return 0;
}

STATIC int
jscript_main_helper(int argc, char *argv[])
{
    char *json = NULL;
    int err;
    
    G.slot  = env_geti("SLOT", 0);
    G.mac   = os_getmacby(SCRIPT_GETBASEMAC);

    /*
    * get input json
    */
    switch(argc) {
        case 1:
            json = os_readfd(0);
            if (NULL==json) {
                return -ENOMEM;
            }
            
            break;
        case 2:
            json = argv[1];
            
            break;
        default:
            return jscript_usage(argc, argv);
    }
    
    G.jobj = jobj_byjson(json);
    if (NULL==G.jobj) {
        return -EBADJSON;
    }

    /*
    * use buildin script
    */
    jobj_t jval = jobj_get(G.jobj, "script");
    if (jval) {
        if (jtype_string != jobj_type(jval)) {
            return jscript_error(-EBADJSON, "bad json");
        }
        
        char *script = jobj_get_string(jval);
        if (os_strneq("/bin/" __THIS_APPNAME, script)) {
            return os_system("%s '%s'", script, json);
        }
    }
    
    /*
    * append recvtime
    */
    if (NULL==jobj_get(G.jobj, "recvtime")) {
        J.recvtime = time(NULL);
        
        char *now = os_fulltime_string(J.recvtime);
        jobj_add(G.jobj, "recvtime", jobj_new_string(now));
    } else {
        G.is_startup = true;
    }
    
    err = jrule_j2o(jscript_jrules(), &J, G.jobj);
    if (err<0) {
        return jscript_error(err, "bad json");
    }
    
    err = jscript_check();
    if (err<0) {
        return err;
    }
    
    return jscript_handle();
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_ignore(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(jscript_main_helper, argc, argv);
}
/******************************************************************************/
