#ifndef __OEM2_H_4b5158900bc642f682fa6a2bab7b7452__
#define __OEM2_H_4b5158900bc642f682fa6a2bab7b7452__
/******************************************************************************/
#define OEM2_NAME               "autelan"

#define OEM2_LSS_SERVER         "lms4." OEM2_NAME ".com"

#include "oem/cert/autelan/lss/server.crt.c"
#include "oem/cert/autelan/lss/client.key.c"
#include "oem/cert/autelan/lss/client.crt.c"

#define OEM2_LSS_INITER OEM_CLOUD_INITER( \
    OEM_LSS_USER,   \
    OEM_LSS_PWD,    \
    OEM2_LSS_SERVER,\
    OEM_LSS_PORT,   \
    OEM_CERT_INITER)
    
#define OEM2_HMS_SERVER         "hello." OEM2_NAME" .com"

#include "oem/cert/autelan/hms/server.crt.c"
#include "oem/cert/autelan/hms/client.key.c"
#include "oem/cert/autelan/hms/client.crt.c"

#define OEM2_HMS_INITER OEM_CLOUD_INITER( \
    OEM_HMS_USER,   \
    OEM_HMS_PWD,    \
    OEM2_HMS_SERVER,\
    OEM_HMS_PORT,   \
    OEM_CERT_INITER)

#define OEM2_RSYNC_SERVER       "lms3." OEM2_NAME ".com"

#define OEM2_RSYNC_INITER  __OEM_RSYNC_INITER( \
    OEM_RSYNC_USER,     \
    OEM_RSYNC_PWDFILE,  \
    OEM_RSYNC_PATH,     \
    OEM2_RSYNC_SERVER,  \
    OEM_RSYNC_PORT,     \
    OEM_RSYNC_TIMEOUT)

#define OEM2_INITER __OEM_INITER( \
    OEM2_RSYNC_INITER,  \
    OEM2_LSS_INITER,    \
    OEM2_HMS_INITER)
/******************************************************************************/
#endif /* __OEM2_H_4b5158900bc642f682fa6a2bab7b7452__ */
