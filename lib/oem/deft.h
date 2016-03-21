#ifndef __DEFT_H_1ff3b93b24154b88bca8102eee0e5eb7__
#define __DEFT_H_1ff3b93b24154b88bca8102eee0e5eb7__
/******************************************************************************/
#define OEM_NAME    "default"
#define OEM_MAC     ""
#define OEM_SN      ""
#define OEM_TYPE    ""
#define OEM_VERSION ""

#define OEM_BASE_INITER __OEM_BASE_INITER( \
    OEM_NAME,   \
    OEM_MAC,    \
    OEM_SN,     \
    OEM_TYPE,   \
    OEM_VERSION)

#define OEM_LSS_USER            "LSS"
#define OEM_LSS_PWD             "LTEFISecurityServer2012-2015"
#define OEM_LSS_SERVER          "lms2.xxx.com"
#define OEM_LSS_PORT            "9999"

#define OEM_LSS_INITER __OEM_LSS_INITER( \
    OEM_LSS_USER,   \
    OEM_LSS_PWD,    \
    OEM_LSS_SERVER, \
    OEM_LSS_PORT)

#define OEM_RSYNC_USER          "rsync"
#define OEM_RSYNC_PWDFILE       "bHRlZmlWMnJzeW5jUFdECg=="
#define OEM_RSYNC_PATH          "version/lte-fi2/mdboard"
#define OEM_RSYNC_SERVER        "lms3.xxx.com"
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
#define OEM_lms_ca_crt      OEM_lms_ca_crt

#include "cert/deft/lms/client.key.c"
#define OEM_lms_client_key  OEM_lms_client_key

#include "cert/deft/lms/client.crt.c"
#define OEM_lms_client_crt  OEM_lms_client_crt

#define OEM_CERT_LMS_INITER {               \
    [OEM_CERT_CA]   = OEM_lms_ca_crt,       \
    [OEM_CERT_KEY]  = OEM_lms_client_key,   \
    [OEM_CERT_CERT] = OEM_lms_client_crt,   \
}   /* end */

#include "cert/deft/lss/ca.crt.c"
#define OEM_lss_ca_crt      OEM_lss_ca_crt

#include "cert/deft/lss/client.key.c"
#define OEM_lss_client_key  OEM_lss_client_key

#include "cert/deft/lss/client.crt.c"
#define OEM_lss_client_crt  OEM_lss_client_crt

#define OEM_CERT_LSS_INITER {               \
    [OEM_CERT_CA]   = OEM_lss_ca_crt,       \
    [OEM_CERT_KEY]  = OEM_lss_client_key,   \
    [OEM_CERT_CERT] = OEM_lss_client_crt,   \
}   /* end */

#include "cert/deft/ums/ca.crt.c"
#define OEM_ums_ca_crt      OEM_ums_ca_crt

#include "cert/deft/ums/client.key.c"
#define OEM_ums_client_key  OEM_ums_client_key

#include "cert/deft/ums/client.crt.c"
#define OEM_ums_client_crt  OEM_ums_client_crt

#define OEM_CERT_UMS_INITER     {           \
    [OEM_CERT_CA]   = OEM_ums_ca_crt,       \
    [OEM_CERT_KEY]  = OEM_ums_client_key,   \
    [OEM_CERT_CERT] = OEM_ums_client_crt,   \
}   /* end */

#define OEM_CERT_INITER                     {   \
    .cert = {                                   \
        [OEM_APP_LMS] = OEM_CERT_LMS_INITER,    \
        [OEM_APP_LSS] = OEM_CERT_LSS_INITER,    \
        [OEM_APP_UMS] = OEM_CERT_UMS_INITER,    \
    },                                          \
}   /* end */

#define OEM_INITER __OEM_INITER( \
    OEM_BASE_INITER,    \
    OEM_RSYNC_INITER,   \
    OEM_LSS_INITER,     \
    OEM_CERT_INITER)
/******************************************************************************/
#endif /* __DEFT_H_1ff3b93b24154b88bca8102eee0e5eb7__ */
