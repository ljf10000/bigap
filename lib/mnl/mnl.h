#ifndef __MNL_H_eb44de3b09944e11999a887b380b1a33__
#define __MNL_H_eb44de3b09944e11999a887b380b1a33__
/******************************************************************************/
#include "utils.h"
#define MNL_MAGIC       "MNL"
#define MNL_VERSION     1

enum {
    MNL_P_MULTICAST,

    MNL_P_END
};

struct mnlmsghdr {
    char        magic[3];
    uint8_t     protocol;

    union {
        struct {
            uint8_t     version;
            uint8_t     reserved8;
            uint16_t    group; /* dst_group */
            uint32_t    spid;
            uint32_t    reserved32;
        } m;
    } u;
};
/******************************************************************************/
#endif /* __MNL_H_eb44de3b09944e11999a887b380b1a33__ */
