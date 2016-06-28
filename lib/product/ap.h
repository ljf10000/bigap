#ifndef __AP_H_b76f05a2ba014f61aed631e24b41d156__
#define __AP_H_b76f05a2ba014f61aed631e24b41d156__
#if IS_PRODUCT_LTEFI_AP
/******************************************************************************/
#ifndef PRODUCT_DIR_ROOT
#define PRODUCT_DIR_ROOT                "/"
#endif

#ifndef PRODUCT_BLOCK_SIZE
#define PRODUCT_BLOCK_SIZE              512
#endif

#ifndef PRODUCT_BOOT_SIZE
#define PRODUCT_BOOT_SIZE               (512*1024)
#endif

#ifndef PRODUCT_FIRMWARE_COUNT
#define PRODUCT_FIRMWARE_COUNT          1
#endif

#ifndef PRODUCT_FIRMWARE_CURRENT
#define PRODUCT_FIRMWARE_CURRENT        0
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
#endif
#endif /* __AP_H_b76f05a2ba014f61aed631e24b41d156__ */
