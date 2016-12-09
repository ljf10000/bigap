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
#define OS_BENV_SHM_ID          (OS_IPC_ID + 2)
#define OS_BENV_SEM_ID          (OS_IPC_ID + 3)
#define OS_HAENV_SEM_ID         (OS_IPC_ID + 4)

#ifdef __PC__
#define PC_VAL(_pc, _not_pc)    _pc
#else
#define PC_VAL(_pc, _not_pc)    _not_pc
#endif

#define PC_DIR(_dir)            PC_VAL(".", _dir)
#define PC_FILE(_dir, _file)    PC_DIR(_dir) "/" _file

#ifdef __DEAMON__
#define DEAMON_VAL(_deamom_val, _normal_val)    _deamom_val
#else
#define DEAMON_VAL(_deamom_val, _normal_val)    _normal_val
#endif

/******************************************************************************/
#ifndef USE_MOD_BLOB
#define USE_MOD_BLOB            PC_VAL(1, 0)
#endif

#ifndef USE_MOD_CHANNEL
#define USE_MOD_CHANNEL         PC_VAL(1, 0)
#endif

#ifndef USE_MOD_COROUTINE
#define USE_MOD_COROUTINE       PC_VAL(1, 0)
#endif

#ifndef USE_MOD_CQUEUE
#define USE_MOD_CQUEUE          PC_VAL(1, 0)
#endif

#ifndef USE_MOD_DB_H1
#define USE_MOD_DB_H1           PC_VAL(1, 1)
#endif

#ifndef USE_MOD_DB_H2
#define USE_MOD_DB_H2           PC_VAL(1, 1)
#endif

#ifndef USE_MOD_DB_H3
#define USE_MOD_DB_H3           PC_VAL(1, 0)
#endif

#ifndef USE_MOD_DB_H4
#define USE_MOD_DB_H4           PC_VAL(1, 0)
#endif

#ifndef USE_MOD_DB_H5
#define USE_MOD_DB_H5           PC_VAL(1, 0)
#endif

#ifndef USE_MOD_DB_H6
#define USE_MOD_DB_H6           PC_VAL(1, 0)
#endif

#ifndef USE_MOD_DB_H7
#define USE_MOD_DB_H7           PC_VAL(1, 0)
#endif

#ifndef USE_MOD_DB_H8
#define USE_MOD_DB_H8           PC_VAL(1, 0)
#endif

#ifndef USE_MOD_DB_H9
#define USE_MOD_DB_H9           PC_VAL(1, 0)
#endif

#ifndef USE_MOD_FCOOKIE
#define USE_MOD_FCOOKIE         PC_VAL(1, 1)
#endif

#ifndef USE_MOD_FD
#define USE_MOD_FD              PC_VAL(1, 0)
#endif

#ifndef USE_MOD_NSQ
#define USE_MOD_NSQ             PC_VAL(1, 1)
#endif

#ifndef USE_MOD_SLICE
#define USE_MOD_SLICE           PC_VAL(1, 0)
#endif
/******************************************************************************/
#ifndef USE_SOCK_SERVER_SELECT
#define USE_SOCK_SERVER_SELECT  PC_VAL(1, 0)
#endif

#ifndef USE_BLOB_COUNT
#define USE_BLOB_COUNT          PC_VAL(1, 1)
#endif

#ifndef USE_JSON_RULE
#define USE_JSON_RULE           PC_VAL(1, 1)
#endif

#ifndef USE_STRING_BLOCK
#define USE_STRING_BLOCK        PC_VAL(0, 0)
#endif

#ifndef USE_UM_WIRELESS
#define USE_UM_WIRELESS         PC_VAL(0, 0)
#endif

#ifndef USE_THIS_ENV
#define USE_THIS_ENV            PC_VAL(0, 0)
#endif

#ifndef USE_JLOG_CONNECT
#define USE_JLOG_CONNECT        PC_VAL(1, 1)
#endif

#ifndef USE_JLOG_BIND
#define USE_JLOG_BIND           PC_VAL(0, 0)
#endif
/******************************************************************************/
#endif /* __CONFIG_H_2e9c3edb1c3440539c6a555bf729eaa9__ */
