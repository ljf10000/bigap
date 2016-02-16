#ifndef __STRING2_H_795ed673d7eb48a4b2d3cc401be937d5__
#define __STRING2_H_795ed673d7eb48a4b2d3cc401be937d5__
/******************************************************************************/
struct string_s;

typedef struct string_s {
    uint32_t tmp:8;     /* all */
    uint32_t top:1;     /* all */
    uint32_t eoz:1;     /* all: end of zero */
    uint32_t eob:1;     /* root: end of body */
    uint32_t clr:1;     /* root: need to clean */
    uint32_t ref:20;    /* root */

    uint32_t len;       /* all */
    uint32_t begin;     /* all: begin to string */

    union {
        char *string;           /* root */
        
        char body[0];           /* root */
        
        struct string_s *root;  /* current */
    } u;
} string_t;

#define STRING_ZERO {0}

#define __STRING_S_INITER(_s, _begin, _dy) { \
    .top    = true,         \
    .eoz    = true,         \
    .dy     = _dy,          \
    .ref    = 1,            \
    .len    = (uint32_t)os_strlen(_s) - _begin, \
    .begin  = (uint32_t)_begin, \
    .u.string = (char *)_s + _begin, \
}

#define __STRING_B_INITER(_bin, _begin, _len, _dy) { \
    .top    = true,         \
    .eoz    = false,        \
    .dy     = _dy,          \
    .ref    = 1,            \
    .len    = (uint32_t)_len,   \
    .begin  = (uint32_t)_begin, \
    .u.string = (char *)_bin + _begin, \
}

#define STRING_S_INITER(_s, _begin, _dy)            __STRING_S_INITER(_s, 0, _dy)
#define STRING_B_INITER(_bin, _begin, _len, _dy)    __STRING_B_INITER(_bin, 0, _len, _dy)

static inline string_t *
__string_root(string_t *s)
{
    return s->top?s:s->u.root;
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
        if (false==s->top) {
            string_t *root = s->u.root;
            
            return root && root->top && root->u.string && root->len;
        } else {
            return NULL!=s->u.string;
        }
    } else {
        return false;
    }
}

static inline char *
string_string(string_t *s)
{
    if (is_good_string(s)) {
        return string_root(s)->u.string + s->begin;
    } else {
        return NULL;
    }
}

static inline string_t *
string_get(string_t *s)
{
    string_t *root = string_root(s);
    if (root) {
        root->ref++;
    }

    return s;
}

static inline void
string_put(string_t *s)
{
    string_t *root = string_root(s);
    if (root && root->ref) {
        root->ref--;
        if (0==root->ref && root->clr) {
            free(root->u.string - root->begin);

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
        s->tmp = p[s->len];
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
        p[s->len] = s->tmp;
        s->tmp = 0;
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
        new.len     = len;
        new.begin   = s->begin + begin;
        new.u.root  = string_get(string_root(s));
    }
    
    return new;
}

static inline int
string_cmp(string_t *a, string_t *b)
{
    if (a) {
        if (b) {
            uint32_t min = (a->len==b->len)?a->len:os_min(a->len, b->len);

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
