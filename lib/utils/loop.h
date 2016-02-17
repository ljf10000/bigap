#ifndef __LOOP_H_71254387166c488dbb36aa9ed0f8b379__
#define __LOOP_H_71254387166c488dbb36aa9ed0f8b379__
/******************************************************************************/
typedef struct loop_sig {
    int fd;
    int *number;
    int count;

    int (*init)(struct loop_sig *loop);
    int (*cb)(struct signalfd_siginfo *siginfo);
} loop_sig_t;

#define LOOP_SIG_INITER(_sigs, _sigs_count, _init, _cb) {   \
    .fd     = INVALID_FD,   \
    .number = _sigs,        \
    .count  = _sigs_count,  \
    .init   = _init,        \
    .cb     = _cb,          \
}

typedef struct loop_timer {
    int fd;

    struct itimerspec new;

    int (*init)(struct loop_timer *loop);
    int (*cb)(uint32_t times);
} loop_timer_t;

#define LOOP_TIMER_INITER(_sec, _nsec, _init, _cb) {   \
    .fd     = INVALID_FD,                       \
    .new    = OS_ITIMESPEC_INITER(_sec, _nsec), \
    .init   = _init,                            \
    .cb     = _cb,                              \
}

typedef struct loop_epoll {
    int fd;

    int *fds;
    int count;

    int (*init)(struct loop_epoll *loop);
    int (*cb)(struct epoll_event *ev);
} loop_epoll_t;

#define LOOP_EPOLL_INITER(_fds, _fds_count, _init, _cb)  {   \
    .fd     = INVALID_FD,   \
    .fds    = _fds,         \
    .count  = _fds_count,   \
    .init   = _init,        \
    .cb     = _cb,          \
}

typedef struct {
    loop_sig_t sig;
    loop_timer_t timer;
    loop_epoll_t epoll;
} loop_t;

#define LOOP_INITER(                                                    \
            _sigs, _sigs_count, _siginit, _sigcb,                       \
            _sec, _nsec, _timerinit, _timercb,                          \
            _fds, _fds_count, _epollinit, _epollcb) {                   \
    .sig    = LOOP_SIG_INITER(_sigs, _sigs_count, _siginit, _sigcb),    \
    .timer  = LOOP_TIMER_INITER(_sec, _nsec, _timerinit, _timercb),     \
    .epoll  = LOOP_EPOLL_INITER(_fds, _fds_count, _epollinit, _epollcb),\
}

static inline bool
__is_use_loop_signal(loop_t *loop)
{
    return loop->sig.cb && loop->sig.number && loop->sig.count;
}

static inline bool
__is_use_loop_timer(loop_t *loop)
{
    return NULL!=loop->timer.cb;
}

static inline bool
__is_use_loop_epoll(loop_t *loop)
{
    return NULL!=loop->epoll.cb;
}

static int
os_loop_add(loop_t *loop, int fd)
{
    struct epoll_event ev;
    
    ev.events   = EPOLLIN;
    ev.data.fd  = fd;
    
    return epoll_ctl(loop->epoll.fd, EPOLL_CTL_ADD, fd, &ev);
}

static int
os_loop_del(loop_t *loop, int fd)
{
    return epoll_ctl(loop->epoll.fd, EPOLL_CTL_DEL, fd, NULL);
}

static int
__loop_signal_init(loop_t *loop)
{
    int i, err;
    sigset_t set;

    if (loop->sig.init) {
        err = (*loop->sig.init)(&loop->sig);
        if (err<0) {
            return err;
        }
    }
    
    sigemptyset(&set);
    for (i=0; i<loop->sig.count; i++) {
        sigaddset(&set, loop->sig.number[i]);
    }
    sigprocmask(SIG_SETMASK, &set, NULL);

    loop->sig.fd = signalfd(-1, &set, 0/*EFD_CLOEXEC*/);
    if (loop->sig.fd<0) {
        return -errno;
    }

    return os_loop_add(loop, loop->sig.fd);
}

static int
__loop_timer_init(loop_t *loop)
{
    int err;
    
    if (loop->timer.init) {
        err = (*loop->timer.init)(&loop->timer);
        if (err<0) {
            return err;
        }
    }
    
    loop->timer.fd = timerfd_create(CLOCK_MONOTONIC, EFD_CLOEXEC);
    if (loop->timer.fd<0) {
        return -errno;
    }

    if (0==loop->timer.new.it_value.tv_sec && 0==loop->timer.new.it_value.tv_nsec) {
        loop->timer.new.it_value.tv_nsec = 1;
    }
    
    int err = timerfd_settime(loop->timer.fd, 0, &loop->timer.new, NULL);
    if (err<0) {
        return -errno;
    }

    return os_loop_add(loop, loop->timer.fd);
}

static int
__loop_epoll_init(loop_t *loop)
{
    int i, err;
    
    if (loop->epoll.init) {
        err = (*loop->epoll.init)(&loop->epoll);
        if (err<0) {
            return err;
        }
    }
    
    if (loop->epoll.fds && loop->epoll.count) {
        for (i=0; i<loop->epoll.count; i++) {
            err = os_loop_add(loop, loop->epoll.fds[i]);
            if (err<0) {
                return err;
            }
        }
    }
    
    return 0;
}

static int
os_loop_init(loop_t *loop)
{
    int err = 0;
    
    loop->epoll.fd = epoll_create1(EPOLL_CLOEXEC);
    if (loop->epoll.fd<0) {
        return -errno;
    }

    if (__is_use_loop_epoll(loop)) {
        err = __loop_epoll_init(loop);
        if (err<0) {
            return err;
        }
    }
    
    if (__is_use_loop_signal(loop)) {
        err = __loop_signal_init(loop);
        if (err<0) {
            return err;
        }
    }

    if (__is_use_loop_timer(loop)) {
        err = __loop_timer_init(loop);
        if (err<0) {
            return err;
        }
    }

    return 0;
}

static void
os_loop_fini(loop_t *loop)
{
    os_close(loop->sig.fd);
    os_close(loop->timer.fd);
    os_close(loop->epoll.fd);
}

static void
__loop_signal_handle(loop_t *loop)
{
    struct signalfd_siginfo siginfo;
    
    int len = read(loop->sig.fd, &siginfo, sizeof(siginfo));
    if (len==sizeof(siginfo)) {
        (*loop->sig.cb)(&siginfo);
    }
}

static void
__loop_timer_handle(loop_t *loop)
{
    uint64_t val = 0;
    
    int len = read(loop->timer.fd, &val, sizeof(val));
    if (len==sizeof(val)) {
        (*loop->timer.cb)((uint32_t)val);
    }
}

static void
__loop_epoll_handle(loop_t *loop, struct epoll_event *ev)
{
    if (loop->epoll.cb) {
        (*loop->epoll.cb)(ev);
    }
}

static int
__loop_handle(loop_t *loop)
{
    struct epoll_event evs[32];
    int i;
    
    int nfds = epoll_wait(loop->epoll.fd, evs, os_count_of(evs), -1);
    if (nfds<0) {
        if (EINTR==errno) {
            return 0;
        } else {
            return -errno;
        }
    }

    for (i=0; i<nfds; i++) {
        struct epoll_event *ev = &evs[i];
        
        if (ev->data.fd==loop->sig.fd) {
            __loop_signal_handle(loop);
        }
        else if (ev->data.fd==loop->timer.fd) {
            __loop_timer_handle(loop);
        }
        else {
            __loop_epoll_handle(loop, ev);
        }
    }

    return 0;
}

static inline int
os_loop(loop_t *loop)
{
    while(1) {
        int err = __loop_handle(loop);
        if (err<0) {
            // log
        }
    }
    
    return 0;
}
/******************************************************************************/
#endif /* __LOOP_H_71254387166c488dbb36aa9ed0f8b379__ */
