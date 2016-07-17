#ifndef __HISI_H_d8e4e7ba4ad94d61945610b31afade5d__
#define __HISI_H_d8e4e7ba4ad94d61945610b31afade5d__
/******************************************************************************/
#include "benv/benv.h"
/******************************************************************************/
#define __hisi_method_name(_mod, _obj, _method) "hisi " #_mod " " #_obj" " #_method
#define __hisi_method_func(_mod, _obj, _method) HI_##_mod##_##_obj##_##_method
#define __hisi_method_err(_mod, _obj, _method, _err)    ({ \
    if (_err) {                        \
        debug_error(__hisi_method_name(_mod, _obj, _method) " error:%d", _err); \
    }                                   \
                                        \
    _err;                               \
})

#define __hisi_method_0(_mod, _obj, _method)    ({  \
    int __err = __hisi_method_func(_mod, _obj, _method)(); \
    __hisi_method_err(_mod, _obj, _method, __err); \
})  /* end */

#define __hisi_method_1(_mod, _obj, _method, _arg1) ({  \
    int __err = __hisi_method_func(_mod, _obj, _method)(_arg1); \
    __hisi_method_err(_mod, _obj, _method, __err); \
})  /* end */

#define __hisi_method_x(_mod, _obj, _method, _arg1, _args...) ({  \
    int __err = __hisi_method_func(_mod, _obj, _method)(_arg1, ##_args); \
    __hisi_method_err(_mod, _obj, _method, __err); \
})  /* end */

#define hisi_unf_method_0(_obj, _method) \
     __hisi_method_0(UNF, _obj, _method)
#define hisi_unf_method_1(_obj, _method, _arg1) \
     __hisi_method_1(UNF, _obj, _method, _arg1)
#define hisi_unf_method_x(_obj, _method, _arg1, _args...) \
     __hisi_method_x(UNF, _obj, _method, _arg1, ##_args)
/******************************************************************************/
#include "hisi/reg.h"
#include "hisi/dog.h"
#include "hisi/otp.h"
#include "hisi/i2c.h"
/******************************************************************************/
#endif /* __HISI_H_d8e4e7ba4ad94d61945610b31afade5d__ */
