#ifndef __LIBBZ_H_17116ab96ac74fce8f0242cf765529b9__
#define __LIBBZ_H_17116ab96ac74fce8f0242cf765529b9__
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
#endif /* __LIBBZ_H_17116ab96ac74fce8f0242cf765529b9__ */