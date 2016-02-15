/*******************************************************************************
Copyright (c) 2015-2016, xxx Networks. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      rshd
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     "net"
#endif

#include "rshd.h"
/******************************************************************************/
static int
__disconnect(void)
{
    os_close(rsh.fd);
}

static int
__connect(void)
{
    int fd = INVALID_FD;
    int err = 0;

    __disconnect();
    
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd<0) {
        log_error("socket error:%d", -errno);
        
        err = -errno; goto error;
    }
    os_closexec(fd);
    os_noblock(fd);
    
    err = connect(fd, &rsh.server, sizeof(rsh.server));
    if (err) {
        err = -errno; goto error;
    }
    
    rsh.fd = fd;
    
    return 0;
error:
    os_close(fd);

    return err;
}

static int
__register(void)
{
    char json[1+OS_LINE_LEN] = {0};
    int err;
    
    os_pgets(json, OS_LINE_LEN, SCRIPT_RIGSTER);
    
    return err;
}

static int
__ping(void)
{
    int err;
    
    err = rsh_msg_cache_send_cmd(rsh.out, rsh.id++, RSH_CMD_ECHO);

    return err;
}

/******************************************************************************/
