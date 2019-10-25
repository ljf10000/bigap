#ifndef __SEM_H_a0a31796eccd4a57a41d91eea4cd2f9e__
#define __SEM_H_a0a31796eccd4a57a41d91eea4cd2f9e__
/******************************************************************************/
#define INVALID_SEM_ID      INVALID_COMMON_ID

typedef struct {
    int id;
    uint32 key;
    bool owner;
} os_sem_t;

#define OS_SEM_INITER(_key) {   \
    .id     = INVALID_SEM_ID,   \
    .key    = _key,             \
    .owner  = false,            \
}

#ifndef SEM_PRINT
#define SEM_PRINT               0
#endif

#if SEM_PRINT
#define sem_println(_fmt, _args...)     os_println(_fmt, ##_args)
#else
#define sem_println(_fmt, _args...)     os_do_nothing()
#endif

#ifdef __BOOT__
#define os_sem_lock(_sem)               os_do_nothing()
#define os_sem_unlock(_sem)             os_do_nothing()
#define os_sem_destroy(_sem)            os_do_nothing()
#define os_sem_create(_sem, _key)       0
#else
extern void
os_sem_lock(os_sem_t *sem);

extern void 
os_sem_unlock(os_sem_t *sem);

extern void 
os_sem_destroy(os_sem_t *sem);

extern int 
os_sem_create(os_sem_t *sem, int key);
#endif
/******************************************************************************/
#endif /* __SEM_H_a0a31796eccd4a57a41d91eea4cd2f9e__ */
