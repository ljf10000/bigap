#ifndef __SEM_H_a0a31796eccd4a57a41d91eea4cd2f9e__
#define __SEM_H_a0a31796eccd4a57a41d91eea4cd2f9e__
#ifdef __APP__
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
#define SEM_DPRINT              1
#endif

#if SEM_DPRINT
#define sem_println(_fmt, _args...)     printf(_fmt "\n", ##_args)
#else
#define sem_println(_fmt, _args...)     os_do_nothing()
#endif

static inline void 
__os_do_sem(int semid, int op, int undo)
{
    struct {
        uint16 sem_num;
        int16 sem_op;
        int16 sem_flag;
    } sem = {0, op, undo};

    int val;

    val = semctl(semid, 0, GETVAL, 1);
    sem_println("sem(%d) getval:%d before op", sem->id, val);
    
    semop(semid, (struct sembuf *)&sem, 1);

    val = semctl(semid, 0, GETVAL, 1);
    sem_println("sem(%d) getval:%d after op", sem->id, val);
}

static inline void 
os_sem_lock(os_sem_t *sem)
{
    sem_println("sem(%d) lock...", sem->id);
    __os_do_sem(sem->id, -1, 0/* SEM_UNDO */);
    sem_println("sem(%d) lock ok", sem->id);
}

static inline void 
os_sem_unlock(os_sem_t *sem)
{
    sem_println("sem(%d) unlock...", sem->id);
    __os_do_sem(sem->id, 1, 0);
    sem_println("sem(%d) unlock ok", sem->id);
}

static inline void 
os_sem_destroy(os_sem_t *sem)
{
    if (sem && is_good_semid(sem->id) && true == sem->owner) {
        semctl(sem->id, 0, IPC_RMID, NULL);
        
        sem_println("sem(%d) destroy", sem->id);
    }
}

static inline int 
os_sem_create(os_sem_t *sem, int key)
{
    int id = 0;
    int flags = IPC_CREAT | IPC_EXCL | 0x1FF;

    if (NULL==sem) {
        return INVALID_SEM_ID;
    }

    sem->key = key;
    sem->id = id = semget(sem->key, 1, flags);
    if (is_good_semid(id)) {
        sem->owner = true;

        int err = semctl(id, 0, SETVAL, 1);
        if (err<0) {
            os_sem_destroy(sem);
            
            sem_println(
                "sem(%d) control(SETVAL, 1) error:%d", 
                id,
                err);
            
            return INVALID_SEM_ID;
        }
        sem_println("sem(%d) SETVAL ok", sem->id);
    }
    else if (EEXIST == errno) {
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
    
    sem_println("sem(%d) %s OK!", 
        id, 
        (true == sem->owner)?"create":"get");

    return id;
}
/******************************************************************************/
#endif /* __APP__ */
#endif /* __SEM_H_a0a31796eccd4a57a41d91eea4cd2f9e__ */

