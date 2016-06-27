#ifndef __PC_H_37e332f8b6b0423d87dadd8a98dec31e__
#define __PC_H_37e332f8b6b0423d87dadd8a98dec31e__
#if IS_PRODUCT_PC
/******************************************************************************/
#ifndef PRODUCT_DOG_COUNT
#define PRODUCT_DOG_COUNT           2
#endif

#ifndef PRODUCT_FLASH_SIZE
#define PRODUCT_FLASH_SIZE          2 /* 2G */
#endif

#ifndef PRODUCT_MEMORY_SIZE
#define PRODUCT_MEMORY_SIZE         2 /* 2G */
#endif

#ifndef PRODUCT_DIR_ROOT
#define PRODUCT_DIR_ROOT            __empty
#endif

#ifndef PRODUCT_DEV_BOOT
#define PRODUCT_DEV_BOOT            "boot"
#endif

#ifndef PRODUCT_DEV_BOOTENV
#define PRODUCT_DEV_BOOTENV         "bootenv"
#endif

#ifndef PRODUCT_FIRMWARE_COUNT
#define PRODUCT_FIRMWARE_COUNT      5
#endif

#ifndef PRODUCT_BOOTARGS_BLOCK0_NAME
#define PRODUCT_BOOTARGS_BLOCK0_NAME    "mmcblk0"
#endif
/******************************************************************************/
#endif
#endif /* __PC_H_37e332f8b6b0423d87dadd8a98dec31e__ */
