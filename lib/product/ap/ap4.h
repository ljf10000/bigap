#ifndef __AP4_H_611eae9f1d6647b3916daca4050b7e08__
#define __AP4_H_611eae9f1d6647b3916daca4050b7e08__
#if IS_PRODUCT_LTEFI_AP4
/******************************************************************************/
#ifndef PRODUCT_MODEL
#define PRODUCT_MODEL                   "WeOct24"
#endif

#ifndef PRODUCT_VERSION
#define PRODUCT_VERSION                 "H1" /* fixme */
#endif

#ifndef PRODUCT_PCBA_MODEL
#define PRODUCT_PCBA_MODEL              "AP152_v0.1"  /* fixme */
#endif

#ifndef PRODUCT_PCBA_VERSION
#define PRODUCT_PCBA_VERSION            "0.1.0.1"
#endif

#define PRODUCT_DOG_COUNT               1
#define PRODUCT_FLASH_SIZE              16  /* 16M */
#define PRODUCT_MEMORY_SIZE             128 /* 128M */
#define PRODUCT_FIRMWARE_COUNT          7
#define PRODUCT_FIRMWARE_CURRENT        0   /* rootfs0 */
#define PRODUCT_KERNEL_SIZE             16  /* 16M */
#define PRODUCT_UPGRADE_COUNT           1
/*
* kernel0 begin(block count) =  320K/64 =   5K =  0x50000
*
* kernel  size (block count) = 1472K/64 =  23K = 0x190000
*/
#define PRODUCT_KERNEL_BLKCOUNT         0x190000
#define PRODUCT_KERNEL_BLKSTART0        0x50000
#define PRODUCT_KERNEL_BLKSTART1        0x50000
#define PRODUCT_KERNEL_BLKSTART2        0x50000
#define PRODUCT_KERNEL_BLKSTART3        0x50000
#define PRODUCT_KERNEL_BLKSTART4        0x50000
#define PRODUCT_KERNEL_BLKSTART5        0x50000
#define PRODUCT_KERNEL_BLKSTART6        0x50000
#define PRODUCT_KERNEL_BLKSTART_DEFT    PRODUCT_KERNEL_BLKSTART1

#ifndef PRODUCT_ROOTFS_MODE
#define PRODUCT_ROOTFS_MODE             PRODUCT_ROOTFS_RO
#endif

#ifndef PRODUCT_DEV_USB_MASTER
#define PRODUCT_DEV_USB_MASTER          "dev/udisk51210" /* fixme */
#endif

#ifndef PRODUCT_DEV_HD_MASTER
#define PRODUCT_DEV_HD_MASTER           "dev/udisk61110" /* fixme */
#endif

#define PRODUCT_DEV_KERNEL0     PRODUCT_IDEV_FLASH(2)
#define PRODUCT_DEV_KERNEL1     PRODUCT_IDEV_FLASH(2)
#define PRODUCT_DEV_KERNEL2     PRODUCT_IDEV_FLASH(2)
#define PRODUCT_DEV_KERNEL3     PRODUCT_IDEV_FLASH(2)
#define PRODUCT_DEV_KERNEL4     PRODUCT_IDEV_FLASH(2)
#define PRODUCT_DEV_KERNEL5     PRODUCT_IDEV_FLASH(2)
#define PRODUCT_DEV_KERNEL6     PRODUCT_IDEV_FLASH(2)

#define PRODUCT_DEV_ROOTFS0     PRODUCT_IDEV_FLASH(3)
#define PRODUCT_DEV_ROOTFS1     PRODUCT_IDEV_FLASH(3)
#define PRODUCT_DEV_ROOTFS2     PRODUCT_IDEV_FLASH(3)
#define PRODUCT_DEV_ROOTFS3     PRODUCT_IDEV_FLASH(3)
#define PRODUCT_DEV_ROOTFS4     PRODUCT_IDEV_FLASH(3)
#define PRODUCT_DEV_ROOTFS5     PRODUCT_IDEV_FLASH(3)
#define PRODUCT_DEV_ROOTFS6     PRODUCT_IDEV_FLASH(3)

#define PRODUCT_DEV_CONFIG0     PRODUCT_IDEV_FLASH(4)
#define PRODUCT_DEV_CONFIG1     PRODUCT_IDEV_FLASH(4)

#define PRODUCT_DEV_ART		PRODUCT_IDEV_FLASH(5)

#define PRODUCT_DEV_ROOT        PRODUCT_IDEV_FLASH(3) /* rootfs0 */

#define PRODUCT_BOOTARGS_BLOCK0 \
        "256K(u-boot)," /* 01 */\
        "64K(u-boot-env),"   /* 02 */\
        "1472K(kernel0)," /* 03 */\
        "14272K(rootfs0),"/* 04 */\
        "256K(config0),"  /* 05 */\
        "64K(art)"       /* 06 */\
        /* end */
/******************************************************************************/
#endif
#endif /* __AP4_H_611eae9f1d6647b3916daca4050b7e08__ */
