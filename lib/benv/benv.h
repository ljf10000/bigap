#ifndef __BENV_H_6a2223c1e9a84ae38f5da34125b3974b__
#define __BENV_H_6a2223c1e9a84ae38f5da34125b3974b__
#include "utils.h"
/******************************************************************************/
#define BENV_DEBUG_SORT             0x01
#define BENV_DEBUG_CMP              0x02
#define BENV_DEBUG_TRACE            0x04

#ifndef BENV_DEBUG
#define BENV_DEBUG                  0 //(BENV_DEBUG_TRACE|BENV_DEBUG_MMC|BENV_DEBUG_SORT|BENV_DEBUG_CMP)
#endif

#ifndef BENV_SBBUG
#define BENV_SBBUG                  0
#endif

#ifndef BENV_LINK_ADDRESS
#define BENV_LINK_ADDRESS           0x9f000000
#endif

#if BENV_DEBUG
#define benv_debug(_debug, _fmt, _args...)  printf(_fmt "\n", ##_args)
#else
#define benv_debug(_debug, _fmt, _args...)  os_do_nothing()
#endif

#define benv_debug_sort(_fmt, _args...)     benv_debug(BENV_DEBUG_SORT, _fmt, ##_args)
#define benv_debug_cmp(_fmt, _args...)      benv_debug(BENV_DEBUG_CMP, _fmt, ##_args)
#define benv_debug_trace(_fmt, _args...)    benv_debug(BENV_DEBUG_TRACE, _fmt, ##_args)

#ifndef BENV_SIZE
#define BENV_SIZE                   4096
#endif

#ifndef BENV_COUNT
#define BENV_COUNT                  4
#endif

#define BENV_FLASH_EMMC             0
#define BENV_FLASH_NOR              1

#ifndef BENV_FLASH
#define BENV_FLASH                  BENV_FLASH_EMMC
#endif

#if BENV_FLASH!=BENV_FLASH_EMMC && \
    BENV_FLASH!=BENV_FLASH_NOR
#   error "invalid BENV_FLASH"
#endif

#if (defined(__BOOT__) || defined(__PC__)) && BENV_COUNT > 1
#define BENV_REPAIR                 1
#else
#define BENV_REPAIR                 0
#endif

#define BENV_START                  PRODUCT_BOOT_SIZE
#define BENV_BLOCK_SIZE             PRODUCT_BLOCK_SIZE
#define BENV_BLOCK_COUNT            (BENV_SIZE/BENV_BLOCK_SIZE) /* 8 */

#ifndef BENV_TRYS
#define BENV_TRYS                   3
#endif

#ifndef BENV_INFO_SIZE
#define BENV_INFO_SIZE              64
#endif

#ifndef BENV_USE_SEM
#define BENV_USE_SEM                0
#endif

#define is_good_benv_idx(_idx)      is_good_enum(_idx, PRODUCT_FIRMWARE_COUNT)
#define is_normal_benv_idx(_idx)    is_good_value(_idx, 1, PRODUCT_FIRMWARE_COUNT)

#define os_firmware_foreach_all(_i) os_foreach(_i, PRODUCT_FIRMWARE_COUNT)
#define os_firmware_foreach(_i)     __os_foreach(_i, 1, PRODUCT_FIRMWARE_COUNT) /* skip 0 */

#if 1
/*
* ok > unknow > fail
*/
#define BENV_FSM_ENUM_MAPPER(_)         \
    _(BENV_FSM_FAIL,    0, "fail"),     \
    _(BENV_FSM_UNKNOW,  1, "unknow"),   \
    _(BENV_FSM_OK,      2, "ok"),       \
    /* end */
DECLARE_ENUM(benv_fsm, BENV_FSM_ENUM_MAPPER, BENV_FSM_END);

static inline enum_ops_t *benv_fsm_ops(void);
static inline bool is_good_benv_fsm(int id);
static inline char *benv_fsm_getnamebyid(int id);
static inline int benv_fsm_getidbyname(const char *name);

#define BENV_FSM_FAIL   BENV_FSM_FAIL
#define BENV_FSM_UNKNOW BENV_FSM_UNKNOW
#define BENV_FSM_OK     BENV_FSM_OK
#define BENV_FSM_END    BENV_FSM_END
#endif

enum {BENV_FSM_INVALID = BENV_FSM_END};

/*
* ok or unknow
*/
static inline bool
is_canused_benv_fsm(int fsm)
{
    return IS_GOOD_VALUE(fsm, BENV_FSM_UNKNOW, BENV_FSM_END);
}

static inline int
benv_fsm_cmp(int a, int b)
{
    return a - b;
}

#define BENV_COOKIE_FIXED       (0  \
    + sizeof(PRODUCT_Vendor)        \
    + sizeof(PRODUCT_COMPANY)       \
    + sizeof(PRODUCT_PCBA_MODEL)    \
)   /* end */

#define BENV_COOKIE_SIZE        (0  \
    + BENV_COOKIE_FIXED             \
    + 1+PRODUCT_VERSION_STRING_LEN  \
    + 1+FULLTIME_STRING_LEN         \
)   /* end */

typedef struct {
    char vendor[sizeof(PRODUCT_Vendor)];
    char company[sizeof(PRODUCT_COMPANY)];
    char model[sizeof(PRODUCT_PCBA_MODEL)];
    char version[1+PRODUCT_VERSION_STRING_LEN];
    char compile[1+FULLTIME_STRING_LEN];
    char pad[BENV_BLOCK_SIZE - BENV_COOKIE_SIZE];
} benv_cookie_t; /* 512 */

extern bool
is_good_benv_cookie(benv_cookie_t *cookie);

extern void
__benv_cookie_show(benv_cookie_t *cookie);

typedef product_version_t benv_version_t;

#define BENV_MIN_VERSION_STRING     PRODUCT_MIN_VERSION_STRING
#define BENV_MAX_VERSION_STRING     PRODUCT_MAX_VERSION_STRING
#define BENV_DEFT_VERSION_STRING    PRODUCT_DEFT_VERSION_STRING
#define BENV_INVALID_VERSION_STRING PRODUCT_INVALID_VERSION_STRING
#define BENV_VERSION_STRING_LEN     PRODUCT_VERSION_STRING_LEN

#define BENV_MIN_VERSION            PRODUCT_MIN_VERSION
#define BENV_MAX_VERSION            PRODUCT_MAX_VERSION
#define BENV_INVALID_VERSION        PRODUCT_INVALID_VERSION
#define BENV_DEFT_VERSION           PRODUCT_DEFT_VERSION

extern char *
benv_version_itoa(benv_version_t *version, char string[]);

extern benv_version_t *
benv_version_atoi(benv_version_t *version, char *string);

static inline int
benv_version_cmp(benv_version_t *a, benv_version_t *b)
{
    return os_objcmp(a, b);
}

static inline bool
benv_version_eq(benv_version_t *a, benv_version_t *b)
{
    return os_objeq(a, b);
}

enum {
    BENV_VCS_COOKIE_SIZE = 15,
};

typedef struct {
    uint32 self;
    uint32 other;
    uint32 upgrade;
    uint32 error;
    
    benv_version_t version;

    char cookie[1+BENV_VCS_COOKIE_SIZE];
} benv_vcs_t; /* 36 */

#define __BENV_VCS(_self, _other, _upgrade, _error, _version)   { \
    .self       = _self,        \
    .other      = _other,       \
    .upgrade    = _upgrade,     \
    .error      = _error,       \
    .version    = _version,     \
    .cookie   = {0},            \
}   /* end */

extern void
__benv_vcs_deft(benv_vcs_t *vcs);

#define BENV_DEFT_VCS   \
    __BENV_VCS(BENV_FSM_UNKNOW, BENV_FSM_UNKNOW, BENV_FSM_OK, 0, BENV_DEFT_VERSION)
#define BENV_MIN_VCS    \
    __BENV_VCS(BENV_FSM_FAIL, BENV_FSM_FAIL, BENV_FSM_FAIL, BENV_TRYS, BENV_MIN_VERSION)
#define BENV_MAX_VCS    \
    __BENV_VCS(BENV_FSM_OK, BENV_FSM_OK, BENV_FSM_OK, 0, BENV_MAX_VERSION)
#define BENV_INVALID_VCS \
    __BENV_VCS(BENV_FSM_INVALID, BENV_FSM_INVALID, BENV_FSM_INVALID, BENV_TRYS, BENV_INVALID_VERSION)

#define is_benv_good(_error)        is_good_enum(_error, BENV_TRYS)

static inline int
benv_error_cmp(uint32 a, unsigned b)
{
    return os_cmp(a, b, is_benv_good, os_cmp_always_eq);
}

extern bool
is_benv_good_vcs(benv_vcs_t *vcs);

typedef struct {
    benv_vcs_t kernel;
    benv_vcs_t rootfs;
} benv_firmware_t;        /* 2*36 = 72 */

static inline void
__benv_firmware_deft(benv_firmware_t *firmware)
{
    __benv_vcs_deft(&firmware->rootfs);
    __benv_vcs_deft(&firmware->kernel);
}

#define BENV_DEFT_FIRMWARE  {   \
    .kernel = BENV_DEFT_VCS,    \
    .rootfs = BENV_DEFT_VCS,    \
}   /* end */

#define BENV_INVALID_FIRMWARE { \
    .kernel = BENV_INVALID_VCS,   \
    .rootfs = BENV_INVALID_VCS,   \
}   /* end */

enum {
    BENV_OS_SIZE = (2*sizeof(uint32) + PRODUCT_FIRMWARE_COUNT*sizeof(benv_firmware_t)),
};

typedef struct {
    uint32 current;
    uint32 reserved;

    benv_firmware_t firmware[PRODUCT_FIRMWARE_COUNT];
    char pad[BENV_BLOCK_SIZE - BENV_OS_SIZE];
} benv_os_t; /* 512 */

extern void
__benv_os_show(benv_os_t *os);

enum {
    BENV_MARK_COUNT = (BENV_BLOCK_SIZE/sizeof(uint32)),   /* 128 */
};

typedef struct {
    uint32 var[BENV_MARK_COUNT];
} benv_mark_t; /* 512 */

enum {
    BENV_INFO_COUNT_PER_BLOCK   = (BENV_BLOCK_SIZE/BENV_INFO_SIZE),                 /* 512/64=8 */
    BENV_INFO_COUNT             = ((BENV_BLOCK_COUNT-3)*BENV_INFO_COUNT_PER_BLOCK), /* 5*8=40 */
};

typedef struct {
    char var[BENV_INFO_COUNT][BENV_INFO_SIZE];
} benv_info_t;

enum {
    BENV_ALL    = -1,
    
    BENV_COOKIE = 0,
    BENV_OS     = 1,
    BENV_MARK   = 2,
    BENV_INFO   = 3,

    BENV_END
};

typedef struct {
    benv_cookie_t   cookie;
    benv_os_t       os;
    benv_mark_t     mark;
    benv_info_t     info;
} benv_env_t;

extern int
__benv_ops_is(uint32 offset);

enum {
    BENV_OPS_NUMBER,
    BENV_OPS_STRING,
    BENV_OPS_VERSION,

    BENV_OPS_END
};

typedef struct benv_ops_s {
#if BENV_SBBUG
    uint32 __r0[2];
#endif
    char  *path;
    uint32 offset;
    uint32 type;

    int  (*check)(struct benv_ops_s* ops, char *value);
    void (*write)(struct benv_ops_s* ops, char *value);
    void (*show) (struct benv_ops_s* ops);
#if BENV_SBBUG
    uint32 __r1[2];
#endif
} benv_ops_t;

typedef struct {
    char *value;        /* for write */
    bool showit;        /* for show */
} benv_cache_t;

/* 
* path without last '*' 
*/
static inline bool
benv_ops_match_wildcard(benv_ops_t *ops, char *path, int len)
{
    return 0==len || 0==os_memcmp(path, ops->path, len);
}

/* 
* path maybe with last '*' 
*/
static inline bool
benv_ops_match(benv_ops_t *ops, char *path, int len, bool wildcard)
{
    if (wildcard) {
        return benv_ops_match_wildcard(ops, path, len - 1);
    } else {
        return len == os_strlen(ops->path)
            && 0==os_memcmp(ops->path, path, len);
    }
}

typedef struct {
    benv_env_t *mirror;
    benv_env_t *env;
    benv_ops_t *ops;
    benv_cache_t *cache;
    int ops_count;

    bool show_empty;
    int show_count;
    bool loaded;
    char *self;
    int fd;
    int error;

    os_shm_t shm;
#if BENV_USE_SEM
    os_sem_t sem;
#endif
} benv_control_t;

#if !defined(__ALLINONE__) && (IS_PRODUCT_PC || IS_PRODUCT_LTEFI_MD_PARTITION_B)
#   define BENV_INITER      benv_control_t *____benv_control = NULL
#else
#   define BENV_INITER      os_fake_declare
#endif
#define BENV_MAIN_INITER    benv_control_t *____benv_control = NULL

extern benv_control_t *____benv_control;

#if BENV_USE_SEM
#define __benv_sem              (&____benv_control->sem)
#endif
#define __benv_shm              (&____benv_control->shm)
#define __benv_shm_address      __benv_shm->address
#define __benv_shm_env          ((benv_env_t *)__benv_shm_address)
#define __benv_errno            ____benv_control->error
#define __benv_fd               ____benv_control->fd
#define __benv_mirror           ____benv_control->mirror
#define __benv_env(_env)        (&__benv_shm_env[_env])
#define __benv_ops              ____benv_control->ops
#define __benv_ops_count        ____benv_control->ops_count
#define __benv_show_count       ____benv_control->show_count
#define __benv_show_empty       ____benv_control->show_empty
#define __benv_self             ____benv_control->self
#define __benv_cache            ____benv_control->cache
#define __benv_cookie(_env)     (&__benv_env(_env)->cookie)
#define __benv_os(_env)         (&__benv_env(_env)->os)
#define __benv_current          __benv_os(0)->current
#define __benv_firmware         __benv_os(0)->firmware
#define __benv_mark(_env)       (&__benv_env(_env)->mark)
#define __benv_info(_env)       (&__benv_env(_env)->info)
#define __benv_block(_env, _idx)    ((char *)__benv_env(_env) + (_idx)*BENV_BLOCK_SIZE)
#define __benv_mirror_block(_idx)   ((char *)__benv_mirror + (_idx)*BENV_BLOCK_SIZE)

#define benv_mark(_idx)         __benv_mark(0)->var[_idx]
#define benv_info(_idx)         __benv_info(0)->var[_idx]

#define benv_offset(_env)       ((_env) * BENV_SIZE)

static inline benv_ops_t *
benv_ops(int idx)
{
    return &__benv_ops[idx];
}

static inline int
benv_ops_idx(benv_ops_t *ops)
{
    return (benv_ops_t *)ops - __benv_ops;
}

static inline void *
benv_ops_obj(benv_ops_t *ops)
{
    return (char *)__benv_env(0) + ops->offset;
}

static inline char *
benv_ops_string(benv_ops_t *ops)
{
    return (char *)benv_ops_obj(ops);
}

static inline uint32 *
benv_ops_number(benv_ops_t *ops)
{
    return (uint32 *)benv_ops_obj(ops);
}
#define benv_ops_fsm(_ops)          benv_ops_number(_ops)

static inline benv_version_t *
benv_ops_version(benv_ops_t *ops)
{
    return (benv_version_t *)benv_ops_obj(ops);
}

static inline int
benv_ops_check(benv_ops_t *ops, char *value)
{
    if (NULL==ops->write) {
        debug_error("no support write %s", ops->path);

        return -ENOSUPPORT;
    } else if (ops->check) {
        return (*ops->check)(ops, value);
    } else {
        return 0;
    }
}

static inline void
benv_ops_show(benv_ops_t *ops)
{
    if (ops->show) {
        (*ops->show)(ops);
    }
}

static inline void
benv_ops_write(benv_ops_t *ops, char *value)
{
    if (ops->write) {
        (*ops->write)(ops, value);
    }
}

#define __benv_ops_selfcheck(_idx, _obj) do{ \
    if ((uintptr)benv_ops(_idx)->_obj > (uintptr)BENV_LINK_ADDRESS) { \
        os_println("benv ops[%d:%s] " #_obj " is %p", \
            _idx,                       \
            benv_ops(_idx)->path,       \
            benv_ops(_idx)->_obj);      \
    }                                   \
}while(0)

static inline void
benv_ops_selfcheck(void)
{
#if BENV_SBBUG
    int i;
    
    for (i=0; i<__benv_ops_count; i++) {
        __benv_ops_selfcheck(i, check);
        __benv_ops_selfcheck(i, write);
        __benv_ops_selfcheck(i, show);
    }
#endif
}

static inline benv_cache_t *
benv_cache(benv_ops_t *ops)
{
    return &__benv_cache[benv_ops_idx(ops)];
}

#define benv_cache_value(_ops)      benv_cache(_ops)->value
#define benv_cache_showit(_ops)     benv_cache(_ops)->showit

static inline void
benv_cache_dump(void)
{
    int i;

    if (__is_ak_debug_trace) {
        os_println("ops count=%d", __benv_ops_count);
        
        for (i = 0; i < __benv_ops_count; i++) {
            benv_ops_t *ops = benv_ops(i);

            os_println("ops idx=%d, path=%s, value=%s, showit=%s",
                benv_ops_idx(ops),
                ops->path,
                benv_cache_value(ops)?benv_cache_value(ops):"nothing",
                benv_cache_showit(ops)?"true":"false");
        }
    }
}

static inline benv_firmware_t *
benv_firmware(int idx)
{
    return &__benv_firmware[idx];
}

static inline benv_vcs_t *
benv_kernel(int idx)
{
    return &benv_firmware(idx)->kernel;
}

static inline benv_vcs_t *
benv_rootfs(int idx)
{
    return &benv_firmware(idx)->rootfs;
}
#define benv_obj(_obj, _idx)  benv_##_obj(_idx)

static inline bool
is_benv_good_kernel(int idx)
{
    if (false==is_good_benv_idx(idx)) {
        debug_trace("bad kernel index:%d", idx);
        
        return false;
    }
    else if (false==is_benv_good_vcs(benv_kernel(idx))) {
        debug_trace("bad kernel%d's vcs");
        
        return false;
    }

    return true;
}

static inline bool
is_benv_good_rootfs(int idx)
{
    if (false==is_good_benv_idx(idx)) {
        debug_trace("bad rootfs index:%d", idx);
        
        return false;
    }
    else if (false==is_benv_good_vcs(benv_rootfs(idx))) {
        debug_trace("bad rootfs%d's vcs");
        
        return false;
    }

    return true;
}

static inline bool
is_benv_good_firmware(int idx)
{
    return is_benv_good_rootfs(idx)
        && is_benv_good_kernel(idx);
}
#define is_benv_good_obj(_obj, _idx)    is_benv_good_##_obj(_idx)

#define is_benv_bad_kernel(_idx)        (false==is_benv_good_kernel(_idx))
#define is_benv_bad_rootfs(_idx)        (false==is_benv_good_rootfs(_idx))
#define is_benv_bad_firmware(_idx)      (false==is_benv_good_firmware(_idx))
#define is_benv_bad_obj(_obj, _idx)     (false==is_benv_good_obj(_obj, _idx))

static inline benv_version_t *
benv_kernel_version(int idx)
{
    return &benv_kernel(idx)->version;
}

static inline benv_version_t *
benv_rootfs_version(int idx)
{
    return &benv_rootfs(idx)->version;
}
#define benv_firmware_version(_idx)     benv_rootfs_version(_idx)
#define benv_obj_version(_obj, _idx)    benv_##_obj##_version(_idx)

static inline int
benv_kernel_version_cmp(int a, int b)
{
    return benv_version_cmp(benv_kernel_version(a), benv_kernel_version(b));
}

static inline int
benv_rootfs_version_cmp(int a, int b)
{
    return benv_version_cmp(benv_rootfs_version(a), benv_rootfs_version(b));
}

static inline int
benv_firmware_version_cmp(int a, int b)
{
    int ret;

    ret = benv_rootfs_version_cmp(a, b);
    if (ret) {
        return ret;
    }

    ret = benv_kernel_version_cmp(a, b);
    if (ret) {
        return ret;
    }

    return 0;
}

#define benv_obj_version_cmp(_obj, _a, _b)  benv_##_obj##_version_cmp(_a, _b)
#define benv_obj_version_eq(_obj, _a, _b)   (0==benv_obj_version_cmp(_obj, _a, _b))

enum {
    __benv_mark_ptest_control   = 0,
    __benv_mark_ptest_result    = 1,
    __benv_mark_debug           = 2,
    
    __benv_mark_protect_begin   = 3,
    __benv_mark_uptimes         = __benv_mark_protect_begin,
    __benv_mark_runtime         = 4,
    __benv_mark_cid_mid         = 5,
    __benv_mark_cid_psn         = 6,
    __benv_mark_noauth          = 7,
    __benv_mark_magic           = 8,
    __benv_mark_protect_end     = 9,
    __benv_mark_jdebug          = __benv_mark_protect_end,
    
    __benv_mark_end
};

static inline void
benv_debug_init(void)
{
#ifdef __BOOT__
    __THIS_DEBUG    = &benv_mark(__benv_mark_debug);
    __THIS_JDEBUG   = &benv_mark(__benv_mark_jdebug);
#endif
}

/*
* step 1:normal
* 1. firmware, bad < good
* 2. version, small < big
*
* step 2:exact
* 1. error, bad < good
* 2. upgrade, fail < unknow < ok
* 3. other, fail < unknow < ok
* 4. self, fail < unknow < ok
*/
extern int
benv_vcs_cmp(char *obj, benv_vcs_t * a, benv_vcs_t * b);

static inline void
benv_obj_cmp_dump(char *obj, int a, int b, int ret)
{
#if BENV_DEBUG & BENV_DEBUG_CMP
    os_println("%s%d %c %s%d", 
        obj, a, 
        OS_CMP_OPERATOR(ret), 
        obj, b);
#endif
}

static inline int
benv_kernel_cmp(int a, int b)
{
    int ret = benv_vcs_cmp("kernel", benv_kernel(a), benv_kernel(b));

    benv_obj_cmp_dump("kernel", a, b, ret);
    
    return ret;
}

static inline int
benv_rootfs_cmp(int a, int b)
{
    int ret = benv_vcs_cmp("rootfs", benv_rootfs(a), benv_rootfs(b));

    benv_obj_cmp_dump("rootfs", a, b, ret);

    return ret;
}

/*
* 1. first,  cmp rootfs
* 2. second, cmp kernel
*/
static inline int
__benv_firmware_cmp(int a, int b)
{
    int ret;

    ret = benv_rootfs_cmp(a, b);
    if (ret) {
        return ret;
    }

    ret = benv_kernel_cmp(a, b);
    if (ret) {
        return ret;
    }
    
    return 0;
}

static inline int
benv_firmware_cmp(int a, int b)
{
    int ret = __benv_firmware_cmp(a, b);

    benv_obj_cmp_dump("firmware", a, b, ret);
    
    return ret;
}

/*
* return sort idx
*/
extern int
__benv_obj_min(char *obj, int sort[], int count, int (*cmp)(int a, int b));

/*
* return sort idx
*/
extern int
__benv_obj_max(char *obj, int sort[], int count, int (*cmp)(int a, int b));

static inline int
benv_kernel_min(int sort[], int count)
{
    return __benv_obj_min("kernel", sort, count, benv_kernel_cmp);
}

static inline int
benv_kernel_max(int sort[], int count)
{
    return __benv_obj_max("kernel", sort, count, benv_kernel_cmp);
}

static inline int
benv_rootfs_min(int sort[], int count)
{
    return __benv_obj_min("rootfs", sort, count, benv_rootfs_cmp);
}

static inline int
benv_rootfs_max(int sort[], int count)
{
    return __benv_obj_max("rootfs", sort, count, benv_rootfs_cmp);
}

static inline int
benv_firmware_min(int sort[], int count)
{
    return __benv_obj_min("firmware", sort, count, benv_firmware_cmp);
}

static inline int
benv_firmware_max(int sort[], int count)
{
    return __benv_obj_max("firmware", sort, count, benv_firmware_cmp);
}

#define benv_obj_min(_obj)    benv_##_obj##_min
#define benv_obj_max(_obj)    benv_##_obj##_max

extern void
__benv_sort(int sort[], int count, int (*maxmin)(int sort[], int count));

#define benv_skips(_idx)                (os_bit(0) | os_bit(__benv_current) | os_bit(_idx))
#define is_benv_skip(_skips, _idx)      os_hasbit(_skips, _idx)

static inline int
benv_first_idx(int current, int skips)
{
    int i;

    os_firmware_foreach(i) {
        if (false==is_benv_skip(skips, i)) {
            return i;
        }
    }

    trace_assert(0, "no found first idx");
    
    return (1==current?2:1);
}

extern int
__benv_sort_count(int skips, int sort[], int size);
#define benv_sort_count(_skips, _sort)    __benv_sort_count(_skips, _sort, os_count_of(_sort))

static inline void
__benv_sort_head_dump(char *name, int sort[], int count)
{
#if BENV_DEBUG & BENV_DEBUG_SORT
    int i;
    
    os_println("%s-sort index", name);
    for (i=0; i<count; i++) {
        os_printf("%-4d", i);
    }
    os_println(__empty);
#endif
}

static inline void
__benv_sort_entry_dump(int sort[], int count)
{
#if BENV_DEBUG & BENV_DEBUG_SORT
    int i;
    
    for (i=0; i<count; i++) {
        os_printf("%-4d", sort[i]);
    }
    os_println(__empty);
#endif
}

static inline void
__benv_sort_before(char *name, int sort[], int count)
{
    __benv_sort_head_dump(name, sort, count);
    __benv_sort_entry_dump(sort, count);
}

static inline void
__benv_sort_after(int sort[], int count)
{
    __benv_sort_entry_dump(sort, count);
}

#define __benv_upsort(_obj, _sort, _count)      do{ \
    int count_in___benv_upsort = _count;                            \
                                                                    \
    __benv_sort_before("up", _sort, count_in___benv_upsort);        \
    __benv_sort(_sort, count_in___benv_upsort, benv_obj_min(_obj)); \
    __benv_sort_after(_sort, count_in___benv_upsort);               \
}while(0)

#define __benv_downsort(_obj, _sort, _count)    do{ \
    int count_in___benv_downsort = _count;                              \
                                                                        \
    __benv_sort_before("down", _sort, count_in___benv_downsort);        \
    __benv_sort(_sort, count_in___benv_downsort, benv_obj_max(_obj));   \
    __benv_sort_after(_sort, count_in___benv_downsort);                 \
}while(0)

#define benv_upsort(_obj, _skips, _sort) \
        __benv_upsort(_obj, _sort, benv_sort_count(_skips, _sort))

#define benv_downsort(_obj, _skips, _sort) \
        __benv_downsort(_obj, _sort, benv_sort_count(_skips, _sort))

/*
* skip 0 and current
*/
#define __benv_find_xest(_obj, _skips, _sort_func, _is_func) ({ \
    int sort_in___benv_find_xest[PRODUCT_FIRMWARE_COUNT] = {0};           \
    int i_in___benv_find_xest;          \
    int idx_in___benv_find_xest;        \
    int xest_in___benv_find_xest = -ENOEXIST; \
    int count_in___benv_find_xest;      \
                                        \
    count_in___benv_find_xest = benv_sort_count(_skips, sort_in___benv_find_xest);\
    _sort_func(_obj, sort_in___benv_find_xest, count_in___benv_find_xest);      \
                                        \
    for (i_in___benv_find_xest=0;       \
         i_in___benv_find_xest<count_in___benv_find_xest; \
         i_in___benv_find_xest++) {     \
        idx_in___benv_find_xest = sort_in___benv_find_xest[i_in___benv_find_xest];                  \
                                        \
        if (_is_func(_obj, idx_in___benv_find_xest)) {      \
            xest_in___benv_find_xest = idx_in___benv_find_xest;                 \
                                        \
            break;                      \
        }                               \
    }                                   \
                                        \
    xest_in___benv_find_xest;                               \
}) /* end */
    
#define __benv_obj_always_true(_obj, _idx)    true

#define benv_find_best(_obj, _skips) ({ \
    int idx_in_benv_find_best = __benv_find_xest(_obj, _skips, __benv_downsort, is_benv_good_obj); \
    debug_trace("find best:%d", idx_in_benv_find_best); \
    idx_in_benv_find_best;                              \
})  /* end */

#define benv_find_worst(_obj, _skips)   ({  \
    int idx_in_benv_find_worst = __benv_find_xest(_obj, _skips, __benv_upsort, __benv_obj_always_true); \
    debug_trace("find worst:%d", idx_in_benv_find_worst); \
    idx_in_benv_find_worst; \
})  /* end */

/*
* skip 0 and idx
*/
#define __benv_find_first_buddy(_obj, _idx, _skips, _is_func) ({    \
    int i_in___benv_find_first_buddy;       \
    int ret_in___benv_find_first_buddy = -ENOEXIST; \
    int skips_in___benv_find_first_buddy = (_skips) | os_bit(_idx); \
                                            \
    os_firmware_foreach(i_in___benv_find_first_buddy) {                \
        if (false==is_benv_skip(skips_in___benv_find_first_buddy, i_in___benv_find_first_buddy) \
            && benv_obj_version_eq(_obj, i_in___benv_find_first_buddy, _idx) \
            && is_benv_good_obj(_obj, i_in___benv_find_first_buddy)) { \
            ret_in___benv_find_first_buddy = i_in___benv_find_first_buddy; \
            break;                          \
        }                                   \
    }                                       \
                                            \
    ret_in___benv_find_first_buddy;         \
})  /* end */

#define benv_find_first_buddy(_obj, _idx, _skips) \
        __benv_find_first_buddy(_obj, _idx, _skips, __benv_obj_always_true)

#define benv_find_first_good_buddy(_obj, _idx, _skips) \
        __benv_find_first_buddy(_obj, _idx, _skips, is_benv_good_obj)

#define benv_find_first_bad_buddy(_obj, _idx, _skips) \
        __benv_find_first_buddy(_obj, _idx, _skips, is_benv_bad_obj)

#define __benv_find_first_byversion(_obj, _version, _skips, _is_func) ({ \
    int i_in___benv_find_first_byversion;               \
    int idx_in___benv_find_first_byversion = -ENOEXIST; \
    int skips_in___benv_find_first_byversion = _skips;  \
    benv_version_t *version_in___benv_find_first_byversion = _version; \
                                                        \
    os_firmware_foreach(i_in___benv_find_first_byversion){ \
        if (false==is_benv_skip(skips_in___benv_find_first_byversion, i_in___benv_find_first_byversion) \
            && benv_version_eq(version_in___benv_find_first_byversion, benv_obj_version(_obj, i_in___benv_find_first_byversion)) \
            && _is_func(_obj, i_in___benv_find_first_byversion)) { \
            idx_in___benv_find_first_byversion = i_in___benv_find_first_byversion; \
            break;                                      \
        }                                               \
    }                                                   \
                                                        \
    idx_in___benv_find_first_byversion;                 \
})  /* end */

#define benv_find_first_byversion(_obj, _version, _skips) \
        __benv_find_first_byversion(_obj, _version, _skips, __benv_obj_always_true)

#define benv_find_first_good_byversion(_obj, _version, _skips) \
        __benv_find_first_byversion(_obj, _version, _skips, is_benv_good_obj)

#define benv_find_first_bad_byversion(_obj, _version, _skips) \
        __benv_find_first_byversion(_obj, _version, _skips, is_benv_bad_obj)

static inline void
__benv_show_header(benv_ops_t *ops)
{
    if (__benv_show_count > 1) {
        os_printf("%s=", ops->path);
    }
}

static inline void
__benv_show_number(benv_ops_t *ops)
{
    __benv_show_header(ops);

    os_println("%u", *benv_ops_number(ops));
}

static inline void
__benv_show_string(benv_ops_t *ops)
{
    char *string = benv_ops_string(ops);

    if (string[0]) {
        __benv_show_header(ops);

        os_println("%s", string);
    }
}

static inline void
__benv_show_string_all(benv_ops_t *ops)
{
    char *string = benv_ops_string(ops);

    __benv_show_header(ops);

    os_println("%s", string[0]?string:__empty);
}

static inline void
__benv_set_number(benv_ops_t *ops, char *value)
{
    *benv_ops_number(ops) = (uint32)(value[0] ? os_atoi(value) : 0);
}

static inline void
__benv_set_string(benv_ops_t *ops, char *value)
{
    char *string = benv_ops_string(ops);

    if (value[0]) {
        os_strcpy(string, value);
    } else {
        string[0] = 0;
    }
}

extern int
__benv_check_version(benv_ops_t *ops, char *value);

static inline void
__benv_show_version(benv_ops_t *ops)
{
    char version_string[1+BENV_VERSION_STRING_LEN];
    
    __benv_show_header(ops);

    os_println("%s", benv_version_itoa(benv_ops_version(ops), version_string));
}

static inline void
__benv_set_version(benv_ops_t *ops, char *value)
{
    benv_version_t *v = benv_ops_version(ops);

    if (value[0]) {
        benv_version_atoi(v, value);
    } else {
        benv_version_t version = BENV_DEFT_VERSION;

        os_objscpy(v, &version);
    }
}

extern int
__benv_check_fsm(benv_ops_t *ops, char *value);

static inline void
__benv_show_fsm(benv_ops_t *ops)
{
    int fsm = *benv_ops_fsm(ops);

    __benv_show_header(ops);

    os_println("%s", benv_fsm_getnamebyid(fsm));
}

static inline void
__benv_set_fsm(benv_ops_t *ops, char *value)
{
    uint32 fsm;

    if (value[0]) {
        fsm = benv_fsm_getidbyname(value);
    } else {
        fsm = BENV_FSM_UNKNOW;
    }

    *benv_ops_fsm(ops) = fsm;
}

extern int
__benv_check_current(benv_ops_t *ops, char *value);

extern int
__benv_check_string(benv_ops_t *ops, char *value);

#define BENV_OPS(_path, _type, _member, _check, _write, _show) { \
    .path   = _path,    \
    .type   = _type,    \
    .offset = offsetof(benv_env_t, _member), \
    .check  = _check,   \
    .write  = _write,   \
    .show   = _show,    \
}   /* end */

#define __BENV_COOKIE_OPS(_member, _show) \
    BENV_OPS("cookies/" #_member, BENV_OPS_STRING, cookie._member, NULL, NULL, _show) \
    /* end */

#define BENV_COOKIE_OPS \
    __BENV_COOKIE_OPS(vendor,   __benv_show_string),    \
    __BENV_COOKIE_OPS(company,  __benv_show_string),    \
    __BENV_COOKIE_OPS(model,    __benv_show_string),    \
    __BENV_COOKIE_OPS(version,  __benv_show_string),    \
    __BENV_COOKIE_OPS(compile,  __benv_show_string)     \
    /* end */

#define __BENV_OS_OPS(_path, _type, _member, _check, _write, _show) \
    BENV_OPS("os/" _path, _type, os._member, _check, _write, _show) \
    /* end */

#define __BENV_FIRMWARE_OPS(_obj, _idx)         \
    __BENV_OS_OPS(#_obj "/" #_idx "/self",      \
        BENV_OPS_NUMBER,                        \
        firmware[_idx]._obj.self,               \
        __benv_check_fsm, __benv_set_fsm, __benv_show_fsm), \
    __BENV_OS_OPS(#_obj "/" #_idx "/other",     \
        BENV_OPS_NUMBER,                        \
        firmware[_idx]._obj.other,              \
        __benv_check_fsm, __benv_set_fsm, __benv_show_fsm), \
    __BENV_OS_OPS(#_obj "/" #_idx "/upgrade",   \
        BENV_OPS_NUMBER,                        \
        firmware[_idx]._obj.upgrade,            \
        __benv_check_fsm, __benv_set_fsm, __benv_show_fsm), \
    __BENV_OS_OPS(#_obj "/" #_idx "/error",     \
        BENV_OPS_NUMBER,                        \
        firmware[_idx]._obj.error,              \
        NULL, __benv_set_number, __benv_show_number), \
    __BENV_OS_OPS(#_obj "/" #_idx "/version",   \
        BENV_OPS_VERSION,                        \
        firmware[_idx]._obj.version,            \
        __benv_check_version, __benv_set_version, __benv_show_version), \
    __BENV_OS_OPS(#_obj "/" #_idx "/cookie",    \
        BENV_OPS_STRING,                        \
        firmware[_idx]._obj.cookie,             \
        __benv_check_string, __benv_set_string, __benv_show_string) \
    /* end */

#define BENV_FIRMWARE_OPS(_idx)         \
    __BENV_FIRMWARE_OPS(kernel, _idx),  \
    __BENV_FIRMWARE_OPS(rootfs, _idx)   \
    /* end */

#define BENV_OS_COMMON_OPS              \
    __BENV_OS_OPS("current",            \
        BENV_OPS_NUMBER,                \
        current,                        \
        __benv_check_current, __benv_set_number, __benv_show_number) \
    /* end */

#define BENV_OS_OPS         \
    BENV_OS_COMMON_OPS,     \
    BENV_FIRMWARE_OPS(0),   \
    BENV_FIRMWARE_OPS(1),   \
    BENV_FIRMWARE_OPS(2),   \
    BENV_FIRMWARE_OPS(3),   \
    BENV_FIRMWARE_OPS(4),   \
    BENV_FIRMWARE_OPS(5),   \
    BENV_FIRMWARE_OPS(6)    \
    /* end */

#define __BENV_MARK_OPS(_path, _idx, _check, _set, _show) \
    BENV_OPS("marks/" _path, BENV_OPS_NUMBER, mark.var[_idx], _check, __benv_set_number, __benv_show_number)

#define __BENV_MARK_OPS_IDX(_idx) \
    __BENV_MARK_OPS(#_idx, _idx, NULL, __benv_set_number, __benv_show_number)

#define __BENV_MARK_OPS_IDX_RO(_idx) \
    __BENV_MARK_OPS(#_idx, _idx, NULL, NULL, __benv_show_number)

#define BENV_MARK_OPS_NAMES \
    __BENV_MARK_OPS("ptest/control",__benv_mark_ptest_control,  NULL, __benv_set_number,__benv_show_number), \
    __BENV_MARK_OPS("ptest/result", __benv_mark_ptest_result,   NULL, __benv_set_number,__benv_show_number), \
    __BENV_MARK_OPS("debug",        __benv_mark_debug,          NULL, __benv_set_number,__benv_show_number), \
    __BENV_MARK_OPS("ut",           __benv_mark_uptimes,        NULL, NULL,             __benv_show_number), \
    __BENV_MARK_OPS("rt",           __benv_mark_runtime,        NULL, NULL,             __benv_show_number), \
    __BENV_MARK_OPS("mid",          __benv_mark_cid_mid,        NULL, NULL,             __benv_show_number), \
    __BENV_MARK_OPS("psn",          __benv_mark_cid_psn,        NULL, NULL,             __benv_show_number), \
    __BENV_MARK_OPS("na",           __benv_mark_noauth,         NULL, NULL,             __benv_show_number), \
    __BENV_MARK_OPS("mg",           __benv_mark_magic,          NULL, NULL,             __benv_show_number), \
    __BENV_MARK_OPS("jdebug",       __benv_mark_jdebug,         NULL, NULL,             __benv_show_number)  \
    /* end */

#define BENV_MARK_OPS_IDX       \
    __BENV_MARK_OPS_IDX(10),    \
    __BENV_MARK_OPS_IDX(11),    \
    __BENV_MARK_OPS_IDX(12),    \
    __BENV_MARK_OPS_IDX(13),    \
    __BENV_MARK_OPS_IDX(14),    \
    __BENV_MARK_OPS_IDX(15),    \
    __BENV_MARK_OPS_IDX(16),    \
    __BENV_MARK_OPS_IDX(17),    \
    __BENV_MARK_OPS_IDX(18),    \
    __BENV_MARK_OPS_IDX(19),    \
                                \
    __BENV_MARK_OPS_IDX(20),    \
    __BENV_MARK_OPS_IDX(21),    \
    __BENV_MARK_OPS_IDX(22),    \
    __BENV_MARK_OPS_IDX(23),    \
    __BENV_MARK_OPS_IDX(24),    \
    __BENV_MARK_OPS_IDX(25),    \
    __BENV_MARK_OPS_IDX(26),    \
    __BENV_MARK_OPS_IDX(27),    \
    __BENV_MARK_OPS_IDX(28),    \
    __BENV_MARK_OPS_IDX(29),    \
                                \
    __BENV_MARK_OPS_IDX(30),    \
    __BENV_MARK_OPS_IDX(31),    \
    __BENV_MARK_OPS_IDX(32),    \
    __BENV_MARK_OPS_IDX(33),    \
    __BENV_MARK_OPS_IDX(34),    \
    __BENV_MARK_OPS_IDX(35),    \
    __BENV_MARK_OPS_IDX(36),    \
    __BENV_MARK_OPS_IDX(37),    \
    __BENV_MARK_OPS_IDX(38),    \
    __BENV_MARK_OPS_IDX(39),    \
    __BENV_MARK_OPS_IDX(40),    \
                                \
    __BENV_MARK_OPS_IDX(41),    \
    __BENV_MARK_OPS_IDX(42),    \
    __BENV_MARK_OPS_IDX(43),    \
    __BENV_MARK_OPS_IDX(44),    \
    __BENV_MARK_OPS_IDX(45),    \
    __BENV_MARK_OPS_IDX(46),    \
    __BENV_MARK_OPS_IDX(47),    \
    __BENV_MARK_OPS_IDX(48),    \
    __BENV_MARK_OPS_IDX(49),    \
                                \
    __BENV_MARK_OPS_IDX(50),    \
    __BENV_MARK_OPS_IDX(51),    \
    __BENV_MARK_OPS_IDX(52),    \
    __BENV_MARK_OPS_IDX(53),    \
    __BENV_MARK_OPS_IDX(54),    \
    __BENV_MARK_OPS_IDX(55),    \
    __BENV_MARK_OPS_IDX(56),    \
    __BENV_MARK_OPS_IDX(57),    \
    __BENV_MARK_OPS_IDX(58),    \
    __BENV_MARK_OPS_IDX(59),    \
                                \
    __BENV_MARK_OPS_IDX(60),    \
    __BENV_MARK_OPS_IDX(61),    \
    __BENV_MARK_OPS_IDX(62),    \
    __BENV_MARK_OPS_IDX(63),    \
    __BENV_MARK_OPS_IDX(64),    \
    __BENV_MARK_OPS_IDX(65),    \
    __BENV_MARK_OPS_IDX(66),    \
    __BENV_MARK_OPS_IDX(67),    \
    __BENV_MARK_OPS_IDX(68),    \
    __BENV_MARK_OPS_IDX(69),    \
                                \
    __BENV_MARK_OPS_IDX(70),    \
    __BENV_MARK_OPS_IDX(71),    \
    __BENV_MARK_OPS_IDX(72),    \
    __BENV_MARK_OPS_IDX(73),    \
    __BENV_MARK_OPS_IDX(74),    \
    __BENV_MARK_OPS_IDX(75),    \
    __BENV_MARK_OPS_IDX(76),    \
    __BENV_MARK_OPS_IDX(77),    \
    __BENV_MARK_OPS_IDX(78),    \
    __BENV_MARK_OPS_IDX(79),    \
                                \
    __BENV_MARK_OPS_IDX(80),    \
    __BENV_MARK_OPS_IDX(81),    \
    __BENV_MARK_OPS_IDX(82),    \
    __BENV_MARK_OPS_IDX(83),    \
    __BENV_MARK_OPS_IDX(84),    \
    __BENV_MARK_OPS_IDX(85),    \
    __BENV_MARK_OPS_IDX(86),    \
    __BENV_MARK_OPS_IDX(87),    \
    __BENV_MARK_OPS_IDX(88),    \
    __BENV_MARK_OPS_IDX(89),    \
                                \
    __BENV_MARK_OPS_IDX(90),    \
    __BENV_MARK_OPS_IDX(91),    \
    __BENV_MARK_OPS_IDX(92),    \
    __BENV_MARK_OPS_IDX(93),    \
    __BENV_MARK_OPS_IDX(94),    \
    __BENV_MARK_OPS_IDX(95),    \
    __BENV_MARK_OPS_IDX(96),    \
    __BENV_MARK_OPS_IDX(97),    \
    __BENV_MARK_OPS_IDX(98),    \
    __BENV_MARK_OPS_IDX(99),    \
                                \
    __BENV_MARK_OPS_IDX(100),   \
    __BENV_MARK_OPS_IDX(101),   \
    __BENV_MARK_OPS_IDX(102),   \
    __BENV_MARK_OPS_IDX(103),   \
    __BENV_MARK_OPS_IDX(104),   \
    __BENV_MARK_OPS_IDX(105),   \
    __BENV_MARK_OPS_IDX(106),   \
    __BENV_MARK_OPS_IDX(107),   \
    __BENV_MARK_OPS_IDX(108),   \
    __BENV_MARK_OPS_IDX(109),   \
                                \
    __BENV_MARK_OPS_IDX(110),   \
    __BENV_MARK_OPS_IDX(111),   \
    __BENV_MARK_OPS_IDX(112),   \
    __BENV_MARK_OPS_IDX(113),   \
    __BENV_MARK_OPS_IDX(114),   \
    __BENV_MARK_OPS_IDX(115),   \
    __BENV_MARK_OPS_IDX(116),   \
    __BENV_MARK_OPS_IDX(117),   \
    __BENV_MARK_OPS_IDX(118),   \
    __BENV_MARK_OPS_IDX(119),   \
                                \
    __BENV_MARK_OPS_IDX(120),   \
    __BENV_MARK_OPS_IDX(121),   \
    __BENV_MARK_OPS_IDX(122),   \
    __BENV_MARK_OPS_IDX(123),   \
    __BENV_MARK_OPS_IDX(124),   \
    __BENV_MARK_OPS_IDX(125),   \
    __BENV_MARK_OPS_IDX(126),   \
    __BENV_MARK_OPS_IDX(127)    \
    /* end */

#define BENV_MARK_OPS       \
    BENV_MARK_OPS_NAMES,    \
    BENV_MARK_OPS_IDX       \
    /* end */

#define __BENV_INFO_OPS_RO(_path, _idx) \
    BENV_OPS("infos/" _path, BENV_OPS_STRING, info.var[_idx], NULL, NULL, __benv_show_string) \
    /* end */

#define __BENV_INFO_OPS_RW(_path, _idx) \
    BENV_OPS("infos/" _path, BENV_OPS_STRING, info.var[_idx], __benv_check_string, __benv_set_string, __benv_show_string) \
    /* end */

enum {
    /*
    * 0 and 1 deleted, resverd
    */
    __benv_info_pcba_model      = 2,
    __benv_info_pcba_mac        = 3,
    __benv_info_pcba_sn         = 4,
    __benv_info_pcba_version    = 5,
    
    __benv_info_product_vendor  = 6,
    __benv_info_product_company = 7,
    __benv_info_product_model   = 8,
    __benv_info_product_mac     = 9,
    __benv_info_product_sn      = 10,
    __benv_info_product_manager = 11,
    __benv_info_product_version = 12,
    
    __benv_info_oem_vendor      = 13,
    __benv_info_oem_company     = 14,
    __benv_info_oem_model       = 15,
    __benv_info_oem_mac         = 16,
    __benv_info_oem_sn          = 17,
    __benv_info_oem_manager     = 18,
    __benv_info_oem_version     = 19,

    __benv_info_pcba_rootrw     = 20,

    __benv_info_idx_max
};

#define BENV_INFO_RO_LIST       {   \
    __benv_info_pcba_model,         \
    __benv_info_pcba_version,       \
    __benv_info_product_vendor,     \
    __benv_info_product_company,    \
    __benv_info_product_model,      \
    __benv_info_product_manager,    \
    __benv_info_product_version,    \
}   /* end */

#define BENV_INFO_OPS_NAMES  \
    __BENV_INFO_OPS_RO("pcba/model",    __benv_info_pcba_model),        \
    __BENV_INFO_OPS_RW("pcba/mac",      __benv_info_pcba_mac),          \
    __BENV_INFO_OPS_RW("pcba/sn",       __benv_info_pcba_sn),           \
    __BENV_INFO_OPS_RO("pcba/version",  __benv_info_pcba_version),      \
    __BENV_INFO_OPS_RW("pcba/rootrw",   __benv_info_pcba_rootrw),       \
                                                                        \
    __BENV_INFO_OPS_RO("product/vendor",    __benv_info_product_vendor),    \
    __BENV_INFO_OPS_RO("product/company",   __benv_info_product_company),   \
    __BENV_INFO_OPS_RO("product/model",     __benv_info_product_model),     \
    __BENV_INFO_OPS_RW("product/mac",       __benv_info_product_mac),       \
    __BENV_INFO_OPS_RW("product/sn",        __benv_info_product_sn),        \
    __BENV_INFO_OPS_RO("product/manager",   __benv_info_product_manager),   \
    __BENV_INFO_OPS_RO("product/version",   __benv_info_product_version),   \
                                                                        \
    __BENV_INFO_OPS_RW("oem/vendor",    __benv_info_oem_vendor),        \
    __BENV_INFO_OPS_RW("oem/company",   __benv_info_oem_company),       \
    __BENV_INFO_OPS_RW("oem/model",     __benv_info_oem_model),         \
    __BENV_INFO_OPS_RW("oem/mac",       __benv_info_oem_mac),           \
    __BENV_INFO_OPS_RW("oem/sn",        __benv_info_oem_sn),            \
    __BENV_INFO_OPS_RW("oem/manager",   __benv_info_oem_manager),       \
    __BENV_INFO_OPS_RW("oem/version",   __benv_info_oem_version)        \
    /* end */

#define __BENV_INFO_OPS_IDX(_idx)   __BENV_INFO_OPS_RW(#_idx, _idx)
#define BENV_INFO_OPS_IDX       \
    __BENV_INFO_OPS_IDX(21),    \
    __BENV_INFO_OPS_IDX(22),    \
    __BENV_INFO_OPS_IDX(23),    \
    __BENV_INFO_OPS_IDX(24),    \
    __BENV_INFO_OPS_IDX(25),    \
    __BENV_INFO_OPS_IDX(26),    \
    __BENV_INFO_OPS_IDX(27),    \
    __BENV_INFO_OPS_IDX(28),    \
    __BENV_INFO_OPS_IDX(29),    \
                                \
    __BENV_INFO_OPS_IDX(30),    \
    __BENV_INFO_OPS_IDX(31),    \
    __BENV_INFO_OPS_IDX(32),    \
    __BENV_INFO_OPS_IDX(33),    \
    __BENV_INFO_OPS_IDX(34),    \
    __BENV_INFO_OPS_IDX(35),    \
    __BENV_INFO_OPS_IDX(36),    \
    __BENV_INFO_OPS_IDX(37),    \
    __BENV_INFO_OPS_IDX(38),    \
    __BENV_INFO_OPS_IDX(39)     \
    /* end */

#define BENV_INFO_OPS       \
    BENV_INFO_OPS_NAMES,    \
    BENV_INFO_OPS_IDX       \
    /* end */

#define BENV_DEFT_OPS { \
    BENV_COOKIE_OPS,    \
    BENV_OS_OPS,        \
    BENV_MARK_OPS,      \
    BENV_INFO_OPS,      \
}   /* end */

static inline int
benv_control_init(void)
{
    if (NULL==____benv_control) {
        benv_ops_t ops[] = BENV_DEFT_OPS;
        int i, count = os_count_of(ops);

        ____benv_control = (benv_control_t *)os_zalloc(sizeof(benv_control_t));
        if (NULL==____benv_control) {
            return -ENOMEM;
        }
        __benv_ops_count = count;
        __benv_fd = INVALID_FD;
        os_shm_init(__benv_shm, OS_BENV_SHM_ID);
#ifdef __BOOT__
        extern void benv_boot_init(void);

        benv_boot_init();
#endif
        __benv_mirror = (benv_env_t *)os_zalloc(sizeof(benv_env_t));
        if (NULL==__benv_mirror) {
            return -ENOMEM;
        }

        __benv_ops = (benv_ops_t *)os_zalloc(sizeof(ops));
        if (NULL==__benv_ops) {
            return -ENOMEM;
        }
        os_memcpy(__benv_ops, ops, sizeof(ops));
        benv_ops_selfcheck();
        
        __benv_cache = (benv_cache_t *)os_zalloc(count*sizeof(benv_cache_t));
        if (NULL==__benv_cache) {
            return -ENOMEM;
        }
    }
    
    return 0;
}

static inline void
__benv_show_byprefix(void (*show)(benv_ops_t* ops), char *prefix)
{
    int i;
    int len = os_strlen(prefix);

    show = show?show:benv_ops_show;
    
    for (i = 0; i < __benv_ops_count; i++) {
        benv_ops_t *ops = benv_ops(i);

        if (benv_ops_match_wildcard(ops, prefix, len)) {
            (*show)(ops);
        }
    }
}

#define benv_show_byprefix(_show, _fmt, _args...) do{\
    char prefix[1+OS_LINE_LEN];             \
                                            \
    os_saprintf(prefix, _fmt, ##_args);     \
                                            \
    __benv_show_byprefix(_show, prefix);    \
}while(0)

static inline void
__benv_handle_write(benv_ops_t *ops)
{
    char *value = benv_cache_value(ops);

    if (value) {
        benv_ops_write(ops, value);
    }
}

static inline void
__benv_handle_show(benv_ops_t *ops)
{
    benv_ops_show(ops);
}

static inline void
__benv_handle(benv_ops_t *ops)
{
    /*
     * show
     */
    if (benv_cache_showit(ops)) {
        __benv_handle_show(ops);
    }
    /*
     * write
     */
    else if (benv_cache_value(ops)) {
        __benv_handle_write(ops);
    }
}

static inline void
benv_handle(int argc, char *argv[])
{
    int i;

    for (i = 0; i < __benv_ops_count; i++) {
        __benv_handle(benv_ops(i));
    }
}

static inline int
benv_usage(void)
{
    os_eprintln("%s name ==> show env by name", __benv_self);
    os_eprintln("%s name1=value1 name2=value2 ... ==> set env by name and value", __benv_self);

    return -EHELP;
}

static inline int
__benv_analysis_write(benv_ops_t *ops, char *args)
{
    char *path = args;
    char *eq = os_strchr(args, '=');
    char *value = eq + 1;
    int err;

    if (benv_ops_match(ops, path, eq - path, false)) {
        err = benv_ops_check(ops, value);
        if (err<0) {
            return err;
        }

        benv_cache_value(ops) = value;
    }
    
    return 0;
}

static inline int
benv_analysis_write(char *args)
{
    int i, err = 0;

    for (i = 0; i < __benv_ops_count; i++) {
        err = __benv_analysis_write(benv_ops(i), args);
        if (err<0) {
            return err;
        }
    }

    return 0;
}

static inline int
__benv_analysis_show(benv_ops_t *ops, char *args)
{
    char *wildcard = os_strlast(args, '*');

    /*
     * if found '*'
     *   first '*' is not last '*'
     * if NOT found '*'
     *   but found last '*'
     */
    if (os_strchr(args, '*') != wildcard) {
        debug_error("only support show 'xxx*'");

        return -EFORMAT;
    } else if (benv_ops_match(ops, args, os_strlen(args), !!wildcard)) {
        debug_trace("need show ops %s", ops->path);
        
        benv_cache_showit(ops) = true;

        __benv_show_count++;
    }

    debug_trace("show what ???");
    
    return 0;
}

static inline int
benv_analysis_show(char *args)
{
    int i, err = 0;
    
    for (i = 0; i < __benv_ops_count; i++) {
        err = __benv_analysis_show(benv_ops(i), args);
        if (err<0) {
            return err;
        }
    }
    
    return 0;
}

static inline int
__benv_analysis(char *args)
{
    /*
     * first is '=', bad format
     */
    if ('=' == args[0]) {
        debug_error("first is '='");

        return -EFORMAT;
    }
    /*
     * first is '*', bad format
     */
    else if ('*' == args[0]) {
        debug_error("first is '*'");

        return -EFORMAT;
    }
    /*
     * found '=', is wirte
     */
    else if (os_strchr(args, '=')) {
        debug_trace("found '=', is write");
        
        return benv_analysis_write(args);
    }
    /*
     * no found '=', is show
     */
    else {
        debug_trace("no found '=', is show");
        
        return benv_analysis_show(args);
    }
}

static inline int
benv_analysis(int argc, char *argv[])
{
    int i, err;
    
    for (i = 0; i < argc; i++) {
        err = __benv_analysis(argv[i]);
        if (err<0) {
            return err;
        }
    }

    return 0;
}

static inline int
benv_command(int argc, char *argv[])
{
    int err, i;

    if (0==argc) {
        return benv_usage();
    }

    err = benv_analysis(argc, argv);
    if (err<0) {
        return err;
    }

    benv_cache_dump();

    benv_handle(argc, argv);
    
    return 0;
}

static inline void
benv_show_cookie(void)
{
    __benv_cookie_show(__benv_cookie(0));
}

static inline void
benv_show_os(void)
{
    __benv_os_show(__benv_os(0));
}

static inline void
__benv_show_path_helper(benv_ops_t* ops)
{
    os_println("%s", ops->path);
}

static inline void
benv_show_path(void)
{
    __benv_show_byprefix(__benv_show_path_helper, NULL);
}

static inline void
__benv_show_helper(benv_ops_t* ops)
{
    switch(ops->type) {
        case BENV_OPS_NUMBER:
            __benv_show_number(ops);
            break;
        case BENV_OPS_STRING:
            if (__benv_show_empty) {
                __benv_show_string_all(ops);
            } else {
                __benv_show_string(ops);
            }
            
            break;
        case BENV_OPS_VERSION:
            __benv_show_version(ops);
            break;
    }
}

static inline void
__benv_show_all(bool show_empty)
{
    int count   = __benv_show_count; __benv_show_count = 2;
    bool empty  = __benv_show_empty; __benv_show_empty = show_empty;
    
    __benv_show_byprefix(__benv_show_helper, NULL);
    
    __benv_show_count = count;
    __benv_show_empty = empty;
}

static inline void
benv_show_all(void)
{
    __benv_show_all(false);
}

static inline void
benv_show_hide(void)
{
    __benv_show_all(true);
}

#ifdef __BOOT__
extern int 
benv_emmc_read(uint32 begin, void *buf, int size);

extern int 
benv_emmc_write(uint32 begin, void *buf, int size);

extern void set_default_env(void);
extern void env_crc_update(void);
extern int saveenv(void);

extern int __benv_read(int env, bool mirror);
#if BENV_FLASH==BENV_FLASH_EMMC
    extern int __benv_write(int env, int idx /* no used */);
#elif BENV_FLASH==BENV_FLASH_NOR
#   define __benv_write(_env, _idx)             0
#endif
#elif defined(__APP__)
#if IS_PRODUCT_LTEFI_MD1
#   define __benv_read(int env, bool mirror)    0
#   define __benv_write(_env, _idx)             0
#elif IS_PRODUCT_LTEFI_MD_PARTITION_B || IS_PRODUCT_PC
static inline int
__benv_read(int env, bool mirror)
{    
    int err = 0;
    void *buf;
    char *name;

    if (mirror) {
        buf = __benv_mirror;
        name = "mirror";
    } else {
        buf = __benv_env(env);
        name = "benv";
    }

    err = lseek(__benv_fd, benv_offset(env), SEEK_SET);
    if (err < 0) { /* <0 is error */
        debug_error("seek benv%d error:%d", env, -errno);

        return -errno;
    }
    
    /*
    * emmc==>block
    */
    if (BENV_SIZE != read(__benv_fd, buf, BENV_SIZE)) {
        debug_error("read benv%d to %s error:%d", env, name, -errno);

        return -errno;
    }

    debug_ok("read benv%d to %s ok.", env, name);
    
    return 0;
}

/*
* write whole env when block = -1
*/
static inline int
__benv_write(int env, int idx)
{
    int err = 0;

    if (BENV_ALL==idx) {
        return -ENOSUPPORT;
    }
    
    debug_io("save benv%d block%d ...", env, idx);
    err = lseek(__benv_fd, benv_offset(env) + idx*BENV_BLOCK_SIZE, SEEK_SET);
    if (err < 0) { /* <0 is error */
         debug_error("seek benv%d block%d error:%d", env, idx, -errno);
        
        return -errno;
    }

    if (BENV_BLOCK_SIZE != write(__benv_fd, __benv_block(env, idx), BENV_BLOCK_SIZE)) {
         debug_error("save benv%d block%d error:%d", env, idx, -errno);
        
        return -errno;
    }
    debug_io("save benv%d block%d ok", env, idx);
    
    return 0;
}

#endif
#endif

static inline int
benv_open(void)
{
    int err = 0, fd;

    err = benv_control_init();
    if (err<0) {
        return err;
    }

    __benv_errno        = 0;
    __benv_show_count   = 0;

    if (__benv_cache) {
        os_memzero(__benv_cache, __benv_ops_count * sizeof(benv_cache_t));
    }

#if BENV_USE_SEM
    err = os_sem_create(__benv_sem, OS_BENV_SEM_ID);
    if (err<0) {
        return err;
    }
#endif
    
#ifdef __APP__
    err = os_shm_create(__benv_shm, BENV_SIZE*BENV_COUNT, false);
    if (err<0) {
        return err;
    }
    
    __benv_fd = os_file_open(PRODUCT_DEV_BOOTENV, O_RDWR | O_SYNC, 0);
    if (false==is_good_fd(__benv_fd)) {
        return __benv_fd;
    }
#endif

    return 0;
}

static inline int
benv_close(void)
{
    if (____benv_control) {
#ifdef __APP__
        os_close(__benv_fd);
        os_shm_destroy(__benv_shm);
#endif

#if BENV_USE_SEM
        os_sem_destroy(__benv_sem);
#endif
    }
    
    return 0;
}

#if BENV_REPAIR
static inline int
__benv_repair(void)
{
    int env, next, goodall = 0, goodfirst = -1, err = 0;
    bool good[BENV_COUNT] = {0};
    bool eq;

    /*
    * check same
    */
    for (env=0; env<BENV_COUNT; env++) {
        next = (env+1)%BENV_COUNT;
        
        eq = os_memeq(__benv_env(env), __benv_env(next), BENV_SIZE);
        if (eq) {
            good[env] = good[next] = true;
            if (goodfirst<0) {
                goodfirst = env;
            }
        }
        
        goodall += eq;
    }

    /*
    * all same, so all good
    */
    if (BENV_COUNT==goodall) {
        return 0;
    }

    if (goodfirst<0) {
        goodfirst = 0;
    }

    os_println("benv repair ...");
    for (env=0; env<BENV_COUNT; env++) {
        if (false==good[env]) {
            /*
            * repair env by goodfirst
            */
            os_memcpy(__benv_env(env),  __benv_env(goodfirst), BENV_SIZE);

            os_println("repair benv%d by benv%d", env, goodfirst);
        }
    }
    os_println("benv repair ok.");
    
    return 0;
}

static inline int
__benv_upgrade(void)
{
    int env;

    os_println("benv upgrade ...");
    for (env=1; env<BENV_COUNT; env++) {
        /*
        * repair by env0
        */
        os_memcpy(__benv_env(env),      __benv_env(0), BENV_SIZE);
    }
    os_println("benv upgrade ok.");
    
#ifdef __BOOT__
    set_default_env();
    env_crc_update();
    saveenv();
#endif

    return 0;
}
#endif

/*
* in boot, 
*   bootenv include benv
*   so, needn't load benv
*/
static inline bool
is_benv_loaded(void)
{
#ifdef __APP__
    return is_good_benv_cookie(__benv_cookie(0));
#else
    return true;
#endif
}

static inline int  benv_repair(void);
static inline void benv_check(void);

static inline int
benv_load(void)
{
    int env, err = 0;

    if (false==is_benv_loaded()) {
        for (env=0; env<BENV_COUNT; env++) {
            err = __benv_read(env, false);
            if (err<0) {
                return err;
            }
        }

        benv_repair();
    }

    benv_debug_init();

    /*
    * just for debug on PC
    */
#ifdef __PC__
    benv_check();
#endif
    
    return 0;
}

static inline int benv_save_nothing(void) {return 0;}

#if BENV_FLASH==BENV_FLASH_EMMC || defined(__APP__)
static inline int
__benv_save(int idx)
{
    int err, env;
    
    /*
    * save all env's block(idx)
    */
    for (env=0; env<BENV_COUNT; env++) {
        err = __benv_write(env, idx);
        if (err<0) {
            return err;
        }
    }

    return 0;
}
#elif BENV_FLASH==BENV_FLASH_NOR
#define __benv_save(_idx)   saveenv()
#endif

#ifdef __APP__
static inline int
__benv_saveby(int idx)
{
    int env, err = 0;

    /*
    * load mirror
    */
    err = __benv_read(0, true);
    if (err<0) {
        return err;
    }
    
    if (os_memeq(__benv_block(0, idx), __benv_mirror_block(idx), BENV_BLOCK_SIZE)) {
        debug_trace("benv0 block%d not changed, needn't save", idx);
        
        return 0;
    }

#if BENV_COUNT > 1
    /*
    * 0==>others
    */
    for (env=1; env<BENV_COUNT; env++) {
        os_memcpy(__benv_block(env, idx), __benv_block(0, idx), BENV_BLOCK_SIZE);
    }
#endif

    return __benv_save(idx);
}

static inline int
benv_save_os(void)
{
    return __benv_saveby(BENV_OS);
}

static inline int
benv_save_mark(void)
{
    return __benv_saveby(BENV_MARK);
}

static inline int
benv_save_info(void)
{
    int idx, err;

    for (idx=BENV_INFO; idx<BENV_BLOCK_COUNT; idx++) {
        err = __benv_saveby(idx);
        if (err<0) {
            return err;
        }
    }

    return 0;
}

static inline int
benv_save(void)
{
    int idx, env, err = 0;

    /*
    * load mirror
    */
    err = __benv_read(0, true);
    if (err<0) {
        return err;
    }

#if BENV_COUNT > 1
    /*
    * 0==>others
    */
    for (env=1; env<BENV_COUNT; env++) {
        os_memcpy(__benv_env(env), __benv_env(0), BENV_SIZE);
    }
#endif

    /*
    * skip block 0(cookie)
    */
    for (idx=1; idx<BENV_BLOCK_COUNT; idx++) {
        if (os_memeq(__benv_block(0, idx), __benv_mirror_block(idx), BENV_BLOCK_SIZE)) {
            debug_trace("benv0 block%d not changed, needn't save", idx);
            
            continue;
        }

        err = __benv_save(idx);
        if (err<0) {
            return err;
        }
    }
    
    return 0;
}
#else
static inline int
benv_save(void)
{
    int env, err = 0;

    /*
    * load mirror
    */
    err = __benv_read(0, true);
    if (err<0) {
        return err;
    }
    
    if (os_memeq(__benv_env(0), __benv_mirror, BENV_SIZE)) {
        debug_trace("benv0 not changed, needn't save");
        
        return 0;
    }

#if BENV_COUNT > 1
    /*
    * 0==>others
    */
    for (env=1; env<BENV_COUNT; env++) {
        os_memcpy(__benv_env(env), __benv_env(0), BENV_SIZE);
    }
#endif

    return __benv_save(BENV_ALL);
}
#endif

static inline int
benv_init(void)
{
    int err;

    BUILD_BUG_ON(BENV_COOKIE_SIZE     > BENV_BLOCK_SIZE);
    BUILD_BUG_ON(BENV_OS_SIZE         > BENV_BLOCK_SIZE);
    BUILD_BUG_ON((uint32)__benv_info_idx_max  >= (uint32)BENV_INFO_COUNT);

    err = os_init();
    if (err<0) {
        return err;
    }
    
    err = benv_open();
        debug_trace_error(err, "open benv");
    if (err<0) {
        return err;
    }

    err = benv_load();
        debug_trace_error(err, "load benv");
    if (err<0) {
        return err;
    }

    return 0;
}

static inline int
benv_fini(void)
{
    int err;
    
    err = benv_save();
        debug_trace_error(err, "save benv");
        
    err = benv_close();
        debug_trace_error(err, "close benv");

    return err;
}

static inline void
benv_check_os(void)
{
    int i;

    __benv_errno = 0;
    os_firmware_foreach_all(i) {
        if (false==is_benv_good_rootfs(i)) {
            __benv_errno = -1;
            os_println("bad rootfs%d", i);
        }
        
        if (false==is_benv_good_kernel(i)) {
            __benv_errno = -1;
            os_println("bad kernel%d", i);
        }
    }
}

#define BENV_DEFT_COOKIE              { \
    .vendor     = PRODUCT_Vendor,       \
    .company    = PRODUCT_COMPANY,      \
    .model      = PRODUCT_PCBA_MODEL,   \
    .version    = PRODUCT_DEFT_VERSION_STRING, \
    .compile    = PRODUCT_BUILD_TIME,   \
}   /* end */

#define BENV_DEFT_OS  {   \
    .current    = PRODUCT_FIRMWARE_CURRENT, \
    .firmware   = {     \
        [0 ... (PRODUCT_FIRMWARE_COUNT-1)] = BENV_DEFT_FIRMWARE, \
    },                  \
}   /* end */

#define BENV_DEFT_MARK  {.var = {0}}
#define BENV_DEFT_INFO  {                                           \
    .var = {                                                        \
        [__benv_info_pcba_model]        = PRODUCT_PCBA_MODEL,       \
        [__benv_info_pcba_mac]          = __empty,                  \
        [__benv_info_pcba_sn]           = __empty,                  \
        [__benv_info_pcba_version]      = PRODUCT_PCBA_VERSION,     \
        [__benv_info_pcba_rootrw]       = PRODUCT_ROOTFS_RO,        \
                                                                    \
        [__benv_info_product_vendor]    = PRODUCT_Vendor,           \
        [__benv_info_product_company]   = PRODUCT_COMPANY,          \
        [__benv_info_product_model]     = PRODUCT_MODEL,            \
        [__benv_info_product_mac]       = __empty,                  \
        [__benv_info_product_sn]        = __empty,                  \
        [__benv_info_product_manager]   = PRODUCT_VENDOR_MANAGER,   \
        [__benv_info_product_version]   = PRODUCT_VERSION,          \
                                                                    \
        [__benv_info_oem_vendor]        = PRODUCT_Vendor,           \
        [__benv_info_oem_company]       = PRODUCT_COMPANY,          \
        [__benv_info_oem_model]         = PRODUCT_MODEL,            \
        [__benv_info_oem_mac]           = __empty,                  \
        [__benv_info_oem_sn]            = __empty,                  \
        [__benv_info_oem_manager]       = PRODUCT_VENDOR_MANAGER,   \
        [__benv_info_oem_version]       = PRODUCT_VERSION,          \
    }                                                               \
}   /* end */

#define BENF_DEFT           {   \
    .cookie = BENV_DEFT_COOKIE, \
    .os     = BENV_DEFT_OS,     \
    .mark   = BENV_DEFT_MARK,   \
    .info   = BENV_DEFT_INFO,   \
}   /* end */

static inline int
benv_repair(void)
{
#if BENV_REPAIR
    benv_cookie_t deft = BENV_DEFT_COOKIE;
    int err = 0, env, idx;
    bool upgrade = false;
    
    /*
    * 1-count-benv upgrade to 4-count-benv
    *   benv-1/2/3 cookie != default cookie(fixed)
    *   so, it is upgrade
    */
    for (env=1; env<BENV_COUNT; env++) {
        if (false==os_memeq(&deft, __benv_cookie(env), BENV_COOKIE_FIXED)) {
            upgrade = true;

            break;
        }
    }

    if (upgrade) {
        __benv_upgrade();
    } else {
        __benv_repair();
    }
#endif

    return 0;
}

static inline bool
is_benv_cookie_fixed(void)
{
    benv_cookie_t deft = BENV_DEFT_COOKIE;
    /*
    * just cmp FIXED
    */
    return os_memeq(&deft, __benv_cookie(0), BENV_COOKIE_FIXED);
}

static inline bool
is_benv_cookie_deft(void)
{
    benv_cookie_t deft = BENV_DEFT_COOKIE;
    
    return os_objeq(&deft, __benv_cookie(0));
}

static inline void
__benv_deft_cookie(void)
{
    benv_cookie_t deft = BENV_DEFT_COOKIE;
    
    os_println("benv cookie restore...");
    os_objcpy(__benv_cookie(0), &deft);
    os_println("benv cookie restore ok");
}

static inline void
__benv_deft_os(void)
{
    int i;

    os_println("benv os restore ...");
    for (i=0; i<PRODUCT_FIRMWARE_COUNT; i++) {
        __benv_firmware_deft(benv_firmware(i));
    }
    __benv_current = PRODUCT_FIRMWARE_CURRENT;
    os_println("benv os restore ok.");
}

static inline void
__benv_deft_info(void)
{
    benv_info_t info = BENV_DEFT_INFO;
    int ro[] = BENV_INFO_RO_LIST;
    int i, idx;

    os_println("benv info restore ...");
    for (i=0; i<os_count_of(ro); i++) {
        idx = ro[i];

        os_strcpy(benv_info(idx), info.var[idx]);
    }
    os_println("benv info restore ok.");
}

static inline void
__benv_deft(void)
{
    os_println("benv restore ...");
    os_objdeft(__benv_env(0), BENF_DEFT);
    os_println("benv restore ok");
}

static inline void
__benv_clean_cookie(void)
{
    os_println("benv cookie clean ...");
    os_objzero(__benv_cookie(0));
    os_println("benv cookie clean ok.");
}

static inline void
__benv_mark_private_save(uint32 backup[])
{
    int i;
    
    for (i=__benv_mark_protect_begin; i<__benv_mark_protect_end; i++) {
        backup[i] = benv_mark(i);
    }
}

static inline void
__benv_mark_private_restore(uint32 backup[])
{
    int i;
    
    for (i=__benv_mark_protect_begin; i<__benv_mark_protect_end; i++) {
        benv_mark(i) = backup[i];
    }
}

static inline void
__benv_clean_mark(void)
{
    uint32 backup[__benv_mark_end] = {0};

    os_println("benv marks clean ...");
    __benv_mark_private_save(backup);
    os_objzero(__benv_mark(0));
    __benv_mark_private_restore(backup);

    os_println("benv marks clean ok");
}

static inline void
__benv_clean_info(void)
{
    os_println("benv infos clean ...");
    
    os_objzero(__benv_info(0));
    __benv_deft_info();
    
    os_println("benv infos clean ok.");
}

static inline void
__benv_clean(void)
{
    uint32 backup[__benv_mark_end] = {0};

    os_println("benv clean ...");
    
    __benv_mark_private_save(backup);
    os_objzero(__benv_env(0));
    __benv_mark_private_restore(backup);
    
    os_println("benv clean ok, need reboot");
}

static inline void 
benv_check(void) 
{
#if defined(__BOOT__) || defined(__PC__)
    if (false==is_benv_cookie_deft()) {
        if (is_benv_cookie_fixed()) {
            /*
            * boot cookie(fixed) == benv cookie(fixed)
            *   so, boot maybe have upgraded
            *       benv cookie need recover
            */
            os_println("benv cookie update...");
        } else {
            /*
            * boot cookie(fixed) != benv cookie(fixed)
            *   so, benv maybe destroy
            *       benv cookie/info need recover
            */
            os_println("benv cookie/info recover...");
            
            __benv_deft_info();
        }
        __benv_deft_cookie();

        benv_show_cookie();
    }
#endif
}

static inline int
benv_cmd_find(int argc, char *argv[])
{
    char *obj = argv[1];
    char *ext = argv[2];

    benv_version_t version;
    int skips = benv_skips(0);
    int idx[3] = {-1, -1, -1};
    int by = -1;
    
    if (os_streq("best", obj)) {
        idx[0] = benv_find_best(rootfs, skips);
        idx[1] = benv_find_best(kernel, skips);
        idx[2] = benv_find_best(firmware, skips);

        os_println("find best rootfs:%d", idx[0]);
        os_println("find best kernel:%d", idx[1]);
        os_println("find best firmware:%d", idx[2]);
    }
    else if (os_streq("worst", obj)) {
        idx[0] = benv_find_worst(rootfs, skips);
        idx[1] = benv_find_worst(kernel, skips);
        idx[2] = benv_find_worst(firmware, skips);

        os_println("find worst rootfs:%d", idx[0]);
        os_println("find worst kernel:%d", idx[1]);
        os_println("find worst firmware:%d", idx[2]);
    }
    else if (os_streq("first.buddy", obj)) {
        by = os_atoi(ext);
        if (0==by) {
            os_println(__THIS_APPNAME " find first.buddy {idx}");

            return -EFORMAT;
        }
        
        idx[0] = benv_find_first_buddy(rootfs, by, skips);
        idx[1] = benv_find_first_buddy(kernel, by, skips);
        idx[2] = benv_find_first_buddy(firmware, by, skips);

        os_println("find rootfs[%d]'s first buddy:%d", by, idx[0]);
        os_println("find kernel[%d]'s first buddy:%d", by, idx[1]);
        os_println("find firmware[%d]'s first buddy:%d", by, idx[2]);
    }
    else if (os_streq("first.good.buddy", obj)) {
        by = os_atoi(ext);
        if (0==by) {
            os_println(__THIS_APPNAME " find first.good.buddy {idx}");

            return -EFORMAT;
        }
        
        idx[0] = benv_find_first_good_buddy(rootfs, by, skips);
        idx[1] = benv_find_first_good_buddy(kernel, by, skips);
        idx[2] = benv_find_first_good_buddy(firmware, by, skips);

        os_println("find rootfs[%d]'s first good buddy:%d", by, idx[0]);
        os_println("find kernel[%d]'s first good buddy:%d", by, idx[1]);
        os_println("find firmware[%d]'s first good buddy:%d", by, idx[2]);
    }
    else if (os_streq("first.bad.buddy", obj)) {
        by = os_atoi(ext);
        if (0==by) {
            os_println(__THIS_APPNAME " find first.bad.buddy {idx}");

            return -EFORMAT;
        }
        
        idx[0] = benv_find_first_bad_buddy(rootfs, by, skips);
        idx[1] = benv_find_first_bad_buddy(kernel, by, skips);
        idx[2] = benv_find_first_bad_buddy(firmware, by, skips);
        
        os_println("find rootfs[%d]'s first bad buddy:%d", by, idx[0]);
        os_println("find kernel[%d]'s first bad buddy:%d", by, idx[1]);
        os_println("find firmware[%d]'s first bad buddy:%d", by, idx[2]);
    }
    else if (os_streq("first.version", obj)) {
        if (NULL==benv_version_atoi(&version, ext)) {
            os_println(__THIS_APPNAME " find first.version {version}");

            return -EFORMAT;
        }
        
        idx[0] = benv_find_first_byversion(rootfs, &version, skips);
        idx[1] = benv_find_first_byversion(kernel, &version, skips);
        idx[2] = benv_find_first_byversion(firmware, &version, skips);
        
        os_println("find first rootfs(%s):%d", ext, idx[0]);
        os_println("find first kernel(%s):%d", ext, idx[1]);
        os_println("find first firmware(%s):%d", ext, idx[2]);
    }
    else if (os_streq("first.good.version", obj)) {
        if (NULL==benv_version_atoi(&version, ext)) {
            os_println(__THIS_APPNAME " find first.good.version {version}");

            return -EFORMAT;
        }
        
        idx[0] = benv_find_first_good_byversion(rootfs, &version, skips);
        idx[1] = benv_find_first_good_byversion(kernel, &version, skips);
        idx[2] = benv_find_first_good_byversion(firmware, &version, skips);
        
        os_println("find first good rootfs(%s):%d", ext, idx[0]);
        os_println("find first good kernel(%s):%d", ext, idx[1]);
        os_println("find first good firmware(%s):%d", ext, idx[2]);
    }
    else if (os_streq("first.bad.version", obj)) {
        if (NULL==benv_version_atoi(&version, ext)) {
            os_println(__THIS_APPNAME " find first.bad.version {version}");

            return -EFORMAT;
        }
        
        idx[0] = benv_find_first_bad_byversion(rootfs, &version, skips);
        idx[1] = benv_find_first_bad_byversion(kernel, &version, skips);
        idx[2] = benv_find_first_bad_byversion(firmware, &version, skips);
        
        os_println("find first bad rootfs(%s):%d", ext, idx[0]);
        os_println("find first bad kernel(%s):%d", ext, idx[1]);
        os_println("find first bad firmware(%s):%d", ext, idx[2]);
    }
    else {
        os_println(__THIS_APPNAME " find best");
        os_println(__THIS_APPNAME " find worst");
        os_println(__THIS_APPNAME " find first.buddy {idx}");
        os_println(__THIS_APPNAME " find first.good.buddy {idx}");
        os_println(__THIS_APPNAME " find first.bad.buddy {idx}");
        os_println(__THIS_APPNAME " find first.version {version}");
        os_println(__THIS_APPNAME " find first.good.version {version}");
        os_println(__THIS_APPNAME " find first.bad.version {version}");

        return -EFORMAT;
    }
    
    return 0;
}

typedef struct {
#if BENV_SBBUG
    uint32 __r0[2];
#endif
    char *action;
    char *obj;
    void (*handle)(void);
#if BENV_SBBUG
    uint32 __r1[2];
#endif
} benv_cmd_t;

#define BENV_CMD(_action, _obj, _handle) {.action = _action, .obj = _obj, .handle = _handle}
#define BENV_CMD_INITER     { \
    BENV_CMD("show",    "cookie",   benv_show_cookie),  \
    BENV_CMD("show",    "os",       benv_show_os),      \
    BENV_CMD("show",    "path",     benv_show_path),    \
    BENV_CMD("show",    "hide",     benv_show_hide),    \
    BENV_CMD("show",    "all",      benv_show_all),     \
                                                        \
    BENV_CMD("check",   "os",       benv_check_os),     \
                                                        \
    BENV_CMD("reset",   "os",       __benv_deft_os),    \
    BENV_CMD("reset",   "info",     __benv_deft_info),  \
    BENV_CMD("reset",   "all",      __benv_deft),       \
                                                        \
    BENV_CMD("clean",   "cookie",   __benv_clean_cookie), \
    BENV_CMD("clean",   "mark",     __benv_clean_mark), \
    BENV_CMD("clean",   "info",     __benv_clean_info), \
    BENV_CMD("clean",   "all",      __benv_clean),      \
}   /* end */

static inline void
benv_cmd_hiden_selfcheck(benv_cmd_t *cmd, int count)
{
#if BENV_SBBUG
    int i;
    
    for (i=0; i<count; i++) {
        if ((uintptr)cmd[i].handle > (uintptr)BENV_LINK_ADDRESS) {
            os_println("benv cmd[%d] handle is %p", i, cmd[i].handle);
        }
    }
#endif
}

static inline bool
benv_cmd_hiden(int argc, char *argv[])
{
    benv_cmd_t cmd[] = BENV_CMD_INITER;
    char *action    = argv[1];
    char *obj       = argv[2];
    int i;

    benv_cmd_hiden_selfcheck(cmd, os_count_of(cmd));
    
    if (os_streq("find", action)) {
        benv_cmd_find(argc-1, argv+1);

        return true;
    }
    else if (3==argc) {
        for (i=0; i<os_count_of(cmd); i++) {
            if (os_streq(action, cmd[i].action) && os_streq(obj, cmd[i].obj)) {
                (*cmd[i].handle)();
                
                return true;
            }
        }
    }
    
    return false;
}

static inline int
benv_cmd(int argc, char *argv[])
{
    __benv_self = cli_argv_dump(argc, argv);

    if (benv_cmd_hiden(argc, argv)) {
        return __benv_errno;
    } else {
        return benv_command(argc - 1, argv + 1);
    }
}

static inline int
__benv_main(int argc, char *argv[])
{
    int err = os_call(benv_init, benv_fini, benv_cmd, argc, argv);

    return shell_error(err);
}

static inline void *
benv_get(char *path)
{
    int i;

    for (i = 0; i < __benv_ops_count; i++) {
        benv_ops_t *ops = benv_ops(i);

        if (os_streq(path, ops->path)) {
            return benv_ops_obj(ops);
        }
    }

    return NULL;
}
/******************************************************************************/
#endif /* __BENV_H_6a2223c1e9a84ae38f5da34125b3974b__ */
