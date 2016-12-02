/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
STATIC char *
__date_string(struct tm *tm, int ifs)
{
    static char current[1+FULLTIME_STRING_LEN];
    
    os_saprintf(current, "%04d%c%02d%c%02d",
                1900 + tm->tm_year, ifs, 1 + tm->tm_mon, ifs, tm->tm_mday);
    
    return current;
}

char *
os_date_string(time_t t)
{
    return __date_string(os_gettm(t), '-');
}

STATIC char *
__time_string(struct tm *tm, int ifs)
{
    static char current[1+FULLTIME_STRING_LEN];
    
    os_saprintf(current, "%02d%c%02d%c%02d",
                tm->tm_hour, ifs, tm->tm_min, ifs, (61==tm->tm_sec)?59:tm->tm_sec);
    
    return current;
}

char *
os_time_string(time_t t)
{
    return __time_string(os_gettm(t), ':');
}

char *
__fulltime_string(struct tm *tm, char ifs[3])
{
    static char current[1+FULLTIME_STRING_LEN];

    os_saprintf(current, "%s%c%s",
                __date_string(tm, ifs[0]),
                ifs[1],
                __time_string(tm, ifs[2]));

    return current;
}

char *
os_fulltime_string(time_t t)
{
    return __fulltime_string(os_gettm(t), __os_fulltime_ifs);
}

char *
os_fulltime_string_link(time_t t)
{
    return __fulltime_string(os_gettm(t), __os_fulltime_ifs_link);
}

STATIC time_t
__os_fulltime(char *fulltime, char *format)
{
    struct tm tm;

    if (strptime(fulltime, format, &tm)) {
        return mktime(&tm);
    } else {
        return time(NULL);
    }
}

time_t
os_fulltime(char *fulltime)
{
    return __os_fulltime(fulltime, "%Y-%m-%d %H:%M:%S");
}

time_t
os_fulltime_link(char *fulltime)
{
    return __os_fulltime(fulltime, "%Y-%m-%d-%H:%M:%S");
}

/******************************************************************************/
