#ifndef __PID_H_53a6199d588c428b864c8a1bdca07622__
#define __PID_H_53a6199d588c428b864c8a1bdca07622__
/******************************************************************************/
typedef struct {
    bool running;
} deamon_t;

#ifdef __APP__
#define DECLARE_REAL_DEAMON         deamon_t __THIS_DEAMON
#define DECLARE_FAKE_DEAMON         extern deamon_t __THIS_DEAMON

#ifdef __ALLINONE__
#   define DECLARE_DEAMON           DECLARE_FAKE_DEAMON
#else
#   define DECLARE_DEAMON           DECLARE_REAL_DEAMON
#endif

DECLARE_FAKE_DEAMON;

static inline deamon_t *
__this_deamon(void)
{
    return &__THIS_DEAMON;
}

static inline int
__os_get_pid(char *pidfile)
{
    int pid = 0;

    os_v_fgeti(&pid, "%s", pidfile);
    
    return pid;
}

static inline int
os_get_pid(void)
{
    int pid = 0;

    os_fgeti(&pid, __THIS_PIDFILE);

    return pid;
}

static inline void
__os_set_pid(char *pidfile, int pid)
{
    os_v_fseti(pid, "%s", pidfile);
}

static inline void
os_set_pid(void)
{
    os_fseti(getpid(), __THIS_PIDFILE);
}

static inline bool
os_pid_exist(int pid)
{
    return pid > 0 && 0==kill(pid, 0);
}

static inline bool
__os_deamon_exist(void)
{
    return os_pid_exist(os_get_pid());
}

static inline int
os_deamon_check(void)
{
#ifdef __DEAMON__
    if (__os_deamon_exist()) {
        return -EDEAMON;
    }
    
    __this_deamon()->running = true;
    
    os_set_pid();
#endif

    return 0;
}

static inline void
os_deamon_exit(void)
{
#ifdef __DEAMON__
    if (__this_deamon()->running) {
        unlink(__THIS_PIDFILE);
    }
#endif
}

#endif
/******************************************************************************/
#endif /* __PID_H_53a6199d588c428b864c8a1bdca07622__ */
