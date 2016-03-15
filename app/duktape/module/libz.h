#ifndef __LIBZ_H_fd23af79026249cf94aff1ae0ccb1f79__
#define __LIBZ_H_fd23af79026249cf94aff1ae0ccb1f79__
/******************************************************************************/
static inline int
__get_gz_header(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    gz_header *p = (gz_header *)obj; os_objzero(p);
    
    return 0;
}

static inline int
__set_gz_header(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    gz_header *p = (gz_header *)obj;
    
    return 0;
}

/******************************************************************************/
#endif /* __LIBZ_H_fd23af79026249cf94aff1ae0ccb1f79__ */