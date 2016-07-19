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
#define ENV_TIMEOUT                 "__ENV_TIMEOUT__"
#define ENV_PWDFILE                 "__ENV_PWDFILE__"
#define ENV_VERSION                 "__ENV_VERSION__"
#define ENV_UPGRADE                 "__ENV_UPGRADE__"   /* upgrade count */
#define ENV_ROOTFS                  "__ENV_ROOTFS__"    /* upgrade rootfs idx */
#define ENV_SERVER                  "__ENV_SERVER__"
#define ENV_FORCE                   "__ENV_FORCE__"
#define ENV_PORT                    "__ENV_PORT__"
#define ENV_USER                    "__ENV_USER__"
#define ENV_PATH                    "__ENV_PATH__"
#define ENV_RESETBY                 "__ENV_RESETBY__"
#define ENV_PASSWORD                "__ENV_PASSWORD__"
#define ENV_TELEPHONE               "__ENV_TELEPHONE__"
#define ENV_APN                     "__ENV_APN__"

typedef struct {
    char *timeout;
    char *pwdfile;
    char *version;
    char *upgrade;
    char *rootfs;
    char *server;
    char *force;
    char *port;
    char *user;
    char *path;
    char *resetby;
    char *password;
    char *telephone;
    char *apn;
} env_cache_t;
/******************************************************************************/
#endif /* __ENV_H_9f26434e76294326b152b1e81f17fb31__ */
