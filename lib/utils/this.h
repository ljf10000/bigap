#ifndef __THIS_H_bb55b32c2226479dab1f126ea1ba1e9f__
#define __THIS_H_bb55b32c2226479dab1f126ea1ba1e9f__
/******************************************************************************/
#ifndef __THIS_APP
#error "must defined __THIS_APP before all #include"
#endif

#define __SYMBOL_CONTRACT2(x, y)            x##y
#define __SYMBOL_CONTRACT(x, y)             __SYMBOL_CONTRACT2(x, y)
#define OS_DEFINE(x)                        __SYMBOL_CONTRACT(x, __COUNTER__)

#define __SYMBOL_TO_STRING2(x)              #x
#define __SYMBOL_TO_STRING(x)               __SYMBOL_TO_STRING2(x)

#define __SYMBOL_TO_THIS3(_prefix, _name)   __THIS_##_prefix##_name
#define __SYMBOL_TO_THIS2(_prefix, _name)   __SYMBOL_TO_THIS3(_prefix, _name)

#if defined(__LIB__)
#   define __SYMBOL_TO_THIS(_name)          __SYMBOL_TO_THIS2(__THIS_APP, _name)
#elif defined(__ALLINONE__) || defined(__BOOT__) || defined(__APP__)
#   define __SYMBOL_TO_THIS(_name)          __SYMBOL_TO_THIS2(only, _name)
#else
#   error "error !!!"
#endif

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
/*
* pc-app/openwrt-app/boot/kernel/busybox: 
*   __THIS_only_debugger
*   __THIS_only_jdebugger
*   __THIS_only_command
*
* pc-lib/openwrt-lib:
*   __THIS_libjs_debugger
*   __THIS_libjs_jdebugger
*
*   __THIS_libweos_debugger
*   __THIS_libweos_jdebugger
*/
#ifndef __THIS_DEBUG
#define __THIS_DEBUG            __SYMBOL_TO_THIS(_debugger)
#endif

#ifndef __THIS_JDEBUG
#define __THIS_JDEBUG           __SYMBOL_TO_THIS(_jdebugger)
#endif

#ifndef __THIS_USAGE
#define __THIS_USAGE            "nothing"
#endif
/******************************************************************************/
#endif /* __THIS_H_bb55b32c2226479dab1f126ea1ba1e9f__ */
