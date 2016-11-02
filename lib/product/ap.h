#ifndef __AP_PRE_H_b76f05a2ba014f61aed631e24b41d156__
#define __AP_PRE_H_b76f05a2ba014f61aed631e24b41d156__
#if IS_PRODUCT_LTEFI_AP
/******************************************************************************/
#define PRODUCT_BLOCK_SIZE              256
#define PRODUCT_BOOT_SIZE               (256*1024)
#define PRODUCT_BOOTM_START             0x9f050000
#define PRODUCT_BOOTCMD_BOOTM           "bootm " __SYMBOL_TO_STRING(PRODUCT_BOOTM_START)
#define PRODUCT_BOOTCMD_LOADK_BEGIN     "bootm " __SYMBOL_TO_STRING(PRODUCT_BOOTM_START) " "

#ifndef PRODUCT_BOOTARGS_BLOCK0_NAME
#define PRODUCT_BOOTARGS_BLOCK0_NAME    "spi0.0"
#endif

#ifndef PRODUCT_DIR_ROOT
#define PRODUCT_DIR_ROOT                "/"
#endif

#ifndef PRODUCT_DEV_PREFIX
#define PRODUCT_DEV_PREFIX              "p"/* fixme */
#endif

#ifndef PRODUCT_DEV_FLASH_MASTER
#define PRODUCT_DEV_FLASH_MASTER        "dev/fixme"
#endif

#ifndef PRODUCT_DEV_SD_MASTER
#define PRODUCT_DEV_SD_MASTER           "dev/fixme"
#endif

#ifndef PRODUCT_BOOTENV_BACKUP
#define PRODUCT_BOOTENV_BACKUP         "config/.backup/.bootenv"
#endif
/******************************************************************************/
#include "product/ap/ap1.h"
#include "product/ap/ap2.h"
#include "product/ap/ap3.h"
#include "product/ap/ap4.h"
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
