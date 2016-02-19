#ifndef __PIPE_H_0cb88994d86a4261ad2f10f1e0525dcf__
#define __PIPE_H_0cb88994d86a4261ad2f10f1e0525dcf__
#ifdef __APP__
/******************************************************************************/
typedef struct {
    uint32_t grow, size, len;
    char *buf;
    
    int fd[2];      /* father/son */
} pipe_std_t;

typedef struct {
    pipe_std_t std[3]; /* std in/out/err */
    
    int err;
    int timeout;    /* ms */
} pipe_info_t;

enum {
    __pipe_father   = 0,
    __pipe_son      = 1,
};

#ifndef PIPE_GROW_MIN
#define PIPE_GROW_MIN           (1*1024)
#endif

#ifndef PIPE_GROW_MAX
#define PIPE_GROW_MAX           (32*1024)
#endif

#ifndef PIPE_GROW_DEFT
#define PIPE_GROW_DEFT          (4*1024)
#endif

#ifndef PIPE_STDOUT_GROW
#define PIPE_STDOUT_GROW        PIPE_GROW_DEFT
#endif

#ifndef PIPE_STDERR_GROW
#define PIPE_STDERR_GROW        PIPE_GROW_DEFT
#endif

#ifndef PIPE_TIMEOUT_MIN
#define PIPE_TIMEOUT_MIN        3000    // 3s
#endif

#ifndef PIPE_TIMEOUT_MAX
#define PIPE_TIMEOUT_MAX        30000   // 30s
#endif

#ifndef PIPE_TIMEOUT_DEFT
#define PIPE_TIMEOUT_DEFT       10000   // 10s
#endif

#define PIPE_TIMEOUT(_timeout)  \
    OS_SAFE_VALUE_DEFT(_timeout, PIPE_TIMEOUT_MIN, PIPE_TIMEOUT_MAX, PIPE_TIMEOUT_DEFT)

#define PIPE_GROW(_grow)        \
    OS_SAFE_VALUE_DEFT(_grow, PIPE_GROW_MIN, PIPE_GROW_MAX, PIPE_GROW_DEFT)

#define PIPE_STD_INITER(_buf, _grow)    {   \
    .buf    = (_buf)?(_buf):(char *)os_zalloc(PIPE_GROW(_grow)), \
    .grow   = PIPE_GROW(_grow),             \
    .size   = PIPE_GROW(_grow),             \
    .fd     = {INVALID_FD, INVALID_FD},     \
}

static inline void
__pipe_std_init(pipe_std_t *std, char *buf, int grow)
{
    std->buf    = buf?buf:(char *)os_zalloc(PIPE_GROW(grow));
    std->grow   = PIPE_GROW(grow);
    std->size   = PIPE_GROW(grow);
    std->fd[0]  = INVALID_FD;
    std->fd[1]  = INVALID_FD;
}

#define PIPE_INFO_INITER(_input, _align_out, _align_err, _timeout) {  \
    .std = {                                                \
        [__stdin]   = PIPE_STD_INITER(_input, 0),           \
        [__stdout]  = PIPE_STD_INITER(NULL, _align_out),    \
        [__stderr]  = PIPE_STD_INITER(NULL, _align_err),    \
    },                                                      \
    .timeout = PIPE_TIMEOUT(_timeout),                      \
}

#define __pipe_std_buf(_std)    ((_std)->buf + (_std)->len)
#define __pipe_std_size(_std)   ((_std)->size - 1)
#define __pipe_std_space(_std)  (__pipe_std_size(_std) - (_std)->len)

static inline int
__pipe_std_grow(pipe_std_t *std)
{
    if (NULL==std->buf) {
        return -EKEYNULL;
    }
    else if (std->len < __pipe_std_size(std)) {
        return 0;
    }

    char *info = (char *)os_realloc(std->buf, std->size + std->grow);
    if (NULL==info) {
        return -ENOMEM;
    }
    os_memzero(std->buf + std->size, std->grow);
    std->size += std->grow;

    return 0;
}

static inline int
__pipe_std_read(pipe_std_t *std)
{
    int err, len;
    int space = __pipe_std_space(std);
    
    len = read(std->fd[__pipe_father], __pipe_std_buf(std), space);
    if (len<0) {
        return -errno;
    }
    else if (len < space) {
        std->len += len;

        return len;
    }

    err = __pipe_std_grow(std);
    if (err<0) {
        return err;
    }

    return __pipe_std_read(std);
}

static inline int
__pipe_info_init(pipe_info_t *info, char *input, int align_out, int align_err, int timeout)
{
    int err;

    os_objzero(info);
    info->timeout = PIPE_TIMEOUT(timeout);

    __pipe_std_init(&info->std[__stdin], input, 0);
    __pipe_std_init(&info->std[__stdout], NULL, align_out);
    __pipe_std_init(&info->std[__stderr], NULL, align_err);

    err = pipe(info->std[__stdout].fd);
    if (err<0) {
        return -errno;
    }
    
    err = pipe(info->std[__stderr].fd);
    if (err<0) {
        return -errno;
    }

    return 0;
}

static inline void
__pipe_info_release(pipe_info_t *info)
{
    if (info) {
#if 0
        os_close(info->std[__stdout].fd[__pipe_father]);
        os_close(info->std[__stderr].fd[__pipe_father]);
        
        os_close(info->std[__stdout].fd[__pipe_son]);
        os_close(info->std[__stderr].fd[__pipe_son]);
#endif
        os_free(info->std[__stderr].buf);
        os_free(info->std[__stdout].buf);
    }
}

static bool
__pipe_father_wait(pipe_info_t *info, int son)
{
    int err, status = 0;
    int pid = waitpid(son, &status, WNOHANG);
        
    if (pid==son) {
        err = info->err = __os_wait_error(status);
        
        os_noblock(info->std[__stdout].fd[__pipe_father]);
        __pipe_std_read(&info->std[__stdout]);

        os_noblock(info->std[__stderr].fd[__pipe_father]);
        __pipe_std_read(&info->std[__stderr]);

        return true;
    }
    else {
        return false;
    }
}

static int
__pipe_father_handle(pipe_info_t *info, int son)
{
    struct timeval tv = {
        .tv_sec     = os_second(info->timeout),
        .tv_usec    = os_usecond(info->timeout),
    };
    fd_set rset;
    int err = 0;
    int max = OS_MAX(info->std[__stdout].fd[__pipe_father], info->std[__stderr].fd[__pipe_father]);

    os_close(info->std[__stdout].fd[__pipe_son]);
    os_close(info->std[__stderr].fd[__pipe_son]);
    
    while(1) {        
        FD_ZERO(&rset);
        FD_SET(info->std[__stdout].fd[__pipe_father], &rset);
        FD_SET(info->std[__stderr].fd[__pipe_father], &rset);

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

        if (FD_ISSET(info->std[__stdout].fd[__pipe_father], &rset)) {
            err = __pipe_std_read(&info->std[__stdout]);
            if (err<0) {
                goto error;
            }
        }
        
        if (FD_ISSET(info->std[__stderr].fd[__pipe_father], &rset)) {
            err = __pipe_std_read(&info->std[__stderr]);
            if (err<0) {
                goto error;
            }
        }

        if (__pipe_father_wait(info, son)) {
            err = 0; goto error;
        }
    }

error:
    os_close(info->std[__stdout].fd[__pipe_father]);
    os_close(info->std[__stderr].fd[__pipe_father]);

    return err;
}

static inline int
__pipe_son_handle(pipe_info_t *info)
{   
    // re-link __stdout/__stderr
    close(__stdout);
    close(__stderr);
    dup2(info->std[__stdout].fd[__pipe_son], __stdout);
    dup2(info->std[__stderr].fd[__pipe_son], __stderr);
    
    os_close(info->std[__stdout].fd[__pipe_father]);
    os_close(info->std[__stderr].fd[__pipe_father]);
    os_close(info->std[__stdout].fd[__pipe_son]);
    os_close(info->std[__stderr].fd[__pipe_son]);

    execl("/bin/sh", "sh", "-c", info->std[__stdin].buf, NULL);

    // go here is error
    return -errno;
}

typedef int os_pipe_system_callback_t(pipe_info_t *info);

static inline int
__os_pipe_system(uint32_t timeout, os_pipe_system_callback_t *cb, char *cmd)
{
    pipe_info_t info;
    int err = 0, pid = 0;

    __pipe_info_init(&info, cmd, PIPE_STDOUT_GROW, PIPE_STDERR_GROW, timeout);
    
    pid = fork();
    if (pid<0) {
        err = -errno;
    }
    else if (pid>0) { // father
        err = __pipe_father_handle(&info, pid);
        if (0==err) {
            (*cb)(&info);
        }
    }
    else { // child
        err = __pipe_son_handle(&info);
    }

error:
    __pipe_info_release(&info);

    return err;
}

static inline int
__os_pipe_vsystem(uint32_t timeout, os_pipe_system_callback_t *cb, char *fmt, ...)
{
    va_list args;
    char *cmd = NULL;
    int err = 0;
    
    va_start(args, fmt);
    err = os_vasprintf(&cmd, fmt, args);
    va_end(args);
    if (err<0) {
        goto error;
    } else {
        err = __os_pipe_system(timeout, cb, cmd);
    }

error:
    os_free(cmd);
    
    return err;
}

#define os_pipe_vsystem(_cb, _fmt, _args...)    __os_pipe_vsystem(PIPE_TIMEOUT_DEFT, _cb, _fmt, ##_args)
#define os_pipe_system(_cb, _cmd)              __os_pipe_system(PIPE_TIMEOUT_DEFT, _cb, _cmd)
/******************************************************************************/
#endif
#endif /* __PIPE_H_0cb88994d86a4261ad2f10f1e0525dcf__ */
