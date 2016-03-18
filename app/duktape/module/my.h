#ifndef __MY_H_86ec299bb02d4b26b31952dd31eef107__
#define __MY_H_86ec299bb02d4b26b31952dd31eef107__
/******************************************************************************/
static inline int
__set___info_t(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    pipe_info_t *p = (pipe_info_t *)obj;
    
    __set_obj_string(ctx, idx, "stdout", p->std[__stdout].buf);
    __set_obj_string(ctx, idx, "stderr", p->std[__stderr].buf);
    __set_obj_int(ctx, idx, "errno", p->err);

    return 0;
}

static inline int
__get_libval_t(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    int err = 0;
    libval_t *p = (libval_t *)obj;

    p->size = __get_obj_uint(ctx, idx, "size");
    switch(p->size) {
        case 8:
            p->u.b8 = __get_obj_uint(ctx, idx, "b8");
            break;
        case 4:
            p->u.b4 = __get_obj_uint(ctx, idx, "b4");
            break;
        default:
            err = -EKEYBAD;
            break;
    }

    return err;
}

static inline int
__set_libval_t(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    int err = 0;
    libval_t *p = (libval_t *)obj;
    
    __set_obj_uint(ctx, idx, "size", p->size);
    switch(p->size) {
        case 8:
            __set_obj_uint(ctx, idx, "b8", p->u.b8);
            break;
        case 4:
            __set_obj_uint(ctx, idx, "b4", p->u.b4);
            break;
        default:
            err = -EKEYBAD;
            break;
    }

    return err;
}
/******************************************************************************/
#endif /* __MY_H_86ec299bb02d4b26b31952dd31eef107__ */