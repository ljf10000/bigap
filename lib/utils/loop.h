#ifndef __LOOP_H_71254387166c488dbb36aa9ed0f8b379__
#define __LOOP_H_71254387166c488dbb36aa9ed0f8b379__
/******************************************************************************/
#ifdef __APP__
#ifndef LOOP_FDLIMIT
#define LOOP_FDLIMIT        1024
#endif

#ifndef LOOP_FDGROW
#define LOOP_FDGROW         1024
#endif

#if 1
#define LOOP_TYPE_ENUM_MAPPER(_)        \
    _(LOOP_TYPE_INOTIFY,0, "inotify"),  \
    _(LOOP_TYPE_SIGNAL, 1, "signal"),   \
    _(LOOP_TYPE_TIMER,  2, "timer"),    \
    _(LOOP_TYPE_NORMAL, 3, "normal"),   \
    _(LOOP_TYPE_FATHER, 4, "father"),   \
    _(LOOP_TYPE_SON,    5, "son"),      \
    /* end */
DECLARE_ENUM(loop_type, LOOP_TYPE_ENUM_MAPPER, LOOP_TYPE_END);

static inline bool is_good_loop_type(int id);
static inline char *loop_type_string(int id);
static inline int loop_type_idx(char *name);

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

typedef int loop_timer_f(struct loop_watcher *watcher, time_t now);
typedef int loop_signal_f(struct loop_watcher *watcher, struct signalfd_siginfo *siginfo, time_t now);
typedef int loop_normal_f(struct loop_watcher *watcher, time_t now);
typedef int loop_son_f(struct loop_watcher *watcher, time_t now);
typedef int loop_inotify_f(struct loop_watcher *watcher, struct inotify_event *ev, time_t now);

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

    void *user;
} loop_watcher_t;

typedef struct {
    int efd;
    time_t now;
    
    autoarray_t watcher;

    uint32 count[LOOP_TYPE_END];
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
        os_println("add master fd=%d", loop->efd);
        debug_ok("loop master init ok.");
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
__loop_watcher_add(loop_t *loop, int fd, int type, void *cb, void *user)
{
    int err;
    char *watchname = loop_type_string(type);
    
    __loop_init(loop);
    
    loop_watcher_t *watcher = __loop_watcher_take(loop, fd);
    if (NULL==watcher) {
        debug_trace("not found %s watcher:%d", watchname, fd);
        
        return NULL;
    }
    else if (__is_good_loop_watcher(watcher)) {
        debug_trace("exist %s watcher:%d", watchname, fd);
        
        return NULL;
    }
    
    watcher->fd     = fd;
    watcher->father = INVALID_FD;
    watcher->type   = type;
    watcher->flag   = 0;
    watcher->cb.cb  = cb;
    watcher->user   = user;
    
    err = __loop_fd_add(loop, fd);
    if (err<0) {
        debug_trace("add %s watcher error: %d", watchname, -errno);
        
        return NULL;
    }
    loop->count[type]++;
    debug_ok("add %s watcher ok.", watchname);
    
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
        debug_error("create inotify fd error:%d", -errno);
        
        return -errno;
    }

    for (i=0; i<count; i++) {
        err = inotify_add_watch(fd, inotify[i].path, inotify[i].mask);
        if (err<0) {
            debug_error("add inotify watch error:%d", -errno);
            
            return -errno;
        }
    }

    loop_watcher_t *watcher = __loop_watcher_add(loop, fd, LOOP_TYPE_INOTIFY, cb, NULL);
    if (NULL==watcher) {
        return -ENOEXIST;
    }

    return 0;
}

static inline int
__loop_add_timer(loop_t *loop, loop_timer_f *cb, struct itimerspec *timer)
{
    int err;
    struct itimerspec tm = *timer;
    
    int fd = timerfd_create(CLOCK_MONOTONIC, EFD_CLOEXEC);
    if (fd<0) {
        debug_error("create timer fd error:%d", -errno);
        
        return -errno;
    }
    
    if (0==tm.it_value.tv_sec && 0==tm.it_value.tv_nsec) {
        tm.it_value.tv_nsec = 1;
    }

    err = timerfd_settime(fd, 0, &tm, NULL);
    if (err<0) {
        debug_error("setup timer error:%d", -errno);
        
        return -errno;
    }

    loop_watcher_t *watcher = __loop_watcher_add(loop, fd, LOOP_TYPE_TIMER, cb, NULL);
    if (NULL==watcher) {
        return -ENOEXIST;
    }
    os_println("add timer fd=%d", fd);
    
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

    loop_watcher_t *watcher = __loop_watcher_add(loop, fd, LOOP_TYPE_SIGNAL, cb, NULL);
    if (NULL==watcher) {
        return -ENOEXIST;
    }

    return 0;
}

static inline int
__loop_add_normal(loop_t *loop, int fd, loop_normal_f *cb, void *user)
{
    loop_watcher_t *watcher = __loop_watcher_add(loop, fd, LOOP_TYPE_NORMAL, cb, user);
    if (NULL==watcher) {
        return -ENOEXIST;
    }
    
    return 0;
}

static inline int
__loop_add_father(loop_t *loop, int fd, loop_son_f *cb, void *user)
{    
    loop_watcher_t *watcher = __loop_watcher_add(loop, fd, LOOP_TYPE_FATHER, cb, user);
    if (NULL==watcher) {
        return -ENOEXIST;
    }
    os_println("add father fd=%d", fd);
    
    return 0;
}

static inline int
__loop_add_son(loop_t *loop, int fd, loop_son_f *cb, int father, void *user)
{
    loop_watcher_t *watcher = __loop_watcher_add(loop, fd, LOOP_TYPE_SON, cb, user);
    if (NULL==watcher) {
        return -ENOEXIST;
    }
    watcher->father = father;
    os_println("add son fd=%d", fd);
    
    return 0;
}

typedef void loop_watcher_handle_f(loop_t *loop, loop_watcher_t *watcher, time_t now);

static inline void
__loop_inotify_handle(loop_t *loop, loop_watcher_t *watcher, time_t now)
{
    char buf[1+OS_PAGE_LEN] = {0};
    struct inotify_event *ev;

    int current = 0;
    int len = read(watcher->fd, buf, 1+OS_PAGE_LEN);
    if (len<0) {
        debug_trace("inotify read error:%d", -errno);

        return;
    }
    
    while(current<len) {
        ev = (struct inotify_event *)(buf + current);
        
        (*watcher->cb.inotify)(watcher, ev, now);

        current += sizeof(struct inotify_event) + ev->len;
    }
}

static inline void
__loop_signal_handle(loop_t *loop, loop_watcher_t *watcher, time_t now)
{
    struct signalfd_siginfo siginfo;

    os_objzero(&siginfo);

    int len = read(watcher->fd, &siginfo, sizeof(siginfo));
    if (len==sizeof(siginfo)) {
        (*watcher->cb.signal)(watcher, &siginfo, now);
    }
}

static inline void
__loop_timer_handle(loop_t *loop, loop_watcher_t *watcher, time_t now)
{
    uint32 times = tm_fd_read(watcher->fd);
    int i;

    for (i=0; i<times; i++) {
        (*watcher->cb.timer)(watcher, now);
    }
}

static inline void
__loop_normal_handle(loop_t *loop, loop_watcher_t *watcher, time_t now)
{
    (*watcher->cb.normal)(watcher, now);
}

static inline void
__loop_father_handle(loop_t *loop, loop_watcher_t *watcher, time_t now)
{
    int fd;
    os_sockaddr_t addr;
    socklen_t addrlen = sizeof(addr);
    
    fd = accept(watcher->fd, &addr.c, &addrlen);
    os_println("accept new fd=%d from %d, errno=%d", fd, watcher->fd, -errno);
    if (is_good_fd(watcher->fd)) {
        __loop_add_son(loop, fd, watcher->cb.cb, watcher->fd, watcher->user);
    }
}

static inline int
__loop_handle_ev(loop_t *loop, struct epoll_event *ev, time_t now)
{
    static loop_watcher_handle_f *map[LOOP_TYPE_END] = {
        [LOOP_TYPE_INOTIFY] = __loop_inotify_handle,
        [LOOP_TYPE_SIGNAL]  = __loop_signal_handle,
        [LOOP_TYPE_TIMER]   = __loop_timer_handle,
        [LOOP_TYPE_NORMAL]  = __loop_normal_handle,
        [LOOP_TYPE_FATHER]  = __loop_father_handle,
        [LOOP_TYPE_SON]     = __loop_normal_handle,
    };

    loop_watcher_t *watcher = __loop_watcher(loop, ev->data.fd);
    if (NULL==watcher) {
        return -ENOEXIST;
    }
    else if (false==is_good_loop_type(watcher->type)) {
        return -EBADFD;
    }
    else {
        (*map[watcher->type])(loop, watcher, now);
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
    time_t now = time(NULL);
    
    for (i=0; i<nfds; i++) {
        __loop_handle_ev(loop, &evs[i], now);
    }

    return 0;
}

static inline int
os_loop_del_watcher(loop_t *loop, int fd)
{
    if (NULL==loop) {
        return -EINVAL0;
    }
    else if (false==is_good_fd(fd)) {
        return -EINVAL1;
    }
    else {
        return __loop_watcher_del(loop, fd);
    }
}

static inline int
os_loop_add_inotify(loop_t *loop, loop_inotify_f *cb, loop_inotify_t inotify[], int count)
{
    if (NULL==loop) {
        return -EINVAL0;
    }
    else if (NULL==cb) {
        return -EINVAL1;
    }
    else if (NULL==inotify) {
        return -EINVAL2;
    }
    else if (count<=0) {
        return -EINVAL3;
    }
    else {
        return __loop_add_inotify(loop, cb, inotify, count);
    }
}

static inline int
os_loop_add_timer(loop_t *loop, loop_timer_f *cb, struct itimerspec *timer)
{
    if (NULL==loop) {
        return -EINVAL0;
    }
    else if (NULL==cb) {
        return -EINVAL1;
    }
    else if (NULL==timer) {
        return -EINVAL2;
    }
    else {
        return __loop_add_timer(loop, cb, timer);
    }
}

static inline int
os_loop_add_signal(loop_t *loop, loop_signal_f *cb, int sigs[], int count)
{
    if (NULL==loop) {
        return -EINVAL0;
    }
    else if (NULL==cb) {
        return -EINVAL1;
    }
    else if (NULL==sigs) {
        return -EINVAL2;
    }
    else if (count<=0) {
        return -EINVAL3;
    }
    else {
        return __loop_add_signal(loop, cb, sigs, count);
    }
}

static inline int
os_loop_add_normal(loop_t *loop, int fd, loop_normal_f *cb, void *user)
{
    if (NULL==loop) {
        return -EINVAL0;
    }
    else if (NULL==cb) {
        return -EINVAL1;
    }
    else if (false==is_good_fd(fd)) {
        return -EBADF;
    }
    else {
        return __loop_add_normal(loop, fd, cb, user);
    }
}

static inline int
os_loop_add_father(loop_t *loop, int fd, loop_son_f *cb, void *user)
{
    if (NULL==loop) {
        return -EINVAL0;
    }
    else if (NULL==cb) {
        return -EINVAL1;
    }
    else if (false==is_good_fd(fd)) {
        return -EBADF;
    }
    else {
        return __loop_add_father(loop, fd, cb, user);
    }
}

#if __CLI_TCP__
#define os_loop_add_CLI(_loop, _name, _cb)  os_loop_add_father(_loop, __clis_FD(_name), _cb, NULL)
#else
#define os_loop_add_CLI(_loop, _name, _cb)  os_loop_add_normal(_loop, __clis_FD(_name), _cb, NULL)
#endif

static inline int
os_loop(loop_t *loop)
{
    if (NULL==loop) {
        return -EINVAL0;
    }
    
    while(1) {
        int err = __loop_handle(loop);
        if (err<0) {
            // log
        }
    }

    __loop_fini(loop);
    
    return 0;
}
#endif
/******************************************************************************/
#endif /* __LOOP_H_71254387166c488dbb36aa9ed0f8b379__ */
