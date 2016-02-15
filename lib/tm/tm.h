#ifndef __TM_H_d485ac3ed1c445f396938162007ea7c3__
#define __TM_H_d485ac3ed1c445f396938162007ea7c3__
/******************************************************************************/
#include "utils.h"
/******************************************************************************/
#ifndef ENV_TMD_UNIX
#define ENV_TMD_UNIX                "__TMD_UNIX__"
#endif

#ifndef ENV_TMC_UNIX
#define ENV_TMC_UNIX                "__TMC_UNIX__"
#endif

#ifndef ENV_TM_TIMEOUT
#define ENV_TM_TIMEOUT              "__TM_TIMEOUT__"
#endif

#ifndef TMD_UNIX
#define TMD_UNIX                    "/tmp/.tmd.unix"
#endif

#ifndef TMC_UNIX
#define TMC_UNIX                    "/tmp/.tmc.%d.unix"
#endif

#ifndef TM_TICKS
#define TM_TICKS                    1000 /* ms */
#endif

#ifndef TM_TIMEOUT
#define TM_TIMEOUT                  3000 /* ms */
#endif

#ifndef TM_NAMESIZE
#define TM_NAMESIZE                 31
#endif

#ifndef TM_CMDSIZE
#define TM_CMDSIZE                  127
#endif

static bool
is_good_tm_args(int delay, int interval, int limit)
{
    if (delay<0 || interval<0 || limit<0) {
        return false;
    } else {
        return interval || (delay && 1==limit);
    }
}

static inline int
get_tm_timeout_env(void) 
{
    return env_geti(ENV_TM_TIMEOUT, TM_TIMEOUT);
}

static inline int
get_tmc_path_env(sockaddr_un_t *addr) 
{
    char path[1+OS_LINE_LEN] = {0};

    os_saprintf(path, TMC_UNIX, getpid());
    
    return __env_copy(ENV_TMC_UNIX, path, 
                get_abstract_path(addr),
                abstract_path_size);
}

static inline int
get_tmd_path_env(sockaddr_un_t *addr) 
{
    return __env_copy(ENV_TMD_UNIX, TMD_UNIX, 
                get_abstract_path(addr),
                abstract_path_size);
}
/******************************************************************************/
#endif /* __TM_H_d485ac3ed1c445f396938162007ea7c3__ */
