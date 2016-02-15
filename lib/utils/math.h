#ifndef __MATH_H_8ac85024e292430e93a9fb835069dbfd__
#define __MATH_H_8ac85024e292430e93a9fb835069dbfd__
/******************************************************************************/
static inline int
os_digitchar2int(int ch)
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

#define os_digitstring2number(_digitstring, _len, _base, _type) ({ \
    _type n = 0;                                \
    char *str = (char *)_digitstring;           \
    int __i;                                    \
                                                \
    for (__i=0; __i<_len; __i++)  {             \
        n *= (_type)_base;                      \
        n += (_type)os_digitchar2int(str[__i]); \
    }                                           \
                                                \
    n;                                          \
})

static inline int
os_hexstring2buf(char *hex, byte buf[], int len)
{
    int i;
    int hexlen = strlen(hex);

    if (hexlen%2) {
        return -EFORMAT;
    }
    else if (hexlen > 2*len) {
        return -ETOOBIG;
    }

    for (i=0; i<hexlen/2; i++) {
        buf[i] = 16 * os_digitchar2int(hex[2*i]) + os_digitchar2int(hex[2*i+1]);
    }

    return hexlen/2;
}
    

#ifdef __BOOT__
#define os_atoi(_string)    simple_strtol(_string, NULL, 0)
#define os_atol(_string)    simple_strtol(_string, NULL, 0)
#define os_atoll(_string)   simple_strtoull(_string, NULL, 0)
#elif defined(__APP__)
#define __os_atox(_type, _func, _string)  ({  \
    _type __x = 0;          \
    char *__s = (_string);  \
                            \
    if (__s) {              \
        __x = _func(__s);   \
    }                       \
                            \
    __x;                    \
})  /* end */

#define os_atoi(_string)    __os_atox(int, atoi, _string)
#define os_atol(_string)    __os_atox(long, atol, _string)
#define os_atoll(_string)   __os_atox(long long, atoll, _string)
#else
#error "need to define atoi/atol/atoll"
#endif
/******************************************************************************/
#endif /* __MATH_H_8ac85024e292430e93a9fb835069dbfd__ */
