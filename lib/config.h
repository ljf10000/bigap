#ifndef __CONFIG_H_2e9c3edb1c3440539c6a555bf729eaa9__
#define __CONFIG_H_2e9c3edb1c3440539c6a555bf729eaa9__
/******************************************************************************/
#if defined(LP64) || defined(ILP64) || defined(LLP64)
#   define OS64                 1
#else
#   define OS64                 0
#endif

#ifndef OS_IFNAME_LEN
#define OS_IFNAME_LEN           (16 - 1)
#endif

#ifndef OS_LINE_SHORT
#define OS_LINE_SHORT           (128 - 1)
#endif

#ifndef OS_LINE_LEN
#define OS_LINE_LEN             (1*1024 - 1)
#endif

#ifndef OS_PAGE_LEN
#define OS_PAGE_LEN             (4*1024 - 1)
#endif

#ifndef OS_BLOCK_LEN
#define OS_BLOCK_LEN            (16*1024 - 1)
#endif

#ifndef OS_FILE_LEN
#define OS_FILE_LEN             (64*1024 - 1)
#endif

#ifndef OS_BIG_LEN
#define OS_BIG_LEN              (256*1024 - 1)
#endif

#ifndef OS_HUGE_LEN
#define OS_HUGE_LEN             (1024*1024 - 1)
#endif

#ifndef OS_FILENAME_LEN
#define OS_FILENAME_LEN         OS_LINE_LEN
#endif

#ifndef OS_PROTECTED
#define OS_PROTECTED            0x2012dead
#endif

#ifndef OS_USE_UTC_TIME
#define OS_USE_UTC_TIME         0
#endif

#ifndef OS_IPC_ID
#define OS_IPC_ID               538050839
#endif

#define OS_AK_SHM_ID            (OS_IPC_ID + 1)
#define OS_HAENV_SEM_ID         (OS_IPC_ID + 2)

#define OS_INLINE               __attribute__((always_inline))
/******************************************************************************/
#endif /* __CONFIG_H_2e9c3edb1c3440539c6a555bf729eaa9__ */
