#ifndef __OEM1_H_35447b03b4ea4e94bae2cefb91393cb7__
#define __OEM1_H_35447b03b4ea4e94bae2cefb91393cb7__
/******************************************************************************/
#define OEM1_NAME               "raytight"

#define OEM1_LSS_SERVER         "its." OEM1_NAME" .com"

#include "oem/cert/raytight/lss/client.key.c"
#include "oem/cert/raytight/lss/client.crt.c"

#define OEM1_LSS_INITER __OEM_LSS_INITER( \
    OEM_LSS_USER,   \
    OEM_LSS_PWD,    \
    OEM1_LSS_SERVER,\
    OEM_LSS_PORT,   \
    __OS_CERT_INITER(OEM_client_crt, OEM_client_key))

#define OEM1_RSYNC_SERVER       "its." OEM1_NAME ".com"

#define OEM1_RSYNC_INITER  __OEM_RSYNC_INITER( \
    OEM_RSYNC_USER,     \
    OEM_RSYNC_PWDFILE,  \
    OEM_RSYNC_PATH,     \
    OEM1_RSYNC_SERVER,  \
    OEM_RSYNC_PORT,     \
    OEM_RSYNC_TIMEOUT)

#define OEM1_INITER __OEM_INITER( \
    OEM1_RSYNC_INITER,   \
    OEM1_LSS_INITER)
/******************************************************************************/
#endif /* __OEM1_H_35447b03b4ea4e94bae2cefb91393cb7__ */
