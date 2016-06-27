#ifndef __BENV_H_6a2223c1e9a84ae38f5da34125b3974b__
#define __BENV_H_6a2223c1e9a84ae38f5da34125b3974b__
#include "utils.h"
/******************************************************************************/
#define BENV_DEBUG_SORT             0x01
#define BENV_DEBUG_CMP              0x02

#ifndef BENV_DEBUG
#define BENV_DEBUG                  0 //(BENV_DEBUG_SORT | BENV_DEBUG_CMP)
#endif

#ifndef BENV_UPGRADE_COUNT
#define BENV_UPGRADE_COUNT          1
#endif

#ifndef BENV_DEV_BLOCK_SIZE
#define BENV_DEV_BLOCK_SIZE         512
#endif

#ifndef BENV_SIZE
#define BENV_SIZE                   4096
#endif

#ifndef BENV_BOOT_SIZE
#define BENV_BOOT_SIZE              (512*1024)
#endif

enum {
    BENV_START = BENV_BOOT_SIZE,
};

#define BENV_BLOCK_SIZE     BENV_DEV_BLOCK_SIZE
#define BENV_BLOCK_COUNT    (BENV_SIZE/BENV_BLOCK_SIZE) /* 8 */

#ifndef BENV_TRYS
#define BENV_TRYS                   3
#endif

#ifndef BENV_VENDOR
#define BENV_VENDOR                 "superwalle"
#endif

#ifndef BENV_COMPANY
#define BENV_COMPANY                "superwalle Technology Co.,Ltd."
#endif

#ifndef BENV_PRODUCT_MODEL
#define BENV_PRODUCT_MODEL          "superwalle-LV1"
#endif

#ifndef BENV_PCBA_MODEL
#define BENV_PCBA_MODEL             "superwalle-pcba"
#endif

#ifndef BENV_INFO_SIZE
#define BENV_INFO_SIZE              64
#endif

#ifndef benv_kernel_size
#define benv_kernel_size            0x8000  /* block count */
#endif

#ifndef benv_rootfs_size
#define benv_rootfs_size            0x80000 /* block count */
#endif

#define benv_obj_size(_obj)         benv_##_obj##_size

#define is_good_benv_idx(_idx)      is_good_enum(_idx, PRODUCT_FIRMWARE_COUNT)
#define is_normal_benv_idx(_idx)    is_good_value(_idx, 1, PRODUCT_FIRMWARE_COUNT)

#define os_firmware_foreach_all(_i) os_foreach(_i, PRODUCT_FIRMWARE_COUNT)
#define os_firmware_foreach(_i)     __os_foreach(_i, 1, PRODUCT_FIRMWARE_COUNT) /* skip 0 */

/*
* ok > unknow > fail
*/
#define __XLIST_BENV_FSM(_)             \
    _(BENV_FSM_FAIL,    0, "fail"),     \
    _(BENV_FSM_UNKNOW,  1, "unknow"),   \
    _(BENV_FSM_OK,      2, "ok"),       \
    /* end */

static inline bool is_good_benv_fsm(int id);
static inline char *benv_fsm_string(int id);
static inline int benv_fsm_idx(char *name);
DECLARE_ENUM(benv_fsm, __XLIST_BENV_FSM, BENV_FSM_END);

#if 1 /* just for sourceinsight */
#define BENV_FSM_FAIL   BENV_FSM_FAIL
#define BENV_FSM_UNKNOW BENV_FSM_UNKNOW
#define BENV_FSM_OK     BENV_FSM_OK
#define BENV_FSM_END    BENV_FSM_END
#endif /* just for sourceinsight */

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

#define BENV_COOKIE_SIZE      (0  \
    + sizeof(BENV_VENDOR)         \
    + sizeof(BENV_COMPANY)        \
    + sizeof(BENV_PCBA_MODEL)     \
)   /* end */

typedef struct {
    struct {
        char vendor[sizeof(BENV_VENDOR)];
        char company[sizeof(BENV_COMPANY)];
    } product;

    struct {
        char model[sizeof(BENV_PCBA_MODEL)];
    } pcba;

    char pad[BENV_BLOCK_SIZE - BENV_COOKIE_SIZE];
} benv_cookie_t; /* 512 */

static inline void
__benv_cookie_show(benv_cookie_t *cookie)
{
    os_println("vendor :%s", cookie->product.vendor);
    os_println("company:%s", cookie->product.company);
    os_println("model  :%s", cookie->pcba.model);
}

#define BENV_DEFT_COOKIE              { \
    .product        = {                 \
        .vendor     = BENV_VENDOR,      \
        .company    = BENV_COMPANY,     \
    },                                  \
    .pcba           = {                 \
        .model      = BENV_PCBA_MODEL,  \
    },                                  \
}   /* end */

typedef struct {
    byte number[4];
} benv_version_t;

enum {
    BENV_MIN_VERSION_NUMBER = 0,
    BENV_MAX_VERSION_NUMBER = 255,
};

#define BENV_MIN_VERSION_STRING   "0.0.0.0"
#define BENV_MAX_VERSION_STRING   "255.255.255.255"
#define BENV_MIN_VERSION          { \
    .number = {                     \
        BENV_MIN_VERSION_NUMBER,    \
        BENV_MIN_VERSION_NUMBER,    \
        BENV_MIN_VERSION_NUMBER,    \
        BENV_MIN_VERSION_NUMBER,    \
    },                              \
}   /* end */

#define BENV_MAX_VERSION          { \
    .number = {                     \
        BENV_MAX_VERSION_NUMBER,    \
        BENV_MAX_VERSION_NUMBER,    \
        BENV_MAX_VERSION_NUMBER,    \
        BENV_MAX_VERSION_NUMBER,    \
    },                              \
}   /* end */

#define BENV_INVALID_VERSION    BENV_MIN_VERSION
#define BENV_DEFT_VERSION       BENV_MIN_VERSION

enum {
    BENV_VERSION_STRING_LEN = sizeof(BENV_MAX_VERSION_STRING) - 1,  /* 15 */
};

static inline char *
__benv_version_itoa(benv_version_t *version, char string[])
{
    os_sprintf(string, "%d.%d.%d.%d",
           version->number[0],
           version->number[1], 
           version->number[2], 
           version->number[3]);

    return string;
}

static inline int
__benv_version_atoi(benv_version_t *version, char *string)
{
    char line[1 + OS_LINE_LEN] = { 0 };
    char *number[4] = { line, NULL, NULL, NULL };
    int i;

    if (NULL==string) {
        return -EFORMAT;
    }
    
    os_strdcpy(line, string);

    /* begin 1 */
    for (i = 1; i < 4; i++) {
        number[i] = os_strchr(number[i - 1], '.');
        if (NULL == number[i]) {
            debug_error("bad version:%s", string);

            return -EFORMAT;
        }

        *(number[i])++ = 0;
    }

    for (i = 0; i < 4; i++) {
        char *end = NULL;

        version->number[i] = os_strtoul(number[i], &end, 0);
        if (false==os_strton_is_good_end(end)) {
            debug_error("bad-version:%s", string);

            return -EFORMAT;
        }
    }

    return 0;
}

static inline char *
benv_version_itoa(benv_version_t *version)
{
    static char string[1+BENV_VERSION_STRING_LEN];

    return __benv_version_itoa(version, string);
}

static inline benv_version_t *
benv_version_atoi(char *string)
{
    static benv_version_t deft = BENV_DEFT_VERSION;
    static benv_version_t version = BENV_DEFT_VERSION;

    return 0==__benv_version_atoi(&version, string)?&version:&deft;
}

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

static inline void
__benv_vcs_deft(benv_vcs_t *vcs)
{
    os_objzero(vcs);
    
    vcs->upgrade    = BENV_FSM_OK;
    vcs->other      = BENV_FSM_UNKNOW;
    vcs->self       = BENV_FSM_UNKNOW;
}

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

static inline bool
benv_vcs_is_good(benv_vcs_t *vcs)
{
    return is_benv_good(vcs->error)           /* no error */
        && BENV_FSM_OK == vcs->upgrade        /* upgrade ok */
        && is_canused_benv_fsm(vcs->self);    /* self startup ok, or have not startup */
}

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

#define BENV_DEFT_OS  {   \
    .current    = PRODUCT_FIRMWARE_CURRENT, \
    .firmware   = {     \
        [0 ... (PRODUCT_FIRMWARE_COUNT-1)] = BENV_DEFT_FIRMWARE, \
    },                  \
}   /* end */

static inline void
__benv_os_show(benv_os_t *os)
{
    int i;
    
#if 0
        index   error   upgrade self    other   version         cookie
kernel  0       0       unknow  unknow  unknow  255.255.255.255 0123456789abcdef
rootfs  0       0       ok      ok      unknow
kernel* 1       0       fail    fail    unknow
rootfs* 1       0       fail    unknow  unknow
#endif

#define benv_os_show_obj(_obj, _idx) do{ \
    os_println("%s%c %-7d %-7d %-7s %-7s %-7s %-15s %s",    \
        #_obj, _idx==os->current?'*':' ',                   \
        _idx, os->firmware[_idx]._obj.error,                \
        benv_fsm_string(os->firmware[_idx]._obj.upgrade),   \
        benv_fsm_string(os->firmware[_idx]._obj.self),      \
        benv_fsm_string(os->firmware[_idx]._obj.other),     \
        benv_version_itoa(&os->firmware[_idx]._obj.version), \
        os->firmware[_idx]._obj.cookie);                    \
    }while(0)
    
    os_println("        index   error   upgrade self    other   version         cookie");
    os_println("======================================================================");
    for (i=0; i<PRODUCT_FIRMWARE_COUNT; i++) {
        benv_os_show_obj(kernel, i);
        benv_os_show_obj(rootfs, i);
        
        if (i<(PRODUCT_FIRMWARE_COUNT-1)) {
            os_println
              ("----------------------------------------------------------------------");
        }
    }
    os_println("======================================================================");
#undef benv_os_show_obj
}

enum {
    BENV_MARK_COUNT = (BENV_BLOCK_SIZE/sizeof(uint32)),   /* 128 */
};

typedef struct {
    uint32 key[BENV_MARK_COUNT];
} benv_mark_t; /* 512 */

#define BENV_DEFT_MARK { .key = {0} }

enum {
    BENV_INFO_COUNT_PER_BLOCK   = (BENV_BLOCK_SIZE/BENV_INFO_SIZE),                 /* 512/64=8 */
    BENV_INFO_COUNT             = ((BENV_BLOCK_COUNT-3)*BENV_INFO_COUNT_PER_BLOCK), /* 5*8=40 */
};

typedef struct {
    char var[BENV_INFO_COUNT][BENV_INFO_SIZE];
} benv_info_t;

#define BENV_DEFT_INFO { .var = {{0}} }

enum {
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

static inline int
__benv_ops_is(uint32 offset)
{
    if (offset >= sizeof(benv_env_t)) {
        return -ENOEXIST;
    }
    else if (offset >= offsetof(benv_env_t, info)) {
        return BENV_INFO;
    }
    else if (offset >= offsetof(benv_env_t, mark)) {
        return BENV_MARK;
    }
    else if (offset >= offsetof(benv_env_t, os)) {
        return BENV_OS;
    }
    else {
        return BENV_COOKIE;
    }
}

#define BENF_DEFT           {   \
    .cookie = BENV_DEFT_COOKIE, \
    .os     = BENV_DEFT_OS,     \
    .mark   = BENV_DEFT_MARK,   \
    .info   = BENV_DEFT_INFO,   \
}   /* end */

typedef struct struct_benv_ops benv_ops_t;
struct struct_benv_ops {
    char *path;
    uint32 offset;
    uint32 flag;

    int  (*check)(benv_ops_t* ops, char *value);
    void (*write)(benv_ops_t* ops, char *value);
    void (*show) (benv_ops_t* ops);
};

typedef struct {
    char *value;        /* for write */
    bool showit;        /* for show */
} benv_cache_t;

/* 
* path without last '*' 
*/
static inline bool
benv_ops_match_wildcard(benv_ops_t * ops, char *path, int len)
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
    benv_env_t *env;
    benv_ops_t *ops;
    benv_cache_t *cache;
    int ops_count;
    
    int show_count;
    bool dirty[BENV_BLOCK_COUNT];
    char *self;
    int fd;
} benv_control_t;

#define BENV_CONTROL_DEFT(_env, _ops, _cache) {   \
    .env        = _env,                         \
    .ops        = _ops,                         \
    .cache      = _cache,                       \
    .ops_count  = (_ops)?os_count_of(_ops):0,   \
    .fd         = INVALID_FD,                   \
}   /* end */


#if !defined(__BUSYBOX__) && (IS_PRODUCT_PC || IS_PRODUCT_LTEFI_B)
#   define BENV_INITER \
        static benv_env_t __benv; \
        benv_control_t benv_control = BENV_CONTROL_DEFT(&__benv, NULL, NULL); \
        os_fake_declare /* end */
#else
#   define BENV_INITER    os_fake_declare
#endif

extern benv_control_t benv_control;
#define __benv_control    (&benv_control)

#define __benv_fd         __benv_control->fd
#define __benv_env        __benv_control->env
#define __benv_ops        __benv_control->ops
#define __benv_ops_count  __benv_control->ops_count
#define __benv_show_count __benv_control->show_count
#define __benv_self       __benv_control->self
#define __benv_dirty      __benv_control->dirty
#define __benv_cache      __benv_control->cache
#define __benv_cookie     (&__benv_env->cookie)
#define __benv_os         (&__benv_env->os)
#define __benv_current    __benv_os->current
#define __benv_firmware   __benv_os->firmware
#define __benv_mark       (&__benv_env->mark)
#define __benv_info       (&__benv_env->info)

#define benv_mark(_idx)   __benv_mark->key[_idx]
#define benv_info(_idx)   __benv_info->var[_idx]

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
    return (char *)__benv_env + ops->offset;
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

static inline void
benv_dirty_byidx(int idx)
{
    __benv_dirty[idx] = true;
    
    debug_trace("set block[%d] dirty", idx);
}

static inline void
benv_dirty_byops(benv_ops_t *ops)
{
    int offset = benv_ops_string(ops) - (char *)__benv_env;
    int idx = offset / BENV_BLOCK_SIZE;

    benv_dirty_byidx(idx);
}

static inline void
benv_dirty_byzone(int begin, int end)
{
    int i;

    for (i=begin; i<end; i++) {
        benv_dirty_byidx(i);
    }
}
#define benv_dirty_all()    benv_dirty_byzone(0, BENV_BLOCK_COUNT)

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

        benv_dirty_byops(ops);
    }
}

static inline benv_cache_t *
benv_cache(benv_ops_t * ops)
{
    return &__benv_cache[benv_ops_idx(ops)];
}

#define benv_cache_value(_ops)      benv_cache(_ops)->value
#define benv_cache_showit(_ops)     benv_cache(_ops)->showit

static inline void
__benv_ops_dump(benv_ops_t *ops)
{
    debug_trace("ops idx=%d, path=%s, value=%s, showit=%s",
        benv_ops_idx(ops),
        ops->path,
        benv_cache_value(ops)?benv_cache_value(ops):"nothing",
        benv_cache_showit(ops)?"true":"false");
}

static inline void
benv_ops_dump(char *name)
{
    int i;

    debug_trace("[%s] ops count=%d", name, __benv_ops_count);
    
    for (i = 0; i < __benv_ops_count; i++) {
        __benv_ops_dump(benv_ops(i));
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
__benv_kernel_is_good(int idx)
{
    return is_good_benv_idx(idx)
        && benv_vcs_is_good(benv_kernel(idx));
}

static inline bool
__benv_rootfs_is_good(int idx)
{
    return is_good_benv_idx(idx)
        && benv_vcs_is_good(benv_rootfs(idx));
}

static inline bool
__benv_firmware_is_good(int idx)
{
    return __benv_rootfs_is_good(idx)
        && __benv_kernel_is_good(idx);
}
#define benv_obj_is_good(_obj, _idx)    __benv_##_obj##_is_good(_idx)

#define __benv_kernel_is_bad(_idx)      (false==__benv_kernel_is_good(_idx))
#define __benv_rootfs_is_bad(_idx)      (false==__benv_rootfs_is_good(_idx))
#define __benv_firmware_is_bad(_idx)    (false==__benv_firmware_is_good(_idx))
#define benv_obj_is_bad(_obj, _idx)     (false==benv_obj_is_good(_obj, _idx))

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

static inline char *
benv_kernel_version_string(int idx)
{
    return benv_version_itoa(benv_kernel_version(idx));
}

static inline char *
benv_rootfs_version_string(int idx)
{
    return benv_version_itoa(benv_rootfs_version(idx));
}
#define benv_firmware_version_string(_idx)      benv_rootfs_version_string(_idx)
#define benv_obj_version_string(_obj, _idx)     benv_##_obj##_version_string(_idx)

static inline int
__benv_kernel_version_cmp(int a, int b)
{
    return benv_version_cmp(benv_kernel_version(a), benv_kernel_version(b));
}

static inline int
__benv_rootfs_version_cmp(int a, int b)
{
    return benv_version_cmp(benv_rootfs_version(a), benv_rootfs_version(b));
}

static inline int
__benv_firmware_version_cmp(int a, int b)
{
    int ret;

    ret = __benv_rootfs_version_cmp(a, b);
    if (ret) {
        return ret;
    }

    ret = __benv_kernel_version_cmp(a, b);
    if (ret) {
        return ret;
    }

    return 0;
}

#define benv_obj_version_cmp(_obj, _a, _b)  __benv_##_obj##_version_cmp(_a, _b)
#define benv_obj_version_eq(_obj, _a, _b)   (0==benv_obj_version_cmp(_obj, _a, _b))

static inline bool
is_benv_cookie_deft(void)
{
    benv_cookie_t deft = BENV_DEFT_COOKIE;
    
    return os_objeq(&deft, __benv_cookie);
}

static inline void
__benv_deft_cookie(void)
{
    os_objdeft(__benv_cookie, BENV_DEFT_COOKIE);

    benv_dirty_byidx(BENV_COOKIE);
}

static inline void
__benv_deft_os(void)
{
    int i;

    for (i=0; i<PRODUCT_FIRMWARE_COUNT; i++) {
        __benv_firmware_deft(benv_firmware(i));
    }
    __benv_current = PRODUCT_FIRMWARE_CURRENT;

    benv_dirty_byidx(BENV_OS);
}

static inline void
__benv_deft(void)
{
    os_objdeft(__benv_env, BENF_DEFT);

    benv_dirty_all();
}

static inline void
__benv_clean_cookie(void)
{
    os_objzero(__benv_cookie);
    
    benv_dirty_byidx(BENV_COOKIE);
    
    os_println("cookie clean");
}

enum {
    __benv_mark_ptest_control   = 0,
    __benv_mark_ptest_result    = 1,

    __benv_mark_private         = 2,
    __benv_mark_debug           = __benv_mark_private,
    __benv_mark_uptimes         = 3,
    __benv_mark_runtime         = 4,
    __benv_mark_cid_mid         = 5,
    __benv_mark_cid_psn         = 6,
    __benv_mark_noauth          = 7,
    __benv_mark_jsdebug         = 8,

    __benv_mark_end
};

static inline void
__benv_mark_private_save(uint32 backup[])
{
    int i;
    
    for (i=__benv_mark_private; i<__benv_mark_end; i++) {
        backup[i] = benv_mark(i);
    }
}

static inline void
__benv_mark_private_restore(uint32 backup[])
{
    int i;
    
    for (i=__benv_mark_private; i<__benv_mark_end; i++) {
        benv_mark(i) = backup[i];
    }
}


static inline void
__benv_clean_mark(void)
{
    uint32 backup[__benv_mark_end] = {0};
    
    __benv_mark_private_save(backup);
    os_objzero(__benv_mark);
    __benv_mark_private_restore(backup);
    
    benv_dirty_byidx(BENV_MARK);
    
    os_println("mark clean");
}

static inline void
__benv_clean_info(void)
{
    os_objzero(__benv_info);
    
    benv_dirty_byzone(BENV_INFO, BENV_BLOCK_COUNT);
    
    os_println("info clean");
}

static inline void
__benv_clean(void)
{
    uint32 backup[__benv_mark_end] = {0};
    
    __benv_mark_private_save(backup);
    os_objzero(__benv_env);
    __benv_mark_private_restore(backup);
    
    benv_dirty_all();
    
    os_println("all clean, need reboot");
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
static inline int
benv_vcs_cmp(char *obj, benv_vcs_t * a, benv_vcs_t * b)
{
    int ret;

    if (benv_vcs_is_good(a) && false==benv_vcs_is_good(b)) {
        /*
        * a is good, b is bad
        */
        return 1;
    }
    else if (false==benv_vcs_is_good(a) && benv_vcs_is_good(b)) {
        /*
        * a is bad, b is good
        */
        return -1;
    }
    
    ret = benv_version_cmp(&a->version, &b->version);
    if (ret) {
        return ret;
    }
    
    ret = benv_error_cmp(a->error, b->error);
    if (ret) {
        return ret;
    }

    ret = benv_fsm_cmp(a->upgrade, b->upgrade);
    if (ret) {
        return ret;
    }

    ret = benv_fsm_cmp(a->other, b->other);
    if (ret) {
        return ret;
    }

    ret = benv_fsm_cmp(a->self, b->self);
    if (ret) {
        return ret;
    }
    
    return 0;
}

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
static inline int
__benv_obj_min(char *obj, int sort[], int count, int (*cmp)(int a, int b))
{
    int i, idx = 0;

    for (i=0; i<count; i++) {
        if (cmp(sort[i], sort[idx]) < 0) {
            idx = i;
        }
    }
    
#if BENV_DEBUG & (BENV_DEBUG_SORT | BENV_DEBUG_CMP)
    os_println("min %s is sort[%d]", obj, idx);
#endif

    return idx;
}

/*
* return sort idx
*/
static inline int
__benv_obj_max(char *obj, int sort[], int count, int (*cmp)(int a, int b))
{
    int i, idx = 0;

    for (i=0; i<count; i++) {
        if (cmp(sort[i], sort[idx]) > 0) {
            idx = i;
        }
    }
    
#if BENV_DEBUG & (BENV_DEBUG_SORT | BENV_DEBUG_CMP)
    os_println("max %s is sort[%d]", obj, idx);
#endif

    return idx;
}

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

static inline void
__benv_sort(int sort[], int count, int (*maxmin)(int sort[], int count))
{
    if (count <= 1) {
        return;
    }
    
    int idx = (*maxmin)(sort, count);
    if (idx) {
        os_swap_value(sort[0], sort[idx]);
    }
    
    __benv_sort(sort+1, count-1, maxmin);
}

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

static inline int
__benv_sort_count(int skips, int sort[], int size)
{
    int i, count = 0;

    for (i=0; i<size; i++) {
        if (false==is_benv_skip(skips, i)) {
            sort[count++] = i;
        }
    }

    return count;
}
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
    int idx_in_benv_find_best = __benv_find_xest(_obj, _skips, __benv_downsort, benv_obj_is_good); \
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
            && benv_obj_is_good(_obj, i_in___benv_find_first_buddy)) { \
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
        __benv_find_first_buddy(_obj, _idx, _skips, benv_obj_is_good)

#define benv_find_first_bad_buddy(_obj, _idx, _skips) \
        __benv_find_first_buddy(_obj, _idx, _skips, benv_obj_is_bad)

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
        __benv_find_first_byversion(_obj, _version, _skips, benv_obj_is_good)

#define benv_find_first_bad_byversion(_obj, _version, _skips) \
        __benv_find_first_byversion(_obj, _version, _skips, benv_obj_is_bad)

static inline void
__benv_show_header(benv_ops_t * ops)
{
    if (__benv_show_count > 1) {
        os_printf("%s=", ops->path);
    }
}

static inline void
__benv_show_number(benv_ops_t * ops)
{
    __benv_show_header(ops);

    os_println("%u", *benv_ops_number(ops));
}

static inline void
__benv_show_string(benv_ops_t * ops)
{
    char *string = benv_ops_string(ops);

    if (string[0]) {
        __benv_show_header(ops);

        os_println("%s", string);
    }
}

static inline void
__benv_set_number(benv_ops_t * ops, char *value)
{
    *benv_ops_number(ops) = (uint32)(value[0] ? os_atoi(value) : 0);
}

static inline void
__benv_set_string(benv_ops_t * ops, char *value)
{
    char *string = benv_ops_string(ops);

    if (value[0]) {
        os_strcpy(string, value);
    } else {
        string[0] = 0;
    }
}

static inline int
__benv_check_version(benv_ops_t * ops, char *value)
{
    benv_version_t version = BENV_INVALID_VERSION;

    if (__benv_version_atoi(&version, value)) {
        /*
         * when set version, must input value
         */
        return -EFORMAT;
    } else {
        return 0;
    }
}

static inline void
__benv_show_version(benv_ops_t * ops)
{
    __benv_show_header(ops);

    os_println("%s", benv_version_itoa(benv_ops_version(ops)));
}

static inline void
__benv_set_version(benv_ops_t * ops, char *value)
{
    benv_version_t *v = benv_ops_version(ops);

    if (value[0]) {
        __benv_version_atoi(v, value);
    } else {
        benv_version_t version = BENV_DEFT_VERSION;

        os_objscpy(v, &version);
    }
}

static inline int
__benv_check_fsm(benv_ops_t * ops, char *value)
{
    if (0==value[0]) {
        /* 
         * clear fsm(to unknow)
         */
        return 0;
    } else if (is_good_benv_fsm(benv_fsm_idx(value))) {
        return 0;
    } else {
        debug_error("bad fsm:%s", value);

        /*
         * when set self/other/upgrade, must input value
         */
        return -EFORMAT;
    }
}

static inline void
__benv_show_fsm(benv_ops_t * ops)
{
    int fsm = *benv_ops_fsm(ops);

    __benv_show_header(ops);

    os_println("%s", benv_fsm_string(fsm));
}

static inline void
__benv_set_fsm(benv_ops_t * ops, char *value)
{
    uint32 fsm;

    if (value[0]) {
        fsm = benv_fsm_idx(value);
    } else {
        fsm = BENV_FSM_UNKNOW;
    }

    *benv_ops_fsm(ops) = fsm;
}

static inline int
__benv_check_current(benv_ops_t * ops, char *value)
{
    char *end = NULL;
    int idx = os_strtol(value, &end, 0);
    
    if (0==value[0]) {
        /*
         * when set kernel/rootfs current, must input value
         */
        debug_error("set current failed, must input value");

        return -EFORMAT;
    }
    else if (false==os_strton_is_good_end(end)) {
        debug_error("input invalid current:%s", value);
        
        return -EFORMAT;
    }
    else if (false==is_good_benv_idx(idx)) {
        debug_error("bad current %d", idx);

        return -EFORMAT;
    }
    else{
        return 0;
    }
}

static inline int
__benv_check_string(benv_ops_t * ops, char *value)
{
    int size = 0;

    switch(__benv_ops_is(ops->offset)) {
        case BENV_OS:
            size = BENV_VCS_COOKIE_SIZE;
            break;
        case BENV_INFO:
            size = BENV_INFO_SIZE;
            break;
        case BENV_COOKIE:
        case BENV_MARK:
        default:
            /*
            * no support set string
            */
            break;
    }
    
    if (size && value[0]) {
        if (os_strlen(value) < size) {
            return 0;
        } else {
            debug_error("max string length %d", size);
            
            return -EFORMAT;
        }
    } else {
        /*
         * when set var string, may NOT input value
         */
        return 0;
    }
}

#define BENV_OPS(_path, _member, _check, _write, _show) { \
    .path   = _path,    \
    .offset = offsetof(benv_env_t, _member), \
    .check  = _check,   \
    .write  = _write,   \
    .show   = _show,    \
}   /* end */

#define __BENV_COOKIE_OPS(_path, _member, _show) \
    BENV_OPS("cookies/" _path, cookie._member, NULL, NULL, _show) \
    /* end */

#define BENV_COOKIE_OPS \
    __BENV_COOKIE_OPS("product/vendor",         \
        product.vendor,  __benv_show_string),   \
    __BENV_COOKIE_OPS("product/company",        \
        product.company, __benv_show_string),   \
    __BENV_COOKIE_OPS("pcba/model",             \
        pcba.model,      __benv_show_string)    \
    /* end */

#define __BENV_OS_OPS(_path, _member, _check, _write, _show) \
    BENV_OPS("os/" _path, os._member, _check, _write, _show) \
    /* end */

#define __BENV_FIRMWARE_OPS(_obj, _idx)         \
    __BENV_OS_OPS(#_obj "/" #_idx "/self",      \
        firmware[_idx]._obj.self,               \
        __benv_check_fsm, __benv_set_fsm, __benv_show_fsm), \
    __BENV_OS_OPS(#_obj "/" #_idx "/other",     \
        firmware[_idx]._obj.other,              \
        __benv_check_fsm, __benv_set_fsm, __benv_show_fsm), \
    __BENV_OS_OPS(#_obj "/" #_idx "/upgrade",   \
        firmware[_idx]._obj.upgrade,            \
        __benv_check_fsm, __benv_set_fsm, __benv_show_fsm), \
    __BENV_OS_OPS(#_obj "/" #_idx "/error",     \
        firmware[_idx]._obj.error,              \
        NULL, __benv_set_number, __benv_show_number), \
    __BENV_OS_OPS(#_obj "/" #_idx "/version",   \
        firmware[_idx]._obj.version,            \
        __benv_check_version, __benv_set_version, __benv_show_version), \
    __BENV_OS_OPS(#_obj "/" #_idx "/cookie",    \
        firmware[_idx]._obj.cookie,             \
        __benv_check_string, __benv_set_string, __benv_show_string) \
    /* end */

#define BENV_FIRMWARE_OPS(_idx)         \
    __BENV_FIRMWARE_OPS(kernel, _idx),  \
    __BENV_FIRMWARE_OPS(rootfs, _idx)   \
    /* end */

#define BENV_OS_COMMON_OPS              \
    __BENV_OS_OPS("current", current,   \
        __benv_check_current, __benv_set_number, __benv_show_number) \
    /* end */

#if 1==PRODUCT_FIRMWARE_COUNT
#define BENV_OS_OPS         \
    BENV_OS_COMMON_OPS,     \
    BENV_FIRMWARE_OPS(0)    \
    /* end */
#elif 3==PRODUCT_FIRMWARE_COUNT
#define BENV_OS_OPS         \
    BENV_OS_COMMON_OPS,     \
    BENV_FIRMWARE_OPS(0),   \
    BENV_FIRMWARE_OPS(1),   \
    BENV_FIRMWARE_OPS(2)    \
    /* end */
#elif 5==PRODUCT_FIRMWARE_COUNT
#define BENV_OS_OPS         \
    BENV_OS_COMMON_OPS,     \
    BENV_FIRMWARE_OPS(0),   \
    BENV_FIRMWARE_OPS(1),   \
    BENV_FIRMWARE_OPS(2),   \
    BENV_FIRMWARE_OPS(3),   \
    BENV_FIRMWARE_OPS(4)    \
    /* end */
#elif 7==PRODUCT_FIRMWARE_COUNT
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
#endif

#define __BENV_MARK_OPS(_path, _idx, _check, _set, _show) \
    BENV_OPS("marks/" _path, mark.key[_idx], _check, __benv_set_number, __benv_show_number)

#define __BENV_MARK_OPS_IDX(_idx) \
    __BENV_MARK_OPS(#_idx, _idx, NULL, __benv_set_number, __benv_show_number)

#define __BENV_MARK_OPS_IDX_RO(_idx) \
    __BENV_MARK_OPS(#_idx, _idx, NULL, NULL, __benv_show_number)

static inline uint32 *
benv_mark_pointer(int idx)
{
    return &benv_mark(idx);
}

static inline uint32
benv_mark_get(int idx)
{
    return benv_mark(idx);
}

static inline uint32
benv_mark_set(int idx, uint32 value)
{
    return (benv_mark(idx) = value);
}

static inline uint32
benv_mark_add(int idx, int value)
{
    return (benv_mark(idx) += value);
}

#define BENV_MARK_OPS_NAMES \
    __BENV_MARK_OPS("ptest/control",__benv_mark_ptest_control,  NULL, __benv_set_number,__benv_show_number), \
    __BENV_MARK_OPS("ptest/result", __benv_mark_ptest_result,   NULL, __benv_set_number,__benv_show_number), \
    __BENV_MARK_OPS("debug",        __benv_mark_debug,          NULL, __benv_set_number,__benv_show_number), \
    __BENV_MARK_OPS("ut",           __benv_mark_uptimes,        NULL, NULL,             __benv_show_number), \
    __BENV_MARK_OPS("rt",           __benv_mark_runtime,        NULL, NULL,             __benv_show_number), \
    __BENV_MARK_OPS("mid",          __benv_mark_cid_mid,        NULL, NULL,             __benv_show_number), \
    __BENV_MARK_OPS("psn",          __benv_mark_cid_psn,        NULL, NULL,             __benv_show_number), \
    __BENV_MARK_OPS("na",           __benv_mark_noauth,         NULL, NULL,             __benv_show_number), \
    __BENV_MARK_OPS("js",           __benv_mark_jsdebug,        NULL, NULL,             __benv_show_number)  \
    /* end */

#ifdef __BOOT__
#define BENV_MARK_OPS_IDX       \
    __BENV_MARK_OPS_IDX(9)      \
    /* end */

#elif defined(__APP__)

#define BENV_MARK_OPS_IDX       \
    __BENV_MARK_OPS_IDX(9),     \
                                \
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
#endif

#define BENV_MARK_OPS       \
    BENV_MARK_OPS_NAMES,    \
    BENV_MARK_OPS_IDX       \
    /* end */

#define __BENV_INFO_OPS(_path, _idx) \
    BENV_OPS("infos/" _path, info.var[_idx], __benv_check_string, __benv_set_string, __benv_show_string) \
    /* end */

enum {
    __benv_info_pcba_vendor     = 0,
    __benv_info_pcba_company    = 1,
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
    
    ____benv_info_idx_max,
    __benv_info_idx_max             = ____benv_info_idx_max - 1,
    __benv_info_block_named_count   = (1 + __benv_info_idx_max/BENV_INFO_COUNT_PER_BLOCK),
};

static inline char *
benv_info_get(int idx)
{
    return benv_info(idx);
}

#define BENV_INFO_OPS_NAMES_COMMON  \
    __BENV_INFO_OPS("pcba/vendor",      __benv_info_pcba_vendor),       \
    __BENV_INFO_OPS("pcba/company",     __benv_info_pcba_company),      \
    __BENV_INFO_OPS("pcba/model",       __benv_info_pcba_model),        \
    __BENV_INFO_OPS("pcba/mac",         __benv_info_pcba_mac),          \
    __BENV_INFO_OPS("pcba/sn",          __benv_info_pcba_sn),           \
    __BENV_INFO_OPS("pcba/version",     __benv_info_pcba_version),      \
    __BENV_INFO_OPS("pcba/rootrw",      __benv_info_pcba_rootrw),       \
                                                                        \
    __BENV_INFO_OPS("product/vendor",   __benv_info_product_vendor),    \
    __BENV_INFO_OPS("product/company",  __benv_info_product_company),   \
    __BENV_INFO_OPS("product/model",    __benv_info_product_model),     \
    __BENV_INFO_OPS("product/mac",      __benv_info_product_mac),       \
    __BENV_INFO_OPS("product/sn",       __benv_info_product_sn),        \
    __BENV_INFO_OPS("product/manager",  __benv_info_product_manager),   \
    __BENV_INFO_OPS("product/version",  __benv_info_product_version),   \
                                                                        \
    __BENV_INFO_OPS("oem/vendor",       __benv_info_oem_vendor),        \
    __BENV_INFO_OPS("oem/company",      __benv_info_oem_company),       \
    __BENV_INFO_OPS("oem/model",        __benv_info_oem_model),         \
    __BENV_INFO_OPS("oem/mac",          __benv_info_oem_mac),           \
    __BENV_INFO_OPS("oem/sn",           __benv_info_oem_sn),            \
    __BENV_INFO_OPS("oem/manager",      __benv_info_oem_manager),       \
    __BENV_INFO_OPS("oem/version",      __benv_info_oem_version)        \
    /* end */

#ifdef __BOOT__
#define BENV_INFO_OPS_NAMES     \
    BENV_INFO_OPS_NAMES_COMMON  \
    /* end */

#define BENV_INFO_OPS   \
    BENV_INFO_OPS_NAMES \
    /* end */

#elif defined(__APP__)
#define BENV_INFO_OPS_NAMES     \
    BENV_INFO_OPS_NAMES_COMMON  \
    /* end */

#define __BENV_INFO_OPS_IDX(_idx)   \
    __BENV_INFO_OPS(#_idx, _idx)    \
    /* end */

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
#endif

#define BENV_DEFT_OPS { \
    BENV_COOKIE_OPS,    \
    BENV_OS_OPS,        \
    BENV_MARK_OPS,      \
    BENV_INFO_OPS,      \
}   /* end */

static inline void
__benv_show_byprefix(char *prefix)
{
    int i;
    int len = prefix ? os_strlen(prefix) : 0;

    for (i = 0; i < __benv_ops_count; i++) {
        benv_ops_t *ops = benv_ops(i);

        if (benv_ops_match_wildcard(ops, prefix, len)) {
            benv_ops_show(ops);
        }
    }
}

#define benv_show_byprefix(_fmt, _args...) do{\
    char prefix[1+OS_LINE_LEN];             \
                                            \
    os_saprintf(prefix, _fmt, ##_args);     \
                                            \
    __benv_show_byprefix(prefix);             \
}while(0)

static inline void
__benv_handle_write(benv_ops_t * ops)
{
    char *value = benv_cache_value(ops);

    if (value) {
        benv_ops_write(ops, value);
    }
}

static inline void
__benv_handle_show(benv_ops_t * ops)
{
    benv_ops_show(ops);
}

static inline void
__benv_handle(benv_ops_t * ops)
{
    /*
     * show
     */
    if (benv_cache_showit(ops)) {
        __benv_handle_show(ops);
    }
    /*
     * wirite
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
__benv_analysis_write(benv_ops_t * ops, char *args)
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
__benv_analysis_show(benv_ops_t * ops, char *args)
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
        benv_cache_showit(ops) = true;

        __benv_show_count++;
    }

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
        return benv_analysis_write(args);
    }
    /*
     * no found '=', is show
     */
    else {
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
    int err;

    if (0==argc) {
        return benv_usage();
    }

    err = benv_analysis(argc, argv);
    if (err<0) {
        return err;
    }

    benv_ops_dump("command");

    benv_handle(argc, argv);

    return 0;
}

#ifdef __BOOT__
extern int 
benv_emmc_read(uint32 begin, void *buf, int size);

extern int 
benv_emmc_write(uint32 begin, void *buf, int size);

#define benv_open()         0
#define benv_close()        0

extern int __benv_load(int idx);    /* idx is benv's block */
extern int __benv_save(int idx);    /* idx is benv's block */
#elif defined(__APP__)
#if IS_PRODUCT_LTEFI_MD1
#define benv_open()         0
#define benv_close()        0
#define __benv_load(_idx)   0
#define __benv_save(_idx)   0
#elif IS_PRODUCT_LTEFI_MD2 || \
      IS_PRODUCT_LTEFI_MD3 || \
      IS_PRODUCT_PC
static inline int
benv_open(void)
{
    __benv_fd = __os_file_lock(PRODUCT_DEV_BOOTENV, O_RDWR, 0, true);

    return is_good_fd(__benv_fd)?0:__benv_fd;
}

static inline int
benv_close(void)
{
    __os_file_unlock(PRODUCT_DEV_BOOTENV, __benv_fd);

    __benv_fd = INVALID_FD;

    return 0;
}

static inline int
__benv_load(int idx)
{
    int offset = BENV_BLOCK_SIZE * idx;
    void *obj = (char *)__benv_env + offset;
    
    int err;
    
    err = lseek(__benv_fd, offset, SEEK_SET);
    if (err < 0) {        /* <0 is error */
        debug_error("seek benv error:%d", -errno);

        return -errno;
    }

    if (BENV_BLOCK_SIZE != read(__benv_fd, obj, BENV_BLOCK_SIZE)) {
        debug_error("read benv error:%d", -errno);

        return -errno;
    }

    return 0;
}

static inline int
__benv_save(int idx /* benv's block */ )
{
    int offset = BENV_BLOCK_SIZE * idx;
    void *obj = (char *)__benv_env + offset;
    int err;

    err = lseek(__benv_fd, offset, SEEK_SET);
    if (err < 0) {        /* <0 is error */
        debug_error("seek benv error:%d", -errno);

        return -errno;
    }

    if (BENV_BLOCK_SIZE != write(__benv_fd, obj, BENV_BLOCK_SIZE)) {
        debug_error("write benv error:%d", -errno);

        return -errno;
    }

    return 0;
}
#endif
#endif

static inline int
benv_loadby(int begin, int count)
{
    int i, err = 0;

    for (i = begin; i < count; i++) {
        err = __benv_load(i);
        if (err<0) {
            /* todo: log */
        }
    }
    
    return err;
}

#define benv_load_nothing(_idx) 0
#define benv_load_cookie()      __benv_load(BENV_COOKIE)
#define benv_load_os()          __benv_load(BENV_OS)
#define benv_load_mark()        __benv_load(BENV_MARK)
#define benv_load_info()        benv_loadby(BENV_INFO, BENV_BLOCK_COUNT)
#define benv_load_info_named()  benv_loadby(BENV_INFO, BENV_INFO + __benv_info_block_named_count)
#define benv_load()             benv_loadby(0, BENV_BLOCK_COUNT)

static inline int
benv_save(void)
{
    int i, err = 0;
    int begin = __benv_dirty[0]?0:1;

    /* 
     * block 0 is read only
     */
    for (i = begin; i < BENV_BLOCK_COUNT; i++) {
        if (__benv_dirty[i]) {
            err = __benv_save(i);
                debug_trace_error(err, "save benv block %d", i);
            if (err<0) {
                /* todo: log */
            }

            __benv_dirty[i] = false;
        }
    }

    return err;
}

static inline int
__benv_init(void)
{
    int err;

    BUILD_BUG_ON(BENV_COOKIE_SIZE     > BENV_BLOCK_SIZE);
    BUILD_BUG_ON(BENV_OS_SIZE         > BENV_BLOCK_SIZE);
    BUILD_BUG_ON(__benv_info_idx_max  >= BENV_INFO_COUNT);
    
    err = os_init();
    if (err<0) {
        return err;
    }
    
    __benv_show_count = 0;
    if (__benv_cache) {
        os_memzero(__benv_cache, __benv_ops_count * sizeof(benv_cache_t));
    }

    os_arrayzero(__benv_dirty);

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
__benv_fini(void)
{
    int err;
    
    err = benv_save();
        debug_trace_error(err, "save benv");
        
    err = benv_close();
        debug_trace_error(err, "close benv");

    return err;
}

static inline void
__benv_show_cookie(void)
{
    __benv_cookie_show(__benv_cookie);
}

static inline void
__benv_show_os(void)
{
    __benv_os_show(__benv_os);
}

static inline bool
__benv_cmd(int argc, char *argv[])
{
#define __benv_cmd_item(_action, _obj, _handle) {.action = _action, .obj = _obj, .handle = _handle}
    static struct {
        char *action;
        char *obj;
        void (*handle)(void);
    } cmd[] = {
        __benv_cmd_item("show",   "cookie",   __benv_show_cookie),
        __benv_cmd_item("show",   "os",       __benv_show_os),
        
        __benv_cmd_item("reset",  "os",       __benv_deft_os),
        __benv_cmd_item("reset",  "all",      __benv_deft),
        
        __benv_cmd_item("clean",  "cookie",   __benv_clean_cookie),
        __benv_cmd_item("clean",  "mark",     __benv_clean_mark),
        __benv_cmd_item("clean",  "info",     __benv_clean_info),
        __benv_cmd_item("clean",  "all",      __benv_clean),
    };
#undef __benv_cmd_item
    char *action    = argv[1];
    char *obj       = argv[2];
    int i;

    if (3==argc) {
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
    
    if (true==__benv_cmd(argc, argv)) {
        return 0;
    }

    return benv_command(argc - 1, argv + 1);
}

static inline int
__benv_main(int argc, char *argv[])
{
    return os_call(__benv_init, __benv_fini, benv_cmd, argc, argv);
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
