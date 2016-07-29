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
#define PRODUCT_LTEFI_MD4       7   // lte-fi v4 md board
#define PRODUCT_LTEFI_AP4       8   // lte-fi v4 ap board

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
#define IS_PRODUCT_LTEFI_MD4    (__PRODUCT__==PRODUCT_LTEFI_MD4)
#define IS_PRODUCT_LTEFI_AP4    (__PRODUCT__==PRODUCT_LTEFI_AP4)

#define IS_PRODUCT_LTEFI_AP     (IS_PRODUCT_LTEFI_AP1 || IS_PRODUCT_LTEFI_AP2 || IS_PRODUCT_LTEFI_AP3 || IS_PRODUCT_LTEFI_AP4)
#define IS_PRODUCT_LTEFI_MD     (IS_PRODUCT_LTEFI_MD1 || IS_PRODUCT_LTEFI_MD2 || IS_PRODUCT_LTEFI_MD3 || IS_PRODUCT_LTEFI_MD4)

#define IS_PRODUCT_LTEFI_V1     (IS_PRODUCT_LTEFI_MD1 || IS_PRODUCT_LTEFI_AP1)
#define IS_PRODUCT_LTEFI_V2     (IS_PRODUCT_LTEFI_MD2 || IS_PRODUCT_LTEFI_AP2)
#define IS_PRODUCT_LTEFI_V3     (IS_PRODUCT_LTEFI_MD3 || IS_PRODUCT_LTEFI_AP3)
#define IS_PRODUCT_LTEFI_V4     (IS_PRODUCT_LTEFI_MD4 || IS_PRODUCT_LTEFI_AP4)

#define IS_PRODUCT_LTEFI_MD_PARTITION_A     (IS_PRODUCT_LTEFI_V1)
#define IS_PRODUCT_LTEFI_MD_PARTITION_B     (IS_PRODUCT_LTEFI_V2 || IS_PRODUCT_LTEFI_V3 || IS_PRODUCT_LTEFI_V4)

#define IS_PRODUCT_LTEFI_MD_CPU_A           (IS_PRODUCT_LTEFI_MD1 || IS_PRODUCT_LTEFI_MD2 || IS_PRODUCT_LTEFI_MD4) /* 3718C */
#define IS_PRODUCT_LTEFI_MD_CPU_B           (IS_PRODUCT_LTEFI_MD3) /* 3798M */

#define PRODUCT_ROOTFS_RW       "rw"
#define PRODUCT_ROOTFS_RO       "ro"

#define PRODUCT_UNIT_K          "K"
#define PRODUCT_UNIT_M          "M"
#define PRODUCT_UNIT_G          "G"

#ifndef PRODUCT_BUILD_TIME
#define PRODUCT_BUILD_TIME      "2016-07-01-00:00:00"
#endif

#ifndef PRODUCT_VENDOR
#define PRODUCT_VENDOR          "superwalle"
#endif

#ifndef PRODUCT_VENDOR_DOMAIN
#define PRODUCT_VENDOR_DOMAIN   PRODUCT_VENDOR ".com"
#endif

#ifndef PRODUCT_VENDOR_URL
#define PRODUCT_VENDOR_URL      "www." PRODUCT_VENDOR_DOMAIN
#endif

#ifndef PRODUCT_COMPANY
#define PRODUCT_COMPANY         PRODUCT_VENDOR " Technology Co.,Ltd." /* fixme */
#endif

#define PRODUCT_FILE(_file)             PRODUCT_DIR_ROOT _file
#define PRODUCT_DIR(_dir)               PRODUCT_DIR_ROOT _dir
#define PRODUCT_IDIR(_dir, _idx)        _dir #_idx

#define PRODUCT_MASTER(_master)         PRODUCT_FILE(_master PRODUCT_DEV_PREFIX)
#define PRODUCT_IDEV(_master, _idx)     PRODUCT_IDIR(PRODUCT_MASTER(_master), _idx)
#define PRODUCT_IDEV_FLASH(_idx)        PRODUCT_IDEV(PRODUCT_DEV_FLASH_MASTER, _idx)
#define PRODUCT_IDEV_SD(_idx)           PRODUCT_IDEV(PRODUCT_DEV_SD_MASTER, _idx)
#define PRODUCT_IDEV_HD(_idx)           PRODUCT_IDEV(PRODUCT_DEV_HD_MASTER, _idx)
#define PRODUCT_IDEV_USB(_idx)          PRODUCT_IDEV(PRODUCT_DEV_USB_MASTER, _idx)
/******************************************************************************/
#include "product/version.h"
#include "product/ap.h"
#include "product/md.h"
#include "product/pc.h"
#include "product/check.h"
/******************************************************************************/
#ifndef PRODUCT_FILE_VERSION
#define PRODUCT_FILE_VERSION            ".version"
#endif

#ifndef PRODUCT_BIN_MD_KERNEL
#define PRODUCT_BIN_MD_KERNEL           "hi_kernel.bin"
#endif

#ifndef PRODUCT_BIN_MD_BASEPARAM
#define PRODUCT_BIN_MD_BASEPARAM        "base_param.bin"
#endif

#ifndef PRODUCT_BIN_MD_PQPARAM
#define PRODUCT_BIN_MD_PQPARAM          "pq_param.bin"
#endif

#ifndef PRODUCT_BIN_MD_BOOT
#define PRODUCT_BIN_MD_BOOT             "fastboot-burn.bin"
#endif

#ifndef PRODUCT_BIN_MD_BOOTENV
#define PRODUCT_BIN_MD_BOOTENV          "bootenv.bin"
#endif

#ifndef PRODUCT_BIN_AP_BOOT
#define PRODUCT_BIN_AP_BOOT             "u-boot.bin"
#endif

#ifndef PRODUCT_BIN_AP_BOOTENV
#define PRODUCT_BIN_AP_BOOTENV          "u-bootenv.bin"
#endif

#ifndef PRODUCT_BIN_AP_FIRMWARE
#define PRODUCT_BIN_AP_FIRMWARE         "openwrt.bin"
#endif

#ifndef PRODUCT_BIN_TOOL
#define PRODUCT_BIN_TOOL                "tool.bin"
#endif

#ifndef PRODUCT_BIN_DATA
#define PRODUCT_BIN_DATA                "data.bin"
#endif

#ifndef PRODUCT_BIN_CONFIG
#define PRODUCT_BIN_CONFIG              "config.bin"
#endif

#ifndef PRODUCT_BIN_ROOTFS
#define PRODUCT_BIN_ROOTFS              "rootfs.bin"
#endif

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

#ifndef PRODUCT_DIR_TOOL
#define PRODUCT_DIR_TOOL                PRODUCT_DIR_FLASH "/tool"
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
#define PRODUCT_IDIR_TOOL(_idx)         PRODUCT_IDIR(PRODUCT_DIR_TOOL,   _idx)
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

#if IS_PRODUCT_LTEFI_MD_PARTITION_B || IS_PRODUCT_PC
#define PRODUCT_IDEV_OBJ(_obj, _idx)    PRODUCT_DEV_##_obj##_idx
#define PRODUCT_IDEV_KERNEL(_idx)       PRODUCT_IDEV_OBJ(KERNEL, _idx)
#define PRODUCT_IDEV_ROOTFS(_idx)       PRODUCT_IDEV_OBJ(ROOTFS, _idx)
#define PRODUCT_IDEV_CONFIG(_idx)       PRODUCT_IDEV_OBJ(CONFIG, _idx)
#define PRODUCT_IDEV_TOOL(_idx)         PRODUCT_IDEV_OBJ(TOOL, _idx)
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
