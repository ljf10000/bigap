#ifndef __MD_H_1cd88dc0a9124864acf00c06ad1ac3de__
#define __MD_H_1cd88dc0a9124864acf00c06ad1ac3de__
#include "product/md/md1.h"
#include "product/md/md2.h"
#include "product/md/md3.h"
#if IS_PRODUCT_LTEFI_MD
/******************************************************************************/
#ifndef PRODUCT_DIR_ROOT
#define PRODUCT_DIR_ROOT                "/"
#endif

#ifndef PRODUCT_FIRMWARE_COUNT
#if 1==PRODUCT_FLASH_SIZE   /* 1G */
#   define PRODUCT_FIRMWARE_COUNT       3
#elif 2==PRODUCT_FLASH_SIZE /* 2G */
#   define PRODUCT_FIRMWARE_COUNT       5
#elif 4==PRODUCT_FLASH_SIZE || /* 4G */ \
      8==PRODUCT_FLASH_SIZE /* 8G */
#   define PRODUCT_FIRMWARE_COUNT       7
#endif /* PRODUCT_FLASH_SIZE */
#endif /* PRODUCT_FIRMWARE_COUNT */


#ifndef PRODUCT_BOOTARGS_BLOCK0_NAME
#define PRODUCT_BOOTARGS_BLOCK0_NAME    "mmcblk0"
#endif

#ifndef PRODUCT_FIRMWARE_CURRENT
#define PRODUCT_FIRMWARE_CURRENT        1
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

#ifndef PRODUCT_DEV_HD_MASTER
#define PRODUCT_DEV_HD_MASTER           "dev/udisk"
#endif
/******************************************************************************/
#endif
#endif /* __MD_H_1cd88dc0a9124864acf00c06ad1ac3de__ */
