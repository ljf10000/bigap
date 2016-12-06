#ifndef __FORMAT_H_099e7e9b6a574b4e87f5851b04db1a9d__
#define __FORMAT_H_099e7e9b6a574b4e87f5851b04db1a9d__
/******************************************************************************/
#define os_isprint(_x)      ((_x)>=0x20 && (_x)<=0x7e)

#if defined(__BOOT__) || defined(__APP__)
#define os_printf(_fmt, _args...)               printf(_fmt, ##_args)
#define os_vprintf(_fmt, _args)                 vprintf(_fmt, _args)
#elif defined(__KERNEL__)
#define os_printf(_fmt, _args...)               printk(KERN_ALERT _fmt, ##_args)
#define os_vprintf(_fmt, _args)                 vprintk(KERN_ALERT _fmt, _args)
#else
#error  "need printf and vprintf"
#endif

#ifdef __APP__
#define os_fprintf(_stream, _fmt, _args...)     fprintf(_stream, _fmt, ##_args)
#define os_vfprintf(_stream, _fmt, _args)       vfprintf(_stream, _fmt, _args)

#define os_eprintf(_fmt, _args...)              os_fprintf(stderr, _fmt, ##_args)
#define os_evprintf(_fmt, _args)                os_vfprintf(stderr, _fmt, _args)
#else
#define os_fprintf(_stream, _fmt, _args...)     os_printf(_fmt, ##_args)
#define os_vfprintf(_stream, _fmt, _args)       os_vprintf(_fmt, _args)

#define os_eprintf(_fmt, _args...)              os_printf(_fmt, ##_args)
#define os_evprintf(_fmt, _args)                os_vprintf(_fmt, _args)
#endif

#define os_sscanf(_buf, _fmt, _args...)         sscanf(_buf, _fmt, ##_args)
#define os_vsscanf(_buf, _fmt, _args)           vsscanf(_buf, _fmt, _args)

#define os_println(_fmt, _args...)              os_printf(_fmt __crlf, ##_args)
#define os_vprintln(_fmt, _args)                os_vprintf(_fmt __crlf, _args)

#define __printab(_count) do{ \
    int i;                      \
                                \
    for (i=0; i<_count; i++) {  \
        os_printf(__tab);       \
    }                           \
}while(0)

#define os_printab(_count, _fmt, _args...) do{ \
    __printab(_count);          \
    os_println(_fmt, ##_args);  \
}while(0)

#define os_vprintab(count, _fmt, _args) do{ \
    __printab(_count);          \
    os_vprintln(_fmt, _args);   \
}while(0)

#define os_fprintln(_stream, _fmt, _args...)    os_fprintf(_stream, _fmt __crlf, ##_args)
#define os_vfprintln(_stream, _fmt, _args)      os_vfprintf(_stream, _fmt __crlf, _args)

#define os_eprintln(_fmt, _args...)             os_eprintf(_fmt __crlf, ##_args)
#define os_evprintln(_fmt, _args)               os_evprintf(_fmt __crlf, _args)

#define os_sprintf(_buf, _fmt, _args...)        sprintf(_buf, _fmt, ##_args)
#define os_vsprintf(_buf, _fmt, _args)          vsprintf(_buf, _fmt, _args)

#define os_sprintln(_buf, _fmt, _args...)       os_sprintf(_buf, _fmt __crlf, ##_args)
#define os_vsprintln(_buf, _fmt, _args)         os_vsprintf(_buf, _fmt __crlf, _args)

#define os_snprintf_is_full(_buffer_space, _snprintf_return_value) \
        ((_snprintf_return_value) >= (_buffer_space))

#ifdef __BOOT__

EXTERN int
os_vsnprintf(char *buf, int size, const char *fmt, va_list args);

EXTERN int
os_snprintf(char *buf, int size, const char *fmt, ...);

EXTERN int
os_vasprintf(char **buf, const char *fmt, va_list args);

EXTERN int
os_asprintf(char **buf, const char *fmt, ...);

#else
#define os_snprintf(_buf, _size, _fmt,_args...) snprintf(_buf, _size, _fmt, ##_args)
#define os_vsnprintf(_buf, _size, _fmt, _args)  vsnprintf(_buf, _size, _fmt, _args)

#define os_asprintf(_pbuf, _fmt, _args...)      asprintf(_pbuf, _fmt, ##_args)
#define os_vasprintf(_pbuf, _fmt, _args)        vasprintf(_pbuf, _fmt, _args)

extern int
os_vsprintf_size(const char *fmt, va_list args);

extern int
os_sprintf_size(const char *fmt, ...);

extern int
os_vasprintf_resv(char **buf, int resv, const char *fmt, va_list args);

extern int
os_asprintf_resv(char **buf, int resv, const char *fmt, ...);
#endif

/*
* snprintf for array buffer
*/
#define os_saprintf(_buf, _fmt, _args...)       ({  \
    BUILD_BUG_NOT_ARRAY(_buf);                      \
    os_snprintf(_buf, sizeof(_buf), _fmt, ##_args); \
})

#define os_vsaprintf(_buf, _fmt, _args)         ({  \
    BUILD_BUG_NOT_ARRAY(_buf);                      \
    os_vsnprintf(_buf, sizeof(_buf), _fmt, _args);  \
})

/*
* snprintf for array buffer + offset
*/
#define os_soprintf(_buf, _offset, _fmt, _args...)  ({  \
    BUILD_BUG_NOT_ARRAY(_buf);                          \
    os_snprintf(_buf+(_offset), sizeof(_buf)-(_offset), _fmt, ##_args); \
})

#define os_vsoprintf(_buf, _offset, _fmt, _args)    ({  \
    BUILD_BUG_NOT_ARRAY(_buf);                          \
    os_vsnprintf(_buf+(_offset), sizeof(_buf)-(_offset), _fmt, _args); \
})

EXTERN int
os_chex2int(int ch);

EXTERN int
os_hex2bin(char *hex, byte *buf, int size);

/*
* space NOT include '\0'
*/
EXTERN int
os_bin2hex(char *hex, int space, byte *buf, int size);

#ifdef __BOOT__
#include "weos/boot/format.c"
#endif
/******************************************************************************/
#endif /* __FORMAT_H_099e7e9b6a574b4e87f5851b04db1a9d__ */
