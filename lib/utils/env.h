#ifndef __ENV_H_9f26434e76294326b152b1e81f17fb31__
#define __ENV_H_9f26434e76294326b152b1e81f17fb31__
#ifdef __APP__
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

static inline int
envs_count(char *env[])
{
    int count;

    if (NULL==env) {
        return 0;
    }
    
    for (count=0; env[count]; count++) {
        os_do_nothing();
    }

    return count;
}

static inline void
envs_append(char *dst[], char *src[])
{
    int i, count = envs_count(dst);

    for (i=0; src[i]; i++) {
        dst[i+count] = src[i];
    }
}

extern char **environ;

static inline void
envs_clone(char *env[])
{
    envs_append(env, environ);
}

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

static inline char *
env_gets(char *envname, char *deft) 
{
    if (envname) {
        char *env = getenv(envname);
        
        if (is_good_env(env)) {
            env_println("get env:%s=%s", envname, env);
            
            return env;
        } else {
            env_println("no-found env:%s, use default:%s", envname, deft);

            return deft;
        }
    } else {
        env_println("empty env, use default:%s", deft);

        return deft;
    }
}

static inline int
__env_copy(char *envname, char *deft, char s[], int size) 
{
    char *env = env_gets(envname, deft);
    if (NULL==env) {
        return 0;
    }
    else if (os_strlen(env) > size - 1) {
        return os_assertV(-ETOOBIG);
    }
    
    os_strlcpy(s, env, size);
    
    return 0;
}

#define env_copy(_envname, _deft, _string)              ({  \
    BUILD_BUG_NOT_ARRAY(_string);                           \
    __env_copy(_envname, _deft, _string, sizeof(_string));  \
})

static inline int
env_geti(char *envname, int deft) 
{
    if (NULL==envname) {
        return os_assertV(deft);
    }
    
    char *env = getenv(envname);
    if (false==is_good_env(env)) {
        env_println("no-found env:%s, use default:%d", envname, deft);
        
        return deft;
    } else {
        int value = os_atoi(env);

        env_println("get env:%s=%d", envname, value);

        return value;
    }
}
/******************************************************************************/
#define use_THIS_ENV    0

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
DECLARE_ENUM(os_env, OS_ENV_ENUM_MAPPER, OS_ENV_END);

static inline bool is_good_os_env(int type);
static inline char *os_env_string(int type);
static inline int os_env_idx(char *type_string);

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

#define DECLARE_FAKE_ENV    extern os_env_cache_t *__THIS_ENV
#define DECLARE_REAL_ENV    os_env_cache_t *__THIS_ENV

#ifdef __ALLINONE__
#   define DECLARE_ENV      DECLARE_FAKE_ENV
#else
#   define DECLARE_ENV      DECLARE_REAL_ENV
#endif

DECLARE_FAKE_ENV;

static inline os_env_cache_t *
__this_env(void)
{
    if (NULL==__THIS_ENV) {
        __THIS_ENV = (os_env_cache_t *)os_zalloc(sizeof(os_env_cache_t));
    }

    return __THIS_ENV;
}

static inline void
os_env_init(void)
{
#if use_THIS_ENV
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
/******************************************************************************/
#endif
#endif /* __ENV_H_9f26434e76294326b152b1e81f17fb31__ */
