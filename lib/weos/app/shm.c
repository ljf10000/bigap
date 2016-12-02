/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
void 
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

int 
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
