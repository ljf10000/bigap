#ifndef __HAENV_H_2348774a939f4da0a188bca3067f44bb__
#define __HAENV_H_2348774a939f4da0a188bca3067f44bb__
#include "utils.h"
#if defined(__BOOT__) || defined(__APP__)
/******************************************************************************/
#ifndef HAENV_DPRINT
#define HAENV_DPRINT                0
#endif

#if HAENV_DPRINT
#define haenv_debug(_fmt, _args...) do{ \
    os_printf("%s ", __func__);         \
    os_println(_fmt, ##_args);          \
}while(0);
#else
#define haenv_debug(_fmt, _args...)     os_do_nothing()
#endif
/******************************************************************************/
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
        __space                 \
    /* end */

#define CONFIG_BOOTARGS_BODY    \
    "mem=2G"                    \
        __space                 \
    "console=ttyAMA0,115200"    \
        __space                 \
    "rootfstype=ext4"           \
        __space                 \
    "rootwait"                  \
        __space                 \
    HAENV_ROOTFS_MODE           \
        __space                 \
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
        __space                 \
    "mmz=ddr,0,0,300M"          \
    /* end */

#ifdef CONFIG_BOOTARGS
#undef CONFIG_BOOTARGS
#endif

#define CONFIG_BOOTARGS         \
    CONFIG_BOOTARGS_HEAD        \
    CONFIG_BOOTARGS_BODY        \
    /* end */

#define HAENV_ALIGN(_len)       OS_ALIGN(_len, sizeof(uint32))

#define HAENV_F_DIRTY           0x01

typedef struct {
    byte    md5[16];
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
    uint32  id;
    uint32  seq;
    
    uint32  saved;  // size: have saved to flash
    uint32  unsaved;// size: not unsaved to flash
    uint32  count;
    
    bool    damaged;
} haenv_t;

typedef struct {
    haenv_t env[HAENV_COUNT];

#ifdef __APP__
    os_sem_t sem;
#endif
    
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
__hae_read(haenv_t *env, uint32 begin, void *buf, uint32 size)
{
    return benv_emmc_read(env->start + begin, buf, size);
}

/*
* begin:  haenv file offset
*/
static inline int
__hae_write(haenv_t *env, uint32 begin, void *buf, uint32 size)
{
    return benv_emmc_write(env->start + begin, buf, size);
}
#else
/*
* begin: haenv file offset
*/
static inline int
__hae_read(haenv_t *env, uint32 begin, void *buf, uint32 size)
{
    int err = os_fseek(env->f, env->start + begin, SEEK_SET);
    if (err<0) {
        haenv_debug("seek error:%d", err);
        
        return err;
    }
    
    err = os_fread(env->f, buf, size);
    if (err<0) {
        haenv_debug("read error:%d", err);
        
        return err;
    }

    return err;
}

/*
* begin:  haenv file offset
*/
static inline int
__hae_write(haenv_t *env, uint32 begin, void *buf, uint32 size)
{
    int err = os_fseek(env->f, env->start + begin, SEEK_SET);
    if (err<0) {
        haenv_debug("seek error:%d", err);
        
        return err;
    }
    
    err = os_fwrite(env->f, buf, size);
    if (err<0) {
        haenv_debug("write error:%d", err);
        
        return err;
    }
    
    return err;
}
#endif

/*
* begin:  haenv offset
*/
static inline int
hae_read(haenv_t *env, uint32 begin, void *buf, uint32 size)
{
    if (false==is_good_haenv_zone(begin, begin + size)) {
        haenv_debug("error: bad zone[0x%x, 0x%x)", begin, begin+size);
        
        return -ERANGE;
    }
    
    int err = __hae_read(env, begin, buf, size);
    if (err<0) {
        haenv_debug("read error:%d", err);
        
        return err;
    }

    return err;
}

/*
* begin:  haenv offset
*/
static inline int
hae_write(haenv_t *env, uint32 begin, void *buf, uint32 size)
{
    if (false==is_good_haenv_zone(begin, begin + size)) {
        haenv_debug("error: bad zone[0x%x, 0x%x)", begin, begin+size);
        
        return -ERANGE;
    }
    
    int err = __hae_write(env, begin, buf, size);
    if (err<0) {
        haenv_debug("read error:%d", err);
        
        return err;
    }

    return err;
}

static inline uint32
hae_offsetof(haenv_t *env, haenv_entry_t *e)
{
    return (uint32)pointer_offsetof(e, env);
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

static inline byte *
haee_md5_begin(haenv_entry_t *e)
{
    return &e->md5[16];
}

static inline uint32
haee_md5_size(haenv_entry_t *e)
{
    return haee_size(e) - 16;
}

static inline void
__haee_md5(haenv_t *env, haenv_entry_t *e, byte md5[16])
{
    md5_content_t ctx;
    
    md5_encode(&ctx, md5, haee_md5_begin(e), haee_md5_size(e));
}

static inline void
haee_md5(haenv_t *env, haenv_entry_t *e)
{
    __haee_md5(env, e, e->md5);
}

static inline bool
__is_good_haee(haenv_t *env, haenv_entry_t *e)
{
    return e >= hae_first(env) && haee_next(e) <= hae_end(env);
}

static inline bool
is_empty_haee(haenv_entry_t *e)
{
    haenv_entry_t zero = {.flag = 0};
    
    return os_objeq(&zero, e);
}

static inline bool
is_good_empty_haee(haenv_t *env, haenv_entry_t *e)
{
    return is_empty_haee(e)
        && e >= hae_first(env) 
        && ((haenv_entry_t *)(e->data + 2*sizeof(uint32))) < hae_end(env);
}

static inline bool
is_good_haee_md5(haenv_t *env, haenv_entry_t *e)
{
    byte md5[16];
    
    __haee_md5(env, e, md5);

    haenv_debug("env[%d] offset==>0x%x md5", 
        env->id, 
        hae_offsetof(env, e));
    os_dump_line(0, md5, 16, NULL);
    os_dump_line(0, e->md5, 16, NULL);
    
    return md5_eq(md5, e->md5);
}

static inline bool
is_good_haee(haenv_t *env, haenv_entry_t *e)
{
    if (is_empty_haee(e)) {
        haenv_debug("env[%d] offset==>0x%x is empty", 
            env->id, 
            hae_offsetof(env, e));
        
        return false;
    } else if (false==is_good_haee_md5(env, e)) {
        haenv_debug("env[%d] offset==>0x%x is bad md5", 
            env->id, 
            hae_offsetof(env, e));
        return false;
    } else {
        return true;
    }
    
    //return false==is_empty_haee(e) && is_good_haee_md5(env, e);
}

static inline int
haee_set(haenv_t *env, haenv_entry_t *e, char *k, char *v, bool dirty)
{
    if (e) {
        e->klen = os_strlen(k);
        e->vlen = os_strlen(v);
        e->seq  = haenv_seq;
        
        os_strmcpy(haee_key(e), k, e->klen);
        os_strmcpy(haee_value(e), v, e->vlen);
        haee_pad_zero(e);
        
        /*
        * set zero before make md5
        */
        e->flag = 0;
        haee_md5(env, e);
        /*
        * set dirty after make md5
        */
        if (dirty) {
            e->flag = HAENV_F_DIRTY;
        }
        
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
    /*
    * 1. set un-dirty
    * 2. save flag
    */
    byte flag = os_clrflag(e->flag, HAENV_F_DIRTY); e->flag = 0;

    /*
    * 3. write env
    */
    int err = hae_write(env, hae_offsetof(env, e), e, haee_size(e));

    /*
    * 4. restore flag
    */
    e->flag = flag;

    return err;
}

#define __haenv_foreach(_i, _env, _begin)   for (_i=_begin, _env = haenv_env(_i); _i<HAENV_COUNT; _i++, _env = haenv_env(_i))
#define haenv_foreach(_i, _env)             __haenv_foreach(_i, _env, 0)

static inline haenv_entry_t *
hae_next(haenv_t *env, haenv_entry_t *current)
{
    haenv_entry_t *e = haee_next(current);
    
    return __is_good_haee(env, e)?e:NULL;
}

#define hae_foreach(_env, _e)       for (_e = hae_first(_env); _e; _e = hae_next(_env, _e))

static inline haenv_entry_t *
hae_empty(haenv_t *env)
{
    haenv_entry_t *e = (haenv_entry_t *)(env->mirror + env->unsaved);
    
    haenv_debug("env[%d] empty offset==>0x%x", 
        env->id, 
        hae_offsetof(env, e));

    return is_good_empty_haee(env, e)?e:NULL;
}

static inline haenv_t *
hae_clone(haenv_t *dst, haenv_t *src)
{
    os_memcpy(dst->mirror, src->mirror, sizeof(src->mirror));

    dst->seq    = src->seq;
    dst->count  = src->count;
    dst->saved  = src->saved;
    dst->unsaved= src->unsaved;
    dst->damaged= src->damaged;
    
    return dst;
}

static inline void
hae_clean(haenv_t *env)
{
    os_memzero(env->mirror, sizeof(env->mirror));

    /*
    * id/start NOT clean
    */
    env->seq    = 0;
    env->count  = 0;
    env->saved  = 0;
    env->unsaved= 0;
    env->damaged= false;
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

    os_fflush(env->f);

    return err;
}

static inline int
hae_check(haenv_t *env)
{
    haenv_entry_t *e;
    int err = 0;
    
    hae_foreach(env, e) {
        if (is_empty_haee(e)) {
            haenv_debug("env[%d] empty count==>%d, offset==>0x%x, saved==>0x%x", 
                env->id, 
                env->count, 
                hae_offsetof(env, e), 
                env->saved);
                
            break;
        }
        else if (is_good_haee(env, e)) {
            env->saved += haee_size(e);

            haenv_debug("env[%d] good count==>%d, offset==>0x%x, saved==>0x%x", 
                env->id, 
                env->count, 
                hae_offsetof(env, e), 
                env->saved);
#if HAENV_DPRINT
            __os_dump_buffer(e, haee_size(e), NULL);
#endif

            env->count++;
            env->seq = e->seq;
        }
        else {
            haenv_debug("env[%d] damaged offset==>0x%x", 
                env->id, 
                hae_offsetof(env, e));
#if HAENV_DPRINT
            __os_dump_buffer(e, haee_size(e), NULL);
#endif
            
            env->damaged = true;
            
            err = -EDAMAGED;
        }
    }

    env->unsaved = env->saved;
    haenv_debug("env[%d] unsaved==>0x%x", env->id, env->unsaved);
    
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
    haenv_debug("env[%d] saved==>0x%x", env->id, env->unsaved);
    
    os_fflush(env->f);
    
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
hae_append(haenv_t *env, char *k, char *v, bool dirty)
{
    haenv_entry_t *e = hae_empty(env);
    if (NULL==e) {
        return NULL;
    }

    int err = haee_set(env, e, k, v, dirty);
    if (err) {
        return NULL;
    }
    env->unsaved += haee_size(e);
    haenv_debug("env[%d] unsaved==>0x%x", env->id, env->unsaved);
    
    return e;
}

#ifdef __APP__
static inline jobj_t
hae_export(haenv_t *env)
{
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

    return NULL;
}

static inline int
hae_gc(haenv_t *env, jobj_t obj)
{
    int err = 0;
    
    hae_clean(env);
    
    jobj_foreach(obj, k, v) {
        haenv_entry_t *e = hae_append(env, k, jobj_get_string(v), false);

        /*
        * set zero before save
        */
        e->flag = 0;
    }

    err = hae_save(env);

    return err;
}
#endif

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
#define __haenv_get_normal()    __haenv_getby(false)
#define __haenv_get_damaged()   __haenv_getby(true)

static inline void
haenv_lock(void)
{
#ifdef __APP__
    haenv_debug("...");
    os_sem_lock(haenv_sem());
    haenv_debug("ok");
#endif
}

static inline void
haenv_unlock(void)
{
#ifdef __APP__
    haenv_debug("...");
    os_sem_unlock(haenv_sem());
    haenv_debug("ok");
#endif
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

    haenv_seq = haenv_first()->seq;
    
    return ret;
}

static inline int
haenv_repaire(void)
{
    haenv_t *normal = __haenv_get_normal();
    if (NULL==normal) {
        return -EDAMAGED;
    }
    
    haenv_t *damaged;
    while(NULL!=(damaged = __haenv_get_damaged())) {
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
    
    haenv_entry_t *e = hae_append(haenv_first(), k, v, true);
    if (NULL==e) {
        return -EFULL;
    }
    
    haenv_t *env;
    __haenv_foreach(i, env, 1) {
        if (haee_clone(hae_empty(env), e)) {
            env->unsaved += haee_size(e);
            
            haenv_debug("env[%d] unsaved==>0x%x", env->id, env->unsaved);
        }
    }
    
    return 0;
}

static inline haenv_entry_t *
haenv_find(char *k)
{
    return hae_find(haenv_first(), k);
}

#ifdef __APP__
static inline int
haenv_export(void)
{
    int err     = 0;
    STREAM f    = NULL;
    jobj_t obj  = NULL;
    char *json  = NULL;
    
    obj = hae_export(haenv_first());
    if (NULL==obj) {
        err = -ENOMEM; goto error;
    }

    f = os_fopen(HAENV_TEMP, "w");
    if (NULL==f) {
        err = -EIO; goto error;
    }

    json = jobj_json(obj);
    err = os_fwrite(f, json, os_strlen(json));
    if (err<0) {
        goto error;
    }
    os_fclose(f);
    
    err = rename(HAENV_TEMP, HAENV_EXPORT);
    if (err<0) {
        err = -errno; goto error;
    }

error:
    jobj_put(obj);
    os_fclose(f);

    return err;
}

static inline int
haenv_gc(bool force)
{
    int err = 0;

    if (false==force && haenv_first()->saved < HAENV_GCSIZE) {
        return 0;
    }

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

    return err;
}
#endif

static inline int
haenv_clean(void)
{
    int i, err;
    haenv_t *env;

    haenv_foreach(i, env) {
        hae_clean(env);
        hae_save(env);
    }
    
    return 0;
}

static inline int
haenv_init(void)
{
    int i, err;
    haenv_t *env;
    STREAM f = NULL;

#ifdef __APP__
    f = os_fopen(HAENV_FILE, "r+");
    if (NULL==f) {
        return -EIO;
    }

    haenv_debug("open " HAENV_FILE);

    err = os_sem_create(haenv_sem(), OS_HAENV_SEM_ID);
    if (err<0) {
        return err;
    }
    haenv_debug("create sem");
#else /* __BOOT__ */
    __THIS_HAENV = (haenv_file_t *)os_zalloc(haenv_file_t);
    if (NULL==__THIS_HAENV) {
        return -ENOMEM;
    }
#endif

    haenv_foreach(i, env) {
        env->f      = f;
        env->id     = (uint32)i;
        env->start  = HAENV_START + i*HAENV_SIZE;
        
        haenv_debug("env[%d] start at 0x%x", i, env->start);
    }
    
    return 0;
}

static inline int
haenv_fini(void)
{
#ifdef __APP__
    os_fclose(haenv_first()->f);
    haenv_debug("close " HAENV_FILE);
    
    /*
    * NEEDN't destroy
    *
    * os_sem_destroy(haenv_sem());
    */
#endif

    
    return 0;
}

/******************************************************************************/
#endif /* defined(__BOOT__) || defined(__APP__) */
#endif /* __HAENV_H_2348774a939f4da0a188bca3067f44bb__ */
