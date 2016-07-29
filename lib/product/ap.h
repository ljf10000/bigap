#ifndef __AP_PRE_H_b76f05a2ba014f61aed631e24b41d156__
#define __AP_PRE_H_b76f05a2ba014f61aed631e24b41d156__
#if IS_PRODUCT_LTEFI_AP
/******************************************************************************/
#ifndef PRODUCT_BLOCK_SIZE
#define PRODUCT_BLOCK_SIZE              512
#endif

#ifndef PRODUCT_BOOT_SIZE
#define PRODUCT_BOOT_SIZE               (512*1024)
#endif

#ifndef PRODUCT_FLASH_SIZE
#define PRODUCT_FLASH_SIZE              4 /* 4G */
#endif

#ifndef PRODUCT_MEMORY_SIZE
#define PRODUCT_MEMORY_SIZE             2 /* 2G */
#endif

#ifndef PRODUCT_ROOTFS_MODE
#define PRODUCT_ROOTFS_MODE             PRODUCT_ROOTFS_RW
#endif

#ifndef PRODUCT_FIRMWARE_COUNT
#define PRODUCT_FIRMWARE_COUNT          1
#endif

#ifndef PRODUCT_FIRMWARE_CURRENT
#define PRODUCT_FIRMWARE_CURRENT        0
#endif

#ifndef PRODUCT_DIR_ROOT
#define PRODUCT_DIR_ROOT                "/"
#endif

#ifndef PRODUCT_DEV_PREFIX
#define PRODUCT_DEV_PREFIX              /* fixme */
#endif

#ifndef PRODUCT_DEV_FLASH_MASTER
#define PRODUCT_DEV_FLASH_MASTER        "dev/fixme"
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
/******************************************************************************/
#include "product/ap/ap1.h"
#include "product/ap/ap2.h"
#include "product/ap/ap3.h"
#include "product/ap/ap4.h"
/******************************************************************************/

/*
* ap not use emmc(with mid/psn), so use boot cookie
*/
#ifndef PRODUCT_BCOOKIE_ENABLE
#define PRODUCT_BCOOKIE_ENABLE          1
#endif

#endif
#endif /* __AP_PRE_H_b76f05a2ba014f61aed631e24b41d156__ */
