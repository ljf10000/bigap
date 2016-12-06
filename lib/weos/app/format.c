/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
int
os_vsprintf_size(const char *fmt, va_list args)
{
    char tmp[4];

    /*
    * 这里只是计算需要多少空间
    */
    return os_vsnprintf(tmp, 0, fmt, args);
}

int
os_sprintf_size(const char *fmt, ...)
{
    va_list args;
    
    va_start(args, fmt);
    int len = os_vsprintf_size(fmt, args);
    va_end(args);
    
    return len;
}

int
os_vasprintf_resv(char **buf, int resv, const char *fmt, va_list args)
{
    int len = 0;
    va_list copy;
    
    va_copy(copy, args);
    len = os_vsnprintf(NULL, 0, fmt, copy);
    va_end(copy);
    
    char *p = (char *)os_zalloc(1+len+resv);
    if (NULL==p) {
        return -ENOMEM;
    }

    len = os_vsnprintf(p+resv, len, fmt, args);
    p[resv+len] = 0;
    
    *buf = p;

    return len;
}

int
os_asprintf_resv(char **buf, int resv, const char *fmt, ...)
{
    va_list args;

    va_start(args, (char *)fmt);
    int len = os_vasprintf_resv(buf, resv, fmt, args);
    va_end(args);

    return len;
}
/******************************************************************************/
