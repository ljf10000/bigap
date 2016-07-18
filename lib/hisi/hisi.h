#ifndef __HISI_H_d8e4e7ba4ad94d61945610b31afade5d__
#define __HISI_H_d8e4e7ba4ad94d61945610b31afade5d__
/******************************************************************************/
#include "benv/benv.h"
/******************************************************************************/
#define __hisi_method_name(_class, _module, _method) "HI_" #_class "_" #_module"_" #_method
#define __hisi_method_func(_class, _module, _method) HI_##_class##_##_module##_##_method
#define __hisi_method_debug(_class, _module, _method, _err)    do{ \
    if (_err) {                        \
        debug_error(__hisi_method_name(_class, _module, _method) " error:%d", _err); \
    }                                   \
}while(0)

#define __hisi_call_0(_class, _module, _method)    ({  \
    int __hisi_call_0_err = __hisi_method_func(_class, _module, _method)(); \
    __hisi_method_debug(_class, _module, _method, __hisi_call_0_err); \
    __hisi_call_0_err;                            \
})  /* end */

#define __hisi_call_x(_class, _module, _method, _arg1, _args...) ({  \
    int __hisi_call_x_err = __hisi_method_func(_class, _module, _method)(_arg1, ##_args); \
    __hisi_method_debug(_class, _module, _method, __hisi_call_x_err); \
    __hisi_call_x_err;                                        \
})  /* end */

#define hisi_unf_call_0(_module, _method) \
     __hisi_call_0(UNF, _module, _method)
#define hisi_unf_call_x(_module, _method, _arg1, _args...) \
     __hisi_call_x(UNF, _module, _method, _arg1, ##_args)
/******************************************************************************/
#include "hisi/reg.h"
#include "hisi/dog.h"
#include "hisi/otp.h"
#include "hisi/i2c.h"
/******************************************************************************/
#endif /* __HISI_H_d8e4e7ba4ad94d61945610b31afade5d__ */
