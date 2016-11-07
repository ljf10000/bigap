#ifndef __CLI_H_277ca663cad74dd5ad59851d69c58e0c__
#define __CLI_H_277ca663cad74dd5ad59851d69c58e0c__
/******************************************************************************/
#ifndef CLI_TIMEOUT
#define CLI_TIMEOUT         3000 /* ms */
#endif

#ifndef CLI_MULTI
#define CLI_MULTI   0
#endif

#ifndef CLI_BUFFER_LEN
#if CLI_MULTI
#   define CLI_BUFFER_LEN           OS_PAGE_LEN
#else
#   define CLI_BUFFER_LEN           OS_BIG_LEN
#endif
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
     int32 err;

    char buf[0];
} cli_buffer_t;

typedef struct {
    int fd;
    bool tcp;
    
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

        cli->addrlen    = sizeof(sockaddr_un_t);

        sockaddr_un_t addr = OS_SOCKADDR_UNIX(__empty);
        os_objcpy(&cli->addr, &addr);
    }
    
    return cli;
}

static inline cli_buffer_t *
__this_clib(void)
{
    return __this_cli()->b;
}

#define __this_clib_err     __this_clib()->err
#define __this_clib_len     __this_clib()->len
#define __this_clib_buf     __this_clib()->buf
#define __this_clib_cursor  (__this_clib_buf  + __this_clib_len)
#define __this_clib_size    (CLI_BUFFER_LEN - sizeof(cli_buffer_t))
#define __this_clib_space   (sizeof(cli_buffer_t) + __this_clib_len)
#define __this_clib_left    ((__this_clib_size>__this_clib_len)?(__this_clib_size - __this_clib_len):0)

static inline void
cli_buffer_clear(void) 
{
    __this_clib_buf[0]   = 0;
    __this_clib_len      = 0;
}

static inline int
__cli_recv(int fd, int timeout /* ms */)
{
    return io_recv(fd, (char *)__this_clib(), CLI_BUFFER_LEN, timeout);
}

static inline int
__cli_reply(int err)
{
    cli_t *cli = __this_cli();
    int len;
    
    debug_cli("send reply[len=%d, err=%d]:%s", __this_clib_len, err, __this_clib_buf);

    __this_clib_err = err;
    if (cli->tcp) {
        len = io_send(cli->fd, (char *)__this_clib(), __this_clib_space);
    } else {
        len = io_sendto(cli->fd, (char *)__this_clib(), __this_clib_space, ((struct sockaddr *)&cli->addr), cli->addrlen);
    }
    cli_buffer_clear();
    
    return len;
}

static inline void
__this_cli_init(int fd, bool tcp)
{
    cli_t *cli = __this_cli();

    cli->fd     = fd;
    cli->tcp    = tcp;
}

static inline int
cli_vsprintf(const char *fmt, va_list args)
{
    va_list copy;

    va_copy(copy, args);
    int vsize = os_vsprintf_size((char *)fmt, copy);
    va_end(copy);

#if CLI_MULTI
    if (__this_clib_left < vsize) {
        __cli_reply(0);
    }
#endif

    if (__this_clib_left < vsize) {
        return -ENOSPACE;
    }

    uint32 left = __this_clib_left;
    int len = os_vsnprintf(__this_clib_cursor, left, fmt, args);
    if (len<0) {
        return -errno;
    }
    else if (len > left) {
        return -ENOSPACE;
    }

    __this_clib_len += len;
    __this_clib_cursor[0] = 0;
    
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

typedef struct {
    bool tcp;
    int timeout;
    
    sockaddr_un_t server, client;
} cli_client_t;

#define CLI_CLIENT_INITER(_server_path, _tcp)       {   \
    .server     = OS_SOCKADDR_ABSTRACT(_server_path),   \
    .client     = OS_SOCKADDR_UNIX(__zero),             \
    .timeout    = CLI_TIMEOUT,                          \
    .tcp        = _tcp,                                 \
}   /* end */

#define CLI_CLIENT_UNIX     "/tmp/." __THIS_APPNAME ".%d.unix"

static inline int
init_cli_client(cli_client_t *c)
{
    c->timeout = env_geti(OS_ENV(TIMEOUT), CLI_TIMEOUT);

    abstract_path_sprintf(&c->client, CLI_CLIENT_UNIX, getpid());

    return 0;
}

static inline int
cli_line_handle(
    cli_table_t tables[],
    int count,
    char *tag,
    char *args,
    int (*reply)(int err),
    int (*end)(int err)
)
{
    int i, err;
    
    for (i=0; i<count; i++) {
        cli_table_t *table = &tables[i];
        
        if (os_streq(table->tag, tag)) {
            err = (*table->u.line_cb)(args);
            
            if (table->syn && reply) {
                int len = (*reply)(err);
                debug_trace("send len:%d", len);

                if (end) {
                    (*end)(err);
                }
            }

            return err;
        }
    }
    
    return -ENOEXIST;
}

static inline int
__cli_d_handle(int fd, cli_table_t *table, int count)
{
    cli_t *cli = __this_cli();
    char buf[1+OS_LINE_LEN] = {0};
    int err;

    __this_cli_init(fd, false);
    cli_buffer_clear();

    if (cli->tcp) {
        err = __io_recv(fd, buf, sizeof(buf), 0);
    } else {
        err = __io_recvfrom(fd, buf, sizeof(buf), 0, (sockaddr_t *)&cli->addr, &cli->addrlen);
        if (is_abstract_sockaddr(&cli->addr)) {
            set_abstract_sockaddr_len(&cli->addr, cli->addrlen);
        }
    }
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

    err = cli_line_handle(table, count, method, args, __cli_reply, cli->tcp?__cli_reply:NULL);

    debug_trace("action:%s %s, error:%d, len:%d, buf:%s", 
        method, args?args:__empty,
        __this_clib_err,
        __this_clib_len,
        __this_clib_buf);

    return err;
}

#define cli_d_handle(_fd, _table) \
    __cli_d_handle(_fd, _table, os_count_of(_table))

static inline int
__cli_buffer_show(void)
{
    if (0==__this_clib_err && __this_clib_len && is_good_str(__this_clib_buf)) {
        os_printf("%s", __this_clib_buf);
    }

    debug_trace("error:%d, len:%d, buf:%s", 
        __this_clib_err,
        __this_clib_len,
        __this_clib_buf);

    return __this_clib_err;
}

static inline int
__cli_c_handle(bool syn, char *buf, cli_client_t *clic)
{
    int fd, err, len;
    
    fd = socket(AF_UNIX, clic->tcp?SOCK_STREAM:SOCK_DGRAM, 0);
    if (fd<0) {
        debug_error("socket error:%d", -errno);
        err = -errno; goto error;
    }
    
    err = bind(fd, (sockaddr_t *)&clic->client, get_abstract_sockaddr_len(&clic->client));
    if (err<0) {
        __debug_error("bind error:%d", -errno);
        err = -errno; goto error;
    }

    struct timeval timeout = OS_TIMEVAL_INITER(clic->timeout, 0);
    err = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
    if (err<0) {
        debug_error("setsockopt SO_RCVTIMEO error:%d", -errno);
        return -errno;
    }

    err = connect(fd, (sockaddr_t *)&clic->server, get_abstract_sockaddr_len(&clic->server));
    if (err<0) {
        debug_error("connect error:%d", -errno);
        err = -errno; goto error;
    }

    len = os_strlen(buf);
    err = io_send(fd, buf, len);
    if (err<0) { /* yes, <0 */
        goto error;
    }
    debug_cli("send repuest[%d]:%s", len, buf);
    
    if (syn) {
        err = __cli_recv(fd, clic->timeout);
        if (err<0) { /* yes, <0 */
            goto error;
        }

        err = __cli_buffer_show();
    }
    
error:
    if (is_good_fd(fd)) {
        close(fd);
    }
    
    return err;
}

static inline int
cli_c_handle(
    char *action,
    bool syn,
    int argc, 
    char *argv[],
    cli_client_t *clic
)
{
    char buf[1+OS_LINE_LEN] = {0};
    int i, len;

    len = os_saprintf(buf, "%s", action);
    for (i=0; i<argc; i++) {
        len += os_snprintf(buf + len, OS_LINE_LEN - len, " %s", argv[i]);
    }

    return __cli_c_handle(syn, buf, clic);
}

#define cli_c_sync_handle(_action, _argc, _argv, _client) \
    cli_c_handle(_action, true, _argc, _argv, _client)

#define cli_c_async_handle(_action, _argc, _argv, _client) \
    cli_c_handle(_action, false, _argc, _argv, _client)

static inline int
cli_u_server_init(sock_server_t *server)
{
    int fd, err;
    
    fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (fd<0) {
    	debug_error("socket error:%d", -errno);
        return -errno;
    }
    os_closexec(fd);
    
    err = bind(fd, &server->addr.c, get_abstract_sockaddr_len(&server->addr.un));
    if (err<0) {
        debug_error("bind error:%d", -errno);
        return -errno;
    }

    int size = 1+CLI_BUFFER_LEN;
    err = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
    if (err<0) {
        debug_error("setsockopt error:%d", -errno);
        return -errno;
    }

    server->fd = fd;
    
    return 0;
}

#define CLI_U_SERVER_INIT(_server, _name) ({  \
    int _err = 0;                           \
                                            \
    set_abstract_path(&(_server)->addr.un, OS_UNIX_PATH(_name)); \
                                            \
    _err = cli_u_server_init(_server);      \
    if (0==_err) {                          \
        debug_ok("init cli server ok");     \
    }                                       \
                                            \
    _err;                                   \
})  /* end */
/******************************************************************************/
static inline int
cli_loops_init(loop_t *loop, char *path, loop_son_f *cb)
{
    sockaddr_un_t server = OS_SOCKADDR_UNIX(__zero);
    int fd, err;

    set_abstract_path(&server, path);
    
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd<0) {
    	debug_error("socket error:%d", -errno);
        return -errno;
    }
    os_closexec(fd);
    
    err = bind(fd, (sockaddr_t *)&server, get_abstract_sockaddr_len(&server));
    if (err<0) {
        debug_error("bind error:%d", -errno);
        return -errno;
    }

    int size = 1+CLI_BUFFER_LEN;
    err = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
    if (err<0) {
        debug_error("setsockopt SO_SNDBUF error:%d", -errno);
        return -errno;
    }

    struct timeval send_timeout = OS_TIMEVAL_INITER(1000, 0);
    err = setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&send_timeout, sizeof(send_timeout));
    if (err<0) {
        debug_error("setsockopt SO_SNDTIMEO error:%d", -errno);
        return -errno;
    }
    
    err = os_loop_add_father(loop, fd, cb);
    if (err<0) {
        debug_error("add cli loop error:%d", err);
        
        return err;
    }

    return 0;
}

static inline int
__cli_loopc_init(cli_client_t *clic)
{
    int fd, err;
    
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd<0) {
        debug_error("socket error:%d", -errno);
        return -errno;
    }
    
    err = bind(fd, (sockaddr_t *)&clic->client, get_abstract_sockaddr_len(&clic->client));
    if (err<0) {
        __debug_error("bind error:%d", -errno);
        return -errno;
    }

    err = connect(fd, (sockaddr_t *)&clic->server, get_abstract_sockaddr_len(&clic->server));
    if (err<0) {
        debug_error("connect error:%d", -errno);
        return -errno;
    }

    struct timeval recv_timeout = OS_TIMEVAL_INITER(clic->timeout, 0);
    err = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&recv_timeout, sizeof(recv_timeout));
    if (err<0) {
        debug_error("setsockopt SO_RCVTIMEO error:%d", -errno);
        return -errno;
    }

    return fd;
}

static inline int
__cli_loopc_send(cli_client_t *clic, int fd, char *buf)
{
    int len = os_strlen(buf);
    
    int err = io_send(fd, buf, len);
    if (err<0) { /* yes, <0 */
        return err;
    }
    debug_cli("send repuest[%d]:%s", len, buf);

    return 0;
}

static inline int
__cli_loopc_recv(cli_client_t *clic, int fd)
{
    int err = 0;
    
    char *buf = (char *)__this_clib();
    while(1) {
        err = __io_recv(fd, buf, CLI_BUFFER_LEN, 0);
        if (err > sizeof(cli_buffer_t)) {
            buf[err] = 0;
            
            err = __cli_buffer_show();
            if (err<0) {
                return err;
            }
        }
        else if (err == sizeof(cli_buffer_t)) {
            return __this_clib_err;
        }
        else if (err > 0) {
            return -ETOOSMALL;
        }
        else if (0==err) {
            return 0;
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
    }

    return 0;
}

static inline int
__cli_loopc_handle(cli_client_t *clic, char *buf)
{
    int fd = INVALID_FD, err = 0;

    err = fd = __cli_loopc_init(clic);
    if (err<0) {
        goto error;
    }

    err = __cli_loopc_send(clic, fd, buf);
    if (err<0) {
        goto error;
    }

    err = __cli_loopc_recv(clic, fd);
    if (err<0) {
        goto error;
    }
    
error:
    if (is_good_fd(fd)) {
        close(fd);
    }
    
    return err;
}

static inline int
cli_loopc_handle(
    cli_client_t *clic,
    char *action,
    int argc, 
    char *argv[]
)
{
    char buf[1+OS_LINE_LEN] = {0};
    int i, len;

    len = os_saprintf(buf, "%s", action);
    for (i=0; i<argc; i++) {
        len += os_snprintf(buf + len, OS_LINE_LEN - len, " %s", argv[i]);
    }

    return __cli_loopc_handle(clic, buf);
}

#endif /* __APP__ */
/******************************************************************************/
#endif /* __CLI_H_277ca663cad74dd5ad59851d69c58e0c__ */
