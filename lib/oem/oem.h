#ifndef __OEM_H_57688f1c133d4ebdae411669109ffdc9__
#define __OEM_H_57688f1c133d4ebdae411669109ffdc9__
/******************************************************************************/
#include "product/product.h"

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

#ifndef OEM_MAC_LEN
#define OEM_MAC_LEN     31
#endif

#ifndef OEM_SN_LEN
#define OEM_SN_LEN      63
#endif

#ifndef OEM_TYPE_LEN
#define OEM_TYPE_LEN    31
#endif

#ifndef OEM_VERSION_LEN
#define OEM_VERSION_LEN 31
#endif

typedef struct {
    char name[1+OEM_NAME_LEN];
    char mac[1+OEM_MAC_LEN];
    char sn[1+OEM_SN_LEN];
    char type[1+OEM_TYPE_LEN];
    char version[1+OEM_VERSION_LEN];
} oem_base_t;

#define __OEM_BASE_INITER(_name, _mac, _sn, _type, _version) { \
    .name   = _name,    \
    .mac    = _mac,     \
    .sn     = _sn,      \
    .type   = _type,    \
    .version= _version, \
}

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

    OEM_APP_END
};

#ifndef OEM_CERT_LEN
#define OEM_CERT_LEN     (8*1024-1)
#endif

typedef struct {
    char cert[OEM_APP_END][OEM_CERT_END][1+OEM_CERT_LEN];
} oem_cert_t;

typedef struct {
    oem_base_t  base;
    oem_rsync_t rsync;
    oem_lss_t   lss;
    oem_cert_t  cert;
} oem_t;

#define __OEM_INITER(_base, _rsync, _lss, _cert) { \
    .base   = _base,    \
    .rsync  = _rsync,   \
    .lss    = _lss,     \
    .cert   = _cert,    \
}
/******************************************************************************/
#include "deft.h"
#include "oem1.h"
/******************************************************************************/
#define __XLIST_OEM(_)          \
    _(OEM_DEFT, 0, OEM_NAME),   \
    _(OEM_1,    1, OEM1_NAME),  \
    /* end */

static inline bool is_good_oem_type(int id);
static inline char *oem_type_string(int id);
static inline int oem_type_idx(char *type);
DECLARE_ENUM(oem_type, __XLIST_OEM, OEM_END);

#if 1 /* just for sourceinsight */
#define OEM_DEFT    OEM_DEFT
#define OEM_1       OEM_1
#define OEM_END     OEM_END
#endif /* just for sourceinsight */

#define DECLARE_FAKE_OEM    extern oem_t __THIS_OEM[]
#define DECLARE_REAL_OEM    oem_t __THIS_OEM[OEM_END] = { \
    [OEM_DEFT]  = OEM_INITER,   \
    [OEM_1]     = OEM1_INITER,  \
}

#ifdef __BUSYBOX__
#   define DECLARE_OEM      DECLARE_FAKE_OEM
#else
#   define DECLARE_OEM      DECLARE_REAL_OEM
#endif

DECLARE_FAKE_OEM;
/******************************************************************************/
#ifndef OEM_FILE
#define OEM_FILE    "/etc/.oem"
#endif

static inline int
__oem_type(void)
{
    int type;
    char vendor[1+OEM_NAME_LEN] = {0};

    if (os_file_exist(OEM_FILE)) {
        os_v_fgets(vendor, OEM_NAME_LEN, OEM_FILE);
        type = oem_type_idx(vendor);
        if (is_good_oem_type(type)) {
            return type;
        }
    }
    
#if IS_PRODUCT_LTEFI_MD1
    os_v_pgets(vendor, OEM_NAME_LEN, "bootm oem.vendor");
#elif IS_PRODUCT_LTEFI_MD2 || IS_PRODUCT_PC
    os_v_pgets(vendor, OEM_NAME_LEN, "benv oem/vendor");
#endif
    type = oem_type_idx(vendor);
    if (is_good_oem_type(type)) {
        return type;
    }

    return OEM_DEFT;
}

static inline oem_t *
__this_oem(void)
{
    static int type = OEM_END;

    if (OEM_END==type) {
        type = __oem_type();
    }

    return &__THIS_OEM[type];
}

#define oem_base_name       __this_oem()->base.name
#define oem_base_mac        __this_oem()->base.mac
#define oem_base_sn         __this_oem()->base.sn
#define oem_base_type       __this_oem()->base.type
#define oem_base_version    __this_oem()->base.version

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
