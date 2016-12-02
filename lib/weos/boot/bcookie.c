/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
DECLARE void
bcookie_fake(struct bcookie *bc)
{
    bc->id[BCOOKIE_COUNT/2]++;
    bc->id[BCOOKIE_COUNT/2]--;
}

DECLARE void
bcookie_cid_dump(uint32 cid[4])
{
    int i;

    for (i=0; i<4; i++) {
        os_println("CID[%d]:%d", i, cid[i]);
    }

    os_println("MID:%d", bcookie_cid_mid(cid));
    os_println("PSN:%d", bcookie_cid_psn(cid));
}

DECLARE int
bcookie_find(byte *mem, int size, int id)
{
    struct bcookie bc = BCOOKIE(id);
    if (size < sizeof(bc)) {
        return -ETOOSMALL;
    }
    
    byte *p = (byte *)os_memmem(mem, size, &bc, sizeof(bc));
    if (NULL==p) {
        debug_error("no-found bcookie:%d", id);
        
        return -ENOEXIST;
    }

    return p - mem;
}

#if PRODUCT_BCOOKIE_ENABLE
#ifdef __BOOT__
DECLARE int
__bcookie_check(uint32 begin, uint32 size, struct bcookie *obj, uint32 osize)
{
    if (osize<=sizeof(struct bcookie)) {
        return os_assertV(-EINVAL5);
    }
    else if (size<osize) {
        return os_assertV(-EINVAL4);
    }
    else {
        return 0;
    }
}

DECLARE int
__bcookie_load(int begin, int size, struct bcookie *obj, int osize)
{
    int err = 0, offset = INVALID_VALUE;
    int id = BCOOKIE_ID(obj);
    byte *mem = NULL;

    err = __bcookie_check(begin, size, obj, osize);
    if (err<0) {
        return err;
    }

    mem = (byte *)os_malloc(size);
    if (NULL==mem) {
        err = -ENOMEM; goto error;
    }

    if (size != benv_emmc_read(begin, mem, size)) {
        debug_error("read emmc error:%d", -errno);
        
        err = -errno; goto error;
    }
    
    offset = bcookie_find(mem, size, id);
    if (offset<0) { /* yes, <0 */
        err = offset; goto error;
    }

    os_memcpy(obj, mem + offset, osize);
    
error:
    if (mem) {
        os_free(mem);
    }
    
    return err;
}

DECLARE int
__bcookie_save(int begin, int size, struct bcookie *obj, int osize)
{
    int err = 0, offset = INVALID_VALUE;
    int id = BCOOKIE_ID(obj);
    byte *mem = NULL;
    
    err = __bcookie_check(begin, size, obj, osize);
    if (err<0) {
        return err;
    }

    mem = (byte *)os_malloc(size);
    if (NULL==mem) {
        err = -ENOMEM; goto error;
    }
    
    if (size != benv_emmc_read(begin, mem, size)) {
        os_println("read emmc error:%d", -errno);
        
        err = -errno; goto error;
    }
    
    offset = bcookie_find(mem, size, id);
    if (offset<0) { /* yes, <0 */
        err = offset; goto error;
    }
    os_memcpy(mem + offset, obj, osize);

    uint32 block_begin = os_align_down(offset, BENV_BLOCK_SIZE)/BENV_BLOCK_SIZE;
    uint32 block_end   = OS_ALIGN(offset + osize - 1, BENV_BLOCK_SIZE)/BENV_BLOCK_SIZE;
    uint32 block_count = block_end - block_begin + 1;
    
    if (block_count * BENV_BLOCK_SIZE != benv_emmc_write(begin + block_begin * BENV_BLOCK_SIZE, 
                    mem + block_begin * BENV_BLOCK_SIZE, 
                    block_count * BENV_BLOCK_SIZE)) {
        os_println("write emmc error:%d", -errno);

        err = -errno; goto error;
    }
    
error:
    if (mem) {
        os_free(mem);
    }
    
    return err;
}

DECLARE int
bcookie_load(struct bcookie *obj, int size)
{
    return __bcookie_load(0, PRODUCT_BOOT_SIZE, obj, size);
}

DECLARE int
bcookie_save(struct bcookie *obj, int size)
{
    return __bcookie_save(0, PRODUCT_BOOT_SIZE, obj, size);
}

#endif
#endif
/******************************************************************************/
