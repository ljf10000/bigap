#ifndef __DOG_H_e9817fb191904b3d906b62af9232e8e3__
#define __DOG_H_e9817fb191904b3d906b62af9232e8e3__
/******************************************************************************/
#ifdef __APP__
/******************************************************************************/
#if IS_PRODUCT_LTEFI_MD
#include "hi_unf_wdg.h"

#if 0
HI_S32 HI_UNF_WDG_Init(HI_VOID);
HI_S32 HI_UNF_WDG_DeInit(HI_VOID);
HI_S32 HI_UNF_WDG_Enable(HI_U32 u32WdgNum);
HI_S32 HI_UNF_WDG_Disable(HI_U32 u32WdgNum);
HI_S32 HI_UNF_WDG_GetTimeout(HI_U32 u32WdgNum, HI_U32 *pu32Value);
HI_S32 HI_UNF_WDG_SetTimeout(HI_U32 u32WdgNum, HI_U32 u32Value);
HI_S32 HI_UNF_WDG_Clear(HI_U32 u32WdgNum);
HI_S32 HI_UNF_WDG_Reset(HI_U32 u32WdgNum);
#endif

#define hisi_dog_init()     hisi_unf_method_0(WDG, Init)
#define hisi_dog_fini()     hisi_unf_method_0(WDG, DeInit)

#define hisi_dog_enable(dog_number)     hisi_unf_method_1(WDG, Enable, dog_number)
#define hisi_dog_disable(dog_number)    hisi_unf_method_1(WDG, Disable, dog_number)

#define hisi_dog_clear(dog_number)      hisi_unf_method_1(WDG, Clear, dog_number)
#define hisi_dog_reset(dog_number)      hisi_unf_method_1(WDG, Reset, dog_number)

#define hisi_dog_get_timeout(dog_number, dog_value_pointer) \
    hisi_unf_method_x(WDG, GetTimeout, dog_number, dog_value_pointer)
#define hisi_dog_set_timeout(dog_number, dog_value) \
    hisi_unf_method_x(WDG, SetTimeout, dog_number, dog_value)

#define hisi_dog_foreach(_method, _args...)  ({ \
    int __i, __err = 0;                 \
                                        \
    for (__i=0; __i<PRODUCT_DOG_COUNT; __i++) { \
        __err = _method(__i, ##_args);  \
        if (__err) {                    \
            break;                      \
        }                               \
    }                                   \
                                        \
    __err;                              \
})
#endif
/******************************************************************************/
#endif /* __APP__ */
#endif /* __DOG_H_e9817fb191904b3d906b62af9232e8e3__ */
