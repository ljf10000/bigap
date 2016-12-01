#ifndef __OEM_H_57688f1c133d4ebdae411669109ffdc9__
#define __OEM_H_57688f1c133d4ebdae411669109ffdc9__
/******************************************************************************/
#ifndef OS_CERT_COUNT
#define OS_CERT_COUNT           128
#endif

typedef struct {
    char *cert;
    char *key;
} os_cert_t;

#define __OS_CERT_INITER(_cert, _key)  { \
    .cert   = _cert,    \
    .key    = _key,     \
}   /* end */

typedef struct {
    char *user;
    char *pwdfile;
    char *path;
    char *server;
    char *port;
    char *timeout;
} oem_rsync_t;

#define __OEM_RSYNC_INITER(_user, _pwdfile, _path, _server, _port, _timeout) { \
    .user   = _user,    \
    .pwdfile= _pwdfile, \
    .path   = _path,    \
    .server = _server,  \
    .port   = _port,    \
    .timeout= _timeout, \
}

#ifndef OEM_NAME_LEN
#define OEM_NAME_LEN            31
#endif

typedef struct {
    char *user;
    char *password;
    char *server;
    char *port;

    os_cert_t cert;
} oem_lss_t;

#define __OEM_LSS_INITER(_user, _pass, _server, _port, _lss_cert) { \
    .user       = _user,    \
    .password   = _pass,    \
    .server     = _server,  \
    .port       = _port,    \
    .cert       = _lss_cert,\
}

typedef struct {
    oem_rsync_t rsync;
    oem_lss_t   lss;
} oem_t;

#define __OEM_INITER(_rsync, _lss) { \
    .rsync  = _rsync,   \
    .lss    = _lss,     \
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
static inline bool is_good_oem_type(int id);
static inline char *oem_type_getnamebyid(int id);
static inline int oem_type_getidbyname(const char *name);

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
#include "oem/cert/weos/client.key.c"
#include "oem/cert/weos/client.crt.c"
#include "oem/cert/weos/this_cert_initer.c"
/******************************************************************************/
#ifndef OEM_FILE
#define OEM_FILE    "/etc/.oem"
#endif

extern int
__oem_type(void);

extern oem_t *
__this_oem(void);

extern os_cert_t *
__this_cert(int idx);

static inline char *
oem_vendor(void)
{
    static char *vendor;

    if (NULL==vendor) {
        vendor = oem_type_getnamebyid(__oem_type());
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
#define oem_lss_key         __this_oem()->lss.cert.key
#define oem_lss_cert        __this_oem()->lss.cert.cert
/******************************************************************************/
#endif /* __OEM_H_57688f1c133d4ebdae411669109ffdc9__ */
