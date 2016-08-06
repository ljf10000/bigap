#ifndef __MD1_H_a2a2800b607e4f778a7661bbb363b5fb__
#define __MD1_H_a2a2800b607e4f778a7661bbb363b5fb__
#if IS_PRODUCT_LTEFI_MD1
/******************************************************************************/
#ifndef PRODUCT_MODEL
#define PRODUCT_MODEL                   PRODUCT_VENDOR "-md1" /* fixme */
#endif

#ifndef PRODUCT_VERSION
#define PRODUCT_VERSION                 PRODUCT_VENDOR "-md1" /* fixme */
#endif

#ifndef PRODUCT_PCBA_MODEL
#define PRODUCT_PCBA_MODEL              PRODUCT_VENDOR "-md1" /* fixme */
#endif

#ifndef PRODUCT_PCBA_VERSION
#define PRODUCT_PCBA_VERSION            PRODUCT_VENDOR "-md1" /* fixme */
#endif

#define PRODUCT_DOG_COUNT               2
#define PRODUCT_FLASH_SIZE              4 /* 4G */
#define PRODUCT_MEMORY_SIZE             2 /* 2G */
#define PRODUCT_FIRMWARE_COUNT          3
#define PRODUCT_FIRMWARE_CURRENT        1
#define PRODUCT_KERNEL_SIZE             8 /* 8M */
/*
* kernel0 begin(block count) =  13M/512 =  26K = 0x06800
*
* kernel  size (block count) = 8M/512 =  16K = 0x4000
*/
#define PRODUCT_KERNEL_BLKCOUNT         0x4000
#define PRODUCT_KERNEL_BLKSTART0        0x06800
#define PRODUCT_KERNEL_BLKSTART_DEFT    PRODUCT_KERNEL_BLKSTART0

#ifndef PRODUCT_ROOTFS_MODE
#define PRODUCT_ROOTFS_MODE             PRODUCT_ROOTFS_RW
#endif

#ifndef PRODUCT_DEV_USB_MASTER
#define PRODUCT_DEV_USB_MASTER          "dev/udisk1310"
#endif

#ifndef PRODUCT_DEV_HD_MASTER
#define PRODUCT_DEV_HD_MASTER           "dev/udisk"
#endif
/******************************************************************************/
#endif
#endif /* __MD1_H_a2a2800b607e4f778a7661bbb363b5fb__ */
