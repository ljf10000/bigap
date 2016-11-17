#ifndef __FD_H_9a5aee0aa8d54fa2b8054102fd51ec21__
#define __FD_H_9a5aee0aa8d54fa2b8054102fd51ec21__
/******************************************************************************/
#if defined(__APP__) && defined(__EXTEND__)
#ifndef FD_INIT_COUNT
#define FD_INIT_COUNT       1024
#endif

#ifndef FD_GROW_COUNT
#define FD_GROW_COUNT       1024
#endif

#ifndef FD_LIMIT
#define FD_LIMIT            (1024*1024)
#endif

#ifndef FD_READTIMEOUT
#define FD_READTIMEOUT      5000 /* ms */
#endif

#ifndef FD_WRITETIMEOUT
#define FD_WRITETIMEOUT     3000 /* ms */
#endif

#if 0
enum EPOLL_EVENTS
{
    EPOLLIN         = 0x001,
    EPOLLPRI        = 0x002,
    EPOLLOUT        = 0x004,
    EPOLLERR        = 0x008,
    EPOLLHUP        = 0x010,
#endif

#if 0
    EPOLLRDNORM     = 0x040,
    EPOLLRDBAND     = 0x080,
    EPOLLWRNORM     = 0x100,
    EPOLLWRBAND     = 0x200,
    EPOLLMSG        = 0x400,
#endif
#if 0
    EPOLLRDHUP      = 0x2000,
#endif

#define FD_F_BIND       (1<<16) //0x00010000
#define FD_F_LISTEN     (1<<17) //0x00020000
#define FD_F_CONNECT    (1<<18) //0x00040000
#define FD_F_SHARE      (1<<19) //0x00080000
#define FD_F_FILE       (1<<20) //0x00100000
#define FD_F_SOCK       (1<<21) //0x00200000
#define FD_F_EPFD       (1<<21) //0x00400000

#if 0
    EPOLLONESHOT    = 1u << 30,
    EPOLLET         = 1u << 31
};
#endif

#define FD_SO_SHARE        0x4000// setsockopt
#define FD_SO_READTIMEOUT  0x4001
#define FD_SO_WRITETIMEOUT 0x4002

/* 
* should be embed in your struct 
*/
typedef struct loop_node {
    int (*cb)(int fd, int event, struct loop_node *node);
} loop_node_t;

typedef struct {
    loop_node_t loop;
    loop_node_t *node;
    uint32 events;
    uint32 flag;
    co_id_t coid;
    co_cred_t cred;
    int read_timeout;
    int write_timeout;
} fd_map_t;
/******************************************************************************/
extern int
fd_add_watcher(int fd, int events, loop_node_t *node);

extern int
fd_mod_watcher(int fd, int events);

extern int
fd_del_watcher(int fd);

extern void
fd_loop(void);

extern co_id_t
fd_looper(void);

extern bool
fd_is_sock(int fd);

extern bool
fd_is_good(int fd);

extern int
fd_open(const char *filename, int flags, ...);

extern int
fd_socket(int domain, int type, int protocol);

extern int
fd_close(int fd);

extern int
fd_bind(int fd, sockaddr_t *addr, socklen_t length);

extern int
fd_connect(int fd, sockaddr_t *addr, socklen_t length);

extern int
fd_listen(int fd, int n);

extern int
fd_accept(int fd, sockaddr_t *addr, socklen_t *length);

extern ssize_t
fd_write(int fd, const void *buffer, size_t size);

extern ssize_t
fd_read(int fd, void *buffer, size_t size);

extern ssize_t
fd_send(int fd, const void *buffer, size_t size, int flags);

extern ssize_t 
fd_recv(int fd, void *buffer, size_t size, int flags);

extern ssize_t
fd_sendto(int fd, const void *buffer, size_t size, int flags, sockaddr_t *addr, socklen_t length);

extern ssize_t
fd_recvfrom(int fd, void *buffer, size_t size, int flags, sockaddr_t *addr, socklen_t *length);

extern ssize_t
fd_sendmsg(int fd, const struct msghdr *message, int flags);

extern ssize_t
fd_recvmsg(int fd, struct msghdr *message, int flags);

extern int
fd_getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlen);

extern int
fd_setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen);

extern int
fd_shutdown(int fd, int how);

extern void
fd_fini(void);

extern void
fd_init(void);
#endif
/******************************************************************************/
#endif /* __FD_H_9a5aee0aa8d54fa2b8054102fd51ec21__ */
