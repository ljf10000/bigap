/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
STATIC int
__io_error(char *method, void *buf, int size, int error)
{
    int err = error;
    
    if (err>0) {
        os_dump_buffer_by(__is_ak_debug_packet, buf, err);
    }
    else if (err<0) { /* yes, <0 */
        // debug_io("%s error:%d", method, -errno);

        return -errno;
    }

    return err;
}

STATIC int
__io_read_error(char *method, void *buf, int size, int error)
{
    int err = __io_error(method, buf, size, error);
    if (0==err) {
        // debug_io("%s nothing", method);
    }

    return err;
}

STATIC int
__io_write_error(char *method, void *buf, int size, int error)
{
    int err = __io_error(method, buf, size, error);
    if (err != size) {
        // debug_io("%s count(%d) < length(%d)", method, err, size);

        return -EIO;
    }

    return err;
}

#define __IO_READ(_action, _fd, _buf, _len, _timeout, _call)  ({ \
    fd_set rset;                                        \
    struct timeval tv = {                               \
        .tv_sec     = os_second(_timeout),              \
        .tv_usec    = os_usecond(_timeout),             \
    };                                                  \
    int err;                                            \
                                                        \
    while(1) {                                          \
        FD_ZERO(&rset);                                 \
        FD_SET(fd, &rset);                              \
                                                        \
        err = select(fd + 1, &rset, NULL, NULL, &tv);   \
        if (-1==err) {                                  \
            if (EINTR==errno) {                         \
                /* is breaked, continue */              \
                debug_signal("%s breaked", _action);    \
                                                        \
                continue;                               \
            } else {                                    \
                /* debug_io("%s error:%d", _action, -errno); */ \
                                                        \
                err = -errno;                           \
            }                                           \
        }                                               \
        else if (0==err) {                              \
            debug_timeout("%s timeout", _action);       \
                                                        \
            err = -ETIMEOUT;                            \
        }                                               \
        else {                                          \
            err = _call;                                \
        }                                               \
                                                        \
        break;                                          \
    }                                                   \
                                                        \
    err;                                                \
})

int
__io_read(int fd, void *buf, int size)
{
    int err = 0;

    do { 
        err = read(fd, buf, size);
    } while(err<0 && EINTR==errno);
    
    return __io_read_error("read", buf, size, err);
}

int
__io_recvfrom(int fd, void *buf, int size, int flag, sockaddr_t *addr, socklen_t *paddrlen)
{
    int err = recvfrom(fd, buf, size, flag, addr, paddrlen);

    return __io_read_error("recvfrom", buf, size, err);
}

int
__io_recv(int fd, void *buf, int size, int flag)
{
    int err = recv(fd, buf, size, flag);

    return __io_read_error("recv", buf, size, err);
}

int
io_read(int fd, void *buf, int size, int timeout /* ms */)
{
    return __IO_READ("read", fd, buf, size, timeout, __io_read(fd, buf, size));
}

void
io_drop(int fd)
{
    char buf[1+OS_LINE_LEN];

    while(read(fd, buf, sizeof(buf)) > 0) {
        os_do_nothing();
    }
}

int
io_write(int fd, void *buf, int len)
{
    int err = write(fd, buf, len);

    return __io_write_error("write", buf, len, err);
}

int
io_recvfrom(int fd, void *buf, int size, int timeout /* ms */, sockaddr_t *addr, socklen_t *paddrlen)
{
    return __IO_READ("recvfrom", fd, buf, size, timeout, __io_recvfrom(fd, buf, size, 0, addr, paddrlen));
}

int
io_sendto(int fd, void *buf, int len, sockaddr_t *addr, int addrlen)
{
    int err = sendto(fd, buf, len, 0, addr, addrlen);

    return __io_write_error("sendto", buf, len, err);
}

int
io_recv(int fd, void *buf, int size, int timeout /* ms */)
{
    return __IO_READ("recv", fd, buf, size, timeout, __io_recv(fd, buf, size, 0));
}

int
io_send(int fd, void *buf, int len)
{
    int err = send(fd, buf, len, 0);
    if (err<0) {
        os_println("io_send err=%d errno=%d", err, -errno);
    }
    
    return __io_write_error("send", buf, len, err);
}

void
__iov_dump(struct iovec *iov, int count)
{
    int i;
    bool debug = __is_ak_debug_packet;
    
    if (debug) {
        for (i=0; i<count; i++) {
            os_dump_buffer(iov[i].iov_base, iov[i].iov_len);
        }
    }
}

int
__iov_dump_error(int error, struct iovec *iov, int count)
{
    if (error<0) {
        error = -errno;
    }
    else if (error>0) {
        __iov_dump(iov, count);
    }
    
    return error;
}

int
io_vsendmsg_l(int fd, sockaddr_t *addr, int size, va_list args)
{
    struct iovec iov[IOV_MAX] = {[0 ... (IOV_MAX-1)] = OS_IOVEC_INITER(NULL, 0)};
    void *base;
    int len, count = 0;
    
    while((base = va_arg(args, void *))) {
        len = va_arg(args, int);

        iov[count].iov_base = base;
        iov[count].iov_len  = len;
        count++;
        
        if (count > IOV_MAX) {
            return -ETOOBIG;
        }
    }

    struct msghdr msg = OS_MSGHDR_INITER(iov, count, addr, size);
    
    return __iov_dump_error(sendmsg(fd, &msg, 0), iov, count);
}

int
io_sendmsg_l(int fd, sockaddr_t *addr, int size, ...)
{
    int len = 0;
    va_list args;
    
    va_start(args, size);
    len = io_vsendmsg_l(fd, addr, size, args);
    va_end(args);
    
    return len;
}

int
io_sendmsg(int fd, sockaddr_t *addr, int size, struct iovec *iov, int count)
{
    struct msghdr msg = OS_MSGHDR_INITER(iov, count, addr, size);
        
    return __iov_dump_error(sendmsg(fd, &msg, 0), iov, count);
}

int
io_writev(int fd, struct iovec *iov, int count)
{
    return __iov_dump_error(writev(fd, iov, count), iov, count);
}

int
sock_servers_init(sock_server_t *server[], int count)
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

int
__sock_servers_fdmax(sock_server_t *server[], int count)
{
    int i, fdmax = 0;
    
    for (i=0; i<count; i++) {
        if (server[i]) {
            fdmax = OS_MAX(fdmax, server[i]->fd);
        }
    }

    return fdmax;
}

void
__sock_servers_prepare(sock_server_t *server[], int count, fd_set *set)
{
    int i;
    
    FD_ZERO(set);
    for (i=0; i<count; i++) {
        if (server[i]) {
            FD_SET(server[i]->fd, set);
        }
    }
}

int
__sock_servers_handle(sock_server_t *server[], int count, fd_set *set)
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

int
sock_servers_run(sock_server_t *server[], int count)
{
    fd_set rset;
    int i, err, fdmax = __sock_servers_fdmax(server, count);

    while(1) {
        __sock_servers_prepare(server, count, &rset);
        
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
                err = __sock_servers_handle(server, count, &rset);
                if (err<0) {
                    return err;
                }
                
                break;
        }
    }

    return 0;
}
/******************************************************************************/
