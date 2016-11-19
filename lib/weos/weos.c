/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      libweos
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     libweos
#endif

#define __LIB__
#include "utils.h"
LIB_INITER;
/******************************************************************************/
#include "blob.c"
#include "channel.c"
#include "coroutine.c"
#include "cqueue.c"
#include "dll.c"
#include "fd.c"
#include "file.c"
#include "json.c"
#include "loop.c"
#include "pipe.c"
#include "slice.c"
#include "timer.c"
/******************************************************************************/
bool __THIS_DEAMON;

int
__oem_type(void)
{
    static int type = -1;

    if (false==is_good_enum(type, OEM_T_END)) {
        char vendor[1+OEM_NAME_LEN] = {0};

        if (os_file_exist(OEM_FILE)) {
            os_v_fgets(vendor, OEM_NAME_LEN, OEM_FILE);
            type = oem_type_idx(vendor);
            if (is_good_oem_type(type)) {
                return type;
            }
        }
        
#if IS_PRODUCT_LTEFI_MD1
        os_v_pgets(vendor, OEM_NAME_LEN, "bootm product.vendor");
#elif IS_PRODUCT_LTEFI_MD_PARTITION_B || IS_PRODUCT_PC
        os_v_pgets(vendor, OEM_NAME_LEN, "benv infos/product/vendor");
#endif
        type = oem_type_idx(vendor);
        if (false==is_good_oem_type(type)) {
            type = OEM_T_DEFT;
        }
    }
    
    return type;
}

oem_t *
__this_oem(void)
{
    static oem_t oem[OEM_T_END];
    static bool inited = false;

    if (false==inited) {
        oem_t tmp[OEM_T_END] = __THIS_OEM_INITER;

        os_memcpy(oem, tmp, sizeof(oem_t) * OEM_T_END);
    }
    
    return &oem[__oem_type()];
}

#if 0
haenv_file_t *
__this_haenv(void)
{
    static haenv_file_t env;

    return &env;
}
#endif

os_env_cache_t *
__this_env(void)
{
    static os_env_cache_t env;
    
    return &env;
}


/******************************************************************************/
