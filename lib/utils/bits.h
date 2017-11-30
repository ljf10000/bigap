#ifndef __BITS_H_472ca09446fa4bc389460b4391206b0f__
#define __BITS_H_472ca09446fa4bc389460b4391206b0f__
/******************************************************************************/
#ifndef OS_ALIGN_SIZE
#define OS_ALIGN_SIZE                   sizeof(void *)
#endif

#ifndef __OS_ALIGN
#define __OS_ALIGN(_x, _align)          ((((_x)+(_align)-1)/(_align))*(_align))
#endif

#ifndef __os_align
#define __os_align(_x, _align)      ({  \
    typeof(_align) m_align = _align;    \
                                        \
    __OS_ALIGN(_x, m_align);            \
})
#endif

#ifndef OS_ALIGN
#define OS_ALIGN(_x, _align)            (((_x)+_align-1) & ~(_align-1))
#endif

#ifndef os_align
#define os_align(_x, _align)        ({  \
    typeof(_align) m_align = _align;    \
                                        \
    OS_ALIGN(_x, m_align);              \
})
#endif

#ifndef os_align_down
#define os_align_down(_x, _align)       ((_x) & ~((_align)-1))
#endif

#define os_power_align(_x)  ({  \
    typeof(_x) m_x = (_x);  \
    typeof(_x) m_y = 0;     \
                            \
    if (m_x > 0) {          \
        int m_bits = 1;     \
                            \
        while((m_x>>=1)) {  \
            m_bits++;       \
        }                   \
                            \
        m_y = 1<<m_bits;    \
    }                       \
                            \
    m_y;                    \
})  /* end */

#ifndef os_setflag
#define os_setflag(_value, _flag)       ((_value) |= (_flag))
#endif

#ifndef os_clrflag
#define os_clrflag(_value, _flag)       ((_value) &= ~(_flag))
#endif

#ifndef os_hasflag
#define os_hasflag(_value, _flag)       ((_flag) == ((_value) & (_flag)))
#endif

#ifndef os_noflag
#define os_noflag(_value, _flag)        (!os_hasflag(_value, _flag))
#endif

#ifndef os_bit
#define os_bit(_bit)                    (1<<(_bit))
#endif

#ifndef os_mask
#define os_mask(_bit)                   (os_bit(_bit) - 1)
#endif

#ifndef os_mask_match
#define os_mask_match(_a, _b, _mask)    (((_a) & (_mask))!=((_b) & (_mask)))
#endif

#ifndef os_setbit
#define os_setbit(_value, _bit)         os_setflag(_value, os_bit(_bit))
#endif

#ifndef os_clrbit
#define os_clrbit(_value, _bit)         os_clrflag(_value, os_bit(_bit))
#endif

#ifndef os_hasbit
#define os_hasbit(_value, _bit)         os_hasflag(_value, os_bit(_bit))
#endif

#ifndef os_nobit
#define os_nobit(_value, _bit)          (!os_hasbit(_value, _bit))
#endif

#ifndef os_bitshift
#define os_bitshift(_flag)      ({  \
    typeof(_flag) m_flag = (_flag); \
    typeof(_flag) m_mask;           \
    int m_pos = 0;                  \
                                    \
    if (m_flag) {                   \
        do {                        \
            m_mask = (1UL<<m_pos);  \
            m_pos++;                \
        }while(!(m_flag & m_mask)); \
    }                               \
                                    \
    m_pos-1;                        \
})
#endif

EXTERN bool
os_bmask_match(byte *a, byte *b, byte *mask, int len);

enum {
    BITMAPSLOT  = (sizeof(uint32)*8),
};

typedef struct {
    uint32 *maps;
    uint32 count;
} os_bitmap_t;

static inline bool
__is_good_bitmap_bit(os_bitmap_t *bp, uint32 bit)
{
    return IS_GOOD_ENUM(bit, bp->count);
}

EXTERN int
os_bitmap_init(os_bitmap_t *bp, uint32 bits);

EXTERN int
os_bitmap_fini(os_bitmap_t *bp);

EXTERN void
os_bitmap_set(os_bitmap_t *bp, uint32 bit);

EXTERN void
os_bitmap_clr(os_bitmap_t *bp, uint32 bit);

EXTERN bool
os_bitmap_isset(os_bitmap_t *bp, uint32 bit);
/******************************************************************************/
enum {
    OS_POSITION_HEAD    = 0x01,
    OS_POSITION_TAIL    = 0x02,
    OS_POSITION_ALL     = (OS_POSITION_HEAD | OS_POSITION_TAIL),
};

#define is_position_head(_pos)  os_hasflag(_pos, OS_POSITION_HEAD)
#define is_position_tail(_pos)  os_hasflag(_pos, OS_POSITION_TAIL)

#ifdef __BOOT__
#include "weos/boot/bits.c"
#endif
/******************************************************************************/
#endif /* __BITS_H_472ca09446fa4bc389460b4391206b0f__ */
