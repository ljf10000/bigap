#ifndef __SM_H_47451518b91b472588606e24d093dabb__
#define __SM_H_47451518b91b472588606e24d093dabb__
/******************************************************************************/
#include "utils.h"
/******************************************************************************/
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

#ifndef SCRIPT_SMD_INIT
#define SCRIPT_SMD_INIT             PC_VAL("./smd.init", SCRIPT_FILE("smd.init"))
#endif

#ifndef SCRIPT_SMD_INIT_DELAY
#define SCRIPT_SMD_INIT_DELAY       3 /* second */
#endif

#define SCRIPT_SMD_INIT_RUN         \
"(sleep " __SYMBOL_TO_STRING(SCRIPT_SMD_INIT_DELAY) ";" SCRIPT_SMD_INIT ";) &"

#if 1
#define SM_STATE_ENUM_MAPPER(_)     \
    _(SM_STATE_INIT,  0, "init"),   \
    _(SM_STATE_FORK,  1, "fork"),   \
    _(SM_STATE_RUN,   2, "run"),    \
    _(SM_STATE_DIE,   3, "die"),    \
    /* end */
DECLARE_ENUM(sm_state, SM_STATE_ENUM_MAPPER, SM_STATE_END);

static inline enum_ops_t *sm_state_ops(void);

#define SM_STATE_INIT   SM_STATE_INIT
#define SM_STATE_FORK   SM_STATE_FORK
#define SM_STATE_RUN    SM_STATE_RUN
#define SM_STATE_DIE    SM_STATE_DIE
#define SM_STATE_END    SM_STATE_END
#endif
/******************************************************************************/
#endif /* __SM_H_47451518b91b472588606e24d093dabb__ */
