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
STATIC int 
rsha_send(rsh_instance_t *instance)
{
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
    return 0;
}

int 
rsha_ack(rsh_instance_t *instance)
{
    return 0;
}

/******************************************************************************/
