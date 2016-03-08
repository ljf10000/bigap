#ifndef __STRING2_H_795ed673d7eb48a4b2d3cc401be937d5__
#define __STRING2_H_795ed673d7eb48a4b2d3cc401be937d5__
/******************************************************************************/
typedef struct {
#if __BYTE_ORDER == __BIG_ENDIAN
    uint32_t tmp:8;     /* all */
    uint32_t top:1;     /* all, is top ??? */
    uint32_t con:1;     /* all, is const ??? */
    uint32_t dyn:1;     /* all, is dynamic ??? */
    uint32_t stk:1;     /* all, is in stack ??? */
    uint32_t eoz:1;     /* all: end of zero ??? */
    uint32_t eob:1;     /* root: end of block ??? */
    uint32_t clr:1;     /* root: need to clear ??? */
    uint32_t rsv:1;
    uint32_t ref:16;    /* root */
#elif __BYTE_ORDER == __LITTLE_ENDIAN
    uint32_t ref:16;
    uint32_t rsv:1;
    uint32_t clr:1;
    uint32_t eob:1;
    uint32_t eoz:1;
    uint32_t stk:1;
    uint32_t dyn:1;
    uint32_t con:1;
    uint32_t top:1;
    uint32_t tmp:8;
#endif
} 
string_opt_t;

#define __STRING_OPT_INITER(_con, _dyn, _stk, _eob, _eoz)   { \
    .tmp    = 0,    \
    .top    = true, \
    .con    = _con, \
    .dyn    = _dyn, \
    .stk    = _stk, \
    .eoz    = _eoz, \
    .eob    = _eob, \
    .clr    = (_con)?false:((_dyn)?((_stk)?false:true):false), \
    .rsv    = 0,    \
    .ref    = 1,    \
}

/*
* global:
*   con: false
*   dyn: false
*   stk: false
*   clr: false
*/
#define __STRING_OPT_GLOBAL_INITER(_eoz)  __STRING_OPT_INITER(false, false, false, false, _eoz)
/*
* const:
*   con: true
*   dyn: false
*   stk: false
*   clr: false
*/
#define __STRING_OPT_CONST_INITER(_eoz)   __STRING_OPT_INITER(true,  false, false, false, _eoz)
/*
* stack:
*   con: false
*   dyn: true
*   stk: true
*   clr: false
*/
#define __STRING_OPT_STACK_INITER(_eoz)   __STRING_OPT_INITER(false, true,  true,  false, _eoz)
/*
* heap:
*   con: false
*   dyn: true
*   stk: false
*   clr: true
*/
#define __STRING_OPT_HEAP_INITER(_eoz)    __STRING_OPT_INITER(false, true,  false, false, _eoz)

#ifndef USE_STRING_BLOCK
#define USE_STRING_BLOCK    0
#endif

typedef struct string_s {
    union {
        string_opt_t o;
        uint32_t v;
    } opt;

#if USE_STRING_BLOCK
    uint32_t size;      /* root */
#endif
    uint32_t begin;     /* all, begin to string */
    uint32_t len;       /* all */

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
#define sopt_stk    opt.o.stk
#define sopt_eoz    opt.o.eoz
#define sopt_eob    opt.o.eob
#define sopt_clr    opt.o.clr
#define sopt_ref    opt.o.ref
#define sopt_rsv    opt.o.rsv
#define sopt_val    opt.o.v






#define STRING_ZERO     { .body = { .string = NULL } }

#define __STRING_P_INITER(_p, _begin, _len) \
    .begin  = _begin,                       \
    .len    = _len,                         \
    .body = {                               \
        .string = (char *)(_p) + (_begin),  \
    },                                      \
    /* end */

#if 1 /* root string */
#define __STRING_S_GLOBAL_INITER(_p, _begin, _len)  {   \
    .opt = { .o = __STRING_OPT_GLOBAL_INITER(true)},    \
    __STRING_P_INITER(_p, _begin, _len),                \
}   /* end */

#define __STRING_S_CONST_INITER(_p, _begin, _len)   {   \
    .opt = { .o = __STRING_OPT_CONST_INITER(true)},     \
    __STRING_P_INITER(_p, _begin, _len),                \
}   /* end */

#define __STRING_S_STACK_INITER(_p, _begin, _len)   {   \
    .opt = { .o = __STRING_OPT_STACK_INITER(true)},     \
    __STRING_P_INITER(_p, _begin, _len),                \
}   /* end */

#define __STRING_S_HEAP_INITER(_p, _begin, _len)    {   \
    .opt = { .o = __STRING_OPT_HEAP_INITER(true)},      \
    __STRING_P_INITER(_p, _begin, _len),                \
}   /* end */
#endif

#if 1 /* root binary */
#define __STRING_B_GLOBAL_INITER(_p, _begin, _len)  {   \
    .opt = { .o = __STRING_OPT_GLOBAL_INITER(false)},   \
    __STRING_P_INITER(_p, _begin, _len),                \
}   /* end */

#define __STRING_B_CONST_INITER(_p, _begin, _len)   {   \
    .opt = { .o = __STRING_OPT_CONST_INITER(false)},    \
    __STRING_P_INITER(_p, _begin, _len),                \
}   /* end */

#define __STRING_B_STACK_INITER(_p, _begin, _len)   {   \
    .opt = { .o = __STRING_OPT_STACK_INITER(false)},    \
    __STRING_P_INITER(_p, _begin, _len),                \
}   /* end */

#define __STRING_B_HEAP_INITER(_p, _begin, _len)    {   \
    .opt = { .o = __STRING_OPT_HEAP_INITER(false)},     \
    __STRING_P_INITER(_p, _begin, _len),                \
}   /* end */
#endif

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
is_string_can_slice(string_t *s, uint32_t begin, uint32_t len)
{
    uint32_t max = begin + len - 1;

    return begin <= max && max <= s->len;
}

#if USE_STRING_BLOCK
static inline string_t *
string_block(char *raw, uint32_t len)
{
    uint32_t need = os_min(os_strlen(raw), len);
    uint32_t size = OS_ALIGN(1 + OS_MIN(len, need), OS_ALIGN_SIZE);

    string_t *s = (string_t *)os_malloc(size + sizeof(string_t));
    if (NULL==s) {
        return NULL;
    }

    string_opt_t opt = __STRING_OPT_INITER(false, true,  false, true, true);
    
    s->opt.o    = opt;
    s->size     = size;
    s->begin    = 0;
    s->len      = need;

    os_strmcpy(string_value(s), raw, need);
    
    return s;
}
#endif

static inline string_t
string_new(const char *raw, uint32_t len)
{
    uint32_t need = os_strlen(raw); 
             need = OS_MIN(need, len);
    uint32_t size = OS_ALIGN(1 + need, OS_ALIGN_SIZE);
    
    char *p = (char *)os_malloc(size);
    if (p) {
        os_strmcpy(p, raw, need);
        
        //string_t s = __STRING_S_HEAP_INITER(p, 0, need);
        string_t s = {
            .opt = { .o = __STRING_OPT_HEAP_INITER(true)},
            __STRING_P_INITER(p, 0, need),
        };
        
        return s;
    } else {
        string_t zero = STRING_ZERO;

        return zero;
    }
}

static inline string_t
string_const(const char *p)
{
    string_t new = __STRING_S_CONST_INITER(p, 0, os_strlen(p));

    return new;
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
string_slice(string_t *s, uint32_t begin, uint32_t len)
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

            uint32_t min = OS_MIN(a->len, b->len);
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
