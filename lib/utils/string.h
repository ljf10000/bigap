#ifndef __STRING_H_433874baec1b41d58eef119d11851217__
#define __STRING_H_433874baec1b41d58eef119d11851217__
/******************************************************************************/
#ifndef __empty
#define __empty         ""
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

#ifndef __space2
#define __space2        __space __space
#endif

#ifndef __space4
#define __space4        __space2 __space2
#endif

#ifndef __blank
#define __blank         " "
#endif

#ifndef __blanks
#define __blanks        "\t \r\n"
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
is_good_str(char *s)
{
    return s && s[0];
}

static inline char *
os_safe_str(char *s)
{
    return s?s:__nil;
}

static inline int
os_strlen(const char *s)
{
    return s?strlen(s):0;
}

static inline char *
os_strcpy(char *dst, const char *src)
{
    return (dst && src)?strcpy(dst, src):dst;
}

/* len is src's length */
static inline char *
os_strmcpy(char *dst, const char *src, int len)
{
    if (dst && src) {
        os_memcpy(dst, src, len);

        dst[len] = 0;
    }

    return dst;
}

/*
* no use strncpy(is unsafe)
*/
static inline uint32_t 
os_strlcpy(char *dst, const char *src, uint32_t size)
{
    char *d = (char *)dst;
    char *s = (char *)src;
    int n, len = 0;
    
    os_assert(NULL!=dst);
    os_assert(NULL!=src);

    if (size > 0) {
        n = size - 1;

        while(*s && n) {
            *d++ = *s++;
            n--;
        }

        len = size - 1 - n;
    }

    dst[len] = 0;
    
    return len;
}

#ifndef os_strdcpy
#define os_strdcpy(_dst, _src)      os_strlcpy(_dst, _src, sizeof(_dst))
#endif

#ifndef os_strscpy
#define os_strscpy(_dst, _src)      os_strlcpy(_dst, _src, sizeof(_src))
#endif

static inline int
os_strcmp(const char *a, const char *b)
{
    if (a) {
        if (b) {
            return (a==b)?0:strcmp(a, b);
        } else {
            return 1;
        }
    } else {
        if (b) {
            return -1;
        } else {
            return 0;
        }
    }
}

static inline int
os_strncmp(const char *a, const char *b, int len)
{
    if (a) {
        if (b) {
            return (a==b)?0:strncmp(a, b, len);
        } else {
            return 1;
        }
    } else {
        if (b) {
            return -1;
        } else {
            return 0;
        }
    }
}

/*
* use a's size
*/
#ifndef os_stracmp
#define os_stracmp(_a, _b)          os_strncmp(_a, _b, sizeof(_a))
#endif

/*
* use b's size
*/
#ifndef os_strbcmp
#define os_strbcmp(_a, _b)          os_strncmp(_a, _b, sizeof(_b))
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
#else
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

#ifndef os_strton_is_good
#define os_strton_is_good(_endptr)          (NULL==(_endptr) || 0==(_endptr)[0])
#endif

static inline char *
__os_strlast(char *s)
{
    return s?(s + os_strlen(s) - 1):NULL;
}

static inline char *
os_strlast(char *s, int ch)
{
    char *last = __os_strlast(s);

    return (*last==ch)?last:NULL;
}

#define os_getstringarrayidx(_array, _string, _begin, _end) \
        __os_getobjarrayidx(_array, _string, os_strcmp, _begin, _end)

typedef bool char_is_f(int ch);

static inline bool 
__char_is(int ch, char_is_f *IS)
{
    if (IS) {
        return (*IS)(ch);
    } else {
#ifdef CHAR_IS_F
        return CHAR_IS_F(ch);
#else
        return NULL!=strchr(__blanks, ch);
#endif
    }
}

static inline char *
os_str_skip(char *s, char_is_f *IS)
{
    char *p = s;

    while(*p && __char_is(*p, IS)) {
        p++;
    }

    return p;
}

/*
* 将 string 内的 old 替换 为 new
*
* 注意 : string被修改，不可重入
*/
static inline char *
os_str_replace(char *s, char_is_f *IS_OLD, int new)
{
    char *p = s;

    while(*p) {
        if (__char_is(*p, IS_OLD)) {
            *p++ = new;
        } else {
            p++;
        }
    }

    return p;
}

/*
* 将 str 内的 连续多个满足条件的字符去重(多个消减为1个)
* 
* 注意 : str被修改，不可重入
*/
static inline char *
os_str_reduce(char *str, char_is_f *IS)
{
    char *p = str; /* 记录指针 */
    char *s = str; /* 扫描指针 */
    bool in_reduce = false; /* reduce 模式 */
    
    while(*s) {
        if (__char_is(*s, IS)) {
            /*
            * 扫描到 去重字符, 则记录之
            *
            * (1) 进入 reduce 模式
            * (2) p 不动，保证只记录一次 去重字符，即达到 reduce 效果
            */
            in_reduce = true;
            
            *p = *s++;

        } else {
            /*
            * 扫描到正常数据(非 去重字符)
            *
            * 如果当前是 reduce 模式
            *   (1) p 走一步，完成 reduce，为记录正常数据做准备
            *   (2) 退出 reduce 模式
            */
            if (in_reduce) {
                p++;
                
                in_reduce = false;
            }

            /*
            * 记录正常数据
            */
            *p++ = *s++;
        }
    }

    *p = 0; /* 丢弃尾部 冗余 */

    return s;
}

/*
* 消除 str 内满足 IS 的 字符
* 
* 注意 : str被修改，不可重入
*/
static inline char *
os_str_strim(char *str, char_is_f *IS)
{
    char *p = str; /* 记录指针 */
    char *s = str; /* 扫描指针 */

    while(*s) {
        if (__char_is(*s, IS)) {
            s++;
        } else {
            *p++ = *s++;
        }
    }

    return s;
}

/*
* 消除 str 左侧满足 IS 的 字符
* 
* 注意 : str被修改，不可重入
*/
static inline char *
os_str_lstrim(char *str, char_is_f *IS)
{
    char *p = str; /* 记录指针 */
    char *s = str; /* 扫描指针 */

    /* find first no-match IS */
    while(*s && __char_is(*s, IS)) {
        s++;
    }

    /* all move to begin */
    while(*s) {
        *p++ = *s++;
    }
    
    *p = 0;
    
    return s;
}

/*
* 消除 str 右侧满足 IS 的 字符
* 
* 注意 : str被修改，不可重入
*/
static inline char *
os_str_rstrim(char *s, char_is_f *IS)
{
    /* pointer to last char */
    char *p = s + os_strlen(s) - 1;

    /* scan, from last char to begin */
    while(p>=s && __char_is(*p, IS)) {
        p--;
    }

    /* now, pointer to the right first no-match IS */
    *(p+1) = 0;
    
    return s;
}

static inline char *
os_str_strim_both(char *s, char_is_f *IS)
{
    return os_str_rstrim(s, IS), os_str_lstrim(s, IS);
}

static inline bool
__char_is_drop(int ch, char_is_f *IS)
{
    if (IS) {
        return (*IS)(ch);
    } else {
#ifdef CHAR_IS_DROP
        return CHAR_IS_DROP(ch)
#else
        return ch=='#';
#endif
    }
}

/*
* 消除 string 右侧满足 IS 的 字符及后面字符
* 
* 注意 : string被修改，不可重入
*/
static inline char *
os_str_drop(char *s, char_is_f *IS)
{
    /* pointer to last char */
    char *p = s;

    /* scan, from last char to begin */
    while(*p && false==__char_is_drop(*p, IS)) {
        p++;
    }

    *p = 0;
    
    return s;
}

static inline bool 
__is_blank_line(char *line)
{
    return 0==line[0] || '\n'==line[0] || ('\r'==line[0] && '\n'==line[1]);
}

static inline bool 
__is_notes_line(char *line, char *notes)
{
    int len = os_strlen(notes);

    return os_memeq(line, notes, len);
}

static inline bool 
__is_notes_line_deft(char *line)
{
    return __is_notes_line(line, __notes);
}

static inline char *
os_str_next(char *s, char_is_f *IS)
{
    char *p = s;

    if (NULL==s) {
        return NULL;
    }
    
    while(*p && false==__char_is(*p, IS)) {
        p++;
    }
    
    if (0==*p) {
        return NULL;
    } else {
        *p++ = 0;

        return p;
    }
}

static inline char *
os_str_next_byifs(char *s, int ifs)
{
    bool is_ifs(int ch)
    {
        return ch==ifs;
    }
    
    return os_str_next(s, is_ifs);
}

#ifndef OS_STRING_BKDR
#define OS_STRING_BKDR      31
#endif

typedef uint32_t bkdr_t;

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

static inline bkdr_t
os_str_BKDR(const char *s, uint32_t *plen)
{
    bkdr_t bkdr = 0;
    
    if (s) {
        const char *p = s;
        uint32_t len = 0;
        
        while(*p) {
            len++;
            bkdr = __bkdr_push(bkdr, *p++);
        }

        if (plen) {
            *plen = len;
        }
    }
    
    return bkdr;
}

static inline bkdr_t
os_str_bkdr(const char *s)
{
    bkdr_t bkdr = 0;

    if (s) {
        const char *p = s;

        while(*p) {
            bkdr = __bkdr_push(bkdr, *p++);
        }
    }
    
    return bkdr;
}

static inline bkdr_t
os_bin_bkdr(const byte *binary, uint32_t len)
{
    bkdr_t bkdr = 0;

    if (binary) {
        int i;
        
        for (i=0; i<len; i++) {
            bkdr = __bkdr_push(bkdr, *(binary + i));
        }
    }
    
    return bkdr;
}
/******************************************************************************/
#endif /* __STRING_H_433874baec1b41d58eef119d11851217__ */
