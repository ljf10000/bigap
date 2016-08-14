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

static inline char *
env_gets(char *envname, char *deft) 
{
    if (NULL==envname) {
        return os_assertV(deft);
    }
    
    char *env = getenv(envname);
    if (false==is_good_env(env)) {
        env_println("no-found env:%s", envname);
        
        return deft;
    } else {
        env_println("get env:%s=%s", envname, env);
        
        return env;
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

#define __ENV_VAR_INITER(_name, _deft) { \
    .name = _name, \
    .deft = _deft, \
    .value= _deft, \
}   /* end */

#define ENV_VAR_INITER(_var)   __ENV_VAR_INITER("__" #_var "__", _var)

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
#define OS_ENV(_name)   "__ENV_" #_name "__"

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
    char *UNIX;     // jlogd/jlogger
    char *family;   // jlogger
} 
env_cache_t;
/******************************************************************************/
#endif /* __ENV_H_9f26434e76294326b152b1e81f17fb31__ */
