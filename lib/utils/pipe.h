#ifndef __PIPE_H_0cb88994d86a4261ad2f10f1e0525dcf__
#define __PIPE_H_0cb88994d86a4261ad2f10f1e0525dcf__
#ifdef __APP__
/******************************************************************************/
#ifndef PIPE_EXPAND_MIN
#define PIPE_EXPAND_MIN         (4*1024)
#endif

#ifndef PIPE_EXPAND_MAX
#define PIPE_EXPAND_MAX         (256*1024)
#endif

#ifndef PIPE_EXPAND_DEFT
#define PIPE_EXPAND_DEFT        (32*1024)
#endif

#ifndef PIPE_TIMEOUT_MIN
#define PIPE_TIMEOUT_MIN        PC_VAL(2000, 3000)  // 3s
#endif

#ifndef PIPE_TIMEOUT_MAX
#define PIPE_TIMEOUT_MAX        PC_VAL(5000, 30000) // 30s
#endif

#ifndef PIPE_TIMEOUT_DEFT
#define PIPE_TIMEOUT_DEFT       PC_VAL(3000, 10000) // 10s
#endif

#define PIPE_TIMEOUT(_timeout)  \
    OS_SAFE_VALUE_DEFT(_timeout, PIPE_TIMEOUT_MIN, PIPE_TIMEOUT_MAX, PIPE_TIMEOUT_DEFT)

#define PIPE_EXPAND(_expand)        \
    OS_SAFE_VALUE_DEFT(_expand, PIPE_EXPAND_MIN, PIPE_EXPAND_MAX, PIPE_EXPAND_DEFT)

#ifndef PIPE_DPRINT
#define PIPE_DPRINT             0
#endif

#if FILE_DPRINT
#define pipe_println(_fmt, _args...)    os_println(_fmt, ##_args)
#else
#define pipe_println(_fmt, _args...)    os_do_nothing()
#endif

typedef int os_pexec_callback_f(int error, char *outsring, char *errstring);
/*
* 1. use file+argv or content
* 2. use env or NULL
* 3. use file or content
*/
typedef struct {
    char **env;     // new env, not include sys env
    char **argv;    // like shell, just argument, not include appname
    char *file;     
    char *content;
    
    int timeout;        // ms

    uint32 size;
    uint32 minsize;
    uint32 expand;

    os_pexec_callback_f *cb;
} pipinfo_t;

#define __PIPINFO_INITER(_env, _timeout, _cb, _size, _minsize, _expand) { \
    .env    = _env,                     \
    .timeout= PIPE_TIMEOUT(_timeout),   \
    .cb     = _cb,                      \
    .size   = PIPE_EXPAND(_size),       \
    .minsize= PIPE_EXPAND(_minsize),    \
    .expand = PIPE_EXPAND(_expand),     \
}   /* end */

#define PIPINFO_INITER(_env, _cb)       \
    __PIPINFO_INITER(_env, 0, _cb, 0, 0, 0)

#define dump_pipinfo(_info, _dump)          do{ \
    _dump("size=%u", (_info)->size);            \
    _dump("minsize=%u", (_info)->minsize);      \
    _dump("expand=%u", (_info)->expand);        \
    _dump("timeout=%d", (_info)->timeout);      \
    if ((_info)->content) {                     \
        _dump("content=%d", (_info)->content);  \
    }                                           \
    if ((_info)->file) {                        \
        _dump("file=%d", (_info)->file);        \
    }                                           \
    if ((_info)->env) {                         \
        envs_dump("old", (_info)->env, _dump);  \
    }                                           \
    if ((_info)->argv) {                        \
        envs_dump("old", (_info)->argv, _dump); \
    }                                           \
}while(0)

extern int
os_pexecv(pipinfo_t *info);

extern int
os_vpexec(pipinfo_t *info, const char *fmt, va_list args);

/*
* auto create content, ignore file and argv
*/
extern int
os_pexec(pipinfo_t *info, const char *fmt, ...);

extern int
os_pexecline(pipinfo_t *info, char *line);

extern int 
os_pexec_jcallback(int error, char *outstring, char *errstring);

extern int
os_pexec_jmap(pipinfo_t *info, char *json);

static inline void
os_pexec_clean(pipinfo_t *info)
{
    os_free(info->content);
    os_free(info->file);
    
    if (info->argv) {
        char *args;

        for (args = info->argv[0]; args; args++) {
            free(args); // NOT os_free
        }

        os_free(info->argv);
    }
}

static inline int
os_pexec_json(char *json)
{
    pipinfo_t info = PIPINFO_INITER(NULL, os_pexec_jcallback);
    int err;

    err = os_pexec_jmap(&info, json);
    if (err<0) {
        goto error;
    }

    err = os_pexecv(&info);
    if (err>0) {
        goto error;
    }

error:
    os_pexec_clean(&info);
    
    return err;
}
/******************************************************************************/
#endif
#endif /* __PIPE_H_0cb88994d86a4261ad2f10f1e0525dcf__ */
