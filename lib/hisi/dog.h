#ifndef __DOG_H_e9817fb191904b3d906b62af9232e8e3__
#define __DOG_H_e9817fb191904b3d906b62af9232e8e3__
/******************************************************************************/
#ifdef __APP__
/******************************************************************************/
#if IS_PRODUCT_LTEFI_MD
#include "hi_unf_wdg.h"
#endif

static inline int hisi_dog_init(void)   { return hisi_unf_call_0(WDG, Init); }
static inline int hisi_dog_fini(void)   { return hisi_unf_call_0(WDG, DeInit); }

#define hisi_dog_enable(dog_number)     hisi_unf_call_x(WDG, Enable, dog_number)
#define hisi_dog_disable(dog_number)    hisi_unf_call_x(WDG, Disable, dog_number)

#define hisi_dog_clear(dog_number)      hisi_unf_call_x(WDG, Clear, dog_number)
#define hisi_dog_reset(dog_number)      hisi_unf_call_x(WDG, Reset, dog_number)

#define hisi_dog_get_timeout(dog_number, dog_value_pointer) \
    hisi_unf_call_x(WDG, GetTimeout, dog_number, dog_value_pointer)
#define hisi_dog_set_timeout(dog_number, dog_value) \
    hisi_unf_call_x(WDG, SetTimeout, dog_number, dog_value)

#define hisi_dog_get_capability(dog_number_pointer) \
    hisi_unf_call_x(WDG, GetCapability, dog_number_pointer)

#define hisi_dog_foreach(_method, _args...)  ({         \
    int hisi_dog_foreach_i, hisi_dog_foreach_err = 0;   \
                                                        \
    for (hisi_dog_foreach_i=0; hisi_dog_foreach_i<PRODUCT_DOG_COUNT; hisi_dog_foreach_i++) { \
        hisi_dog_foreach_err = _method(hisi_dog_foreach_i, ##_args);  \
        if (hisi_dog_foreach_err) {                     \
            break;                                      \
        }                                               \
    }                                                   \
                                                        \
    hisi_dog_foreach_err;                               \
})
/******************************************************************************/
#endif
#endif /* __DOG_H_e9817fb191904b3d906b62af9232e8e3__ */
