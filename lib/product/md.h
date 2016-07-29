#ifndef __MD_PRE_H_1cd88dc0a9124864acf00c06ad1ac3de__
#define __MD_PRE_H_1cd88dc0a9124864acf00c06ad1ac3de__
#if IS_PRODUCT_LTEFI_MD
/******************************************************************************/
#define PRODUCT_BLOCK_SIZE              512
#define PRODUCT_BOOT_SIZE               (512*1024)
#define PRODUCT_BOOTM_START             0x1000000
#define PRODUCT_BOOTCMD_BOOTM           "bootm " __SYMBOL_TO_STRING(PRODUCT_BOOTM_START)
#define PRODUCT_BOOTCMD_LOADK_BEGIN     "mmc read 0 " __SYMBOL_TO_STRING(PRODUCT_BOOTM_START) " "

#ifndef PRODUCT_BOOTARGS_BLOCK0_NAME
#define PRODUCT_BOOTARGS_BLOCK0_NAME    "mmcblk0"
#endif

#ifndef PRODUCT_DIR_ROOT
#define PRODUCT_DIR_ROOT                "/"
#endif

#ifndef PRODUCT_DEV_PREFIX
#define PRODUCT_DEV_PREFIX              "p"
#endif

#ifndef PRODUCT_DEV_FLASH_MASTER
#define PRODUCT_DEV_FLASH_MASTER        "dev/mmcblk0"
#endif

#ifndef PRODUCT_DEV_SD_MASTER
#define PRODUCT_DEV_SD_MASTER           "dev/mmcblk1"
#endif
/******************************************************************************/
#include "product/md/md1.h"
#include "product/md/md2.h"
#include "product/md/md3.h"
#include "product/md/md4.h"
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
* md used emmc(with mid/psn), so not use boot cookie
*/
#ifndef PRODUCT_BCOOKIE_ENABLE
#define PRODUCT_BCOOKIE_ENABLE          0
#endif
/******************************************************************************/
#endif
#endif /* __MD_PRE_H_1cd88dc0a9124864acf00c06ad1ac3de__ */
