#ifndef __CLI_H_277ca663cad74dd5ad59851d69c58e0c__
#define __CLI_H_277ca663cad74dd5ad59851d69c58e0c__
/******************************************************************************/
#ifdef __APP__
#ifndef SIMPILE_RESPONSE_SIZE
#define SIMPILE_RESPONSE_SIZE       (256*1024-1-sizeof(int)-3*sizeof(uint32_t))
#endif

typedef struct {
    uint32_t len, r0, r1;
    int err;
    char buf[1+SIMPILE_RESPONSE_SIZE];
} simpile_response_t;

#define simpile_res_hsize           offsetof(simpile_response_t, buf)

#define DECLARE_FAKE_SIMPILE_RES    extern simpile_response_t __THIS_IPCBUFFER
#define DECLARE_REAL_SIMPILE_RES    simpile_response_t __THIS_IPCBUFFER

#ifdef __BUSYBOX__
#define DECLARE_SIMPILE_RES         DECLARE_FAKE_SIMPILE_RES
#else
#define DECLARE_SIMPILE_RES         DECLARE_REAL_SIMPILE_RES
#endif

DECLARE_FAKE_SIMPILE_RES;

static inline simpile_response_t *
__simpile_res(void)
{
#ifdef __SIMPILE_RES__
    return &__THIS_IPCBUFFER;
#else
    return NULL;
#endif
}

#define simpile_res_err     __simpile_res()->err
#define simpile_res_buf     __simpile_res()->buf
#define simpile_res_len     __simpile_res()->len
#define simpile_res_size    (simpile_res_hsize + simpile_res_len)
#define simpile_res_left    (SIMPILE_RESPONSE_SIZE - simpile_res_size)

#define simpile_res_zero()  os_objzero(__simpile_res())
#define simpile_res_clear() do{ \
    simpile_res_buf[0]  = 0;    \
    simpile_res_len     = 0;    \
}while(0)
#define simpile_res_error(_err)     (simpile_res_err=(_err))
#define simpile_res_ok              simpile_res_error(0)

#define simpile_res_sprintf(_fmt, _args...)     ({  \
    int __len = 0;                                  \
    if (simpile_res_len < SIMPILE_RESPONSE_SIZE) {  \
        __len = os_snprintf(                        \
            simpile_res_buf + simpile_res_len,      \
            simpile_res_left,                       \
            _fmt, ##_args);                         \
        if (__len <= simpile_res_left) {            \
            simpile_res_len += __len;               \
        } else {                                    \
            __len = 0;                              \
        }                                           \
    }                                               \
                                                    \
    __len;                                          \
})  /* end */

#define simpile_res_recv(_fd, _timeout) \
    io_recv(_fd, (char *)__simpile_res(), sizeof(simpile_response_t), _timeout)

#define simpile_res_send(_fd) \
    io_send(_fd, (char *)__simpile_res(), simpile_res_size)

#define simpile_res_recvfrom(_fd, _timeout, _addr, _paddrlen) \
    io_recvfrom(_fd, (char *)__simpile_res(), sizeof(simpile_response_t), _timeout, _addr, _paddrlen)

#define simpile_res_sendto(_fd, _addr, _addrlen) \
    io_sendto(_fd, (char *)__simpile_res(), simpile_res_size, _addr, _addrlen)

typedef struct {
    int timeout;
    
    sockaddr_un_t server, client;
} simpile_client_t;

#define SIMPILE_CLIENT_INITER(_timeout, _server_path) { \
    .server     = OS_SOCKADDR_UNIX("\0" _server_path),  \
    .client     = OS_SOCKADDR_UNIX(__empty),            \
    .timeout    = _timeout,                             \
}   /* end */

static int
____simpile_d_handle(char *line, cmd_table_t *table, int count, int (*after)(void))
{
    char *method = line;
    char *args   = line;
    int err;
    
    simpile_res_zero();
    
    os_str_strim_both(method, NULL);
    os_str_reduce(method, NULL);
    
    os_cmd_shift(args);
    
    err = cmd_line_handle(table, count, method, args, after);
    simpile_res_error(err);

    debug_trace("action:%s %s, error:%d, len:%d, buf:%s", 
        method, args?args:__empty,
        simpile_res_err,
        simpile_res_len,
        simpile_res_buf);

    return err;
}

static inline int
__simpile_d_handle(int fd, cmd_table_t *table, int count)
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
        return simpile_res_sendto(fd, (sockaddr_t *)pclient, addrlen);
    }
    
    return ____simpile_d_handle(buf, table, count, after);
}

#define simpile_d_handle(_fd, _table) \
    __simpile_d_handle(_fd, _table, os_count_of(_table))

static int
__simpile_c_handle(
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
        err = simpile_res_recv(fd, timeout);
        if (err<0) { /* yes, <0 */
            goto error;
        }

        if (0==simpile_res_err && simpile_res_len && is_good_str(simpile_res_buf)) {
            os_println("%s", simpile_res_buf);
        }

        debug_trace("action:%s, error:%d, len:%d, buf:%s", 
            buf,
            simpile_res_err,
            simpile_res_len,
            simpile_res_buf);
        err = shell_error(simpile_res_err);
    }
    
error:
    if (is_good_fd(fd)) {
        close(fd);
    }
    
    return err;
}

static int
simpile_c_handle(
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

    return __simpile_c_handle(syn, buf, server, client, timeout);
}

typedef struct simpile_server {
    int fd;
    os_sockaddr_t addr;
    
    int (*init)(struct simpile_server *server);
    int (*handle)(struct simpile_server *server);
} simpile_server_t;

static inline int
simpile_u_server_init(simpile_server_t *server)
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

    int size = 1+SIMPILE_RESPONSE_SIZE;
    err = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
    if (err<0) {
        debug_error("setsockopt error:%d", -errno);
        return -errno;
    }

    server->fd = fd;
    
    return 0;
}

static inline int
__simpile_server_init(simpile_server_t *server[], int count)
{
    int i, err;

    for (i=0; i<count; i++) {
        err = (*server[i]->init)(server[i]);
        if (err) {
            return err;
        }
    }
    
    return 0;
}
#define simpile_server_init(_server)  __simpile_server_init(_server, os_count_of(_server))

static inline int
__simpile_server_fdmax(simpile_server_t *server[], int count)
{
    int i, fdmax = 0;
    
    for (i=0; i<count; i++) {
        fdmax = os_max(fdmax, server[i]->fd);
    }

    return fdmax;
}

static inline void
__simpile_server_prepare(simpile_server_t *server[], int count, fd_set *set)
{
    int i;
    
    FD_ZERO(set);
    for (i=0; i<count; i++) {
        FD_SET(server[i]->fd, set);
    }
}

static inline int
____simpile_server_handle(simpile_server_t *server[], int count, fd_set *set)
{
    int i, err;
    
    for (i=0; i<count; i++) {
        if (FD_ISSET(server[i]->fd, set)) {
            err = (*server[i]->handle)(server[i]);
            if (err) {
                /* log, but no return */
            }
        }
    }

    return 0;
}

static inline int
__simpile_server_handle(simpile_server_t *server[], int count)
{
    fd_set rset;
    int i, err, fdmax = __simpile_server_fdmax(server, count);

    while(1) {
        __simpile_server_prepare(server, count, &rset);
        
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
                err = ____simpile_server_handle(server, count, &rset);
                if (err) {
                    return err;
                }
                
                break;
        }
    }

    return 0;
}

#define simpile_server_handle(_server)  __simpile_server_handle(_server, os_count_of(_server))

#endif /* __APP__ */
/******************************************************************************/
#endif /* __CLI_H_277ca663cad74dd5ad59851d69c58e0c__ */
