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

#define __cli_line_next(_args)              os_str_next_byifs(_args, ' ')
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
cli_line_handle(cli_table_t tables[], int count, char *tag, char *args, int (*after)(void))
{
    int i, err;
    
    for (i=0; i<count; i++) {
        cli_table_t *table = &tables[i];
        
        if (0==os_strcmp(table->tag, tag)) {
            err = (*table->u.line_cb)(args);
            
            if (table->syn && after) {
                int len = (*after)();
                
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
        
        if (0==os_strcmp(table->tag, argv[0])) {
            return (table->u.argv_cb)(argc-1, argv+1);
        }
    }

    return -ENOEXIST;
}
#endif /* defined(__APP__) || defined(__BOOT__) */
/******************************************************************************/
#ifdef __APP__
#ifndef CLI_BUFFER_SIZE
#define CLI_BUFFER_SIZE      (256*1024-1-sizeof(int)-3*sizeof(uint32_t))
#endif

typedef struct {
    uint32_t len, r0, r1;
    int err;
    char buf[1+CLI_BUFFER_SIZE];
} cli_buffer_t;

#define cli_buffer_hsize            offsetof(cli_buffer_t, buf)

#define DECLARE_FAKE_CLI_BUFFER     extern cli_buffer_t __THIS_CLI_BUFFER
#define DECLARE_REAL_CLI_BUFFER     cli_buffer_t __THIS_CLI_BUFFER

#ifdef __BUSYBOX__
#   define DECLARE_CLI_BUFFER       DECLARE_FAKE_CLI_BUFFER
#else
#   define DECLARE_CLI_BUFFER       DECLARE_REAL_CLI_BUFFER
#endif

DECLARE_FAKE_CLI_BUFFER;

static inline cli_buffer_t *
__this_cli_buffer(void)
{
    return &__THIS_CLI_BUFFER;
}

#define cli_buffer_err      __this_cli_buffer()->err
#define cli_buffer_len      __this_cli_buffer()->len
#define cli_buffer_buf      __this_cli_buffer()->buf
#define cli_buffer_cursor   (cli_buffer_buf + cli_buffer_len)
#define cli_buffer_size     (cli_buffer_hsize + cli_buffer_len)
#define cli_buffer_left     (CLI_BUFFER_SIZE - cli_buffer_size)

#define cli_buffer_zero()   os_objzero(__this_cli_buffer())
#define cli_buffer_clear()  do{ \
    cli_buffer_buf[0]  = 0;     \
    cli_buffer_len     = 0;     \
}while(0)
#define cli_buffer_error(_err)      (cli_buffer_err=(_err))
#define cli_buffer_ok               cli_buffer_error(0)

static inline int
cli_sprintf(const char *fmt, ...)
{
    int len = 0;
    va_list args;

    if (cli_buffer_len >= CLI_BUFFER_SIZE) {
        return -ENOSPACE;
    }
    
    va_start(args, (char *)fmt);
    len = os_vsnprintf(cli_buffer_cursor, cli_buffer_left, fmt, args);
    va_end(args);

    if (len<0) {
        return -errno;
    }
    else if (len > cli_buffer_left) {
        return -ENOSPACE;
    }

    cli_buffer_len = len;
    
    return len;
}

#define cli_recv(_fd, _timeout) \
    io_recv(_fd, (char *)__this_cli_buffer(), sizeof(cli_buffer_t), _timeout)

#define cli_send(_fd) \
    io_send(_fd, (char *)__this_cli_buffer(), cli_buffer_size)

#define cli_recvfrom(_fd, _timeout, _addr, _paddrlen) \
    io_recvfrom(_fd, (char *)__this_cli_buffer(), sizeof(cli_buffer_t), _timeout, _addr, _paddrlen)

#define cli_sendto(_fd, _addr, _addrlen) \
    io_sendto(_fd, (char *)__this_cli_buffer(), cli_buffer_size, _addr, _addrlen)

typedef struct {
    int timeout;
    
    sockaddr_un_t server, client;
} cli_client_t;

#define CLI_CLIENT_INITER(_timeout, _server_path) {     \
    .server     = OS_SOCKADDR_UNIX("\0" _server_path),  \
    .client     = OS_SOCKADDR_UNIX(__empty),            \
    .timeout    = _timeout,                             \
}   /* end */

static int
____cli_d_handle(char *line, cli_table_t *table, int count, int (*after)(void))
{
    char *method = line;
    char *args   = line;
    int err;
    
    cli_buffer_zero();
    
    os_str_strim_both(method, NULL);
    os_str_reduce(method, NULL);
    
    cli_shift(args);
    
    err = cli_line_handle(table, count, method, args, after);
    cli_buffer_error(err);

    debug_trace("action:%s %s, error:%d, len:%d, buf:%s", 
        method, args?args:__empty,
        cli_buffer_err,
        cli_buffer_len,
        cli_buffer_buf);

    return err;
}

static inline int
__cli_d_handle(int fd, cli_table_t *table, int count)
{
    char buf[1+OS_LINE_LEN] = {0};
    sockaddr_un_t client = OS_SOCKADDR_UNIX(__empty);
    sockaddr_un_t *pclient = &client;
    socklen_t addrlen = sizeof(client);
    int err;
    
    err = __io_recvfrom(fd, buf, sizeof(buf), 0, (sockaddr_t *)pclient, &addrlen);
    if (err<0) { /* yes, <0 */
        return err;
    }
    buf[err] = 0;
    
    if (is_abstract_sockaddr(pclient)) {
        set_abstract_sockaddr_len(pclient, addrlen);

        debug_trace("recv from:%s", get_abstract_path(pclient));
    }
    
    int after(void)
    {
        return cli_sendto(fd, (sockaddr_t *)pclient, addrlen);
    }
    
    return ____cli_d_handle(buf, table, count, after);
}

#define cli_d_handle(_fd, _table) \
    __cli_d_handle(_fd, _table, os_count_of(_table))

static int
__cli_c_handle(
    bool syn,
    char *buf, 
    sockaddr_un_t *server, 
    sockaddr_un_t *client,
    int timeout
)
{
    int fd;
    int err;
    int len;
    
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

    if (syn) {
        err = cli_recv(fd, timeout);
        if (err<0) { /* yes, <0 */
            goto error;
        }

        if (0==cli_buffer_err && cli_buffer_len && is_good_str(cli_buffer_buf)) {
            os_printf("%s", cli_buffer_buf);
        }

        debug_trace("action:%s, error:%d, len:%d, buf:%s", 
            buf,
            cli_buffer_err,
            cli_buffer_len,
            cli_buffer_buf);
        err = shell_error(cli_buffer_err);
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
    os_sockaddr_t addr;
    
    int (*init)(struct cli_server *server);
    int (*handle)(struct cli_server *server);
} cli_server_t;

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

    int size = 1+CLI_BUFFER_SIZE;
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
        err = (*server[i]->init)(server[i]);
        if (err<0) {
            return err;
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
        fdmax = OS_MAX(fdmax, server[i]->fd);
    }

    return fdmax;
}

static inline void
__cli_server_prepare(cli_server_t *server[], int count, fd_set *set)
{
    int i;
    
    FD_ZERO(set);
    for (i=0; i<count; i++) {
        FD_SET(server[i]->fd, set);
    }
}

static inline int
__cli_server_handle(cli_server_t *server[], int count, fd_set *set)
{
    int i, err;
    
    for (i=0; i<count; i++) {
        if (FD_ISSET(server[i]->fd, set)) {
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
