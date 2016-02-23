#ifndef __SHM_H_9251a716aabe4602b0c492b50399af54__
#define __SHM_H_9251a716aabe4602b0c492b50399af54__
#ifdef __APP__
/******************************************************************************/
#define INVALID_SHM_ID      INVALID_COMMON_ID
#define INVALID_SHM_ADDR    ((void *)(-1))
#define INVALID_SHM_SIZE    0

typedef struct {
    int id;
    unsigned int size;
    unsigned int key;
    void *address;
} os_shm_t;

#define OS_SHM_INITER(_key) {   \
    .id     = INVALID_SHM_ID,   \
    .key    = _key,             \
    .address= INVALID_SHM_ADDR, \
}

#ifndef SHM_DPRINT
#define SHM_DPRINT              0
#endif

#if SHM_DPRINT
#define shm_println(_fmt, _args...)     printf(_fmt "\n", ##_args)
#else
#define shm_println(_fmt, _args...)     os_do_nothing()
#endif

static inline void 
os_shm_init(os_shm_t *shm, unsigned int key)
{
    shm->id = INVALID_SHM_ID;
    shm->key = key;
    shm->address = INVALID_SHM_ADDR;
}

static inline void 
os_shm_destroy(os_shm_t *shm)
{
    if (shm && INVALID_SHM_ADDR != shm->address) {
        shmdt(shm->address);
        shm->address = INVALID_SHM_ADDR;
        
        shm_println("shm destroy(key:%#x, size:%#x, id:%d, address:%p)", 
            shm->key,
            shm->size,
            shm->id,
            shm->address);
    }
}

static inline int 
os_shm_create(os_shm_t *shm, unsigned int size, bool readonly)
{
    int flags = IPC_CREAT | (readonly?SHM_RDONLY:0) | 0x1FF;
    size_t pagesize;
    
    if (NULL==shm) {
        return -EINVAL1;
    }
    else if (INVALID_SHM_ADDR != shm->address) {
        return shm->id;
    }
    
    pagesize = getpagesize();
    shm_println("pagesize=%d", pagesize);
    
    shm->size = OS_ALIGN(size, pagesize);
    shm->id = shmget(shm->key, shm->size, flags);
    if (false==is_good_shmid(shm->id)) {
        shm_println(
            "shm create(key:%#x, size:%#x, flags:%#x) error:%d", 
            shm->key,
            shm->size,
            flags,
            -errno);
        
        return -errno;
    } else {
        shm_println(
            "shm create(key:%#x, size:%#x, flags:%#x) id:%d", 
            shm->key,
            shm->size,
            flags,
            shm->id);
    }
    
    shm->address = shmat(shm->id, NULL, 0);
    if (INVALID_SHM_ADDR == shm->address) {
        shm_println(
            "shm map(key:%#x, size:%#x, flags:%#x, id:%d) error:%d", 
            shm->key,
            shm->size,
            flags,
            shm->id,
            -errno);
        
        return -errno;
    } else {
        shm_println(
            "shm map(key:%#x, size:%#x, flags:%#x, id:%d) address:%p", 
            shm->key,
            shm->size,
            flags,
            shm->id,
            shm->address);
    }
    
    return shm->id;
}
/******************************************************************************/
#endif /* __APP__ */
#endif /* __SHM_H_9251a716aabe4602b0c492b50399af54__ */

