#ifndef __UM_H_cdb651156406414c97daca3b0f7527a6__
#define __UM_H_cdb651156406414c97daca3b0f7527a6__
/******************************************************************************/
#include "utils.h"
/******************************************************************************/
#ifndef UM_USE_MONITOR
#define UM_USE_MONITOR          0
#endif

#ifndef UM_USE_GC
#define UM_USE_GC               1
#endif

#ifndef ENV_UMD_UNIX
#define ENV_UMD_UNIX            "__UMD_UNIX__"
#endif

#ifndef ENV_UMD_GC
#define ENV_UMD_GC              "__UMD_GC__" /* ms */
#endif

#ifndef ENV_UMC_UNIX
#define ENV_UMC_UNIX            "__UMC_UNIX__"
#endif

#ifndef ENV_UM_TIMEOUT
#define ENV_UM_TIMEOUT          "__UM_TIMEOUT__" /* ms */
#endif

#ifndef UMD_UNIX
#define UMD_UNIX                "/tmp/.umd.unix"
#endif

#ifndef UMC_UNIX
#define UMC_UNIX                "/tmp/.umc.%d.unix"
#endif

#ifndef UM_TIMEOUT
#define UM_TIMEOUT              3000    /* ms */
#endif

#ifndef UMD_GC
#ifdef __PC__
#   define UMD_GC               60  /* second */
#else
#   define UMD_GC               600 /* second */
#endif
#endif

static inline int
get_umd_path_env(sockaddr_un_t *addr) 
{
    return __env_copy(ENV_UMD_UNIX, UMD_UNIX, 
                get_abstract_path(addr),
                abstract_path_size);
}

static inline int
get_umc_path_env(sockaddr_un_t *addr) 
{
    char path[1+OS_LINE_LEN] = {0};

    os_saprintf(path, UMC_UNIX, getpid());
    
    return __env_copy(ENV_UMC_UNIX, path, 
                get_abstract_path(addr),
                abstract_path_size);
}

static inline int
get_um_timeout_env(void) 
{
    return env_geti(ENV_UM_TIMEOUT, UM_TIMEOUT);
}

static inline int
get_umd_gc_env(void) 
{
    return env_geti(ENV_UMD_GC, UMD_GC);
}
/******************************************************************************/
#endif /* __UM_H_cdb651156406414c97daca3b0f7527a6__ */
