/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
DECLARE void
sb_fini(simple_buffer_t *sb)
{
    os_free(sb->buf);
}

DECLARE int
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

DECLARE void
sb_clean(simple_buffer_t *sb)
{
    if (sb) {
        sb->len = 0;
    }
}

DECLARE int
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

DECLARE char *
sb_buf(simple_buffer_t *sb)
{
    return sb_field(sb, buf, NULL);
}

DECLARE uint32
sb_len(simple_buffer_t *sb)
{
    return sb_field(sb, len, 0);
}

DECLARE uint32
sb_size(simple_buffer_t *sb)
{
    return sb_field(sb, size, 0);
}

DECLARE uint32
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

DECLARE char *
sb_cursor(simple_buffer_t *sb)
{
    if (sb && sb_left(sb)) {
        return sb->buf + sb->len;
    } else {
        return NULL;
    }
}

DECLARE int
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

DECLARE int
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

DECLARE int
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

DECLARE int
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
/******************************************************************************/
