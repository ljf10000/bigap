#ifndef __TIME_H_9ed57e652d154962bbebe5adf9a06c11__
#define __TIME_H_9ed57e652d154962bbebe5adf9a06c11__
/******************************************************************************/
#define os_second(_ms)          ((_ms)/1000)
#define os_usecond(_ms)         (((_ms) % 1000) * 1000)
#define os_nsecond(_ms)         (((_ms) % 1000) * 1000 * 1000)

#define os_ms2tick(_ms, _tick)  (__os_align(_ms, _tick)/_tick)

#ifdef __BOOT__
#define os_usleep(_us)          udelay(_us)
#elif defined(__APP__)
#define os_usleep(_us)          usleep(_us)
#endif

#define __os_date_format        "1900-01-01"
#define __os_time_format        "00:00:00"
#define __os_fulltime_format    __os_date_format __space __os_time_format
#define __os_fulltime_ifs       "- :"
#define __os_fulltime_ifs_link  "--:"

enum { FULLTIME_STRING_LEN = sizeof(__os_fulltime_format) - 1 };

#ifdef __APP__
static inline struct tm *
os_localtime(time_t t)
{
    return localtime(&t);
}

static inline struct tm *
os_gmtime(time_t t)
{
    return gmtime(&t);
}

#if OS_USE_UTC_TIME
#   define os_gettm(_time)  os_gmtime(_time)
#else
#   define os_gettm(_time)  os_localtime(_time)
#endif

extern char *
os_date_string(time_t t);

extern char *
os_time_string(time_t t);

extern char *
__fulltime_string(struct tm *tm, char ifs[3]);

extern char *
os_fulltime_string(time_t t);

extern char *
os_fulltime_string_link(time_t t);

extern time_t
os_fulltime(char *fulltime);

extern time_t
os_fulltime_link(char *fulltime);

#endif /* __APP__ */
/******************************************************************************/
#endif /* __TIME_H_9ed57e652d154962bbebe5adf9a06c11__ */
