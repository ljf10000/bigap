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
    typeof(_align) __align_in___os_align = _align; \
                                        \
    __OS_ALIGN(_x, __align_in___os_align); \
})
#endif

#ifndef __OS_ALIGN_DOWN
#define __OS_ALIGN_DOWN(_x, _align)     ((((_x)+(_align)-1)/(_align) - 1)*(_align))
#endif

#ifndef __os_align_down
#define __os_align_down(_x, _align) ({  \
    typeof(_align) __align_in___os_align_down = _align; \
                                        \
    __OS_ALIGN_DOWN(_x, __align_in___os_align_down); \
})
#endif

#ifndef OS_ALIGN
#define OS_ALIGN(_x, _align)            (((_x)+(_align)-1) & ~((_align)-1))
#endif

#ifndef os_align
#define os_align(_x, _align)        ({  \
    typeof(_align) __align_in_os_align = _align; \
                                        \
    OS_ALIGN(_x, __align_in_os_align);  \
})
#endif

#ifndef os_align_down
#define os_align_down(_x, _align)       ((_x) & ~((_align)-1))
#endif

#define os_power_align(_x)  ({  \
    typeof(_x) __x_in_os_power_align = (_x);  \
    typeof(_x) __y_in_os_power_align = 0; \
                            \
    if (__x_in_os_power_align > 0) { \
        int __bits = 1;     \
                            \
        while((__x_in_os_power_align>>=1)) { \
            __bits++;       \
        }                   \
                            \
        __y_in_os_power_align = 1<<__bits; \
    }                       \
                            \
    __y_in_os_power_align;  \
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
    typeof(_flag) __flag_in_os_bitshift = (_flag); \
    typeof(_flag) __mask_in_os_bitshift; \
    int __pos = 0;                  \
                                    \
    if (__flag_in_os_bitshift) {    \
        do {                        \
            __mask_in_os_bitshift = (1UL<<__pos); \
            __pos++;                \
        }while(!(__flag_in_os_bitshift & __mask_in_os_bitshift)); \
    }                               \
                                    \
    __pos-1;                        \
})
#endif

static inline bool
os_bmask_match(byte *a, byte *b, byte *mask, int len)
{
    int i;

    for (i=0; i<len; i++) {
        if (false==os_mask_match(a[i], b[i], mask[i])) {
            return false;
        }
    }

    return true;
}

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

static inline int
os_bitmap_init(os_bitmap_t *bp, uint32 bits)
{
    uint32 count = OS_ALIGN(bits, BITMAPSLOT)/BITMAPSLOT;
    
    bp->maps = (uint32 *)os_zalloc(count * sizeof(uint32));
    if (NULL==bp->maps) {
        return -ENOMEM;
    }
    bp->count = count;

    return 0;
}

static inline int
os_bitmap_fini(os_bitmap_t *bp)
{
    if (bp) {
        bp->count = 0;
        
        os_free(bp->maps);
    }
}

static inline void
os_bitmap_set(os_bitmap_t *bp, uint32 bit)
{
    if (bp && __is_good_bitmap_bit(bp, bit)) {
        uint32 __bit = bit%BITMAPSLOT;
        
        os_setbit(bp->maps[bit/BITMAPSLOT], __bit);
    }
}

static inline void
os_bitmap_clr(os_bitmap_t *bp, uint32 bit)
{
    if (bp && __is_good_bitmap_bit(bp, bit)) {
        uint32 __bit = bit%BITMAPSLOT;
        
        os_clrbit(bp->maps[bit/BITMAPSLOT], __bit);
    }
}

static inline bool
os_bitmap_isset(os_bitmap_t *bp, uint32 bit)
{
    if (bp && __is_good_bitmap_bit(bp, bit)) {
        uint32 __bit = bit%BITMAPSLOT;
        
        return os_hasbit(bp->maps[bit/BITMAPSLOT], __bit);
    } else {
        return false;
    }
}
/******************************************************************************/
#endif /* __BITS_H_472ca09446fa4bc389460b4391206b0f__ */
