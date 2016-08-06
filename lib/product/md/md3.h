#ifndef __MD3_H_a6505c1ba56b48df9e21eb182d06c773__
#define __MD3_H_a6505c1ba56b48df9e21eb182d06c773__
#if IS_PRODUCT_LTEFI_MD3
/******************************************************************************/
#ifndef PRODUCT_MODEL
#define PRODUCT_MODEL                   "WeJan1"
#endif

#ifndef PRODUCT_VERSION
#define PRODUCT_VERSION                 "H1"
#endif

#ifndef PRODUCT_PCBA_MODEL
#define PRODUCT_PCBA_MODEL              "HI3798M_V1.1"
#endif

#ifndef PRODUCT_PCBA_VERSION
#define PRODUCT_PCBA_VERSION            "1.1.1.2"
#endif

#define PRODUCT_DOG_COUNT               3
#define PRODUCT_FLASH_SIZE              8 /* 8G */
#define PRODUCT_MEMORY_SIZE             2 /* 2G */
#define PRODUCT_FIRMWARE_COUNT          7
#define PRODUCT_FIRMWARE_CURRENT        1
#define PRODUCT_KERNEL_SIZE             16 /* 16M */
/*
* kernel0 begin(block count) =  12M/512 =  24K = 0x06000
* kernel1 begin(block count) =  28M/512 =  56K = 0x0E000
* kernel2 begin(block count) =  44M/512 =  88K = 0x16000
* kernel3 begin(block count) =  60M/512 = 120K = 0x1E000
* kernel4 begin(block count) =  76M/512 = 152K = 0x26000
* kernel5 begin(block count) =  92M/512 = 184K = 0x2E000
* kernel6 begin(block count) = 108M/512 = 216K = 0x36000
*
* kernel  size (block count) = 16M/512 =  32K = 0x8000
*/
#define PRODUCT_KERNEL_BLKCOUNT         0x8000
#define PRODUCT_KERNEL_BLKSTART0        0x06000
#define PRODUCT_KERNEL_BLKSTART1        0x0E000
#define PRODUCT_KERNEL_BLKSTART2        0x16000
#define PRODUCT_KERNEL_BLKSTART3        0x1E000
#define PRODUCT_KERNEL_BLKSTART4        0x26000
#define PRODUCT_KERNEL_BLKSTART5        0x2E000
#define PRODUCT_KERNEL_BLKSTART6        0x36000
#define PRODUCT_KERNEL_BLKSTART_DEFT    PRODUCT_KERNEL_BLKSTART1

#ifndef PRODUCT_ROOTFS_MODE
#define PRODUCT_ROOTFS_MODE             PRODUCT_ROOTFS_RO
#endif

#ifndef PRODUCT_DEV_USB_MASTER
#define PRODUCT_DEV_USB_MASTER          "dev/udisk51210"
#endif

#ifndef PRODUCT_DEV_HD_MASTER
#define PRODUCT_DEV_HD_MASTER           "dev/udisk61110"
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

#define PRODUCT_BOOTARGS_BLOCK0 \
        "512K(fastboot),"/*01 */\
        "512K(bootenv),"/* 02 */\
        "4M(baseparam),"/* 03 */\
        "4M(pqparam),"  /* 04 */\
        "3M(logo),"     /* 05 */\
                                \
        "16M(kernel0)," /* 06 */\
        "16M(kernel1)," /* 07 */\
        "16M(kernel2)," /* 08 */\
        "16M(kernel3)," /* 09 */\
        "16M(kernel4)," /* 10 */\
        "16M(kernel5)," /* 11 */\
        "16M(kernel6)," /* 12 */\
                                \
        "200M(rootfs0),"/* 13 */\
        "200M(rootfs1),"/* 14 */\
        "200M(rootfs2),"/* 15 */\
        "200M(rootfs3),"/* 16 */\
        "200M(rootfs4),"/* 17 */\
        "200M(rootfs5),"/* 18 */\
        "200M(rootfs6),"/* 19 */\
                                \
        "32M(config0)," /* 20 */\
        "32M(config1)," /* 21 */\
                                \
        "1024M(tool0)," /* 22 */\
        "1024M(tool1)," /* 23 */\
                                \
        "1900M(data0)," /* 24 */\
        "1900M(data1)," /* 25 */\
                                \
        "-(others)"     /* 26 */\
        /* end */
/******************************************************************************/
#endif
#endif /* __MD3_H_a6505c1ba56b48df9e21eb182d06c773__ */
