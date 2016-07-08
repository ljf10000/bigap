#define TTYUSB "/dev/ttyUSB"

#include "ttyUSB.h"

#ifndef __BUSYBOX__
#define ttyUSB_main  main
#endif

int ttyUSB_main(int argc, char *argv[])
{
	return ttyUSB_H_main(argc, argv);
}
