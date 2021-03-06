#ifndef __CHECK_H_5b9ab40ec4e840a9b5403ab1c8e22def__
#define __CHECK_H_5b9ab40ec4e840a9b5403ab1c8e22def__
/******************************************************************************/
#ifndef PRODUCT_BLOCK_SIZE
#error "must define PRODUCT_BLOCK_SIZE!"
#endif

#ifndef PRODUCT_BOOT_SIZE
#error "must define PRODUCT_BOOT_SIZE!"
#endif

#ifndef PRODUCT_FLASH_SIZE
#error "must define PRODUCT_FLASH_SIZE!"
#endif

#ifndef PRODUCT_MEMORY_SIZE
#error "must define PRODUCT_MEMORY_SIZE!"
#endif

#ifndef PRODUCT_FIRMWARE_COUNT
#error "must define PRODUCT_FIRMWARE_COUNT!"
#endif

#ifndef PRODUCT_FIRMWARE_CURRENT
#error "must define PRODUCT_FIRMWARE_CURRENT!"
#endif

#ifndef PRODUCT_DIR_ROOT
#error "must define PRODUCT_DIR_ROOT!"
#endif

#ifndef PRODUCT_DEV_PREFIX
#error "must define PRODUCT_DEV_PREFIX!"
#endif

#ifndef PRODUCT_DEV_FLASH_MASTER
#error "must define PRODUCT_DEV_FLASH_MASTER!"
#endif

#ifndef PRODUCT_DEV_SD_MASTER
#error "must define PRODUCT_DEV_SD_MASTER!"
#endif

#ifndef PRODUCT_DEV_HD_MASTER
#error "must define PRODUCT_DEV_HD_MASTER!"
#endif

#ifndef PRODUCT_DEV_USB_MASTER
#error "must define PRODUCT_DEV_USB_MASTER!"
#endif

#ifndef PRODUCT_ROOTFS_MODE
#error "must define PRODUCT_ROOTFS_MODE!"
#endif

#if IS_PRODUCT_LTEFI_AP //AP is 16M
#if 16!=PRODUCT_FLASH_SIZE
#error "invalid PRODUCT_FLASH_SIZE!"
#endif
#else	//MD PC is 4G 8G
#if 4!=PRODUCT_FLASH_SIZE && \
	8!=PRODUCT_FLASH_SIZE
#error "invalid PRODUCT_FLASH_SIZE!"
#endif
#endif

#if 1!=PRODUCT_FIRMWARE_COUNT && \
    3!=PRODUCT_FIRMWARE_COUNT && \
    7!=PRODUCT_FIRMWARE_COUNT
#error "invalid PRODUCT_FIRMWARE_COUNT!"
#endif
/******************************************************************************/
#endif /* __CHECK_H_5b9ab40ec4e840a9b5403ab1c8e22def__ */
