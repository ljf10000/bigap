/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#if PRODUCT_BCOOKIE_ENABLE
#ifdef __APP__
STATIC int
__bcookie_check(char *file, uint32 fsize, struct bcookie *obj, uint32 osize)
{
    if (NULL==file) {
        return os_assertV(-EINVAL8);
    }
    else if (NULL==obj) {
        return os_assertV(-EINVAL7);
    }
    else if (osize<=sizeof(struct bcookie)) {
        return os_assertV(-EINVAL5);
    }
    else if (fsize<osize) {
        return os_assertV(-EINVAL4);
    }
    else {
        return 0;
    }
}

STATIC int
__bcookie_load(char *file, int fsize, struct bcookie *obj, int osize)
{
    int fd = INVALID_FD, err = 0, offset = INVALID_VALUE;
    int id = BCOOKIE_ID(obj);
    byte *mem = NULL;

    err = __bcookie_check(file, fsize, obj, osize);
    if (err<0) {
        return err;
    }

    debug_trace("before open %s", file);
    
    fd = __os_file_lock(file, O_RDONLY, S_IRUSR | S_IWUSR, true);
    if (false==is_good_fd(fd)) {
        err = -errno; goto error;
    }
    
    mem = (byte *)os_malloc(fsize);
    if (NULL==mem) {
        err = -ENOMEM; goto error;
    }

    if (fsize != read(fd, mem, fsize)) {
        debug_error("read boot error:%d", -errno);
        
        err = -errno; goto error;
    }
    
    offset = bcookie_find(mem, fsize, id);
    if (offset<0) { /* yes, <0 */
        err = offset; goto error;
    }

    os_memcpy(obj, mem + offset, osize);
    
error:
    __os_file_unlock(fd);
    if (mem) {
        os_free(mem);
    }
    
    return err;
}

STATIC int
__bcookie_save(char *file, int fsize, struct bcookie *obj, int osize)
{
    int fd = INVALID_FD, err = 0, offset = INVALID_VALUE;
    int id = BCOOKIE_ID(obj);
    byte *mem = NULL;
    
    err = __bcookie_check(file, fsize, obj, osize);
    if (err<0) {
        return err;
    }
    
    fd = __os_file_lock(file, O_RDWR, S_IRUSR | S_IWUSR, true);
    if (false==is_good_fd(fd)) {
        err = -errno; goto error;
    }
    
    mem = (byte *)os_malloc(fsize);
    if (NULL==mem) {
        err = -ENOMEM; goto error;
    }
    
    if (fsize != read(fd, mem, fsize)) {
        debug_error("read boot error:%d", -errno);
        
        err = -errno; goto error;
    }
    
    offset = bcookie_find(mem, fsize, id);
    if (offset<0) { /* yes, <0 */
        err = offset; goto error;
    }
    
    err = lseek(fd, offset, SEEK_SET);
    if (err < 0) {        /* <0 is error */
        debug_error("seek boot error:%d", -errno);

        err = -errno; goto error;
    }
    
    if (osize != write(fd, obj, osize)) {
        debug_error("write boot error:%d", -errno);

        err = -errno; goto error;
    }
    
error:
    __os_file_unlock(fd);
    if (mem) {
        os_free(mem);
    }
    
    return err;
}

DECLARE int
bcookie_load(struct bcookie *obj, int size)
{
    return __bcookie_load(PRODUCT_DEV_BOOT, PRODUCT_BOOT_SIZE, obj, size);
}

DECLARE int
bcookie_save(struct bcookie *obj, int size)
{
    return __bcookie_save(PRODUCT_DEV_BOOT, PRODUCT_BOOT_SIZE, obj, size);
}

#endif
#endif
/******************************************************************************/
