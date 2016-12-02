/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
DECLARE int
os_chex2int(int ch)
{
    switch(ch) {
        case '0' ... '9':
            return ch - '0';
        case 'a' ... 'f':
            return ch - 'a' + 10;
        case 'A' ... 'F':
            return ch - 'A' + 10;
        default:
            return os_assertV(0);
    }
}

DECLARE int
os_hex2bin(char *hex, byte *buf, int size)
{
    int i;
    int len = strlen(hex);

    if (len%2) {
        return -EBADHEX;
    }
    else if ((size + size) < len) {
        return -ENOSPACE;
    }

    int hexlen = len/2;
    for (i=0; i<hexlen; i++) {
        buf[i] = 16 * os_chex2int(hex[2*i]) + os_chex2int(hex[2*i+1]);
    }

    return hexlen;
}

DECLARE int
os_bin2hex(char *hex, int space, byte *buf, int size)
{
    int i, len = size+size;
    
    if (len < space) {
        return -ENOSPACE;
    }

    for (i=0; i<size; i++) {
        os_sprintf(hex + 2*i, "%.2X", buf[i]);
    }
    hex[len] = 0;
    
    return len;
}

#ifdef __BOOT__

#ifndef BOOT_SNPRINTF_BUFSIZE
#define BOOT_SNPRINTF_BUFSIZE   4096
#endif

DECLARE int
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

DECLARE int
os_snprintf(char *buf, int size, const char *fmt, ...)
{
    va_list args;

    va_start(args, (char *)fmt);
    int len = os_vsnprintf(buf, size, fmt, args);
    va_end(args);

    return len;
}

DECLARE int
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

DECLARE int
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
