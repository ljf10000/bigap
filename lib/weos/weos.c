/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      libweos
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     libweos
#endif

#define __RUNAS__       1 
#define __LIB__
#include "utils.h"
#include "benv/benv.h"
LIB_INITER;
/******************************************************************************/
#include "app.c"
#include "boot.c"
/******************************************************************************/
bool __THIS_DEAMON;

int
__oem_type(void)
{
    static int type = -1;

    if (false==IS_GOOD_ENUM(type, OEM_T_END)) {
        char vendor[1+OEM_NAME_LEN] = {0};

        if (os_file_exist(OEM_FILE)) {
            os_v_fgets(vendor, OEM_NAME_LEN, OEM_FILE);
            type = oem_type_getidbyname(vendor);
            if (is_good_oem_type(type)) {
                return type;
            }
        }
        
#if IS_PRODUCT_LTEFI_MD1
        os_v_pgets(vendor, OEM_NAME_LEN, "bootm product.vendor");
#elif IS_PRODUCT_LTEFI_MD_PARTITION_B || IS_PRODUCT_PC
        os_v_pgets(vendor, OEM_NAME_LEN, "benv infos/product/vendor");
#endif
        type = oem_type_getidbyname(vendor);
        if (false==is_good_oem_type(type)) {
            type = OEM_T_DEFT;
        }
    }
    
    return type;
}

oem_t *
__this_oem(void)
{
    static oem_t oem[OEM_T_END] = __THIS_OEM_INITER;

    return &oem[__oem_type()];
}

#include "oem/cert/weos/this_cert_initer.c"
os_cert_t *
__this_cert(int idx)
{
    static os_cert_t cert[OS_CERT_COUNT] = __THIS_CERT_INITERS;

    if (IS_GOOD_ENUM(idx, OS_CERT_COUNT)) {
        return &cert[idx];
    } else {
        return &cert[0];
    }
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
