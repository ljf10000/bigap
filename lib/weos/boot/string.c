/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
DECLARE_ENUM(string_type, STRING_TYPE_ENUM_MAPPER, STRING_T_END);

DECLARE int
os_strcount(const char *s, int ch)
{
    const char *p = s;
    int count = 0;

    while(*p) {
        if (*p++ == ch) {
            count++;
        }
    }

    return count;
}

DECLARE char *
os_strmcpy(char *dst, const char *src, int len)
{
    if (dst && src) {
        os_memcpy(dst, src, len);

        dst[len] = 0;
    }

    return dst;
}

DECLARE uint32 
os_strlcpy(char *dst, const char *src, uint32 size)
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

DECLARE int
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

DECLARE int
os_strncmp(const char *a, const char *b, int len)
{
    if (a) {
        if (b) {
            return ((a==b) || len<=0)?0:strncmp(a, b, len);
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

DECLARE char *
os_str_skip(const char *s, char_is_f *is)
{
    char *p = (char *)s;

    while(*p && os_char_is(*p, is)) {
        p++;
    }

    return p;
}

DECLARE char *
os_str_skip_env(const char *s)
{
    char *begin = (char *)s;
    char *p, *eq;

    while(1) {
        // find '='
        eq = os_strchr(begin, '=');
        if (NULL==eq) {
            return begin;
        }

        // find ' '
        p = os_strchr(eq, ' ');
        if (NULL==p) {
            return begin;
        }

        begin = ++p; // skip ' '
    }
}

DECLARE char *
os_str_replace(char *s, char_is_f *is, int new)
{
    char *p = s;

    while(*p) {
        if (os_char_is(*p, is)) {
            *p++ = new;
        } else {
            p++;
        }
    }

    return p;
}

DECLARE char *
os_str_reduce(char *str, char_is_f *is)
{
    char *p = str; /* 记录指针 */
    char *s = str; /* 扫描指针 */
    bool in_reduce = false; /* reduce 模式 */
    
    while(*s) {
        if (os_char_is(*s, is)) {
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

DECLARE char *
os_str_strim(char *str, char_is_f *is)
{
    char *p = str; /* 记录指针 */
    char *s = str; /* 扫描指针 */

    while(*s) {
        if (os_char_is(*s, is)) {
            s++;
        } else {
            *p++ = *s++;
        }
    }

    return s;
}

DECLARE char *
os_str_lstrim(char *str, char_is_f *is)
{
    char *p = str; /* 记录指针 */
    char *s = str; /* 扫描指针 */

    // begin with is
    if (os_char_is(*p, is)) {
        /* find first no-match is */
        while(*s && os_char_is(*s, is)) {
            s++;
        }

        /* all move to begin */
        while(*s) {
            *p++ = *s++;
        }
        
        *p = 0;
    }
    
    return s;
}

DECLARE char *
__os_str_rstrim(char *s, int len, char_is_f *is)
{
    /* pointer to last char */
    char *p = ____os_strlast(s, len);

    /* scan, from last char to begin */
    while(p>=s && os_char_is(*p, is)) {
        p--;
    }

    /* now, pointer to the right first no-match is */
    *(p+1) = 0;
    
    return s;
}

DECLARE bool
os_str_is_end_by(const char *s, char *end)
{
    uint32 slen = os_strlen(s);
    uint32 elen = os_strlen(end);

    if (slen >= elen) {
        return os_memeq(s + slen - elen, end, elen);
    } else {
        return false;
    }
}

DECLARE char *
os_str_drop(char *s, char_is_f *is)
{
    /* pointer to last char */
    char *p = s;

    /* scan, from last char to begin */
    while(*p && false==__char_is_drop(*p, is)) {
        p++;
    }

    *p = 0;
    
    return s;
}

DECLARE char *
os_str_next(char *s, char_is_f *is)
{
    char *p = s;

    if (NULL==s) {
        return NULL;
    }
    
    while(*p && false==os_char_is(*p, is)) {
        p++;
    }
    
    if (0==*p) {
        return NULL;
    } else {
        *p++ = 0;

        return p;
    }
}

DECLARE char *
os_str_next_byifs(char *s, char *ifs)
{
    bool is_ifs(int ch)
    {
        return NULL!=os_strchr(ifs, ch);
    }
    
    return os_str_next(s, is_ifs);
}

DECLARE bkdr_t
os_str_BKDR_push(bkdr_t bkdr, const char *s, uint32 *plen)
{
    if (s) {
        const char *p = s;
        uint32 len = 0;
        
        while(*p) {
            len++;
            bkdr = __bkdr_push(bkdr, *p++);
        }

        if (plen) {
            *plen += len;
        }
    }
    
    return bkdr;
}

DECLARE bkdr_t
os_str_bkdr_push(bkdr_t bkdr, const char *s)
{
    if (s) {
        const char *p = s;

        while(*p) {
            bkdr = __bkdr_push(bkdr, *p++);
        }
    }
    
    return bkdr;
}

DECLARE bkdr_t
os_bin_bkdr_push(bkdr_t bkdr, const void *binary, uint32 len)
{
    if (binary && len) {
        int i;
        
        for (i=0; i<len; i++) {
            bkdr = __bkdr_push(bkdr, *((byte *)binary + i));
        }
    }
    
    return bkdr;
}
/******************************************************************************/
