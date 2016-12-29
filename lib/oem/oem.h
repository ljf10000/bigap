#ifndef __OEM_H_57688f1c133d4ebdae411669109ffdc9__
#define __OEM_H_57688f1c133d4ebdae411669109ffdc9__
/******************************************************************************/
#ifdef __APP__
#ifndef OS_CERT_COUNT
#define OS_CERT_COUNT           32
#endif

#define is_good_os_cert(_cid)   is_good_enum(_cid, OS_CERT_COUNT)

typedef struct {
    char *cert;     // curl --cert
    char *key;      // curl --key
    char *cacert;   // curl --cacert, it is server.crt
} os_cert_t;

#define __OS_CERT_INITER(_cert, _key, _cacert)  { \
    .cert   = _cert,    \
    .key    = _key,     \
    .cacert = _cacert,  \
}   /* end */

#define OEM_CERT_INITER __OS_CERT_INITER(OEM_client_crt, OEM_client_key, OEM_server_crt)

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
} oem_cloud_t;

#define OEM_CLOUD_INITER(_user, _pass, _server, _port, _lss_cert) { \
    .user       = _user,    \
    .password   = _pass,    \
    .server     = _server,  \
    .port       = _port,    \
    .cert       = _lss_cert,\
}

typedef struct {
    oem_rsync_t rsync;
    oem_cloud_t lss;
    oem_cloud_t hms;
} oem_t;

#define __OEM_INITER(_rsync, _lss, _hms) { \
    .rsync  = _rsync,   \
    .lss    = _lss,     \
    .hms    = _hms,     \
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
#define oem_lss_cacert      __this_oem()->lss.cert.cacert
#define oem_lss_key         __this_oem()->lss.cert.key
#define oem_lss_cert        __this_oem()->lss.cert.cert

#define oem_hms_user        __this_oem()->hms.user
#define oem_hms_password    __this_oem()->hms.password
#define oem_hms_server      __this_oem()->hms.server
#define oem_hms_port        __this_oem()->hms.port
#define oem_hms_cacert      __this_oem()->hms.cert.cacert
#define oem_hms_key         __this_oem()->hms.cert.key
#define oem_hms_cert        __this_oem()->hms.cert.cert
#endif
/******************************************************************************/
#endif /* __OEM_H_57688f1c133d4ebdae411669109ffdc9__ */
