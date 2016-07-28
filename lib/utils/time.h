#ifndef __TIME_H_9ed57e652d154962bbebe5adf9a06c11__
#define __TIME_H_9ed57e652d154962bbebe5adf9a06c11__
/******************************************************************************/
#define os_second(_ms)          ((_ms)/1000)
#define os_usecond(_ms)         (((_ms) % 1000) * 1000)
#define os_nsecond(_ms)         (((_ms) % 1000) * 1000 * 1000)

#define os_ms2tick(_ms, _tick)  (__os_align(_ms, _tick)/_tick)

#define __os_date_format        "1900-01-01"
#define __os_time_format        "00:00:00"
#define __os_fulltime_format    __os_date_format __space __os_time_format
#define __os_fulltime_ifs       "- :"
#define __os_fulltime_ifs_link  "--:"

enum { FULLTIME_STRING_LEN = sizeof(__os_fulltime_format) - 1 };

#ifdef __APP__
static inline struct tm *
os_localtime(time_t *c)
{
    if (c) {
        return localtime(c);
    } else {
        time_t t = time(NULL);

        return localtime(&t);
    }
}

static inline struct tm *
os_gmtime(time_t *c)
{
    if (c) {
        return gmtime(c);
    } else {
        time_t t = time(NULL);

        return gmtime(&t);
    }
}

#if OS_USE_UTC_TIME
#   define os_gettm(_time)  os_gmtime(_time)
#else
#   define os_gettm(_time)  os_localtime(_time)
#endif

static inline char *
__date_string(struct tm *tm, int ifs)
{
    static char current[1+FULLTIME_STRING_LEN];
    
    os_saprintf(current, "%04d%c%02d%c%02d",
                1900 + tm->tm_year, ifs, 1 + tm->tm_mon, ifs, tm->tm_mday);
    
    return current;
}

static inline char *
os_date_string(time_t *t)
{
    return __date_string(os_gettm(t), '-');
}

static inline char *
__time_string(struct tm *tm, int ifs)
{
    static char current[1+FULLTIME_STRING_LEN];
    
    os_saprintf(current, "%02d%c%02d%c%02d",
                tm->tm_hour, ifs, tm->tm_min, ifs, (61==tm->tm_sec)?59:tm->tm_sec);
    
    return current;
}

static inline char *
os_time_string(time_t *t)
{
    return __time_string(os_gettm(t), ':');
}

static inline char *
__fulltime_string(struct tm *tm, char ifs[3])
{
    static char current[1+FULLTIME_STRING_LEN];

    os_saprintf(current, "%s%c%s",
                __date_string(tm, ifs[0]),
                ifs[1],
                __time_string(tm, ifs[2]));

    return current;
}

static inline char *
os_fulltime_string(time_t *t)
{
    return __fulltime_string(os_gettm(t), __os_fulltime_ifs);
}

static inline char *
os_fulltime_string_link(time_t *t)
{
    return __fulltime_string(os_gettm(t), __os_fulltime_ifs_link);
}

#endif /* __APP__ */
/******************************************************************************/
#endif /* __TIME_H_9ed57e652d154962bbebe5adf9a06c11__ */
