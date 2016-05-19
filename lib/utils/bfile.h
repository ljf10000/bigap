#ifndef __BFILE_H_46bf2ff024244a7cbfd6b90899a7d09f__
#define __BFILE_H_46bf2ff024244a7cbfd6b90899a7d09f__
/******************************************************************************/
typedef struct {
    char *filename;
    uint mode;
    
    byte **pages;
    uint pagesize;
    uint count;
    
    uint size;
    uint offset;
} BFILE;

static inline uint
__bf_space(BFILE *f)
{
    return f->count * f->pagesize;
}

static inline uint
__bf_left(BFILE *f)
{
    return __bf_space(f) - f->size;
}

static inline uint
__bf_current(BFILE *f)
{
    return f->size / f->pagesize;
}

static inline byte *
__bf_current_page(BFILE *f)
{
    return f->pages[__bf_current(f)];
}

static inline uint
__bf_current_offset(BFILE *f)
{
    return f->offset % f->pagesize;
}

static inline void
__bf_page_write(BFILE *f, uint idx, uint offset, void *buffer, int size)
{
    os_memcpy(f->pages[idx] + offset, buffer, size);
}

static inline void
__bf_page_append(BFILE *f, void *buffer, int size)
{
    __bf_page_write(f, __bf_current(f), __bf_current_offset(f), buffer, size);
    
    f->size     += size;
    f->offset   += size;
}

static inline int
__bf_grow(BFILE *f, int size)
{
    uint count  = OS_ALIGN(size, f->pagesize)/f->pagesize;
    int i,  err = 0;
    
    byte **pages = (byte **)os_zalloc((f->count + count) * sizeof(byte *));
    if (NULL==pages) {
        err = -ENOMEM; goto error;
    }

    for (i=0; i<count; i++) {
        byte *page = (byte *)os_malloc(f->pagesize);
        if (NULL==page) {
            err = -ENOMEM; goto error;
        }

        pages[f->count+i] = page;
    }
    os_memcpy(pages, f->pages, f->count * sizeof(byte *));

    os_free(f->pages);
    f->pages = pages;
    f->count += count;
    
    return 0;
error:
    for (i=0; i<count; i++) {
        os_free(f->pages[f->count+i]);
    }
    os_free(pages);
    
    return err;
}

static inline int
__bf_grow_one(BFILE *f)
{
    return __bf_grow(f, 1);
}

static inline int
__bf_append(BFILE *f, void *buffer, int size)
{
    int err = 0;
    
    uint left   = __bf_left(f);

    
    if (size <= left) {
        return __bf_page_append(f, buffer, size);
    } else { /* size > left */
        __bf_page_append(f, buffer, left);

        buffer  += left;
        size    -= left;

        __bf_grow_one(f);

        return __bf_append(f, buffer, size);
    }
}

static inline BFILE *
os_bopen(char *filename, char *mode)
{
    BFILE *f = (BFILE *)os_zalloc(sizeof(*f));
    if (NULL==f) {
        return NULL;
    }

    f->pagesize = getpagesize();
    f->filename = os_strdup(filename);
    // todo: mode

    return f;
}

static inline int
os_bclose(BFILE *f)
{
    if (NULL==f) {
        return -EBADF;
    }
    
    if (f->pages) {
        int i;
        
        for (i=0; i<f->count; i++) {
            os_free(f->pages[i]);
        }
        
        os_free(f->pages);
    }
    
    os_free(f->filename);
}

static inline int
os_bseek(BFILE *f, int offset, int fromware)
{
    if (NULL==f) {
        return -EBADF;
    }

    uint oldoffset;
    
    switch(fromware) {
        case SEEK_SET:
            oldoffset = 0;
            break;
        case SEEK_CUR:
            oldoffset = f->offset;
            break;
        case SEEK_END:
            oldoffset = f->size;
            break;
        default:
            return -EINVAL3;
    }
    
    if (offset < 0) {
        if (oldoffset < (-offset)) {
            return -EINVAL2;
        }
    } else {
        if (oldoffset + offset > f->size) {
            return -EINVAL2;
        }
    }

    f->offset = oldoffset + offset;

    return 0;
}

static inline int
os_bwrite(BFILE *f, void *buffer, int size)
{
    if (NULL==f) {
        return -EBADF;
    }
    
}

static inline int
os_bread(BFILE *f, void *buffer, int size)
{
    if (NULL==f) {
        return -EBADF;
    }
    
}
/******************************************************************************/
#endif /* __BFILE_H_46bf2ff024244a7cbfd6b90899a7d09f__ */
