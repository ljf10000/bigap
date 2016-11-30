#ifndef __AUTOARRAY_H_8c73d7a7887b4bfc95a89f91321059f2__
#define __AUTOARRAY_H_8c73d7a7887b4bfc95a89f91321059f2__
/******************************************************************************/
typedef struct {
    byte *base;     /* array memory */
    uint32 size;    /* item size */
    uint32 count;   /* item total count */
    uint32 limit;   /* item total limit */
    uint32 grow;    /* once grow count */
    
    void (*init)(void *item);
    void (*clean)(void *item);
} autoarray_t;

extern void
os_aa_clean(autoarray_t *aa);

extern int
os_aa_init(
    autoarray_t *aa, 
    uint32 size, 
    uint32 count,
    uint32 limit, 
    uint32 grow, 
    void (*init)(void *item),
    void (*clean)(void *item)
);

extern void *
os_aa_get(autoarray_t *aa, int idx, bool grow);

static inline uint32
os_aa_count(autoarray_t *aa)
{
    return aa?aa->count:os_assertV(0);
}
/******************************************************************************/
#endif /* __AUTOARRAY_H_8c73d7a7887b4bfc95a89f91321059f2__ */
