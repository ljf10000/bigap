#ifndef __JLOG_H_c174923fabe845e980f9379209210cc3__
#define __JLOG_H_c174923fabe845e980f9379209210cc3__
/******************************************************************************/
#ifndef JLOG_UNIX
#define JLOG_UNIX           "/tmp/.jlog.unix"
#endif

/*
* 2: AF_INET
* 1: AF_UNIX
*/
#ifndef JLOG_FAMILY
#define JLOG_FAMILY         AF_UNIX
#endif

#ifndef JLOG_SERVER
#define JLOG_SERVER         PC_VAL("0.0.0.0", "1.0.0.6")
#endif

#ifndef JLOG_PORT
#define JLOG_PORT           4110
#endif

#define JLOG_KEY_HEADER     "__header__"
#define JLOG_KEY_APP        "app"
#define JLOG_KEY_SUB        "sub"
#define JLOG_KEY_TYPE       "type"
#define JLOG_KEY_FILE       "file"
#define JLOG_KEY_FUNC       "func"
#define JLOG_KEY_LINE       "line"
#define JLOG_KEY_LEVEL      "level"
#define JLOG_KEY_PRI        "pri"
#define JLOG_KEY_TIME       "time"
#define JLOG_KEY_MAC        "mac"
#define JLOG_KEY_SEQ        "seq"

#define JLOG_KEY_TYPE_C     "c"
#define JLOG_KEY_TYPE_SH    "shell"

#ifdef __BOOT__
#define	LOG_EMERG	        0	/* system is unusable */
#define	LOG_ALERT	        1	/* action must be taken immediately */
#define	LOG_CRIT	        2	/* critical conditions */
#define	LOG_ERR		        3	/* error conditions */
#define	LOG_WARNING	        4	/* warning conditions */
#define	LOG_NOTICE	        5	/* normal but significant condition */
#define	LOG_INFO	        6	/* informational */
#define	LOG_DEBUG	        7	/* debug-level messages */
#define	LOG_PRIMASK	        0x07	/* mask to extract priority part (internal) */
				            /* extract priority */
#define	LOG_PRI(p)	((p) & LOG_PRIMASK)
#endif

#define	JLOG_MAKEPRI(fac, pri)	    (((fac) << 3) | (pri))

/*
* get level from PRI
*/
#define JLOG_LEVEL(_PRI)            ((_PRI) >> 3)
/*
* make PRI by level & pri
*/
// #define JLOG_DEBUGLEVEL(_level)     JLOG_MAKEPRI(_level, LOG_DEBUG)

static inline uint32
JLOG_DEBUGLEVEL(uint32 level)
{
    uint32 DEBUGLEVEL = JLOG_MAKEPRI(level, LOG_DEBUG);

#if 0
    os_println("level=%d, DEBUGLEVEL=0x%x", level, DEBUGLEVEL);
#endif

    return DEBUGLEVEL;
}

static inline void
__jlog_header(
    const char *app,
    const char *sub,
    const char *file,
    const char *func,
    uint32 line,
    uint32 PRI)
{
    int level = JLOG_LEVEL(PRI);
    
    os_eprintf("[app:%s", app);
    if (sub) {
        os_eprintf(",sub:%s", sub);
    }
    if (file) {
        os_eprintf(",file:%s", file);
    }
    if (func) {
        os_eprintf(",func:%s", func);
    }
    if (line) {
        os_eprintf(",line:%d", line);
    }
    if (level) {
        os_eprintf(",level:%s", __ak_debug_getname(level));
    }
    os_eprintf("] ");
}

#define __jlog_vprintf(_app, _sub, _file, _func, _line, _PRI, _fmt, _args) ({ \
    __jlog_header(_app, _sub, _file, _func, _line, _PRI); \
    os_evprintln(__tab _fmt, _args);  \
    0;                          \
})  /* end */

#define __jlog_printf(_app, _sub, _file, _func, _line, _PRI, _fmt, _args...) ({ \
    __jlog_header(_app, _sub, _file, _func, _line, _PRI); \
    os_eprintln(__tab _fmt, ##_args); \
    0;                          \
})  /* end */

#define jlog_vprintf(_sub, _PRI, _fmt, _args)     \
    __jlog_vprintf(__THIS_APPNAME, _sub, __THIS_FILENAME, __func__, __LINE__, _PRI, _fmt, ##_args)

#define jlog_printf(_sub, _PRI, _fmt, _args...)   \
    __jlog_printf(__THIS_APPNAME, _sub, __THIS_FILENAME, __func__, __LINE__, _PRI, _fmt, ##_args)

#ifdef __APP__
#define use_jlog_connect    1
#define use_jlog_bind       0

extern int
__jlog(jobj_t obj, const char *app, const char *sub, uint32 PRI);

/*
* v: as vsprintf
*/
extern int
__jvlogger(
    const char *app, 
    const char *sub, 
    const char *file, 
    const char *func, 
    uint32 line, 
    uint32 PRI, 
    const char *fmt, 
    va_list args
);

/*
* v: as vsprintf
*/
extern int
__dvlogger(
    const char *app, 
    const char *sub, 
    const char *file, 
    const char *func, 
    uint32 line, 
    uint32 PRI, 
    const char *fmt, 
    va_list args
);

extern int
__jlogger(
    const char *app, 
    const char *sub, 
    const char *file, 
    const char *func, 
    uint32 line, 
    uint32 PRI, 
    const char *fmt, 
    ...
);

extern int
__dlogger(
    const char *app, 
    const char *sub, 
    const char *file, 
    const char *func, 
    uint32 line, 
    uint32 PRI, 
    const char *fmt, 
    ...
);

/*
* c: by command
*/
extern int
__clogger(
    const char *app, 
    const char *sub, 
    const char *file, 
    const char *func, 
    uint32 line,
    uint32 PRI,
    char *json
);

/*
* by command
*/
extern int
__jformat(
    const char *app, 
    const char *sub, 
    const char *file, 
    const char *func, 
    uint32 line, 
    uint32 PRI, 
    const char *fmt, 
    int argc,
    char *argv[]
);

extern void
__jlog_close(void);

extern int
__jlog_init(void);

static inline int
jlog_init(void)
{
    int err;

    err = __jlog_init();
    if (err<0) {
        return err;
    }

    os_println(__THIS_APPNAME " " __THIS_FILENAME " jlog init OK!");

    return 0;
}

static inline void
jlog_fini(void)
{
#ifndef __JLOGD__
    __jlog_close();
#endif
}

#else
#define DECLARE_JLOG        os_fake_declare

#define __jvlogger(_app, _sub, _file, _func, _line, _PRI, _fmt, _args) \
        __jlog_vprintf(_app, _sub, _file, _func, _line, _PRI, _fmt, _args)

#define __dvlogger(_app, _sub, _file, _func, _line, _PRI, _fmt, _args) \
        __jlog_vprintf(_app, _sub, _file, _func, _line, _PRI, _fmt, _args)

#define __jlogger(_app, _sub, _file, _func, _line, _PRI, _fmt, _args...) \
        __jlog_printf(_app, _sub, _file, _func, _line, _PRI, _fmt, ##_args)

#define __dlogger(_app, _sub, _file, _func, _line, _PRI, _fmt, _args...) \
        __jlog_printf(_app, _sub, _file, _func, _line, _PRI, _fmt, ##_args)

#define __clogger(_app, _sub, _file, _func, _line, _PRI, _json) \
        __jlog_printf(_app, _sub, _file, _func, _line, _PRI, "%s", _json)

#define jlog_init()     0
#define jlog_fini()     os_do_nothing()
#endif

#define jvlogger(_sub, _PRI, _fmt, _args)     \
    __jvlogger(__THIS_APPNAME, _sub, __THIS_FILENAME, __func__, __LINE__, _PRI, _fmt, ##_args)

#define dvlogger(_sub, _PRI, _fmt, _args)     \
    __dvlogger(__THIS_APPNAME, _sub, __THIS_FILENAME, __func__, __LINE__, _PRI, _fmt, ##_args)

#define jlogger(_sub, _PRI, _fmt, _args...)   \
    __jlogger(__THIS_APPNAME, _sub, __THIS_FILENAME, __func__, __LINE__, _PRI, _fmt, ##_args)

#define dlogger(_sub, _PRI, _fmt, _args...)   \
    __dlogger(__THIS_APPNAME, _sub, __THIS_FILENAME, __func__, __LINE__, _PRI, _fmt, ##_args)

#define clogger(_sub, _PRI, _json)   \
    __clogger(__THIS_APPNAME, _sub, __THIS_FILENAME, __func__, __LINE__, _PRI, _json)

#define __jvemerg(_sub, _fmt, _args)        jvlogger(_sub, LOG_EMERG, _fmt, _args)
#define __jvalert(_sub, _fmt, _args)        jvlogger(_sub, LOG_ALERT, _fmt, _args)
#define __jvcrit(_sub, _fmt, _args)         jvlogger(_sub, LOG_CRIT, _fmt, _args)
#define __jverror(_sub, _fmt, _args)        jvlogger(_sub, LOG_ERR, _fmt, _args)
#define __jvwarning(_sub, _fmt, _args)      jvlogger(_sub, LOG_WARNING, _fmt, _args)
#define __jvnotice(_sub, _fmt, _args)       jvlogger(_sub, LOG_NOTICE, _fmt, _args)
#define __jvinfo(_sub, _fmt, _args)         jvlogger(_sub, LOG_INFO, _fmt, _args)
#define __jvdebug(_sub, _fmt, _args)        jvlogger(_sub, LOG_DEBUG, _fmt, _args)

#define __jemerg(_sub, _fmt, _args...)      jlogger(_sub, LOG_EMERG, _fmt, ##_args)
#define __jalert(_sub, _fmt, _args...)      jlogger(_sub, LOG_ALERT, _fmt, ##_args)
#define __jcrit(_sub, _fmt, _args...)       jlogger(_sub, LOG_CRIT, _fmt, ##_args)
#define __jerror(_sub, _fmt, _args...)      jlogger(_sub, LOG_ERR, _fmt, ##_args)
#define __jwarning(_sub, _fmt, _args...)    jlogger(_sub, LOG_WARNING, _fmt, ##_args)
#define __jnotice(_sub, _fmt, _args...)     jlogger(_sub, LOG_NOTICE, _fmt, ##_args)
#define __jinfo(_sub, _fmt, _args...)       jlogger(_sub, LOG_INFO, _fmt, ##_args)
#define __jdebug(_sub, _fmt, _args...)      jlogger(_sub, LOG_DEBUG, _fmt, ##_args)

#define jvemerg(_fmt, _args)        __jvemerg(NULL, _fmt, _args)
#define jvalert(_fmt, _args)        __jvalert(NULL, _fmt, _args)
#define jvcrit(_fmt, _args)         __jvcrit(NULL, _fmt, _args)
#define jverror(_fmt, _args)        __jverror(NULL, _fmt, _args)
#define jvwarning(_fmt, _args)      __jvwarning(NULL, _fmt, _args)
#define jvnotice(_fmt, _args)       __jvnotice(NULL, _fmt, _args)
#define jvinfo(_fmt, _args)         __jvinfo(NULL, _fmt, _args)
#define jvdebug(_fmt, _args)        __jvdebug(NULL, _fmt, _args)

#define jemerg(_fmt, _args...)      __jemerg(NULL, _fmt, ##_args)
#define jalert(_fmt, _args...)      __jalert(NULL, _fmt, ##_args)
#define jcrit(_fmt, _args...)       __jcrit(NULL, _fmt, ##_args)
#define jerror(_fmt, _args...)      __jerror(NULL, _fmt, ##_args)
#define jwarning(_fmt, _args...)    __jwarning(NULL, _fmt, ##_args)
#define jnotice(_fmt, _args...)     __jnotice(NULL, _fmt, ##_args)
#define jinfo(_fmt, _args...)       __jinfo(NULL, _fmt, ##_args)
#define jdebug(_fmt, _args...)      __jdebug(NULL, _fmt, ##_args)

#define jvdlogger(_logger, _level, _fmt, _args)  do{ \
    if (__is_ak_debug(_level)) {                    \
        _logger(NULL, JLOG_DEBUGLEVEL(_level), _fmt, _args);  \
    }                                               \
}while(0)

#define jdlogger(_logger, _level, _fmt, _args...)  do{ \
    if (__is_ak_debug(_level)) {                  \
        _logger(NULL, JLOG_DEBUGLEVEL(_level), _fmt, ##_args); \
    }                                               \
}while(0)

/*
* use for logger inline function (__JLOGGER__/__JLOGD__/__BOOT__)
*/
#define __vdebug_ok(_fmt, _args)        jvdlogger(jlog_vprintf, __ak_debug_ok, _fmt, _args)
#define __vdebug_bug(_fmt, _args)       jvdlogger(jlog_vprintf, __ak_debug_bug, _fmt, _args)
#define __vdebug_error(_fmt, _args)     jvdlogger(jlog_vprintf, __ak_debug_error, _fmt, _args)
#define __vdebug_trace(_fmt, _args)     jvdlogger(jlog_vprintf, __ak_debug_trace, _fmt, _args)
#define __vdebug_init(_fmt, _args)      jvdlogger(jlog_vprintf, __ak_debug_init, _fmt, _args)
#define __vdebug_config(_fmt, _args)    jvdlogger(jlog_vprintf, __ak_debug_config, _fmt, _args)
#define __vdebug_json(_fmt, _args)      jvdlogger(jlog_vprintf, __ak_debug_json, _fmt, _args)
#define __vdebug_io(_fmt, _args)        jvdlogger(jlog_vprintf, __ak_debug_io, _fmt, _args)
#define __vdebug_timer(_fmt, _args)     jvdlogger(jlog_vprintf, __ak_debug_timer, _fmt, _args)
#define __vdebug_timeout(_fmt, _args)   jvdlogger(jlog_vprintf, __ak_debug_timeout, _fmt, _args)
#define __vdebug_aging(_fmt, _args)     jvdlogger(jlog_vprintf, __ak_debug_aging, _fmt, _args)
#define __vdebug_event(_fmt, _args)     jvdlogger(jlog_vprintf, __ak_debug_event, _fmt, _args)
#define __vdebug_entry(_fmt, _args)     jvdlogger(jlog_vprintf, __ak_debug_entry, _fmt, _args)
#define __vdebug_packet(_fmt, _args)    jvdlogger(jlog_vprintf, __ak_debug_packet, _fmt, _args)
#define __vdebug_proto(_fmt, _args)     jvdlogger(jlog_vprintf, __ak_debug_proto, _fmt, _args)
#define __vdebug_flow(_fmt, _args)      jvdlogger(jlog_vprintf, __ak_debug_flow, _fmt, _args)
#define __vdebug_file(_fmt, _args)      jvdlogger(jlog_vprintf, __ak_debug_file, _fmt, _args)
#define __vdebug_signal(_fmt, _args)    jvdlogger(jlog_vprintf, __ak_debug_signal, _fmt, _args)
#define __vdebug_shell(_fmt, _args)     jvdlogger(jlog_vprintf, __ak_debug_shell, _fmt, _args)
#define __vdebug_gc(_fmt, _args)        jvdlogger(jlog_vprintf, __ak_debug_gc, _fmt, _args)
#define __vdebug_js(_fmt, _args)        jvdlogger(jlog_vprintf, __ak_debug_js, _fmt, _args)
#define __vdebug_format(_fmt, _args)    jvdlogger(jlog_vprintf, __ak_debug_format, _fmt, _args)
#define __vdebug_cli(_fmt, _args)       jvdlogger(jlog_vprintf, __ak_debug_cli, _fmt, _args)
#define __vdebug_loop(_fmt, _args)      jvdlogger(jlog_vprintf, __ak_debug_loop, _fmt, _args)
#define __vdebug_test(_fmt, _args)      jvdlogger(jlog_vprintf, __ak_debug_test, _fmt, _args)

#define __debug_ok(_fmt, _args...)      jdlogger(jlog_printf, __ak_debug_ok, _fmt, ##_args)
#define __debug_bug(_fmt, _args...)     jdlogger(jlog_printf, __ak_debug_bug, _fmt, ##_args)
#define __debug_error(_fmt, _args...)   jdlogger(jlog_printf, __ak_debug_error, _fmt, ##_args)
#define __debug_trace(_fmt, _args...)   jdlogger(jlog_printf, __ak_debug_trace, _fmt, ##_args)
#define __debug_init(_fmt, _args...)    jdlogger(jlog_printf, __ak_debug_init, _fmt, ##_args)
#define __debug_config(_fmt, _args...)  jdlogger(jlog_printf, __ak_debug_config, _fmt, ##_args)
#define __debug_json(_fmt, _args...)    jdlogger(jlog_printf, __ak_debug_json, _fmt, ##_args)
#define __debug_io(_fmt, _args...)      jdlogger(jlog_printf, __ak_debug_io, _fmt, ##_args)
#define __debug_timer(_fmt, _args...)   jdlogger(jlog_printf, __ak_debug_timer, _fmt, ##_args)
#define __debug_timeout(_fmt, _args...) jdlogger(jlog_printf, __ak_debug_timeout, _fmt, ##_args)
#define __debug_aging(_fmt, _args...)   jdlogger(jlog_printf, __ak_debug_aging, _fmt, ##_args)
#define __debug_event(_fmt, _args...)   jdlogger(jlog_printf, __ak_debug_event, _fmt, ##_args)
#define __debug_entry(_fmt, _args...)   jdlogger(jlog_printf, __ak_debug_entry, _fmt, ##_args)
#define __debug_packet(_fmt, _args...)  jdlogger(jlog_printf, __ak_debug_packet, _fmt, ##_args)
#define __debug_proto(_fmt, _args...)   jdlogger(jlog_printf, __ak_debug_proto, _fmt, ##_args)
#define __debug_flow(_fmt, _args...)    jdlogger(jlog_printf, __ak_debug_flow, _fmt, ##_args)
#define __debug_file(_fmt, _args...)    jdlogger(jlog_printf, __ak_debug_file, _fmt, ##_args)
#define __debug_signal(_fmt, _args...)  jdlogger(jlog_printf, __ak_debug_signal, _fmt, ##_args)
#define __debug_shell(_fmt, _args...)   jdlogger(jlog_printf, __ak_debug_shell, _fmt, ##_args)
#define __debug_gc(_fmt, _args...)      jdlogger(jlog_printf, __ak_debug_gc, _fmt, ##_args)
#define __debug_js(_fmt, _args...)      jdlogger(jlog_printf, __ak_debug_js, _fmt, ##_args)
#define __debug_format(_fmt, _args...)  jdlogger(jlog_printf, __ak_debug_format, _fmt, ##_args)
#define __debug_cli(_fmt, _args...)     jdlogger(jlog_printf, __ak_debug_cli, _fmt, ##_args)
#define __debug_loop(_fmt, _args...)    jdlogger(jlog_printf, __ak_debug_loop, _fmt, ##_args)
#define __debug_test(_fmt, _args...)    jdlogger(jlog_printf, __ak_debug_test, _fmt, ##_args)

/*
* use for others
*/
#define vdebug_ok(_fmt, _args)          jvdlogger(dvlogger, __ak_debug_ok, _fmt, _args)
#define vdebug_bug(_fmt, _args)         jvdlogger(dvlogger, __ak_debug_bug, _fmt, _args)
#define vdebug_error(_fmt, _args)       jvdlogger(dvlogger, __ak_debug_error, _fmt, _args)
#define vdebug_trace(_fmt, _args)       jvdlogger(dvlogger, __ak_debug_trace, _fmt, _args)
#define vdebug_init(_fmt, _args)        jvdlogger(dvlogger, __ak_debug_init, _fmt, _args)
#define vdebug_config(_fmt, _args)      jvdlogger(dvlogger, __ak_debug_config, _fmt, _args)
#define vdebug_json(_fmt, _args)        jvdlogger(dvlogger, __ak_debug_json, _fmt, _args)
#define vdebug_io(_fmt, _args)          jvdlogger(dvlogger, __ak_debug_io, _fmt, _args)
#define vdebug_timer(_fmt, _args)       jvdlogger(dvlogger, __ak_debug_timer, _fmt, _args)
#define vdebug_timeout(_fmt, _args)     jvdlogger(dvlogger, __ak_debug_timeout, _fmt, _args)
#define vdebug_aging(_fmt, _args)       jvdlogger(dvlogger, __ak_debug_aging, _fmt, _args)
#define vdebug_event(_fmt, _args)       jvdlogger(dvlogger, __ak_debug_event, _fmt, _args)
#define vdebug_entry(_fmt, _args)       jvdlogger(dvlogger, __ak_debug_entry, _fmt, _args)
#define vdebug_packet(_fmt, _args)      jvdlogger(dvlogger, __ak_debug_packet, _fmt, _args)
#define vdebug_proto(_fmt, _args)       jvdlogger(dvlogger, __ak_debug_proto, _fmt, _args)
#define vdebug_flow(_fmt, _args)        jvdlogger(dvlogger, __ak_debug_flow, _fmt, _args)
#define vdebug_file(_fmt, _args)        jvdlogger(dvlogger, __ak_debug_file, _fmt, _args)
#define vdebug_signal(_fmt, _args)      jvdlogger(dvlogger, __ak_debug_signal, _fmt, _args)
#define vdebug_shell(_fmt, _args)       jvdlogger(dvlogger, __ak_debug_shell, _fmt, _args)
#define vdebug_gc(_fmt, _args)          jvdlogger(dvlogger, __ak_debug_gc, _fmt, _args)
#define vdebug_js(_fmt, _args)          jvdlogger(dvlogger, __ak_debug_js, _fmt, _args)
#define vdebug_format(_fmt, _args)      jvdlogger(dvlogger, __ak_debug_format, _fmt, _args)
#define vdebug_cli(_fmt, _args)         jvdlogger(dvlogger, __ak_debug_cli, _fmt, _args)
#define vdebug_loop(_fmt, _args)        jvdlogger(dvlogger, __ak_debug_loop, _fmt, _args)
#define vdebug_test(_fmt, _args)        jvdlogger(dvlogger, __ak_debug_test, _fmt, _args)

#define debug_ok(_fmt, _args...)        jdlogger(dlogger, __ak_debug_ok, _fmt, ##_args)
#define debug_bug(_fmt, _args...)       jdlogger(dlogger, __ak_debug_bug, _fmt, ##_args)
#define debug_error(_fmt, _args...)     jdlogger(dlogger, __ak_debug_error, _fmt, ##_args)
#define debug_trace(_fmt, _args...)     jdlogger(dlogger, __ak_debug_trace, _fmt, ##_args)
#define debug_init(_fmt, _args...)      jdlogger(dlogger, __ak_debug_init, _fmt, ##_args)
#define debug_config(_fmt, _args...)    jdlogger(dlogger, __ak_debug_config, _fmt, ##_args)
#define debug_json(_fmt, _args...)      jdlogger(dlogger, __ak_debug_json, _fmt, ##_args)
#define debug_io(_fmt, _args...)        jdlogger(dlogger, __ak_debug_io, _fmt, ##_args)
#define debug_timer(_fmt, _args...)     jdlogger(dlogger, __ak_debug_timer, _fmt, ##_args)
#define debug_timeout(_fmt, _args...)   jdlogger(dlogger, __ak_debug_timeout, _fmt, ##_args)
#define debug_aging(_fmt, _args...)     jdlogger(dlogger, __ak_debug_aging, _fmt, ##_args)
#define debug_event(_fmt, _args...)     jdlogger(dlogger, __ak_debug_event, _fmt, ##_args)
#define debug_entry(_fmt, _args...)     jdlogger(dlogger, __ak_debug_entry, _fmt, ##_args)
#define debug_packet(_fmt, _args...)    jdlogger(dlogger, __ak_debug_packet, _fmt, ##_args)
#define debug_proto(_fmt, _args...)     jdlogger(dlogger, __ak_debug_proto, _fmt, ##_args)
#define debug_flow(_fmt, _args...)      jdlogger(dlogger, __ak_debug_flow, _fmt, ##_args)
#define debug_file(_fmt, _args...)      jdlogger(dlogger, __ak_debug_file, _fmt, ##_args)
#define debug_signal(_fmt, _args...)    jdlogger(dlogger, __ak_debug_signal, _fmt, ##_args)
#define debug_shell(_fmt, _args...)     jdlogger(dlogger, __ak_debug_shell, _fmt, ##_args)
#define debug_gc(_fmt, _args...)        jdlogger(dlogger, __ak_debug_gc, _fmt, ##_args)
#define debug_js(_fmt, _args...)        jdlogger(dlogger, __ak_debug_js, _fmt, ##_args)
#define debug_format(_fmt, _args...)    jdlogger(dlogger, __ak_debug_format, _fmt, ##_args)
#define debug_cli(_fmt, _args...)       jdlogger(dlogger, __ak_debug_cli, _fmt, ##_args)
#define debug_loop(_fmt, _args...)      jdlogger(dlogger, __ak_debug_loop, _fmt, ##_args)
#define debug_test(_fmt, _args...)      jdlogger(dlogger, __ak_debug_test, _fmt, ##_args)

#define __trace_error(_err, _trace_ok, _trace_error, _fmt, _args...) do{    \
    if (_err<0) {                                     \
        _trace_error("[ERROR:%d]" _fmt, _err, ##_args); \
    } else {                                        \
        _trace_ok("[OK]" _fmt, ##_args);            \
    }                                               \
}while(0)

#define trace_error(_err, _fmt, _args...)           \
        __trace_error(_err, os_eprintln, os_eprintln, _fmt, ##_args)

#define __debug_ok_error(_err, _fmt, _args...)      \
    __trace_error(_err, __debug_ok, __debug_error, _fmt, ##_args)

#define debug_ok_error(_err, _fmt, _args...)        \
    __trace_error(_err, debug_ok, debug_error, _fmt, ##_args)

#define __debug_trace_error(_err, _fmt, _args...)   \
    __trace_error(_err, __debug_trace, __debug_error, _fmt, ##_args)

#define debug_trace_error(_err, _fmt, _args...)     \
    __trace_error(_err, debug_trace, debug_error, _fmt, ##_args)

#define __debug_assert(_condition, _debug, _fmt, _args...) do{ \
    bool __cond = _condition;   \
    if (!__cond) {              \
        _debug(_fmt, ##_args);  \
        os_assert(0);           \
    }                           \
}while(0)

#define __trace_assert(_condition, _fmt, _args...)  \
    __debug_assert(_condition, __debug_trace, _fmt, ##_args)

#define trace_assert(_condition, _fmt, _args...)    \
    __debug_assert(_condition, debug_trace, _fmt, ##_args)

#define __error_assert(_condition, _fmt, _args...)  \
    __debug_assert(_condition, __debug_error, _fmt, ##_args)

#define error_assert(_condition, _fmt, _args...)    \
    __debug_assert(_condition, debug_error, _fmt, ##_args)

#define __debug_assertV(_value, _debug, _fmt, _args...) ({ \
    _debug(_fmt, ##_args);  \
    os_assertV(_value);     \
})  /* end */

#define __trace_assertV(_value, _fmt, _args...) \
    __debug_assertV(_value, __debug_trace, _fmt, ##_args)

#define trace_assertV(_value, _fmt, _args...)   \
    __debug_assertV(_value, debug_trace, _fmt, ##_args)

#define __error_assertV(_value, _fmt, _args...) \
    __debug_assertV(_value, __debug_error, _fmt, ##_args)

#define error_assertV(_value, _fmt, _args...)   \
    __debug_assertV(_value, debug_error, _fmt, ##_args)
/******************************************************************************/
#endif /* __JLOG_H_c174923fabe845e980f9379209210cc3__ */
