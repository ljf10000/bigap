#ifndef __SHM_H_9251a716aabe4602b0c492b50399af54__
#define __SHM_H_9251a716aabe4602b0c492b50399af54__
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

#ifdef __BOOT__
#define os_shm_destroy(_shm)    os_do_nothing()
#define os_shm_create(_shm)     0
#else
extern void 
os_shm_destroy(os_shm_t *shm);

extern int 
os_shm_create(os_shm_t *shm, unsigned int size, bool readonly);
#endif
/******************************************************************************/
#endif /* __SHM_H_9251a716aabe4602b0c492b50399af54__ */

