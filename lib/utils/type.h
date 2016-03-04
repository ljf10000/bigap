#ifndef __TYPE_H_65c53c7cd7364495b1d962dec632cfdb__
#define __TYPE_H_65c53c7cd7364495b1d962dec632cfdb__
/******************************************************************************/
#if 1 /* just for sourceinsight */
#ifndef int8_t
#define int8_t          int8_t
#endif

#ifndef uint8_t
#define uint8_t         uint8_t
#endif

#ifndef int16_t
#define int16_t         int16_t
#endif

#ifndef uint16_t
#define uint16_t        uint16_t
#endif

#ifndef int32_t
#define int32_t         int32_t
#endif

#ifndef uint32_t
#define uint32_t        uint32_t
#endif

#ifndef float32_t
#define float32_t       float
#endif

#ifndef int64_t
#define int64_t         int64_t
#endif

#ifndef uint64_t
#define uint64_t        uint64_t
#endif

#ifndef float64_t
#define float64_t       double
#endif
#endif /* just for sourceinsight */

#ifdef  bool
#undef  bool
#endif
#define bool            uint32_t
#define __bool(_x)      ((bool)!!(_x))

#ifdef  true
#undef  true
#endif
#define true            1

#ifdef  false
#undef  false
#endif
#define false           0

#ifndef byte
#define byte            uint8_t
#endif

enum {
    OS_DISABLE          = 0,
    OS_ENABLE           = 1,
};

enum {
    OS_OFF              = 0,
    OS_ON               = 1,
};

enum {
    OS_OK               = 0,
    OS_FAIL             = 1,
};

enum {
    __stdin             = 0,
    __stdout            = 1,
    __stderr            = 2,
};

enum {
    __MV_GO             = 0,
    __MV_BREAK          = 1,
};

typedef int32_t mv_t;

typedef union {
    mv_t v;

    struct {
        int32_t error:24;
        int32_t control:8;
    } v2;
    
    struct {
        int32_t error:16;
        int32_t control:8;
        int32_t private:8;
    } v3;
    
    struct {
        int32_t error:8;
        int32_t control:8;
        int32_t private:8;
        int32_t value:8;
    } v4;
}
mv_u;

#define MV_INITER               { .v = 0 }

#define mv2_error(_mv)          (_mv).v2.error
#define mv2_control(_mv)        (_mv).v2.control
#define __mv2_INITER(_control, _error)  { \
    .v2 = {                 \
        .error  = _error,   \
        .control= _control, \
    }                       \
}

static inline mv_t 
__mv2_return(int control, int error)
{
    mv_u mv = __mv2_INITER(control, error);

    return mv.v;
}

#define mv2_break(_result)      __mv2_return(__MV_BREAK, _result)
#define mv2_go(_result)         __mv2_return(__MV_GO, _result)
enum {
    mv2_ok = 0,
};

#define is_mv2_break(_mv)       (__MV_BREAK==mv2_control(_mv))
#define is_mv2_go(_mv)          (__MV_GO==mv2_control(_mv))
/******************************************************************************/
#endif /* __TYPE_H_65c53c7cd7364495b1d962dec632cfdb__ */
