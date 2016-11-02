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
    _(LOOP_TYPE_INOTIFY,0, "inotify"),  \
    _(LOOP_TYPE_SIGNAL, 1, "signal"),   \
    _(LOOP_TYPE_TIMER,  2, "timer"),    \
    _(LOOP_TYPE_NORMAL, 3, "normal"),   \
    _(LOOP_TYPE_FATHER, 4, "father"),   \
    _(LOOP_TYPE_SON,    5, "son"),      \
    /* end */

static inline bool is_good_loop_type(int id);
static inline char *loop_type_string(int id);
static inline int loop_type_idx(char *name);
DECLARE_ENUM(loop_type, LOOP_LIST, LOOP_TYPE_END);

#define LOOP_TYPE_INOTIFY   LOOP_TYPE_INOTIFY
#define LOOP_TYPE_SIGNAL    LOOP_TYPE_SIGNAL
#define LOOP_TYPE_TIMER     LOOP_TYPE_TIMER
#define LOOP_TYPE_NORMAL    LOOP_TYPE_NORMAL
#define LOOP_TYPE_FATHER    LOOP_TYPE_FATHER
#define LOOP_TYPE_SON       LOOP_TYPE_SON
#define LOOP_TYPE_END       LOOP_TYPE_END
#endif

struct loop_watcher;

typedef struct {
    char *path;
    uint32 mask;
} loop_inotify_t;

typedef int loop_timer_f(struct loop_watcher *watcher, uint32 times);
typedef int loop_signal_f(struct loop_watcher *watcher, struct signalfd_siginfo *siginfo);
typedef int loop_normal_f(struct loop_watcher *watcher);
typedef int loop_son_f(struct loop_watcher *watcher);
typedef int loop_inotify_f(struct loop_watcher *watcher, struct inotify_event *ev);

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
        loop_inotify_f  *inotify;
        
        void *cb;
    } cb;
} loop_watcher_t;

static inline void
__loop_watcher_initer(loop_watcher_t *watcher, int fd, int type, void *cb)
{
    watcher->fd     = fd;
    watcher->father = INVALID_FD;
    watcher->type   = type;
    watcher->flag   = 0;
    watcher->cb.cb  = cb;
}

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

static inline loop_watcher_t *
__loop_watcher_add(loop_t *loop, int fd, int type, void *cb)
{
    int err;

    __loop_init(loop);
    
    loop_watcher_t *watcher = __loop_watcher_take(loop, fd);
    if (NULL==watcher) {
        debug_trace("not found watcher:%d", fd);
        
        return NULL;
    }
    else if (__is_good_loop_watcher(watcher)) {
        debug_trace("exist watcher:%d", fd);
        
        return NULL;
    }

    __loop_watcher_initer(watcher, fd, type, cb);
    
    err = __loop_fd_add(loop, fd);
    if (err<0) {
        return err;
    }
    loop->count[type]++;
    
    return watcher;
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
__loop_add_inotify(loop_t *loop, loop_inotify_f *cb, loop_inotify_t inotify[], int count)
{
    int i, fd, err;
    
    fd = inotify_init1(IN_CLOEXEC);
    if (fd<0) {
        return -errno;
    }

    for (i=0; i<count; i++) {
        err = inotify_add_watch(fd, inotify[i].path, inotify[i].mask);
        if (err<0) {
            return err;
        }
    }

    loop_watcher_t *watcher = __loop_watcher_add(loop, fd, LOOP_TYPE_INOTIFY, cb);
    if (NULL==watcher) {
        return -ENOEXIST;
    }

    return 0;
}

static inline int
__loop_add_timer(loop_t *loop, loop_timer_f *cb, struct itimerspec *timer)
{
    int err;
    
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

    loop_watcher_t *watcher = __loop_watcher_add(loop, fd, LOOP_TYPE_TIMER, cb);
    if (NULL==watcher) {
        return -ENOEXIST;
    }
    
    return 0;
}

static inline int
__loop_add_signal(loop_t *loop, loop_signal_f *cb, int sigs[], int count)
{
    int i, err;
    sigset_t set;
        
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

    loop_watcher_t *watcher = __loop_watcher_add(loop, fd, LOOP_TYPE_SIGNAL, cb);
    if (NULL==watcher) {
        return -ENOEXIST;
    }

    return 0;
}

static inline int
__loop_add_normal(loop_t *loop, int fd, loop_normal_f *cb)
{
    loop_watcher_t *watcher = __loop_watcher_add(loop, fd, LOOP_TYPE_NORMAL, cb);
    if (NULL==watcher) {
        return -ENOEXIST;
    }
    
    return 0;
}

static inline int
__loop_add_father(loop_t *loop, int fd, loop_son_f *cb)
{
    loop_watcher_t *watcher = __loop_watcher_add(loop, fd, LOOP_TYPE_FATHER, cb);
    if (NULL==watcher) {
        return -ENOEXIST;
    }
    
    return 0;
}

static inline int
__loop_add_son(loop_t *loop, int fd, loop_son_f *cb, int father)
{
    loop_watcher_t *watcher = __loop_watcher_add(loop, fd, LOOP_TYPE_SON, cb);
    if (NULL==watcher) {
        return -ENOEXIST;
    }
    watcher->father = father;
    
    return 0;
}

static inline void
__loop_inotify_handle(loop_watcher_t *watcher)
{
    struct inotify_event ev;

    os_objzero(&ev);

    int len = read(watcher->fd, &ev, sizeof(ev));
    if (len==sizeof(ev)) {
        (*watcher->cb.inotify)(watcher, &ev);
    }
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
        __loop_add_son(loop, fd, watcher->cb.cb, watcher->fd);
    }
}

static inline int
__loop_handle_ev(loop_t *loop, struct epoll_event *ev)
{
    static void (*map[LOOP_TYPE_END])(loop_watcher_t *watcher) = {
        [LOOP_TYPE_INOTIFY] = __loop_inotify_handle,
        [LOOP_TYPE_SIGNAL]  = __loop_signal_handle,
        [LOOP_TYPE_TIMER]   = __loop_timer_handle,
        [LOOP_TYPE_NORMAL]  = __loop_normal_handle,
        [LOOP_TYPE_SON]     = __loop_normal_handle,
    };
    
    loop_watcher_t *watcher = __loop_watcher(loop, ev->data.fd);
    if (NULL==watcher) {
        return -ENOEXIST;
    }
    else if (false==is_good_loop_type(watcher->type)) {
        return -EBADFD;
    }
    else if (LOOP_TYPE_FATHER==watcher->type) {
        __loop_father_handle(loop, watcher);
    }
    else {
        (*map[watcher->type])(watcher);
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
os_loop_del_watcher(loop_t *loop, int fd)
{
    if (loop) {
        return __loop_watcher_del(loop, fd);
    } else {
        return -EINVAL0;
    }
}

static inline int
os_loop_add_inotify(loop_t *loop, loop_inotify_f *cb, loop_inotify_t inotify[], int count)
{
    if (loop) {
        return __loop_add_inotify(loop, cb, inotify, count);
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

    __loop_fini(loop);
    
    return 0;
}
/******************************************************************************/
#endif /* __LOOP_H_71254387166c488dbb36aa9ed0f8b379__ */
