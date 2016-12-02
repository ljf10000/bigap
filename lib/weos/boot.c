/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#include "boot/addr.c"
#include "boot/ak.c"
#include "boot/base64.c"
#include "boot/bcookie.c"
#include "boot/benv.c"
#include "boot/cmd.c"
#include "boot/crc32.c"
#include "boot/dump.c"
#include "boot/env.c"
#include "boot/format.c"
#include "boot/md5.c"
#include "boot/string.c"
/******************************************************************************/
#ifdef __BOOT__

#ifndef BOOT_SNPRINTF_BUFSIZE
#define BOOT_SNPRINTF_BUFSIZE   4096
#endif

int
os_vsnprintf(char *buf, int size, const char *fmt, va_list args)
{
    int bsize = size?size-1:0;
    int len = 0;
    
    char *p = (char *)os_zalloc(BOOT_SNPRINTF_BUFSIZE);
    if (NULL==p) {
        return -ENOMEM;
    }

    len = os_vsprintf(p, fmt, args);

    if (len <= bsize) {
        strcpy(buf, p);
    } else {
        len = bsize;
        os_memcpy(buf, p, len);
    }

error:
    os_free(p);

    return len;
}

int
os_snprintf(char *buf, int size, const char *fmt, ...)
{
    va_list args;

    va_start(args, (char *)fmt);
    int len = os_vsnprintf(buf, size, fmt, args);
    va_end(args);

    return len;
}

int
os_vasprintf(char **buf, const char *fmt, va_list args)
{
    int len = 0;
    
    char *p = (char *)os_zalloc(BOOT_SNPRINTF_BUFSIZE);
    if (NULL==p) {
        return -ENOMEM;
    }

    len = os_vsprintf(p, fmt, args);
    *buf = p;

    return len;
}

int
os_asprintf(char **buf, const char *fmt, ...)
{
    va_list args;

    va_start(args, (char *)fmt);
    int len = os_vasprintf(buf, fmt, args);
    va_end(args);

    return len;
}
#endif /* __BOOT__ */
/******************************************************************************/

