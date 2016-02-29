#ifndef __DLL_H_246c16b7624f4197910c79d8f0707b65__
#define __DLL_H_246c16b7624f4197910c79d8f0707b65__
#ifdef __APP__
/******************************************************************************/
enum {
    LIB_T_POINTER,
    LIB_T_V32,
    LIB_T_V64,

    LIB_T_END
};

typedef struct {
    int size;

    union {
        uint64_t b8;
        uint32_t b4;
        void *p;
    } u;
} libval_t;

#define LIBVAL0_INITER()      {.size = 0}
#define LIBVAL4_INITER(_val)  { \
    .size = sizeof(uint32_t),   \
    .u = {                      \
        .b4 = (uint32_t)_val,   \
    },                          \
} /* end */
#define LIBVAL8_INITER(_val)  { \
    .size = sizeof(uint64_t),   \
    .u = {                      \
        .b8 = (uint64_t)(_val), \
    },                          \
} /* end */
#define LIBVALp_INITER(_val)  { \
    .size = sizeof(void *),     \
    .u = {                      \
        .p = (void *)(_val),    \
    },                          \
} /* end */

#define LIBRESULT_INITER(_type) {.size = sizeof(_type)}

typedef struct {
    int count;
    libval_t *param;
    
    libval_t result;
} libproto_t;

#define LIBPROTO_INITER(_type, _params)  {  \
    .count = os_count_of(_params),          \
    .param = _params,                       \
    .result = LIBRESULT_INITER(_type),      \
}

typedef void func_v_v(void);
typedef void func_v_4x(uint32_t, ...);
typedef void func_v_8x(uint64_t, ...);
typedef void func_v_px(void *, ...);

typedef uint32_t func_4_v(void);
typedef uint32_t func_4_4x(uint32_t, ...);
typedef uint32_t func_4_8x(uint64_t, ...);
typedef uint32_t func_4_px(void *, ...);

typedef uint64_t func_8_v(void);
typedef uint64_t func_8_4x(uint32_t, ...);
typedef uint64_t func_8_8x(uint64_t, ...);
typedef uint64_t func_8_px(void *, ...);

typedef void *func_p_v(void);
typedef void *func_p_4x(uint32_t, ...);
typedef void *func_p_8x(uint64_t, ...);
typedef void *func_p_px(void *, ...);

#define LIBVAL(_val)                (4==(_val)->size?(_val)->u.b4:(_val)->u.b8)
#define LIBFUN(_f, _type)           ((_type *)(_f))
#define LIBPARAM(_proto, _idx)      (&(_proto)->param[_idx])

#define LIBPARAMVAL(_proto, _idx)   LIBVAL(LIBPARAM(_proto, _idx))
#define LIBPARAM0(_proto)           /* nothing */
#define LIBPARAM1(_proto)           LIBPARAMVAL(_proto, 0)
#define LIBPARAM2(_proto)           LIBPARAM1(_proto), LIBPARAMVAL(_proto, 1)
#define LIBPARAM3(_proto)           LIBPARAM2(_proto), LIBPARAMVAL(_proto, 2)
#define LIBPARAM4(_proto)           LIBPARAM3(_proto), LIBPARAMVAL(_proto, 3)
#define LIBPARAM5(_proto)           LIBPARAM4(_proto), LIBPARAMVAL(_proto, 4)
#define LIBPARAM6(_proto)           LIBPARAM5(_proto), LIBPARAMVAL(_proto, 5)
#define LIBPARAM7(_proto)           LIBPARAM6(_proto), LIBPARAMVAL(_proto, 6)
#define LIBPARAM8(_proto)           LIBPARAM7(_proto), LIBPARAMVAL(_proto, 7)
#define LIBPARAM9(_proto)           LIBPARAM8(_proto), LIBPARAMVAL(_proto, 8)
#define LIBPARAMx(_proto, _count)   LIBPARAM##_count(_proto)

#define LIBCALL(_f, _proto, _count)     ({  \
    int __err = 0;                          \
                                            \
    if (0==(_proto)->result.size) {         \
        if (0==(_count)) {                  \
            LIBFUN(_f, func_v_v)();         \
        } else if (4==LIBPARAM(_proto, 0)->size) { \
            LIBFUN(_f, func_v_4x)(LIBPARAMx(_proto, _count)); \
        } else if (8==LIBPARAM(_proto, 0)->size) { \
            LIBFUN(_f, func_v_8x)(LIBPARAMx(_proto, _count));  \
        } else {                            \
            __err = EDLLPARAMSIZE;          \
        }                                   \
    } else if (4==(_proto)->result.size) {  \
        if (0==(_count)) {                  \
            (_proto)->result.u.b4 = LIBFUN(_f, func_4_v)(); \
        } else if (4==LIBPARAM(_proto, 0)->size) { \
            (_proto)->result.u.b4 = LIBFUN(_f, func_4_4x)(LIBPARAMx(_proto, _count)); \
        } else if (8==LIBPARAM(_proto, 0)->size) {                            \
            (_proto)->result.u.b4 = LIBFUN(_f, func_4_8x)(LIBPARAMx(_proto, _count));  \
        } else {                            \
            __err = EDLLPARAMSIZE;          \
        }                                   \
    } else if (8==(_proto)->result.size) {  \
        if (0==(_count)) {                  \
            (_proto)->result.u.b8 = LIBFUN(_f, func_8_v)(); \
        } else if (4==LIBPARAM(_proto, 0)->size) { \
            (_proto)->result.u.b8 = LIBFUN(_f, func_8_4x)(LIBPARAMx(_proto, _count)); \
        } else if (8==LIBPARAM(_proto, 0)->size) {                            \
            (_proto)->result.u.b8 = LIBFUN(_f, func_8_8x)(LIBPARAMx(_proto, _count));  \
        } else {                            \
            __err = EDLLPARAMSIZE;          \
        }                                   \
    } else {                                \
        __err = -EDLLRESULTSIZE;            \
    }                                       \
                                            \
    __err;                                  \
}) /* end */

static int
__libcall(void *f, libproto_t *proto)
{
    int err = 0;
    
    switch(proto->count) {
        default:
        case 0:
            err = LIBCALL(f, proto, 0);
            break;
        case 1:
            err = LIBCALL(f, proto, 1);
            break;
        case 2:
            err = LIBCALL(f, proto, 2);
            break;
        case 3:
            err = LIBCALL(f, proto, 3);
            break;
        case 4:
            err = LIBCALL(f, proto, 4);
            break;
        case 5:
            err = LIBCALL(f, proto, 5);
            break;
        case 6:
            err = LIBCALL(f, proto, 6);
            break;
        case 7:
            err = LIBCALL(f, proto, 7);
            break;
        case 8:
            err = LIBCALL(f, proto, 8);
            break;
        case 9:
            err = LIBCALL(f, proto, 9);
            break;
    }

    return err;
}

static int
os_libcall(const char *lib, const char *sym, libproto_t *proto)
{
    int err = 0;
    
    void *h = dlopen(lib, RTLD_NOW | RTLD_GLOBAL);
    if (NULL==h) {
        debug_error("load %s error:%d", lib, -errno);
        err = -EDLLLOAD; goto error;
    }
    
    dlerror();

    char *errstring;
    void *f = dlsym(h, sym);
    if (NULL==f) {
        err = -EDLLSYMLOAD; goto error;
    }
    else if (NULL!=(errstring=dlerror())) {
        debug_error("load %s:%s error:%s", lib, sym, errstring);

        err = -EDLLSYMLOAD; goto error;
    }
    debug_trace("load %s:%s=%p", lib, sym, f);

    err = __libcall(f, proto);
    if (err<0) {
        goto error;
    }
    else if (proto->result.u.b4) {
        err = (int)proto->result.u.b4; goto error;
    }
    
error:
    if (h) {
        dlclose(h);
    }
    
    return err;
}
/******************************************************************************/
#endif /* __APP__ */
#endif /* __DLL_H_246c16b7624f4197910c79d8f0707b65__ */
