/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      umd
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     timer
#endif

#define __DEAMON__
#include "umd.h"

STATIC int
umd_timer(loop_watcher_t *watcher, time_t now)
{
    umd.ticks++;
    
    umd_user_timer(watcher, now);
    umd_conn_timer(watcher, now);

    return 0;
}

STATIC int
umd_timer_server_init(sock_server_t *server)
{
    struct itimerspec tm = OS_ITIMESPEC_INITER(os_second(1000*umd.cfg.ticks), os_nsecond(1000*umd.cfg.ticks));
    int err;

    err = os_loop_add_timer(&umd.loop, umd_timer, &tm);
    if (err<0) {
        debug_error("add loop timer error:%d", err);
        
        return err;
    }
    
    debug_ok("init timer ok");
    
    return 0;
}

sock_server_t umd_timer_server = 
    SOCK_SERVER_INITER(UMD_SERVER_TIMER, 0, umd_timer_server_init, NULL);
/******************************************************************************/
