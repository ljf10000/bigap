#ifndef __VERSION_H_b1c4e3849614480f9e606e430a6f4ee7__
#define __VERSION_H_b1c4e3849614480f9e606e430a6f4ee7__
/******************************************************************************/
typedef struct {
    unsigned char number[4];
} product_version_t;

#define PRODUCT_VERSION(a, b, c, d)     {.number = {a,b,c,d}}

#ifndef PRODUCT_MIN_VERSION_STRING
#define PRODUCT_MIN_VERSION_STRING      "0.0.0.0"
#endif

#ifndef PRODUCT_MIN_VERSION
#define PRODUCT_MIN_VERSION             PRODUCT_VERSION(0, 0, 0, 0)
#endif

#ifndef PRODUCT_MAX_VERSION_STRING
#define PRODUCT_MAX_VERSION_STRING      "255.255.255.255"
#endif

#ifndef PRODUCT_MAX_VERSION
#define PRODUCT_MAX_VERSION             PRODUCT_VERSION(255, 255, 255, 255)
#endif

#ifndef PRODUCT_INVALID_VERSION_STRING
#define PRODUCT_INVALID_VERSION_STRING  PRODUCT_MIN_VERSION_STRING
#endif

#ifndef PRODUCT_INVALID_VERSION
#define PRODUCT_INVALID_VERSION         PRODUCT_MIN_VERSION
#endif

enum {
    PRODUCT_VERSION_STRING_LEN = sizeof(PRODUCT_MAX_VERSION_STRING) - 1,  /* 15 */
};

#ifndef PRODUCT_DEFT_VERSION_A
#define PRODUCT_DEFT_VERSION_A          0
#endif

#ifndef PRODUCT_DEFT_VERSION_B
#define PRODUCT_DEFT_VERSION_B          0
#endif

#ifndef PRODUCT_DEFT_VERSION_C
#define PRODUCT_DEFT_VERSION_C          0
#endif

#ifndef PRODUCT_DEFT_VERSION_D
#define PRODUCT_DEFT_VERSION_D          1
#endif

#define PRODUCT_DEFT_VERSION            PRODUCT_VERSION(PRODUCT_DEFT_VERSION_A, PRODUCT_DEFT_VERSION_B, PRODUCT_DEFT_VERSION_C, PRODUCT_DEFT_VERSION_D)
#define PRODUCT_DEFT_VERSION_STRING             \
    __SYMBOL_TO_STRING(PRODUCT_DEFT_VERSION_A)  \
    "."                                         \
    __SYMBOL_TO_STRING(PRODUCT_DEFT_VERSION_B)  \
    "."                                         \
    __SYMBOL_TO_STRING(PRODUCT_DEFT_VERSION_C)  \
    "."                                         \
    __SYMBOL_TO_STRING(PRODUCT_DEFT_VERSION_D)  \
    /* end */
/******************************************************************************/
/* must last */
#include "product/v1.h"
#include "product/v2.h"
#include "product/v3.h"
#include "product/v4.h"
/******************************************************************************/
#endif /* __VERSION_H_b1c4e3849614480f9e606e430a6f4ee7__ */
