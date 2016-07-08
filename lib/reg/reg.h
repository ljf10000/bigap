#ifndef __REG_H_39a60c01c27c42d5aa2f5334d87d22fd__
#define __REG_H_39a60c01c27c42d5aa2f5334d87d22fd__
/******************************************************************************/
#include "product/product.h"
#if IS_PRODUCT_LTEFI_MD
/******************************************************************************/
#define REG_DEBUG    0

typedef struct {
    uint32_t addr;
    uint32_t value;
#if REG_DEBUG
    char name[16];
#endif
} os_reg_t;

#if REG_DEBUG
#define OS_REG(_name, _addr, _value)    { \
    .addr   = _addr,    \
    .value  = _value,   \
    .name   = _name,    \
}   /* end */
#else
#define OS_REG(_name, _addr, _value)    { \
    .addr   = _addr,    \
    .value  = _value,   \
}   /* end */
#endif
/*
* just for fastboot !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/
#if defined(__FASTBOOT__)

#if REG_DEBUG
#define io_read(_addr) ({ \
    uint32_t addr   = (_addr); \
    uint32_t value  = *(volatile uint32_t *)addr; \
    printf("read addr:0x%x, value:0x%x\n", addr, value); \
    value; \
})

#define io_write(_addr, _val) do{   \
    uint32_t addr   = (_addr);  \
    uint32_t value  = (_val);   \
                                    \
    *(volatile uint32_t *)addr = value; \
                                    \
    printf("write addr:0x%x, value:0x%x\n", addr, value); \
}while(0)
#else
#define io_read( _addr)         (*(volatile uint32_t *)(_addr))
#define io_write(_addr, _val)   (*(volatile uint32_t *)(_addr) = (_val))
#endif

static inline void
__os_reg_write(os_reg_t reg[], int count)
{
    int i;

    for (i=0; i<count; i++) {
        io_write(reg[i].addr, reg[i].value);
    }
}
#define os_reg_write(_reg)      __os_reg_write(_reg, sizeof(_reg)/sizeof(_reg[0]))
#endif

enum {
    __DOG_REG0  = 0xF8A2C000,
    __DOG_REG1  = 0xF8A2D000,
    __DOG_REG2  = 0xF8A2E000,

    __OTP_PRIVATER  = 0xF8AB02B0,
    __OTP_CUSTOMER  = 0xF8AB02C0,
};
#define __DOG_REGn(_dog)    __DOG_REG##_dog

enum {
    __DOG_LOAD      = 0x0000,
    __DOG_VALUE     = 0x0004,
    __DOG_CONTROL   = 0x0008,
    __DOG_INTCLR    = 0x000C,
    __DOG_RIS       = 0x0010,
    __DOG_MIS       = 0x0014,
    __DOG_LOCK      = 0x0C00,
};

#define __dog_reg(_dog, _reg)         (__DOG_REGn(_dog) + (_reg))

enum {
    __DOG_LOAD_DEFAULT      = (12000000*150),
    __DOG_LOCK_OPEN         = 0x1ACCE551,
    __DOG_LOCK_CLOSE        = 0,
    __DOG_CONTROL_ENABLE    = 0x3,
    __DOG_CONTROL_DISABLE   = 0,
};

#define __OS_DOG_ENABLE(_dog)   \
    OS_REG("dog lock",      __dog_reg(_dog, __DOG_LOCK),     __DOG_LOCK_OPEN), \
    OS_REG("dog load",      __dog_reg(_dog, __DOG_LOAD),     __DOG_LOAD_DEFAULT), \
    OS_REG("dog control",   __dog_reg(_dog, __DOG_CONTROL),  __DOG_CONTROL_ENABLE), \
    OS_REG("dog lock",      __dog_reg(_dog, __DOG_LOCK),     __DOG_LOCK_CLOSE) \
    /* end */

#if IS_PRODUCT_LTEFI_MD_CPU_A
#define OS_DOG_ENABLE { \
    __OS_DOG_ENABLE(0), \
    __OS_DOG_ENABLE(1), \
}   /* end */
#elif IS_PRODUCT_LTEFI_MD_CPU_B
#define OS_DOG_ENABLE { \
    __OS_DOG_ENABLE(0), \
    __OS_DOG_ENABLE(1), \
    __OS_DOG_ENABLE(2), \
}   /* end */
#else
#error "invalid __PRODUCT__"
#endif
/******************************************************************************/
#endif /* IS_PRODUCT_LTEFI_MD */
#endif /* __REG_H_39a60c01c27c42d5aa2f5334d87d22fd__ */
