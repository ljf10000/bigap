#ifndef __MATH_H_8ac85024e292430e93a9fb835069dbfd__
#define __MATH_H_8ac85024e292430e93a9fb835069dbfd__
/******************************************************************************/
static inline int
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

#define os_hex2number(_hex, _len, _base, _type) ({ \
    _type n = 0;                                \
    char *str = (char *)_hex;                   \
    int __i;                                    \
                                                \
    for (__i=0; __i<_len; __i++)  {             \
        n *= (_type)_base;                      \
        n += (_type)os_chex2int(str[__i]);      \
    }                                           \
                                                \
    n;                                          \
})

static inline int
os_hextobin(char *hex, byte *buf, int size)
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

/*
* space NOT include '\0'
*/
static inline int
os_bintohex(char *hex, int space, byte *buf, int size)
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
#define os_atoi(_string)    simple_strtol(_string, NULL, 0)
#define os_atol(_string)    simple_strtol(_string, NULL, 0)
#define os_atoll(_string)   simple_strtoull(_string, NULL, 0)
#elif defined(__APP__)
#define __os_atox(_type, _func, _string)    ({  \
    _type x_in___os_atox = 0;                   \
    char *s_in___os_atox = (_string);           \
                                                \
    if (s_in___os_atox) {                       \
        x_in___os_atox = _func(s_in___os_atox); \
    }                                           \
                                                \
    x_in___os_atox;                             \
})  /* end */

#define os_atoi(_string)    __os_atox(int, atoi, _string)
#define os_atol(_string)    __os_atox(long, atol, _string)
#define os_atoll(_string)   __os_atox(long long, atoll, _string)
#else
#error "need to define atoi/atol/atoll"
#endif
/******************************************************************************/
#endif /* __MATH_H_8ac85024e292430e93a9fb835069dbfd__ */
