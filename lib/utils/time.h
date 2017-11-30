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

#define OS_EXAMPLE_DATE         "1900-01-01"
#define OS_EXAMPLE_TIME         "00:00:00"
#define OS_EXAMPLE_FULLTIME     OS_EXAMPLE_DATE __space OS_EXAMPLE_TIME
#define OS_IFS_FULLTIME         "-#:"

enum {
    DATE_STRING_LEN      = sizeof(OS_EXAMPLE_DATE) - 1,
    TIME_STRING_LEN      = sizeof(OS_EXAMPLE_TIME) - 1,
    FULLTIME_STRING_LEN  = sizeof(OS_EXAMPLE_FULLTIME) - 1,
};

typedef char date_string_t[1+DATE_STRING_LEN];
typedef char time_string_t[1+TIME_STRING_LEN];
typedef char fulltime_string_t[1+FULLTIME_STRING_LEN];

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

static inline char *
date_string_helper(time_t t, date_string_t string, int ifs)
{
    struct tm *tm = os_gettm(t);
    
    os_snprintf(string, 1+DATE_STRING_LEN,
                "%04d%c%02d%c%02d",
                OS_TIME_YEAR(tm), ifs, OS_TIME_MON(tm), ifs, OS_TIME_DAY(tm));

    return string;
}

static inline char *
date_string(time_t t, date_string_t string)
{
    return date_string_helper(t, string, '-');
}

static inline char *
unsafe_date_string(time_t t)
{
    static date_string_t string;
    
    return date_string(t, string);
}


static inline char *
time_string_helper(time_t t, time_string_t string, int ifs)
{
    struct tm *tm = os_gettm(t);
    
    os_snprintf(string, 1+TIME_STRING_LEN,
                "%02d%c%02d%c%02d",
                OS_TIME_HOUR(tm), ifs, OS_TIME_MIN(tm), ifs, OS_TIME_SEC(tm));
    
    return string;
}

static inline char *
time_string(time_t t, time_string_t string)
{
    return time_string_helper(t, string, ':');
}

static inline char *
unsafe_time_string(time_t t)
{
    static time_string_t string;
    
    return time_string(t, string);
}


static inline char *
fulltime_string_helper(time_t t, fulltime_string_t string, char ifs[3])
{
    struct tm *tm = os_gettm(t);

    os_snprintf(string, 1+FULLTIME_STRING_LEN,
                "%04d%c%02d%c%02d"
                "%c"
                "%02d%c%02d%c%02d",
                OS_TIME_YEAR(tm), ifs[0], OS_TIME_MON(tm), ifs[0], OS_TIME_DAY(tm),
                ifs[1],
                OS_TIME_HOUR(tm), ifs[2], OS_TIME_MIN(tm), ifs[2], OS_TIME_SEC(tm));

    return string;
}


static inline char *
fulltime_string(time_t t, fulltime_string_t string)
{
    return fulltime_string_helper(t, string, OS_IFS_FULLTIME);
}

static inline char *
unsafe_fulltime_string(time_t t)
{
    static fulltime_string_t string;
    
    return fulltime_string(t, string);
}


static inline time_t
os_fulltime_helper(char *fulltime, char *format)
{
    struct tm tm;

    if (strftime(fulltime, 1+FULLTIME_STRING_LEN, format, &tm)) {
        return mktime(&tm);
    } else {
        return time(NULL);
    }
}


static inline time_t
os_fulltime(char *fulltime)
{
    return os_fulltime_helper(fulltime, "%Y-%m-%d#%H:%M:%S");
}



#endif /* __APP__ */
/******************************************************************************/
#endif /* __TIME_H_9ed57e652d154962bbebe5adf9a06c11__ */
