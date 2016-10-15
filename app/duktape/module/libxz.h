#ifndef __LIBXZ_H_cb5f61dd32b64d6a8ddfe1ef06ff23cc__
#define __LIBXZ_H_cb5f61dd32b64d6a8ddfe1ef06ff23cc__
/******************************************************************************/
#define xzopen(ctx, func)       ({  \
    char *filename = (char *)duk_require_string(ctx, 0); \
    char *mode = (char *)duk_require_string(ctx, 1); \
                                    \
    HFILE f = func(filename, mode); \
                                    \
    js_push_pointer(ctx, f);         \
                                    \
    1;                              \
})

#define xzdopen(ctx, func)      ({  \
    int fd = duk_require_int(ctx, 0); \
    char *mode = (char *)duk_require_string(ctx, 1); \
                                    \
    HFILE f = func(fd, mode);       \
                                    \
    js_push_pointer(ctx, f);         \
                                    \
    1;                              \
})

#define xzread(ctx, func)       ({  \
    duk_size_t bsize = 0;           \
                                    \
    HFILE f = (HFILE)duk_require_pointer(ctx, 0); \
    duk_buffer_t buf = duk_require_buffer_data(ctx, 1, &bsize); \
                                    \
    int err = func(f, buf, bsize);  \
                                    \
	duk_push_int(ctx, err);         \
                                    \
	1;                              \
})

#define xzreadEx(ctx, func)     ({  \
    int err = 0;                    \
                                    \
    HFILE f = (HFILE)duk_require_pointer(ctx, 0); \
    int size = duk_require_int(ctx, 1); \
                                    \
    duk_buffer_t buf = js_push_dynamic_buffer(ctx, size); \
    if (NULL==buf) {                \
        err = __js_seterrno(ctx, -ENOMEM); \
    } else {                        \
        err = func(f, buf, size);   \
    }                               \
                                    \
	duk_push_int(ctx, err);         \
                                    \
	1;                              \
})

#define xzwrite(ctx, func)      ({  \
    duk_size_t bsize = 0;           \
    duk_buffer_t buf;               \
                                    \
    HFILE f = (HFILE)duk_require_pointer(ctx, 0); \
                                    \
    int err = duk_require_buffer_or_lstring(ctx, 1, &buf, &bsize); \
    if (err<0) {                    \
        __js_seterrno(ctx, err);       \
    } else {                        \
        err = func(f, buf, bsize);  \
    }                               \
                                    \
	duk_push_int(ctx, err);         \
                                    \
	1;                              \
})

#define xzerror(ctx, func)      ({  \
    HFILE f = (HFILE)duk_require_pointer(ctx, 0); \
                                    \
    int err = 0;                    \
    char *errstring = (char *)func(f, &err); \
                                    \
    duk_push_int(ctx, err);         \
                                    \
    1;                              \
})
/******************************************************************************/
#endif /* __LIBXZ_H_cb5f61dd32b64d6a8ddfe1ef06ff23cc__ */