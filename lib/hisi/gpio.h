#ifndef __GPIO_H_4b6ed674420e47bd924bf348f1c637ba__
#define __GPIO_H_4b6ed674420e47bd924bf348f1c637ba__
/******************************************************************************/
#ifdef __APP__
/******************************************************************************/
#if IS_PRODUCT_LTEFI_MD
#include "hi_unf_gpio.h"
#endif

static inline int hisi_gpio_init(void)   { return hisi_unf_call_0(GPIO, Init); }
static inline int hisi_gpio_fini(void)   { return hisi_unf_call_0(GPIO, DeInit); }

#define hisi_gpio_read_bit(gpio_number, gpio_high_volt_pointer) \
    hisi_unf_call_x(GPIO, ReadBit, gpio_number, gpio_high_volt_pointer)
#define hisi_gpio_write_bit(gpio_number, gpio_high_volt) \
    hisi_unf_call_x(GPIO, WriteBit, gpio_number, gpio_high_volt)

#define hisi_gpio_get_dir_bit(gpio_number, gpio_dir_pointer) \
    hisi_unf_call_x(GPIO, GetDirBit, gpio_number, gpio_dir_pointer)
#define hisi_gpio_set_dir_bit(gpio_number, gpio_dir) \
    hisi_unf_call_x(GPIO, SetDirBit, gpio_number, gpio_dir)

#define hisi_gpio_get_output_type(gpio_number, gpio_output_type_pointer) \
    hisi_unf_call_x(GPIO, GetOutputType, gpio_number, gpio_output_type_pointer)
#define hisi_gpio_set_output_type(gpio_number, gpio_output_type) \
    hisi_unf_call_x(GPIO, SetOutputType, gpio_number, gpio_output_type)

#define hisi_gpio_query_int(gpio_number_pointer, ms) \
    hisi_unf_call_x(GPIO, QueryInt, gpio_number_pointer, ms)
#define hisi_gpio_set_int_type(gpio_number, type) \
    hisi_unf_call_x(GPIO, SetIntType, gpio_number, type)
#define hisi_gpio_set_int_enable(gpio_number, enable) \
    hisi_unf_call_x(GPIO, SetIntEnable, gpio_number, enable)
/******************************************************************************/
#endif /* __APP__ */
#endif /* __GPIO_H_4b6ed674420e47bd924bf348f1c637ba__ */
