#ifndef __KSLICE_H_bf993a740c234b43bbc68f79402884ed__
#define __KSLICE_H_bf993a740c234b43bbc68f79402884ed__
/******************************************************************************/
#include "slice/slice.h"

#define SLICE_LOCAL_FOR_RECV_BY_SKB(_skb) {   \
    .len    = (_skb)->len,          \
    .size   = (_skb)->len + skb_tailroom(_skb), \
    .resv   = skb_headroom(_skb),   \
    .head   = (_skb)->data,         \
}

#define SLICE_LOCAL_FOR_SEND_BY_SKB(_skb) \
        __SLICE_INITER((_skb)->data, (_skb)->len + skb_tailroom(_skb), ATNL_HSIZE, true)
/******************************************************************************/
#endif /* __KSLICE_H_bf993a740c234b43bbc68f79402884ed__ */
