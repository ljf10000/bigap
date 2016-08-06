#ifndef __PC_H_37e332f8b6b0423d87dadd8a98dec31e__
#define __PC_H_37e332f8b6b0423d87dadd8a98dec31e__
#if IS_PRODUCT_PC
/******************************************************************************/
#ifndef PRODUCT_LTEFI_VERSION
#define PRODUCT_LTEFI_VERSION           3
#endif

#ifndef PRODUCT_BCOOKIE_ENABLE
#define PRODUCT_BCOOKIE_ENABLE          1
#endif

#ifndef PRODUCT_DOG_COUNT
#define PRODUCT_DOG_COUNT               2
#endif

#ifndef PRODUCT_MODEL
#define PRODUCT_MODEL                   PRODUCT_VENDOR "-pc" /* fixme */
#endif

#ifndef PRODUCT_VERSION
#define PRODUCT_VERSION                 PRODUCT_VENDOR "-pc" /* fixme */
#endif

#ifndef PRODUCT_PCBA_MODEL
#define PRODUCT_PCBA_MODEL              PRODUCT_VENDOR "-pc" /* fixme */
#endif

#ifndef PRODUCT_PCBA_VERSION
#define PRODUCT_PCBA_VERSION            PRODUCT_VENDOR "-pc" /* fixme */
#endif

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

#ifndef PRODUCT_DEV_USB_MASTER
#define PRODUCT_DEV_USB_MASTER          "dev/udisk1110"
#endif

#ifndef PRODUCT_DIR_ROOT
#define PRODUCT_DIR_ROOT                /* empty */
#endif

#ifndef PRODUCT_DEV_BOOT
#define PRODUCT_DEV_BOOT                "boot"      /* no /dev/xxx, use local file*/
#endif

#ifndef PRODUCT_DEV_BOOTENV
#define PRODUCT_DEV_BOOTENV             "bootenv"   /* no /dev/xxx, use local file*/
#endif

#ifndef PRODUCT_DEV_PREFIX
#define PRODUCT_DEV_PREFIX              "p"
#endif

#ifndef PRODUCT_DEV_FLASH_MASTER
#define PRODUCT_DEV_FLASH_MASTER        "flash"
#endif

#ifndef PRODUCT_DEV_SD_MASTER
#define PRODUCT_DEV_SD_MASTER           "sd"
#endif

#ifndef PRODUCT_DEV_HD_MASTER
#define PRODUCT_DEV_HD_MASTER           "hd"
#endif

#ifndef PRODUCT_FIRMWARE_COUNT
#define PRODUCT_FIRMWARE_COUNT          7
#endif

#ifndef PRODUCT_FIRMWARE_CURRENT
#define PRODUCT_FIRMWARE_CURRENT        1
#endif

#ifndef PRODUCT_BOOTARGS_BLOCK0_NAME
#define PRODUCT_BOOTARGS_BLOCK0_NAME    "mmcblk0"
#endif

#define PRODUCT_DEV_KERNEL0     PRODUCT_IDEV_FLASH(6)
#define PRODUCT_DEV_KERNEL1     PRODUCT_IDEV_FLASH(7)
#define PRODUCT_DEV_KERNEL2     PRODUCT_IDEV_FLASH(8)
#define PRODUCT_DEV_KERNEL3     PRODUCT_IDEV_FLASH(9)
#define PRODUCT_DEV_KERNEL4     PRODUCT_IDEV_FLASH(10)
#define PRODUCT_DEV_KERNEL5     PRODUCT_IDEV_FLASH(11)
#define PRODUCT_DEV_KERNEL6     PRODUCT_IDEV_FLASH(12)

#define PRODUCT_DEV_ROOTFS0     PRODUCT_IDEV_FLASH(13)
#define PRODUCT_DEV_ROOTFS1     PRODUCT_IDEV_FLASH(14)
#define PRODUCT_DEV_ROOTFS2     PRODUCT_IDEV_FLASH(15)
#define PRODUCT_DEV_ROOTFS3     PRODUCT_IDEV_FLASH(16)
#define PRODUCT_DEV_ROOTFS4     PRODUCT_IDEV_FLASH(17)
#define PRODUCT_DEV_ROOTFS5     PRODUCT_IDEV_FLASH(18)
#define PRODUCT_DEV_ROOTFS6     PRODUCT_IDEV_FLASH(19)

#define PRODUCT_DEV_CONFIG0     PRODUCT_IDEV_FLASH(20)
#define PRODUCT_DEV_CONFIG1     PRODUCT_IDEV_FLASH(21)

#define PRODUCT_DEV_TOOL0       PRODUCT_IDEV_FLASH(22)
#define PRODUCT_DEV_TOOL1       PRODUCT_IDEV_FLASH(23)

#define PRODUCT_DEV_DATA0       PRODUCT_IDEV_FLASH(24)
#define PRODUCT_DEV_DATA1       PRODUCT_IDEV_FLASH(25)

#define PRODUCT_DEV_OTHER       PRODUCT_IDEV_FLASH(26)
#define PRODUCT_DEV_ROOT        PRODUCT_IDEV_FLASH(14) /* rootfs1 */
/******************************************************************************/
#endif
#endif /* __PC_H_37e332f8b6b0423d87dadd8a98dec31e__ */
