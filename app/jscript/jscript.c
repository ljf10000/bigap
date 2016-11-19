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
    "mustbefore":"1900-01-01 00:00:00", #option, just for run:this
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
    
    static inline bool is_good_script_type(int id);
    static inline char *script_type_string(int id);
    static inline int script_type_idx(char *name);
    
#define SCRIPT_TYPE_JS      SCRIPT_TYPE_JS
#define SCRIPT_TYPE_SH      SCRIPT_TYPE_SH
#define SCRIPT_TYPE_END     SCRIPT_TYPE_END
#endif
    
#if 1
#define SCRIPT_RUN_ENUM_MAPPER(_)           \
        _(SCRIPT_RUN_THIS,  0,  "this"),    \
        _(SCRIPT_RUN_NEXT,  1,  "next"),    \
        /* end */
    DECLARE_ENUM(script_run, SCRIPT_RUN_ENUM_MAPPER, SCRIPT_RUN_END);
    
    static inline bool is_good_script_run(int id);
    static inline char *script_run_string(int id);
    static inline int script_run_idx(char *name);
    
#define SCRIPT_RUN_THIS     SCRIPT_RUN_THIS
#define SCRIPT_RUN_NEXT     SCRIPT_RUN_NEXT
#define SCRIPT_RUN_END      SCRIPT_RUN_END
#endif
    
#if 1
#define SCRIPT_CACHE_ENUM_MAPPER(_)         \
        _(SCRIPT_CACHE_NONE,    0,  "none"),    \
        _(SCRIPT_CACHE_CACHE,   1,  "cache"),   \
        _(SCRIPT_CACHE_FLASH,   2,  "flash"),   \
        /* end */
    DECLARE_ENUM(script_cache, SCRIPT_CACHE_ENUM_MAPPER, SCRIPT_CACHE_END);
    
    static inline bool is_good_script_cache(int id);
    static inline char *script_cache_string(int id);
    static inline int script_cache_idx(char *name);
    
#define SCRIPT_CACHE_NONE       SCRIPT_CACHE_NONE
#define SCRIPT_CACHE_CACHE      SCRIPT_CACHE_CACHE
#define SCRIPT_CACHE_FLASH      SCRIPT_CACHE_FLASH
#define SCRIPT_CACHE_END        SCRIPT_CACHE_END
#endif
    
#if 1
#define SCRIPT_SCOPE_ENUM_MAPPER(_)                 \
        _(SCRIPT_SCOPE_GLOBAL,      0,  "global"),      \
        _(SCRIPT_SCOPE_INSTANCE,    1,  "instance"),    \
        /* end */
    DECLARE_ENUM(script_scope, SCRIPT_SCOPE_ENUM_MAPPER, SCRIPT_SCOPE_END);
    
    static inline bool is_good_script_scope(int id);
    static inline char *script_scope_string(int id);
    static inline int script_scope_idx(char *name);
    
#define SCRIPT_SCOPE_GLOBAL     SCRIPT_SCOPE_GLOBAL
#define SCRIPT_SCOPE_INSTANCE   SCRIPT_SCOPE_INSTANCE
#define SCRIPT_SCOPE_END        SCRIPT_SCOPE_END
#endif

#if 1
typedef struct {
    char *name;
    char *topic;
    char *channel;
    char *cache;
    char *flash;
} jinstance_t;

#define JINSTANCE_JENUM_MAPPER(_) \
    _(JINSTANCE_NAME,   0,  "name",     jtype_string,   JRULE_MUST, NULL), \
    _(JINSTANCE_TOPIC,  1,  "topic",    jtype_string,   JRULE_MUST, NULL), \
    _(JINSTANCE_CHANNEL,2,  "channel",  jtype_string,   JRULE_MUST, NULL), \
    _(JINSTANCE_CACHE,  3,  "cache",    jtype_string,   JRULE_MUST, NULL), \
    _(JINSTANCE_FLASH,  4,  "flash",    jtype_string,   JRULE_MUST, NULL), \
    /* end */
DECLARE_JENUM(jinstance, JINSTANCE_JENUM_MAPPER, JINSTANCE_END);

static inline bool is_good_jinstance(int id);
static inline char *jinstance_string(int id);
static inline int jinstance_idx(char *name);

static inline jrule_ops_t *jinstance_jrule_ops(void);
static inline int jinstance_jcheck(jobj_t jobj);
static inline int jinstance_jrepair(jobj_t jobj);

#define JINSTANCE_NAME      JINSTANCE_NAME
#define JINSTANCE_TOPIC     JINSTANCE_TOPIC
#define JINSTANCE_CHANNEL   JINSTANCE_CHANNEL
#define JINSTANCE_CACHE     JINSTANCE_CACHE
#define JINSTANCE_FLASH     JINSTANCE_FLASH
#define JINSTANCE_END       JINSTANCE_END
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
    time_t mustbefore;
    
    jinstance_t instance;
}
jscript_t;
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
    "mustbefore":"1900-01-01 00:00:00", #option, just for run:this
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

#define JSCRIPT_SCRIPT_DEFAULT          "/bin/jscript"

#define JSCRIPT_JENUM_MAPPER(_) \
    _(JSCRIPT_TYPE,     0,  "type",     jtype_string,   0,          "sh"),      \
    _(JSCRIPT_RUN,      1,  "run",      jtype_string,   0,          "this"),    \
    _(JSCRIPT_CACHE,    2,  "cache",    jtype_string,   0,          "none"),    \
    _(JSCRIPT_SCOPE,    4,  "scope",    jtype_string,   0,          "instance"),\
    _(JSCRIPT_SLOT,     5,  "slot",     jtype_int,      0,          0),         \
    _(JSCRIPT_SEQ,      6,  "seq",      jtype_int,      JRULE_MUST, 0),         \
    _(JSCRIPT_ARGUMENT, 7,  "argument", jtype_array,    0,          NULL),      \
    _(JSCRIPT_FILENAME, 8,  "filename", jtype_string,   0,          NULL),      \
    _(JSCRIPT_CONTENT,  9,  "content",  jtype_string,   0,          NULL),      \
    _(JSCRIPT_URL,      10, "url",      jtype_string,   0,          NULL),      \
    _(JSCRIPT_MD5,      11, "md5",      jtype_string,   0,          NULL),      \
    _(JSCRIPT_BOARD,    12, "board",    jtype_string,   0,          NULL),      \
    _(JSCRIPT_ID,       13, "id",       jtype_string,   JRULE_MUST, NULL),      \
    _(JSCRIPT_REPLY,    14, "reply",    jtype_string,   0,          NULL),      \
    _(JSCRIPT_SCRIPT,   15, "script",   jtype_string,   0,          JSCRIPT_SCRIPT_DEFAULT), \
    _(JSCRIPT_INSTANCE, 16, "instance", jtype_object,   JRULE_MUST, jinstance_jrepair), \
    /* end */
DECLARE_JENUM(jscript, JSCRIPT_JENUM_MAPPER, JSCRIPT_END);

static inline bool is_good_jscript(int id);
static inline char *jscript_string(int id);
static inline int jscript_idx(char *name);

static inline jrule_ops_t *jscript_jrule_ops(void);
static inline int jscript_jcheck(jobj_t jobj);
static inline int jscript_jrepair(jobj_t jobj);

#define JSCRIPT_TYPE        JSCRIPT_TYPE
#define JSCRIPT_RUN         JSCRIPT_RUN
#define JSCRIPT_CACHE       JSCRIPT_CACHE
#define JSCRIPT_SCOPE       JSCRIPT_SCOPE
#define JSCRIPT_SLOT        JSCRIPT_SLOT
#define JSCRIPT_SEQ         JSCRIPT_SEQ
#define JSCRIPT_ARGUMENT    JSCRIPT_ARGUMENT
#define JSCRIPT_FILENAME    JSCRIPT_FILENAME
#define JSCRIPT_CONTENT     JSCRIPT_CONTENT
#define JSCRIPT_URL         JSCRIPT_URL
#define JSCRIPT_MD5         JSCRIPT_MD5
#define JSCRIPT_BOARD       JSCRIPT_BOARD
#define JSCRIPT_ID          JSCRIPT_ID
#define JSCRIPT_REPLY       JSCRIPT_REPLY
#define JSCRIPT_SCRIPT      JSCRIPT_SCRIPT
#define JSCRIPT_INSTANCE    JSCRIPT_INSTANCE
#define JSCRIPT_END         JSCRIPT_END
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
