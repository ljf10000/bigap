/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#if !defined(__UCLIBC__)

typedef struct {
    bool init;
    int epfd;

    /*
    * idle is the first coroutine
    *   so use 0 as invalid looper
    */
    co_id_t looper;

    /*
    * (map & evs) have same count
    */
    autoarray_t map; /* fd_map_t       */
    autoarray_t evs; /* struct epoll_event */
} fd_control_t;

ALWAYS_INLINE fd_control_t *
__this_fd(void)
{
    static fd_control_t *control;

    if (NULL==control) {
        control = (fd_control_t *)os_zalloc(sizeof(fd_control_t)); 
    }
    
    return control;
}

ALWAYS_INLINE void
__fd_control_init(void)
{
    __this_fd()->epfd = INVALID_FD;
}

#define ____fd_map          (&__this_fd()->map)
#define ____fd_evs          (&__this_fd()->evs)
#define ____fd_count        os_aa_count(____fd_map)
#define ____fd_epfd         __this_fd()->epfd
#define ____fd_looper       __this_fd()->looper
/******************************************************************************/
ALWAYS_INLINE struct epoll_event *
__fd_evs(int idx) 
{ 
    return (struct epoll_event *)os_aa_get(____fd_evs, idx, false); 
}

ALWAYS_INLINE fd_map_t *
__fd_map(int fd, bool grow)
{
    if (grow) {
        /*
        * try grow evs 
        */
        (void)os_aa_get(____fd_evs, fd, true);
    }
    
    return (fd_map_t *)os_aa_get(____fd_map, fd, grow);
}

ALWAYS_INLINE fd_map_t *
__fd_get(int fd)
{
    fd_map_t *map = __fd_map(fd, false);

    return (map && map->flag)?map:NULL;
}

ALWAYS_INLINE bool
__fd_is_sock(fd_map_t *map)
{
    return os_hasflag(map->flag, FD_F_SOCK);
}

STATIC int 
__fd_watcher(int fd, int event, loop_node_t *node)
{
    fd_map_t *map = safe_container_of(node, fd_map_t, loop);

    switch(event) {
        case EPOLLIN:
            break;
        default:
            return 0;
    }
    
    co_ready(map->coid, map->cred, false);

    return 0;
}

STATIC int
__fd_create(int fd, int flag)
{
    fd_map_t *map;

    /* check exist? */
    map = __fd_get(fd);
    if (map) {
        trace_assert(0, "fd(%d) exist", fd);
    }
    
    map = __fd_map(fd, true);
    if (NULL==map) {
        return -ENOMEM;
    }
    map->flag = flag;
    if (____fd_looper) {
        map->coid = co_self();
        map->loop.cb = __fd_watcher;
        map->read_timeout = FD_READTIMEOUT;
        map->write_timeout= FD_WRITETIMEOUT;
    }
    
    /* no block */
    if (__fd_is_sock(map)) {
        os_closexec(fd);
    }
    
    return fd;
}

STATIC void
__fd_loop_handle(void)
{
    co_mask_t mask = co_evmask_read_and_zero();
    
    if (mask) {
        union co_mail mail;
        
        if (os_hasbit(mask, CO_EV_KILL)) {
            //do nothing
        }
        
        if (os_hasbit(mask, CO_EV_MAIL)) {
            while(0==co_mail_recv(&mail)) {
                // handle mail
            }
        }
    }
}

STATIC int 
__fd_epoll_create(int size)
{
    int fd;
    
    fd = epoll_create(size);
    if (is_good_fd(fd)) {
        fd = __fd_create(fd, FD_F_EPFD);
    }

    return fd;
}

STATIC int
__fd_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
{
    return epoll_wait(epfd, events, maxevents, timeout);
}

STATIC void
__fd_loop_once(void)
{
    int i, count;
    int timeout = ____fd_looper?0:1;

    count = __fd_epoll_wait(____fd_epfd, __fd_evs(0), ____fd_count, timeout);
    for  (i=0; i<count; i++) {
        struct epoll_event *evs = __fd_evs(i);
        fd_map_t *map;
        loop_node_t *node;
        uint32 events;
        int err, fd;
        
        if (NULL==evs) {
            trace_assert(0, "evs nill");
            
            continue;
        }
        
        fd = evs->data.fd;
        if (false==IS_GOOD_ENUM(fd, ____fd_count)) {
            trace_assert(0, "bad fd(%d)", fd);
            
            continue;
        }

        map = __fd_get(fd);
        if (NULL==map) {
            trace_assert(0, "no found fd(%d) map", fd);

            continue;
        }
        
        node = map->node;
        if (NULL==node) {
            trace_assert(0, "fd map node is nil");
            
            continue;
        }

        events = evs->events & map->events;
        if (!events) {
            continue;
        }
        
        err = node->cb(fd, events, node);
        if (err<0) {
            // error, log it
            continue;
        }
    }
}

STATIC int
__fd_loop_main(void *no_used)
{
    (void)no_used;
    
    while(1) {
        if (____fd_looper) {
            __fd_loop_handle();
        }
        
        __fd_loop_once();

        if (____fd_looper) {
            co_yield();
        }
    }

    return 0;
}

STATIC int
__fd_add_watcher(int fd, int events, loop_node_t *node)
{
    int err;
    fd_map_t *map;
    struct epoll_event ev = {
        .data = { .fd = fd },
        .events = events,
    };
    
    map = __fd_get(fd);
    if (NULL==map) {
        return os_assertV(-EINVAL1);
    }
    
    if (map->node) {
        return os_assertV(-EEXIST);
    }
    
    ev.events = events;
    err = epoll_ctl(____fd_epfd, EPOLL_CTL_ADD, fd, &ev);
    if (err < 0) {
        return -errno;
    }
    map->node   = node;
    map->events = events;
    
    return 0;
}

STATIC int
__fd_mod_watcher(int fd, int events)
{
    int err;
    fd_map_t *map;
    struct epoll_event ev = {
        .data = { .fd = fd },
    };

    map = __fd_get(fd);
    if (NULL==map) {
        return os_assertV(-EINVAL1);
    }

    if (NULL==map->node) {
        return os_assertV(-EBADF);
    }

    if (events==map->events) {
        return 0;
    }

    ev.events = events;
    err = epoll_ctl(____fd_epfd, EPOLL_CTL_MOD, fd, &ev);
    if (err < 0) {
        return -errno;
    }
    map->events = events;

    return 0;
}

STATIC int
__fd_del_watcher(int fd)
{
    int err;
    fd_map_t *map;
    struct epoll_event ev = {
        .data = { .fd = fd },
        .events = 0xffffffff,
    };

    map = __fd_get(fd);
    if (NULL==map) {
        return os_assertV(-EINVAL1);
    }

    if (NULL==map->node) {
        return os_assertV(-ENOENT);
    }
    
    err = epoll_ctl(____fd_epfd, EPOLL_CTL_DEL, fd, &ev);
    if (err < 0) {
        return -errno;
    }
    map->events = 0;
    map->node   = NULL;
    
    return 0;
}

#define __fd_op(_fd, _op, _flag) do{ \
    fd_map_t *map = __fd_get(_fd);    \
    if (map) {                          \
        map->flag _op (_flag);          \
    }                                   \
}while(0)

STATIC void
__fd_close(int fd)
{
    __fd_op(fd, =, 0);
}

STATIC void
__fd_bind(int fd)
{
    __fd_op(fd, |=, FD_F_BIND);
}

STATIC void
__fd_listen(int fd)
{
    __fd_op(fd, |=, FD_F_LISTEN);
}

STATIC void
__fd_connect(int fd)
{
    __fd_op(fd, |=, FD_F_CONNECT);
}

STATIC int
____fd_read(int fd)
{
    fd_map_t *map;
    
    map = __fd_get(fd);
    if (NULL==map) {
        return os_assertV(-EINVAL1);
    }
    else if (false==__fd_is_sock(map)) {
        return 0;
    }
    
    map->cred = co_cred();

    __fd_add_watcher(fd, EPOLLIN, &map->loop);
    
    return co_suspend(map->read_timeout, map->cred);
}

STATIC int
__fd_getopt(int fd, int level, int optname, void *optval, sockaddr_len_t *optlen)
{
    fd_map_t *map;

    map = __fd_get(fd);
    if (NULL==map) {
        return os_assertV(-EINVAL1);
    }
    else if (false==__fd_is_sock(map)) {
        return 0;
    }
    
    switch(optname) {
        case FD_SO_SHARE:
            *(bool *)optval = os_hasflag(map->flag, FD_F_SHARE);
            
            break;
        case FD_SO_READTIMEOUT:
            *(int *)optval = map->read_timeout;
            
            break;
            
            break;
        case FD_SO_WRITETIMEOUT:
            *(int *)optval = map->write_timeout;
            
            break;
        default:
            break;
    }

    return 0;
}

STATIC int
__fd_setopt(int fd, int level, int optname, const void *optval, sockaddr_len_t optlen)
{
    fd_map_t *map;

    map = __fd_get(fd);
    if (NULL==map) {
        return os_assertV(-EINVAL1);
    }
    else if (false==__fd_is_sock(map)) {
        return 0;
    }
    
    switch(optname) {
        case FD_SO_SHARE:
            map->flag |= FD_F_SHARE;
            
            break;
        case FD_SO_READTIMEOUT:
            map->read_timeout = *(int *)optval;
            
            break;
        case FD_SO_WRITETIMEOUT:
            map->write_timeout= *(int *)optval;
            
            break;
        default:
            break;
    }

    return 0;
}

#define __fd_read(_fd, _buf, _size)    ({   \
    int __fd_read_err = read(_fd, _buf, _size); \
    if (__fd_read_err) {                     \
        debug_error(                        \
            "fd read error(%d) errno(%d)",  \
            __fd_read_err, -errno);         \
    }                                       \
    __fd_read_err;                          \
})  /* end */

#define __fd_recv(_fd, _buf, _size, _flag) ({  \
    int __fd_recv_err = recv(_fd, _buf, _size, _flag);\
    if (__fd_recv_err) {                    \
        debug_error(                        \
            "fd recv error(%d) errno(%d)",  \
            __fd_recv_err, -errno);         \
    }                                       \
    __fd_recv_err;                          \
})  /* end */

#define __fd_recvfrom(_fd, _buf, _size, _flag, _addr, _plen)   ({  \
    int __fd_recvfrom_err = recvfrom(_fd, _buf, _size, _flag, _addr, _plen);  \
    if (__fd_recvfrom_err) {                \
        debug_error(                        \
            "fd recvfrom error(%d) errno(%d)", \
            __fd_recvfrom_err, -errno);     \
    }                                       \
    __fd_recvfrom_err;                      \
})  /* end */

#define __fd_recvmsg(_fd, _msg, _flag) ({   \
    int __fd_recvmsg_err = recvmsg(_fd, _msg, _flag);    \
    if (__fd_recvmsg_err) {                 \
        debug_error(                        \
            "fd recvmsg error(%d) errno(%d)", \
            __fd_recvmsg_err, -errno);      \
    }                                       \
    __fd_recvmsg_err;                       \
})  /* end */
/******************************************************************************/
int
fd_add_watcher(int fd, int events, loop_node_t *node)
{
    if (false==is_good_fd(fd)) {
        return os_assertV(-EBADF);
    }
    else if (0==events) {
        return os_assertV(-EINVAL1);
    }
    else if (NULL==node) {
        return os_assertV(-EINVAL2);
    }
    else {
        return __fd_add_watcher(fd, events, node);
    }
}

int
fd_mod_watcher(int fd, int events)
{
    if (false==is_good_fd(fd)) {
        return os_assertV(-EBADF);
    }
    else if (0==events) {
        return os_assertV(-EINVAL1);
    }
    else {
        return __fd_mod_watcher(fd, events);
    }
}

int
fd_del_watcher(int fd)
{
    if (false==is_good_fd(fd)) {
        return os_assertV(-EBADF);
    } else {
        return __fd_del_watcher(fd);
    }
}

void
fd_loop(void)
{
    if (____fd_looper) {
        co_resume(____fd_looper);
    } else {
        __fd_loop_main(NULL);
    }
}

co_id_t
fd_looper(void)
{
    if (0==____fd_looper) {
        ____fd_looper = co_new("looper", __fd_loop_main, NULL, 0, 0, CO_PRI_HIGHEST, false);
    } else {
        trace_assert(0, "fd looper exist");
    }
    
    return ____fd_looper;
}

bool
fd_is_sock(int fd)
{
    fd_map_t *map = __fd_get(fd);

    return map && __fd_is_sock(map);
}

bool
fd_is_good(int fd)
{
    if (is_good_fd(fd)) {
        return NULL!=__fd_get(fd);
    } else {
        return false;
    }
}

int
fd_open(const char *filename, int flags, ...)
{
    mode_t mode = 0;
    int fd;
    
    if (flags & O_CREAT) {
        va_list args;
        
        va_start(args, flags);
        mode = va_arg(args, mode_t);
        va_end(args);
    }
    
    fd = open(filename, flags, mode);
    if (fd<0) {
        return fd;
    } else {
        return __fd_create(fd, FD_F_FILE);
    }
}

int
fd_socket(int domain, int type, int protocol)
{
    int fd;

    fd = socket(domain, type, protocol);
    if (is_good_fd(fd)) {
        os_closexec(fd);
        
        fd = __fd_create(fd, FD_F_SOCK);
    }
    if (fd<0) {
        debug_error("fd create error(%d) errno(%d)", fd, -errno);
    }

    return fd;
}

int
fd_close(int fd)
{
    int err = 0;

    if (is_good_fd(fd)) {
        __fd_close(fd);
        err = os_close(fd);
        if (err<0) {
            debug_error("fd close error(%d) errno(%d)", err, -errno);
        }
    }
    
    return err;
}

int
fd_bind(int fd, sockaddr_t *addr, sockaddr_len_t length)
{
    int err = 0;

    err = bind(fd, addr, length);
    if (0==err) {
        __fd_bind(fd);
    } else {
        debug_error("fd bind error(%d)", err);
    }
    
    return err;
}

int
fd_connect(int fd, sockaddr_t *addr, sockaddr_len_t length)
{
    int err = 0;

    err = connect(fd, addr, length);
    if (0==err) {
        __fd_connect(fd);
    } else {
        debug_error("fd connect error(%d) errno(%d)", err, -errno);
    }
        
    return err;
}

int
fd_listen(int fd, int n)
{
    int err = 0;

    err = listen(fd, n);
    if (0==err) {
        __fd_listen(fd);
    } else {
        debug_error("fd listen error(%d) errno(%d)", err, -errno);
    }

    return err;
}

int
fd_accept(int fd, sockaddr_t *addr, sockaddr_len_t *length)
{
    int err = 0;
    
    err = accept(fd, addr, length);
    if (is_good_fd(err)) {
        err = __fd_create(err, FD_F_SOCK);
    }
    if (err<0) {
        debug_error("fd accept error(%d) errno(%d)", err, -errno);
    }
    
    return err;
}

ssize_t
fd_write(int fd, const void *buffer, size_t size)
{
    int err = 0;

    err = write(fd, buffer, size);
    if (err<0) {
        debug_error("fd write error(%d) errno(%d)", err, -errno);
    }
    
    if (ENOBUFS==errno && fd_is_sock(fd)) {
        /* 
        * no sndbuf space
        *   need to epoll ???
        */
    }
    
    return err;
}

ssize_t
fd_read(int fd, void *buffer, size_t size)
{
    int err = 0;

    if (0==____fd_looper) {
        return __fd_read(fd, buffer, size);
    }
    
    err = ____fd_read(fd);
    if (err<0) {
        // retry ???
        return err;
    }
    else if (1==err) {
        return -ETIMEOUT;// timeout
    }
    else {
        return __fd_read(fd, buffer, size);
    }
}

ssize_t
fd_send(int fd, const void *buffer, size_t size, int flags)
{
    int err = 0;

    err = send(fd, buffer, size, flags);
    if (err<0) {
        debug_error("fd send error(%d) errno(%d)", err, -errno);
    }
    if (ENOBUFS==errno) {
        /* 
        * no sndbuf space
        *   need to epoll ???
        */
    }

    return err;
}

ssize_t 
fd_recv(int fd, void *buffer, size_t size, int flags)
{
    int err = 0;

    if (0==____fd_looper) {
        return __fd_recv(fd, buffer, size, flags);
    }

    err = ____fd_read(fd);
    if (err<0) {
        // retry ???
        return err;
    }
    else if (1==err) {
        return -ETIMEOUT;// timeout
    }
    else {
        return __fd_recv(fd, buffer, size, flags);
    }
}

ssize_t
fd_sendto(int fd, const void *buffer, size_t size, int flags, sockaddr_t *addr, sockaddr_len_t length)
{
    int err = 0;

    err = sendto(fd, buffer, size, flags, addr, length);
    if (err<0) {
        debug_error("fd sendto error(%d) errno(%d)", err, -errno);
    }
    if (ENOBUFS==errno) {
        /* 
        * no sndbuf space
        *   need to epoll ???
        */
    }

    return err;
}

ssize_t
fd_recvfrom(int fd, void *buffer, size_t size, int flags, sockaddr_t *addr, sockaddr_len_t *length)
{
    int err = 0;

    if (0==____fd_looper) {
        return __fd_recvfrom(fd, buffer, size, flags, addr, length);
    }

    err = ____fd_read(fd);
    if (err<0) {
        // retry ???
        return err;
    }
    else if (1==err) {
        return -ETIMEOUT;// timeout
    }
    else {
        return __fd_recvfrom(fd, buffer, size, flags, addr, length);
    }
}

ssize_t
fd_sendmsg(int fd, const struct msghdr *message, int flags)
{
    int err = 0;

    err = sendmsg(fd, message, flags);
    if (err<0) {
        debug_error("fd sendmsg error(%d) errno(%d)", err, -errno);
    }
    if (ENOBUFS==errno) {
        /* 
        * no sndbuf space
        *   need to epoll ???
        */
    }

    return err;
}

ssize_t
fd_recvmsg(int fd, struct msghdr *message, int flags)
{
    int err = 0;

    if (0==____fd_looper) {
        return __fd_recvmsg(fd, message, flags);
    }

    err = ____fd_read(fd);
    if (err<0) {
        // retry ???
        return err;
    }
    else if (1==err) {
        return -ETIMEOUT;// timeout
    }
    else {
        return __fd_recvmsg(fd, message, flags);
    }
}

int
fd_getsockopt(int fd, int level, int optname, void *optval, sockaddr_len_t *optlen)
{
    switch(optname) {
        case FD_SO_SHARE:
        case FD_SO_READTIMEOUT:
        case FD_SO_WRITETIMEOUT:
            return __fd_getopt(fd, level, optname, optval, optlen);
        default:
            return getsockopt(fd, level, optname, optval, optlen);
    }    
}

int
fd_setsockopt(int fd, int level, int optname, const void *optval, sockaddr_len_t optlen)
{
    switch(optname) {
        case FD_SO_SHARE:
        case FD_SO_READTIMEOUT:
        case FD_SO_WRITETIMEOUT:
            return __fd_setopt(fd, level, optname, optval, optlen);
        default:
            return setsockopt(fd, level, optname, optval, optlen);
    }    
}

int
fd_shutdown(int fd, int how)
{
    int err = 0;
    
    err = shutdown(fd, how);
    if (err<0) {
        return err;
    }

    __fd_close(fd);

    return err;
}

/******************************************************************************/
void
fd_fini(void)
{
    if (__this_fd()->init) {
        __this_fd()->init = false;
        
        fd_close(____fd_epfd);
        os_aa_destroy(____fd_map);
        os_aa_destroy(____fd_evs);
        
        co_fini();
    }
}

void
fd_init(void)
{
    int err = 0;

    if (false==__this_fd()->init) {
        __this_fd()->init = true;
        
        co_init();

        __fd_control_init();
        
#define FD_AA_ARGS  FD_INIT_COUNT, FD_LIMIT, FD_GROW_COUNT, NULL, NULL
        err = os_aa_create(____fd_map, sizeof(fd_map_t), FD_AA_ARGS);
        if (err<0) {
            goto error;
        }
        
        err = os_aa_create(____fd_evs, sizeof(struct epoll_event), FD_AA_ARGS);
        if (err<0) {
            goto error;
        }
        
        ____fd_epfd = __fd_epoll_create(FD_INIT_COUNT);
        if (____fd_epfd < 0) {
            goto error;
        }
    }
    
    return;
error:
   fd_fini();
}

#endif
/******************************************************************************/
