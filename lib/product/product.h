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

#define IS_PRODUCT_LTEFI_AP     (IS_PRODUCT_LTEFI_AP1 || IS_PRODUCT_LTEFI_AP2 || IS_PRODUCT_LTEFI_AP3)
#define IS_PRODUCT_LTEFI_MD     (IS_PRODUCT_LTEFI_MD1 || IS_PRODUCT_LTEFI_MD2 || IS_PRODUCT_LTEFI_MD3)

#define IS_PRODUCT_LTEFI_V1     (IS_PRODUCT_LTEFI_MD1 || IS_PRODUCT_LTEFI_AP1)
#define IS_PRODUCT_LTEFI_V2     (IS_PRODUCT_LTEFI_MD2 || IS_PRODUCT_LTEFI_AP2)
#define IS_PRODUCT_LTEFI_V3     (IS_PRODUCT_LTEFI_MD3 || IS_PRODUCT_LTEFI_AP3)

#define IS_PRODUCT_LTEFI_A      (IS_PRODUCT_LTEFI_V1)
#define IS_PRODUCT_LTEFI_B      (IS_PRODUCT_LTEFI_V2 || IS_PRODUCT_LTEFI_V3)
/******************************************************************************/
#include "product/pc.h"
#include "product/ap1.h"
#include "product/ap2.h"
#include "product/ap3.h"
#include "product/md1.h"
#include "product/md2.h"
#include "product/md3.h"
/******************************************************************************/
#ifndef PRODUCT_MEMORY_SIZE_MAX
#define PRODUCT_MEMORY_SIZE_MAX     8 /* G */
#endif

#ifndef PRODUCT_MEMORY_SIZE
#error "invalid PRODUCT_MEMORY_SIZE!"
#elif PRODUCT_MEMORY_SIZE > PRODUCT_MEMORY_SIZE_MAX
#define PRODUCT_MEMORY_SIZE_UNIT    "M"
#else
#define PRODUCT_MEMORY_SIZE_UNIT    "G"
#endif

#define PRODUCT_MEMORY_SIZE_STRING  __SYMBOL_TO_STRING(PRODUCT_MEMORY_SIZE) PRODUCT_MEMORY_SIZE_UNIT
/******************************************************************************/
#ifndef PRODUCT_MEMORY_FREQ
#define PRODUCT_MEMORY_FREQ         300
#endif

#ifndef PRODUCT_MEMORY_FREQ_UNIT
#define PRODUCT_MEMORY_FREQ_UNIT    "M"
#endif

#define PRODUCT_MEMORY_FREQ_STRING  __SYMBOL_TO_STRING(PRODUCT_MEMORY_FREQ) PRODUCT_MEMORY_FREQ_UNIT
/******************************************************************************/
#endif /* __PRODUCT_H_3aeec50c6570419493004695d20c38b5__ */
