#ifndef __DLL_H_246c16b7624f4197910c79d8f0707b65__
#define __DLL_H_246c16b7624f4197910c79d8f0707b65__
#ifdef __APP__
/******************************************************************************/
#if 1
#define LIBVAL_TYPE_ENUM_MAPPER(_)  \
    _(LIBVAL_F32,   0, "f32"),  \
    _(LIBVAL_I32,   1, "i32"),  \
    _(LIBVAL_U32,   2, "u32"),  \
    _(LIBVAL_F64,   3, "f64"),  \
    _(LIBVAL_I64,   4, "i64"),  \
    _(LIBVAL_U64,   5, "u64"),  \
    _(LIBVAL_BUF,   6, "buf"),  \
    _(LIBVAL_STR,   7, "str"),  \
    _(LIBVAL_PTR,   8, "ptr"),  \
    _(LIBVAL_VOD,   9, "void"), \
    /* end */
EXTERN_ENUMER(libval_type, LIBVAL_TYPE_ENUM_MAPPER, LIBVAL_END);

EXTERN enum_ops_t *libval_type_ops(void);
EXTERN bool is_good_libval_type(int id);
EXTERN char *libval_type_getnamebyid(int id);
EXTERN int libval_type_getidbyname(const char *name);

#define LIBVAL_F32  LIBVAL_F32
#define LIBVAL_I32  LIBVAL_I32
#define LIBVAL_U32  LIBVAL_U32
#define LIBVAL_F64  LIBVAL_F64
#define LIBVAL_I64  LIBVAL_I64
#define LIBVAL_U64  LIBVAL_U64
#define LIBVAL_BUF  LIBVAL_BUF
#define LIBVAL_STR  LIBVAL_STR
#define LIBVAL_PTR  LIBVAL_PTR
#define LIBVAL_VOD  LIBVAL_VOD
#define LIBVAL_END  LIBVAL_END
#endif

#define LIBVAL_IN       0x01
#define LIBVAL_OUT      0x02
#define LIBVAL_INOUT    (LIBVAL_IN | LIBVAL_OUT)

typedef struct {
    uint32 size;  /* sizeof */
    uint32 type;  /* LIBVAL_END */
    uint32 len;   /* just for buffer/string */
    uint32 dir;   /* LIBVAL_IN/LIBVAL_OUT/LIBVAL_INOUT */

    union {
        float32 f4;
        int32   i4;
        uint32  u4;

        float64 f8;
        int64   i8;
        uint64  u8;

        byte    *b;
        char    *s;
        void    *p;
    } u;
} 
libval_t;

#define LIBVAL_F4_INITER(_dir, _val) { \
    .size = sizeof(float32),    \
    .type = LIBVAL_F32,         \
    .dir  = _dir,               \
    .u = {                      \
        .f4 = (float32)_val,    \
    },                          \
} /* end */

#define LIBVAL_I4_INITER(_dir, _val) { \
    .size = sizeof(int32),      \
    .type = LIBVAL_I32,         \
    .dir  = _dir,               \
    .u = {                      \
        .i4 = (int32)_val,      \
    },                          \
} /* end */

#define LIBVAL_U4_INITER(_dir, _val) { \
    .size = sizeof(uint32),     \
    .type = LIBVAL_U32,         \
    .dir  = _dir,               \
    .u = {                      \
        .u4 = (uint32)_val,     \
    },                          \
} /* end */

#define LIBVAL_F8_INITER(_dir, _val) { \
    .size = sizeof(float64),    \
    .type = LIBVAL_F64,         \
    .dir  = _dir,               \
    .u = {                      \
        .f8 = (float64)_val,    \
    },                          \
} /* end */

#define LIBVAL_I8_INITER(_dir, _val) { \
    .size = sizeof(int64),      \
    .type = LIBVAL_I64,         \
    .dir  = _dir,               \
    .u = {                      \
        .i8 = (int64)_val,      \
    },                          \
} /* end */

#define LIBVAL_U8_INITER(_dir, _val) { \
    .size = sizeof(uint64),     \
    .type = LIBVAL_U64,         \
    .dir  = _dir,               \
    .u = {                      \
        .u8 = (uint64)_val,     \
    },                          \
} /* end */

#define LIBVAL_B_INITER(_dir, _val, _len) { \
    .size = sizeof(byte *),     \
    .len  = _len,               \
    .dir  = _dir,               \
    .type = LIBVAL_BUF,         \
    .u = {                      \
        .b = (byte *)_val,      \
    },                          \
} /* end */

#define LIBVAL_S_INITER(_dir, _val) { \
    .size = sizeof(char *),     \
    .type = LIBVAL_STR,         \
    .dir  = _dir,               \
    .u = {                      \
        .s = (char *)_val,      \
    },                          \
} /* end */

#define LIBVAL_P_INITER(_dir, _val) { \
    .size = sizeof(void *),     \
    .type = LIBVAL_PTR,         \
    .dir  = _dir,               \
    .u = {                      \
        .p = (void *)_val,      \
    },                          \
} /* end */

#define LIBVAL_V_INITER(_dir, _val) { \
    .size = 0,                  \
    .type = LIBVAL_VOD,         \
    .dir  = 0,                  \
    .u = {                      \
        .p = NULL,              \
    },                          \
} /* end */

typedef struct {
    int count;
    libval_t *param;
    
    libval_t result;
} libproto_t;

#define ____LIBPROTO_INITER(_result_size, _params, _param_count)  {  \
    .count = _param_count,                  \
    .param = _params,                       \
    .result = {.size = _result_size},       \
}
#define __LIBPROTO_INITER(_result_size, _params)    ____LIBPROTO_INITER(_result_size, _params, os_count_of(_params))
#define LIBPROTO_INITER(_type, _params)  __LIBPROTO_INITER(sizeof(_type), _params)

typedef void func_0_0(void);
typedef void func_0_4(uint32, ...);
typedef void func_0_8(uint64, ...);

typedef uint32 func_4_0(void);
typedef uint32 func_4_4(uint32, ...);
typedef uint32 func_4_8(uint64, ...);

typedef uint64 func_8_0(void);
typedef uint64 func_8_4(uint32, ...);
typedef uint64 func_8_8(uint64, ...);

#if 0
#define LIBVAL(_val)                      ( \
    LIBVAL_F32==(_val)->type?(_val)->u.f4:( \
    LIBVAL_I32==(_val)->type?(_val)->u.i4:( \
    LIBVAL_U32==(_val)->type?(_val)->u.u4:( \
    LIBVAL_F64==(_val)->type?(_val)->u.f8:( \
    LIBVAL_I64==(_val)->type?(_val)->u.i8:( \
    LIBVAL_U64==(_val)->type?(_val)->u.u8:( \
    LIBVAL_BUF==(_val)->type?(_val)->u.b: ( \
    LIBVAL_STR==(_val)->type?(_val)->u.s: ( \
    LIBVAL_PTR==(_val)->type?(_val)->u.p: ( \
0)))))))))) /* end */
#else
#define LIBVAL(_val)                (4==(_val)->size?(_val)->u.u4:(_val)->u.u8)
#endif

#define LIBFUN(_prototype, _f)      ((_prototype *)(_f))
#define LIBPARAM(_proto, _IDX)      (&(_proto)->param[_IDX])

#ifndef LIBPARAMMAX
#define LIBPARAMMAX                 9
#endif

#define LIBPARAMCOUNT(n)            (LIBPARAMMAX>=n)

#define LIBPARAMVAL(_proto, _IDX)   LIBVAL(LIBPARAM(_proto, _IDX))
#define LIBPARAM0(_proto)           /* nothing */
#define LIBPARAM1(_proto)           LIBPARAMVAL(_proto, 0)
#if LIBPARAMCOUNT(2)
#   define LIBPARAM2(_proto)        LIBPARAM1(_proto), LIBPARAMVAL(_proto, 1)
#endif
#if LIBPARAMCOUNT(3)
#   define LIBPARAM3(_proto)        LIBPARAM2(_proto), LIBPARAMVAL(_proto, 2)
#endif
#if LIBPARAMCOUNT(4)
#   define LIBPARAM4(_proto)        LIBPARAM3(_proto), LIBPARAMVAL(_proto, 3)
#endif
#if LIBPARAMCOUNT(5)
#   define LIBPARAM5(_proto)        LIBPARAM4(_proto), LIBPARAMVAL(_proto, 4)
#endif
#if LIBPARAMCOUNT(6)
#   define LIBPARAM6(_proto)        LIBPARAM5(_proto), LIBPARAMVAL(_proto, 5)
#endif
#if LIBPARAMCOUNT(7)
#   define LIBPARAM7(_proto)        LIBPARAM6(_proto), LIBPARAMVAL(_proto, 6)
#endif
#if LIBPARAMCOUNT(8)
#   define LIBPARAM8(_proto)        LIBPARAM7(_proto), LIBPARAMVAL(_proto, 7)
#endif
#if LIBPARAMCOUNT(9)
#   define LIBPARAM9(_proto)        LIBPARAM8(_proto), LIBPARAMVAL(_proto, 8)
#endif
#if LIBPARAMCOUNT(10)
#   define LIBPARAM10(_proto)       LIBPARAM9(_proto), LIBPARAMVAL(_proto, 9)
#endif
#if LIBPARAMCOUNT(11)
#   define LIBPARAM11(_proto)       LIBPARAM10(_proto),LIBPARAMVAL(_proto, 10)
#endif
#if LIBPARAMCOUNT(12)
#   define LIBPARAM12(_proto)       LIBPARAM11(_proto),LIBPARAMVAL(_proto, 11)
#endif
#if LIBPARAMCOUNT(13)
#   define LIBPARAM13(_proto)       LIBPARAM12(_proto),LIBPARAMVAL(_proto, 12)
#endif
#if LIBPARAMCOUNT(14)
#   define LIBPARAM14(_proto)       LIBPARAM13(_proto),LIBPARAMVAL(_proto, 13)
#endif
#if LIBPARAMCOUNT(15)
#   define LIBPARAM15(_proto)       LIBPARAM14(_proto),LIBPARAMVAL(_proto, 14)
#endif
#if LIBPARAMCOUNT(16)
#   define LIBPARAM16(_proto)       LIBPARAM15(_proto),LIBPARAMVAL(_proto, 15)
#endif
#if LIBPARAMCOUNT(17)
#   define LIBPARAM17(_proto)       LIBPARAM16(_proto),LIBPARAMVAL(_proto, 16)
#endif
#if LIBPARAMCOUNT(18)
#   define LIBPARAM18(_proto)       LIBPARAM17(_proto),LIBPARAMVAL(_proto, 17)
#endif
#if LIBPARAMCOUNT(19)
#   define LIBPARAM19(_proto)       LIBPARAM18(_proto),LIBPARAMVAL(_proto, 18)
#endif

#define LIBPARAMx(_proto, _COUNT)   LIBPARAM##_COUNT(_proto)

#define __LIBCALL0(_f, _proto, _COUNT)  ({  \
    int __LIBCALL0_err = 0;                 \
                                            \
    switch(LIBPARAM(_proto, 0)->size) {     \
        case 4: LIBFUN(func_0_4, _f)(LIBPARAMx(_proto, _COUNT)); break; \
        case 8: LIBFUN(func_0_8, _f)(LIBPARAMx(_proto, _COUNT)); break; \
        default: __LIBCALL0_err = -EDLLPARAMSIZE; break; \
    }                                       \
                                            \
    __LIBCALL0_err;                         \
})

#define __LIBCALL4(_f, _proto, _COUNT)  ({  \
    int __LIBCALL4_err = 0;                 \
                                            \
    switch(LIBPARAM(_proto, 0)->size) {     \
        case 4: (_proto)->result.u.u4 = LIBFUN(func_4_4, _f)(LIBPARAMx(_proto, _COUNT)); break; \
        case 8: (_proto)->result.u.u4 = LIBFUN(func_4_8, _f)(LIBPARAMx(_proto, _COUNT)); break; \
        default: __LIBCALL4_err = -EDLLPARAMSIZE; break; \
    }                                       \
                                            \
    __LIBCALL4_err;                         \
})

#define __LIBCALL8(_f, _proto, _COUNT)  ({  \
    int __LIBCALL8_err = 0;                 \
                                            \
    switch(LIBPARAM(_proto, 0)->size) {     \
        case 4: (_proto)->result.u.u8 = LIBFUN(func_8_4, _f)(LIBPARAMx(_proto, _COUNT)); break; \
        case 8: (_proto)->result.u.u8 = LIBFUN(func_8_8, _f)(LIBPARAMx(_proto, _COUNT)); break; \
        default: __LIBCALL8_err = -EDLLPARAMSIZE; break; \
    }                                       \
                                            \
    __LIBCALL8_err;                         \
})

#define LIBCALLx(_f, _proto, _COUNT)    ({  \
    int __LIBCALLx_err = 0;                 \
                                            \
    switch((_proto)->result.size) {         \
        case 0: __LIBCALLx_err = __LIBCALL0(_f,_proto,_COUNT); break; \
        case 4: __LIBCALLx_err = __LIBCALL4(_f,_proto,_COUNT); break; \
        case 8: __LIBCALLx_err = __LIBCALL8(_f,_proto,_COUNT); break; \
        default:__LIBCALLx_err = -EDLLRESULTSIZE; break; \
    }                                       \
                                            \
    __LIBCALLx_err;                         \
}) /* end */

extern int
LIBCALLv(void *f, libproto_t *proto);

extern int
os_libcall(const char *lib, const char *sym, libproto_t *proto);
/******************************************************************************/
#endif /* __APP__ */
#endif /* __DLL_H_246c16b7624f4197910c79d8f0707b65__ */
