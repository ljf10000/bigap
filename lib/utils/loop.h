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
EXTERN_ENUM(loop_type, LOOP_TYPE_ENUM_MAPPER, LOOP_TYPE_END);

EXTERN enum_ops_t *loop_type_ops(void);
EXTERN bool is_good_loop_type(int id);
EXTERN char *loop_type_getnamebyid(int id);
EXTERN int loop_type_getidbyname(const char *name);

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

extern int
os_loop_del_watcher(loop_t *loop, int fd);

extern int
os_loop_add_inotify(loop_t *loop, loop_inotify_f *cb, loop_inotify_t inotify[], int count);

extern int
os_loop_add_timer(loop_t *loop, loop_timer_f *cb, struct itimerspec *timer);

extern int
os_loop_add_signal(loop_t *loop, loop_signal_f *cb, int sigs[], int count);

extern int
os_loop_add_normal(loop_t *loop, int fd, loop_normal_f *cb, void *user);

extern int
os_loop_add_father(loop_t *loop, int fd, loop_son_f *cb, void *user);

#if __CLI_TCP__
#define __os_loop_add_cli(_loop, _name, _cb)  os_loop_add_father(_loop, __clis_FD(_name), _cb, NULL)
#else
#define __os_loop_add_cli(_loop, _name, _cb)  os_loop_add_normal(_loop, __clis_FD(_name), _cb, NULL)
#endif

#define os_loop_add_cli(_loop, _cb) __os_loop_add_cli(_loop, __THIS_APPNAME, _cb, NULL)

extern void 
os_loop_fini(loop_t *loop);

extern int
os_loop(loop_t *loop);
#endif
/******************************************************************************/
#endif /* __LOOP_H_71254387166c488dbb36aa9ed0f8b379__ */
