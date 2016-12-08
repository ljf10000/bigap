#ifndef __UM_H_cdb651156406414c97daca3b0f7527a6__
#define __UM_H_cdb651156406414c97daca3b0f7527a6__
/******************************************************************************/
#include "utils.h"
/******************************************************************************/
#ifndef UM_USE_WIRELESS
#define UM_USE_WIRELESS         0
#endif

#ifndef UM_TIMEOUT
#define UM_TIMEOUT              3000    /* ms */
#endif

#ifndef UM_SSID_MAX
#define UM_SSID_MAX             128
#endif

#define UMC_USAGE \
    "tmc usage:"                            __crlf \
    __tab " bind   {mac} {ip}"              __crlf \
    __tab " unbind {mac}"                   __crlf \
    __tab " fake   {mac} {ip}"              __crlf \
    __tab " unfake {mac}"                   __crlf \
    __tab " auth   {mac} {group} {json}"    __crlf \
    __tab " deauth {mac}"                   __crlf \
    __tab " reauth {mac}"                   __crlf \
    __tab " tag    {mac} {key} [value]"     __crlf \
    __tab " gc"                             __crlf \
    __tab " show [stat | json]"             __crlf \
    /* end */

/******************************************************************************/
#endif /* __UM_H_cdb651156406414c97daca3b0f7527a6__ */
