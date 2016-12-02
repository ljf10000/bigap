#ifndef __MATH_H_8ac85024e292430e93a9fb835069dbfd__
#define __MATH_H_8ac85024e292430e93a9fb835069dbfd__
/******************************************************************************/
STATIC INLINE int
os_chex2int(int ch);

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

STATIC INLINE int
os_hex2bin(char *hex, byte *buf, int size);

/*
* space NOT include '\0'
*/
STATIC INLINE int
os_bin2hex(char *hex, int space, byte *buf, int size);

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
#define os_atof(_string)    __os_atox(double, atof, _string)
#else
#error "need to define atoi/atol/atoll"
#endif
/******************************************************************************/
#endif /* __MATH_H_8ac85024e292430e93a9fb835069dbfd__ */
