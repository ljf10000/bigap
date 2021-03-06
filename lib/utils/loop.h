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

#ifndef LOOP_DPRINT
#define LOOP_DPRINT         0
#endif

#if LOOP_DPRINT
#define loop_println(_fmt, _args...)    printf(_fmt "\n", ##_args)
#else
#define loop_println(_fmt, _args...)    os_do_nothing()
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

static inline enum_ops_t *loop_type_ops(void);
static inline bool is_good_loop_type(int id);
static inline char *loop_type_getnamebyid(int id);
static inline int loop_type_getidbyname(const char *name);

#define LOOP_TYPE_INOTIFY   LOOP_TYPE_INOTIFY
#define LOOP_TYPE_SIGNAL    LOOP_TYPE_SIGNAL
#define LOOP_TYPE_TIMER     LOOP_TYPE_TIMER
#define LOOP_TYPE_NORMAL    LOOP_TYPE_NORMAL
#define LOOP_TYPE_FATHER    LOOP_TYPE_FATHER
#define LOOP_TYPE_SON       LOOP_TYPE_SON
#define LOOP_TYPE_END       LOOP_TYPE_END
#endif

enum {
    LOOP_F_ONCE = 0x01,
    LOOP_F_AUTO_DEL_SON = 0x02,
};

typedef struct {
    char *path;
    uint32 mask;
} loop_inotify_t;

typedef struct loop_watcher_s loop_watcher_t;

typedef int loop_timer_f(loop_watcher_t *watcher, time_t now);
typedef int loop_signal_f(loop_watcher_t *watcher, struct signalfd_siginfo *siginfo, time_t now);
typedef int loop_normal_f(loop_watcher_t *watcher, time_t now);
typedef int loop_son_f(loop_watcher_t *watcher, time_t now);
typedef int loop_inotify_f(loop_watcher_t *watcher, struct inotify_event *ev, time_t now);

struct loop_watcher_s {
    int fd;
    int father;     // only for son
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
};

typedef struct {
    int efd;
    time_t now;

    autoarray_t watcher;

    uint32 count[LOOP_TYPE_END];
} loop_t;

#define LOOP_INITER     {   \
    .efd    = INVALID_FD,   \
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
os_loop_add_father(loop_t *loop, int fd, loop_son_f *cb, bool auto_del_son, void *user);

#define __os_loop_cli_add(_loop, _tcp, _name, _cb, _user)   ({  \
    int __err;                                                  \
                                                                \
    if (_tcp) {                                                 \
        __err = os_loop_add_father(_loop, __clis_FD(_tcp, _name), _cb, true, _user); \
    } else {                                                    \
        __err = os_loop_add_normal(_loop, __clis_FD(_tcp, _name), _cb, _user); \
    }                                                           \
                                                                \
    __err;                                                      \
})  /* end */
#define os_loop_cli_add(_loop, _tcp, _cb, _user) \
    __os_loop_cli_add(_loop, _tcp, __THIS_APPNAME, _cb, _user)
#define os_loop_cli_tcp(_loop, _cb, _user) \
    os_loop_cli_add(_loop, true, _cb, _user)
#define os_loop_cli_udp(_loop, _cb, _user) \
    os_loop_cli_add(_loop, false, _cb, _user)

extern void 
os_loop_fini(loop_t *loop);

extern int
os_loop_init(loop_t *loop);

extern int
os_loop(loop_t *loop);
#endif
/******************************************************************************/
#endif /* __LOOP_H_71254387166c488dbb36aa9ed0f8b379__ */
