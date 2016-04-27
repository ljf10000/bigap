/******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      zlib
#endif

#include "utils.h"
#include "zlib.h"

OS_INITER;

#include "zlib.pacp.c"

struct pktinfo {
    char *pkt;
    int len;
};

#define PKT(x)  [x] = {
    .pkt = pkt##x,
    .len = sizeof(pkt##x),
}

static struct pktinfo pkt[] = {
    PKT(1), PKT(2), PKT(3), PKT(4), PKT(5), PKT(6), PKT(7), PKT(8), PKT(9), 
    PKT(10), PKT(11), PKT(12), PKT(13), PKT(14), PKT(15), PKT(16), PKT(17), PKT(18), PKT(19), 
    PKT(20), PKT(21), PKT(22), PKT(23), PKT(24), PKT(25), PKT(26), PKT(27), PKT(28), PKT(29), 
    PKT(30), PKT(31), PKT(32), PKT(33), PKT(34), PKT(35), PKT(36), PKT(37), PKT(38), PKT(39), 
    PKT(40), PKT(41), PKT(42), PKT(43), PKT(44), PKT(45), PKT(46), PKT(47), PKT(48), PKT(49), 
    PKT(50), PKT(51), PKT(52), PKT(53), PKT(54), PKT(55), PKT(56), PKT(57), PKT(58), PKT(59), 
    PKT(60), PKT(61), PKT(62), PKT(63), PKT(64), PKT(65), PKT(66), PKT(67), PKT(68), PKT(69), 
    PKT(70), PKT(71), PKT(72), PKT(73), PKT(74), PKT(75), PKT(76), PKT(77), PKT(78), PKT(79), 
    PKT(80), PKT(81), PKT(82), PKT(83), PKT(84), PKT(85), PKT(86), PKT(87), PKT(88), PKT(89), 
    PKT(90), PKT(91), PKT(92), PKT(93), PKT(94), PKT(95), PKT(96), PKT(97), PKT(98), PKT(99), 
};

static 

int __main(int argc, char *argv[])
{
    byte output[2048];
    int i, level, err = 0;
    unsigned long output_len = sizeof(output);

    for (i=0; i<os_count_of(pkt); i++) {
        for (level=Z_BEST_SPEED; level<=Z_BEST_COMPRESSION; level++) {
            err = compress2(output, &output_len, pkt[i].pkt, pkt[i].len, level);

            os_println("input:%d, level:%d, output:%d",
                pkt[i].len,
                level, 
                output_len);
        }
    }
    
    return 0;
}


int main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(__main, argc, argv);
}
/******************************************************************************/
