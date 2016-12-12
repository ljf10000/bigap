#ifndef __FCOOKIE_H_39d80845ac764a84bfdfa3166c751c51__
#define __FCOOKIE_H_39d80845ac764a84bfdfa3166c751c51__
#include "oem/oem.h"
/******************************************************************************/
#ifndef USE_MOD_FCOOKIE
#define USE_MOD_FCOOKIE         PC_VAL(1, 1)
#endif

#ifdef __APP__
/*
* NOT depend actions
*/

enum {
    FCOOKIE_BEGIN   = 0,
    
    FCOOKIE_FILE    = FCOOKIE_BEGIN,
    FCOOKIE_DOMAIN,

    FCOOKIE_END
};

static inline bool
is_good_fcookie_id(int id)
{
    return IS_GOOD_VALUE(id, FCOOKIE_BEGIN, FCOOKIE_END);
}

enum {
    FCOOKIE_INVALID         = 0,
    FCOOKIE_FILE_BEGIN      = 1,
    
    FCOOKIE_RSYNC_PWDFILE   = FCOOKIE_FILE_BEGIN,
    FCOOKIE_LSS_CERT        = 2,
    FCOOKIE_LSS_KEY         = 3,

    FCOOKIE_CERT_BEGIN      = 4,
    FCOOKIE_CERT_END        = FCOOKIE_CERT_BEGIN + OS_CERT_COUNT,   // 4+32=36
    
    FCOOKIE_KEY_BEGIN       = FCOOKIE_CERT_END,                     // 36
    FCOOKIE_KEY_END         = FCOOKIE_KEY_BEGIN + OS_CERT_COUNT,    // 36+32=68
    
    FCOOKIE_FILE_END        = FCOOKIE_KEY_END,                      // 68
};

static inline bool
is_good_fcookie_file_id(int id)
{
    return IS_GOOD_VALUE(id, FCOOKIE_FILE_BEGIN, FCOOKIE_FILE_END);
}

typedef struct {
    char *info;
    int mode;
} fcookie_file_t;

#define FCOOKIE_FILE_PREFIX     "/tmp/.cookie"

enum {
    FCOOKIE_FILE_LEN = sizeof(FCOOKIE_FILE_PREFIX ".XXXXXXXXXXXXXXXX"),
};

extern char *
__fcookie_file_create(fcookie_file_t *cert, char *tmp_file);

extern char *
__fcookie_file(int id, char *tmp_file);

/*
* unsafe
*/
extern char *
fcookie_file(int id);
/******************************************************************************/

typedef struct {
    bool https;
    char *domain;
    char *path[0];
    int count;
} fcookie_domain_t;

extern char *
__fcookie_domain_get(char *domain);

#endif
/******************************************************************************/
#endif /* __FCOOKIE_H_39d80845ac764a84bfdfa3166c751c51__ */
