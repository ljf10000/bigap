#ifndef __TTYUSB_H_2d9ec51fce224bc793537797ce1f4d47__
#define __TTYUSB_H_2d9ec51fce224bc793537797ce1f4d47__
/******************************************************************************/
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
/******************************************************************************/
static inline void serial_init(int fd)
{
	struct termios options;
	tcgetattr(fd, &options);
	options.c_cflag |= ( CLOCAL | CREAD );
	options.c_cflag &= ~CSIZE;
	options.c_cflag &= ~CRTSCTS;
	options.c_cflag |= CS8;
	options.c_cflag &= ~CSTOPB;
	options.c_iflag |= IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;

//	cfsetispeed(&options, B9600);
//	cfsetospeed(&options, B9600);
	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);
	tcsetattr(fd,TCSANOW,&options);

	tcflush(fd, TCIOFLUSH);
}

static inline int send(int fd, char *cmdbuf)
{
	int nread,nwrite;
	char buff[128];
	char reply[128];
	char *pbuf = buff;
	(void)cmdbuf;
	memset(buff,0,sizeof(buff));
	pbuf += sprintf(pbuf, "%s", cmdbuf);
	pbuf += sprintf(pbuf, "\r");


	nwrite = write(fd,buff,strlen(buff));

	sleep(1);
	memset(reply,0,sizeof(reply));
	nread = read(fd,reply,sizeof(reply));
	printf("Response(%d):%s\n",nread,reply);

	return 0;
}

static inline int ttyUSB_H_main(int argc, char *argv[])
{
	int fd;
	char cmdbuf[128];
	int no;
	char dev[128] = {0};

	if (argv[1] == NULL) {
		printf("Error: ttyUSB [number]\n");
		exit(1);
	}
	
	no = strtoul(argv[1], NULL, 10);
	sprintf(dev, "%s%d", TTYUSB, no);

	fd = open(dev, O_RDWR|O_NOCTTY|O_NDELAY);
	if (-1 != fd)
	{
		if (argv[2] == NULL)
		{
			printf("Usage: ttyUSB [AT command]\n");
		}
		else
		{	
			serial_init(fd);
			printf("Input %s=", argv[2]);
			send(fd, argv[2]);
		}
		close(fd);
	}
	else 
		perror("Can't Open Serial Port");

	return 0;
}
/******************************************************************************/
#endif /* __TTYUSB_H_2d9ec51fce224bc793537797ce1f4d47__ */
