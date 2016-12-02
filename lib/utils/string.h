#ifndef __STRING_H_433874baec1b41d58eef119d11851217__
#define __STRING_H_433874baec1b41d58eef119d11851217__
/******************************************************************************/
#ifndef __empty
#define __empty         ""
#endif

#ifndef __zero
#define __zero          "\0"
#endif

#ifndef __space
#define __space         " "
#endif

#ifndef __null
#define __null          "null"
#endif

#ifndef __nil
#define __nil           "nil"
#endif

#ifndef __ok
#define __ok            "ok"
#endif

#ifndef __fail
#define __fail          "fail"
#endif

#ifndef __space2
#define __space2        __space __space
#endif

#ifndef __space4
#define __space4        __space2 __space2
#endif

#ifndef __blanks
#define __blanks        "\t \r\n"
#endif

#ifndef __ok_string
#define __ok_string(__is_ok)        ((__is_ok)?__ok:__fail)
#endif

#ifndef __true_string
#define __true_string(__is_true)    ((__is_true)?"true":"false")
#endif

#ifndef __tab
#   ifndef __TAB_AS_SPACE
#       define __tab            "\t"
#   elif 2==__TAB_AS_SPACE
#       define __tab            __space2
#   else
#       define __tab            __space4
#   endif
#endif

#ifndef __tab2
#define __tab2          __tab __tab
#endif

#ifndef __tab3
#define __tab3          __tab2 __tab
#endif

#ifndef __tab4
#define __tab4          __tab3 __tab
#endif

#ifndef __crlf
#define __crlf          "\n"
#endif

#ifndef __crlf2
#define __crlf2         __crlf __crlf
#endif

#ifndef __crlf3
#define __crlf3         __crlf2 __crlf
#endif

#ifndef __crlf4
#define __crlf4         __crlf3 __crlf
#endif

#ifndef __notes
#define __notes         "#"
#endif

#ifndef __unknow
#define __unknow        "unknow"
#endif

#ifndef __true
#define __true          "true"
#endif

#ifndef __false
#define __false         "false"
#endif

static inline bool
is_good_str(const char *s)
{
    return s && s[0];
}

static inline char *
os_safe_str(const char *s)
{
    return s?(char *)s:__nil;
}

STATIC INLINE int
os_strcount(const char *s, int ch);

static inline int
os_strlen(const char *s)
{
    return s?strlen(s):0;
}

static inline char *
os_strdup(const char *s)
{
    return s?strdup(s):NULL;
}

static inline char *
os_strcpy(char *dst, const char *src)
{
    return (dst && src)?strcpy(dst, src):dst;
}

/* len is src's length */
STATIC INLINE char *
os_strmcpy(char *dst, const char *src, int len);

/*
* no use strncpy(is unsafe)
*/
STATIC INLINE uint32 
os_strlcpy(char *dst, const char *src, uint32 size);

#ifndef os_strdcpy
#define os_strdcpy(_dst, _src)          ({  \
    BUILD_BUG_NOT_ARRAY(_dst);              \
    os_strlcpy(_dst, _src, sizeof(_dst));   \
})
#endif

#ifndef os_strscpy
#define os_strscpy(_dst, _src)          ({  \
    BUILD_BUG_NOT_ARRAY(_src);              \
    os_strlcpy(_dst, _src, sizeof(_src));   \
})
#endif

STATIC INLINE int
os_strcmp(const char *a, const char *b);

static inline bool
os_streq(const char *a, const char *b)
{
    return 0==os_strcmp(a, b);
}

static inline bool
os_strneq(const char *a, const char *b)
{
    return !!os_strcmp(a, b);
}

STATIC INLINE int
os_strncmp(const char *a, const char *b, int len);

/*
* use a's size
*/
#ifndef os_stracmp
#define os_stracmp(_a, _b)          ({  \
    BUILD_BUG_NOT_ARRAY(_a);            \
    os_strncmp(_a, _b, sizeof(_a)-1);   \
})
#endif

/*
* use b's size
*/
#ifndef os_strbcmp
#define os_strbcmp(_a, _b)          ({  \
    BUILD_BUG_NOT_ARRAY(_b);            \
    os_strncmp(_a, _b, sizeof(_b)-1);   \
})
#endif

/*
* use a's size
*/
#ifndef os_straeq
#define os_straeq(_a, _b)           (0==os_stracmp(_a, _b))
#endif

/*
* use b's size
*/
#ifndef os_strbeq
#define os_strbeq(_a, _b)           (0==os_strbcmp(_a, _b))
#endif

#ifndef os_strtok
#define os_strtok(_s, _delim)       strtok(_s, _delim)
#endif

#ifndef os_strchr
#define os_strchr(_s, _ch)          strchr(_s, _ch)
#endif

#ifndef os_strrchr
#define os_strrchr(_s, _ch)         strrchr(_s, _ch)
#endif

#ifndef os_strstr
#define os_strstr(_s, _sub)         strstr(_s, _sub)
#endif

#ifndef os_strrstr
#define os_strrstr(_s, _sub)        strrstr(_s, _sub)
#endif

#define os_strtok_foreach(_sub, _s, _delim) \
        for ((_sub)=os_strtok(_s, _delim);(_sub);(_sub)=os_strtok(NULL, _delim))

#ifdef __BOOT__
#ifndef os_strtol
#define os_strtol(_nptr, _endptr, _base)    simple_strtol(_nptr, _endptr, _base)
#endif

#ifndef os_strtoll
#define os_strtoll(_nptr, _endptr, _base)   simple_strtoll(_nptr, _endptr, _base)
#endif

#ifndef os_strtoul
#define os_strtoul(_nptr, _endptr, _base)   simple_strtoul(_nptr, _endptr, _base)
#endif

#ifndef os_strtoull
#define os_strtoull(_nptr, _endptr, _base)  simple_strtoull(_nptr, _endptr, _base)
#endif
#else /* __BOOT__ */
#ifndef os_strtol
#define os_strtol(_nptr, _endptr, _base)    strtol(_nptr, _endptr, _base)
#endif

#ifndef os_strtoll
#define os_strtoll(_nptr, _endptr, _base)   strtoll(_nptr, _endptr, _base)
#endif

#ifndef os_strtoul
#define os_strtoul(_nptr, _endptr, _base)   strtoul(_nptr, _endptr, _base)
#endif

#ifndef os_strtoull
#define os_strtoull(_nptr, _endptr, _base)  strtoull(_nptr, _endptr, _base)
#endif
#endif /* __BOOT__ */

#ifndef os_strton_is_good_end
#define os_strton_is_good_end(_endptr)      (NULL==(_endptr) || 0==(_endptr)[0])
#endif

static inline char *
____os_strlast(const char *s, int len)
{
    return s?((char *)s + len - 1):NULL;
}

static inline char *
__os_strlast(const char *s)
{
    return ____os_strlast(s, os_strlen(s));
}

static inline char *
os_strlast(const char *s, int ch)
{
    char *last = __os_strlast(s);

    return (*last==ch)?last:NULL;
}

#define os_array_search_str(_array, _string, _begin, _end) \
        os_array_search(_array, _string, os_strcmp, _begin, _end)

typedef bool char_is_f(int ch);

static inline bool
os_iscrlf(int ch)
{
    return '\r'==ch || '\n'==ch;
}

static inline bool
os_iswildcard(int ch)
{
    return '*'==ch || '-'==ch || '_'==ch || '.'==ch;
}

static inline bool
os_str_is_wildcard(const char *s, char_is_f *is)
{
    return s \
        && (is?(*is)(s[0]):('*'==s[0]))
        && 0==s[1];
}

static inline bool
os_isblank(int ch)
{
    return ' '==ch || '\t'==ch;
}

static inline bool 
os_char_is(int ch, char_is_f *is)
{
    return is?(*is)(ch):(os_isblank(ch) || os_iscrlf(ch));
}

STATIC INLINE char *
os_str_skip(const char *s, char_is_f *is);

/*
* 将 string 内的 is 替换 为 new
*
* 注意 : string被修改，不可重入
*/
STATIC INLINE char *
os_str_replace(char *s, char_is_f *is, int new);

/*
* 将 str 内的 连续多个满足条件的字符去重(多个消减为1个)
* 
* 注意 : str被修改，不可重入
*/
STATIC INLINE char *
os_str_reduce(char *str, char_is_f *is);

/*
* 消除 str 内满足 is 的 字符
* 
* 注意 : str被修改，不可重入
*/
STATIC INLINE char *
os_str_strim(char *str, char_is_f *is);

/*
* 消除 str 左侧满足 is 的 字符
* 
* 注意 : str被修改，不可重入
*/
STATIC INLINE char *
os_str_lstrim(char *str, char_is_f *is);

/*
* 消除 str 右侧满足 is 的 字符
* 
* 注意 : str被修改，不可重入
*/
STATIC INLINE char *
__os_str_rstrim(char *s, int len, char_is_f *is);

static inline char *
os_str_rstrim(char *s, char_is_f *is)
{
    return __os_str_rstrim(s, os_strlen(s), is);
}

static inline char *
os_str_strim_both(char *s, char_is_f *is)
{
    return os_str_rstrim(s, is), os_str_lstrim(s, is);
}

STATIC INLINE bool
os_str_is_end_by(const char *s, char *end);

static inline bool
__char_is_drop(int ch, char_is_f *is)
{
    if (is) {
        return (*is)(ch);
    } else {
        return ch=='#';
    }
}

/*
* 消除 string 右侧满足 is 的 字符及后面字符
* 
* 注意 : string被修改，不可重入
*/
STATIC INLINE char *
os_str_drop(char *s, char_is_f *is);

static inline bool 
__is_blank_line(const char *line)
{
    return 0==line[0] 
        || ('\n'==line[0] && 0==line[1])
        || ('\r'==line[0] && '\n'==line[1] && 0==line[2]);
}

static inline bool 
__is_notes_line(const char *line, const char *notes)
{
    int len = os_strlen(notes);

    return os_memeq(line, notes, len);
}

static inline bool 
__is_notes_line_deft(const char *line)
{
    return __is_notes_line(line, __notes);
}

STATIC INLINE char *
os_str_next(char *s, char_is_f *is);

STATIC INLINE char *
os_str_next_byifs(char *s, char *ifs);

#ifndef OS_STRING_BKDR
#define OS_STRING_BKDR      31
#endif

typedef uint32 bkdr_t;

static inline bkdr_t
__bkdr_push(bkdr_t a, bkdr_t b)
{
    return a * OS_STRING_BKDR + b;
}

static inline bkdr_t
__bkdr_pop(bkdr_t a, bkdr_t b)
{
    return (a - b) / OS_STRING_BKDR;
}

STATIC INLINE bkdr_t
os_str_BKDR_push(bkdr_t bkdr, const char *s, uint32 *plen);

static inline bkdr_t
os_str_BKDR(const char *s, uint32 *plen)
{
    return os_str_BKDR_push(0, s, plen);
}

STATIC INLINE bkdr_t
os_str_bkdr_push(bkdr_t bkdr, const char *s);

static inline bkdr_t
os_str_bkdr(const char *s)
{
    return os_str_bkdr_push(0, s);
}

STATIC INLINE bkdr_t
os_bin_bkdr_push(bkdr_t bkdr, const void *binary, uint32 len);

static inline bkdr_t
os_bin_bkdr(const void *binary, uint32 len)
{
    return os_bin_bkdr_push(0, binary, len);
}

#include "weos/boot/string.c"
/******************************************************************************/
#endif /* __STRING_H_433874baec1b41d58eef119d11851217__ */
