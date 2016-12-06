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
    uint32 len;
    uint32 size;
    uint32 minsize;
    uint32 expand;
    uint32 flags;
    
    char *buf;
} simple_buffer_t;

EXTERN void
sb_fini(simple_buffer_t *sb);

#define SB_AUTO_INITER(_size, _minsize, _expand)  _size, _minsize, _expand, SB_F_EXPAND_AUTO

EXTERN int
sb_init(simple_buffer_t *sb, uint32 size, uint32 minsize, uint32 expand, uint32 flags);

EXTERN void
sb_clean(simple_buffer_t *sb);

EXTERN int
sb_expand(simple_buffer_t *sb, uint32 expand);

EXTERN char *
sb_buf(simple_buffer_t *sb);

EXTERN uint32
sb_len(simple_buffer_t *sb);

EXTERN uint32
sb_left(simple_buffer_t *sb);

EXTERN char *
sb_cursor(simple_buffer_t *sb);

EXTERN int
sb_shift(simple_buffer_t *sb, int len);

#define sb_append_number(_sb, _number) ({ \
    int __err = 0; \
    if (sb_left(_sb) >= sizeof(_number) || 0==sb_expand(_sb, 0)) { \
        *(typeof(_number) *)sb_cursor(_sb) = _number; \
        sb_shift(_sb, sizeof(_number)); \
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
sb_vsprintf(simple_buffer_t *sb, const char *fmt, va_list args);

EXTERN int
sb_sprintf(simple_buffer_t *sb, const char *fmt, ...);

#define sb_sprintln(_sb, _fmt, _args...)    sb_sprintf(_sb, _fmt __crlf, ##_args)

#ifdef __BOOT__
#include "weos/boot/sbuffer.c"
#endif
/******************************************************************************/
#endif /* __SBUFFER_H_3338a77658884b5e8d82fcb23fb8b61c__ */
