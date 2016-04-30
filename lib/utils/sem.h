#ifndef __SEM_H_a0a31796eccd4a57a41d91eea4cd2f9e__
#define __SEM_H_a0a31796eccd4a57a41d91eea4cd2f9e__
/******************************************************************************/
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

#ifndef SEM_DPRINT
#define SEM_DPRINT              0
#endif

#if SEM_DPRINT
#define sem_println(_fmt, _args...)     printf(_fmt "\n", ##_args)
#else
#define sem_println(_fmt, _args...)     os_do_nothing()
#endif

static inline void 
__os_do_sem(int semid, int op, int undo)
{
#ifdef __APP__
    struct {
        unsigned short sem_num;
        short sem_op;
        short sem_flag;
    } sem = {0, op, undo};
    
    semop(semid, (struct sembuf *)&sem, 1);
#endif
}

static inline void 
os_sem_lock (os_sem_t *sem)
{
#ifdef __APP__
    __os_do_sem(sem->id, -1, SEM_UNDO);
#endif
}

static inline void 
os_sem_unlock(os_sem_t *sem)
{
#ifdef __APP__
    __os_do_sem(sem->id, 1, 0);
#endif
}

static inline void 
os_sem_destroy(os_sem_t *sem)
{
#ifdef __APP__
    if (sem && is_good_semid(sem->id) && true == sem->owner) {
        semctl(sem->id, 0, IPC_RMID, NULL);
        
        sem_println("sem(%d) destroy", sem->id);
    }
#endif
}

static inline int 
os_sem_create(os_sem_t *sem, int key)
{
    int id = 0;
    
#ifdef __APP__
    int flags = IPC_CREAT | IPC_EXCL | 0x1FF;

    if (NULL==sem) {
        return INVALID_SEM_ID;
    }

    sem->key = key;
    sem->id = id = semget(sem->key, 1, flags);
    if (false==is_good_semid(id)){
        if (EEXIST == errno) {
            flags = IPC_CREAT | 0x1FF;
            sem->id = id = semget(sem->key, 1, flags);
            if (false==is_good_semid(id)){ 
                sem_println(
                    "sem get(key:%#x, flags:%#x) error:%d", 
                    sem->key,
                    flags,
                    -errno);
                
                return INVALID_SEM_ID;
            }
        } else {
            sem_println(
                "sem create (key:%#x, flags:%#x) error:%d.", 
                sem->key,
                flags,
                -errno);
            
            return INVALID_SEM_ID;
        }
    }
    else {
        sem->owner = true;
    }
    
    if (true == sem->owner) {
        int ret = semctl(id, 0, SETVAL, 1);
        
        if (ret<0) {
            os_sem_destroy(sem);
            
            sem_println(
                "sem(%d) control(SETVAL, 1) error:%d", 
                id,
                ret);
            
            return INVALID_SEM_ID;
        }
    }
    
    sem_println("sem(%d) %s OK!", 
        id, 
        (true == sem->owner)?"create":"get");
#endif

    return id;
}
/******************************************************************************/
#endif /* __SEM_H_a0a31796eccd4a57a41d91eea4cd2f9e__ */

