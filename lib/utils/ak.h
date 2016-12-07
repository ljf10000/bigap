#ifndef __AK_H_2e420d20ac8a47b188d92ef8448db5fa__
#define __AK_H_2e420d20ac8a47b188d92ef8448db5fa__
/******************************************************************************/
#ifndef OS_APPNAMELEN
#define OS_APPNAMELEN           (32 - sizeof(uint32) - 1)
#endif

#ifndef OS_APPKEYLEN
#define OS_APPKEYLEN            (32 - 1)
#endif

#ifndef AK_PATH
#define AK_PATH                 "/tmp/ak"
#endif

#ifndef AK_PATH_DEFT
#define AK_PATH_DEFT            "/etc/ak"
#endif

#ifndef AK_BITS
#define AK_BITS                 10
#endif

#ifndef AK_LIMIT
#define AK_LIMIT                ((1<<AK_BITS) - 1)  // 1023
#endif

#ifndef ENV_AK_DEBUG
#define ENV_AK_DEBUG            "__AK_DEBUG__"
#endif

#ifndef ENV_JS_DEBUG
#define ENV_JS_DEBUG            "__JS_DEBUG__"
#endif

#ifndef AK_DEBUG_NAME
#define AK_DEBUG_NAME           "debug"
#endif

#ifndef JS_DEBUG_NAME
#define JS_DEBUG_NAME           "jdebug"
#endif

enum { INVALID_AKID = 0};

#ifndef AK_DPRINT
#define AK_DPRINT               0
#endif

#if AK_DPRINT
#define ak_println(_fmt, _args...)  printf(_fmt "\n", ##_args)
#else
#define ak_println(_fmt, _args...)  os_do_nothing()
#endif

typedef uint32 akid_t;

#if defined(__BOOT__)
    extern akid_t *__THIS_DEBUG;
    extern akid_t *__THIS_JDEBUG;
#else
    extern akid_t __THIS_only_debugger;
    extern akid_t __THIS_only_jdebugger;
    extern akid_t __THIS_libjs_debugger;
    extern akid_t __THIS_libjs_jdebugger;
    extern akid_t __THIS_libweos_debugger;
    extern akid_t __THIS_libweos_jdebugger;
#endif
/******************************************************************************/
#define AK_DEBUG_ENUM_MAPPER(_)                 \
    _(____ak_debug_ok,          0, "ok"),       \
    _(____ak_debug_bug,         1, "bug"),      \
    _(____ak_debug_error,       2, "error"),    \
    _(____ak_debug_trace,       3, "trace"),    \
    _(____ak_debug_init,        4, "init"),     \
    _(____ak_debug_config,      5, "config"),   \
    _(____ak_debug_json,        6, "json"),     \
    _(____ak_debug_io,          7, "io"),       \
    _(____ak_debug_timer,       8, "timer"),    \
    _(____ak_debug_timeout,     9, "timeout"),  \
    _(____ak_debug_aging,       10,"aging"),    \
    _(____ak_debug_event,       11,"event"),    \
    _(____ak_debug_entry,       12,"entry"),    \
    _(____ak_debug_packet,      13,"packet"),   \
    _(____ak_debug_proto,       14,"proto"),    \
    _(____ak_debug_flow,        15,"flow"),     \
    _(____ak_debug_file,        16,"file"),     \
    _(____ak_debug_signal,      17,"signal"),   \
    _(____ak_debug_shell,       18,"shell"),    \
    _(____ak_debug_gc,          19,"gc"),       \
    _(____ak_debug_js,          20,"js"),       \
    _(____ak_debug_format,      21,"format"),   \
    _(____ak_debug_cli,         22,"cli"),      \
    _(____ak_debug_loop,        23,"loop"),     \
    _(____ak_debug_r1,          24,"r1"),       \
    _(____ak_debug_r2,          25,"r2"),       \
    _(____ak_debug_r3,          26,"r3"),       \
    _(____ak_debug_r4,          27,"r4"),       \
    _(____ak_debug_test,        28,"test"),     \
    /* end */
    /* 29/30/31 reserved for log pri */

EXTERN_ENUM(ak_DEBUG, AK_DEBUG_ENUM_MAPPER, ____ak_debug_end);

EXTERN enum_ops_t *ak_DEBUG_ops(void);
EXTERN bool is_good_ak_DEBUG(int id);
EXTERN char *ak_DEBUG_getnamebyid(int id);
EXTERN int ak_DEBUG_getidbyname(const char *name);

enum {
    __ak_debug_ok           = os_bit(____ak_debug_ok),
    __ak_debug_bug          = os_bit(____ak_debug_bug),
    __ak_debug_error        = os_bit(____ak_debug_error),
    __ak_debug_trace        = os_bit(____ak_debug_trace),
    __ak_debug_init         = os_bit(____ak_debug_init),
    __ak_debug_config       = os_bit(____ak_debug_config),
    __ak_debug_json         = os_bit(____ak_debug_json),
    __ak_debug_io           = os_bit(____ak_debug_io),
    __ak_debug_timer        = os_bit(____ak_debug_timer),
    __ak_debug_timeout      = os_bit(____ak_debug_timeout),
    __ak_debug_aging        = os_bit(____ak_debug_aging),
    __ak_debug_event        = os_bit(____ak_debug_event),
    __ak_debug_entry        = os_bit(____ak_debug_entry),
    __ak_debug_packet       = os_bit(____ak_debug_packet),
    __ak_debug_proto        = os_bit(____ak_debug_proto),
    __ak_debug_flow         = os_bit(____ak_debug_flow),
    __ak_debug_file         = os_bit(____ak_debug_file),
    __ak_debug_signal       = os_bit(____ak_debug_signal),
    __ak_debug_shell        = os_bit(____ak_debug_shell),
    __ak_debug_gc           = os_bit(____ak_debug_gc),
    __ak_debug_js           = os_bit(____ak_debug_js),
    __ak_debug_format       = os_bit(____ak_debug_format),
    __ak_debug_cli          = os_bit(____ak_debug_cli),
    __ak_debug_loop         = os_bit(____ak_debug_loop),
    __ak_debug_test         = os_bit(____ak_debug_test),

    __ak_debug_all          = os_mask(____ak_debug_end),
};

#ifndef __ak_debug_default
#define __ak_debug_default          (__ak_debug_error | __ak_debug_bug)
#endif

#ifndef __js_debug_default
#define __js_debug_default          0
#endif

#ifndef __ak_debug_string_default
#define __ak_debug_string_default   "bug"
#endif

#ifndef __js_debug_string_default
#define __js_debug_string_default   "0"
#endif

static inline uint32
__ak_debug_getbyname(char *name)
{
    int idx = ak_DEBUG_getidbyname(name);

    return os_bit(idx);
}

static inline char *
__ak_debug_getname(uint32 level)
{
    int idx = os_bitshift(level);

    return ak_DEBUG_getnamebyid(idx);
}

#if defined(__BOOT__)
#   define __ak_debug       (__THIS_DEBUG?(*__THIS_DEBUG):__ak_debug_default)
#   define __js_debug       (__THIS_JDEBUG?(*__THIS_JDEBUG):__js_debug_default)
#elif defined(__APP__)
#   if __RUNAS__==RUN_AS_COMMAND
#       define __ak_debug   __THIS_DEBUG
#       define __js_debug   __THIS_JDEBUG
#   else /* run as deamon/unknow */
#       define __ak_debug   ak_get(__THIS_DEBUG, __ak_debug_default)
#       define __js_debug   ak_get(__THIS_JDEBUG, __js_debug_default)
#   endif
#elif defined(__KERNEL__)
#   define __ak_debug       __THIS_DEBUG
#   define __js_debug       __THIS_JDEBUG
#else
#   error "invalid __THIS_DEBUG"
#   error "invalid __THIS_JDEBUG"
#endif

#define __is_ak_debug(_level)   (os_hasflag(__ak_debug, _level))
#define __is_js_debug(_level)   (os_hasflag(__js_debug, _level))

#define __is_ak_debug_ok        __is_ak_debug(__ak_debug_ok)
#define __is_ak_debug_bug       __is_ak_debug(__ak_debug_bug)
#define __is_ak_debug_error     __is_ak_debug(__ak_debug_error)
#define __is_ak_debug_trace     __is_ak_debug(__ak_debug_trace)
#define __is_ak_debug_init      __is_ak_debug(__ak_debug_init)
#define __is_ak_debug_config    __is_ak_debug(__ak_debug_config)
#define __is_ak_debug_json      __is_ak_debug(__ak_debug_json)
#define __is_ak_debug_io        __is_ak_debug(__ak_debug_io)
#define __is_ak_debug_timer     __is_ak_debug(__ak_debug_timer)
#define __is_ak_debug_timeout   __is_ak_debug(__ak_debug_timeout)
#define __is_ak_debug_aging     __is_ak_debug(__ak_debug_aging)
#define __is_ak_debug_event     __is_ak_debug(__ak_debug_event)
#define __is_ak_debug_entry     __is_ak_debug(__ak_debug_entry)
#define __is_ak_debug_packet    __is_ak_debug(__ak_debug_packet)
#define __is_ak_debug_proto     __is_ak_debug(__ak_debug_proto)
#define __is_ak_debug_flow      __is_ak_debug(__ak_debug_flow)
#define __is_ak_debug_file      __is_ak_debug(__ak_debug_file)
#define __is_ak_debug_signal    __is_ak_debug(__ak_debug_signal)
#define __is_ak_debug_shell     __is_ak_debug(__ak_debug_shell)
#define __is_ak_debug_gc        __is_ak_debug(__ak_debug_gc)
#define __is_ak_debug_js        __is_ak_debug(__ak_debug_js)
#define __is_ak_debug_format    __is_ak_debug(__ak_debug_format)
#define __is_ak_debug_cli       __is_ak_debug(__ak_debug_cli)
#define __is_ak_debug_loop      __is_ak_debug(__ak_debug_loop)
#define __is_ak_debug_test      __is_ak_debug(__ak_debug_test)

#if 1
#define AK_SYS_ENUM_MAPPER(_)               \
    _(__AK_SYS_DEBUG,   0, AK_DEBUG_NAME),  \
    _(__AK_SYS_JDEBUG,  1, JS_DEBUG_NAME),  \
    /* end */
EXTERN_ENUM(ak_sys, AK_SYS_ENUM_MAPPER, __AK_SYS_END);

EXTERN enum_ops_t *ak_sys_ops(void);
EXTERN bool is_good_ak_sys(int id);
EXTERN char *ak_sys_getnamebyid(int id);
EXTERN int ak_sys_getidbyname(const char *name);

#define __AK_SYS_DEBUG  __AK_SYS_DEBUG
#define __AK_SYS_JDEBUG __AK_SYS_JDEBUG
#define __AK_SYS_END    __AK_SYS_END
#endif

EXTERN uint32
__ak_sys_debug(char *var);

EXTERN uint32
__ak_sys_value(int sys, char *line);

EXTERN int
__ak_get_value(char *key, char *value);
/******************************************************************************/
extern bool __THIS_COMMAND;

static inline int __ak_init(void);

#if !defined(__APP__) || __RUNAS__==RUN_AS_COMMAND

#define __ak_getidbyname(_app, _key)    0
#define __ak_getidbynameEx(_app, _key)  0
#define __ak_getnamebyid(_akid)         NULL
#define __ak_getkeybyid(_akid)          NULL
#define __ak_getvaluebyid(_akid)        NULL
#define __ak_setvaluebyid(_akid, _v)    0

/*
* kernel/boot/(app cmd)
*/
#define ak_getidbyname(_key)            0
#define ak_getidbynameEx(_key)          0

#define ak_get(_akid, _deft)            (_akid)
#define ak_set(_akid, _value)           0

#define ak_load()                       0
#define ak_foreach(_foreach)            0
#define ak_fini()                       0

static inline int 
ak_init(void)
{    
    __ak_init();

    return 0;
}
#else /* app unknow/deamon */
extern akid_t 
__ak_getidbyname(char *app, char *k);

extern akid_t 
__ak_getidbynameEx(char *app, char *k);

extern char *
__ak_getnamebyid(akid_t akid);

extern char * 
__ak_getkeybyid(akid_t akid);

extern uint32 * 
__ak_getvaluebyid(akid_t akid);

extern int 
__ak_setvaluebyid(akid_t akid, uint32 v);

static inline akid_t 
ak_getidbyname(char *k)
{
    if (__THIS_COMMAND) {
        return 0;
    } else {
        return __ak_getidbyname(__THIS_APPNAME, k);
    }
}

static inline akid_t 
ak_getidbynameEx(char *k)
{
    if (__THIS_COMMAND) {
        return 0;
    } else {
        return __ak_getidbynameEx(__THIS_APPNAME, k);
    }
}

static inline uint32
ak_get(akid_t akid, uint32 deft)
{
    if (__THIS_COMMAND) {
        return akid;
    } else {
        uint32 *p = __ak_getvaluebyid(akid);

        return p?(*p):deft;
    }
}

static inline int 
ak_set(akid_t akid, uint32 v)
{
    if (false==__THIS_COMMAND) {
        return __ak_setvaluebyid(akid, v);
    } else {
        return 0;
    }
}

extern int 
ak_load(void);

extern int
ak_foreach(mv_t (*foreach)(char *app, char *k, uint32 v));

extern int 
ak_fini(void);

extern int 
ak_init_helper(void);

static inline int 
ak_init(void) 
{
    int err = ak_init_helper();
    if(err<0) {
        goto error;
    }
    
    __ak_init();
    
    ak_println("init OK!");
    os_println(__THIS_APPNAME " " __THIS_FILENAME " ak init OK!");
    
    return 0;
error:
    ak_println("init failed!");
    
    ak_fini();

    return err;
}

#endif /* !defined(__APP__) || defined(__COMMAND__) */

#if defined(__APP__) && (__RUNAS__ & RUN_AS_COMMAND)
static inline void 
__ak_init_command() 
{
    char *value;
    
    value = env_gets(ENV_AK_DEBUG, __ak_debug_string_default);
    __THIS_DEBUG = __ak_get_value(AK_DEBUG_NAME, value);
    ak_println("__THIS_DEBUG=%s==>0x%x", value, __THIS_DEBUG);

    value = env_gets(ENV_JS_DEBUG, __js_debug_string_default);
    __THIS_JDEBUG = __ak_get_value(JS_DEBUG_NAME, value);
    ak_println("__THIS_JDEBUG=%s==>0x%x", value, __THIS_JDEBUG);
}
#endif

#if defined(__APP__) && (__RUNAS__ & RUN_AS_DEAMON)
static inline void 
__ak_init_deamon() 
{
    __THIS_DEBUG    = ak_getidbyname(AK_DEBUG_NAME);
    __THIS_JDEBUG   = ak_getidbyname(JS_DEBUG_NAME);
}
#endif

#if defined(__APP__) && (__RUNAS__==RUN_AS_UNKNOW)
static inline void 
__ak_init_unknow() 
{
    char *value = getenv(OS_ENV(DEAMON));
    
    __THIS_COMMAND = (NULL==value);
    ak_println(OS_ENV(DEAMON) "=%s, __THIS_COMMAND=%d", value, __THIS_COMMAND);

    if (__THIS_COMMAND) {
        __ak_init_command();
    } else {
        __ak_init_deamon();
    }
}
#endif

static inline int 
__ak_init(void)
{
    ak_println("ak runas %d", __RUNAS__);
    
#ifdef __APP__
#if __RUNAS__==RUN_AS_COMMAND
    __ak_init_command();
#elif __RUNAS__==RUN_AS_DEAMON
    __ak_init_deamon();
#elif __RUNAS__==RUN_AS_UNKNOW
    __ak_init_unknow();
#else
#   error "bad __RUNAS__"
#endif
#endif

    return 0;
}
/******************************************************************************/
typedef struct {
    akid_t id;
    char *key;
    uint32 deft;
} ak_var_t;

#define AK_VAR_INITER(_key, _deft)  { \
    .id = INVALID_VALUE,    \
    .key = _key,            \
    .deft = _deft,          \
}

static inline int
ak_var_init(ak_var_t *var)
{
    if (INVALID_VALUE==var->id) {
        var->id = ak_getidbyname(var->key);

        return 0;
    } else {
        return -EREINIT;
    }
}

static inline uint32
ak_var_get(ak_var_t *var)
{
    return ak_get(var->id, var->deft);
}

static inline int 
ak_var_set(ak_var_t *var, uint32 v)
{
    return ak_set(var->id, v);
}

#ifdef __BOOT__
#include "weos/boot/ak.c"
#endif
/******************************************************************************/
#endif /* __AK_H_2e420d20ac8a47b188d92ef8448db5fa__ */
