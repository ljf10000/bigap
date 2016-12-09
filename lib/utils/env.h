#ifndef __ENV_H_9f26434e76294326b152b1e81f17fb31__
#define __ENV_H_9f26434e76294326b152b1e81f17fb31__
/******************************************************************************/
#ifndef ENV_PRINT
#define ENV_PRINT   0
#endif

#if ENV_PRINT
#define env_println(_fmt, _args...)     os_println(_fmt, ##_args)
#else
#define env_println(_fmt, _args...)     os_do_nothing()
#endif

#define is_good_env(_env)               is_good_str(_env)

#ifndef ENV_JPATH
#define ENV_JPATH           "JPATH"
#endif

#ifndef ENV_JCACHE
#define ENV_JCACHE          "JCACHE"
#endif

#ifndef js_CACHE
#define js_CACHE            "/tmp/jcache"
#endif

#ifndef js_PATH
#define js_PATH             "/tmp/js:/lib/js"
#endif

#define __argv_dump(_dump, _argc, _argv) ({ \
    int i;                                  \
                                            \
    for (i=0; i<_argc; i++) {               \
        _dump("function:%s argv[%d]=%s",    \
            __func__, i, _argv[i]);         \
    }                                       \
                                            \
    _argv[0];                               \
})

#define argv_dump(_argc, _argv)     __argv_dump(debug_trace, _argc, _argv)

EXTERN int
envs_count(char *env[]);

EXTERN void
envs_append(char *dst[], char *src[]);

extern char **environ;

static inline void
envs_clone(char *env[])
{
    envs_append(env, environ);
}

EXTERN int 
zip2line_helper(char line[], int size, char *s, int sep);
#define zip2bin_helper(_line, _size, _s)    zip2line_helper(_line, _size, _s, 0)
#define zip2str_helper(_line, _size, _s)    zip2line_helper(_line, _size, _s, ' ')

EXTERN int 
zip2line(char buf[], int size, char *ss[], bool (*is_good)(int idx), int sep);

EXTERN int
argv_zip2line(char buf[], int size, int argc, char *argv[], int sep);
#define argv_zip2bin(_bin, _size, _argc, _argv) argv_zip2line(_bin, _size, _argc, _argv, 0)
#define argv_zip2str(_str, _size, _argc, _argv) argv_zip2line(_str, _size, _argc, _argv, ' ')

EXTERN int
envs_zip2line(char buf[], int size, char *env[], int sep);
#define envs_zip2bin(_bin, _size, _env)     envs_zip2line(_bin, _size, _env, 0)
#define envs_zip2str(_str, _size, _env)     envs_zip2line(_str, _size, _env, ' ')

EXTERN int
argv_unzipbin(char buf[], int argc, char *argv[]);

EXTERN int
envs_unzipbin(char buf[], int count, char *env[]);

/*
* new first
*/
#define envs_merge(_old, _new)          ({  \
    char **__array = NULL;                  \
    int __count = envs_count(_old);         \
    if (__count) {                          \
        __count += 1 + envs_count(_new);    \
        __array = (char **)os_zalloc(sizeof(char *) * __count); \
        if (__array) {                      \
            envs_append(__array, _new);     \
            envs_append(__array, _old);     \
        }                                   \
    } else {                                \
        __array = _new;                     \
    }                                       \
    __array;                                \
})  /* end */

#define envs_dump(_tag, _env, _dump)    do{ \
    int __i;                                \
                                            \
    for (__i=0; _env[__i]; __i++) {         \
        _dump("%s[%d]:%s", _tag, __i, _env[__i]); \
    }                                       \
}while(0)

EXTERN char *
env_gets(char *envname, char *deft) ;

EXTERN int
__env_copy(char *envname, char *deft, char s[], int size) ;

#define env_copy(_envname, _deft, _string)              ({  \
    BUILD_BUG_NOT_ARRAY(_string);                           \
    __env_copy(_envname, _deft, _string, sizeof(_string));  \
})

EXTERN int
env_geti(char *envname, int deft) ;
/******************************************************************************/
#if 1
#define OS_ENV_ENUM_MAPPER(_)           \
    _(ENV_TIMEOUT,  0,  "timeout"),     \
    _(ENV_INTERVAL, 1,  "interval"),    \
    _(ENV_PWDFILE,  2,  "pwdfile"),     \
    _(ENV_VERSION,  3,  "version"),     \
    _(ENV_UPGRADE,  4,  "upgrade"),     \
    _(ENV_ROOTFS,   5,  "rootfs"),      \
    _(ENV_SERVER,   6,  "server"),      \
    _(ENV_FORCE,    7,  "force"),       \
    _(ENV_PORT,     8,  "port"),        \
    _(ENV_USER,     9,  "user"),        \
    _(ENV_PATH,     10, "path"),        \
    _(ENV_RESETBY,  11, "resetby"),     \
    _(ENV_PASSWORD, 12, "password"),    \
    _(ENV_TELEPHONE,13, "telephone"),   \
    _(ENV_APN,      14, "apn"),         \
    _(ENV_TRY,      15, "try"),         \
    _(ENV_SCRIPT,   17, "script"),      \
    _(ENV_STANTARD, 18, "stantard"),    \
    _(ENV_LOW,      19, "low"),         \
    _(ENV_HIGH,     20, "high"),        \
    _(ENV_UNIX,     21, "unix"),        \
    _(ENV_FAMILY,   22, "family"),      \
    _(ENV_GC,       23, "gc"),          \
    _(ENV_CONF,     24, "conf"),        \
    /* end */
EXTERN_ENUM(os_env, OS_ENV_ENUM_MAPPER, OS_ENV_END);

EXTERN enum_ops_t *os_env_ops(void);
EXTERN bool is_good_os_env(int id);
EXTERN char *os_env_getnamebyid(int id);
EXTERN int os_env_getidbyname(const char *name);

#define ENV_TIMEOUT     ENV_TIMEOUT
#define ENV_INTERVAL    ENV_INTERVAL
#define ENV_PWDFILE     ENV_PWDFILE
#define ENV_VERSION     ENV_VERSION
#define ENV_UPGRADE     ENV_UPGRADE
#define ENV_ROOTFS      ENV_ROOTFS
#define ENV_SERVER      ENV_SERVER
#define ENV_FORCE       ENV_FORCE
#define ENV_PORT        ENV_PORT
#define ENV_USER        ENV_USER
#define ENV_PATH        ENV_PATH
#define ENV_RESETBY     ENV_RESETBY
#define ENV_PASSWORD    ENV_PASSWORD
#define ENV_TELEPHONE   ENV_TELEPHONE
#define ENV_APN         ENV_APN
#define ENV_TRY         ENV_TRY
#define ENV_SCRIPT      ENV_SCRIPT
#define ENV_STANTARD    ENV_STANTARD
#define ENV_LOW         ENV_LOW
#define ENV_HIGH        ENV_HIGH
#define ENV_UNIX        ENV_UNIX
#define ENV_FAMILY      ENV_FAMILY
#define ENV_GC          ENV_GC
#define ENV_CONF        ENV_CONF
#define OS_ENV_END      OS_ENV_END
#endif

typedef struct {
    char *env[OS_ENV_END];
} os_env_cache_t;

extern os_env_cache_t *
__this_env(void);

static inline void
os_env_init(void)
{
#if USE_THIS_ENV
    int i;
    
#define OS_ENV_GET(_key, _value, _name) \
    __this_env()->env[_key] = getenv("__env_" _name "__")

    OS_ENV_ENUM_MAPPER(OS_ENV_GET) i=0;
#undef OS_ENV_GET
#endif
}

static inline char *
os_env_get(int idx)
{
    if (is_good_os_env(idx)) {
        return __this_env()->env[idx];
    } else {
        return NULL;
    }
}

static inline char *
os_env_set(int idx, char *env)
{
    if (is_good_os_env(idx)) {
        return (__this_env()->env[idx] = env);
    } else {
        return NULL;
    }
}

static inline char *
os_env_deft(int idx, char *deft)
{
    if (is_good_os_env(idx)) {
        char *env = os_env_get(idx);
        
        if (false==is_good_env(env)) {
            env_println("no-found env:%s, use default:%s", env, deft);
            
            return os_env_set(idx, deft);
        } else {
            env_println("get env:%s=%s", env, env);
            
            return env;
        }
    } else {
        return NULL;
    }
}
/******************************************************************************/
typedef struct {
    char *name;
    char *deft;
    char value[1+OS_LINE_LEN];
} env_string_t;

typedef struct {
    char *name;
    int deft;
    int value;
} env_number_t;

#define ENV_VAR_INITER(_name, _deft) { \
    .name = _name, \
    .deft = _deft, \
    .value= _deft, \
}   /* end */

static inline int
env_string_init(env_string_t *env)
{
    return env_copy(env->name, env->deft, env->value);
}

static inline void
env_number_init(env_number_t *env)
{
    env->value = env_geti(env->name, env->deft);
}
/******************************************************************************/
#define OS_ENV(_name)       "__ENV_" #_name "__"

typedef struct {
    char *timeout;  // sysupgrade/sysrepair/hisi vcc
    char *interval; // hisi vcc
    char *pwdfile;  // sysupgrade/sysrepair
    char *version;  // sysupgrade/sysrepair
    char *upgrade;  // sysupgrade
    char *rootfs;   // sysupgrade
    char *server;   // sysupgrade/sysrepair/jlogd/jlogger
    char *force;    // sysupgrade/sysusbupgrade
    char *port;     // sysupgrade/sysrepair
    char *user;     // sysupgrade/sysrepair/hisi dial
    char *path;     // sysupgrade/sysrepair/hisi vcc
    char *resetby;  // sysreset
    char *password; // hisi dial
    char *telephone;// hisi dial
    char *apn;      // hisi dial
    char *try;      // hisi dial
    char *script;   // hisi acc/vcc
    char *stantard; // hisi voltage
    char *low;      // hisi voltage
    char *high;     // hisi voltage
    char *family;   // jlogger
    char *gc;       // umd
    char *conf;     // *
} 
env_cache_t;

#ifdef __BOOT__
#include "weos/boot/env.c"
#endif
/******************************************************************************/
#endif /* __ENV_H_9f26434e76294326b152b1e81f17fb31__ */
