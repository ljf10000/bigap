#ifndef __IO_H_cbe4cb82b7a74230bc3204daea2c6f49__
#define __IO_H_cbe4cb82b7a74230bc3204daea2c6f49__
/******************************************************************************/
#include "utils/time.h"
/******************************************************************************/
#ifdef __APP__

extern int
__io_read(int fd, void *buf, int size);

extern int
__io_recvfrom(int fd, void *buf, int size, int flag, sockaddr_t *addr, socklen_t *paddrlen);

extern int
__io_recv(int fd, void *buf, int size, int flag);

extern int
io_read(int fd, void *buf, int size, int timeout /* ms */);

extern void
io_drop(int fd);

extern int
io_write(int fd, void *buf, int len);

extern int
io_recvfrom(int fd, void *buf, int size, int timeout /* ms */, sockaddr_t *addr, socklen_t *paddrlen);

extern int
io_sendto(int fd, void *buf, int len, sockaddr_t *addr, int addrlen);

extern int
io_recv(int fd, void *buf, int size, int timeout /* ms */);

extern int
io_send(int fd, void *buf, int len);

extern void
__iov_dump(struct iovec *iov, int count);

extern int
__iov_dump_error(int error, struct iovec *iov, int count);

#ifndef IOV_MAX
#define IOV_MAX     32
#endif

extern int
io_vsendmsg_l(int fd, sockaddr_t *addr, int size, va_list args);

extern int
io_sendmsg_l(int fd, sockaddr_t *addr, int size, ...);

extern int
io_sendmsg(int fd, sockaddr_t *addr, int size, struct iovec *iov, int count);

extern int
io_writev(int fd, struct iovec *iov, int count);

typedef struct sock_server {
    int fd;
    int id;
    os_sockaddr_t addr;
    
    int (*init)(struct sock_server *server);
    int (*handle)(struct sock_server *server);
} sock_server_t;

#define __SOCK_SERVER_INITER(_id, _addr, _init, _handle) { \
    .id     = _id,          \
    .fd     = INVALID_FD,   \
    .addr   = _addr,        \
    .init   = _init,        \
    .handle = _handle,      \
} /* end */

#define SOCK_SERVER_INITER(_id, _family, _init, _handle) \
    __SOCK_SERVER_INITER(_id, OS_SOCKADDR_INITER(_family), _init, _handle)

#define SOCK_USERVER_INITER(_id, _name, _init, _handle) \
    __SOCK_SERVER_INITER(_id, OS_SOCKADDR_ABSTRACT_INITER(_name), _init, _handle)

extern int
sock_servers_init(sock_server_t *server[], int count);

#ifndef USE_SOCK_SERVER_SELECT
#define USE_SOCK_SERVER_SELECT  PC_VAL(1, 0)
#endif

#if USE_SOCK_SERVER_SELECT
extern int
sock_servers_run(sock_server_t *server[], int count);
#endif

#endif
/******************************************************************************/
#endif /* __IO_H_cbe4cb82b7a74230bc3204daea2c6f49__ */
