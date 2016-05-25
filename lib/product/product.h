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

#ifndef __PRODUCT__
#ifdef  __PC__
#   define __PRODUCT__          PRODUCT_PC
#else
#   define __PRODUCT__          PRODUCT_LTEFI_MD3
#endif
#endif

#define IS_PRODUCT_PC           (__PRODUCT__==PRODUCT_PC)
#define IS_PRODUCT_LTEFI_MD1    (__PRODUCT__==PRODUCT_LTEFI_MD1)
#define IS_PRODUCT_LTEFI_AP1    (__PRODUCT__==PRODUCT_LTEFI_AP1)
#define IS_PRODUCT_LTEFI_MD2    (__PRODUCT__==PRODUCT_LTEFI_MD2)
#define IS_PRODUCT_LTEFI_AP2    (__PRODUCT__==PRODUCT_LTEFI_AP2)
#define IS_PRODUCT_LTEFI_MD3    (__PRODUCT__==PRODUCT_LTEFI_MD3)
#define IS_PRODUCT_LTEFI_AP3    (__PRODUCT__==PRODUCT_LTEFI_AP3)

#define __PRODUCT_VERSION(_product)     (((_product)+1)/2)
#define __PRODUCT_LTEFI_AP(_product)    ((_product) && 0==((_product)%2))
#define __PRODUCT_LTEFI_MD(_product)    ((_product) && 1==((_product)%2))

#define PRODUCT_VERSION         __PRODUCT_VERSION(__PRODUCT__)
#define IS_PRODUCT_LTEFI_AP     __PRODUCT_LTEFI_AP(__PRODUCT__)
#define IS_PRODUCT_LTEFI_MD     __PRODUCT_LTEFI_MD(__PRODUCT__)
/******************************************************************************/
#include "product/pc.h"
#include "product/ap1.h"
#include "product/ap2.h"
#include "product/ap3.h"
#include "product/md1.h"
#include "product/md2.h"
#include "product/md3.h"
/******************************************************************************/
#ifndef OS_DOG_COUNT
#define OS_DOG_COUNT    1
#endif

/******************************************************************************/
#endif /* __PRODUCT_H_3aeec50c6570419493004695d20c38b5__ */
