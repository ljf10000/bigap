#ifndef __CQUEUE_H_04b4815f18564b8799c9ffda96d7e97d__
#define __CQUEUE_H_04b4815f18564b8799c9ffda96d7e97d__
#ifdef __EXTEND__
/******************************************************************************/
typedef struct {
    uint32 flag;
    
    channel_t *ch;
    
    void (*free)(void *q);
} cqueue_t;

extern bool
cq_is_block(cqueue_t *cq);

extern int
cq_init(cqueue_t *cq, int count, uint32 flag);

extern void
cq_fini(cqueue_t *cq);

extern int
cq_get(cqueue_t *cq, uint32 idx, void **pointer);

extern int
cq_set(cqueue_t *cq, uint32 idx, void *pointer);

extern int
cq_read(cqueue_t *cq, void **pointer);

extern int
cq_write(cqueue_t *cq, void *pointer);
/******************************************************************************/
#endif
#endif /* __CQUEUE_H_04b4815f18564b8799c9ffda96d7e97d__ */
