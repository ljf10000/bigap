#ifndef __OEM2_H_4b5158900bc642f682fa6a2bab7b7452__
#define __OEM2_H_4b5158900bc642f682fa6a2bab7b7452__
/******************************************************************************/
#define OEM2_NAME               "autelan"

#define OEM2_LSS_SERVER         "lms4." OEM2_NAME ".com"

#define OEM2_LSS_INITER __OEM_LSS_INITER( \
    OEM_LSS_USER,   \
    OEM_LSS_PWD,    \
    OEM2_LSS_SERVER,\
    OEM_LSS_PORT)

#define OEM2_RSYNC_SERVER       "lms3." OEM2_NAME ".com"

#define OEM2_RSYNC_INITER  __OEM_RSYNC_INITER( \
    OEM_RSYNC_USER,     \
    OEM_RSYNC_PWDFILE,  \
    OEM_RSYNC_PATH,     \
    OEM2_RSYNC_SERVER,  \
    OEM_RSYNC_PORT,     \
    OEM_RSYNC_TIMEOUT)

#include "cert/autelan/lms/ca.crt.c"
#include "cert/autelan/lms/client.key.c"
#include "cert/autelan/lms/client.crt.c"

#define OEM2_CERT_LMS_INITER {              \
    [OEM_CA_CERT]       = OEM_ca_crt,       \
    [OEM_CLIENT_KEY]    = OEM_client_key,   \
    [OEM_CLIENT_CERT]   = OEM_client_crt,   \
}   /* end */

#include "cert/autelan/lss/ca.crt.c"
#include "cert/autelan/lss/client.key.c"
#include "cert/autelan/lss/client.crt.c"

#define OEM2_CERT_LSS_INITER {              \
    [OEM_CA_CERT]       = OEM_ca_crt,       \
    [OEM_CLIENT_KEY]    = OEM_client_key,   \
    [OEM_CLIENT_CERT]   = OEM_client_crt,   \
}   /* end */

#include "cert/autelan/ums/ca.crt.c"
#include "cert/autelan/ums/client.key.c"
#include "cert/autelan/ums/client.crt.c"

#define OEM2_CERT_UMS_INITER     {          \
    [OEM_CA_CERT]       = OEM_ca_crt,       \
    [OEM_CLIENT_KEY]    = OEM_client_key,   \
    [OEM_CLIENT_CERT]   = OEM_client_crt,   \
}   /* end */

#define OEM2_CERT_INITER                     {  \
    .cert = {                                   \
        [OEM_APP_LMS] = OEM2_CERT_LMS_INITER,   \
        [OEM_APP_LSS] = OEM2_CERT_LSS_INITER,   \
        [OEM_APP_UMS] = OEM2_CERT_UMS_INITER,   \
    },                                          \
}   /* end */

#define OEM2_INITER __OEM_INITER( \
    OEM2_RSYNC_INITER,  \
    OEM2_LSS_INITER,    \
    OEM2_CERT_INITER)
/******************************************************************************/
#endif /* __OEM2_H_4b5158900bc642f682fa6a2bab7b7452__ */
