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

static inline void
sb_fini(simple_buffer_t *sb)
{
    os_free(sb->buf);
}

#define SB_AUTO_INITER(_size, _minsize, _expand)  _size, _minsize, _expand, SB_F_EXPAND_AUTO

static inline int
sb_init(simple_buffer_t *sb, uint32 size, uint32 minsize, uint32 expand, uint32 flags)
{
    if (NULL==sb) {
        return -EKEYNULL;
    }
    
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
sb_clean(simple_buffer_t *sb)
{
    if (sb) {
        sb->len = 0;
    }
}

static inline int
sb_expand(simple_buffer_t *sb, uint32 expand)
{
    if (NULL==sb) {
        return -EKEYNULL;
    }
    else if (false==os_hasflag(sb->flags, SB_F_EXPAND_AUTO)) {
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
#define sb_field(_sb, _member, _error)  (_sb)?(_sb)->_member:_error

static inline char *
sb_buf(simple_buffer_t *sb)
{
    return sb_field(sb, buf, NULL);
}

static inline uint32
sb_len(simple_buffer_t *sb)
{
    return sb_field(sb, len, 0);
}

static inline uint32
sb_size(simple_buffer_t *sb)
{
    return sb_field(sb, size, 0);
}

static inline uint32
sb_left(simple_buffer_t *sb)
{
    if (NULL==sb) {
        return 0;
    }
    else if (sb->size > sb->len) {
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
    if (sb && sb_left(sb)) {
        return sb->buf + sb->len;
    } else {
        return NULL;
    }
}

static inline int
sb_shift(simple_buffer_t *sb, int len)
{
    uint32 left;

    if (NULL==sb) {
        return -EKEYNULL;
    }
    else if (len > 0 && sb_left(sb) < len) {
        return -ENOSPACE;
    }
    else if (len < 0 && sb->len < (-len)) {
        return -ENOSPACE;
    }

    sb->len += len;
    
    *sb_cursor(sb) = 0;
    
    return 0;
}

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

static inline int
sb_append_buffer(simple_buffer_t *sb, void *buf, uint32 len)
{
    if (NULL==sb || NULL==buf) {
        return -EKEYNULL;
    }
    else if (sb_left(sb) >= len || 0==sb_expand(sb, len)) {
        os_memcpy(sb->buf, buf, len);

        sb_shift(sb, len);

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
sb_vsprintf(simple_buffer_t *sb, const char *fmt, va_list args)
{
    va_list copy;

    if (NULL==sb || NULL==fmt) {
        return -EKEYNULL;
    }

    va_copy(copy, args);
    int vsize = os_vsprintf_size(fmt, copy);
    va_end(copy);

    if (sb_left(sb) >= vsize || 0==sb_expand(sb, vsize)) {
        os_vsnprintf(sb_cursor(sb), sb_left(sb), fmt, args);

        sb_shift(sb, vsize);

        return 0;
    } else {
        return -ENOSPACE;
    }
}

static inline int
sb_sprintf(simple_buffer_t *sb, const char *fmt, ...)
{
    va_list args;
    int err;
    
    if (NULL==sb || NULL==fmt) {
        return -EKEYNULL;
    }
    
    va_start(args, fmt);
    err = sb_vsprintf(sb, fmt, args);
    va_end(args);
    
    return err;
}

#define sb_sprintln(_sb, _fmt, _args...)    sb_sprintf(_sb, _fmt __crlf, ##_args)
/******************************************************************************/
#endif /* __SBUFFER_H_3338a77658884b5e8d82fcb23fb8b61c__ */
