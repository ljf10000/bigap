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
__os_deamon_exist(void)
{
    return os_pid_exist(os_get_pid());
}

#ifdef __DEAMON__
    extern int  os_deamon_check(void);
    extern void os_deamon_exit(void);
#else
    #define os_deamon_check()   0
    #define os_deamon_exit()    os_do_nothing()
#endif
/******************************************************************************/
#endif
#endif /* __PID_H_53a6199d588c428b864c8a1bdca07622__ */
