#ifndef __OEM_H_57688f1c133d4ebdae411669109ffdc9__
#define __OEM_H_57688f1c133d4ebdae411669109ffdc9__
/******************************************************************************/
#ifndef OS_CERT_COUNT
#define OS_CERT_COUNT           100
#endif

typedef struct {
    char *cert;
    char *key;
} os_cert_t;

#define __OS_CERT_INITER(_cert, _key)  { \
    .cert       = _cert,    \
    .key        = _key,     \
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
#include "cert/deft/lss/client.key.c"
#include "cert/deft/lss/client.crt.c"

#define __THIS_CERT_INITER      {
    [0]     = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [1]     = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [2]     = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [3]     = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [4]     = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [5]     = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [6]     = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [7]     = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [8]     = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [9]     = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [20]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [21]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [22]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [23]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [24]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [25]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [26]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [27]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [28]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [29]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [30]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [31]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [32]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [33]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [34]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [35]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [36]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [37]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [38]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [39]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [40]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [41]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [42]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [43]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [44]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [45]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [46]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [47]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [48]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [49]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [40]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [51]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [52]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [53]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [54]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [55]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [56]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [57]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [58]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [59]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [60]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [61]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [62]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [63]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [64]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [65]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [66]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [67]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [68]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [69]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [70]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [71]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [72]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [73]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [74]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [75]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [76]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [77]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [78]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [79]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [80]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [81]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [82]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [83]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [84]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [85]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [86]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [87]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [88]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [89]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [90]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [91]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [92]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [93]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [94]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [95]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [96]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [97]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [98]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
    [99]    = __OS_CERT_INITER(OEM_client_crt, OEM_client_key),    \
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
__this_cert(void);

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
