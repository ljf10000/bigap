/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
DECLARE bool
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

DECLARE int
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

DECLARE int
os_bitmap_fini(os_bitmap_t *bp)
{
    if (bp) {
        bp->count = 0;
        
        os_free(bp->maps);
    }

    return 0;
}

DECLARE void
os_bitmap_set(os_bitmap_t *bp, uint32 bit)
{
    if (bp && __is_good_bitmap_bit(bp, bit)) {
        uint32 __bit = bit%BITMAPSLOT;
        
        os_setbit(bp->maps[bit/BITMAPSLOT], __bit);
    }
}

DECLARE void
os_bitmap_clr(os_bitmap_t *bp, uint32 bit)
{
    if (bp && __is_good_bitmap_bit(bp, bit)) {
        uint32 __bit = bit%BITMAPSLOT;
        
        os_clrbit(bp->maps[bit/BITMAPSLOT], __bit);
    }
}

DECLARE bool
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
