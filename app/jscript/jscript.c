/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      jscript
#endif

#include "utils.h"

OS_INITER;
/******************************************************************************/
/*
if content:exist, filename:exist, then
        cache must cache/flash
{
    "type": "sh/js",                    #default: sh, just for content
    "content": "content",               #must base64 encode
    
    "filename": "filename",
    "url": "file url for get",          #option with filename
    "md5": "md5 string",                #must with filename
    "argument": [                       #option
        "arg1",
        "arg2",
        "arg3",
        ...
    ],

    "sendtime": "1900-01-01 00:00:00",  #must, the message time, utc format
    "recvtime": "1900-01-01 00:00:00",  #if run:next, append and save it
    "before":"1900-01-01 00:00:00",     #option, just for run:this
    "run": "this/next",                 #default: this
                                        #   this: run at this period
                                        #   next: run at next period

    "cache": "none/cache/flash",        #default: none
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
        "channel:" "channel",           #must exist
        
        "cache": "global cache path",   #must exist
        "flash": "global flash path"    #must exist
    }
}
*/
#if 1
#define SCRIPT_TYPE_ENUM_MAPPER(_)      \
    _(SCRIPT_TYPE_JS,   0,  "js"),  \
    _(SCRIPT_TYPE_SH,   1,  "sh"),  \
    /* end */
DECLARE_ENUM(script_type, SCRIPT_TYPE_ENUM_MAPPER, SCRIPT_TYPE_END);
    
#define SCRIPT_TYPE_JS      SCRIPT_TYPE_JS
#define SCRIPT_TYPE_SH      SCRIPT_TYPE_SH
#define SCRIPT_TYPE_END     SCRIPT_TYPE_END

static inline enum_ops_t *script_type_ops_getter(void);
#endif
    
#if 1
#define SCRIPT_RUN_ENUM_MAPPER(_)           \
    _(SCRIPT_RUN_THIS,  0,  "this"),    \
    _(SCRIPT_RUN_NEXT,  1,  "next"),    \
    /* end */
DECLARE_ENUM(script_run, SCRIPT_RUN_ENUM_MAPPER, SCRIPT_RUN_END);
    
#define SCRIPT_RUN_THIS     SCRIPT_RUN_THIS
#define SCRIPT_RUN_NEXT     SCRIPT_RUN_NEXT
#define SCRIPT_RUN_END      SCRIPT_RUN_END

static inline enum_ops_t *script_run_ops_getter(void);
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

static inline enum_ops_t *script_cache_ops_getter(void);
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

static inline enum_ops_t *script_scope_ops_getter(void);
#endif

#if 1
typedef struct {
    char *name;
    char *topic;
    char *channel;
    char *cache;
    char *flash;
} jinstance_t;

#define JINSTANCE_JRULE_MAPPER(_) \
    _(offsetof(jinstance_t, name), name, "name",    \
            string, sizeof(char *), JRULE_MUST,     \
            JRULE_VAR_POINTER_INITER(jrule_strdup), \
            JRULE_VAR_NULL,                         \
            JRULE_VAR_NULL),                        \
    _(offsetof(jinstance_t, topic), topic, "topic", \
            string, sizeof(char *), JRULE_MUST,     \
            JRULE_VAR_POINTER_INITER(jrule_strdup), \
            JRULE_VAR_NULL,                         \
            JRULE_VAR_NULL),                        \
    _(offsetof(jinstance_t, channel), channel, "channel",  \
            string, sizeof(char *), JRULE_MUST,     \
            JRULE_VAR_POINTER_INITER(jrule_strdup), \
            JRULE_VAR_NULL,                         \
            JRULE_VAR_NULL),                        \
    _(offsetof(jinstance_t, cache), cache, "cache", \
            string, sizeof(char *), JRULE_MUST,     \
            JRULE_VAR_POINTER_INITER(jrule_strdup), \
            JRULE_VAR_NULL,                         \
            JRULE_VAR_NULL),                        \
    _(offsetof(jinstance_t, flash), flash, "flash", \
            string, sizeof(char *), JRULE_MUST,     \
            JRULE_VAR_POINTER_INITER(jrule_strdup), \
            JRULE_VAR_NULL,                         \
            JRULE_VAR_NULL),                        \
    /* end */
DECLARE_JRULER(jinstance, JINSTANCE_JRULE_MAPPER);

static inline jrule_t *jinstance_jrules(void);
static inline int jinstance_o2j(jobj_t jobj);
static inline int jinstance_j2o(jobj_t jobj);
#endif

#if 1
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

#define JSCRIPT_SCRIPT_DEFAULT          "/bin/jscript"

#define JSCRIPT_JRULE_MAPPER(_) \
    _(offsetof(jscript_t, type), type, "type",          \
            enum, sizeof(int), 0,                       \
            JRULE_VAR_POINTER_INITER(script_type_ops_getter), \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_ENUM_INITER(SCRIPT_TYPE_SH)),     \
    _(offsetof(jscript_t, run), run, "run",             \
            enum, sizeof(int), 0,                       \
            JRULE_VAR_POINTER_INITER(script_run_ops_getter), \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_ENUM_INITER(SCRIPT_RUN_THIS)),    \
    _(offsetof(jscript_t, cache), cache, "cache",       \
            enum, sizeof(int), 0,                       \
            JRULE_VAR_POINTER_INITER(script_cache_ops_getter), \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_ENUM_INITER(SCRIPT_CACHE_NONE)),  \
    _(offsetof(jscript_t, scope), scope, "scope",       \
            enum, sizeof(int), 0,                       \
            JRULE_VAR_POINTER_INITER(script_scope_ops_getter), \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_ENUM_INITER(SCRIPT_SCOPE_GLOBAL)),\
    _(offsetof(jscript_t, slot), slot, "slot",          \
            int, sizeof(int), JRULE_BORDER,             \
            JRULE_VAR_INT_INITER(0),                    \
            JRULE_VAR_INT_INITER(1),                    \
            JRULE_VAR_INT_INITER(0)),                   \
    _(offsetof(jscript_t, seq), seq, "seq",             \
            u64, sizeof(uint64), JRULE_MUST,            \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(jscript_t, filename), filename, "filename", \
            string, sizeof(char *), 0,                  \
            JRULE_VAR_POINTER_INITER(jrule_strdup),     \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(jscript_t, content), content, "content", \
            string, sizeof(char *), 0,                  \
            JRULE_VAR_POINTER_INITER(jrule_strdup),     \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(jscript_t, url), url, "url",             \
            string, sizeof(char *), 0,                  \
            JRULE_VAR_POINTER_INITER(jrule_strdup),     \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(jscript_t, md5), md5, "md5",             \
            string, sizeof(char *), 0,                  \
            JRULE_VAR_POINTER_INITER(jrule_strdup),     \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(jscript_t, board), board, "board",       \
            string, sizeof(char *), 0,                  \
            JRULE_VAR_POINTER_INITER(jrule_strdup),     \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(jscript_t, id), id, "id",                \
            string, sizeof(char *), JRULE_MUST,         \
            JRULE_VAR_POINTER_INITER(jrule_strdup),     \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(jscript_t, reply), reply, "reply",       \
            string, sizeof(char *), 0,                  \
            JRULE_VAR_POINTER_INITER(jrule_strdup),     \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(jscript_t, script), script, "script",    \
            string, sizeof(char *), 0,                  \
            JRULE_VAR_POINTER_INITER(jrule_strdup),     \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_POINTER_INITER(JSCRIPT_SCRIPT_DEFAULT)), \
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
            u32, sizeof(uint32), JRULE_MUST | JRULE_BORDER, \
            JRULE_VAR_INT32_INITER(0),                  \
            JRULE_VAR_INT32_INITER(3600),               \
            JRULE_VAR_NULL),                            \
    _(offsetof(jscript_t, argument), argument, "argument", \
            array, sizeof(char *), 0,                   \
            JRULE_VAR_INT32_INITER(0),                  \
            JRULE_VAR_INT32_INITER(3600),               \
            JRULE_VAR_NULL),                            \
    _(offsetof(jscript_t, instance), instance, "instance", \
            object, sizeof(jinstance_t), 0,             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_POINTER_INITER(jinstance_jrules)), \
    /* end */
DECLARE_JRULER(jscript, JSCRIPT_JENUM_MAPPER);

static inline jrule_t *jscript_jrules(void);
static inline int jscript_o2j(void *obj, jobj_t jobj);
static inline int jscript_j2o(void *obj, jobj_t jobj);
#endif

static jscript_t script;

static int
__main(int argc, char *argv[])
{
    return 0;
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(__main, argc, argv);
}
/******************************************************************************/
