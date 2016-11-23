#ifndef __OEM_H_57688f1c133d4ebdae411669109ffdc9__
#define __OEM_H_57688f1c133d4ebdae411669109ffdc9__
/******************************************************************************/
#ifndef OEM_RSYNC_USER_LEN
#define OEM_RSYNC_USER_LEN      31
#endif

#ifndef OEM_RSYNC_PWDFILE_LEN
#define OEM_RSYNC_PWDFILE_LEN   127
#endif

#ifndef OEM_RSYNC_PATH_LEN
#define OEM_RSYNC_PATH_LEN      127
#endif

#ifndef OEM_RSYNC_SERVER_LEN
#define OEM_RSYNC_SERVER_LEN    31
#endif

#ifndef OEM_RSYNC_PORT_LEN
#define OEM_RSYNC_PORT_LEN      15
#endif

#ifndef OEM_RSYNC_TIMEOUT_LEN
#define OEM_RSYNC_TIMEOUT_LEN   15
#endif

typedef struct {
    char user[1+OEM_RSYNC_USER_LEN];
    char pwdfile[1+OEM_RSYNC_PWDFILE_LEN];
    char path[1+OEM_RSYNC_PATH_LEN];
    char server[1+OEM_RSYNC_SERVER_LEN];
    char port[1+OEM_RSYNC_PORT_LEN];
    char timeout[1+OEM_RSYNC_TIMEOUT_LEN];
} oem_rsync_t;

#define __OEM_RSYNC_INITER(_user, _pwdfile, _path, _server, _port, _timeout) { \
    .user   = _user,    \
    .pwdfile= _pwdfile, \
    .path   = _path,    \
    .server = _server,  \
    .port   = _port,    \
    .timeout= _timeout, \
}

#ifndef OEM_LSS_USER_LEN
#define OEM_LSS_USER_LEN        31
#endif

#ifndef OEM_LSS_PASSWORD_LEN
#define OEM_LSS_PASSWORD_LEN    127
#endif

#ifndef OEM_LSS_SERVER_LEN
#define OEM_LSS_SERVER_LEN      31
#endif

#ifndef OEM_LSS_PORT_LEN
#define OEM_LSS_PORT_LEN        15
#endif

typedef struct {
    char user[1+OEM_LSS_USER_LEN];
    char password[1+OEM_LSS_PASSWORD_LEN];
    char server[1+OEM_LSS_SERVER_LEN];
    char port[1+OEM_LSS_PORT_LEN];
} oem_lss_t;

#define __OEM_LSS_INITER(_user, _pass, _server, _port) { \
    .user       = _user,    \
    .password   = _pass,    \
    .server     = _server,  \
    .port       = _port,    \
}

#ifndef OEM_NAME_LEN
#define OEM_NAME_LEN    31
#endif

enum {
    OEM_CA_CERT,
    OEM_CLIENT_KEY,
    OEM_CLIENT_CERT,
#if 0
    OEM_CA_KEY,
    OEM_SERVER_KEY,
    OEM_SERVER_CERT,
#endif

    OEM_CERT_END
};

enum {
    OEM_APP_LMS,
    OEM_APP_LSS,
    OEM_APP_UMS,
#if 0
    OEM_APP_NSQ,
#endif
    OEM_APP_END
};

#ifndef OEM_CERT_LEN
#define OEM_CERT_LEN     (8*1024-1)
#endif

typedef struct {
    char cert[OEM_APP_END][OEM_CERT_END][1+OEM_CERT_LEN];
} oem_cert_t;

typedef struct {
    oem_rsync_t rsync;
    oem_lss_t   lss;
    oem_cert_t  cert;
} oem_t;

#define __OEM_INITER(_rsync, _lss, _cert) { \
    .rsync  = _rsync,   \
    .lss    = _lss,     \
    .cert   = _cert,    \
}
/******************************************************************************/
#include "oem0.h"
#include "oem1.h"
#include "oem2.h"
/******************************************************************************/
#if 1
#define OEM_T_ENUM_MAPPER(_)        \
    _(OEM_T_DEFT, 0, OEM_NAME),     \
    _(OEM_T_1,    1, OEM1_NAME),    \
    _(OEM_T_2,    2, OEM2_NAME),    \
    /* end */
DECLARE_ENUM(oem_type, OEM_T_ENUM_MAPPER, OEM_T_END);

static inline enum_ops_t *oem_type_ops(void);

#define OEM_T_DEFT  OEM_T_DEFT
#define OEM_T_1     OEM_T_1
#define OEM_T_2     OEM_T_2
#define OEM_T_END   OEM_T_END
#endif

#define __THIS_OEM_INITER       { \
    [OEM_T_DEFT]  = OEM_INITER,   \
    [OEM_T_1]     = OEM1_INITER,  \
    [OEM_T_2]     = OEM2_INITER,  \
}   /* end */
/******************************************************************************/
#ifndef OEM_FILE
#define OEM_FILE    "/etc/.oem"
#endif

extern int
__oem_type(void);

extern oem_t *
__this_oem(void);

static inline char *
oem_vendor(void)
{
    static char *vendor;

    if (NULL==vendor) {
        vendor = oem_type_ops()->getname(__oem_type());
    }

    return vendor;
}

#define oem_rsync_user      __this_oem()->rsync.user
#define oem_rsync_pwdfile   __this_oem()->rsync.pwdfile
#define oem_rsync_path      __this_oem()->rsync.path
#define oem_rsync_server    __this_oem()->rsync.server
#define oem_rsync_port      __this_oem()->rsync.port
#define oem_rsync_timeout   __this_oem()->rsync.timeout

#define oem_lss_user        __this_oem()->lss.user
#define oem_lss_password    __this_oem()->lss.password
#define oem_lss_server      __this_oem()->lss.server
#define oem_lss_port        __this_oem()->lss.port

#define oem_lss_ca          __this_oem()->cert.cert[OEM_APP_LSS][OEM_CA_CERT]
#define oem_lss_key         __this_oem()->cert.cert[OEM_APP_LSS][OEM_CLIENT_KEY]
#define oem_lss_cert        __this_oem()->cert.cert[OEM_APP_LSS][OEM_CLIENT_CERT]

#define oem_lms_ca          __this_oem()->cert.cert[OEM_APP_LMS][OEM_CA_CERT]
#define oem_lms_key         __this_oem()->cert.cert[OEM_APP_LMS][OEM_CLIENT_KEY]
#define oem_lms_cert        __this_oem()->cert.cert[OEM_APP_LMS][OEM_CLIENT_CERT]

#define oem_ums_ca          __this_oem()->cert.cert[OEM_APP_UMS][OEM_CA_CERT]
#define oem_ums_key         __this_oem()->cert.cert[OEM_APP_UMS][OEM_CLIENT_KEY]
#define oem_ums_cert        __this_oem()->cert.cert[OEM_APP_UMS][OEM_CLIENT_CERT]
/******************************************************************************/
#endif /* __OEM_H_57688f1c133d4ebdae411669109ffdc9__ */
