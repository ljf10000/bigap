/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      rsha
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     timer
#endif

#define __DEAMON__
#include "rsha.h"
/******************************************************************************/
typedef void rsha_timer_handle_f(rsh_instance_t *instance, time_t now);

STATIC void
rsha_init_tigger(rsh_instance_t *instance, time_t now)
{
    if (is_rsh_fsm_init(instance)) {
        rsh_register(instance, now);    // ==> registered
    }
}

STATIC void
rsha_registered_tigger(rsh_instance_t *instance, time_t now)
{
    if (is_rsh_fsm_retgistered(instance)) {
        rsh_resolve(instance, now);     // ==> resolved
    }
}

STATIC void
rsha_resolved_tigger(rsh_instance_t *instance, time_t now)
{
    if (is_rsh_fsm_resolved(instance)) {
        rsh_run(instance, now);         // ==> run
    }
}

STATIC void
rsha_echo_tigger(rsh_instance_t *instance, time_t now)
{
    rsh_echo_t *echo = rshi_echo(instance);
    
    if (is_rsh_fsm_run(instance) &&
        echo->send &&
        now &&
        echo->send < now &&
        (now - echo->send) > echo->interval) {
        
        echo->send = now;
        rsha_echo(instance);          // ==> run
    }
}

STATIC bool
is_rsha_timeout(time_t last, time_t now, uint32 max)
{
    return  last && now && last < now && (now - last) >= max;
}

STATIC bool
is_rsha_echo_timeout(rsh_echo_t *echo, time_t now)
{
    return is_rsha_timeout(echo->recv, now, echo->interval * echo->times);
}

STATIC void
rsha_echo_timeout(rsh_instance_t *instance, time_t now)
{
    if (is_rsh_fsm_run(instance) && is_rsha_echo_timeout(rshi_echo(instance), now)) {
        rshi_fsm(instance, RSH_FSM_INIT, now);
        rshi_fsm_init(instance);
    }
}

STATIC bool
is_rsha_fsm_timeout(time_t fsm_time, time_t now)
{
    return is_rsha_timeout(fsm_time, now, os_second(RSHA_FSM_TIMEOUT));
}

// keep sort
STATIC rsha_timer_handle_f *rsha_handler[] = {
    rsha_init_tigger,
    rsha_registered_tigger,
    rsha_resolved_tigger,
    rsha_echo_tigger,
    
    rsha_echo_timeout,
};

STATIC void 
rsha_timer_handle(rsh_instance_t *instance, time_t now)
{
    int i;

    for (i=0; i<os_count_of(rsha_handler); i++) {
        (*rsha_handler[i])(instance, now);
    }
}

STATIC int
rsha_timer(loop_watcher_t *watcher, time_t now)
{
    mv_t foreach(rsh_instance_t *instance)
    {
        rsha_timer_handle(instance, now);

        return mv2_ok;
    }
    
    rshi_foreach(foreach, true);
    
    rsha.ticks++;
    
    return 0;
}

int
init_rsha_timer(void)
{
    struct itimerspec iticks = OS_ITIMESPEC_INITER(os_second(RSHA_TICKS), os_usecond(RSHA_TICKS));
    int err;
    
    err = os_loop_add_timer(&rsha.loop, rsha_timer, &iticks);
    if (err<0) {
        debug_error("add loop timer error:%d", err);
        
        return err;
    }

    return 0;
}

/******************************************************************************/
