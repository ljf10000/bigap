#ifndef __I2C_H_3cdd1f6874ff48afb679ccdf4053853f__
#define __I2C_H_3cdd1f6874ff48afb679ccdf4053853f__
/******************************************************************************/
#ifdef __APP__
/******************************************************************************/
#if IS_PRODUCT_LTEFI_MD
#include "hi_unf_i2c.h"
#endif

static inline int hisi_i2c_init(void)   { return hisi_unf_call_0(I2C, Init); }
static inline int hisi_i2c_fini(void)   { return hisi_unf_call_0(I2C, DeInit); }

static inline int hisi_i2c_read(i2c_number, dev_addr, reg_addr, reg_addr_count, buf, size) \
    hisi_unf_call_x(I2C, Read, i2c_number, dev_addr, reg_addr, reg_addr_count, buf, size)
#define hisi_i2c_write(i2c_number, dev_addr, reg_addr, reg_addr_count, buf, size) \
    hisi_unf_call_x(I2C, Write, i2c_number, dev_addr, reg_addr, reg_addr_count, buf, size)

#define hisi_i2c_set_rate(i2c_number, i2c_rate) \
    hisi_unf_call_x(I2C, SetRate, i2c_number, i2c_rate)
#define hisi_i2c_set_rate_ex(i2c_number, i2c_rate) \
    hisi_unf_call_x(I2C, SetRateEx, i2c_number, i2c_rate)

#define hisi_i2c_create_gpio(i2c_number, scl_gpio_number, sda_gpio_number) \
    hisi_unf_call_x(I2C, CreateGpioI2c, i2c_number, scl_gpio_number, sda_gpio_number)
#define hisi_i2c_destroy_gpio(i2c_number) \
    hisi_unf_call_x(I2C, DestroyGpioI2c, i2c_number)

#define hisi_i2c_get_capability(i2c_number_pointer) \
    hisi_unf_call_x(I2C, GetCapability, i2c_number_pointer)
/******************************************************************************/
#endif /* __APP__ */
#endif /* __I2C_H_3cdd1f6874ff48afb679ccdf4053853f__ */
