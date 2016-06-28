#ifndef __MD2_H_68c306e9e7ce4cc2b95c6fc7fd632357__
#define __MD2_H_68c306e9e7ce4cc2b95c6fc7fd632357__
#if IS_PRODUCT_LTEFI_MD2
/******************************************************************************/
#ifndef PRODUCT_PCBA_MODEL
#define PRODUCT_PCBA_MODEL          PRODUCT_VENDOR "-pcba" /* fixme */
#endif

#ifndef PRODUCT_DOG_COUNT
#define PRODUCT_DOG_COUNT           2
#endif

#ifndef PRODUCT_FLASH_SIZE
#define PRODUCT_FLASH_SIZE          4 /* 4G */
#endif

#ifndef PRODUCT_MEMORY_SIZE
#define PRODUCT_MEMORY_SIZE         2 /* 2G */
#endif

#ifndef PRODUCT_ROOTFS_MODE
#define PRODUCT_ROOTFS_MODE         PRODUCT_ROOTFS_RO
#endif

#ifndef PRODUCT_DEV_USB_MASTER
#define PRODUCT_DEV_USB_MASTER      "dev/udisk1110"
#endif
/******************************************************************************/
#endif
#endif /* __MD2_H_68c306e9e7ce4cc2b95c6fc7fd632357__ */
