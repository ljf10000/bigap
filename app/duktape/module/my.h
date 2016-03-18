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
    libval_t *p = (libval_t *)obj;

    p->size = __get_obj_uint(ctx, idx, "size");
    p->type = __get_obj_uint(ctx, idx, "type");
    p->dir  = __get_obj_uint(ctx, idx, "dir");

    switch(p->type) {
        case LIBVAL_F32:
            p->u.f4 = (float32_t)__get_obj_number(ctx, idx, "f4");
            break;
        case LIBVAL_I32:
            p->u.i4 = __get_obj_int(ctx, idx, "i4");
            break;
        case LIBVAL_U32:
            p->u.u4 = __get_obj_uint(ctx, idx, "u4");
            break;
        case LIBVAL_F64:
            p->u.f8 = __get_obj_number(ctx, idx, "f8");
            break;
        case LIBVAL_I64:
            p->u.i8 = (int64_t)__get_obj_number(ctx, idx, "i8");
            break;
        case LIBVAL_U64:
            p->u.u8 = (uint64_t)__get_obj_number(ctx, idx, "u8");
            break;
        case LIBVAL_BUF:
            p->u.b = __get_obj_buffer(ctx, idx, "b", &p->len);
            break;
        case LIBVAL_STR:
            p->u.s = __get_obj_string(ctx, idx, "s", &p->len);
            break;
        case LIBVAL_PTR:
            p->u.p = __get_obj_pointer(ctx, idx, "p");
            break;
        case LIBVAL_VOD: /* down */
        default:
            os_do_nothing();
            break;
    }

    return 0;
}

static inline int
__set_libval_t(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    libval_t *p = (libval_t *)obj;
    
    __set_obj_uint(ctx, idx, "size", p->size);
    __set_obj_uint(ctx, idx, "type", p->type);
    __set_obj_uint(ctx, idx, "dir",  p->dir);
    __set_obj_uint(ctx, idx, "len",  p->len);

    switch(p->type) {
        case LIBVAL_F32:
            __set_obj_number(ctx, idx, "f4", (float64_t)p->u.f4);
            break;
        case LIBVAL_I32:
            __set_obj_int(ctx, idx, "i4", p->u.i4);
            break;
        case LIBVAL_U32:
            __set_obj_uint(ctx, idx, "i4", p->u.i4);
            break;
        case LIBVAL_F64:
            __set_obj_number(ctx, idx, "f8", p->u.f8);
            break;
        case LIBVAL_I64:
            __set_obj_number(ctx, idx, "i8", (float64_t)p->u.i8);
            break;
        case LIBVAL_U64:
            __set_obj_number(ctx, idx, "u8", (float64_t)p->u.u8);
            break;
        case LIBVAL_BUF:
            os_do_nothing();
            break;
        case LIBVAL_STR:
            os_do_nothing();
            break;
        case LIBVAL_PTR:
            __set_obj_pointer(ctx, idx, "p", p->u.p);
            break;
        case LIBVAL_VOD: /* down */
        default:
            os_do_nothing();
            break;
    }
    
    return 0;
}
/******************************************************************************/
#endif /* __MY_H_86ec299bb02d4b26b31952dd31eef107__ */
