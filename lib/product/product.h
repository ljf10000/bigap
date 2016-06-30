#ifndef __PRODUCT_H_3aeec50c6570419493004695d20c38b5__
#define __PRODUCT_H_3aeec50c6570419493004695d20c38b5__
/******************************************************************************/
#define PRODUCT_PC              0   // pc
#define PRODUCT_LTEFI_MD1       1   // lte-fi v1 md board
#define PRODUCT_LTEFI_AP1       2   // lte-fi v1 ap board
#define PRODUCT_LTEFI_MD2       3   // lte-fi v2 md board
#define PRODUCT_LTEFI_AP2       4   // lte-fi v2 ap board
#define PRODUCT_LTEFI_MD3       5   // lte-fi v3 md board
#define PRODUCT_LTEFI_AP3       6   // lte-fi v3 ap board

#ifdef __PC__
#define __PRODUCT__             PRODUCT_PC
#endif

#ifndef __PRODUCT__
#error "must define __PRODUCT__=???"
#endif

#define IS_PRODUCT_PC           (__PRODUCT__==PRODUCT_PC)
#define IS_PRODUCT_LTEFI_MD1    (__PRODUCT__==PRODUCT_LTEFI_MD1)
#define IS_PRODUCT_LTEFI_AP1    (__PRODUCT__==PRODUCT_LTEFI_AP1)
#define IS_PRODUCT_LTEFI_MD2    (__PRODUCT__==PRODUCT_LTEFI_MD2)
#define IS_PRODUCT_LTEFI_AP2    (__PRODUCT__==PRODUCT_LTEFI_AP2)
#define IS_PRODUCT_LTEFI_MD3    (__PRODUCT__==PRODUCT_LTEFI_MD3)
#define IS_PRODUCT_LTEFI_AP3    (__PRODUCT__==PRODUCT_LTEFI_AP3)

#define IS_PRODUCT_LTEFI_AP     (IS_PRODUCT_LTEFI_AP1 || IS_PRODUCT_LTEFI_AP2 || IS_PRODUCT_LTEFI_AP3)
#define IS_PRODUCT_LTEFI_MD     (IS_PRODUCT_LTEFI_MD1 || IS_PRODUCT_LTEFI_MD2 || IS_PRODUCT_LTEFI_MD3)

#define IS_PRODUCT_LTEFI_V1     (IS_PRODUCT_LTEFI_MD1 || IS_PRODUCT_LTEFI_AP1)
#define IS_PRODUCT_LTEFI_V2     (IS_PRODUCT_LTEFI_MD2 || IS_PRODUCT_LTEFI_AP2)
#define IS_PRODUCT_LTEFI_V3     (IS_PRODUCT_LTEFI_MD3 || IS_PRODUCT_LTEFI_AP3)

#if IS_PRODUCT_LTEFI_V1
#   define PRODUCT_LTEFI_VERSION    1
#elif IS_PRODUCT_LTEFI_V2
#   define PRODUCT_LTEFI_VERSION    2
#elif IS_PRODUCT_LTEFI_V3
#   define PRODUCT_LTEFI_VERSION    3
#else
#   define PRODUCT_LTEFI_VERSION    1
#endif

#define IS_PRODUCT_LTEFI_A      (IS_PRODUCT_LTEFI_V1)
#define IS_PRODUCT_LTEFI_B      (IS_PRODUCT_LTEFI_V2 || IS_PRODUCT_LTEFI_V3)

#define PRODUCT_ROOTFS_RW       "rw"
#define PRODUCT_ROOTFS_RO       "ro"

#define PRODUCT_UNIT_K          "K"
#define PRODUCT_UNIT_M          "M"
#define PRODUCT_UNIT_G          "G"

#ifndef PRODUCT_VENDOR
#define PRODUCT_VENDOR          "superwalle"
#endif

#ifndef PRODUCT_COMPANY
#define PRODUCT_COMPANY         PRODUCT_VENDOR " Technology Co.,Ltd." /* fixme */
#endif

#ifndef PRODUCT_MIN_VERSION_STRING
#define PRODUCT_MIN_VERSION_STRING      "0.0.0.0"
#endif

#ifndef PRODUCT_MAX_VERSION_STRING
#define PRODUCT_MAX_VERSION_STRING      "255.255.255.255"
#endif

#ifndef PRODUCT_INVALID_VERSION
#define PRODUCT_INVALID_VERSION         PRODUCT_MIN_VERSION_STRING
#endif

#ifndef PRODUCT_DEFT_VERSION
#define PRODUCT_DEFT_VERSION            PRODUCT_DEFT_VERSION
#endif
/******************************************************************************/
#include "product/ap/ap1.h"
#include "product/ap/ap2.h"
#include "product/ap/ap3.h"
#include "product/md/md1.h"
#include "product/md/md2.h"
#include "product/md/md3.h"
#include "product/v1.h"
#include "product/v2.h"
#include "product/v3.h"
#include "product/ap.h"
#include "product/md.h"
#include "product/pc.h"
#include "product/check.h"
/******************************************************************************/
#define PRODUCT_FILE(_file)             PRODUCT_DIR_ROOT _file
#define PRODUCT_DIR(_dir)               PRODUCT_DIR_ROOT _dir
#define PRODUCT_IDIR(_dir, _idx)        _dir #_idx

#define PRODUCT_MASTER(_master)         PRODUCT_FILE(_master PRODUCT_DEV_PREFIX)
#define PRODUCT_IDEV(_master, _idx)     PRODUCT_IDIR(PRODUCT_MASTER(_master), _idx)
#define PRODUCT_IDEV_FLASH(_idx)        PRODUCT_IDEV(PRODUCT_DEV_FLASH_MASTER, _idx)
#define PRODUCT_IDEV_SD(_idx)           PRODUCT_IDEV(PRODUCT_DEV_SD_MASTER, _idx)
#define PRODUCT_IDEV_HD(_idx)           PRODUCT_IDEV(PRODUCT_DEV_HD_MASTER, _idx)
#define PRODUCT_IDEV_USB(_idx)          PRODUCT_IDEV(PRODUCT_DEV_USB_MASTER, _idx)

#ifndef PRODUCT_DIR_FLASH
#define PRODUCT_DIR_FLASH               PRODUCT_DIR("mnt/flash")
#endif

#ifndef PRODUCT_DIR_SD
#define PRODUCT_DIR_SD                  PRODUCT_DIR("mnt/sd")
#endif

#ifndef PRODUCT_DIR_HD
#define PRODUCT_DIR_HD                  PRODUCT_DIR("mnt/hd")
#endif

#ifndef PRODUCT_DIR_USB
#define PRODUCT_DIR_USB                 PRODUCT_DIR("mnt/usb")
#endif

#ifndef PRODUCT_DIR_KERNEL
#define PRODUCT_DIR_KERNEL              PRODUCT_DIR_FLASH "/kernel"
#endif

#ifndef PRODUCT_DIR_ROOTFS
#define PRODUCT_DIR_ROOTFS              PRODUCT_DIR_FLASH "/rootfs"
#endif

#ifndef PRODUCT_DIR_CONFIG
#define PRODUCT_DIR_CONFIG              PRODUCT_DIR_FLASH "/config"
#endif

#ifndef PRODUCT_DIR_DATA
#define PRODUCT_DIR_DATA                PRODUCT_DIR_FLASH "/data"
#endif

#ifndef PRODUCT_DIR_OTHER
#define PRODUCT_DIR_OTHER               PRODUCT_DIR_FLASH "/other"
#endif

#define PRODUCT_IDIR_KERNEL(_idx)       PRODUCT_IDIR(PRODUCT_DIR_KERNEL, _idx)
#define PRODUCT_IDIR_ROOTFS(_idx)       PRODUCT_IDIR(PRODUCT_DIR_ROOTFS, _idx)
#define PRODUCT_IDIR_CONFIG(_idx)       PRODUCT_IDIR(PRODUCT_DIR_CONFIG, _idx)
#define PRODUCT_IDIR_DATA(_idx)         PRODUCT_IDIR(PRODUCT_DIR_DATA,   _idx)

#ifndef PRODUCT_DEV_BOOT
#define PRODUCT_DEV_BOOT                PRODUCT_IDEV_FLASH(1) /* boot */
#endif

#ifndef PRODUCT_DEV_BOOTENV
#define PRODUCT_DEV_BOOTENV             PRODUCT_IDEV_FLASH(2) /* boot env */
#endif

#ifndef PRODUCT_DEV_BASEPARAM
#define PRODUCT_DEV_BASEPARAM           PRODUCT_IDEV_FLASH(3) /* baseparam */
#endif

#ifndef PRODUCT_DEV_PQPARAM
#define PRODUCT_DEV_PQPARAM             PRODUCT_IDEV_FLASH(4) /* pqparam */
#endif

#ifndef PRODUCT_DEV_LOGO
#define PRODUCT_DEV_LOGO                PRODUCT_IDEV_FLASH(5) /* logo */
#endif

#ifndef PRODUCT_DEV_HD
#define PRODUCT_DEV_HD                  PRODUCT_IDEV_HD(1)
#endif

#ifndef PRODUCT_DEV_SD
#define PRODUCT_DEV_SD                  PRODUCT_IDEV_SD(1)
#endif

#ifndef PRODUCT_DEV_USB
#define PRODUCT_DEV_USB                 PRODUCT_IDEV_USB(1)
#endif

#ifndef PRODUCT_PROC_CMDLINE
#define PRODUCT_PROC_CMDLINE            "/proc/cmdline"
#endif

#ifndef PRODUCT_PROC_CMDLINE_ROOT
#define PRODUCT_PROC_CMDLINE_ROOT       "root="
#endif

#ifndef PRODUCT_MEMORY_SIZE_MAX
#define PRODUCT_MEMORY_SIZE_MAX         8 /* G */
#endif

#if PRODUCT_MEMORY_SIZE > PRODUCT_MEMORY_SIZE_MAX
#   define PRODUCT_MEMORY_SIZE_UNIT     PRODUCT_UNIT_M
#else
#   define PRODUCT_MEMORY_SIZE_UNIT     PRODUCT_UNIT_G
#endif
#define PRODUCT_MEMORY_SIZE_STRING      __SYMBOL_TO_STRING(PRODUCT_MEMORY_SIZE) PRODUCT_MEMORY_SIZE_UNIT

#ifndef PRODUCT_MEMORY_FREQ
#define PRODUCT_MEMORY_FREQ             300
#endif

#ifndef PRODUCT_MEMORY_FREQ_UNIT
#define PRODUCT_MEMORY_FREQ_UNIT        PRODUCT_UNIT_M
#endif
#define PRODUCT_MEMORY_FREQ_STRING      __SYMBOL_TO_STRING(PRODUCT_MEMORY_FREQ) PRODUCT_MEMORY_FREQ_UNIT

#if IS_PRODUCT_LTEFI_MD2 || \
    IS_PRODUCT_LTEFI_MD3 || \
    IS_PRODUCT_PC
#if 3==PRODUCT_FIRMWARE_COUNT
#define PRODUCT_DEV_KERNEL0     PRODUCT_IDEV_FLASH(6)
#define PRODUCT_DEV_KERNEL1     PRODUCT_IDEV_FLASH(7)
#define PRODUCT_DEV_KERNEL2     PRODUCT_IDEV_FLASH(8)

/* 9 is unused */
#define PRODUCT_DEV_ROOTFS0     PRODUCT_IDEV_FLASH(10)
#define PRODUCT_DEV_ROOTFS1     PRODUCT_IDEV_FLASH(11)
#define PRODUCT_DEV_ROOTFS2     PRODUCT_IDEV_FLASH(12)

#define PRODUCT_DEV_CONFIG0     PRODUCT_IDEV_FLASH(13)
#define PRODUCT_DEV_CONFIG1     PRODUCT_IDEV_FLASH(14)

#define PRODUCT_DEV_DATA0       PRODUCT_IDEV_FLASH(15)
#define PRODUCT_DEV_DATA1       PRODUCT_IDEV_FLASH(16)

#define PRODUCT_DEV_OTHER       PRODUCT_IDEV_FLASH(17)
#define PRODUCT_DEV_ROOT        PRODUCT_IDEV_FLASH(11) /* rootfs1 */

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
                                \
        "4M(unused),"   /* 09 */\
                                \
        "200M(rootfs0),"/* 10 */\
        "200M(rootfs1),"/* 11 */\
        "200M(rootfs2),"/* 12 */\
                                \
        "16M(config0)," /* 13 */\
        "16M(config1)," /* 14 */\
                                \
        "148M(data0),"  /* 15 */\
        "148M(data1),"  /* 16 */\
        "-(others)"     /* 17 */\
        /* end */
#elif 5==PRODUCT_FIRMWARE_COUNT
#define PRODUCT_DEV_KERNEL0     PRODUCT_IDEV_FLASH(6)
#define PRODUCT_DEV_KERNEL1     PRODUCT_IDEV_FLASH(7)
#define PRODUCT_DEV_KERNEL2     PRODUCT_IDEV_FLASH(8)
#define PRODUCT_DEV_KERNEL3     PRODUCT_IDEV_FLASH(9)
#define PRODUCT_DEV_KERNEL4     PRODUCT_IDEV_FLASH(10)

#define PRODUCT_DEV_ROOTFS0     PRODUCT_IDEV_FLASH(11)
#define PRODUCT_DEV_ROOTFS1     PRODUCT_IDEV_FLASH(12)
#define PRODUCT_DEV_ROOTFS2     PRODUCT_IDEV_FLASH(13)
#define PRODUCT_DEV_ROOTFS3     PRODUCT_IDEV_FLASH(14)
#define PRODUCT_DEV_ROOTFS4     PRODUCT_IDEV_FLASH(15)

#define PRODUCT_DEV_CONFIG0     PRODUCT_IDEV_FLASH(16)
#define PRODUCT_DEV_CONFIG1     PRODUCT_IDEV_FLASH(17)

#define PRODUCT_DEV_DATA0       PRODUCT_IDEV_FLASH(18)
#define PRODUCT_DEV_DATA1       PRODUCT_IDEV_FLASH(19)

#define PRODUCT_DEV_OTHER       PRODUCT_IDEV_FLASH(20)
#define PRODUCT_DEV_ROOT        PRODUCT_IDEV_FLASH(12) /* rootfs1 */

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
                                \
        "256M(rootfs0),"/* 11 */\
        "256M(rootfs1),"/* 12 */\
        "256M(rootfs2),"/* 13 */\
        "256M(rootfs3),"/* 14 */\
        "256M(rootfs4),"/* 15 */\
                                \
        "32M(config0)," /* 16 */\
        "32M(config1)," /* 17 */\
                                \
        "300M(data0),"  /* 18 */\
        "300M(data1),"  /* 19 */\
        "-(others)"     /* 20 */\
        /* end */
#elif 7==PRODUCT_FIRMWARE_COUNT
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

#define PRODUCT_DEV_DATA0       PRODUCT_IDEV_FLASH(22)
#define PRODUCT_DEV_DATA1       PRODUCT_IDEV_FLASH(23)

#define PRODUCT_DEV_OTHER       PRODUCT_IDEV_FLASH(24)
#define PRODUCT_DEV_ROOT        PRODUCT_IDEV_FLASH(14) /* rootfs1 */

#if 4==PRODUCT_FLASH_SIZE
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
        "256M(rootfs0),"/* 13 */\
        "256M(rootfs1),"/* 14 */\
        "256M(rootfs2),"/* 15 */\
        "256M(rootfs3),"/* 16 */\
        "256M(rootfs4),"/* 17 */\
        "256M(rootfs5),"/* 18 */\
        "256M(rootfs6),"/* 19 */\
                                \
        "32M(config0)," /* 20 */\
        "32M(config1)," /* 21 */\
                                \
        "820M(data0),"  /* 22 */\
        "820M(data1),"  /* 23 */\
        "-(others)"     /* 24 */\
        /* end */
#elif 8==PRODUCT_FLASH_SIZE
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
        "768M(rootfs0),"/* 13 */\
        "768M(rootfs1),"/* 14 */\
        "768M(rootfs2),"/* 15 */\
        "768M(rootfs3),"/* 16 */\
        "768M(rootfs4),"/* 17 */\
        "768M(rootfs5),"/* 18 */\
        "768M(rootfs6),"/* 19 */\
                                \
        "32M(config0)," /* 20 */\
        "32M(config1)," /* 21 */\
                                \
        "1076M(data0)," /* 22 */\
        "1076M(data1)," /* 23 */\
        "-(others)"     /* 24 */\
        /* end */
#endif /* PRODUCT_FLASH_SIZE */
#endif /* PRODUCT_FIRMWARE_COUNT */

#define PRODUCT_IDEV_OBJ(_obj, _idx)    PRODUCT_DEV_##_obj##_idx
#define PRODUCT_IDEV_KERNEL(_idx)       PRODUCT_IDEV_OBJ(KERNEL, _idx)
#define PRODUCT_IDEV_ROOTFS(_idx)       PRODUCT_IDEV_OBJ(ROOTFS, _idx)
#define PRODUCT_IDEV_CONFIG(_idx)       PRODUCT_IDEV_OBJ(CONFIG, _idx)
#define PRODUCT_IDEV_DATA(_idx)         PRODUCT_IDEV_OBJ(DATA, _idx)

#ifndef PRODUCT_BOOTARGS_ROOTFSTYPE
#define PRODUCT_BOOTARGS_ROOTFSTYPE     "ext4"
#endif

#ifndef PRODUCT_BOOTARGS_CONSOLE
#define PRODUCT_BOOTARGS_CONSOLE        "ttyAMA0"
#endif

#ifndef PRODUCT_BOOTARGS_CONSOLE_BAUD
#define PRODUCT_BOOTARGS_CONSOLE_BAUD   115200
#endif

#define PRODUCT_BOOTARGS_HEAD   \
    "root=" PRODUCT_DEV_ROOT    \
        " "                     \
    "rootfstype=" PRODUCT_BOOTARGS_ROOTFSTYPE \
        " "                     \
    "rootwait"                  \
        " "                     \
    PRODUCT_ROOTFS_MODE         \
        " "                     \
    /* end */

#define PRODUCT_BOOTARGS_BODY   \
    "mem=" PRODUCT_MEMORY_SIZE_STRING \
        " "                     \
    "mmz=ddr,0,0," PRODUCT_MEMORY_FREQ_STRING \
        " "                     \
    "console=" PRODUCT_BOOTARGS_CONSOLE "," __SYMBOL_TO_STRING(PRODUCT_BOOTARGS_CONSOLE_BAUD) \
        " "                     \
    "blkdevparts="              \
        PRODUCT_BOOTARGS_BLOCK0_NAME ":" PRODUCT_BOOTARGS_BLOCK0 \
    /* end */

#ifdef CONFIG_BOOTARGS
#undef CONFIG_BOOTARGS
#endif

#define CONFIG_BOOTARGS         \
    PRODUCT_BOOTARGS_HEAD       \
    PRODUCT_BOOTARGS_BODY       \
    /* end */
#endif
/******************************************************************************/
#endif /* __PRODUCT_H_3aeec50c6570419493004695d20c38b5__ */
