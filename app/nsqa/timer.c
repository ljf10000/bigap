/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      nsqa
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     timer
#endif

#define __DEAMON__
#include "nsqa.h"
/******************************************************************************/
struct itimerspec nsq_ticks = OS_ITIMESPEC_INITER(NSQ_TICKS, 0);

static int
nsq_timer(loop_watcher_t *watcher, uint32 times)
{
    return 0;
}

int
init_nsq_timer(void)
{
    int err;
    
    err = os_loop_add_timer(&nsqa.loop, nsq_timer, &nsq_ticks);
    if (err<0) {
        debug_error("add loop timer error:%d", err);
        
        return err;
    }

    return 0;
}

/******************************************************************************/
