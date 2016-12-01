#ifndef __FCOOKIE_H_39d80845ac764a84bfdfa3166c751c51__
#define __FCOOKIE_H_39d80845ac764a84bfdfa3166c751c51__
/******************************************************************************/
#include "oem/oem.h"

/*
* NOT depend actions
*/

enum {
    FCOOKIE_BEGIN,
    
    FCOOKIE_FILE    = FCOOKIE_BEGIN,
    FCOOKIE_DOMAIN,

    FCOOKIE_END
};

static inline bool
is_good_fcookie_id(int id)
{
    return id >= FCOOKIE_BEGIN && id < FCOOKIE_END;
}

enum {
    FCOOKIE_CERT_INVALID    = 0,

    FCOOKIE_FILE_BEGIN      = 1,
    FCOOKIE_RSYNC_PWDFILE   = FCOOKIE_FILE_BEGIN,
    
    FCOOKIE_LSS_CERT        = 2,
    FCOOKIE_LSS_KEY         = 3,
    
    FCOOKIE_CERT_BEGIN      = 4,
    FCOOKIE_KEY_BEGIN       = FCOOKIE_CERT_BEGIN + OS_CERT_COUNT,
    FCOOKIE_COOKIE_FILE_END = FCOOKIE_KEY_BEGIN + OS_CERT_COUNT,
};

static inline bool
is_good_fcookie_file_id(int id)
{
    return id >= FCOOKIE_FILE_BEGIN && id < FCOOKIE_COOKIE_FILE_END;
}

typedef struct {
    char *info;
    int mode;
} fcookie_file_t;

#define __FCOOKIE_FILE_ENTRY(_info, _mode) {  \
    .info = _info,  \
    .mode = _mode,  \
}   /* end */

#define FCOOKIE_FILE_ENTRY(_info)   __FCOOKIE_FILE_ENTRY(_info, 0)
#define __FCOOKIE_CERT_ENTRY(_idx)  [FCOOKIE_CERT_BEGIN+_idx] = FCOOKIE_FILE_ENTRY(__this_cert(_idx)->cert)
#define __FCOOKIE_KEY_ENTRY(_idx)   [FCOOKIE_KEY_BEGIN+_idx] = FCOOKIE_FILE_ENTRY(__this_cert(_idx)->key)
#define FCOOKIE_CERT_ENTRY(_idx)    __FCOOKIE_CERT_ENTRY(_idx), __FCOOKIE_KEY_ENTRY(_idx)

#define FCOOKIE_FILES    { \
    [FCOOKIE_RSYNC_PWDFILE] = __FCOOKIE_FILE_ENTRY(oem_rsync_pwdfile, 0x600), \
    [FCOOKIE_LSS_CERT]      = FCOOKIE_FILE_ENTRY(oem_lss_cert),  \
    [FCOOKIE_LSS_KEY]       = FCOOKIE_FILE_ENTRY(oem_lss_key),   \
    FCOOKIE_CERT_ENTRY(0), \
    FCOOKIE_CERT_ENTRY(1), \
    FCOOKIE_CERT_ENTRY(2), \
    FCOOKIE_CERT_ENTRY(3), \
    FCOOKIE_CERT_ENTRY(4), \
    FCOOKIE_CERT_ENTRY(5), \
    FCOOKIE_CERT_ENTRY(6), \
    FCOOKIE_CERT_ENTRY(7), \
    FCOOKIE_CERT_ENTRY(8), \
    FCOOKIE_CERT_ENTRY(9), \
    FCOOKIE_CERT_ENTRY(10), \
    FCOOKIE_CERT_ENTRY(11), \
    FCOOKIE_CERT_ENTRY(12), \
    FCOOKIE_CERT_ENTRY(13), \
    FCOOKIE_CERT_ENTRY(14), \
    FCOOKIE_CERT_ENTRY(15), \
    FCOOKIE_CERT_ENTRY(16), \
    FCOOKIE_CERT_ENTRY(17), \
    FCOOKIE_CERT_ENTRY(18), \
    FCOOKIE_CERT_ENTRY(19), \
    FCOOKIE_CERT_ENTRY(20), \
    FCOOKIE_CERT_ENTRY(21), \
    FCOOKIE_CERT_ENTRY(22), \
    FCOOKIE_CERT_ENTRY(23), \
    FCOOKIE_CERT_ENTRY(24), \
    FCOOKIE_CERT_ENTRY(25), \
    FCOOKIE_CERT_ENTRY(26), \
    FCOOKIE_CERT_ENTRY(27), \
    FCOOKIE_CERT_ENTRY(28), \
    FCOOKIE_CERT_ENTRY(29), \
    FCOOKIE_CERT_ENTRY(30), \
    FCOOKIE_CERT_ENTRY(31), \
    FCOOKIE_CERT_ENTRY(32), \
    FCOOKIE_CERT_ENTRY(33), \
    FCOOKIE_CERT_ENTRY(34), \
    FCOOKIE_CERT_ENTRY(35), \
    FCOOKIE_CERT_ENTRY(36), \
    FCOOKIE_CERT_ENTRY(37), \
    FCOOKIE_CERT_ENTRY(38), \
    FCOOKIE_CERT_ENTRY(39), \
    FCOOKIE_CERT_ENTRY(40), \
    FCOOKIE_CERT_ENTRY(41), \
    FCOOKIE_CERT_ENTRY(42), \
    FCOOKIE_CERT_ENTRY(43), \
    FCOOKIE_CERT_ENTRY(44), \
    FCOOKIE_CERT_ENTRY(45), \
    FCOOKIE_CERT_ENTRY(46), \
    FCOOKIE_CERT_ENTRY(47), \
    FCOOKIE_CERT_ENTRY(48), \
    FCOOKIE_CERT_ENTRY(49), \
    FCOOKIE_CERT_ENTRY(50), \
    FCOOKIE_CERT_ENTRY(51), \
    FCOOKIE_CERT_ENTRY(52), \
    FCOOKIE_CERT_ENTRY(53), \
    FCOOKIE_CERT_ENTRY(54), \
    FCOOKIE_CERT_ENTRY(55), \
    FCOOKIE_CERT_ENTRY(56), \
    FCOOKIE_CERT_ENTRY(57), \
    FCOOKIE_CERT_ENTRY(58), \
    FCOOKIE_CERT_ENTRY(59), \
    FCOOKIE_CERT_ENTRY(60), \
    FCOOKIE_CERT_ENTRY(61), \
    FCOOKIE_CERT_ENTRY(62), \
    FCOOKIE_CERT_ENTRY(63), \
    FCOOKIE_CERT_ENTRY(64), \
    FCOOKIE_CERT_ENTRY(65), \
    FCOOKIE_CERT_ENTRY(66), \
    FCOOKIE_CERT_ENTRY(67), \
    FCOOKIE_CERT_ENTRY(68), \
    FCOOKIE_CERT_ENTRY(69), \
    FCOOKIE_CERT_ENTRY(70), \
    FCOOKIE_CERT_ENTRY(71), \
    FCOOKIE_CERT_ENTRY(72), \
    FCOOKIE_CERT_ENTRY(73), \
    FCOOKIE_CERT_ENTRY(74), \
    FCOOKIE_CERT_ENTRY(75), \
    FCOOKIE_CERT_ENTRY(76), \
    FCOOKIE_CERT_ENTRY(77), \
    FCOOKIE_CERT_ENTRY(78), \
    FCOOKIE_CERT_ENTRY(79), \
    FCOOKIE_CERT_ENTRY(80), \
    FCOOKIE_CERT_ENTRY(81), \
    FCOOKIE_CERT_ENTRY(82), \
    FCOOKIE_CERT_ENTRY(83), \
    FCOOKIE_CERT_ENTRY(84), \
    FCOOKIE_CERT_ENTRY(85), \
    FCOOKIE_CERT_ENTRY(86), \
    FCOOKIE_CERT_ENTRY(87), \
    FCOOKIE_CERT_ENTRY(88), \
    FCOOKIE_CERT_ENTRY(89), \
    FCOOKIE_CERT_ENTRY(90), \
    FCOOKIE_CERT_ENTRY(91), \
    FCOOKIE_CERT_ENTRY(92), \
    FCOOKIE_CERT_ENTRY(93), \
    FCOOKIE_CERT_ENTRY(94), \
    FCOOKIE_CERT_ENTRY(95), \
    FCOOKIE_CERT_ENTRY(96), \
    FCOOKIE_CERT_ENTRY(97), \
    FCOOKIE_CERT_ENTRY(98), \
    FCOOKIE_CERT_ENTRY(99), \
}   /* end */

#define FCOOKIE_FILE_PREFIX     "/tmp/.cookie"

enum {
    FCOOKIE_FILE_LEN = sizeof(FCOOKIE_FILE_PREFIX ".XXXXXXXXXXXXXXXX"),
};

static inline bool
__fcookie_file_exist(char *file)
{
    int fd = open(file, O_RDONLY, S_IRUSR | S_IRGRP);

    return fd<0?false:(close(fd), true);
}

#ifdef IN_CURL
static inline char *
__fcookie_file_create(fcookie_file_t *cert, char *tmp_file)
{
    char *file = NULL;
    char *decode = NULL;
    int len = 0;

    int err = Curl_base64_decode(cert->info, &decode, &len);
    if (err<0) {
        return NULL;
    }

    STREAM f = fopen(tmp_file, "r+");
    if (NULL==f) {
        goto error;
    }

    fwrite(decode, len, 1, f);
    fclose(f);
    
error:
    if (decode) {
        free(decode);
    }
    
    return file;
}
#else
static inline char *
__fcookie_file_create(fcookie_file_t *cert, char *tmp_file)
{
    char *cmd = (char *)calloc(256, 1024); /* 256K */
    if (NULL==cmd) {
        return NULL;
    }
    
    sprintf(cmd, "echo %s | base64 -d > %s", cert->info, tmp_file);
    system(cmd);
    
    if (cert->mode) {
        chmod(tmp_file, cert->mode);
    }

    free(cmd);

    return tmp_file;
}
#endif

static inline char *
__fcookie_file(int id, char *tmp_file)
{
    /*
    * mktemp reutrn it, must static
    */
    int err;
    fcookie_file_t cookie[FCOOKIE_COOKIE_FILE_END] = FCOOKIE_FILES;

    if (false==is_good_fcookie_file_id(id)) {
        return NULL;
    }

    fcookie_file_t *cert = &cookie[id];
    if (NULL==cert) {
        return NULL;
    }
        
    do{
        sprintf(tmp_file, FCOOKIE_FILE_PREFIX ".%x%x", getpid(), rand());
    } while(__fcookie_file_exist(tmp_file));
    
    return __fcookie_file_create(cert, tmp_file);
}

static inline char *
fcookie_file(int id)
{
    static char tmp_file[1+FCOOKIE_FILE_LEN];

    return __fcookie_file(id, tmp_file);
}
/******************************************************************************/

typedef struct {
    bool https;
    char *domain;
    char *path[0];
    int count;
} fcookie_domain_t;

static inline char *
__fcookie_domain_get(char *domain)
{
    static char *domains[] = {
        "http://update1.9797168.com:821/wifibox",
        
        "https://atbus.superwalle.com:8443/LMS/lte/lte",
        ".xxx.com",
        ".pepfi.com",
        "ftp://lms2.superwalle.com",
        
    };

    return NULL;
}

/******************************************************************************/
#endif /* __FCOOKIE_H_39d80845ac764a84bfdfa3166c751c51__ */
