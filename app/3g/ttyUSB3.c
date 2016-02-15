#define TTYUSB "/dev/ttyUSB3"

#include "ttyUSB.h"

#ifndef __BUSYBOX__
#define ttyUSB3_main  main
#endif

int ttyUSB3_main(int argc, char *argv[])
{
	return ttyUSB_main(argc, argv);
}
