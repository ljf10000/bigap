#ifndef __CQUEUE_H_04b4815f18564b8799c9ffda96d7e97d__
#define __CQUEUE_H_04b4815f18564b8799c9ffda96d7e97d__
/******************************************************************************/
#ifndef USE_MOD_CQUEUE
#define USE_MOD_CQUEUE          PC_VAL(1, 0)
#endif

typedef struct {
    uint32 flag;
    
    channel_t *ch;
    
    void (*free)(void *q);
} cqueue_t;

enum {
    CQUEUE_F_BLOCK      = 0x01,
};

EXTERN bool
cq_is_block(cqueue_t *cq);

EXTERN int
cq_init(cqueue_t *cq, int count, uint32 flag);

EXTERN void
cq_fini(cqueue_t *cq);

EXTERN int
cq_get(cqueue_t *cq, uint32 idx, void **pointer);

EXTERN int
cq_set(cqueue_t *cq, uint32 idx, void *pointer);

EXTERN int
cq_read(cqueue_t *cq, void **pointer);

EXTERN int
cq_write(cqueue_t *cq, void *pointer);

#ifdef __BOOT__
#include "weos/boot/cqueue.c"
#endif
/******************************************************************************/
#endif /* __CQUEUE_H_04b4815f18564b8799c9ffda96d7e97d__ */
