#ifndef __MD3_H_a6505c1ba56b48df9e21eb182d06c773__
#define __MD3_H_a6505c1ba56b48df9e21eb182d06c773__
#if IS_PRODUCT_LTEFI_MD3
/******************************************************************************/
#ifndef PRODUCT_MODEL
#define PRODUCT_MODEL               PRODUCT_VENDOR "-LV3" /* fixme */
#endif

#ifndef PRODUCT_PCBA_MODEL
#define PRODUCT_PCBA_MODEL          PRODUCT_VENDOR "-pcba" /* fixme */
#endif

#ifndef PRODUCT_DOG_COUNT
#define PRODUCT_DOG_COUNT           3
#endif

#ifndef PRODUCT_FLASH_SIZE
#define PRODUCT_FLASH_SIZE          8 /* 8G */
#endif

#ifndef PRODUCT_MEMORY_SIZE
#define PRODUCT_MEMORY_SIZE         2 /* 2G */
#endif

#ifndef PRODUCT_ROOTFS_MODE
#define PRODUCT_ROOTFS_MODE         PRODUCT_ROOTFS_RO
#endif

#ifndef PRODUCT_DEV_USB_MASTER
#define PRODUCT_DEV_USB_MASTER      "dev/udisk1110" /* fixme */
#endif
/******************************************************************************/
#endif
#endif /* __MD3_H_a6505c1ba56b48df9e21eb182d06c773__ */
