#ifndef __STRING2_H_795ed673d7eb48a4b2d3cc401be937d5__
#define __STRING2_H_795ed673d7eb48a4b2d3cc401be937d5__
/******************************************************************************/
#if 1
#define __XLIST_STRING_TYPE(_)          \
    _(STRING_T_GLOBAL,  0, "global"),   \
    _(STRING_T_CONST,   1, "const"),    \
    _(STRING_T_HEAP,    3, "heap"),     \
    /* end */

static inline bool is_good_string_type(int type);
static inline char *string_type_string(int type);
static inline int string_type_idx(char *type_string);
DECLARE_ENUM(string_type, __XLIST_STRING_TYPE, STRING_T_END);

#define STRING_T_GLOBAL     STRING_T_GLOBAL
#define STRING_T_CONST      STRING_T_CONST
#define STRING_T_HEAP       STRING_T_HEAP
#define STRING_T_END        STRING_T_END
#endif

typedef struct {
#if __BYTE_ORDER == __BIG_ENDIAN
    uint32 tmp:8;     /* all */
    uint32 top:1;     /* all, is top ??? */
    uint32 con:1;     /* all, is const ??? */
    uint32 dyn:1;     /* all, is dynamic ??? */
    uint32 eoz:1;     /* all: end of zero ??? */
    uint32 eob:1;     /* root: end of block ??? */
    uint32 clr:1;     /* root: need to clear ??? */
    uint32 bin:1;     /* root, is binary */
    uint32 rsv:1;
    uint32 ref:16;    /* root */
#elif __BYTE_ORDER == __LITTLE_ENDIAN
    uint32 ref:16;
    uint32 rsv:1;
    uint32 bin:1;
    uint32 clr:1;
    uint32 eob:1;
    uint32 eoz:1;
    uint32 dyn:1;
    uint32 con:1;
    uint32 top:1;
    uint32 tmp:8;
#endif
} 
string_opt_t;

#define __STRING_OPT_INITER(_bin, _con, _dyn, _eoz, _eob) { \
    .tmp    = 0,    \
    .top    = true, \
    .con    = _con, \
    .dyn    = _dyn, \
    .eoz    = _eoz, \
    .eob    = _eob, \
    .clr    = (!(_con)) && (_dyn), \
    .bin    = _bin, \
    .rsv    = 0,    \
    .ref    = 1,    \
}

#define STRING_OPT_INITER(_bin, _con, _dyn, _eoz)   __STRING_OPT_INITER(_bin, _con, _dyn, _eoz, false)
#define STRING_OPT_RO_INITER(_bin, _dyn, _eoz)      STRING_OPT_INITER(_bin, true,  _dyn,  _eoz)
#define STRING_OPT_RW_INITER(_bin, _dyn, _eoz)      STRING_OPT_INITER(_bin, false, _dyn,  _eoz)
#define STRING_OPT_DYNAMIC_INITER(_bin, _con, _eoz) STRING_OPT_INITER(_bin, _con,  true,  _eoz)
#define STRING_OPT_STATIC_INITER(_bin, _con, _eoz)  STRING_OPT_INITER(_bin, _con,  false, _eoz)
#define STRING_OPT_RO_DYNAMIC_INITER(_bin, _eoz)    STRING_OPT_INITER(_bin, true,  true,  _eoz)
#define STRING_OPT_RW_DYNAMIC_INITER(_bin, _eoz)    STRING_OPT_INITER(_bin, false, true,  _eoz)
#define STRING_OPT_RO_STATIC_INITER(_bin, _eoz)     STRING_OPT_INITER(_bin, true,  false, _eoz)
#define STRING_OPT_RW_STATIC_INITER(_bin, _eoz)     STRING_OPT_INITER(_bin, false, false, _eoz)

#ifndef USE_STRING_BLOCK
#define USE_STRING_BLOCK    0
#endif

typedef struct string_s {
    union {
        string_opt_t o;
        uint32 v;
    } opt;

#if USE_STRING_BLOCK
    uint32 size;      /* root */
#endif
    uint32 begin;     /* all, begin to string */
    uint32 len;       /* all */

    union {
        char *string;           /* root */
        char block[0];          /* root */
        struct string_s *root;  /* current */
    } body;
} 
string_t;

#define sopt_tmp    opt.o.tmp
#define sopt_top    opt.o.top
#define sopt_con    opt.o.con
#define sopt_dyn    opt.o.dyn
#define sopt_eoz    opt.o.eoz
#define sopt_eob    opt.o.eob
#define sopt_clr    opt.o.clr
#define sopt_bin    opt.o.bin
#define sopt_rsv    opt.o.rsv
#define sopt_ref    opt.o.ref
#define sopt_val    opt.o.v

#define STRING_ZERO     { .body = { .string = NULL } }

#define __STRING_P_INITER(_p, _begin, _len) \
    .begin  = _begin,                       \
    .len    = _len,                         \
    .body = {                               \
        .string = (char *)(_p) + (_begin),  \
    }   /* end */

#define __STRING_INITER(_p, _begin, _len, _bin, _con, _dyn, _eoz) { \
    .opt = { .o = __STRING_OPT_INITER(_bin, _con, _dyn, _eoz, false)}, \
    __STRING_P_INITER(_p, _begin, _len), \
}   /* end */

#define __STRING_S_INITER(_p, _begin, _len, _con, _dyn, _eoz)   __STRING_INITER(_p, _begin, _len, false, _con, _dyn, _eoz)
#define __STRING_S_RO_INITER(_p, _begin, _len, _dyn, _eoz)      __STRING_S_INITER(_p, _begin, _len, true, _dyn, _eoz)
#define __STRING_S_RW_INITER(_p, _begin, _len, _dyn, _eoz)      __STRING_S_INITER(_p, _begin, _len, false, _dyn, _eoz)
#define __STRING_S_DYNAMIC_INITER(_p, _begin, _len, _con, _eoz) __STRING_S_INITER(_p, _begin, _len, _con, true, _eoz)
#define __STRING_S_STATIC_INITER(_p, _begin, _len, _con, _eoz)  __STRING_S_INITER(_p, _begin, _len, _con, false, _eoz)
#define __STRING_S_RO_DYNAMIC_INITER(_p, _begin, _len, _eoz)    __STRING_S_INITER(_p, _begin, _len, true, true, _eoz)
#define __STRING_S_RW_DYNAMIC_INITER(_p, _begin, _len, _eoz)    __STRING_S_INITER(_p, _begin, _len, false, true, _eoz)
#define __STRING_S_RO_STATIC_INITER(_p, _begin, _len, _eoz)     __STRING_S_INITER(_p, _begin, _len, true, false, _eoz)
#define __STRING_S_RW_STATIC_INITER(_p, _begin, _len, _eoz)     __STRING_S_INITER(_p, _begin, _len, true, false, _eoz)

#define __STRING_B_INITER(_p, _begin, _len, _con, _dyn, _eoz)   __STRING_INITER(_p, _begin, _len, true, _con, _dyn, _eoz)
#define __STRING_B_RO_INITER(_p, _begin, _len, _dyn, _eoz)      __STRING_B_INITER(_p, _begin, _len, true, _dyn, _eoz)
#define __STRING_B_RW_INITER(_p, _begin, _len, _dyn, _eoz)      __STRING_B_INITER(_p, _begin, _len, false, _dyn, _eoz)
#define __STRING_B_DYNAMIC_INITER(_p, _begin, _len, _con, _eoz) __STRING_B_INITER(_p, _begin, _len, _con, true, _eoz)
#define __STRING_B_STATIC_INITER(_p, _begin, _len, _con, _eoz)  __STRING_B_INITER(_p, _begin, _len, _con, false, _eoz)
#define __STRING_B_RO_DYNAMIC_INITER(_p, _begin, _len, _eoz)    __STRING_B_INITER(_p, _begin, _len, true, true, _eoz)
#define __STRING_B_RW_DYNAMIC_INITER(_p, _begin, _len, _eoz)    __STRING_B_INITER(_p, _begin, _len, false, true, _eoz)
#define __STRING_B_RO_STATIC_INITER(_p, _begin, _len, _eoz)     __STRING_B_INITER(_p, _begin, _len, true, false, _eoz)
#define __STRING_B_RW_STATIC_INITER(_p, _begin, _len, _eoz)     __STRING_B_INITER(_p, _begin, _len, true, false, _eoz)

#define STRING_S_RO_INITER(_p, _begin, _len, _dyn)      __STRING_S_RO_INITER(_p, _begin, _len, _dyn, true)
#define STRING_S_RW_INITER(_p, _begin, _len, _dyn)      __STRING_S_RW_INITER(_p, _begin, _len, _dyn, true)
#define STRING_S_DYNAMIC_INITER(_p, _begin, _len, _con) __STRING_S_DYNAMIC_INITER(_p, _begin, _len, _con, true)
#define STRING_S_STATIC_INITER(_p, _begin, _len, _con)  __STRING_S_STATIC_INITER(_p, _begin, _len, _con, true)
#define STRING_S_RO_DYNAMIC_INITER(_p, _begin, _len)    __STRING_S_RO_DYNAMIC_INITER(_p, _begin, _len, true)
#define STRING_S_RW_DYNAMIC_INITER(_p, _begin, _len)    __STRING_S_RW_DYNAMIC_INITER(_p, _begin, _len, true)
#define STRING_S_RO_STATIC_INITER(_p, _begin, _len)     __STRING_S_RO_STATIC_INITER(_p, _begin, _len, true)
#define STRING_S_RW_STATIC_INITER(_p, _begin, _len)     __STRING_S_RW_STATIC_INITER(_p, _begin, _len, true)

#define STRING_B_RO_INITER(_p, _begin, _len, _dyn)      __STRING_B_RO_INITER(_p, _begin, _len, _dyn, true)
#define STRING_B_RW_INITER(_p, _begin, _len, _dyn)      __STRING_B_RW_INITER(_p, _begin, _len, _dyn, true)
#define STRING_B_DYNAMIC_INITER(_p, _begin, _len, _con) __STRING_B_DYNAMIC_INITER(_p, _begin, _len, _con, true)
#define STRING_B_STATIC_INITER(_p, _begin, _len, _con)  __STRING_B_STATIC_INITER(_p, _begin, _len, _con, true)
#define STRING_B_RO_DYNAMIC_INITER(_p, _begin, _len)    __STRING_B_RO_DYNAMIC_INITER(_p, _begin, _len, true)
#define STRING_B_RW_DYNAMIC_INITER(_p, _begin, _len)    __STRING_B_RW_DYNAMIC_INITER(_p, _begin, _len, true)
#define STRING_B_RO_STATIC_INITER(_p, _begin, _len)     __STRING_B_RO_STATIC_INITER(_p, _begin, _len, true)
#define STRING_B_RW_STATIC_INITER(_p, _begin, _len)     __STRING_B_RW_STATIC_INITER(_p, _begin, _len, true)

static inline string_t *
__string_root(string_t *s)
{
    return s->sopt_top?s:s->body.root;
}

static inline string_t *
string_root(string_t *s)
{
    return s?__string_root(s):NULL;
}

static inline char *
__string_body(string_t *s)
{
    string_t *root = __string_root(s);

    if (NULL==root) {
        return NULL;
    }
#if USE_STRING_BLOCK
    else if (s->sopt_eob) {
        return root->body.block;
    }
#endif
    else {
        return root->body.string;
    }
}

static inline char *
string_body(string_t *s)
{
    return s?__string_body(s):NULL;
}

static inline char *
__string_value(string_t *s)
{
    char *body = __string_body(s);
    if (body) {
        return body + s->begin;
    } else {
        return NULL;
    }
}

static inline char *
string_value(string_t *s)
{
    return s?__string_value(s):NULL;
}

static inline char *
__string_eoz(string_t *s)
{
    return __string_value(s) + s->len;
}

static inline char *
string_eoz(string_t *s)
{
    return s?__string_eoz(s):NULL;
}

static inline bool
__is_good_string(string_t *s)
{
#if USE_STRING_BLOCK
    if (s->sopt_eob) {
        return true;
    }
#endif

    return is_good_str(__string_value(s));
}

static inline bool
is_good_string(string_t *s)
{
    return s?__is_good_string(s):false;
}

static inline bool
is_string_can_slice(string_t *s, uint32 begin, uint32 len)
{
    uint32 max = begin + len - 1;

    return begin <= max && max <= s->len;
}

#if USE_STRING_BLOCK
static inline string_t *
__string_block(const char *raw, uint32 len, bool bin)
{
    uint32 size = OS_ALIGN(1 + len, OS_ALIGN_SIZE);
    string_t *s = (string_t *)os_malloc(size + sizeof(string_t));
    if (NULL==s) {
        return NULL;
    }

    string_opt_t opt = __STRING_OPT_INITER(bin, false, true, true, true);
    
    s->opt.o    = opt;
    s->size     = size;
    s->begin    = 0;
    s->len      = len;

    os_strmcpy(string_value(s), raw, len);
    
    return s;
}

static inline string_t *
string_block(const char *raw)
{
    return __string_block(raw, os_strlen(raw), false);
}

static inline string_t *
binary_block(const char *raw, uint32 len)
{
    return __string_block(raw, len, true);
}
#endif

static inline string_t
__string_new(const char *raw, uint32 len, bool bin, bool con)
{
    uint32 size = OS_ALIGN(1 + len, OS_ALIGN_SIZE);
    
    char *p = (char *)os_malloc(size);
    if (p) {
        os_strmcpy(p, raw, len);
        
        string_t s = __STRING_S_INITER(p, 0, len, con, true, true);
        
        return s;
    } else {
        string_t zero = STRING_ZERO;

        return zero;
    }
}

static inline string_t
string_new(const char *raw, bool con)
{
    return __string_new(raw, os_strlen(raw), false, con);
}

static inline string_t
binary_new(const char *raw, uint32 len, bool con)
{
    return __string_new(raw, len, true, con);
}

static inline string_t *
string_get(string_t *s)
{
    string_t *root = string_root(s);
    if (root) {
        root->sopt_ref++;
    }

    return s;
}

static inline bool
__string_put(string_t *s)
{
    string_t *root = string_root(s);
    if (root && root->sopt_ref) {
        root->sopt_ref--;
        if (0==root->sopt_ref && root->sopt_clr) {
#if USE_STRING_BLOCK
            if (root->sopt_eob) {
                free(root);

                return true;
            } else {
#endif
                free(root->body.string);
                
                root->body.string = NULL;
                root->len   = 0;
                root->begin = 0;
#if USE_STRING_BLOCK
                root->size  = 0;
            }
#endif
        }
    }

    return false;
}

#define string_put(_s)  do{ \
    if (__string_put(_s)) { \
        _s = NULL;          \
    }                       \
}while(0)

static inline string_t
string_slice(string_t *s, uint32 begin, uint32 len)
{
    string_t new = STRING_ZERO;
    
    if (is_string_can_slice(s, begin, len)) {
        new = *s;
        new.sopt_top    = false;
        
        new.len         = len;
        new.begin       = s->begin + begin;
        new.body.root   = string_get(s);
    }
    
    return new;
}

/*
* unsafe
*/
static inline char *
string_steal(string_t *s)
{
    char *v, *eoz;

    if (NULL!=(v=string_value(s)) &&
        NULL!=(eoz=string_eoz(s)) &&
        *eoz) {
        s->sopt_tmp = *eoz;
        *eoz = 0;
    }

    return v;
}

/*
* unsafe
*/
static inline void
string_restore(string_t *s)
{
    char *v, *eoz;

    if (NULL!=(v=string_value(s)) &&
        NULL!=(eoz=string_eoz(s)) &&
        0==*eoz) {
        *eoz = s->sopt_tmp;
        s->sopt_tmp = 0;
    }
}

static inline int
string_cmp(string_t *a, string_t *b)
{
    if (a) {
        if (b) {
            char *va = string_value(a);
            char *vb = string_value(b);
            if (va==vb) {
                return 0;
            }

            uint32 min = OS_MIN(a->len, b->len);
            int cmp = os_memcmp(va, vb, min);
            if (a->len==b->len) {
                return cmp;
            }
            else if (a->len < b->len) {
                return cmp?cmp:-1;
            }
            else if (a->len > b->len) {
                return cmp?cmp:1;
            }
        }
        else {
            return 1;
        }
    }
    else {
        if (b) {
            return -1;
        } else {
            return 0;
        }
    }
}

static inline bool
string_eq(string_t *a, string_t *b)
{
    if (a) {
        if (b) {
            if (a->len==b->len) {
                char *va = string_value(a);
                char *vb = string_value(b);

                return va==vb || 0==memcmp(va, vb, a->len);
            } else {
                return false;
            }
        } else {
            return false;
        }
    } else {
        if (b) {
            return false;
        } else {
            return true;
        }
    }
}


/******************************************************************************/
#endif /* __STRING2_H_795ed673d7eb48a4b2d3cc401be937d5__ */
