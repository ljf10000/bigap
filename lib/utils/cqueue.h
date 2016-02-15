#ifndef __CQUEUE_H_04b4815f18564b8799c9ffda96d7e97d__
#define __CQUEUE_H_04b4815f18564b8799c9ffda96d7e97d__
/******************************************************************************/
enum {
    CQUEUE_F_BLOCK      = 0x01,
};

typedef struct {
    uint32_t flag;
    
    channel_t *ch;
    
    void (*free)(void *q);
} cqueue_t;

static inline bool
cq_is_block(cqueue_t *cq)
{
    return os_hasflag(cq->flag, CQUEUE_F_BLOCK);
}

static inline int
cq_init(cqueue_t *cq, int count, uint32_t flag)
{
    cq->ch = os_pch_new(count);
    if (NULL==cq->ch) {
        return -ENOMEM;
    }
    
    cq->flag = flag;
    
    return 0;
}

static inline void
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

static inline int
cq_get(cqueue_t *cq, uint32_t idx, void **pointer)
{
    return os_pch_get(cq->ch, idx, pointer);
}

static inline int
cq_set(cqueue_t *cq, uint32_t idx, void *pointer)
{
    void *old = NULL;
    
    cq_get(cq, idx, &old);
    if (old && cq->free) {
        (*cq->free)(old);
    }
    
    return os_pch_set(cq->ch, idx, pointer);
}

static inline int
cq_read(cqueue_t *cq, void **pointer)
{
    return os_pch_read(cq->ch, pointer);
}

static inline int
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
#endif /* __CQUEUE_H_04b4815f18564b8799c9ffda96d7e97d__ */
