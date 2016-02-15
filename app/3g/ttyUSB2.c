#define TTYUSB "/dev/ttyUSB2"

#include "ttyUSB.h"

#ifndef __BUSYBOX__
#define ttyUSB2_main  main
#endif

int ttyUSB2_main(int argc, char *argv[])
{
	return ttyUSB_main(argc, argv);
}
