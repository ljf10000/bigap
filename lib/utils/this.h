#ifndef __THIS_H_bb55b32c2226479dab1f126ea1ba1e9f__
#define __THIS_H_bb55b32c2226479dab1f126ea1ba1e9f__
/******************************************************************************/
#ifndef __THIS_APP
#error "must defined __THIS_APP before all #include"
#endif

#define __SYMBOL_CONTRACT2(x, y)    x##y
#define __SYMBOL_CONTRACT(x, y)     __SYMBOL_CONTRACT2(x, y)
#define OS_DEFINE(x)                __SYMBOL_CONTRACT(x, __COUNTER__)

#define __SYMBOL_TO_STRING2(x)      #x
#define __SYMBOL_TO_STRING(x)       __SYMBOL_TO_STRING2(x)

#ifdef __ALLINONE__
#define allinone_main           __SYMBOL_CONTRACT(__THIS_APP, _main)
#else
#define allinone_main           main
#endif

#ifndef __THIS_APPNAME
#define __THIS_APPNAME          __SYMBOL_TO_STRING(__THIS_APP)
#endif

#ifndef __THIS_FILENAME
#ifdef __THIS_FILE
#   define __THIS_FILENAME      __SYMBOL_TO_STRING(__THIS_FILE)
#else
#   define __THIS_FILENAME      __THIS_APPNAME
#endif
#endif

#ifndef __THIS_LOCKFILE
#define __THIS_LOCKFILE         "/tmp/." __THIS_APPNAME ".lock"
#endif

#ifndef __THIS_PIDFILE
#define __THIS_PIDFILE          "/tmp/." __THIS_APPNAME ".pid"
#endif
/******************************************************************************/
#define __SYMBOL_TO_VAR2(_prefix, _name)        __THIS_##_prefix##_name
#define __SYMBOL_TO_VAR(_prefix, _name)         __SYMBOL_TO_VAR2(_prefix, _name)

#if defined(__ALLINONE__) || defined(__BOOT__)
#   define __SYMBOL_TO_THIS(_name)              __SYMBOL_TO_VAR(_, _name)
#else
#   define __SYMBOL_TO_THIS(_name)              __SYMBOL_TO_VAR(__THIS_APP, _name)
#endif

#ifndef __THIS_ENV
#define __THIS_ENV              __SYMBOL_TO_THIS(_env)
#endif

#ifndef __THIS_COMMAND
#define __THIS_COMMAND          __SYMBOL_TO_THIS(_command)
#endif

#ifndef __THIS_JLOGGER
#define __THIS_JLOGGER          __SYMBOL_TO_THIS(_jlogger)
#endif

#ifndef __THIS_TIMER
#define __THIS_TIMER            __SYMBOL_TO_THIS(_timer)
#endif

#ifndef __THIS_COROUTINE
#define __THIS_COROUTINE        __SYMBOL_TO_THIS(_coroutine)
#endif

#ifndef __THIS_FD
#define __THIS_FD               __SYMBOL_TO_THIS(_fd)
#endif

#ifndef __THIS_DEBUG
#define __THIS_DEBUG            __SYMBOL_TO_THIS(_debugger)
#endif

#ifndef __THIS_JDEBUG
#define __THIS_JDEBUG           __SYMBOL_TO_THIS(_js_debugger)
#endif

#ifndef __THIS_LOCKFD
#define __THIS_LOCKFD           __SYMBOL_TO_THIS(_lock_fd)
#endif

#ifndef __THIS_DEAMON
#define __THIS_DEAMON           __SYMBOL_TO_THIS(_deamon)
#endif

#ifndef __THIS_OEM
#define __THIS_OEM              __SYMBOL_TO_THIS(_oem)
#endif

#ifndef __THIS_AK
#define __THIS_AK               __SYMBOL_TO_THIS(_ak)
#endif

#ifndef __THIS_CLI_BUFFER
#define __THIS_CLI_BUFFER       __SYMBOL_TO_THIS(_cli_buffer)
#endif

#ifndef __THIS_CLI_MAGIC
#define __THIS_CLI_MAGIC         __SYMBOL_TO_THIS(_cli_addr)
#endif

#ifndef __THIS_HAENV
#define __THIS_HAENV            __SYMBOL_TO_THIS(_haenv)
#endif
/******************************************************************************/
#endif /* __THIS_H_bb55b32c2226479dab1f126ea1ba1e9f__ */
