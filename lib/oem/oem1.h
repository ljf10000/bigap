#ifndef __OEM1_H_35447b03b4ea4e94bae2cefb91393cb7__
#define __OEM1_H_35447b03b4ea4e94bae2cefb91393cb7__
/******************************************************************************/
#define OEM1_NAME               "raytight"

#define OEM1_BASE_INITER __OEM_BASE_INITER( \
    OEM1_NAME,  \
    OEM_MAC,    \
    OEM_SN,     \
    OEM_TYPE,   \
    OEM_VERSION)

#define OEM1_LSS_SERVER         "its.raytight.com"

#define OEM1_LSS_INITER __OEM_LSS_INITER( \
    OEM_LSS_USER,   \
    OEM_LSS_PWD,    \
    OEM1_LSS_SERVER,\
    OEM_LSS_PORT)

#define OEM1_RSYNC_SERVER       "its.raytight.com"

#define OEM1_RSYNC_INITER  __OEM_RSYNC_INITER( \
    OEM_RSYNC_USER,     \
    OEM_RSYNC_PWDFILE,  \
    OEM_RSYNC_PATH,     \
    OEM1_RSYNC_SERVER,  \
    OEM_RSYNC_PORT,     \
    OEM_RSYNC_TIMEOUT)

#include "cert/raytight/lms/ca.crt.c"
#define OEM1_lms_ca_crt     OEM1_lms_ca_crt

#include "cert/raytight/lms/client.key.c"
#define OEM1_lms_client_key OEM1_lms_client_key

#include "cert/raytight/lms/client.crt.c"
#define OEM1_lms_client_crt OEM1_lms_client_crt

#define OEM1_CERT_LMS_INITER {              \
    [OEM_CERT_CA]   = OEM1_lms_ca_crt,      \
    [OEM_CERT_KEY]  = OEM1_lms_client_key,  \
    [OEM_CERT_CERT] = OEM1_lms_client_crt,  \
}   /* end */

#include "cert/raytight/lss/ca.crt.c"
#define OEM1_lss_ca_crt     OEM1_lss_ca_crt

#include "cert/raytight/lss/client.key.c"
#define OEM1_lss_client_key OEM1_lss_client_key

#include "cert/raytight/lss/client.crt.c"
#define OEM1_lss_client_crt OEM1_lss_client_crt

#define OEM1_CERT_LSS_INITER {              \
    [OEM_CERT_CA]   = OEM1_lss_ca_crt,      \
    [OEM_CERT_KEY]  = OEM1_lss_client_key,  \
    [OEM_CERT_CERT] = OEM1_lss_client_crt,  \
}   /* end */

#include "cert/raytight/ums/ca.crt.c"
#define OEM1_ums_ca_crt     OEM1_ums_ca_crt

#include "cert/raytight/ums/client.key.c"
#define OEM1_ums_client_key OEM1_ums_client_key

#include "cert/raytight/ums/client.crt.c"
#define OEM1_ums_client_crt OEM1_ums_client_crt

#define OEM1_CERT_UMS_INITER     {  \
    [OEM_CERT_CA]   = OEM1_ums_ca_crt,  \
    [OEM_CERT_KEY]  = OEM1_ums_client_key, \
    [OEM_CERT_CERT] = OEM1_ums_client_crt,\
}   /* end */

#define OEM1_CERT_INITER                    {   \
    .cert = {                                   \
        [OEM_APP_LMS] = OEM1_CERT_LMS_INITER,   \
        [OEM_APP_LSS] = OEM1_CERT_LSS_INITER,   \
        [OEM_APP_UMS] = OEM1_CERT_UMS_INITER,   \
    },                                          \
}

#define OEM1_INITER __OEM_INITER( \
    OEM1_BASE_INITER,    \
    OEM1_RSYNC_INITER,   \
    OEM1_LSS_INITER,     \
    OEM1_CERT_INITER)
/******************************************************************************/
#endif /* __OEM1_H_35447b03b4ea4e94bae2cefb91393cb7__ */
