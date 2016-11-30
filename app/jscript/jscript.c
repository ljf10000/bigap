/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      jscript
#endif

#include "utils.h"

OS_INITER;
/******************************************************************************/
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
        "topic": "topic",               #must exist
        "channel": "channel",           #must exist
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
    _(SCRIPT_RUN_EVERY, 2,  "every"),   \
    /* end */
DECLARE_ENUM(script_run, SCRIPT_RUN_ENUM_MAPPER, SCRIPT_RUN_END);

#define SCRIPT_RUN_THIS     SCRIPT_RUN_THIS
#define SCRIPT_RUN_NEXT     SCRIPT_RUN_NEXT
#define SCRIPT_RUN_EVERY    SCRIPT_RUN_EVERY
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
    _(SCRIPT_CACHE_FLASH,   2,  "flash"),   \
    /* end */
DECLARE_ENUM(script_cache, SCRIPT_CACHE_ENUM_MAPPER, SCRIPT_CACHE_END);

#define SCRIPT_CACHE_NONE       SCRIPT_CACHE_NONE
#define SCRIPT_CACHE_CACHE      SCRIPT_CACHE_CACHE
#define SCRIPT_CACHE_FLASH      SCRIPT_CACHE_FLASH
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
    char *flash;
} jinstance_t;

#if USE_JRULE
#define JINSTANCE_JRULE_MAPPER(_) \
    _(offsetof(jinstance_t, name), name, "name",    \
            string, sizeof(char *), JRULE_MUST,     \
            JRULE_VAR_STRDUP,                       \
            JRULE_VAR_NULL,                         \
            JRULE_VAR_NULL),                        \
    _(offsetof(jinstance_t, topic), topic, "topic", \
            string, sizeof(char *), JRULE_MUST,     \
            JRULE_VAR_STRDUP,                       \
            JRULE_VAR_NULL,                         \
            JRULE_VAR_NULL),                        \
    _(offsetof(jinstance_t, channel), channel, "channel",  \
            string, sizeof(char *), JRULE_MUST,     \
            JRULE_VAR_STRDUP,                       \
            JRULE_VAR_NULL,                         \
            JRULE_VAR_NULL),                        \
    _(offsetof(jinstance_t, cache), cache, "cache", \
            string, sizeof(char *), JRULE_MUST,     \
            JRULE_VAR_STRDUP,                       \
            JRULE_VAR_NULL,                         \
            JRULE_VAR_NULL),                        \
    _(offsetof(jinstance_t, flash), flash, "flash", \
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

#if USE_JRULE
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
            object, sizeof(jinstance_t), 0,             \
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
    jobj_t jobj;
    
    bool is_startup;

    char dir[1+OS_LINE_LEN];
    char filename[1+OS_LINE_LEN];
} G;

static int
usage(int argc, char *argv[])
{
    os_eprintln(__THIS_APPNAME);
    os_eprintln(__tab __THIS_APPNAME " json [args1 args2 args3]");

    return -EINVAL;
}

static int
__rpc(void)
{
    return os_system(JSCRIPT_REMOTE " %d '%s'", G.slot, jobj_json(G.jobj));
}

static int
__exec(void)
{
    time_t now = time(NULL);

    if (0==J.period || now < (J.sendtime + J.period)) {
        char *json = jobj_json(G.jobj);
#if 1
        return os_pexec_json(json);
#else
        return os_system(JSCRIPT_EXEC " '%s'", json);
#endif
    }

    return 0;
}

static char *
__dir(void)
{
    if (G.is_startup) {
        return JSCRIPT_STARTUP;
    }
    else if (SCRIPT_SCOPE_INSTANCE==J.scope) {
        if (false==is_good_str(G.dir)) {
            os_saprintf(G.dir, JSCRIPT_CACHE "/%s", J.instance.name);
        }

        return G.dir;
    }
    else {
        return JSCRIPT_CACHE;
    }
}

static char *
__filename(void)
{
    if (false==is_good_str(G.filename)) {
        os_saprintf(G.filename, "%s/%s", __dir(), J.filename);
    }

    return G.filename;
}

static bool
__md5eqf(char *filename)
{
    byte md5[OS_MD5_SIZE];
    char md5string[1+OS_MD5_SIZE] = {0};

    md5_file(filename, md5);

    md5_bin2hex(md5string, md5);

    return os_streq(md5string, J.md5);
}

static bool
__md5eqc(char *content)
{
    byte md5[OS_MD5_SIZE];
    char md5string[1+OS_MD5_SIZE] = {0};

    md5_encode(md5, content, os_strlen(content));

    md5_bin2hex(md5string, md5);

    return os_streq(md5string, J.md5);
}

static int
__check(void)
{
    if (NULL==J.filename && NULL==J.content) {
        return -EBADJSON;
    }
    
    return 0;
}

static int
__handle(void)
{
    int err;
    
    if (NULL==J.filename) {
        /*
        * no filename
        *   needn't save, just exec
        */
        return __exec();
    }
    else if (SCRIPT_CACHE_NONE==J.cache) {
        /*
        * no cache
        *   needn't save, just exec
        */
        return __exec();
    }

    char *filename = __filename();
    bool exist  = os_file_exist(filename);

    if (J.content) {
        if (false==exist || (J.md5 && false==__md5eqc(J.content))) {
            /*
            * file not-exist
            * file exist, diff md5
            *   save it
            */
            os_writefile(filename, J.content, os_strlen(J.content), false, false);
        }
    }
    else if (J.url) {
        if (false==exist || (J.md5 && false==__md5eqf(filename))) {
            /*
            * file not-exist
            * file exist, diff md5
            *   re-get it
            */
            os_getfile_byurl(filename, J.url);
        }
    }

    return __exec();
}

static int
__main(int argc, char *argv[])
{
    char *json = NULL;
    int err;
    
    G.slot = env_geti("SLOT", 0);
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
            return usage(argc, argv);
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
            return -EBADJSON;
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
        char *now = os_fulltime_string(time(NULL));
        
        jobj_add(G.jobj, "recvtime", jobj_new_string(now));

        G.is_startup = false;
    }
    
    err = jrule_j2o(jscript_jrules(), &J, G.jobj);
    if (err<0) {
        return err;
    }
    
    err = __check();
    if (err<0) {
        return err;
    }
    
    if (G.is_startup) {
        return __exec();
    }
    else if (G.slot == J.slot) {
        return __handle();
    }
    else {
        return __rpc();
    }
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(__main, argc, argv);
}
/******************************************************************************/
