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

#define SMC_USAGE \
    "smc usage:"                                            __crlf \
    __tab "smc insert deamon {name} {pidfile} {command}"    __crlf \
    __tab "smc insert normal {name} {command}"              __crlf \
    __tab "smc remove {name}"                               __crlf \
    __tab "smc show [name]"                                 __crlf \
    /* end */
/******************************************************************************/
#endif /* __SM_H_47451518b91b472588606e24d093dabb__ */
