#ifndef __DEFINE_LAST_H_5a4fe08a14914125b8195bd01c736a46__
#define __DEFINE_LAST_H_5a4fe08a14914125b8195bd01c736a46__
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

#define PRODUCT_IDIR_KERNEL(_idx)       PRODUCT_IDIR(PRODUCT_DIR_KERNEL, _idx)
#define PRODUCT_IDIR_ROOTFS(_idx)       PRODUCT_IDIR(PRODUCT_DIR_ROOTFS, _idx)
#define PRODUCT_IDIR_CONFIG(_idx)       PRODUCT_IDIR(PRODUCT_DIR_CONFIG, _idx)
#define PRODUCT_IDIR_TOOL(_idx)         PRODUCT_IDIR(PRODUCT_DIR_TOOL,   _idx)
#define PRODUCT_IDIR_DATA(_idx)         PRODUCT_IDIR(PRODUCT_DIR_DATA,   _idx)

#ifndef PRODUCT_DEV_BOOT
#if IS_PRODUCT_LTEFI_AP
#define PRODUCT_DEV_BOOT                PRODUCT_IDEV_FLASH(0) /* boot */
#else
#define PRODUCT_DEV_BOOT                PRODUCT_IDEV_FLASH(1) /* boot */
#endif
#endif

#ifndef PRODUCT_DEV_BOOTENV
#if IS_PRODUCT_LTEFI_AP
#define PRODUCT_DEV_BOOTENV             PRODUCT_IDEV_FLASH(1) /* boot env */
#else
#define PRODUCT_DEV_BOOTENV             PRODUCT_IDEV_FLASH(2) /* boot env */
#endif
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

#define PRODUCT_IDEV_OBJ(_obj, _idx)    PRODUCT_DEV_##_obj##_idx
#define PRODUCT_IDEV_KERNEL(_idx)       PRODUCT_IDEV_OBJ(KERNEL, _idx)
#define PRODUCT_IDEV_ROOTFS(_idx)       PRODUCT_IDEV_OBJ(ROOTFS, _idx)
#define PRODUCT_IDEV_CONFIG(_idx)       PRODUCT_IDEV_OBJ(CONFIG, _idx)
#define PRODUCT_IDEV_TOOL(_idx)         PRODUCT_IDEV_OBJ(TOOL, _idx)
#define PRODUCT_IDEV_DATA(_idx)         PRODUCT_IDEV_OBJ(DATA, _idx)

#if IS_PRODUCT_LTEFI_MD_PARTITION_B || IS_PRODUCT_PC
#if IS_PRODUCT_LTEFI_AP
#define PRODUCT_BOOTARGS_HEAD   \
    "root=" PRODUCT_DEV_ROOT    \
		" "						\
    "rootfstype=" PRODUCT_ROOTFSTYPE \
		" "						\
	"init=/sbin/init"			\
		" "						\
    /* end */
#else
#define PRODUCT_BOOTARGS_HEAD   \
    "root=" PRODUCT_DEV_ROOT    \
        " "                     \
    "rootfstype=" PRODUCT_ROOTFSTYPE \
        " "                     \
    "rootwait"                  \
        " "                     \
    PRODUCT_ROOTFS_MODE         \
        " "                     \
    /* end */
#endif

#if 0
#define PRODUCT_BOOTARGS_BODY   \
    "mem=" PRODUCT_MEMORY_SIZE_STRING \
        " "                     \
    "mmz=ddr,0,0," PRODUCT_MEMORY_FREQ_STRING \
        " "                     \
    "console=" PRODUCT_CONSOLE "," __SYMBOL_TO_STRING(PRODUCT_CONSOLE_BAUD) \
        " "                     \
    "blkdevparts="              \
        PRODUCT_BOOTARGS_BLOCK0_NAME ":" PRODUCT_BOOTARGS_BLOCK0 \
    /* end */
#else
#if IS_PRODUCT_LTEFI_AP
#define PRODUCT_BOOTARGS_BODY   \
	"console=" PRODUCT_CONSOLE "," __SYMBOL_TO_STRING(PRODUCT_CONSOLE_BAUD) \
        " "                     \
    "mtdparts="              \
        PRODUCT_BOOTARGS_BLOCK0_NAME ":" PRODUCT_BOOTARGS_BLOCK0 \
    /* end */
#else
#define PRODUCT_BOOTARGS_BODY   \
    "mem=" PRODUCT_MEMORY_SIZE_STRING \
        " "                     \
    "console=" PRODUCT_CONSOLE "," __SYMBOL_TO_STRING(PRODUCT_CONSOLE_BAUD) \
        " "                     \
    "blkdevparts="              \
        PRODUCT_BOOTARGS_BLOCK0_NAME ":" PRODUCT_BOOTARGS_BLOCK0 \
    /* end */
#endif
#endif

#ifdef CONFIG_BOOTARGS
#undef CONFIG_BOOTARGS
#endif

#define CONFIG_BOOTARGS         \
    PRODUCT_BOOTARGS_HEAD       \
    PRODUCT_BOOTARGS_BODY       \
    /* end */
#endif
/******************************************************************************/
#endif /* __DEFINE_LAST_H_5a4fe08a14914125b8195bd01c736a46__ */
