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
