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
#define HAENV_COUNT                 3
#endif

#ifndef HAENV_SIZE
#define HAENV_SIZE                  (64*1024)
#endif
#define HAENV_SIZE_MASK             (HAENV_SIZE-1)

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
#else /* offset of file(/dev/bootenv) */
#   define HAENV_START              HAENV_BOOTENV_SIZE
#endif
#endif

#ifndef HAENV_ROOT
#define HAENV_ROOT                  "/dev/mmcblk0p14"
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

typedef struct {
    byte md5[HAENV_MD5_SIZE];
    byte used;
    byte klen;
    uint16 vlen;
    uint32 seq;
    
    char data[0];
} haenv_entry_t;

typedef struct {
    char mirror[HAENV_SIZE];
    
    uint32 saved;   // size: have saved to flash
    uint32 unsaved; // size: not unsaved to flash
} haenv_t;

typedef struct {
    haenv_t env[1+HAENV_COUNT];

    STREAM f;
} haenv_file_t;

#define HAENV_TEMP  HAENV_COUNT

extern haenv_file_t haenv_file;

#define haenv_foreach(_i)   for (_i=0; _i<HAENV_COUNT; _i++)

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

static inline haenv_entry_t *
haee_next(haenv_entry_t *e)
{
    return (haenv_entry_t *)(e->data + haee_dsize(e));
}

/*
* zone as [begin, end)
*/
static inline bool
is_good_haenv_zone(uint32 begin, uint32 end)
{
    return  begin < end && begin < HAENV_SIZE && end <= HAENV_SIZE;
}

static inline bool
__is_good_haenv_entryEx(haenv_entry_t *e)
{
    // md5 check

    return e->used;
}

static inline bool
__is_good_haenv_entry(haenv_t *env, haenv_entry_t *e)
{
    if ((char *)e < env->mirror) {
        /*
        * e's begin < mirror's begin
        */
        return false;
    }
    else if ((char *)e + haee_size(e) > env->mirror + HAENV_SIZE) {
        /*
        * e' end > mirror's end
        */
        return false;
    }
    else {
        return true;
    }
}

static inline bool
is_good_haenv_entry(haenv_t *env, haenv_entry_t *e)
{
    return __is_good_haenv_entryEx(e) && __is_good_haenv_entry(env, e);
}

static inline haenv_entry_t *
__haenv_entry_first(haenv_t *env)
{
    haenv_entry_t *e = (haenv_entry_t *)env->mirror;

    return __is_good_haenv_entry(env, e)?e:NULL;
}

static inline haenv_entry_t *
__haenv_entry_next(haenv_t *env, haenv_entry_t *current)
{
    haenv_entry_t *e = haee_next(current);
    
    return __is_good_haenv_entry(env, e)?e:NULL;
}

static inline haenv_entry_t *
__haenv_entry_empty(haenv_t *env)
{
    haenv_entry_t *e = (haenv_entry_t *)(env->mirror + env->size);

    return __is_good_haenv_entry(env, e)?e:NULL;
}

static inline haenv_entry_t *
haenv_entry_first(haenv_t *env)
{
    haenv_entry_t *e = (haenv_entry_t *)env->mirror;

    return is_good_haenv_entry(env, e)?e:NULL;
}

static inline haenv_entry_t *
haenv_entry_next(haenv_t *env, haenv_entry_t *current)
{
    haenv_entry_t *e = haee_next(current);
    
    return is_good_haenv_entry(env, e)?e:NULL;
}

static inline haenv_entry_t *
haenv_entry_empty(haenv_t *env)
{
    haenv_entry_t *e = (haenv_entry_t *)(env->mirror + env->size);

    return is_good_haenv_entry(env, e)?e:NULL;
}

static inline haenv_entry_t *
haenv_entry_append(haenv_t *env, haenv_entry_t *new)
{
    haenv_entry_t *e = haenv_entry_empty(env);
    
    os_memcpy(e, new, haee_size(new));
    
    return e;
}

static inline hash_idx_t
haenv_cache_hash(hash_node_t *node)
{
    haenv_cache_t *c = HAENV_CACHE(node);

    return os_str_bkdr(haee_key(c->entry));
}

static inline bool
haenv_cache_eq(hash_node_t *node, char *key)
{
    haenv_cache_t *c = HAENV_CACHE(node);

    return os_strcmp(key, haee_key(c->entry));
}

static inline haenv_cache_t *
haenv_cache_find(haenv_t *env, char *key)
{
    bool eq(hash_node_t *node)
    {
        return haenv_cache_eq(node, key);
    }

    return h1_find(&env->table, haenv_cache_hash, eq);
}

static inline haenv_cache_t *
__haenv_cache_merge(haenv_cache_t *c, haenv_entry_t *e)
{
    if (c->entry->seq < e->seq) {
        c->entry = e;
    }
    
    return c;
}

static inline haenv_cache_t *
__haenv_cache_new(haenv_t *env, haenv_entry_t *e)
{
    haenv_cache_t *c = (haenv_cache_t *)os_zalloc(sizeof(*c));
    if (c) {
        c->entry = e;
        
        int err = h1_add(&env->table, &c->node, haenv_cache_hash);
        if (err<0) {
            return NULL;
        }
    }

    return c;
}

static inline haenv_cache_t *
__haenv_cache_load(haenv_t *env, haenv_entry_t *e)
{
    haenv_cache_t *c = haenv_cache_find(env, haee_key(e));
    
    return c?__haenv_cache_merge(c, e):__haenv_cache_new(env, e);
}

static inline int
haenv_cache_load(haenv_t *env)
{
    haenv_entry_t *e;

    for (e = haenv_entry_first(env); e; e = haenv_entry_next(env, e)) {

    }
}

static inline int
haenv_clear(haenv_t *env)
{
    mv_t foreach(h1_node_t *node)
    {
        haenv_cache_t *c = HAENV_CACHE(node);

        h1_del(&env->table, node);

        os_free(c);
    }
    
    int err = h1_foreach_safe(&env->table, foreach);
    if (0==err) {
        env->read_size = 0;
        env->size = 0;
    }

    return err;
}

static inline int
haenv_clone(haenv_t *dst, haenv_t *src)
{
    mv_t foreach(h1_node_t *node)
    {
        haenv_cache_t *c = HAENV_CACHE(node);

        
    }
    
    return h1_foreach_safe(&src->table, foreach);
}

static inline int
__haenv_init(haenv_file_t *ha)
{
    int i, err;

    haenv_foreach(i) {
        haenv_t *env = &ha->env[i];

        err = h1_init(&env->table);
        if (err<0) {
            return err;
        }
    }
    
    return 0;
}

static inline uint32
haenv_offset(uint32 id, uint32 begin)
{
    return id * HAENV_SIZE + begin;
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
haenv_fread(STREAM f, uint32 begin, byte *buf, uint32 size)
{    
    return benv_emmc_read(begin + HAENV_START, buf, size);
}

/*
* begin:  haenv file offset
*/
static inline int
haenv_fwrite(STREAM f, uint32 begin, byte *buf, uint32 size)
{
    return benv_emmc_write(begin + HAENV_START, buf, size);
}

static inline int
haenv_init(haenv_file_t *ha)
{
    return __haenv_init(ha);
}

static inline int
haenv_fini(haenv_file_t *ha)
{
    return 0;
}
#else
/*
* begin: haenv file offset
*/
static inline int
haenv_fread(STREAM f, uint32 begin, byte *buf, uint32 size)
{
    uint32 start = begin + HAENV_START;

    int err = os_fseek(f, start, SEEK_SET);
    if (err<0) {
        return err;
    }
    
    return os_fread(start, buf, size);
}

/*
* begin:  haenv file offset
*/
static inline int
haenv_fwrite(STREAM f, uint32 begin, byte *buf, uint32 size)
{
    uint32 start = begin + HAENV_START;

    int err = os_fseek(f, start, SEEK_SET);
    if (err<0) {
        return err;
    }
    
    return benv_emmc_write(start, buf, size);
}

static inline int
haenv_init(haenv_file_t *ha)
{
    STREAM f = os_fopen();
    if (NULL==f) {
        return -EIO;
    }

    ha->f = f;

    return __haenv_init(ha);
}

static inline int
haenv_fini(haenv_file_t *ha)
{
    os_fclose(ha->f);

    return 0;
}
#endif

/*
* begin:  haenv offset
*/
static inline int
haenv_read(STREAM f, uint32 id, uint32 begin, byte *buf, uint32 size)
{
    if (false==is_good_haenv_zone(begin, begin + size)) {
        return -ERANGE;
    }
    
    return haenv_fread(haenv_offset(id, begin), buf, size);
}

/*
* begin:  haenv offset
*/
static inline int
haenv_write(STREAM f, uint32 id, uint32 begin, byte *buf, uint32 size)
{
    if (false==is_good_haenv_zone(begin, begin + size)) {
        return -ERANGE;
    }
    
    return haenv_fwrite(haenv_offset(id, begin), buf, size);
}

static inline int
haenv_load(haenv_file_t *ha)
{
    int i, err;
    haenv_t *env;
    
    haenv_foreach(i) {
        env = &ha->env[i];
        
        err = haenv_read(ha->f, i, 0, env->mirror, HAENV_SIZE);
        if (err<0) {
            return err;
        }


        
    }

    return 0;
}

/******************************************************************************/
#endif /* defined(__BOOT__) || defined(__APP__) */
#endif /* __HAENV_H_2348774a939f4da0a188bca3067f44bb__ */
