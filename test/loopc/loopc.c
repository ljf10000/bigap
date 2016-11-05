/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      loopc
#endif

#include "utils.h"

OS_INITER;

#define BUFFERSIZE 15

static struct {
    os_sockaddr_t server;
    os_sockaddr_t client;

    char buf[1+BUFFERSIZE];
} loopc = {
    .server = OS_SOCKADDR_INITER(AF_UNIX),
    .client = OS_SOCKADDR_ABSTRACT_INITER("loopc"),
};

static int
client(int type, char *path, char *msg)
{
    int fd, err, len;
    
    set_abstract_path(&loopc.server.un, path);
    
    fd = socket(AF_UNIX, type, 0);
    if (fd<0) {
        debug_error("socket error:%d", -errno);
        
        return -errno;
    }
    
    err = bind(fd, (sockaddr_t *)&loopc.client.c, get_abstract_sockaddr_len(&loopc.client.un));
    if (err<0) {
        __debug_error("bind error:%d", -errno);
        return -errno;
    }

    err = connect(fd, (sockaddr_t *)&loopc.server.c, get_abstract_sockaddr_len(&loopc.server.un));
    if (err<0) {
        debug_error("connect error:%d", -errno);
        return -errno;
    }

    len = os_strlen(msg);
    err = io_send(fd, msg, len);
    if (err<0) { /* yes, <0 */
        return err;
    }
    debug_cli("send repuest[%d]:%s", len, msg);

    while(1) {
        len = __io_recv(fd, loopc.buf, BUFFERSIZE);
        if (BUFFERSIZE==len) {
            loopc.buf[len] = 0;
            os_printf("%s", loopc.buf);
        }
        else if (len>0) {
            loopc.buf[len] = 0;
            os_println("%s", loopc.buf);

            return 0;
        }
        else if (0==len) {
            os_println("peer closed");

            return 0;
        }
        else if (len<0) {
            os_println("err=%d, errno=%d", len, -errno);

            return -errno;
        }
    }
    
    return 0;
}

static int
__main(int argc, char *argv[])
{
    int err;
    
    if (3!=argc) {
        return -EINVAL0;
    }

    char *type = argv[1];
    char *msg = argv[2];
    
    if (0==os_strcmp("tcp", type)) {
        err = client(SOCK_STREAM, OS_UNIX_PATH("loops.tcp"), msg);
    }
    else if (0==os_strcmp("udp", type)) {
        err = client(SOCK_DGRAM, OS_UNIX_PATH("loops.udp"), msg);
    }
    else {
        return -EINVAL0;
    }

    return err;
}

int main(int argc, char *argv[])
{ 
    int err = os_main(__main, argc, argv);

    return shell_error(err);
}

/******************************************************************************/
