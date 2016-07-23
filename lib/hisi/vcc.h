#ifndef __VCC_H_2e9fca53231e4e68a112c3bc5fe18832__
#define __VCC_H_2e9fca53231e4e68a112c3bc5fe18832__
/******************************************************************************/
#ifdef __APP__
/******************************************************************************/
#ifndef VCC_INTERVAL
#define VCC_INTERVAL        100 // ms
#endif

#ifndef VCC_TIMEOUT
#define VCC_TIMEOUT         15 // second
#endif

#ifndef VCC_PATH
#define VCC_PATH            "/tmp/proc/vcc"
#endif

#ifndef VCC_SCRIPT
#define VCC_SCRIPT          "/tmp/cb/vcc.cb"
#endif

#ifndef VCC_FILE_STATUS
#define VCC_FILE_STATUS     "status"
#endif

#ifndef VCC_FILE_DURATION
#define VCC_FILE_DURATION   "duration"
#endif

#define VCC_GPIO_COUNT      2
/******************************************************************************/
#endif
#endif /* __VCC_H_2e9fca53231e4e68a112c3bc5fe18832__ */
