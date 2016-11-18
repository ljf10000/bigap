/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
enum {
    CQUEUE_F_BLOCK      = 0x01,
};

bool
cq_is_block(cqueue_t *cq)
{
    return os_hasflag(cq->flag, CQUEUE_F_BLOCK);
}

int
cq_init(cqueue_t *cq, int count, uint32 flag)
{
    cq->ch = os_pch_new(count);
    if (NULL==cq->ch) {
        return -ENOMEM;
    }
    
    cq->flag = flag;
    
    return 0;
}

void
cq_fini(cqueue_t *cq)
{
    if (cq->free) {
        void *pointer = NULL;
        
        while(0==os_pch_read(cq->ch, &pointer)) {
            (*cq->free)(pointer);
        }
    }

    os_ch_free(cq->ch);
}

int
cq_get(cqueue_t *cq, uint32 idx, void **pointer)
{
    return os_pch_get(cq->ch, idx, pointer);
}

int
cq_set(cqueue_t *cq, uint32 idx, void *pointer)
{
    void *old = NULL;
    
    cq_get(cq, idx, &old);
    if (old && cq->free) {
        (*cq->free)(old);
    }
    
    return os_pch_set(cq->ch, idx, pointer);
}

int
cq_read(cqueue_t *cq, void **pointer)
{
    return os_pch_read(cq->ch, pointer);
}

int
cq_write(cqueue_t *cq, void *pointer)
{
    int err;
    bool droped = false;

retry:
    err = os_pch_write(cq->ch, pointer);
    if (err && false==cq_is_block(cq) && false==droped) {
        void *drop = NULL;
        int ret = cq_read(cq, &drop);
        if (ret) {
            return err;
        }
        
        if (cq->free) {
            (*cq->free)(drop);
        }

        droped = true;

        goto retry;
    }

    return err;
}

/******************************************************************************/
