/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      benv
#endif

#include "benv/benv.h"
#include "benv/bcookie.h"

#if !(IS_PRODUCT_PC || IS_PRODUCT_LTEFI_MD_PARTITION_B)
#error "just support ltefi(v2/v3/pc)"
#endif

#ifdef __BOOT__
#     if IS_PRODUCT_LTEFI_MD || IS_PRODUCT_PC
#       include "benv_boot_md.c"
#   elif IS_PRODUCT_LTEFI_AP
#       include "benv_boot_ap.c"
#   endif
#elif defined(__APP__)
#   include "benv_app.c"
#endif
/******************************************************************************/
