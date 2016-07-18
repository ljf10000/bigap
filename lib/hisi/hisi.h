#ifndef __HISI_H_d8e4e7ba4ad94d61945610b31afade5d__
#define __HISI_H_d8e4e7ba4ad94d61945610b31afade5d__
/******************************************************************************/
#include "benv/benv.h"
/******************************************************************************/
#define __hisi_class_module_method_name(_class, _module, _method) "HI_" #_class "_" #_module"_" #_method
#define __hisi_class_module_method_func(_class, _module, _method) HI_##_class##_##_module##_##_method
#define __hisi_class_module_method_error(_class, _module, _method, _err)    ({ \
    if (_err) {                        \
        debug_error(__hisi_class_module_method_name(_class, _module, _method) " error:%d", _err); \
    }                                   \
                                        \
    (_err)<0?(_err):-(_err);            \
})

#define __hisi_class_module_call_0(_class, _module, _method)    ({  \
    int __hisi_class_module_call_0_err = __hisi_class_module_method_func(_class, _module, _method)(); \
    __hisi_class_module_method_error(_class, _module, _method, __hisi_class_module_call_0_err); \
})  /* end */

#define __hisi_class_module_call_x(_class, _module, _method, _arg1, _args...) ({  \
    int __hisi_class_module_call_x_err = __hisi_class_module_method_func(_class, _module, _method)(_arg1, ##_args); \
    __hisi_class_module_method_error(_class, _module, _method, __hisi_class_module_call_x_err); \
})  /* end */
/******************************************************************************/
#define __hisi_module_method_name(_module, _method) "HI_" #_module"_" #_method
#define __hisi_module_method_func(_module, _method) HI_##_module##_##_method
#define __hisi_module_method_error(_module, _method, _err)      ({ \
    if (_err) {                        \
        debug_error(__hisi_module_method_name(_module, _method) " error:%d", _err); \
    }                                   \
                                        \
    (_err)<0?(_err):-(_err);            \
})

#define __hisi_module_call_0(_module, _method)    ({  \
    int __hisi_module_call_0_err = __hisi_module_method_func(_module, _method)(); \
    __hisi_module_method_error(_module, _method, __hisi_module_call_0_err); \
})  /* end */

#define __hisi_module_call_x(_module, _method, _arg1, _args...) ({  \
    int __hisi_module_call_x_err = __hisi_module_method_func(_module, _method)(_arg1, ##_args); \
    __hisi_module_method_error(_module, _method, __hisi_module_call_x_err); \
})  /* end */
/******************************************************************************/
#define hisi_unf_call_0(_module, _method) \
     __hisi_class_module_call_0(UNF, _module, _method)
#define hisi_unf_call_x(_module, _method, _arg1, _args...) \
     __hisi_class_module_call_x(UNF, _module, _method, _arg1, ##_args)

#define hisi_module_call_0(_module, _method) \
     __hisi_module_call_0(_module, _method)
#define hisi_module_call_x(_module, _method, _arg1, _args...) \
     __hisi_module_call_x(_module, _method, _arg1, ##_args)
/******************************************************************************/
#include "hisi/reg.h"
#include "hisi/dog.h"
#include "hisi/otp.h"
#include "hisi/i2c.h"
#include "hisi/3g.h"
/******************************************************************************/
static inline int
hisi_init(void)
{
    int err;
    
    err = os_init();
    if (err<0) {
        return err;
    }

    err = hisi_sys_init();
    if (err<0) {
        return err;
    }

    return 0;
}

static inline int
hisi_fini(void)
{
    hisi_sys_fini();
    
    os_fini();
}
/******************************************************************************/
#endif /* __HISI_H_d8e4e7ba4ad94d61945610b31afade5d__ */
