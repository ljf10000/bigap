/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#if USE_MOD_CERT
#include "oem/cert/weos/fcookie_cert_initer.c"
#else
#define FCOOKIE_CERT_INITERS
#endif

#define __FCOOKIE_FILE_ENTRY(_info, _mode) {  \
    .info = _info,  \
    .mode = _mode,  \
}   /* end */

#define FCOOKIE_FILE_ENTRY(_info)   __FCOOKIE_FILE_ENTRY(_info, 0)
#define FCOOKIE_CERT_ENTRY(_idx)    [FCOOKIE_CERT_BEGIN+_idx] = FCOOKIE_FILE_ENTRY(__this_cert(_idx)->cert)
#define FCOOKIE_KEY_ENTRY(_idx)     [FCOOKIE_KEY_BEGIN+_idx] = FCOOKIE_FILE_ENTRY(__this_cert(_idx)->key)
#define FCOOKIE_CERT_INITER(_idx)   FCOOKIE_CERT_ENTRY(_idx), FCOOKIE_KEY_ENTRY(_idx)

#define FCOOKIE_INITER    { \
    [FCOOKIE_RSYNC_PWDFILE] = __FCOOKIE_FILE_ENTRY(oem_rsync_pwdfile, 0x600), \
    [FCOOKIE_LSS_CERT]      = FCOOKIE_FILE_ENTRY(oem_lss_cert),     \
    [FCOOKIE_LSS_KEY]       = FCOOKIE_FILE_ENTRY(oem_lss_key),      \
    [FCOOKIE_LSS_CACERT]    = FCOOKIE_FILE_ENTRY(oem_lss_cacert),   \
    FCOOKIE_CERT_INITERS    \
}   /* end */

char *
__fcookie_file_create(fcookie_file_t *cert, char *tmp_file)
{
    os_system("echo %s | base64 -d > %s", cert->info, tmp_file);
    
    if (cert->mode) {
        chmod(tmp_file, cert->mode);
    }

    return tmp_file;
}

char *
fcookie_file(int id, char *tmp_file)
{
    /*
    * mktemp reutrn it, must static
    */
    int err;
    fcookie_file_t cookie[FCOOKIE_FILE_END] = FCOOKIE_INITER;

    if (false==is_good_fcookie_file_id(id)) {
        return NULL;
    }

    fcookie_file_t *cert = &cookie[id];
    if (NULL==cert) {
        return NULL;
    }
        
    do{
        sprintf(tmp_file, FCOOKIE_FILE_PREFIX ".%x%x", getpid(), rand());
    } while(os_file_exist(tmp_file));
    
    return __fcookie_file_create(cert, tmp_file);
}

char *
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
