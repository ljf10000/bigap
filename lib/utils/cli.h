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

#ifndef CLI_SOCK_TYPE
#define CLI_SOCK_TYPE       SOCK_DGRAM
#endif

#if CLI_SOCK_TYPE!=SOCK_STREAM || CLI_SOCK_TYPE!=SOCK_DGRAM
#error "cli sock type must SOCK_STREAM | SOCK_DGRAM"
#endif

#ifndef CLI_BUFFER_SIZE
#define CLI_BUFFER_SIZE         (256*1024)
#endif

typedef struct {
    uint32 len;     /* just buf len */
    uint32 size;    /* include header */
    uint32 _r;
    int err;

    char buf[0];
} cli_buffer_t;

#define DECLARE_FAKE_CLI_BUFFER     extern cli_buffer_t *__THIS_CLI_BUFFER
#define DECLARE_REAL_CLI_BUFFER     cli_buffer_t *__THIS_CLI_BUFFER

#ifdef __ALLINONE__
#   define DECLARE_CLI_BUFFER       DECLARE_FAKE_CLI_BUFFER
#else
#   define DECLARE_CLI_BUFFER       DECLARE_REAL_CLI_BUFFER
#endif

DECLARE_FAKE_CLI_BUFFER;

static inline cli_buffer_t *
__this_cli_buffer(void)
{
    if (NULL==__THIS_CLI_BUFFER) {
        __THIS_CLI_BUFFER = (cli_buffer_t *)os_zalloc(CLI_BUFFER_SIZE);
        __THIS_CLI_BUFFER->size = CLI_BUFFER_SIZE;
    }
    
    return __THIS_CLI_BUFFER;
}
#define cli_buffer_err      __this_cli_buffer()->err
#define cli_buffer_len      __this_cli_buffer()->len
#define cli_buffer_buf      __this_cli_buffer()->buf
#define cli_buffer_size     (__this_cli_buffer()->size - sizeof(cli_buffer_t) - 1)
#define cli_buffer_cursor   (cli_buffer_buf + cli_buffer_len)
#define cli_buffer_space    (sizeof(cli_buffer_t) + cli_buffer_len)
#define cli_buffer_left     ((cli_buffer_size>cli_buffer_len)?(cli_buffer_size-cli_buffer_len):0)
#define CLI_BUFFER_LEN      (cli_buffer_len + sizeof(cli_buffer_t) + 1)

static inline cli_buffer_t *
__this_cli_buffer_expand(uint32 expand)
{
    expand = OS_MAX(CLI_BUFFER_SIZE, expand);
    expand = OS_ALIGN(expand, CLI_BUFFER_SIZE);
    
    uint32 size = __this_cli_buffer()->size + OS_MAX(CLI_BUFFER_SIZE, expand);
    cli_buffer_t *buf = (cli_buffer_t *)os_realloc(__THIS_CLI_BUFFER, size);
    if (NULL==buf) {
        return NULL;
    }
    buf->size = size;
    __THIS_CLI_BUFFER = buf;

    return __THIS_CLI_BUFFER;
}

static inline cli_buffer_t *
__this_cli_buffer_promise(void)
{
    if (CLI_BUFFER_SIZE < CLI_BUFFER_LEN) {
        cli_buffer_t *buf = (cli_buffer_t *)os_realloc(__THIS_CLI_BUFFER, CLI_BUFFER_LEN);
        if (NULL==buf) {
            return buf;
        }
        buf->size = CLI_BUFFER_LEN;
        __THIS_CLI_BUFFER = buf;
    }
    
    return __THIS_CLI_BUFFER;
}

#define cli_buffer_clear()  do{ \
    cli_buffer_len     = 0;     \
    cli_buffer_buf[0]  = 0;     \
}while(0)

static inline int
cli_buffer_error(int err)
{
    cli_buffer_err = err;

    return err;
}
#define cli_buffer_ok       cli_buffer_error(0)

static inline int
cli_vsprintf(const char *fmt, va_list args)
{
    va_list copy;

    va_copy(copy, args);
    uint32 vsize = os_vsprintf_size((char *)fmt, copy);
    va_end(copy);
    
#if CLI_SOCK_TYPE==SOCK_STREAM
    if (cli_buffer_left < vsize) {
        __this_cli_buffer_expand(vsize);
    }
#endif

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
    cli_buffer_cursor[0] = 0;

    return len;
}

static inline int
cli_sprintf(const char *fmt, ...)
{
    int len = 0;
    va_list args;
    
    va_start(args, (char *)fmt);
    len = cli_vsprintf(fmt, args);
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

static int
____cli_d_handle(char *line, cli_table_t *table, int count, int (*reply)(int err))
{
    char *method = line;
    char *args   = line;
    int err;
        
    os_str_strim_both(method, NULL);
    os_str_reduce(method, NULL);

    cli_shift(args);

    cli_buffer_clear();
    err = cli_line_handle(table, count, method, args, reply);

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
    socklen_t addrlen = sizeof(client);
    int len, err;

#if CLI_SOCK_TYPE==SOCK_STREAM
    fd = accept(fd, (sockaddr_t *)&client, &addrlen);
    if (fd<0) {
        return fd;
    }
    
    len = __io_recv(fd, buf, sizeof(buf), 0);
    if (len<0) { /* yes, <0 */
        return len;
    }
#else
    len = __io_recvfrom(fd, buf, sizeof(buf), 0, (sockaddr_t *)&client, &addrlen);
    if (len<0) { /* yes, <0 */
        return len;
    }
#endif

    buf[len] = 0;
    
    if (is_abstract_sockaddr(&client)) {
        set_abstract_sockaddr_len(&client, addrlen);

        debug_cli("recv request from:%s", get_abstract_path(&client));
    }
    debug_cli("recv request[%d]:%s", len, buf);
    
    int reply(int err)
    {
        cli_buffer_err = err;

        debug_cli("send reply[%d]:%s", cli_buffer_space, (char *)__this_cli_buffer());

#if CLI_SOCK_TYPE==SOCK_STREAM
        return io_send(fd, (char *)__this_cli_buffer(), cli_buffer_space);
#else
        return io_sendto(fd, (char *)__this_cli_buffer(), cli_buffer_space, (sockaddr_t *)&client, addrlen);
#endif
    }
    
    err = ____cli_d_handle(buf, table, count, reply);

#if CLI_SOCK_TYPE==SOCK_STREAM
    close(fd);
#endif

    return err;
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
    int fd, err, len;
    
    fd = socket(AF_UNIX, CLI_SOCK_TYPE, 0);
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
#if CLI_SOCK_TYPE==SOCK_STREAM
        err = io_recv(fd, (char *)__this_cli_buffer(), sizeof(cli_buffer_t), timeout);
        if (err<0) { /* yes, <0 */
            goto error;
        }
        
        __this_cli_buffer_promise();
        
        err = io_recv(fd, cli_buffer_buf, cli_buffer_len, timeout);
        if (err<0) { /* yes, <0 */
            goto error;
        }
        cli_buffer_cursor[0] = 0;
#else
        err = io_recv(fd, (char *)__this_cli_buffer(), CLI_BUFFER_SIZE, timeout);
        if (err<0) { /* yes, <0 */
            goto error;
        }
#endif

        if (0==cli_buffer_err && cli_buffer_len) {
            os_printf("%s", cli_buffer_buf);
        }

        debug_trace("action:%s, error:%d, len:%d, buf:%s", 
            buf,
            cli_buffer_err,
            cli_buffer_len,
            cli_buffer_buf);
        err = cli_buffer_err;
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
    
    fd = socket(AF_UNIX, CLI_SOCK_TYPE, 0);
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

#if CLI_SOCK_TYPE==SOCK_STREAM
    listen(fd, 0);
#endif

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
