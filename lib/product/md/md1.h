#ifndef __MD1_H_a2a2800b607e4f778a7661bbb363b5fb__
#define __MD1_H_a2a2800b607e4f778a7661bbb363b5fb__
#if IS_PRODUCT_LTEFI_MD1
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
#define PRODUCT_ROOTFS_MODE         PRODUCT_ROOTFS_RW
#endif

#ifndef PRODUCT_DEV_USB_MASTER
#define PRODUCT_DEV_USB_MASTER      "dev/udisk1310"
#endif

/******************************************************************************/
#endif
#endif /* __MD1_H_a2a2800b607e4f778a7661bbb363b5fb__ */
