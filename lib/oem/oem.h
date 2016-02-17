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
    OEM_CERT_CA,
    OEM_CERT_KEY,
    OEM_CERT_CERT,

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
}
oem_t;

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
enum {
    OEM_DEFT,
    OEM_1,  /* RAYTIGHT */
    
    OEM_END
};

static inline bool
is_good_oem_type(int type)
{
    return is_good_enum(type, OEM_END);
}

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
__oem_type_get_by_vendor(char *vendor)
{
    static struct {
        int type;
        char *vendor;
    } map[] = {
        { .type = OEM_DEFT, .vendor = OEM_NAME},
        { .type = OEM_1,    .vendor = OEM1_NAME},
    };

    int i;

    for (i=0; i<os_count_of(map); i++) {
        if (0==os_strcmp(vendor, map[i].vendor)) {
            return map[i].type;
        }
    }

    return OEM_END;
}

static inline int
__oem_type(void)
{
    int type;
    char vendor[1+OEM_NAME_LEN] = {0};

    if (os_file_exist(OEM_FILE)) {
        os_v_fgets(vendor, OEM_NAME_LEN, OEM_FILE);
        type = __oem_type_get_by_vendor(vendor);
        if (is_good_oem_type(type)) {
            return type;
        }
    }
    
#if IS_PRODUCT_LTEFI_MD1
    os_v_pgets(vendor, OEM_NAME_LEN, "bootm oem.vendor");
#elif IS_PRODUCT_LTEFI_MD2 || IS_PRODUCT_PC
    os_v_pgets(vendor, OEM_NAME_LEN, "benv oem/vendor");
#endif
    type = __oem_type_get_by_vendor(vendor);
    if (is_good_oem_type(type)) {
        return type;
    }

    return OEM_DEFT;
}

static inline oem_t *
__oem_get(void)
{
    static int type = OEM_END;

    if (OEM_END==type) {
        type = __oem_type();
    }

    return &__THIS_OEM[type];
}

#define oem_base_name       __oem_get()->base.name
#define oem_base_mac        __oem_get()->base.mac
#define oem_base_sn         __oem_get()->base.sn
#define oem_base_type       __oem_get()->base.type
#define oem_base_version    __oem_get()->base.version

#define oem_rsync_user      __oem_get()->rsync.user
#define oem_rsync_pwdfile   __oem_get()->rsync.pwdfile
#define oem_rsync_path      __oem_get()->rsync.path
#define oem_rsync_server    __oem_get()->rsync.server
#define oem_rsync_port      __oem_get()->rsync.port
#define oem_rsync_timeout   __oem_get()->rsync.timeout

#define oem_lss_user        __oem_get()->lss.user
#define oem_lss_password    __oem_get()->lss.password
#define oem_lss_server      __oem_get()->lss.server
#define oem_lss_port        __oem_get()->lss.port

#define oem_lss_ca          __oem_get()->cert.cert[OEM_APP_LSS][OEM_CERT_CA]
#define oem_lss_key         __oem_get()->cert.cert[OEM_APP_LSS][OEM_CERT_KEY]
#define oem_lss_cert        __oem_get()->cert.cert[OEM_APP_LSS][OEM_CERT_CERT]

#define oem_lms_ca          __oem_get()->cert.cert[OEM_APP_LMS][OEM_CERT_CA]
#define oem_lms_key         __oem_get()->cert.cert[OEM_APP_LMS][OEM_CERT_KEY]
#define oem_lms_cert        __oem_get()->cert.cert[OEM_APP_LMS][OEM_CERT_CERT]

#define oem_ums_ca          __oem_get()->cert.cert[OEM_APP_UMS][OEM_CERT_CA]
#define oem_ums_key         __oem_get()->cert.cert[OEM_APP_UMS][OEM_CERT_KEY]
#define oem_ums_cert        __oem_get()->cert.cert[OEM_APP_UMS][OEM_CERT_CERT]
/******************************************************************************/
#endif /* __OEM_H_57688f1c133d4ebdae411669109ffdc9__ */
