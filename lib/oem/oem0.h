#ifndef __OEM0_H_1ff3b93b24154b88bca8102eee0e5eb7__
#define __OEM0_H_1ff3b93b24154b88bca8102eee0e5eb7__
/******************************************************************************/
#define OEM_NAME                PRODUCT_VENDOR

#define OEM_LSS_USER            "LSS"
#define OEM_LSS_PWD             "LTEFISecurityServer2012-2015"
#define OEM_LSS_SERVER          "lms4." OEM_NAME ".com"
#define OEM_LSS_PORT            "9999"

#define OEM_LSS_INITER __OEM_LSS_INITER( \
    OEM_LSS_USER,   \
    OEM_LSS_PWD,    \
    OEM_LSS_SERVER, \
    OEM_LSS_PORT)

#define OEM_RSYNC_USER          "rsync"
#define OEM_RSYNC_PWDFILE       "bHRlZmlWMnJzeW5jUFdECg=="
#define OEM_RSYNC_PATH          "version/lte-fi" __SYMBOL_TO_STRING(PRODUCT_LTEFI_VERSION) "/mdboard"
#define OEM_RSYNC_SERVER        "lms3." OEM_NAME ".com"
#define OEM_RSYNC_PORT          "873"
#define OEM_RSYNC_TIMEOUT       "300"

#define OEM_RSYNC_INITER  __OEM_RSYNC_INITER( \
    OEM_RSYNC_USER,     \
    OEM_RSYNC_PWDFILE,  \
    OEM_RSYNC_PATH,     \
    OEM_RSYNC_SERVER,   \
    OEM_RSYNC_PORT,     \
    OEM_RSYNC_TIMEOUT)

#include "cert/deft/lms/ca.crt.c"
#include "cert/deft/lms/client.key.c"
#include "cert/deft/lms/client.crt.c"

#define OEM_CERT_LMS_INITER {               \
    [OEM_CA_CERT]       = OEM_ca_crt,       \
    [OEM_CLIENT_KEY]    = OEM_client_key,   \
    [OEM_CLIENT_CERT]   = OEM_client_crt,   \
}   /* end */

#include "cert/deft/lss/ca.crt.c"
#include "cert/deft/lss/client.key.c"
#include "cert/deft/lss/client.crt.c"

#define OEM_CERT_LSS_INITER {               \
    [OEM_CA_CERT]       = OEM_ca_crt,       \
    [OEM_CLIENT_KEY]    = OEM_client_key,   \
    [OEM_CLIENT_CERT]   = OEM_client_crt,   \
}   /* end */

#include "cert/deft/ums/ca.crt.c"
#include "cert/deft/ums/client.key.c"
#include "cert/deft/ums/client.crt.c"

#define OEM_CERT_UMS_INITER     {           \
    [OEM_CA_CERT]       = OEM_ca_crt,       \
    [OEM_CLIENT_KEY]    = OEM_client_key,   \
    [OEM_CLIENT_CERT]   = OEM_client_crt,   \
}   /* end */

#define OEM_CERT_INITER                     {   \
    .cert = {                                   \
        [OEM_APP_LMS] = OEM_CERT_LMS_INITER,    \
        [OEM_APP_LSS] = OEM_CERT_LSS_INITER,    \
        [OEM_APP_UMS] = OEM_CERT_UMS_INITER,    \
    },                                          \
}   /* end */

#define OEM_INITER __OEM_INITER( \
    OEM_RSYNC_INITER,   \
    OEM_LSS_INITER,     \
    OEM_CERT_INITER)
/******************************************************************************/
#endif /* __OEM0_H_1ff3b93b24154b88bca8102eee0e5eb7__ */
