#ifndef __LOOP_H_71254387166c488dbb36aa9ed0f8b379__
#define __LOOP_H_71254387166c488dbb36aa9ed0f8b379__
/******************************************************************************/
#ifndef LOOP_FDLIMIT
#define LOOP_FDLIMIT        1024
#endif

#ifndef LOOP_FDGROW
#define LOOP_FDGROW         1024
#endif

#if 1
#define LOOP_LIST(_) \
    _(LOOP_TYPE_SIGNAL, 0, "signal"),   \
    _(LOOP_TYPE_TIMER,  1, "timer"),    \
    _(LOOP_TYPE_NORMAL, 2, "normal"),   \
    _(LOOP_TYPE_FATHER, 3, "father"),   \
    _(LOOP_TYPE_SON,    4, "son"),      \
    /* end */

static inline bool is_good_loop_type(int id);
static inline char *loop_type_string(int id);
static inline int loop_type_idx(char *name);
DECLARE_ENUM(loop_type, LOOP_LIST, LOOP_TYPE_END);

#define LOOP_TYPE_SIGNAL    LOOP_TYPE_SIGNAL
#define LOOP_TYPE_TIMER     LOOP_TYPE_TIMER
#define LOOP_TYPE_NORMAL    LOOP_TYPE_NORMAL
#define LOOP_TYPE_FATHER    LOOP_TYPE_FATHER
#define LOOP_TYPE_SON       LOOP_TYPE_SON
#define LOOP_TYPE_END       LOOP_TYPE_END
#endif

struct loop_watcher;

typedef int loop_timer_f(struct loop_watcher *watcher, uint32 times);
typedef int loop_signal_f(struct loop_watcher *watcher, struct signalfd_siginfo *siginfo);
typedef int loop_normal_f(struct loop_watcher *watcher);
typedef int loop_son_f(struct loop_watcher *watcher);

typedef struct loop_watcher {
    int fd;
    int father;
    uint16 type;
    uint16 flag;

    union {
        loop_timer_f    *timer;
        loop_signal_f   *signal;
        loop_normal_f   *normal;
        loop_son_f      *son;
        
        void *cb;
    } cb;
} loop_watcher_t;

#define LOOP_WATCHER_INITER(_fd, _type, _cb) { \
    .fd     = _fd, \
    .father = INVALID_FD, \
    .type   = _type, \
    .flag   = 0, \
    .cb     = { .cb = _cb }, \
}   /* end */

typedef struct {
    int efd;

    autoarray_t watcher;

    uint32 count[LOOP_TYPE_END];
    
    struct itimerspec tm;
} loop_t;

#define LOOP_INITER     {   \
    .efd    = INVALID_FD,   \
    .count  = {             \
        [0 ... (LOOP_TYPE_END-1)] = INVALID_FD, \
    }                       \
}   /* end */

static inline bool
__is_good_loop(loop_t *loop)
{
    return loop && is_good_fd(loop->efd);
}

static inline bool
__is_good_loop_watcher(loop_watcher_t *watcher)
{
    return watcher && is_good_fd(watcher->fd);
}

static inline int
__loop_fd_add(loop_t *loop, int fd)
{
    struct epoll_event ev;
    
    ev.events   = EPOLLIN;
    ev.data.fd  = fd;
    
    return epoll_ctl(loop->efd, EPOLL_CTL_ADD, fd, &ev);
}

static inline int
__loop_fd_del(loop_t *loop, int fd)
{
    return epoll_ctl(loop->efd, EPOLL_CTL_DEL, fd, NULL);
}

static inline void
__loop_watcher_constructor(void *item)
{
    loop_watcher_t *watcher = (loop_watcher_t *)item;

    os_objzero(watcher);
    watcher->fd = INVALID_FD;
}

static inline void
__loop_watcher_destructor(void *item)
{
    loop_watcher_t *watcher = (loop_watcher_t *)item;

    if (is_good_fd(watcher->fd)) {
        os_close(watcher->fd);
    }

    __loop_watcher_constructor(watcher);
}

static inline int
__loop_watcher_fini(loop_t *loop)
{
    if (loop->watcher.base) {
        os_aa_clean(&loop->watcher);
    }

    return 0;
}

static inline int
__loop_watcher_init(loop_t *loop, uint32 limit)
{   
    if (NULL==loop->watcher.base) {
        if (0==limit) {
            limit = LOOP_FDLIMIT;
        }
        
        int err = os_aa_init(&loop->watcher,
            sizeof(loop_watcher_t),     // size
            limit,                      // count
            limit,                      // limit
            LOOP_FDGROW,                // grow
            __loop_watcher_constructor, // init
            __loop_watcher_destructor); // clean
        if (err<0) {
            debug_error("loop watcher init error:%d", err);

            return err;
        }

        debug_ok("loop watcher init ok.");
    }

    return 0;
}

static inline loop_watcher_t *
__loop_watcher_take(loop_t *loop, int fd)
{
    if (false==is_good_fd(fd)) {
        return NULL;
    }

    loop_watcher_t *watcher = os_aa_get(&loop->watcher, fd, false);

    return watcher;
}

static inline loop_watcher_t *
__loop_watcher(loop_t *loop, int fd)
{
    loop_watcher_t *watcher = __loop_watcher_take(loop, fd);
    if (__is_good_loop_watcher(watcher)) {
        return watcher;
    } else {
        return NULL;
    }
}

static inline int
__loop_watcher_add(loop_t *loop, loop_watcher_t *w)
{
    int err;
    
    loop_watcher_t *watcher = __loop_watcher_take(loop, w->fd);
    if (NULL==watcher) {
        debug_trace("not found watcher:%d", w->fd);
        
        return -ENOSPACE;
    }
    else if (__is_good_loop_watcher(watcher)) {
        debug_trace("exist watcher:%d", w->fd);
        
        return -EEXIST;
    }
    
    os_objcpy(watcher, w);
    
    err = __loop_fd_add(loop, w->fd);
    if (err<0) {
        return err;
    }
    loop->count[w->type]++;
    
    return 0;
}

static inline int
__loop_watcher_del(loop_t *loop, int fd)
{
    loop_watcher_t *watcher = __loop_watcher(loop, fd);
    if (NULL==watcher) {
        return -ENOEXIST;
    }

    loop->count[watcher->type]--;
    __loop_watcher_destructor(watcher);

    return 0;
}

static inline int
__loop_master_init(loop_t *loop)
{
    if (false==is_good_fd(loop->efd)) {
        loop->efd = epoll_create1(EPOLL_CLOEXEC);
        if (loop->efd<0) {
            return -errno;
        }

        debug_ok("loop master init ok.");
    }

    return 0;
}

#define __loop_fini(_loop)  do{ \
    __loop_watcher_fini(_loop); \
    os_close((_loop)->efd);     \
}while(0)

static inline int
__loop_init(loop_t *loop)
{
    int err;
    
    err = __loop_master_init(loop);
    if (err<0) {
        return err;
    }
    
    err = __loop_watcher_init(loop, 0);
    if (err<0) {
        return err;
    }
    
    return 0;
}

static inline int
__loop_add_timer(loop_t *loop, loop_timer_f *cb, struct itimerspec *timer)
{
    int err;
    
    __loop_init(loop);

    int fd = timerfd_create(CLOCK_MONOTONIC, EFD_CLOEXEC);
    if (fd<0) {
        return -errno;
    }

    loop->tm = *timer;
    if (0==loop->tm.it_value.tv_sec && 0==loop->tm.it_value.tv_nsec) {
        loop->tm.it_value.tv_nsec = 1;
    }

    err = timerfd_settime(fd, 0, &loop->tm, NULL);
    if (err<0) {
        debug_error("create timer fd error:%d", -errno);
        
        return -errno;
    }

    loop_watcher_t w = LOOP_WATCHER_INITER(fd, LOOP_TYPE_TIMER, cb);
    err = __loop_watcher_add(loop, &w);
    if (err<0) {
        return err;
    }
    
    return 0;
}

static inline int
__loop_add_signal(loop_t *loop, loop_signal_f *cb, int sigs[], int count)
{
    int i, err;
    sigset_t set;
    
    __loop_init(loop);

    sigemptyset(&set);
    for (i=0; i<count; i++) {
        sigaddset(&set, sigs[i]);
    }
    sigprocmask(SIG_SETMASK, &set, NULL);

    int fd = signalfd(-1, &set, EFD_CLOEXEC);
    if (fd<0) {
        debug_error("create signal fd error:%d", -errno);
        
        return -errno;
    }

    loop_watcher_t w = LOOP_WATCHER_INITER(fd, LOOP_TYPE_SIGNAL, cb);
    err = __loop_watcher_add(loop, &w);
    if (err<0) {
        return err;
    }

    return 0;
}

static inline int
__loop_add_normal(loop_t *loop, int fd, loop_normal_f *cb)
{
    int err;
    
    __loop_init(loop);

    loop_watcher_t w = LOOP_WATCHER_INITER(fd, LOOP_TYPE_NORMAL, cb);
    err = __loop_watcher_add(loop, &w);
    if (err<0) {
        return err;
    }
    
    return 0;
}

static inline int
__loop_add_father(loop_t *loop, int fd, loop_son_f *cb)
{
    int err;
    
    __loop_init(loop);

    loop_watcher_t w = LOOP_WATCHER_INITER(fd, LOOP_TYPE_FATHER, cb);
    err = __loop_watcher_add(loop, &w);
    if (err<0) {
        return err;
    }
    
    return 0;
}

static inline int
__loop_add_son(loop_t *loop, int fd, int father, loop_son_f *cb)
{
    int err;
    
    __loop_init(loop);

    loop_watcher_t w = LOOP_WATCHER_INITER(fd, LOOP_TYPE_SON, cb);
    w.father = father;
    
    err = __loop_watcher_add(loop, &w);
    if (err<0) {
        return err;
    }
    
    return 0;
}

static inline void
__loop_signal_handle(loop_watcher_t *watcher)
{
    struct signalfd_siginfo siginfo;

    os_objzero(&siginfo);

    int len = read(watcher->fd, &siginfo, sizeof(siginfo));
    if (len==sizeof(siginfo)) {
        (*watcher->cb.signal)(watcher, &siginfo);
    }
}

static inline void
__loop_timer_handle(loop_watcher_t *watcher)
{
    uint64 val = 0;
    
    int len = read(watcher->fd, &val, sizeof(val));
    if (len==sizeof(val)) {
        (*watcher->cb.timer)(watcher, (uint32)val);
    }
}

static inline void
__loop_normal_handle(loop_watcher_t *watcher)
{
    (*watcher->cb.normal)(watcher);
}

static inline void
__loop_father_handle(loop_t *loop, loop_watcher_t *watcher)
{
    int fd;
    struct sockaddr addr;
    socklen_t addrlen;
    
    fd = accept(watcher->fd, &addr, &addrlen);
    if (is_good_fd(watcher->fd)) {
        __loop_add_son(loop, fd, watcher->fd, watcher->cb.cb);
    }
}

static inline int
__loop_handle_ev(loop_t *loop, struct epoll_event *ev)
{
    loop_watcher_t *watcher = __loop_watcher(loop, ev->data.fd);
    if (NULL==watcher) {
        return -ENOEXIST;
    }

    switch(watcher->type) {
        case LOOP_TYPE_SIGNAL:
            __loop_signal_handle(watcher);
            
            break;
        case LOOP_TYPE_TIMER:
            __loop_timer_handle(watcher);
            
            break;
        case LOOP_TYPE_FATHER:
            __loop_father_handle(loop, watcher);
            
            break;
        case LOOP_TYPE_SON: /* down */
        case LOOP_TYPE_NORMAL:
            __loop_normal_handle(watcher);
            
            break;
        default:
            return -EBADFD;
    }

    return 0;
}

static inline int
__loop_handle(loop_t *loop)
{
    struct epoll_event evs[32];
    int i;
    
    int nfds = epoll_wait(loop->efd, evs, os_count_of(evs), -1);
    if (nfds<0) {
        if (EINTR==errno) {
            return 0;
        } else {
            return -errno;
        }
    }

    for (i=0; i<nfds; i++) {
        __loop_handle_ev(loop, &evs[i]);
    }

    return 0;
}

static inline int
os_loop_init(loop_t *loop)
{
    if (loop) {
        return __loop_init(loop);
    } else {
        return -EINVAL0;
    }
}

static inline void
os_loop_fini(loop_t *loop)
{
    if (loop) {
        __loop_fini(loop);
    }
}

static inline int
os_loop_del_watcher(loop_t *loop, int fd)
{
    if (loop) {
        return __loop_watcher_del(loop, fd);
    } else {
        return -EINVAL0;
    }
}

static inline int
os_loop_add_timer(loop_t *loop, loop_timer_f *cb, struct itimerspec *timer)
{
    if (loop) {
        return __loop_add_timer(loop, cb, timer);
    } else {
        return -EINVAL0;
    }
}

static inline int
os_loop_add_signal(loop_t *loop, loop_signal_f *cb, int sigs[], int count)
{
    if (loop) {
        return __loop_add_signal(loop, cb, sigs, count);
    } else {
        return -EINVAL0;
    }
}

static inline int
os_loop_add_normal(loop_t *loop, int fd, loop_normal_f *cb)
{
    if (loop) {
        return __loop_add_normal(loop, fd, cb);
    } else {
        return -EINVAL0;
    }
}

static inline int
os_loop_add_father(loop_t *loop, int fd, loop_son_f *cb)
{
    if (loop) {
        return __loop_add_father(loop, fd, cb);
    } else {
        return -EINVAL0;
    }
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
