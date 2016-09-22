/* =====================================================================================
 *
 *       Filename:  update.c==串口通信程序
 *    Description:  for updating test 
 *        Version:  1.0
 *        Created:  2011年06月10日 17时57分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yao
 *        Company: 
 * =====================================================================================
 */

#include <stdio.h> /*标准输入输出定义*/
#include <stdlib.h> /*标准函数库定义*/
#include <string.h>
#include <unistd.h> /*Unix 标准函数定义*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> /*文件控制定义*/
#include <termios.h> /*PPSIX 终端控制定义*/
#include <errno.h> /*错误号定义*/
#include <pthread.h>

#include <sys/time.h>
#include <assert.h>

#define FALSE -1
#define TRUE 0

int fd;
pthread_t pid_Send;
pthread_t pid_Recv;

//设置波特率的例子函数： 
/**
*@brief 设置串口通信速率
*@param fd 类型 int 打开串口的文件句柄
*@param speed 类型 int 串口速度
*@return void
*/
int speed_arr[] = {B921600,B115200,B38400, B19200, B9600, B4800, B2400, B1200, B300};
int name_arr[] = {921600,115200,38400, 19200, 9600, 4800, 2400, 1200, 300};

void set_speed(int fd, int speed)
{
    int i;
    int status;
    struct termios Opt;
//    tcgetattr(fd, &Opt);
    if ( tcgetattr( fd,&Opt) != 0) 
    {
        printf("get serial failed\n");
        return;
    }    
    for ( i= 0; i < sizeof(speed_arr) / sizeof(int); i++) 
    {
        if (speed == name_arr[i]) 
        {
            tcflush(fd, TCIOFLUSH);
            cfsetispeed(&Opt, speed_arr[i]);
            cfsetospeed(&Opt, speed_arr[i]);
            status = tcsetattr(fd, TCSANOW, &Opt);
            if (status != 0) 
            {
                perror("tcsetattr fd1");
                return;
            }
            tcflush(fd,TCIOFLUSH);
        }
    }
    printf("speed set ok\n");
}


//设置效验的函数： 
/**
*@brief 设置串口数据位，停止位和效验位
*@param fd 类型 int 打开的串口文件句柄
*@param databits 类型 int 数据位 取值 为 7 或者8
*@param stopbits 类型 int 停止位 取值为 1 或者2
*@param parity 类型 int 效验类型 取值为N,E,O,,S
*/
int set_Parity(int fd,int databits,int stopbits,int parity)
{
    struct termios options;
    if ( tcgetattr( fd,&options) != 0) 
    {
        perror("SetupSerial 1");
        return(FALSE);
    }
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= (CLOCAL|CREAD);
    switch (databits) /*设置数据位数*/
    {
        case 7:
        options.c_cflag |= CS7;
        break;
        case 8:
        options.c_cflag |= CS8;
        break;
        default:
        fprintf(stderr,"Unsupported data sizen"); return (FALSE);
    }
    switch (parity)
    {
        case 'n':
        case 'N':
        options.c_cflag &= ~PARENB; /* Clear parity enable */
        options.c_iflag &= ~INPCK; /* Enable parity checking */
        break;
        case 'o':
        case 'O':
        options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/
        options.c_iflag |= INPCK; /* Disnable parity checking */
        break;
        case 'e':
        case 'E':
        options.c_cflag |= PARENB; /* Enable parity */
        options.c_cflag &= ~PARODD; /* 转换为偶效验*/
        options.c_iflag |= INPCK; /* Disnable parity checking */
        break;
        case 'S':
        case 's': /*as no parity*/
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;break;
        default:
        fprintf(stderr,"Unsupported parityn");
        return (FALSE);
    }
    /* 设置停止位*/
    switch (stopbits)
    {
        case 1:
        options.c_cflag &= ~CSTOPB;
        break;
        case 2:
        options.c_cflag |= CSTOPB;
        break;
        default:
        fprintf(stderr,"Unsupported stop bitsn");
        return (FALSE);
    }
    /* Set input parity option */
    if (parity != 'n')
        options.c_iflag |= INPCK;
    options.c_iflag = IGNPAR|IGNBRK;
    /* raw input */
        options.c_lflag = 0;
         /* raw ouput */
        options.c_oflag = 0;

    tcflush(fd,TCIFLUSH);
    //下面两个参数比较重要 一个设置超时时间 一个设置读取多少字节显示 
    //具体视情况而定 但是这两个参数如要用其中一个的特性 另一个需要设置为0
    //例如我读取15个字节显示 则将超时时间设置成0 即不起作用
    options.c_cc[VTIME] = 150; /* 设置超时15 seconds*/
    options.c_cc[VMIN] = 0 ; /* Update the options and do it NOW */
    if (tcsetattr(fd,TCSANOW,&options) != 0)
    {
        perror("SetupSerial 3");
        return (FALSE);
    }
    printf("parity set ok\n");
    return (TRUE);
}

 /* flush (empty) tty input and output queues */ 
void tty_flush(void)

{ 
    if(tcflush(fd, TCIOFLUSH) < 0) 
        printf("tty_flush: tcflush error"); 
}
/**********************************************************************
代码说明：16进制
**********************************************************************/
/*********************************************************************/
int OpenDev(char *Dev)
{
    int fd = open( Dev, O_RDWR| O_NOCTTY | O_NDELAY);
    if (-1 == fd)
    {
        perror("Can't Open Serial Port");
        return -1;
    }
    else
    {
        printf("dev open ok\n");
        return fd;
    }
}
/**
 * @brief
 * 第一步
 * the serial can be used       
 * @return  true  success  false  failed
 */
int update_step1(char *dev)
{
//    char *dev = "/dev/ttyUSB1"; //串口 AT  

    int ret=0;
    //step1 
    fd = OpenDev(dev);

    if(fd==-1)
    {
	printf("error! device open failed ! \n");
	return 0;
    }
   	set_speed(fd,115200);
    if (set_Parity(fd,8,1,'N') == FALSE) 
    {
        printf("Set Parity Error\n");
        return 0;
     }  
     write(fd,"AT\r",15);
     sleep(3);
     close(fd); 
     return 1;      
}

#ifndef __BUSYBOX__
#define clearTTY_main  main
#endif

/*update_step1-22*/
int clearTTY_main(int argc, char *argv[])
{
    if(!update_step1(argv[1]))
    {
        printf("step 1 update failed !\n");
	exit(0);
    }

    return  0;
}

