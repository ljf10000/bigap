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
static void
__nsq_timer(time_t now)
{

}

static int
nsq_timer(loop_watcher_t *watcher, time_t now)
{
    __nsq_timer(now);
    
    return 0;
}

int
init_nsq_timer(void)
{
    struct itimerspec iticks = OS_ITIMESPEC_INITER(os_second(nsqa.ticks), os_usecond(nsqa.ticks));
    int err;
    
    err = os_loop_add_timer(&nsqa.loop, nsq_timer, &iticks);
    if (err<0) {
        debug_error("add loop timer error:%d", err);
        
        return err;
    }

    return 0;
}

/******************************************************************************/
