#ifndef __OEM0_H_1ff3b93b24154b88bca8102eee0e5eb7__
#define __OEM0_H_1ff3b93b24154b88bca8102eee0e5eb7__
/******************************************************************************/
#define OEM_NAME                PRODUCT_Vendor

#define OEM_LSS_USER            "LSS"
#define OEM_LSS_PWD             "LTEFISecurityServer2012-2015"
#define OEM_LSS_SERVER          "lms4." PRODUCT_VENDOR ".com"
#define OEM_LSS_PORT            "9999"

#include "oem/cert/deft/lss/server.crt.c"
#include "oem/cert/deft/lss/client.key.c"
#include "oem/cert/deft/lss/client.crt.c"

#define OEM_LSS_INITER OEM_CLOUD_INITER( \
    OEM_LSS_USER,   \
    OEM_LSS_PWD,    \
    OEM_LSS_SERVER, \
    OEM_LSS_PORT,   \
    OEM_CERT_INITER)

#define OEM_HMS_USER            "HMS"
#define OEM_HMS_PWD             "HelloMicroServer2016-2018"
#define OEM_HMS_SERVER          "hello." PRODUCT_VENDOR ".com"
#define OEM_HMS_PORT            "8740"

#include "oem/cert/deft/hms/server.crt.c"
#include "oem/cert/deft/hms/client.key.c"
#include "oem/cert/deft/hms/client.crt.c"

#define OEM_HMS_INITER OEM_CLOUD_INITER( \
    OEM_HMS_USER,   \
    OEM_HMS_PWD,    \
    OEM_HMS_SERVER, \
    OEM_HMS_PORT,   \
    OEM_CERT_INITER)

#define OEM_RSYNC_USER          "rsync"
#define OEM_RSYNC_PWDFILE       "bHRlZmlWMnJzeW5jUFdECg=="
#define OEM_RSYNC_PATH          "version/lte-fi" __SYMBOL_TO_STRING(PRODUCT_LTEFI_VERSION) "/mdboard"
#define OEM_RSYNC_SERVER        "lms3." PRODUCT_VENDOR ".com"
#define OEM_RSYNC_PORT          "873"
#define OEM_RSYNC_TIMEOUT       "300"

#define OEM_RSYNC_INITER  __OEM_RSYNC_INITER( \
    OEM_RSYNC_USER,     \
    OEM_RSYNC_PWDFILE,  \
    OEM_RSYNC_PATH,     \
    OEM_RSYNC_SERVER,   \
    OEM_RSYNC_PORT,     \
    OEM_RSYNC_TIMEOUT)

#define OEM_INITER __OEM_INITER( \
    OEM_RSYNC_INITER,   \
    OEM_LSS_INITER,     \
    OEM_HMS_INITER)
/******************************************************************************/
#endif /* __OEM0_H_1ff3b93b24154b88bca8102eee0e5eb7__ */
