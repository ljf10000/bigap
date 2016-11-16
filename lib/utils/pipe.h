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

typedef int os_pexec_callback_f(int error, char *outsring, char *errstring);

typedef struct {
    simple_buffer_t sb;

    int fd[2];      /* father/son */
} pipe_std_t;

#define PIPE_STD_INITER(_size, _minsize, _expand) { \
    .sb = SB_INITER(_size, _minsize, _expand, SB_F_EXPAND_AUTO), \
    .fd     = {INVALID_FD, INVALID_FD}, \
}   /* end */

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

#define __PIPEINFO_INITER(_env, _timeout, _cb, _size, _minsize, _expand) { \
    .env    = _env,                     \
    .timeout= PIPE_TIMEOUT(_timeout),   \
    .cb     = _cb,                      \
    .size   = PIPE_EXPAND(_size),       \
    .minsize= PIPE_EXPAND(_minsize),    \
    .expand = PIPE_EXPAND(_expand),     \
}   /* end */

#define PIPEINFO_INITER(_env, _cb) \
    __PIPEINFO_INITER(_env, 0, _cb, 0, 0, 0)

typedef struct {
    pipe_std_t std[3];  // std in/out/err
    pipinfo_t info;
    
    int err;
} pipexec_t;

#define PIPEXEC_INITER(_size, _minsize, _expand) {   \
    .std = {                    \
        [1]  = PIPE_STD_INITER(_size, _minsize, _expand), \
        [2]  = PIPE_STD_INITER(_size, _minsize, _expand), \
    },                          \
    .err = 0,                   \
}   /* end */

enum {
    __pipe_father   = 0,
    __pipe_son      = 1,
};

static inline char *
__pipe_std_cursor(pipe_std_t *std)
{
    return sb_cursor(&std->sb);
}

static inline uint32
__pipe_std_left(pipe_std_t *std)
{
    return sb_left(&std->sb);
}

static inline int
__pipe_std_expand(pipe_std_t *std)
{
    return sb_expand(&std->sb, 0);
}

static inline int
__pipe_std_read(pipe_std_t *std)
{
    int err, len, left;

    while(1) {
        left = __pipe_std_left(std);
        
        len = read(std->fd[__pipe_father], __pipe_std_cursor(std), left);
        if (len<0) {
            return -errno;
        }
        else if (len < left) {
            std->sb.len += len;
            
            return len;
        }

        err = __pipe_std_expand(std);
        if (err<0) {
            return err;
        }
    }

    return -ENOMEM;
}

static inline int
__pipexec_init(pipexec_t *pe, pipinfo_t *info)
{
    int err;
    char **env;
    
    os_objcpy(&pe->info, info);
    
    err = sb_init(&pe->std[1].sb, 0, 0, 0, 0);
    if (err<0) {
        return err;
    }

    err = sb_init(&pe->std[2].sb, 0, 0, 0, 0);
    if (err<0) {
        return err;
    }
    
    err = pipe(pe->std[1].fd);
    if (err<0) {
        return -errno;
    }
    
    err = pipe(pe->std[2].fd);
    if (err<0) {
        return -errno;
    }

    return 0;
}

static inline void
__pipexec_fini(pipexec_t *pe)
{
    if (pe) {
#if 1
        os_close(pe->std[1].fd[__pipe_father]);
        os_close(pe->std[2].fd[__pipe_father]);
        
        os_close(pe->std[1].fd[__pipe_son]);
        os_close(pe->std[2].fd[__pipe_son]);
#endif
        sb_fini(&pe->std[1].sb);
        sb_fini(&pe->std[2].sb);
    }
}

static inline bool
__p_father_wait(pipexec_t *pe, int son)
{
    int err, status = 0;
    int pid = waitpid(son, &status, WNOHANG);

    if (pid==son) {
        err = pe->err = __os_wait_error(status);

        os_noblock(pe->std[1].fd[__pipe_father]);
        __pipe_std_read(&pe->std[1]);

        os_noblock(pe->std[2].fd[__pipe_father]);
        __pipe_std_read(&pe->std[2]);

        return true;
    }
    else {
        return false;
    }
}

static inline int
__p_father_handle(pipexec_t *pe, int son)
{
    struct timeval tv = {
        .tv_sec     = os_second(pe->info.timeout),
        .tv_usec    = os_usecond(pe->info.timeout),
    };
    fd_set rset;
    
    int max = OS_MAX(pe->std[1].fd[__pipe_father], pe->std[2].fd[__pipe_father]);
    int err = 0;

    os_close(pe->std[1].fd[__pipe_son]);
    os_close(pe->std[2].fd[__pipe_son]);
    
    while(1) {        
        FD_ZERO(&rset);
        FD_SET(pe->std[1].fd[__pipe_father], &rset);
        FD_SET(pe->std[2].fd[__pipe_father], &rset);

        int err = select(1+max, &rset, NULL, NULL, &tv);
        switch(err) {
            case -1:/* error */
                if (EINTR==errno) {
                    break;
                } else {
                    err = -errno; goto error;
                }
            case 0: /* timeout, retry */
                err = -ETIMEOUT; goto error;
            default: /* to accept */
                break;
        }

        if (FD_ISSET(pe->std[1].fd[__pipe_father], &rset)) {
            err = __pipe_std_read(&pe->std[1]);
            if (err<0) {
                goto error;
            }
        }
        
        if (FD_ISSET(pe->std[2].fd[__pipe_father], &rset)) {
            err = __pipe_std_read(&pe->std[2]);
            if (err<0) {
                goto error;
            }
        }

        if (__p_father_wait(pe, son)) {
            err = 0; goto error;
        }
    }

error:
    os_close(pe->std[1].fd[__pipe_father]);
    os_close(pe->std[2].fd[__pipe_father]);

    return err;
}

static inline int
__p_son_handle(pipexec_t *pe)
{
    pipinfo_t *info = &pe->info;
    int err, count;

    // re-link 1/2
    close(1);
    close(2);

    dup2(pe->std[1].fd[__pipe_son], 1);
    dup2(pe->std[2].fd[__pipe_son], 2);
    
    os_close(pe->std[1].fd[__pipe_father]);
    os_close(pe->std[2].fd[__pipe_father]);
    os_close(pe->std[1].fd[__pipe_son]);
    os_close(pe->std[2].fd[__pipe_son]);

    // append env(private + global)
    
    info->env = envs_merge(environ, info->env);
    envs_dump("env", info->env, os_println);
    
    if (info->content) {
        char *argv[] = {"bash", "-c", info->content, NULL};

        envs_dump("current argv", argv, os_println);
        
        execvpe("/bin/bash", argv, info->env);
    }
    else if (info->file) {
        char *argv[] = {os_basename(info->file), NULL};
        
        envs_dump("old argv", info->argv, os_println);
        envs_dump("new argv", argv, os_println);
        
        info->argv = envs_merge(info->argv, argv);
        envs_dump("current argv", info->argv, os_println);

        execvpe(info->file, info->argv, info->env);
    }
    else {
        return -ENOSUPPORT;
    }
    
    // go here is error
    return -errno;
}

/*
* use file and argv, ignore content
*/
static inline int
os_pexecv(pipinfo_t *info)
{
    pipexec_t pe = PIPEXEC_INITER(info->size, info->minsize, info->expand);
    int err = 0, pid = 0;
    
    if (NULL==info) {
        return -EINVAL0;
    }
    else if (NULL==info->cb) {
        return -EINVAL1;
    }
    
    __pipexec_init(&pe, info);
    
    pid = fork();
    if (pid<0) {
        err = -errno;
    }
    else if (pid>0) { // father
        err = __p_father_handle(&pe, pid);
        if (0==err) {
            (*info->cb)(pe.err, pe.std[1].sb.buf, pe.std[2].sb.buf);
        }
    }
    else { // child
        err = __p_son_handle(&pe);
    }

error:
    __pipexec_fini(&pe);

    return err;
}

static inline int
os_vpexec(pipinfo_t *info, const char *fmt, va_list args)
{
    int err = 0;
    char *cmd = NULL;
    pipinfo_t clone;

    err = os_vasprintf(&cmd, fmt, args);
    if (err>0) {
        os_objcpy(&clone, info);

        clone.content = cmd;

        err = os_pexecv(&clone);
    }
    os_free(cmd);

    return err;
}

/*
* auto create content, ignore file and argv
*/
static inline int
os_pexec(pipinfo_t *info, const char *fmt, ...)
{
    va_list args;

    va_start(args, (char *)fmt);
    int err = os_vpexec(info, fmt, args);
    va_end(args);

    return err;
}

static inline int
os_pexecline(pipinfo_t *info, char *line)
{
    int err, count = os_strcount(line, ' ');
    char *input = os_strdup(line);
    char *argv[1+count];
    char *p;
    
    count = 0;
    os_strtok_foreach(p, input, " ") {
        argv[count++] = p;
    }
    argv[count] = NULL;
    
    if (os_file_exist(argv[0])) {
        info->file = argv[0];
        info->argv = &argv[1];

        err = os_pexecv(info);
    } else {
        info->content = line;
        
        err = os_pexec(info, "%s", line);
    }
    
    os_free(input);

    return 0;
}

/******************************************************************************/
#endif
#endif /* __PIPE_H_0cb88994d86a4261ad2f10f1e0525dcf__ */
