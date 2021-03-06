#ifndef __PID_H_53a6199d588c428b864c8a1bdca07622__
#define __PID_H_53a6199d588c428b864c8a1bdca07622__
#ifdef __APP__
/******************************************************************************/
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
os_deamon_exist(void)
{
    return os_pid_exist(os_get_pid());
}

extern bool __THIS_DEAMON;

static inline int
os_deamon_init(void)
{
#ifdef __DEAMON__
    if (os_deamon_exist()) {
        return -EDEAMON;
    }
    
    __THIS_DEAMON = true;
    
    os_set_pid();
#endif

    return 0;
}

static inline void
os_deamon_fini(void)
{
#ifdef __DEAMON__
    if (__THIS_DEAMON) {
        unlink(__THIS_PIDFILE);
    }
#endif
}
/******************************************************************************/
#endif
#endif /* __PID_H_53a6199d588c428b864c8a1bdca07622__ */
