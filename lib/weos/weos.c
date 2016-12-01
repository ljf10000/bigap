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
LIB_INITER;
/******************************************************************************/
#include "autoarray.c"
#include "blob.c"
#include "channel.c"
#include "coroutine.c"
#include "cqueue.c"
#include "dll.c"
#include "env.c"
#include "fd.c"
#include "file.c"
#include "json.c"
#include "loop.c"
#include "pipe.c"
#include "slice.c"
#include "timer.c"

#include "boot.c"
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

#include "cert/deft/lss/client.key.c"
#include "cert/deft/lss/client.crt.c"

#define __THIS_CERT_INITER      {
    [0]     = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [1]     = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [2]     = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [3]     = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [4]     = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [5]     = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [6]     = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [7]     = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [8]     = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [9]     = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [20]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [21]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [22]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [23]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [24]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [25]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [26]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [27]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [28]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [29]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [30]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [31]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [32]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [33]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [34]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [35]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [36]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [37]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [38]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [39]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [40]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [41]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [42]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [43]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [44]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [45]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [46]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [47]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [48]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [49]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [40]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [51]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [52]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [53]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [54]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [55]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [56]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [57]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [58]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [59]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [60]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [61]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [62]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [63]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [64]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [65]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [66]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [67]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [68]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [69]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [70]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [71]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [72]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [73]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [74]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [75]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [76]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [77]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [78]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [79]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [80]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [81]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [82]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [83]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [84]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [85]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [86]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [87]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [88]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [89]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [90]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [91]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [92]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [93]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [94]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [95]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [96]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [97]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [98]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [99]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
}   /* end */

os_cert_t *
__this_cert(void)
{
    static os_cert_t cert[OEM_CERT_COUNT] = __THIS_CERT_INITER;

    return cert;
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
