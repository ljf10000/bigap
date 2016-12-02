/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifdef IN_CURL
char *
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
char *
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

#include "oem/cert/weos/fcookie_cert_initer.c"

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
    [FCOOKIE_LSS_CERT]      = FCOOKIE_FILE_ENTRY(oem_lss_cert),  \
    [FCOOKIE_LSS_KEY]       = FCOOKIE_FILE_ENTRY(oem_lss_key),   \
    FCOOKIE_CERT_INITERS    \
}   /* end */

char *
__fcookie_file(int id, char *tmp_file)
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
fcookie_file(int id)
{
    static char tmp_file[1+FCOOKIE_FILE_LEN];

    return __fcookie_file(id, tmp_file);
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
