/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      rsha
#endif
    
#ifndef __THIS_FILE
#define __THIS_FILE     proto
#endif
    
#define __DEAMON__
#include "rsha.h"
/******************************************************************************/
int 
rsha_send(rsh_instance_t *instance)
{
    rsh_msg_t *msg = rsha_msg;
    int len = msg->len, err;

    rsh_msg_hton(msg);
    
    err = io_sendto(instance->fd, msg, len, &instance->server, sizeof(instance->server));
    if (err<0) {
        return err;
    }
    
    return 0;
}

int 
rsha_recver(loop_watcher_t *watcher, time_t now)
{
    rsh_instance_t *instance = (rsh_instance_t *)watcher->user;

    return 0;
}

int 
rsha_echo(rsh_instance_t *instance)
{
    rsh_msg_t *msg = rsha_msg;

    os_objzero(msg);

    msg->cmd    = RSH_CMD_ECHO;
    msg->flag   = RSH_F_SYN;
    msg->seq    = instance->seq++;

    rsh_msg_bfill(msg, rsha.basemac, NULL, 0);
    
    return rsha_send(instance);
}

int 
rsha_ack(rsh_instance_t *instance)
{
    rsh_msg_t *msg = rsha_msg;

    msg->flag   = RSH_F_ACK;
    msg->seq    = 0;
    msg->ack    = instance->seq_peer;
    msg->error  = 0;

    rsh_msg_bfill(msg, rsha.basemac, NULL, 0);
    
    return 0;
}

/******************************************************************************/
