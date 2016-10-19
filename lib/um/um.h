#ifndef __UM_H_cdb651156406414c97daca3b0f7527a6__
#define __UM_H_cdb651156406414c97daca3b0f7527a6__
/******************************************************************************/
#include "utils.h"
/******************************************************************************/
#ifndef UM_USE_MONITOR
#define UM_USE_MONITOR          0
#endif

#ifndef UM_USE_SYNC
#define UM_USE_SYNC             0
#endif

#ifndef UM_TIMEOUT
#define UM_TIMEOUT              3000    /* ms */
#endif

#ifndef UMD_UNIX
#define UMD_UNIX                "/tmp/.umd.unix"
#endif

#ifndef UMC_UNIX
#define UMC_UNIX                "/tmp/.umc.%d.unix"
#endif

static inline int
get_umd_path_env(sockaddr_un_t *addr) 
{
    return __env_copy(OS_ENV(UMD_UNIX), UMD_UNIX, 
                get_abstract_path(addr),
                abstract_path_size);
}

static inline int
get_umc_path_env(sockaddr_un_t *addr) 
{
    char path[1+OS_LINE_LEN] = {0};

    os_saprintf(path, UMC_UNIX, getpid());
    
    return __env_copy(OS_ENV(UMC_UNIX), path, 
                get_abstract_path(addr),
                abstract_path_size);
}

static inline int
get_um_timeout_env(void) 
{
    return env_geti(OS_ENV(TIMEOUT), UM_TIMEOUT);
}
/******************************************************************************/
#endif /* __UM_H_cdb651156406414c97daca3b0f7527a6__ */
