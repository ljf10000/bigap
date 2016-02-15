#define TTYUSB "/dev/ttyUSB1"

#include "ttyUSB.h"

#ifndef __BUSYBOX__
#define ttyUSB1_main  main
#endif

int ttyUSB1_main(int argc, char *argv[])
{
	return ttyUSB_main(argc, argv);
}
