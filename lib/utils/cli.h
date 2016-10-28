#ifndef __CLI_H_277ca663cad74dd5ad59851d69c58e0c__
#define __CLI_H_277ca663cad74dd5ad59851d69c58e0c__
/******************************************************************************/
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

static int
cli_line_handle(cli_table_t tables[], int count, char *tag, char *args, int (*reply)(int err))
{
    int i, err;
    
    for (i=0; i<count; i++) {
        cli_table_t *table = &tables[i];
        
        if (os_streq(table->tag, tag)) {
            err = (*table->u.line_cb)(args);
            
            if (table->syn && reply) {
                int len = (*reply)(err);
                
                debug_trace("send len:%d", len);
            }

            return err;
        }
    }
    
    return -ENOEXIST;
}

static int
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
    sockaddr_un_t addr;
    socklen_t len;
    int fd;
} cli_addr_t;

typedef struct {
    uint32 len;
    uint32 _r0, _r1;
    int err;
} cli_header_t;

#ifndef CLI_BUFFER_SIZE
#define CLI_BUFFER_SIZE         (256*1024)
#endif

typedef struct {
    cli_header_t header;

    char buf[0];
} cli_buffer_t;

#define DECLARE_FAKE_CLI_BUFFER     extern cli_buffer_t *__THIS_CLI_BUFFER
#define DECLARE_REAL_CLI_BUFFER     cli_buffer_t *__THIS_CLI_BUFFER

#define DECLARE_FAKE_CLI_ADDR       extern cli_addr_t __THIS_CLI_ADDR
#define DECLARE_REAL_CLI_ADDR       cli_addr_t __THIS_CLI_ADDR = { .addr = OS_SOCKADDR_UNIX(__empty), .len = sizeof(sockaddr_un_t) }

#ifdef __ALLINONE__
#   define DECLARE_CLI_BUFFER       DECLARE_FAKE_CLI_BUFFER
#   define DECLARE_CLI_ADDR         DECLARE_FAKE_CLI_ADDR
#else
#   define DECLARE_CLI_BUFFER       DECLARE_REAL_CLI_BUFFER
#   define DECLARE_CLI_ADDR         DECLARE_REAL_CLI_ADDR
#endif

DECLARE_FAKE_CLI_BUFFER;
DECLARE_FAKE_CLI_ADDR;

static inline cli_addr_t *
__this_cli_addr(void)
{
    return &__THIS_CLI_ADDR;
}

static inline cli_buffer_t *
__this_cli_buffer(void)
{
    if (NULL==__THIS_CLI_BUFFER) {
        __THIS_CLI_BUFFER = (cli_buffer_t *)os_zalloc(CLI_BUFFER_SIZE);
    }
    
    return __THIS_CLI_BUFFER;
}

#define cli_buffer_err      __this_cli_buffer()->header.err
#define cli_buffer_len      __this_cli_buffer()->header.len
#define cli_buffer_buf      __this_cli_buffer()->buf
#define cli_buffer_cursor   (cli_buffer_buf  + cli_buffer_len)
#define cli_buffer_size     (CLI_BUFFER_SIZE - sizeof(cli_header_t) - 1)
#define cli_buffer_space    (sizeof(cli_header_t) + cli_buffer_len)
#define cli_buffer_left     ((cli_buffer_size>cli_buffer_len)?(cli_buffer_size - cli_buffer_len):0)

static inline void
cli_buffer_clear(void) 
{
    cli_buffer_buf[0]   = 0;
    cli_buffer_len      = 0;
}

static inline int
__cli_recv(int fd, int timeout /* ms */)
{
    return io_recv(fd, (char *)__this_cli_buffer(), CLI_BUFFER_SIZE, timeout);
}

static inline int
__cli_send(int fd)
{
    return io_send(fd, (char *)__this_cli_buffer(), cli_buffer_space);
}

static inline int
__cli_recvfrom(int fd, int timeout /* ms */, sockaddr_t *addr, socklen_t *paddrlen)
{
    return io_recvfrom(fd, (char *)__this_cli_buffer(), CLI_BUFFER_SIZE, timeout, addr, paddrlen);
}

static inline int
__cli_sendto(int fd, sockaddr_t *addr, socklen_t addrlen)
{
    return io_sendto(fd, (char *)__this_cli_buffer(), cli_buffer_space, addr, addrlen);
}

static inline int
__cli_reply(int err)
{
    cli_addr_t *ca = __this_cli_addr();
    
    cli_buffer_err = err;

    debug_cli("send reply[%d]:%s", cli_buffer_space, (char *)__this_cli_buffer());
    
    int ret = __cli_sendto(ca->fd, (sockaddr_t *)&ca->addr, ca->len);

    cli_buffer_clear();
    
    return ret;
}

static inline int
cli_vsprintf(const char *fmt, va_list args)
{
    va_list copy;

    va_copy(copy, args);
    int vsize = os_vsprintf_size((char *)fmt, copy);
    va_end(copy);

    if (cli_buffer_left < vsize) {
        return -ENOSPACE;
    }

    int len = os_vsnprintf(cli_buffer_cursor, cli_buffer_left, fmt, args);
    if (len<0) {
        return -errno;
    }
    else if (len > cli_buffer_left) {
        return -ENOSPACE;
    }

    cli_buffer_len += len;

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
    int timeout;
    
    sockaddr_un_t server, client;
} cli_client_t;

#define CLI_CLIENT_INITER(_timeout, _server_path) {     \
    .server     = OS_SOCKADDR_UNIX("\0" _server_path),  \
    .client     = OS_SOCKADDR_UNIX(__empty),            \
    .timeout    = _timeout,                             \
}   /* end */

static inline int
__cli_d_handle(int fd, cli_table_t *table, int count)
{
    cli_addr_t *ca = __this_cli_addr();
    char buf[1+OS_LINE_LEN] = {0};
    int err;

    ca->fd = fd;
    cli_buffer_clear();
    
    err = __io_recvfrom(fd, buf, sizeof(buf), 0, (sockaddr_t *)&ca->addr, &ca->len);
    if (err<0) { /* yes, <0 */
        return err;
    }
    buf[err] = 0;
    
    if (is_abstract_sockaddr(&ca->addr)) {
        set_abstract_sockaddr_len(&ca->addr, ca->len);

        debug_cli("recv request from:%s", get_abstract_path(&ca->addr));
    }
    debug_cli("recv request[%d]:%s", err, buf);
    
    char *method = buf;
    char *args   = buf;

    os_str_strim_both(method, NULL);
    os_str_reduce(method, NULL);
    cli_shift(args);

    err = cli_line_handle(table, count, method, args, __cli_reply);

    debug_trace("action:%s %s, error:%d, len:%d, buf:%s", 
        method, args?args:__empty,
        cli_buffer_err,
        cli_buffer_len,
        cli_buffer_buf);

    return err;
}

#define cli_d_handle(_fd, _table) \
    __cli_d_handle(_fd, _table, os_count_of(_table))

static inline int
__cli_c_printf(void)
{
    if (0==cli_buffer_err && cli_buffer_len && is_good_str(cli_buffer_buf)) {
        os_printf("%s", cli_buffer_buf);
    }

    debug_trace("error:%d, len:%d, buf:%s", 
        cli_buffer_err,
        cli_buffer_len,
        cli_buffer_buf);

    return cli_buffer_err;
}

static inline int
__cli_c_handle(
    bool syn,
    char *buf, 
    sockaddr_un_t *server, 
    sockaddr_un_t *client,
    int timeout
)
{
    int fd, err, len;
    
    fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (fd<0) {
        debug_error("socket error:%d", -errno);
        err = -errno; goto error;
    }
    
    err = bind(fd, (sockaddr_t *)client, get_abstract_sockaddr_len(client));
    if (err<0) {
        __debug_error("bind error:%d", -errno);
        err = -errno; goto error;
    }

    err = connect(fd, (sockaddr_t *)server, get_abstract_sockaddr_len(server));
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
        err = __cli_recv(fd, timeout);
        if (err<0) { /* yes, <0 */
            goto error;
        }

        err = __cli_c_printf();

        os_noblock(fd);
        while(1) {
            if (__io_recv(fd, (char *)__this_cli_buffer(), CLI_BUFFER_SIZE, 0)<0) {
                goto error;
            }

            err = __cli_c_printf();
        }
    }
    
error:
    if (is_good_fd(fd)) {
        close(fd);
    }
    
    return err;
}

static int
cli_c_handle(
    char *action,
    bool syn,
    int argc, 
    char *argv[],
    sockaddr_un_t *server, 
    sockaddr_un_t *client,
    int timeout
)
{
    char buf[1+OS_LINE_LEN] = {0};
    int i, len;

    len = os_saprintf(buf, "%s", action);
    for (i=0; i<argc; i++) {
        len += os_snprintf(buf + len, OS_LINE_LEN - len, " %s", argv[i]);
    }

    return __cli_c_handle(syn, buf, server, client, timeout);
}

typedef struct cli_server {
    int fd;
    int id;
    os_sockaddr_t addr;
    
    int (*init)(struct cli_server *server);
    int (*handle)(struct cli_server *server);
} cli_server_t;

#define CLI_SERVER_INITER(_id, _family, _init, _handle) { \
    .id     = _id, \
    .fd     = INVALID_FD, \
    .addr   = OS_SOCKADDR_INITER(_family), \
    .init   = _init, \
    .handle = _handle, \
} /* end */

static inline int
cli_u_server_init(cli_server_t *server)
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

    int size = CLI_BUFFER_SIZE;
    err = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
    if (err<0) {
        debug_error("setsockopt error:%d", -errno);
        return -errno;
    }

    server->fd = fd;
    
    return 0;
}

static inline int
__cli_server_init(cli_server_t *server[], int count)
{
    int i, err;
    
    for (i=0; i<count; i++) {
        if (server[i]) {
            err = (*server[i]->init)(server[i]);
            if (err<0) {
                return err;
            }
        }
    }
    
    return 0;
}
#define cli_server_init(_server)  __cli_server_init(_server, os_count_of(_server))

static inline int
__cli_server_fdmax(cli_server_t *server[], int count)
{
    int i, fdmax = 0;
    
    for (i=0; i<count; i++) {
        if (server[i]) {
            fdmax = OS_MAX(fdmax, server[i]->fd);
        }
    }

    return fdmax;
}

static inline void
__cli_server_prepare(cli_server_t *server[], int count, fd_set *set)
{
    int i;
    
    FD_ZERO(set);
    for (i=0; i<count; i++) {
        if (server[i]) {
            FD_SET(server[i]->fd, set);
        }
    }
}

static inline int
__cli_server_handle(cli_server_t *server[], int count, fd_set *set)
{
    int i, err;
    
    for (i=0; i<count; i++) {
        if (server[i] && FD_ISSET(server[i]->fd, set)) {
            err = (*server[i]->handle)(server[i]);
            if (err<0) {
                /* log, but no return */
            }
        }
    }

    return 0;
}

static inline int
__cli_server_run(cli_server_t *server[], int count)
{
    fd_set rset;
    int i, err, fdmax = __cli_server_fdmax(server, count);

    while(1) {
        __cli_server_prepare(server, count, &rset);
        
        err = select(fdmax+1, &rset, NULL, NULL, NULL);
        switch(err) {
            case -1:/* error */
                if (EINTR==errno) {
                    // is breaked
                    debug_event("select breaked");
                    continue;
                } else {
                    debug_trace("select error:%d", -errno);
                    return -errno;
                }
            case 0: /* timeout, retry */
                debug_timeout("select timeout");
                
                return os_assertV(-ETIMEOUT);
            default: /* to accept */
                err = __cli_server_handle(server, count, &rset);
                if (err<0) {
                    return err;
                }
                
                break;
        }
    }

    return 0;
}

#define cli_server_run(_server)  __cli_server_run(_server, os_count_of(_server))

#endif /* __APP__ */
/******************************************************************************/
#endif /* __CLI_H_277ca663cad74dd5ad59851d69c58e0c__ */
