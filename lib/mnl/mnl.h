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
    byte     protocol;

    union {
        struct {
            uint8     version;
            uint8     reserved8;
            uint16    group; /* dst_group */
            uint32    spid;
            uint32    reserved32;
        } m;
    } u;
};
/******************************************************************************/
#endif /* __MNL_H_eb44de3b09944e11999a887b380b1a33__ */
