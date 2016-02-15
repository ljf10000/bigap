#ifndef __THIS_H_bb55b32c2226479dab1f126ea1ba1e9f__
#define __THIS_H_bb55b32c2226479dab1f126ea1ba1e9f__
/******************************************************************************/
#ifndef __THIS_APP
#error "must defined __THIS_APP before all #include"
#endif

#define __SYMBOL_TO_STRING2(x)  #x
#define __SYMBOL_TO_STRING(x)   __SYMBOL_TO_STRING2(x)

#define __SYMBOL_TO_VAR2(_prefix, _name)    __##_prefix##_name
#ifdef __BUSYBOX__
#define __SYMBOL_TO_VAR(_prefix, _name)     __this##_name
#else
#define __SYMBOL_TO_VAR(_prefix, _name)     __SYMBOL_TO_VAR2(_prefix, _name)
#endif

#ifndef __THIS_NAME
#define __THIS_NAME             __SYMBOL_TO_STRING(__THIS_APP)
#endif

#ifndef __THIS_FILE
#define __THIS_FILE             __THIS_NAME
#endif

#ifndef __THIS_JLOG
#define __THIS_JLOG             __SYMBOL_TO_VAR(__THIS_APP, _jlog)
#endif

#ifndef __THIS_TIMER
#define __THIS_TIMER            __SYMBOL_TO_VAR(__THIS_APP, _timer)
#endif

#ifndef __THIS_COROUTINE
#define __THIS_COROUTINE        __SYMBOL_TO_VAR(__THIS_APP, _coroutine)
#endif

#ifndef __THIS_FD
#define __THIS_FD               __SYMBOL_TO_VAR(__THIS_APP, _fd)
#endif

#ifndef __THIS_DEBUG
#define __THIS_DEBUG            __SYMBOL_TO_VAR(__THIS_APP, _debugger)
#endif

#ifndef __THIS_LOCKFD
#define __THIS_LOCKFD           __SYMBOL_TO_VAR(__THIS_APP, _lock_fd)
#endif

#ifndef __THIS_LOCKFILE
#define __THIS_LOCKFILE         "/tmp/." __THIS_NAME ".lock"
#endif

#ifndef __THIS_PIDFILE
#define __THIS_PIDFILE          "/tmp/." __THIS_NAME ".pid"
#endif

#ifndef __THIS_DEAMON
#define __THIS_DEAMON           __SYMBOL_TO_VAR(__THIS_APP, _deamon)
#endif

#ifndef __THIS_OEM
#define __THIS_OEM              __SYMBOL_TO_VAR(__THIS_APP, _oem)
#endif

#ifndef __THIS_AK
#define __THIS_AK               __SYMBOL_TO_VAR(__THIS_APP, _ak)
#endif
/******************************************************************************/
#endif /* __THIS_H_bb55b32c2226479dab1f126ea1ba1e9f__ */
