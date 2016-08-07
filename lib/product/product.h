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
/******************************************************************************/
#include "product/define_first.h"
#include "product/vendor.h"
#include "product/version.h"
#include "product/board.h"
#include "product/check.h"
#include "product/define_last.h"
/******************************************************************************/
#endif /* __PRODUCT_H_3aeec50c6570419493004695d20c38b5__ */
