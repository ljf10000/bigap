#ifndef __STRING2_H_795ed673d7eb48a4b2d3cc401be937d5__
#define __STRING2_H_795ed673d7eb48a4b2d3cc401be937d5__
/******************************************************************************/
struct string_s;

typedef struct {
    uint32_t tmp:8;     /* all */
    uint32_t top:1;     /* all */
    uint32_t con:1;     /* all, const */
    uint32_t dyn:1;     /* all, dynamic */
    uint32_t stk:1;     /* all, in stack */
    uint32_t eoz:1;     /* all: end of zero */
    uint32_t eob:1;     /* root: end of block */
    uint32_t clr:1;     /* root: need to clear */
    uint32_t rsv:1;
    uint32_t ref:16;    /* root */
} string_opt_t;

#define __STRING_OPT_INITER(_con, _dyn, _stk, _eoz, _clr) { \
    .tmp    = 0,    \
    .top    = true, \
    .con    = _con, \
    .dyn    = _dyn, \
    .stk    = _stk, \
    .eoz    = _eoz, \
    .eob    = false,\
    .clr    = _clr, \
    .rsv    = 0,    \
    .ref    = 1,    \
}

typedef struct string_s {
    union {
        string_opt_t o;
        uint32_t v;
    } opt;
    
    uint32_t begin;     /* all: begin to string */
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

#define STRING_ZERO { .body = { .string = NULL } }

#define __STRING_S_INITER(_s, _begin, _len, _con, _dyn, _stk, _eoz, _clr) { \
    .opt = {                \
        .o = __STRING_OPT_INITER(_con, _dyn, _stk, _eoz, _clr), \
    },                      \
    .begin  = (uint32_t)_begin, \
    .len    = (uint32_t)_len, \
    .body = {               \
        .string = (char *)_s + (uint32_t)_begin, \
    },                      \
}

#define __STRING_S_CONST_INITER(_s, _begin, _len) \
    __STRING_S_INITER(_s, _begin, _len, true, false, false, true, false)

#define STRING_S_CONST_INITER(_s) \
    __STRING_S_CONST_INITER(_s, 0, sizeof(_s) - 1)

#define __STRING_B_INITER(_b, _begin, _len, _con, _dyn, _stk, _clr) \
    __STRING_S_INITER(_b, _begin, _len, _con, _dyn, _stk, false, _clr)

#define __STRING_B_CONST_INITER(_b, _begin, _len) \
    __STRING_B_INITER(_b, _begin, _len, true, false, false, false)

#define STRING_B_CONST_INITER(_b) \
    __STRING_B_CONST_INITER(_b, 0, sizeof(_s))

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

static inline bool
is_good_string(string_t *s)
{
    if (s && s->len) {
        if (false==s->sopt_top) {
            string_t *root = s->body.root;
            
            return root && root->sopt_top && root->body.string && root->len;
        } else {
            return NULL!=s->body.string;
        }
    } else {
        return false;
    }
}

static inline char *
string_string(string_t *s)
{
    if (is_good_string(s)) {
        return string_root(s)->body.string + s->begin;
    } else {
        return NULL;
    }
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

static inline void
string_put(string_t *s)
{
    string_t *root = string_root(s);
    if (root && root->sopt_ref) {
        root->sopt_ref--;
        if (0==root->sopt_ref && root->sopt_clr) {
            free(root->body.string - root->begin);

            os_objzero(s);
        }
    }
}

/*
* unsafe
*/
static inline char *
string_steal(string_t *s)
{
    char *p = string_string(s);
    if (p) {
        s->sopt_tmp = p[s->len];
        p[s->len] = 0;
    }

    return p;
}

/*
* unsafe
*/
static inline void
string_restore(string_t *s)
{
    char *p = string_string(s);
    if (p) {
        p[s->len] = s->sopt_tmp;
        s->sopt_tmp = 0;
    }
}

static inline bool
string_can_slice(string_t *s, uint32_t begin, uint32_t len)
{
    uint32_t max = begin + len - 1;

    return 0 <= begin && begin <= max && max <= s->len;
}

static inline string_t
string_slice(string_t *s, uint32_t begin, uint32_t len)
{
    string_t new = STRING_ZERO;
    
    if (string_can_slice(s, begin, len)) {
        new = *s;
        new.sopt_top    = false;
        new.sopt_eoz    = false;
        
        new.len         = len;
        new.begin       = s->begin + begin;
        new.body.root   = string_get(string_root(s));
    }
    
    return new;
}

static inline int
string_cmp(string_t *a, string_t *b)
{
    if (a) {
        if (b) {
            uint32_t min = (a->len==b->len)?a->len:OS_MIN(a->len, b->len);

            return memcmp(string_string(a), string_string(b), min);
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

static inline bool
string_eq(string_t *a, string_t *b)
{
    if (a) {
        if (b) {
            if (a->len==b->len) {
                char *ha = string_string(a);
                char *hb = string_string(b);

                return ha==hb || 0==memcmp(ha, hb, a->len);
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
