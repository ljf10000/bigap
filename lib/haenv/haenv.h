#ifndef __HAENV_H_2348774a939f4da0a188bca3067f44bb__
#define __HAENV_H_2348774a939f4da0a188bca3067f44bb__
#include "utils.h"
#if defined(__BOOT__) || defined(__APP__)
/******************************************************************************/
#define HAENV_DEBUG_SORT            0x01
#define HAENV_DEBUG_CMP             0x02

#ifndef HAENV_DEBUG
#define HAENV_DEBUG                 0 //(HAENV_DEBUG_SORT | HAENV_DEBUG_CMP)
#endif

#ifndef HAENV_COUNT
#define HAENV_COUNT                 4
#endif

#ifndef HAENV_SIZE
#define HAENV_SIZE                  (64*1024)
#endif
#define HAENV_GCSIZE                (HAENV_SIZE*3/4)

#ifndef HAENV_BLOCK_SIZE
#define HAENV_BLOCK_SIZE            512
#endif
#define HAENV_BLOCK_COUNT           (HAENV_SIZE/HAENV_BLOCK_SIZE) // 128

#ifndef HAENV_BOOT_SIZE
#define HAENV_BOOT_SIZE             (512*1024)
#endif

#ifndef HAENV_BOOTENV_SIZE
#define HAENV_BOOTENV_SIZE          (512*1024 - HAENV_SIZE*HAENV_COUNT)
#endif

#ifndef HAENV_START
#ifdef __BOOT__ /* offset of flash(global) */
#   define HAENV_START              (HAENV_BOOT_SIZE + HAENV_BOOTENV_SIZE)
#else /* offset of HAENV_FILE */
#   ifdef __PC__
#       define HAENV_START          0
#   else
#       define HAENV_START          HAENV_BOOTENV_SIZE
#   endif
#endif
#endif

#ifndef HAENV_FILE
#   ifdef __PC__
#       define HAENV_FILE           "bootenv"
#   else
#       define HAENV_FILE           "/dev/mmcblk0p02"
#   endif
#endif

#ifndef HAENV_ROOT
#define HAENV_ROOT                  "/dev/mmcblk0p14"
#endif

/*
* for read
*/
#ifndef HAENV_EXPORT
#define HAENV_EXPORT                "/tmp/.haenv"
#endif

/*
* after write, load to it, and rename to HAENV_EXPORT
*/
#ifndef HAENV_TEMP
#define HAENV_TEMP                  "/tmp/.haenv.tmp"
#endif

#define HAENV_ROOTFS_MODE_TYPE_RW   1
#define HAENV_ROOTFS_MODE_TYPE_RO   2

#ifndef HAENV_ROOTFS_MODE_TYPE
#define HAENV_ROOTFS_MODE_TYPE       HAENV_ROOTFS_MODE_TYPE_RO
#endif

#ifndef HAENV_ROOTFS_MODE
#   if HAENV_ROOTFS_MODE_TYPE==HAENV_ROOTFS_MODE_TYPE_RW
#       define HAENV_ROOTFS_MODE          "rw"
#   elif HAENV_ROOTFS_MODE_TYPE==HAENV_ROOTFS_MODE_TYPE_RO
#       define HAENV_ROOTFS_MODE          "ro"
#   else
#       error "invalid HAENV_ROOTFS_MODE_TYPE"
#   endif
#endif


#define CONFIG_BOOTARGS_HEAD    \
    "root=" HAENV_ROOT          \
        " "                     \
    /* end */

#define CONFIG_BOOTARGS_BODY    \
    "mem=2G"                    \
        " "                     \
    "console=ttyAMA0,115200"    \
        " "                     \
    "rootfstype=ext4"           \
        " "                     \
    "rootwait"                  \
        " "                     \
    HAENV_ROOTFS_MODE           \
        " "                     \
    "blkdevparts="              \
        "mmcblk0:"              \
        "512K(fastboot),"/*01 */\
        "512K(bootenv),"/* 02 */\
        "4M(baseparam),"/* 03 */\
        "4M(pqparam),"  /* 04 */\
        "3M(logo),"     /* 05 */\
                                \
        "16M(kernel0)," /* 06 */\
        "16M(kernel1)," /* 07 */\
        "16M(kernel2)," /* 08 */\
        "16M(kernel3)," /* 09 */\
        "16M(kernel4)," /* 10 */\
        "16M(kernel5)," /* 11 */\
        "16M(kernel6)," /* 12 */\
                                \
        "256M(rootfs0),"/* 13 */\
        "256M(rootfs1),"/* 14 */\
        "256M(rootfs2),"/* 15 */\
        "256M(rootfs3),"/* 16 */\
        "256M(rootfs4),"/* 17 */\
        "256M(rootfs5),"/* 18 */\
        "256M(rootfs6),"/* 19 */\
                                \
        "32M(config0)," /* 20 */\
        "32M(config1)," /* 21 */\
                                \
        "820M(data0),"  /* 22 */\
        "820M(data1),"  /* 23 */\
        "-(others)"             \
        " "                     \
    "mmz=ddr,0,0,300M"          \
    /* end */

#ifdef CONFIG_BOOTARGS
#undef CONFIG_BOOTARGS
#endif

#define CONFIG_BOOTARGS         \
    CONFIG_BOOTARGS_HEAD        \
    CONFIG_BOOTARGS_BODY        \
    /* end */

#define HAENV_MD5_SIZE          16

#define HAENV_ALIGN(_len)       OS_ALIGN(_len, sizeof(uint32))

#define HAENV_F_DIRTY           0x01

typedef struct {
    byte    md5[HAENV_MD5_SIZE];
    byte    flag;
    byte    klen;
    uint16  vlen;
    
    uint32  seq;
    
    char data[0];
} haenv_entry_t;

typedef struct {    
    char mirror[HAENV_SIZE];
    
    STREAM f;       // set @init
    uint32  start;  // set @init
    
    uint32  saved;  // size: have saved to flash
    uint32  unsaved;// size: not unsaved to flash
    
    bool    damaged;
} haenv_t;

typedef struct {
    haenv_t env[HAENV_COUNT];
    
    os_sem_t sem;
    
    uint32 seq;     // next seq
} haenv_file_t;

#if defined(__BOOT__)
#   define DECLARE_REAL_HAENV       haenv_file_t *__THIS_HAENV
#   define DECLARE_FAKE_HAENV       extern haenv_file_t *__THIS_HAENV
#   define DECLARE_HAENV            DECLARE_REAL_HAENV
#else
#   define DECLARE_REAL_HAENV       haenv_file_t __THIS_HAENV
#   define DECLARE_FAKE_HAENV       extern haenv_file_t __THIS_HAENV
#   ifdef __BUSYBOX__
#       define DECLARE_HAENV        DECLARE_FAKE_HAENV
#   else
#       define DECLARE_HAENV        DECLARE_REAL_HAENV
#   endif
#endif

DECLARE_FAKE_HAENV;

static inline haenv_file_t *
haenv(void)
{
#ifdef __BOOT__
    return __THIS_HAENV;
#else
    return &__THIS_HAENV;
#endif
}

#define haenv_seq       haenv()->seq
#define haenv_env(_id)  (&haenv()->env[_id])
#define haenv_first()   haenv_env(0)
#define haenv_sem()     (&haenv()->sem)

/*
* zone as [begin, end)
*/
static inline bool
is_good_haenv_zone(uint32 begin, uint32 end)
{
    return  begin < end && begin < HAENV_SIZE && end <= HAENV_SIZE;
}


#ifdef __BOOT__
extern int 
benv_emmc_read(uint32 begin, void *buf, int size);

extern int 
benv_emmc_write(uint32 begin, void *buf, int size);

/*
* begin: haenv file offset
*/
static inline int
__hae_read(haenv_t *env, uint32 begin, byte *buf, uint32 size)
{
    return benv_emmc_read(env->start + begin, buf, size);
}

/*
* begin:  haenv file offset
*/
static inline int
__hae_write(haenv_t *env, uint32 begin, byte *buf, uint32 size)
{
    return benv_emmc_write(env->start + begin, buf, size);
}
#else
/*
* begin: haenv file offset
*/
static inline int
__hae_read(haenv_t *env, uint32 begin, byte *buf, uint32 size)
{
    int err = os_fseek(env->f, env->start + begin, SEEK_SET);
    if (err<0) {
        return err;
    }
    
    return os_fread(env->f, buf, size);
}

/*
* begin:  haenv file offset
*/
static inline int
__hae_write(haenv_t *env, uint32 begin, byte *buf, uint32 size)
{
    int err = os_fseek(env->f, env->start + begin, SEEK_SET);
    if (err<0) {
        return err;
    }
    
    return os_fwrite(env->f, buf, size);
}
#endif

/*
* begin:  haenv offset
*/
static inline int
hae_read(haenv_t *env, uint32 begin, byte *buf, uint32 size)
{
    if (false==is_good_haenv_zone(begin, begin + size)) {
        return -ERANGE;
    }
    
    return __hae_read(env, begin, buf, size);
}

/*
* begin:  haenv offset
*/
static inline int
hae_write(haenv_t *env, uint32 begin, byte *buf, uint32 size)
{
    if (false==is_good_haenv_zone(begin, begin + size)) {
        return -ERANGE;
    }
    
    return __hae_write(env, begin, buf, size);
}

static inline uint32
hae_offsetof(haenv_t *env, haenv_entry_t *e)
{
    return (uint32)pointer_offset(e, env);
}

static inline bool
is_haee_dirty(haenv_entry_t *e)
{
    return os_hasflag(e->flag, HAENV_F_DIRTY);
}

/*
* key + ['\0'] + [pad]
*/
static inline uint32
haee_ksize(haenv_entry_t *e)
{
    return e->klen?HAENV_ALIGN(1 + e->klen):0;
}

static inline uint32
haee_kpad_len(haenv_entry_t *e)
{
    return e->klen?(haee_ksize(e) - e->klen - 1):0;
}

static inline char *
haee_key(haenv_entry_t *e)
{
    return e->klen?e->data:NULL;
}

static inline char *
haee_kpad(haenv_entry_t *e)
{
    return e->klen?(haee_key(e) + e->klen + 1):NULL;
}

static inline void
haee_kpad_zero(haenv_entry_t *e)
{
    char *pad = haee_kpad(e);
    if (pad) {
        os_memzero(pad, haee_kpad_len(e));
    }
}

/*
* value + ['\0'] + [pad]
*/
static inline uint32
haee_vsize(haenv_entry_t *e)
{
    /*
    * string length NOT include '\0'
    * string value keep '\0', for string function(strlen/strstr/...)
    */
    return e->vlen?HAENV_ALIGN(1 + e->vlen):0;
}

static inline uint32
haee_vpad_len(haenv_entry_t *e)
{
    return e->vlen?(haee_vsize(e) - e->vlen - 1):0;
}

static inline char *
haee_value(haenv_entry_t *e)
{
    return e->vlen?(e->data + haee_ksize(e)):NULL;
}

static inline void *
haee_vpad(haenv_entry_t *e)
{
    return e->vlen?(haee_value(e) + e->vlen + 1):NULL;
}

static inline void
haee_vpad_zero(haenv_entry_t *e)
{
    char *pad = haee_vpad(e);
    if (pad) {
        os_memzero(pad, haee_vpad_len(e));
    }
}

static inline uint32
haee_dsize(haenv_entry_t *e)
{
    return haee_ksize(e) + haee_vsize(e);
}

static inline uint32
haee_dlen(haenv_entry_t *e)
{
    return haee_ksize(e) + e->vlen;
}

static inline uint32
haee_size(haenv_entry_t *e)
{
    return haee_dsize(e) + sizeof(haenv_entry_t);
}

static inline void
haee_pad_zero(haenv_entry_t *e)
{
    haee_kpad_zero(e);
    haee_vpad_zero(e);
}

static inline haenv_entry_t *
haee_next(haenv_entry_t *e)
{
    return (haenv_entry_t *)(e->data + haee_dsize(e));
}

static inline haenv_entry_t *
hae_first(haenv_t *env)
{
    return (haenv_entry_t *)env->mirror;
}

static inline haenv_entry_t *
hae_end(haenv_t *env)
{
    return (haenv_entry_t *)(env->mirror + HAENV_SIZE);
}

static inline bool
is_good_haee(haenv_t *env, haenv_entry_t *e)
{
    return e >= hae_first(env) && haee_next(e) <= hae_end(env);
}

static inline bool
is_good_haee_md5(haenv_t *env, haenv_entry_t *e)
{
    return true;
}

static inline int
haee_set(haenv_entry_t *e, char *k, char *v)
{
    if (e) {
        e->klen = os_strlen(k);
        e->vlen = os_strlen(v);
        e->seq  = haenv_seq;
        e->flag = HAENV_F_DIRTY;
        
        os_strmcpy(haee_key(e), k, e->klen);
        os_strmcpy(haee_value(e), v, e->vlen);
        haee_pad_zero(e);

        // todo: md5

        return 0;
    } else {
        return -EKEYNULL;
    }
}

static inline haenv_entry_t *
haee_clone(haenv_entry_t *dst, haenv_entry_t *src)
{
    return (haenv_entry_t *)os_memcpy(dst, src, haee_size(src));
}

static inline int
haee_flush(haenv_t *env, haenv_entry_t *e)
{
    os_clrflag(e->flag, HAENV_F_DIRTY);
    
    return hae_write(env, hae_offsetof(env, e), e, haee_size(e));
}

static inline int
haee_check(haenv_entry_t *e)
{
    return 0;
}

#define HAENV_FOREACH(i)                    for (_i=0; _i<HAENV_COUNT; _i++)
#define __haenv_foreach(_i, _env, _begin)   for (_i=_begin, _env = haenv_env(_i); _i<HAENV_COUNT; _i++, _env = haenv_env(_i))
#define haenv_foreach(_i, _env)             __haenv_foreach(_i, _env, 0)

static inline haenv_entry_t *
hae_next(haenv_t *env, haenv_entry_t *current)
{
    haenv_entry_t *e = haee_next(current);
    
    return is_good_haee(env, e)?e:NULL;
}

#define hae_foreach(_env, _e)       for (_e = hae_first(_env); _e; _e = hae_next(_env, _e))

static inline haenv_entry_t *
hae_empty(haenv_t *env)
{
    haenv_entry_t *e = (haenv_entry_t *)(env->mirror + env->unsaved);

    return is_good_haee(env, e)?e:NULL;
}

static inline haenv_t *
hae_clone(haenv_t *dst, haenv_t *src)
{
    os_memcpy(dst->mirror, src->mirror, sizeof(src->mirror));

    dst->saved  = src->saved;
    dst->unsaved= src->unsaved;
    dst->damaged= src->damaged;

    return dst;
}

static inline void
hae_clean(haenv_t *env)
{
    os_memzero(env->mirror);

    env->saved  = 0;
    env->unsaved= 0;
    env->damaged= false;
}

static inline bool
hae_eq(haenv_t *a, haenv_t *b)
{
    return os_memcmp(a->mirror, b->mirror, sizeof(b->mirror))
        && a->saved     == b->saved
        && a->unsaved   == b->unsaved
        && a->damaged   == a->damaged;
}

static inline int
hae_load(haenv_t *env)
{
    return hae_read(env, 0, env->mirror, sizeof(env->mirror));
}

static inline int
hae_save(haenv_t *env)
{
    int err = hae_write(env, 0, env->mirror, sizeof(env->mirror));

    fflush(env->f);

    return err;
}

static inline int
hae_check(haenv_t *env)
{
    haenv_entry_t *e;
    int err = 0;
    
    hae_foreach(env, e) {
        if (false==is_good_haee_md5(env, e)) {
            env->damaged = true;
            
            err = -EDAMAGED;
        } else {
            env->saved += haee_size(e);
        }
    }

    env->unsaved = env->saved;
    
    return err;
}

static inline int
hae_flush(haenv_t *env)
{
    haenv_entry_t *e;
    int err, ret = 0;
    
    hae_foreach(env, e) {
        if (is_haee_dirty(e)) {
            err = haee_flush(env, e);
            if (err<0) {
                ret = err;
                // todo: log
            }
        }
    }
    env->saved = env->unsaved;

    fflush(env->f);
    
    return ret;
}

static inline haenv_entry_t *
hae_find(haenv_t *env, char *k)
{
    haenv_entry_t *e, *found = NULL;
    
    hae_foreach(env, e) {
        if (os_streq(haee_key(e), k)) {
            found = e;
        }
    }

    return found;
}

static inline haenv_entry_t *
hae_append(haenv_t *env, char *k, char *v)
{
    haenv_entry_t *e = hae_empty(env);
    if (NULL==e) {
        return NULL;
    }

    int err = haee_set(e, k, v);
    if (err) {
        return NULL;
    }
    env->unsaved += haee_size(e);
    
    return e;
}

static inline jobj_t
hae_export(haenv_t *env)
{
#ifdef __APP__
    jobj_t obj = jobj_new_object();
    if (NULL==obj) {
        return NULL;
    }
    
    haenv_entry_t *e;

    hae_foreach(env, e) {
        int err = jobj_add_string(obj, haee_key(e), haee_value(e));
        if (err<0) {
            goto error;
        }
    }

    return obj;
error:
    jobj_put(obj);
#endif

    return NULL;
}

static inline int
hae_gc(haenv_t *env, jobj_t obj)
{
    int err = 0;
    
#ifdef __APP__
    char *k, *v;
    
    hae_clean(env);
    
    jobj_foreach(obj, k, v) {
        hae_append(env, k, v);
    }

    err = hae_save(env);
#endif

    return err;
}

static inline haenv_t *
__haenv_getby(bool damaged)
{
    int i;
    haenv_t *env;
    
    haenv_foreach(i, env) {
        if (damaged == env->damaged) {
            return env;
        }
    }

    return NULL;
}

static inline haenv_t *
__haenv_get_normal(void)
{
    return __haenv_getby(false);
}

static inline haenv_t *
__haenv_get_damaged(void)
{
    return __haenv_getby(true);
}

static inline void
haenv_lock(void)
{
    os_sem_lock(haenv_sem());
}

static inline void
haenv_unlock(void)
{
    os_sem_unlock(haenv_sem());
}

static inline int
haenv_load(void)
{
    int i, err, ret = 0;
    haenv_t *env;
    
    haenv_foreach(i, env) {
        err = hae_load(env);
        if (err<0) {
           ret = err;
           // todo: log
        }
    }

    return ret;
}

static inline int
haenv_check(void)
{
    int i, err, ret = 0;
    haenv_t *env;
    
    haenv_foreach(i, env) {
        hae_check(env);
        if (err<0) {
           ret = err;
           // todo: log
        }
    }

    return ret;
}

static inline int
haenv_repaire(void)
{
    haenv_t *normal = __haenv_get_normal();
    haenv_t *damaged;

    while(normal && NULL!=(damaged = __haenv_get_damaged())) {
        hae_clone(damaged, normal);
        hae_save(damaged);
    }

    return 0;
}

static inline int
haenv_flush(void)
{
    int i, err, ret = 0;
    haenv_t *env;
    
    haenv_foreach(i, env) {
        err = hae_flush(env);
        if (err<0) {
            ret = err;
            // todo: log
        }
    }
    
    return ret;
}

static inline int
haenv_append(char *k, char *v)
{
    int i, err;

    haenv_seq++;
    
    haenv_entry_t *e = hae_append(haenv_first(), k, v);
    if (NULL==e) {
        return -EFULL;
    }
    
    haenv_t *env;
    __haenv_foreach(i, env, 1) {
        if (haee_clone(hae_empty(env), e)) {
            env->unsaved += haee_size(e);
        }
    }
    
    return 0;
}

static inline haenv_entry_t *
haenv_find(char *k)
{
    return hae_find(haenv_first(), k);
}

static inline int
haenv_export(void)
{
    int err = 0;
    
#ifdef __APP__
    STREAM f = NULL;
    
    jobj_t obj = hae_export(haenv_first());
    if (NULL==obj) {
        err = -ENOMEM; goto error;
    }

    f = os_fopen(HAENV_TMPFILE, "w");
    if (NULL==f) {
        err = -EIO; goto error;
    }

    char *json = jobj_string(obj);

    err = os_fwrite(f, json, os_strlen(json));
    if (err<0) {
        goto error;
    }
    fflush(f);
    os_fclose(f);
    
    err = rename(HAENV_TMPFILE, HAENV_EXPORT);
    if (err<0) {
        err = -errno; goto error;
    }
    
error:
    jobj_put(obj);
    os_fclose(f);
#endif

    return err;
}

static inline int
haenv_gc(void)
{
    int err = 0;
    
#ifdef __APP__
    if (haenv_first()->saved > HAENV_GCSIZE) {
        jobj_t obj = hae_export(haenv_first());
        if (NULL==obj) {
            err = -ENOMEM; goto error;
        }

        err = hae_gc(haenv_first(), obj);
        if (err<0) {
            goto error;
        }

        int i;
        haenv_t *env;
        __haenv_foreach(i, env, 1) {
            hae_clone(env, haenv_first());
            hae_save(env);
        }
error:
        jobj_put(obj);
    }
#endif

    return err;
}

static inline int
haenv_init(void)
{
    int i, err;
    haenv_t *env;
    STREAM f = NULL;

#ifdef __APP__
    STREAM f = os_fopen(HAENV_FILE, "r+");
    if (NULL==f) {
        return -EIO;
    }
#endif

    err = os_sem_create(haenv_sem(), OS_HAENV_SEM_ID);
    if (err<0) {
        return err;
    }
    
    haenv_foreach(i, env) {
        env->f  = f;
        env->start = HAENV_START + i*HAENV_SIZE;
    }
    
    return 0;
}

static inline int
haenv_fini(void)
{
#ifdef __APP__
    os_fclose(haenv_first()->f);
#endif

    // os_sem_destroy(haenv_sem());
    
    return 0;
}

/******************************************************************************/
#endif /* defined(__BOOT__) || defined(__APP__) */
#endif /* __HAENV_H_2348774a939f4da0a188bca3067f44bb__ */
