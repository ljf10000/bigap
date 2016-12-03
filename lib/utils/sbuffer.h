#ifndef __SBUFFER_H_3338a77658884b5e8d82fcb23fb8b61c__
#define __SBUFFER_H_3338a77658884b5e8d82fcb23fb8b61c__
/******************************************************************************/
#ifndef SB_MINSIZE
#define SB_MINSIZE          4096
#endif

#ifndef SB_EXPAND
#define SB_EXPAND           4096
#endif

enum {
    SB_F_EXPAND_AUTO = 0x01,
};

typedef struct {
    int err;
    
    uint32 len;
    uint32 size;
    uint32 minsize;
    uint32 expand;
    uint32 flags;
    
    char *buf;
} simple_buffer_t;

#define SB_INITER(_size, _minsize, _expand, _flags) { \
    .buf    = NULL,         \
    .err    = 0,            \
    .len    = 0,            \
    .size   = 1 + (_size),  \
    .minsize= _minsize,     \
    .expand = _expand,      \
    .flags  = _flags,       \
}   /* end */

static inline void
sb_fini(simple_buffer_t *sb)
{
    os_free(sb->buf);
}

EXTERN int
sb_init(simple_buffer_t *sb, uint32 size, uint32 minsize, uint32 expand, uint32 flags);

static inline void
sb_clean(simple_buffer_t *sb)
{
    sb->len = 0;
}

EXTERN int
sb_expand(simple_buffer_t *sb, uint32 expand);

EXTERN uint32
sb_left(simple_buffer_t *sb);

EXTERN char *
sb_cursor(simple_buffer_t *sb);

EXTERN void
sb_backspace(simple_buffer_t *sb, uint32 count);

#define sb_append_number(_sb, _number) ({ \
    int __err = 0; \
    if (sb_left(_sb) >= sizeof(_number) || 0==sb_expand(_sb, 0)) { \
        *(typeof(_number) *)sb_cursor(_sb) = _number; \
        (_sb)->len += sizeof(_number); \
    } else { \
        __err = -ENOSPACE; \
    } \
    __err; \
})

EXTERN int
sb_append_buffer(simple_buffer_t *sb, void *buf, uint32 len);

static inline int
sb_append_string(simple_buffer_t *sb, char *string)
{
    return sb_append_buffer(sb, string, os_strlen(string));
}

EXTERN int
sb_vsprintf(simple_buffer_t *sb, char *fmt, va_list args);

EXTERN int
sb_sprintf(simple_buffer_t *sb, char *fmt, ...);

#define sb_sprintln(_sb, _fmt, _args...)    sb_sprintf(_sb, _fmt __crlf, ##_args)

#ifdef __BOOT__
#include "weos/boot/sbuffer.c"
#endif
/******************************************************************************/
#endif /* __SBUFFER_H_3338a77658884b5e8d82fcb23fb8b61c__ */
