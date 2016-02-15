#define TTYUSB "/dev/ttyUSB0"

#include "ttyUSB.h"

#ifndef __BUSYBOX__
#define ttyUSB0_main  main
#endif

int ttyUSB0_main(int argc, char *argv[])
{
	return ttyUSB_main(argc, argv);
}
