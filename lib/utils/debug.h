#ifndef __DEBUG_H_ad2ef6604c9a4224a1d7128d95e28db6__
#define __DEBUG_H_ad2ef6604c9a4224a1d7128d95e28db6__
/******************************************************************************/
#ifdef __APP__

#define BACKTRACE_by_none               0
#define BACKTRACE_by_libc_backtrace     1
#define BACKTRACE_by_libunwind          2

#ifndef __BACKTRACE__
#define __BACKTRACE__       BACKTRACE_by_libc_backtrace
#endif

#define BACKTRACE_DESTROY_STACK_FOREVER(_up) do{   \
    int var;            \
    int *p = &var;      \
                        \
    while(1) {          \
        os_println("write %p to zero", p); \
        if (_up) {      \
            *p++ = 0;   \
        } else {        \
            *p-- = 0;   \
        }               \
    }                   \
}while(0)

#define BACKTRACE_DESTROY_STACK_BYSIZE(_up, _size)  do{   \
    int var;                        \
    char *p = (char *)&var;         \
                                    \
    os_println("write stack %s %d's byte to zero", _up?"up":"down", _size); \
                                    \
    if (_up) {                      \
        os_memzero(p, _size);          \
    } else {                        \
        os_memzero(p - _size, _size);  \
    }                               \
}while(0)

#define BACKTRACE_WRITE_POINTER(_v)    do {   \
    int i;                  \
    int *p = _v;            \
                            \
    for (i=0; i<10; i++) {  \
        os_println("write pointer(%p) in function %s", p, __func__); \
    }                       \
                            \
    *p = 0;                 \
}while(0)

#define BACKTRACE_READ_POINTER(_v)    do {   \
    int i;                  \
    int *p = _v;            \
                            \
    for (i=0; i<10; i++) {  \
        os_println("read pointer(%p) in function %s", p, __func__); \
    }                       \
                            \
    i = *p;                 \
}while(0)

#ifndef BACKTRACE_PATH
#define BACKTRACE_PATH          PC_VAL(".", "/tmp/.backtrace")
#endif

#define BACKTRACE_NAME          BACKTRACE_PATH "/" __THIS_APPNAME
#define BACKTRACE_FILE_TRACE    BACKTRACE_NAME ".trace"
#define BACKTRACE_FILE_MAPS     BACKTRACE_NAME ".maps"
#define BACKTRACE_FILE_LOCATE   BACKTRACE_NAME ".locate"

#ifndef BACKTRACE_ASIZE
#define BACKTRACE_ASIZE         256
#endif

#if __BACKTRACE__==BACKTRACE_by_libunwind
extern void __sighandle_callstack(int signo);
#elif __BACKTRACE__==BACKTRACE_by_libc_backtrace
extern void __sighandle_callstack(int signo);
#else
static inline void __sighandle_callstack(int signo) { os_do_nothing(); }
#endif /* BACKTRACE_TYPE */

static inline void __sighandle_donothing(int signo) { os_do_nothing(); }
static inline int os_fini(void);

static inline void 
__sighandle_exit(int signo)
{
    os_fini();
    
    exit(signo);
}

extern void
__setup_signal(void (*handle)(int signo), int sigs[], int count);

#define setup_signal(_handle, _sigs) \
        __setup_signal(_handle, _sigs, os_count_of(_sigs))

#define os_signal_ignore_list    SIGPIPE
static inline void
setup_signal_ignore(void (*handle)(int signo))
{
    int sigs[] = {os_signal_ignore_list};

    setup_signal(handle?handle:SIG_IGN, sigs);
}

#define os_signal_user_list     SIGUSR1, SIGUSR2
static inline void
setup_signal_user(void (*handle)(int signo))
{
    int sigs[] = {os_signal_user_list};

    setup_signal(handle?handle:__sighandle_exit, sigs);
}

#define os_signal_exit_list     SIGINT, SIGHUP, SIGTERM, SIGABRT
static inline void
setup_signal_exit(void (*handle)(int signo))
{
    int sigs[] = {os_signal_exit_list};

    setup_signal(handle?handle:__sighandle_exit, sigs);
}

#define os_signal_timer_list    SIGALRM
static inline void
setup_signal_timer(void (*handle)(int signo))
{
    int sigs[] = {os_signal_timer_list};

    setup_signal(handle?handle:__sighandle_donothing, sigs);
}

#define os_signal_callstack_list    SIGBUS, SIGILL, SIGFPE, SIGSEGV
static inline void
setup_signal_callstack(void (*handle)(int signo))
{
    int sigs[] = {os_signal_callstack_list};

    setup_signal(handle?handle:__sighandle_callstack, sigs);
}
#else
#define setup_signal_ignore(_handle)        os_do_nothing()
#define setup_signal_user(_handle)          os_do_nothing()
#define setup_signal_exit(_handle)          os_do_nothing()
#define setup_signal_timer(_handle)         os_do_nothing()
#define setup_signal_callstack(_handle)     os_do_nothing()
#endif /* __APP__ */
/******************************************************************************/
#endif /* __DEBUG_H_ad2ef6604c9a4224a1d7128d95e28db6__ */
