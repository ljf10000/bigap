#ifndef __DEFINE_FIRST_H_61a77863687d4885bf84afb7966c6bb3__
#define __DEFINE_FIRST_H_61a77863687d4885bf84afb7966c6bb3__
/******************************************************************************/
#define PRODUCT_ROOTFS_RW               "rw"
#define PRODUCT_ROOTFS_RO               "ro"

#define PRODUCT_UNIT_K                  "K"
#define PRODUCT_UNIT_M                  "M"
#define PRODUCT_UNIT_G                  "G"

#ifndef PRODUCT_BUILD_TIME
#define PRODUCT_BUILD_TIME              "2016-07-01-00:00:00"
#endif

#ifndef PRODUCT_MEMORY_SIZE_MAX
#define PRODUCT_MEMORY_SIZE_MAX         8 /* G */
#endif

#if PRODUCT_MEMORY_SIZE > PRODUCT_MEMORY_SIZE_MAX
#   define PRODUCT_MEMORY_SIZE_UNIT     PRODUCT_UNIT_M
#else
#   define PRODUCT_MEMORY_SIZE_UNIT     PRODUCT_UNIT_G
#endif

#ifndef PRODUCT_MEMORY_FREQ
#define PRODUCT_MEMORY_FREQ             300
#endif

#ifndef PRODUCT_MEMORY_FREQ_UNIT
#define PRODUCT_MEMORY_FREQ_UNIT        PRODUCT_UNIT_M
#endif

#define PRODUCT_MEMORY_SIZE_STRING      __SYMBOL_TO_STRING(PRODUCT_MEMORY_SIZE) PRODUCT_MEMORY_SIZE_UNIT
#define PRODUCT_MEMORY_FREQ_STRING      __SYMBOL_TO_STRING(PRODUCT_MEMORY_FREQ) PRODUCT_MEMORY_FREQ_UNIT

#ifndef PRODUCT_ROOTFSTYPE
#define PRODUCT_ROOTFSTYPE              "ext4"
#endif

#ifndef PRODUCT_CONSOLE
#if IS_PRODUCT_LTEFI_AP
#define PRODUCT_CONSOLE					"ttyS0"
#else
#define PRODUCT_CONSOLE                 "ttyAMA0"
#endif
#endif

#ifndef PRODUCT_CONSOLE_BAUD
#define PRODUCT_CONSOLE_BAUD            115200
#endif
/******************************************************************************/
#define PRODUCT_FILE(_file)             PRODUCT_DIR_ROOT _file
#define PRODUCT_DIR(_dir)               PRODUCT_DIR_ROOT _dir
#define PRODUCT_IDIR(_dir, _idx)        _dir #_idx

#define DIR_SCRIPT                      "lib/script"
#define SCRIPT_FILE(_file)              PRODUCT_FILE(DIR_SCRIPT "/" _file)

#define PRODUCT_MASTER(_master)         PRODUCT_FILE(_master PRODUCT_DEV_PREFIX)
#define PRODUCT_IDEV(_master, _idx)     PRODUCT_IDIR(PRODUCT_MASTER(_master), _idx)
#define PRODUCT_IDEV_FLASH(_idx)        PRODUCT_IDEV(PRODUCT_DEV_FLASH_MASTER, _idx)
#define PRODUCT_IDEV_SD(_idx)           PRODUCT_IDEV(PRODUCT_DEV_SD_MASTER, _idx)
#define PRODUCT_IDEV_HD(_idx)           PRODUCT_IDEV(PRODUCT_DEV_HD_MASTER, _idx)
#define PRODUCT_IDEV_USB(_idx)          PRODUCT_IDEV(PRODUCT_DEV_USB_MASTER, _idx)
/******************************************************************************/
#endif /* __DEFINE_FIRST_H_61a77863687d4885bf84afb7966c6bb3__ */
