#ifndef __JSCRIPT_H_f2d1a60baa9f41bcb1cb7c751964def2__
#define __JSCRIPT_H_f2d1a60baa9f41bcb1cb7c751964def2__
#ifdef __APP__
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

#ifndef JSCRIPT_SCRIPT
#define JSCRIPT_SCRIPT      PC_VAL("../jscript/jscript", "/bin/jscript")
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
#define JSCRIPT_TYPE_ENUM_MAPPER(_)  \
    _(JSCRIPT_TYPE_JS,   0,  "js"),  \
    _(JSCRIPT_TYPE_SH,   1,  "sh"),  \
    /* end */
DECLARE_ENUM(jscript_type, JSCRIPT_TYPE_ENUM_MAPPER, JSCRIPT_TYPE_END);

#define JSCRIPT_TYPE_JS      JSCRIPT_TYPE_JS
#define JSCRIPT_TYPE_SH      JSCRIPT_TYPE_SH
#define JSCRIPT_TYPE_END     JSCRIPT_TYPE_END

static inline enum_ops_t *jscript_type_ops(void);
static inline bool is_good_jscript_type(int id);
static inline char *jscript_type_getnamebyid(int id);
static inline int jscript_type_getidbyname(const char *name);
#endif

#if 1
#define JSCRIPT_RUN_ENUM_MAPPER(_)       \
    _(JSCRIPT_RUN_THIS,  0,  "this"),    \
    _(JSCRIPT_RUN_NEXT,  1,  "next"),    \
    /* end */
DECLARE_ENUM(jscript_run, JSCRIPT_RUN_ENUM_MAPPER, JSCRIPT_RUN_END);

#define JSCRIPT_RUN_THIS     JSCRIPT_RUN_THIS
#define JSCRIPT_RUN_NEXT     JSCRIPT_RUN_NEXT
#define JSCRIPT_RUN_END      JSCRIPT_RUN_END

static inline enum_ops_t *jscript_run_ops(void);
static inline bool is_good_jscript_run(int id);
static inline char *jscript_run_getnamebyid(int id);
static inline int jscript_run_getidbyname(const char *name);
#endif

#if 1
#define JSCRIPT_CACHE_ENUM_MAPPER(_)         \
    _(JSCRIPT_CACHE_NONE,    0,  "none"),    \
    _(JSCRIPT_CACHE_CACHE,   1,  "cache"),   \
    /* end */
DECLARE_ENUM(jscript_cache, JSCRIPT_CACHE_ENUM_MAPPER, JSCRIPT_CACHE_END);

#define JSCRIPT_CACHE_NONE       JSCRIPT_CACHE_NONE
#define JSCRIPT_CACHE_CACHE      JSCRIPT_CACHE_CACHE
#define JSCRIPT_CACHE_END        JSCRIPT_CACHE_END

static inline enum_ops_t *jscript_cache_ops(void);
static inline bool is_good_jscript_cache(int id);
static inline char *jscript_cache_getnamebyid(int id);
static inline int jscript_cache_getidbyname(const char *name);
#endif

#if 1
#define JSCRIPT_SCOPE_ENUM_MAPPER(_)                 \
    _(JSCRIPT_SCOPE_GLOBAL,      0,  "global"),      \
    _(JSCRIPT_SCOPE_INSTANCE,    1,  "instance"),    \
    /* end */
DECLARE_ENUM(jscript_scope, JSCRIPT_SCOPE_ENUM_MAPPER, JSCRIPT_SCOPE_END);

#define JSCRIPT_SCOPE_GLOBAL     JSCRIPT_SCOPE_GLOBAL
#define JSCRIPT_SCOPE_INSTANCE   JSCRIPT_SCOPE_INSTANCE
#define JSCRIPT_SCOPE_END        JSCRIPT_SCOPE_END

static inline enum_ops_t *jscript_scope_ops(void);
static inline bool is_good_jscript_scope(int id);
static inline char *jscript_scope_getnamebyid(int id);
static inline int jscript_scope_getidbyname(const char *name);
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
            string, sizeof(char *), JRULE_F_MUST,   \
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
            string, sizeof(char *), JRULE_F_MUST,   \
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

    struct {
        int slot;
        char *basemac;

        bool is_startup;
    } dev;
}
jscript_t;

#define JSCRIPT_ZERO    {.script = NULL}

#if 1
#define JSCRIPT_JRULE_MAPPER(_) \
    _(offsetof(jscript_t, type), type, "type",          \
            enum, sizeof(int), 0,                       \
            JRULE_VAR_ENUM(jscript_type_ops),           \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_INT(JSCRIPT_TYPE_SH)),            \
    _(offsetof(jscript_t, run), run, "run",             \
            enum, sizeof(int), 0,                       \
            JRULE_VAR_ENUM(jscript_run_ops),            \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_INT(JSCRIPT_RUN_THIS)),           \
    _(offsetof(jscript_t, cache), cache, "cache",       \
            enum, sizeof(int), 0,                       \
            JRULE_VAR_ENUM(jscript_cache_ops),          \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_INT(JSCRIPT_CACHE_NONE)),         \
    _(offsetof(jscript_t, scope), scope, "scope",       \
            enum, sizeof(int), 0,                       \
            JRULE_VAR_ENUM(jscript_scope_ops),          \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_INT(JSCRIPT_SCOPE_GLOBAL)),       \
    _(offsetof(jscript_t, slot), slot, "slot",          \
            int, sizeof(int), JRULE_F_BORDER,           \
            JRULE_VAR_INT(0),                           \
            JRULE_VAR_INT(1),                           \
            JRULE_VAR_INT(0)),                          \
    _(offsetof(jscript_t, seq), seq, "seq",             \
            u64, sizeof(uint64), JRULE_F_MUST,          \
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
            string, sizeof(char *), JRULE_F_MUST,       \
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
            time, sizeof(time_t), JRULE_F_MUST,         \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(jscript_t, recvtime), recvtime, "recvtime", \
            time, sizeof(time_t), 0,                    \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(jscript_t, period), period, "period",    \
            u32, sizeof(uint32), JRULE_F_MUST,          \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(jscript_t, instance), instance, "instance", \
            object, sizeof(jinstance_t), JRULE_F_MUST,  \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_RULES(jinstance_jrules)),         \
    /* end */
DECLARE_JRULER(jscript, JSCRIPT_JRULE_MAPPER);

static inline jrule_t *jscript_jrules(void);
#endif

extern int
jscript_exec(char *json);
/******************************************************************************/
#endif /* __APP__ */
#endif /* __JSCRIPT_H_f2d1a60baa9f41bcb1cb7c751964def2__ */
