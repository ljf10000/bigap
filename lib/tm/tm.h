#ifndef __TM_H_d485ac3ed1c445f396938162007ea7c3__
#define __TM_H_d485ac3ed1c445f396938162007ea7c3__
/******************************************************************************/
#include "utils.h"
/******************************************************************************/
#ifndef TM_TICKS
#define TM_TICKS                    1000 /* ms */
#endif

#ifndef TM_NAMESIZE
#define TM_NAMESIZE                 31
#endif

#ifndef TM_CMDSIZE
#define TM_CMDSIZE                  127
#endif

#ifndef SCRIPT_TMD_INIT
#define SCRIPT_TMD_INIT             PC_VAL("./tmd.init", SCRIPT_FILE("tmd.init"))
#endif

#ifndef SCRIPT_TMD_INIT_DELAY
#define SCRIPT_TMD_INIT_DELAY       3 /* second */
#endif

#define SCRIPT_TMD_INIT_RUN         \
"(sleep " __SYMBOL_TO_STRING(SCRIPT_TMD_INIT_DELAY) ";" SCRIPT_TMD_INIT ";) &"

static inline bool
is_good_tm_args(int delay, int interval, int limit)
{
    if (delay<0 || interval<0 || limit<0) {
        return false;
    } else {
        return interval || (delay && 1==limit);
    }
}

/******************************************************************************/
#endif /* __TM_H_d485ac3ed1c445f396938162007ea7c3__ */
