#ifndef __CLI_H_277ca663cad74dd5ad59851d69c58e0c__
#define __CLI_H_277ca663cad74dd5ad59851d69c58e0c__
/******************************************************************************/
#ifndef __CLI_TCP__
#define __CLI_TCP__         1
#endif

#if __CLI_TCP__
#define CLI_SOCK_TYPE       SOCK_STREAM
#define CLI_REPLY_END       __cli_reply
#define CLI_BUFFER_LEN      OS_PAGE_LEN /* test tcp with page size */
#else
#define CLI_SOCK_TYPE       SOCK_DGRAM
#define CLI_REPLY_END       NULL
#define CLI_BUFFER_LEN      OS_BIG_LEN
#endif

#ifndef CLI_TIMEOUT
#define CLI_TIMEOUT         5000 /* ms */
#endif

#if defined(__APP__) || defined(__BOOT__)
typedef struct {
    char *tag;

    bool syn; /* just for server, client not use it */
    
    union {
        void *cb;
        int (*line_cb)(char *args);
        int (*argv_cb)(int argc, char *argv[]);
    } u;
} cli_table_t;

#define __CLI_ENTRY(_tag, _syn, _cb)   { \
    .tag    = _tag,         \
    .syn    = _syn,         \
    .u      = {             \
        .cb = _cb,          \
    },                      \
}

#define CLI_ENTRY(_tag, _cb)    __CLI_ENTRY(_tag, true, _cb)

#define __cli_argv_dump(_dump, _argc, _argv) ({ \
    int i;                                  \
                                            \
    for (i=0; i<_argc; i++) {               \
        _dump("function:%s argv[%d]=%s",    \
            __func__, i, _argv[i]);         \
    }                                       \
                                            \
    _argv[0];                               \
})

#define cli_argv_dump(_argc, _argv)         __cli_argv_dump(debug_trace, _argc, _argv)

#define __cli_line_next(_args)              os_str_next_byifs(_args, __space)
#define __cli_shift(_args, _count)      do{ \
    int i, count = (_count);                \
                                            \
    for (i=0; i<count; i++) {               \
        _args = __cli_line_next(_args);     \
    }                                       \
}while(0)

#define cli_shift(_args)                do{ \
    _args = __cli_line_next(_args);         \
}while(0)

static inline int
cli_argv_handle(cli_table_t tables[], int count, int argc, char *argv[])
{
    int i;

    if (argc < 1) {
        return -EINVAL0;
    }
    
    for (i=0; i<count; i++) {
        cli_table_t *table = &tables[i];
        
        if (os_streq(table->tag, argv[0])) {
            return (table->u.argv_cb)(argc-1, argv+1);
        }
    }

    return -ENOEXIST;
}
#endif /* defined(__APP__) || defined(__BOOT__) */
/******************************************************************************/
#ifdef __APP__

typedef struct {
    uint32 len;
    uint32 flag;
    uint32 type;
     int32 err;
} cli_header_t;

typedef struct {
    cli_header_t hdr;
    
    char buf[0];
} cli_buffer_t;

typedef struct {
    int fd;

    sockaddr_un_t   addr;
    socklen_t       addrlen;

    cli_buffer_t    *b;
} cli_t;

#define DECLARE_FAKE_CLI        extern cli_t __THIS_CLI
#define DECLARE_REAL_CLI        cli_t __THIS_CLI;

#ifdef __ALLINONE__
#   define DECLARE_CLI          DECLARE_FAKE_CLI
#else
#   define DECLARE_CLI          DECLARE_REAL_CLI
#endif

DECLARE_FAKE_CLI;

static inline cli_t *
__this_cli(void)
{
    cli_t *cli = &__THIS_CLI;
    
    if (NULL==cli->b) {
        cli->b = (cli_buffer_t *)os_zalloc(1+CLI_BUFFER_LEN);
        if (NULL==cli->b) {
            return NULL;
        }
        
        cli->addr.sun_family = AF_UNIX;
        cli->addrlen = sizeof(sockaddr_un_t);
    }
    
    return cli;
}

static inline cli_buffer_t *
__clib(void)
{
    return __this_cli()->b;
}

#define __clib_err      __clib()->hdr.err
#define __clib_len      __clib()->hdr.len
#define __clib_buf      __clib()->buf
#define __clib_cursor   (__clib_buf  + __clib_len)
#define __clib_space    (sizeof(cli_header_t) + __clib_len)
#define __clib_SIZE     (CLI_BUFFER_LEN - sizeof(cli_header_t))
#define __clib_left     ((__clib_SIZE>__clib_len)?(__clib_SIZE - __clib_len):0)

static inline int
__clib_show(void)
{
    if (0==__clib_err && __clib_len && is_good_str(__clib_buf)) {
        os_printf("%s", __clib_buf);
    }

    return __clib_err;
}

static inline void
__clib_cut(uint32 len)
{
    __clib_buf[len] = 0;
}

static inline void
__clib_clear(void) 
{
    __clib_len = 0;
    
    __clib_cut(0);
}

static inline int
__cli_reply(int err)
{
    cli_t *cli = __this_cli();
    int len;
    
    debug_cli("send reply[len=%d, err=%d]:%s", __clib_len, err, __clib_buf);

    __clib_err = err;
#if __CLI_TCP__
    len = io_send(cli->fd, __clib(), __clib_space);
#else
    len = io_sendto(cli->fd, __clib(), __clib_space, ((struct sockaddr *)&cli->addr), cli->addrlen);
#endif
    __clib_clear();
    
    return len;
}

static inline int
cli_vsprintf(const char *fmt, va_list args)
{
    va_list copy;

    va_copy(copy, args);
    int vsize = os_vsprintf_size((char *)fmt, copy);
    va_end(copy);

#if CLI_MULTI
    if (__clib_left < vsize) {
        __cli_reply(0);
    }
#endif

    if (__clib_left < vsize) {
        return -ENOSPACE;
    }

    uint32 left = __clib_left;
    int len = os_vsnprintf(__clib_cursor, left, fmt, args);
    if (len<0) {
        return -errno;
    }
    else if (len > left) {
        return -ENOSPACE;
    }

    __clib_len += len;
    __clib_cursor[0] = 0;
    
    return len;
}

static inline int
cli_sprintf(const char *fmt, ...)
{
    va_list args;

    va_start(args, (char *)fmt);
    int len = cli_vsprintf(fmt, args);
    va_end(args);

    return len;
}

#if 1
typedef struct {
    int timeout;
    
    sockaddr_un_t server, client;
} cli_client_t;

#define CLI_CLIENT_INITER(_server_PATH)             {   \
    .server     = OS_SOCKADDR_ABSTRACT(_server_PATH),   \
    .client     = OS_SOCKADDR_UNIX(__zero),             \
    .timeout    = CLI_TIMEOUT,                          \
}   /* end */

#define CLI_CLIENT_UNIX     "/tmp/." __THIS_APPNAME ".%d.unix"

static inline int
__clic_fd(cli_client_t *clic)
{
    int fd = INVALID_FD, err = 0;
    int type;
    
    sockaddr_un_t *client = &clic->client;
    abstract_path_sprintf(client, CLI_CLIENT_UNIX, getpid());
    clic->timeout = env_geti(OS_ENV(TIMEOUT), CLI_TIMEOUT);

    fd = socket(AF_UNIX, CLI_SOCK_TYPE, 0);
    if (fd<0) {
        debug_error("socket error:%d", -errno);
        return -errno;
    }
    os_closexec(fd);

#if __CLI_TCP__
    struct timeval timeout = OS_TIMEVAL_INITER(os_second(clic->timeout), os_usecond(clic->timeout));
    
    err = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
    if (err<0) {
        debug_error("setsockopt SO_RCVTIMEO error:%d", -errno);
        return -errno;
    }
#endif

    err = bind(fd, (sockaddr_t *)client, get_abstract_sockaddr_len(client));
    if (err<0) {
        debug_error("bind(%s) error:%d", get_abstract_path(client), -errno);
        return -errno;
    }

    sockaddr_un_t *server = &clic->server;
    err = connect(fd, (sockaddr_t *)server, get_abstract_sockaddr_len(server));
    if (err<0) {
        debug_error("connect(%s) error:%d", get_abstract_path(server), -errno);
        return -errno;
    }

    return fd;
}

#if __CLI_TCP__
static inline int
__clic_recv(int fd, int timeout)
{
    cli_t *cli = __this_cli();
    int err = 0, len;

    while(1) {
        len = sizeof(cli_header_t);
        err = __io_recv(fd, __clib(), len, 0);
        if (err==len) {
            // is last
            if (0==__clib_len) {
                return __clib_show();
            }

            len = __clib_len;
            err = __io_recv(fd, __clib_buf, len, 0);
            if (err==len) {
                __clib_show();
            } else {
                goto error;
            }
        } else {
error:
            if (err > len) {
                return -ETOOBIG;
            }
            else if (err>0) {
                return -ETOOSMALL;
            }
            // err = 0
            else if (0==err) {
                return __clib_show();
            }
            else if (EINTR==errno) {
                continue;
            }
            else if (EAGAIN==errno) {
                /*
                * timeout
                */
                return -ETIMEOUT;
            }
            else {
                return -errno;
            }
        }
    }
    
    return 0;
}
#else
static inline int
__clic_recv(int fd, int timeout)
{
    cli_t *cli = __this_cli();
    int err = 0;

    err = io_recv(fd, __clib(), CLI_BUFFER_LEN, timeout);
    // err > hdr
    if (err >= (int)sizeof(cli_header_t)) {
        __clib_cut(err - sizeof(cli_header_t));
        
        return __clib_show();
    }
    // 0 < err < hdr
    else if (err > 0) {
        return -ETOOSMALL;
    }
    // err < 0
    else {
        return err;
    }
}
#endif

static inline int
__clic_handle(bool syn, char *buf, cli_client_t *clic)
{
    int fd = INVALID_FD, err = 0;

    fd = __clic_fd(clic);
    if (fd<0) {
        return fd;
    }

    int len = os_strlen(buf);
    err = io_send(fd, buf, len);
    if (err<0) { /* yes, <0 */
        goto error;
    }
    debug_cli("send repuest[%d]:%s", len, buf);

    if (false==syn) {
        err = 0; goto error;
    }

    err = __clic_recv(fd, clic->timeout);
    if (err<0) {
        goto error;
    }
    
error:
    os_close(fd);
    
    return err;
}

static inline int
clic_handle(cli_client_t *clic, bool syn, char *action, int argc, char *argv[])
{
    char buf[1+OS_LINE_LEN] = {0};
    int i, len;

    len = os_saprintf(buf, "%s", action);
    for (i=0; i<argc; i++) {
        len += os_snprintf(buf + len, OS_LINE_LEN - len, " %s", argv[i]);
    }

    return __clic_handle(syn, buf, clic);
}

#define clic_sync_handle(_client, _action, _argc, _argv) \
    clic_handle(_client, true, _action, _argc, _argv)

#define clic_async_handle(_client, _action, _argc, _argv) \
    clic_handle(_client, false, _action, _argc, _argv)
#endif
/******************************************************************************/
#if 1
static inline int
__clis_fd(sockaddr_un_t *server)
{
    int fd = INVALID_FD, err = 0;
    
    fd = socket(AF_UNIX, CLI_SOCK_TYPE, 0);
    if (fd<0) {
    	debug_error("socket error:%d", -errno);
        return -errno;
    }
    os_closexec(fd);
    
    err = bind(fd, (sockaddr_t *)server, get_abstract_sockaddr_len(server));
    if (err<0) {
        debug_error("bind(%s) error:%d", get_abstract_path(server), -errno);
        
        return -errno;
    }

#if __CLI_TCP__
    err = listen(fd, 0);
    if (err<0) {
        debug_error("listen(%s) error:%d", get_abstract_path(server), -errno);
        
        return -errno;
    }
#endif

    return fd;
}

#define __clis_FD(_name)    ({  \
    sockaddr_un_t server = OS_SOCKADDR_ABSTRACT(_name); \
                                \
    __clis_fd(&server);         \
})  /* end */

static inline int
cli_line_handle(
    cli_table_t tables[],
    int count,
    char *tag,
    char *args,
    int (*reply)(int err),
    int (*reply_end)(int err)
)
{
    int i, len, err;
    
    for (i=0; i<count; i++) {
        cli_table_t *table = &tables[i];
        
        if (os_streq(table->tag, tag)) {
            err = (*table->u.line_cb)(args);
            
            if (table->syn && reply) {
                len = (*reply)(err);
                debug_trace("send len:%d", len);

                if (len>0 && reply_end) {
                    (*reply_end)(err);
                }
            }

            return err;
        }
    }
    
    return -ENOEXIST;
}

static inline int
__clis_handle(int fd, cli_table_t *table, int count)
{
    cli_t *cli = __this_cli();
    char buf[1+OS_LINE_LEN] = {0};
    int err;

    __this_cli()->fd = fd;
    __clib_clear();
    
#if __CLI_TCP__
    err = __io_recv(fd, buf, sizeof(buf), 0);
#else
    err = __io_recvfrom(fd, buf, sizeof(buf), 0, (sockaddr_t *)&cli->addr, &cli->addrlen);
    if (is_abstract_sockaddr(&cli->addr)) {
        set_abstract_sockaddr_len(&cli->addr, cli->addrlen);
    }
#endif
    if (err<0) { /* yes, <0 */
        return err;
    }
    buf[err] = 0;
    debug_cli("recv request[%d]:%s", err, buf);
    
    char *method = buf;
    char *args   = buf;

    os_str_strim_both(method, NULL);
    os_str_reduce(method, NULL);
    cli_shift(args);

    err = cli_line_handle(table, count, method, args, __cli_reply, CLI_REPLY_END);

    debug_trace("action:%s %s, error:%d, len:%d, buf:%s", 
        method, args?args:__empty,
        __clib_err,
        __clib_len,
        __clib_buf);

    return err;
}

#define clis_handle(_fd, _table) \
    __clis_handle(_fd, _table, os_count_of(_table))
#endif
#endif /* __APP__ */
/******************************************************************************/
#endif /* __CLI_H_277ca663cad74dd5ad59851d69c58e0c__ */
