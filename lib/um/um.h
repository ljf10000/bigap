#ifndef __UM_H_cdb651156406414c97daca3b0f7527a6__
#define __UM_H_cdb651156406414c97daca3b0f7527a6__
/******************************************************************************/
#include "utils.h"

//guoshuai-hostapd-data
#ifdef __OPENWRT__
#ifdef __BUSYBOX__
#include <sys/types.h>
#include <iwinfo/api/wext.h>
#include <asm/byteorder.h>
#include <ieee80211_external.h>
#include <stdint.h>
#endif
#endif
//end

/******************************************************************************/
#ifndef UM_SSID_MAX
#define UM_SSID_MAX             128
#endif

#ifndef USE_UMD_WIRELESS
#define USE_UMD_WIRELESS        PC_VAL(0, 0)
#endif

#ifndef USE_UMD_CONN_PROTOCOL
#define USE_UMD_CONN_PROTOCOL   PC_VAL(1, 0)
#endif
/******************************************************************************/
#endif /* __UM_H_cdb651156406414c97daca3b0f7527a6__ */
