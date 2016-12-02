#ifndef __BCOOKIE_H_c0ee689aca594ee09d9e8f2223369c41__
#define __BCOOKIE_H_c0ee689aca594ee09d9e8f2223369c41__
#include "benv/benv.h"
/******************************************************************************/
enum {
    BCOOKIE_OTP     = 538050839,
    BCOOKIE_CID,
};

enum {
    BCOOKIE_COUNT   = 7,
};

struct bcookie {
    uint32 id[BCOOKIE_COUNT];
};

#define BCOOKIE(_id)    {   \
    .id = {                 \
        [0] = 0x01330235,   \
        [1] = 0x2012dead,   \
        [2] = 0xf00d1e00,   \
        [3] = _id,          \
        [4] = 0x00e1d00f,   \
        [5] = 0xdaed2102,   \
        [6] = 0x53023310,   \
    },                      \
}   /* end */

#define BCOOKIE_ID(_obj)    (_obj)->id[BCOOKIE_COUNT/2]
#define BCOOKIE_OBJ(_id)    { .header = BCOOKIE(_id) }

extern void
bcookie_fake(struct bcookie *bc);

struct bcookie_otp {
    struct bcookie header;
    byte custom[16];
    byte private[16];
};

struct bcookie_cid {
    struct bcookie header;
    uint32 cid[4];
};

#define bcookie_cid_psn(_cid)   (((_cid[2] & 0xffff) << 16) | (_cid[3] >> 16))
#define bcookie_cid_mid(_cid)   (_cid[0] >> 24)

extern void
bcookie_cid_dump(uint32 cid[4]);

extern int
bcookie_find(byte *mem, int size, int id);

#if PRODUCT_BCOOKIE_ENABLE
extern int
bcookie_load(struct bcookie *obj, int size);

extern int
bcookie_save(struct bcookie *obj, int size);
#endif /* PRODUCT_BCOOKIE_ENABLE */
/******************************************************************************/
#endif /* __BCOOKIE_H_c0ee689aca594ee09d9e8f2223369c41__ */
