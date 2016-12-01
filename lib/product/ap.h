#ifndef __AP_PRE_H_b76f05a2ba014f61aed631e24b41d156__
#define __AP_PRE_H_b76f05a2ba014f61aed631e24b41d156__
#if IS_PRODUCT_LTEFI_AP
/******************************************************************************/
#include "product/ap/ap1.h"
#include "product/ap/ap2.h"
#include "product/ap/ap3.h"
#include "product/ap/ap4.h"
/******************************************************************************/
#ifndef PRODUCT_BLOCK_SIZE
#define PRODUCT_BLOCK_SIZE              512
#endif

#ifndef PRODUCT_BOOT_SIZE
#define PRODUCT_BOOT_SIZE               (256*1024)
#endif

#ifndef PRODUCT_FLASH_SIZE
#define PRODUCT_FLASH_SIZE              16  /* 16M */
#endif

#ifndef PRODUCT_MEMORY_SIZE
#define PRODUCT_MEMORY_SIZE             128 /* 128M */
#endif

#ifndef PRODUCT_ROOTFS_MODE
#define PRODUCT_ROOTFS_MODE             PRODUCT_ROOTFS_RW
#endif

#ifndef PRODUCT_FIRMWARE_COUNT
#define PRODUCT_FIRMWARE_COUNT          7
#endif

#ifndef PRODUCT_FIRMWARE_CURRENT
#define PRODUCT_FIRMWARE_CURRENT        0
#endif

#ifndef PRODUCT_UPGRADE_COUNT
#define PRODUCT_UPGRADE_COUNT           1
#endif

#define PRODUCT_BOOTM_START             0x9f050000
#define PRODUCT_BOOTCMD_BOOTM           "bootm " __SYMBOL_TO_STRING(PRODUCT_BOOTM_START)
#define PRODUCT_BOOTCMD_LOADK_BEGIN     "bootm " __SYMBOL_TO_STRING(PRODUCT_BOOTM_START) " "

#ifndef PRODUCT_BOOTARGS_BLOCK0_NAME
#define PRODUCT_BOOTARGS_BLOCK0_NAME    "spi0.0"
#endif

#ifndef PRODUCT_DIR_ROOT
#define PRODUCT_DIR_ROOT                ""
#endif

#ifndef PRODUCT_DEV_PREFIX
#define PRODUCT_DEV_PREFIX              ""
#endif

#ifndef PRODUCT_DEV_FLASH_MASTER
#define PRODUCT_DEV_FLASH_MASTER        "/dev/mtdblock"
#endif

#ifndef PRODUCT_DEV_SD_MASTER
#define PRODUCT_DEV_SD_MASTER           "dev/fixme"
#endif

#ifndef PRODUCT_DEV_HD_MASTER
#define PRODUCT_DEV_HD_MASTER           "dev/fixme"
#endif

#ifndef PRODUCT_DEV_USB_MASTER
#define PRODUCT_DEV_USB_MASTER          "dev/fixme"
#endif

#ifndef PRODUCT_BOOTENV_BACKUP
#define PRODUCT_BOOTENV_BACKUP         "config/.backup/.bootenv"
#endif
/******************************************************************************/
#define PRODUCT_BOOTCMD_LOADK_END       " " __SYMBOL_TO_STRING(PRODUCT_KERNEL_BLKCOUNT)
#define PRODUCT_BOOTCMD_LOADK           \
        PRODUCT_BOOTCMD_LOADK_BEGIN     \
        __SYMBOL_TO_STRING(PRODUCT_KERNEL_BLKSTART_DEFT) \
        PRODUCT_BOOTCMD_LOADK_END       \
        /* end */
#define PRODUCT_BOOTCOMMAND             \
        PRODUCT_BOOTCMD_LOADK       ";" \
        PRODUCT_BOOTCMD_BOOTM       ";" \
        /* end */

/*
* ap not use emmc(with mid/psn), so use boot cookie
*/
#ifndef PRODUCT_BCOOKIE_ENABLE
#define PRODUCT_BCOOKIE_ENABLE          1
#endif

#endif
#endif /* __AP_PRE_H_b76f05a2ba014f61aed631e24b41d156__ */
