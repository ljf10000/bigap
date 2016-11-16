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

static inline int
sb_init(simple_buffer_t *sb, uint32 size, uint32 minsize, uint32 expand, uint32 flags)
{
    if (0==sb->minsize) {
        sb->minsize = minsize?minsize:SB_MINSIZE;
    }
    
    if (0==sb->expand) {
        sb->expand  = expand?expand:SB_EXPAND;
    }

    if (0==sb->size) {
        if (size < sb->minsize) {
            size = sb->minsize;
        }
        
        sb->size = size;
    }

    if (0==sb->flags) {
        sb->flags = flags;
    }
    
    sb->buf = (char *)os_zalloc(1 + sb->size);
    if (NULL==sb->buf) {
        return -ENOMEM;
    }
    sb->len = 0;
    
    return 0;
}

static inline void
sb_fini(simple_buffer_t *sb)
{
    os_free(sb->buf);
}

static inline void
sb_clean(simple_buffer_t *sb)
{
    sb->len = 0;
}

static inline int
sb_expand(simple_buffer_t *sb, uint32 expand)
{
    if (false==os_hasflag(sb->flags, SB_F_EXPAND_AUTO)) {
        return -ENOSPACE;
    }
    
    if (0==expand) {
        if (sb->expand >= sb->minsize) {
            expand = sb->expand;
        } else {
            expand = sb->minsize * sb->expand;
        }
    }

    uint32 newsize = sb->size + expand;
    
    char *buf = (char *)os_realloc(sb->buf, 1 + newsize);
    if (NULL==buf) {
        return -ENOMEM;
    }

    sb->buf = buf;
    sb->size = newsize;

    return 0;
}

static inline uint32
sb_left(simple_buffer_t *sb)
{
    if (sb->size > sb->len) {
        return sb->size - sb->len;
    }
    else if (os_hasflag(sb->flags, SB_F_EXPAND_AUTO)) {
        sb_expand(sb, 0);

        return sb->size - sb->len;
    }
    else {
        return 0;
    }
}

static inline char *
sb_cursor(simple_buffer_t *sb)
{
    if (sb_left(sb)) {
        return sb->buf + sb->len;
    } else {
        return NULL;
    }
}

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

static inline int
sb_append_buffer(simple_buffer_t *sb, void *buf, uint32 len)
{
    if (sb_left(sb) >= len || 0==sb_expand(sb, len)) {
        os_memcpy(sb->buf, buf, len);
        sb->len += len;

        return 0;
    } else {
        return -ENOSPACE;
    }

}

static inline int
sb_append_string(simple_buffer_t *sb, char *string)
{
    return sb_append_buffer(sb, string, os_strlen(string));
}

static inline int
sb_vsprintf(simple_buffer_t *sb, char *fmt, va_list args)
{
    va_list copy;

    va_copy(copy, args);
    int vsize = os_vsprintf_size(fmt, copy);
    va_end(copy);

    if (sb_left(sb) >= vsize || 0==sb_expand(sb, vsize)) {
        os_vsnprintf(sb->buf, sb_left(sb), fmt, args);

        return 0;
    } else {
        return -ENOSPACE;
    }
}

static inline int
sb_sprintf(simple_buffer_t *sb, char *fmt, ...)
{
    va_list args;
    int err;
    
    va_start(args, fmt);
    err = sb_vsprintf(sb, fmt, args);
    va_end(args);
    
    return err;
}
/******************************************************************************/
#endif /* __SBUFFER_H_3338a77658884b5e8d82fcb23fb8b61c__ */
