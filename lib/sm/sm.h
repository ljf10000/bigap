#ifndef __SM_H_47451518b91b472588606e24d093dabb__
#define __SM_H_47451518b91b472588606e24d093dabb__
/******************************************************************************/
#include "utils.h"
/******************************************************************************/
#ifndef ENV_SMD_UNIX
#define ENV_SMD_UNIX                "__SMD_UNIX__"
#endif

#ifndef ENV_SMC_UNIX
#define ENV_SMC_UNIX                "__SMC_UNIX__"
#endif

#ifndef ENV_SM_TIMEOUT
#define ENV_SM_TIMEOUT              "__SM_TIMEOUT__"
#endif

#ifndef SMD_UNIX
#define SMD_UNIX                    "/tmp/.smd.unix"
#endif

#ifndef SMC_UNIX
#define SMC_UNIX                    "/tmp/.smc.%d.unix"
#endif

#ifndef SM_TIMEOUT
#define SM_TIMEOUT                  3000 /* ms */
#endif

#ifndef SM_NAMESIZE
#define SM_NAMESIZE                 31
#endif

#ifndef SM_CMDSIZE
#define SM_CMDSIZE                  127
#endif

#ifndef SM_PIDFILE
#define SM_PIDFILE                  127
#endif

#ifndef SM_TIMER
#define SM_TIMER                    1 /* second */
#endif

#ifndef SCRIPT_SM_GETPID
#ifdef __PC__
#   define SCRIPT_SM_GETPID         "./.sm.getpid"
#else
#   define SCRIPT_SM_GETPID         "/usr/sbin/.sm.getpid"
#endif
#endif

#ifndef SCRIPT_SM_INIT
#ifdef __PC__
#   define SCRIPT_SM_INIT           "./.sm.init"
#else
#   define SCRIPT_SM_INIT           "/tmp/.sm.init"
#endif
#endif

#define __SM_LIST(_) \
    _(SM_STATE_INIT,  0, "init"), \
    _(SM_STATE_FORK,  1, "fork"), \
    _(SM_STATE_RUN,   2, "run"),  \
    _(SM_STATE_DIE,   3, "die"),  \
    /* end */

static inline bool is_good_sm_state(int id);
static inline char *sm_state_string(int id);
static inline int sm_state_idx(char *name);
DECLARE_ENUM(sm_state, __SM_LIST, SM_STATE_END);

static inline int
get_smd_path_env(sockaddr_un_t *addr) 
{
    return __env_copy(ENV_SMD_UNIX, SMD_UNIX, 
                get_abstract_path(addr),
                abstract_path_size);
}

static inline int
get_smc_path_env(sockaddr_un_t *addr) 
{
    char path[1+OS_LINE_LEN] = {0};

    os_saprintf(path, SMC_UNIX, getpid());
    
    return __env_copy(ENV_SMC_UNIX, path, 
                get_abstract_path(addr),
                abstract_path_size);
}

static inline int
get_sm_timeout_env(void) 
{
    return env_geti(ENV_SM_TIMEOUT, SM_TIMEOUT);
}
/******************************************************************************/
#endif /* __SM_H_47451518b91b472588606e24d093dabb__ */
