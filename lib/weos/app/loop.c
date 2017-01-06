/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
typedef void loop_watcher_handle_f(loop_t *loop, loop_watcher_t *watcher, time_t now);

ALWAYS_INLINE bool
__is_good_loop(loop_t *loop)
{
    return loop && is_good_fd(loop->efd);
}

ALWAYS_INLINE bool
__is_good_loop_watcher(loop_watcher_t *watcher)
{
    return watcher && is_good_fd(watcher->fd);
}

ALWAYS_INLINE int
__loop_fd_add(loop_t *loop, int fd)
{
    struct epoll_event ev;
    
    ev.events   = EPOLLIN;
    ev.data.fd  = fd;

    return epoll_ctl(loop->efd, EPOLL_CTL_ADD, fd, &ev);
}

ALWAYS_INLINE int
__loop_fd_del(loop_t *loop, int fd)
{
    return epoll_ctl(loop->efd, EPOLL_CTL_DEL, fd, NULL);
}

STATIC int
__loop_master_init(loop_t *loop)
{
    if (false==is_good_fd(loop->efd)) {
        loop->efd = epoll_create1(EPOLL_CLOEXEC);
        if (loop->efd<0) {
            return -errno;
        }
          debug_loop("add master fd=%d", loop->efd);
        loop_println("add master fd=%d", loop->efd);
        
            debug_ok("loop master init ok.");
        loop_println("loop master init ok.");
    }

    return 0;
}

STATIC void
__loop_watcher_constructor(void *item)
{
    loop_watcher_t *watcher = (loop_watcher_t *)item;

    os_objzero(watcher);
    
    watcher->fd     = INVALID_FD;
    watcher->father = INVALID_FD;
    watcher->son    = INVALID_FD;
}

STATIC void
__loop_watcher_destructor(void *item)
{
    loop_watcher_t *watcher = (loop_watcher_t *)item;

    if (is_good_fd(watcher->fd)) {
        os_close(watcher->fd);
    }

    __loop_watcher_constructor(watcher);
}

STATIC int
__loop_watcher_fini(loop_t *loop)
{
    if (loop->watcher.base) {
        os_aa_destroy(&loop->watcher);
    }

    return 0;
}

STATIC int
__loop_watcher_init(loop_t *loop, uint32 limit)
{   
    if (NULL==loop->watcher.base) {
        if (0==limit) {
            limit = LOOP_FDLIMIT;
        }
        
        int err = os_aa_create(&loop->watcher,
            sizeof(loop_watcher_t),     // size
            limit,                      // count
            limit,                      // limit
            LOOP_FDGROW,                // grow
            __loop_watcher_constructor, // init
            __loop_watcher_destructor); // clean
        if (err<0) {
             debug_error("loop watcher init error:%d", err);
            loop_println("loop watcher init error:%d", err);

            return err;
        }

            debug_ok("loop watcher init ok.");
        loop_println("loop watcher init ok.");
    }

    return 0;
}

STATIC int
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

ALWAYS_INLINE loop_watcher_t *
__loop_watcher_take(loop_t *loop, int fd)
{
    if (false==is_good_fd(fd)) {
        return NULL;
    }

    loop_watcher_t *watcher = os_aa_get(&loop->watcher, fd, false);

    return watcher;
}

ALWAYS_INLINE loop_watcher_t *
__loop_watcher(loop_t *loop, int fd)
{
    loop_watcher_t *watcher = __loop_watcher_take(loop, fd);
    if (__is_good_loop_watcher(watcher)) {
        return watcher;
    } else {
        return NULL;
    }
}

STATIC loop_watcher_t *
__loop_watcher_add(loop_t *loop, int fd, int type, int flag, void *cb, void *user)
{
    int err;
    char *watchname = loop_type_getnamebyid(type);
    
    __loop_init(loop);
    
    loop_watcher_t *watcher = __loop_watcher_take(loop, fd);
    if (NULL==watcher) {
          debug_loop("not found %s watcher:%d", watchname, fd);
        loop_println("not found %s watcher:%d", watchname, fd);
        
        return NULL;
    }
    else if (__is_good_loop_watcher(watcher)) {
          debug_loop("exist %s watcher:%d", watchname, fd);
        loop_println("exist %s watcher:%d", watchname, fd);
        
        return NULL;
    }
    
    watcher->fd     = fd;
    watcher->father = INVALID_FD;
    watcher->son    = INVALID_FD;
    watcher->type   = type;
    watcher->flag   = flag;
    watcher->cb.cb  = cb;
    watcher->user   = user;
    
    err = __loop_fd_add(loop, fd);
    if (err<0) {
          debug_loop("add %s watcher error: %d", watchname, -errno);
        loop_println("add %s watcher error: %d", watchname, -errno);
        
        return NULL;
    }
    loop->count[type]++;
      debug_loop("add %s watcher ok.", watchname);
    loop_println("add %s watcher ok.", watchname);
    
    return watcher;
}

STATIC int
__loop_watcher_del(loop_t *loop, int fd)
{
    loop_watcher_t *watcher = __loop_watcher(loop, fd);
    if (NULL==watcher) {
        return -ENOEXIST;
    }
    int type = watcher->type;
    
    loop->count[type]--;
    __loop_watcher_destructor(watcher);
    
      debug_loop("del %s watcher ok.", loop_type_getnamebyid(type));
    loop_println("del %s watcher ok.", loop_type_getnamebyid(type));
    
    return 0;
}

STATIC int
__loop_add_inotify(loop_t *loop, loop_inotify_f *cb, loop_inotify_t inotify[], int count)
{
    int i, fd, err;
    
    fd = inotify_init1(IN_CLOEXEC);
    if (fd<0) {
         debug_error("create inotify fd error:%d", -errno);
        loop_println("create inotify fd error:%d", -errno);
        
        return -errno;
    }
    os_closexec(fd);
    
    for (i=0; i<count; i++) {
        err = inotify_add_watch(fd, inotify[i].path, inotify[i].mask);
        if (err<0) {
             debug_error("add inotify watch error:%d", -errno);
            loop_println("add inotify watch error:%d", -errno);
            
            return -errno;
        }
    }

    loop_watcher_t *watcher = __loop_watcher_add(loop, fd, LOOP_TYPE_INOTIFY, 0, cb, NULL);
    if (NULL==watcher) {
        return -ENOEXIST;
    }

    return 0;
}

STATIC int
__loop_add_timer(loop_t *loop, loop_timer_f *cb, struct itimerspec *timer)
{
    int err;
    struct itimerspec tm = *timer;
    
    int fd = timerfd_create(CLOCK_MONOTONIC, EFD_CLOEXEC);
    if (fd<0) {
         debug_error("create timer fd error:%d", -errno);
        loop_println("create timer fd error:%d", -errno);
        
        return -errno;
    }
    os_closexec(fd);
    
    if (0==tm.it_value.tv_sec && 0==tm.it_value.tv_nsec) {
        tm.it_value.tv_nsec = 1;
    }

    err = timerfd_settime(fd, 0, &tm, NULL);
    if (err<0) {
         debug_error("setup timer error:%d", -errno);
        loop_println("setup timer error:%d", -errno);
        
        return -errno;
    }

    loop_watcher_t *watcher = __loop_watcher_add(loop, fd, LOOP_TYPE_TIMER, 0, cb, NULL);
    if (NULL==watcher) {
        return -ENOEXIST;
    }
      debug_loop("add timer fd=%d", fd);
    loop_println("add timer fd=%d", fd);
    
    return 0;
}

STATIC int
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
        loop_println("create signal fd error:%d", -errno);
        
        return -errno;
    }
    os_closexec(fd);
    
    loop_watcher_t *watcher = __loop_watcher_add(loop, fd, LOOP_TYPE_SIGNAL, 0, cb, NULL);
    if (NULL==watcher) {
        return -ENOEXIST;
    }

    return 0;
}

STATIC int
__loop_add_normal(loop_t *loop, int fd, loop_normal_f *cb, void *user)
{
    loop_watcher_t *watcher = __loop_watcher_add(loop, fd, LOOP_TYPE_NORMAL, 0, cb, user);
    if (NULL==watcher) {
        return -ENOEXIST;
    }
    
    return 0;
}

STATIC int
__loop_add_father(loop_t *loop, int fd, loop_son_f *cb, bool auto_del_son, void *user)
{
    int flag = auto_del_son?LOOP_F_AUTO_DEL_SON:0;
    loop_watcher_t *watcher = __loop_watcher_add(loop, fd, LOOP_TYPE_FATHER, flag, cb, user);
    if (NULL==watcher) {
        return -ENOEXIST;
    }
      debug_loop("add father fd=%d auto_del_son=%d user=%d", fd, auto_del_son, user);
    loop_println("add father fd=%d auto_del_son=%d user=%d", fd, auto_del_son, user);
    
    return 0;
}

STATIC int
__loop_add_son(loop_t *loop, int fd, loop_watcher_t *father)
{
    int flag = os_hasflag(father->flag, LOOP_F_AUTO_DEL_SON)?LOOP_F_ONCE:0;
    loop_watcher_t *watcher = __loop_watcher_add(loop, fd, LOOP_TYPE_SON, flag, father->cb.cb, father->user);
    if (NULL==watcher) {
        return -ENOEXIST;
    }

    // bind father
    father->son = fd;
    watcher->father = father->fd;
    
      debug_loop("add son fd=%d flag=0x%x user=%d", fd, flag, watcher->user);
    loop_println("add son fd=%d flag=0x%x user=%d", fd, flag, watcher->user);
    
    return 0;
}

STATIC void
__loop_inotify_handle(loop_t *loop, loop_watcher_t *watcher, time_t now)
{
    char buf[1+OS_PAGE_LEN] = {0};
    struct inotify_event *ev;

    int current = 0;
    int len = read(watcher->fd, buf, 1+OS_PAGE_LEN);
    if (len<0) {
          debug_loop("inotify read error:%d", -errno);
        loop_println("inotify read error:%d", -errno);

        return;
    }
    
    while(current<len) {
        ev = (struct inotify_event *)(buf + current);
        
        (*watcher->cb.inotify)(watcher, ev, now);

        current += sizeof(struct inotify_event) + ev->len;
    }
}

STATIC void
__loop_signal_handle(loop_t *loop, loop_watcher_t *watcher, time_t now)
{
    struct signalfd_siginfo siginfo;

    os_objzero(&siginfo);

    int len = read(watcher->fd, &siginfo, sizeof(siginfo));
    if (len==sizeof(siginfo)) {
        (*watcher->cb.signal)(watcher, &siginfo, now);
    }
}

STATIC void
__loop_timer_handle(loop_t *loop, loop_watcher_t *watcher, time_t now)
{
    uint32 times = tm_fd_read(watcher->fd);
    int i;

    for (i=0; i<times; i++) {
        (*watcher->cb.timer)(watcher, now);
    }
}

STATIC void
__loop_normal_handle(loop_t *loop, loop_watcher_t *watcher, time_t now)
{
    (*watcher->cb.normal)(watcher, now);
}

STATIC void
__loop_father_handle(loop_t *loop, loop_watcher_t *watcher, time_t now)
{
    int fd, err;
    os_sockaddr_t addr;
    sockaddr_len_t addrlen = sizeof(addr);
    loop_watcher_t *son;
    
    // try release last son
    if (is_good_fd(watcher->son) &&
        (son = __loop_watcher(loop, watcher->son)) &&
        os_hasflag(son->flag, LOOP_F_ONCE)) {

        __loop_watcher_del(loop, watcher->son);
        watcher->son = INVALID_FD;
    }
    
    fd = accept(watcher->fd, &addr.c, &addrlen);
      debug_loop("accept new fd=%d from %d, errno=%d", fd, watcher->fd, is_good_fd(fd)?0:-errno);
    loop_println("accept new fd=%d from %d, errno=%d", fd, watcher->fd, is_good_fd(fd)?0:-errno);
    if (is_good_fd(fd)) {
        os_closexec(fd);
        __loop_add_son(loop, fd, watcher);
    }
}

STATIC int
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
        loop_println("loop watcher not exit by fd:%d", ev->data.fd);
        
        return -ENOEXIST;
    }
    else if (false==is_good_loop_type(watcher->type)) {
        loop_println("bad loop watcher type:%d", watcher->type);
        
        return -EBADFD;
    }
    else {
        (*map[watcher->type])(loop, watcher, now);
    }
    
    return 0;
}

STATIC int
__loop_handle(loop_t *loop)
{
    static uint64 count;
    struct epoll_event evs[32];
    int i;
    
    int nfds = epoll_wait(loop->efd, evs, os_count_of(evs), -1);
    if (nfds<0) {
        if (EINTR==errno) {
            loop_println("loop break");
            
            return 0;
        } else {
            loop_println("loop wait error:%d", -errno);
            
            return -errno;
        }
    }
    time_t now = time(NULL);
    
    for (i=0; i<nfds; i++) {
        count++;
        
        __loop_handle_ev(loop, &evs[i], now);
    }

    return 0;
}

int
os_loop_del_watcher(loop_t *loop, int fd)
{
    if (NULL==loop) {
        return -EINVAL0;
    }
    else if (false==is_good_fd(fd)) {
        return -EBADFD;
    }
    else {
        return __loop_watcher_del(loop, fd);
    }
}

int
os_loop_add_inotify(loop_t *loop, loop_inotify_f *cb, loop_inotify_t inotify[], int count)
{
    if (NULL==loop) {
        return -EINVAL0;
    }
    else if (NULL==cb) {
        return -EBADCB;
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

int
os_loop_add_timer(loop_t *loop, loop_timer_f *cb, struct itimerspec *timer)
{
    if (NULL==loop) {
        return -EINVAL0;
    }
    else if (NULL==cb) {
        return -EBADCB;
    }
    else if (NULL==timer) {
        return -EINVAL2;
    }
    else {
        return __loop_add_timer(loop, cb, timer);
    }
}

int
os_loop_add_signal(loop_t *loop, loop_signal_f *cb, int sigs[], int count)
{
    if (NULL==loop) {
        return -EINVAL0;
    }
    else if (NULL==cb) {
        return -EBADCB;
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

int
os_loop_add_normal(loop_t *loop, int fd, loop_normal_f *cb, void *user)
{
    if (NULL==loop) {
        return -EINVAL0;
    }
    else if (NULL==cb) {
        return -EBADCB;
    }
    else if (false==is_good_fd(fd)) {
        return -EBADF;
    }
    else {
        return __loop_add_normal(loop, fd, cb, user);
    }
}

int
os_loop_add_father(loop_t *loop, int fd, loop_son_f *cb, bool auto_del_son, void *user)
{
    if (NULL==loop) {
        return -EINVAL0;
    }
    else if (NULL==cb) {
        return -EBADCB;
    }
    else if (false==is_good_fd(fd)) {
        return -EBADF;
    }
    else {
        return __loop_add_father(loop, fd, cb, auto_del_son, user);
    }
}

void 
os_loop_fini(loop_t *loop)
{
    __loop_watcher_fini(loop);

    os_close(loop->efd);
}

int
os_loop_init(loop_t *loop)
{
    os_objzero(loop);

    loop->efd = INVALID_FD;

    return 0;
}

int
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

    os_loop_fini(loop);
    
    return 0;
}
/******************************************************************************/
