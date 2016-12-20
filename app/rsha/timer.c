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

STATIC bool
is_rsha_timeout(time_t last, time_t now, uint32 max)
{
    return  last && now && last < now && (now - last) >= max;
}

STATIC void
rsha_init_tigger(rsh_instance_t *instance, time_t now)
{
    if (is_rsh_fsm_init(instance)) {
        rshi_register(instance, now);    // ==> registered
    }
}

STATIC void
rsha_registered_tigger(rsh_instance_t *instance, time_t now)
{
    if (is_rsh_fsm_retgistered(instance)) {
        rshi_resolve(instance, now);     // ==> resolved
    }
}

STATIC void
rsha_resolved_tigger(rsh_instance_t *instance, time_t now)
{
    if (is_rsh_fsm_resolved(instance)) {
        rshi_run(instance, now);         // ==> run
    }
}

STATIC bool
is_rsha_echo_tigger(rsh_echo_t *echo, time_t now)
{
    return is_rsha_timeout(echo->send, now, echo->interval);
}

STATIC void
rsha_echo_tigger(rsh_instance_t *instance, time_t now)
{
    rsh_echo_t *echo = rshi_echo_get(instance);
    
    if (is_rsh_fsm_run(instance) && is_rsha_echo_tigger(echo, now)) {
        echo->send = now;
        
        rshi_echo(instance, now);   // ==> run
    }
}

STATIC bool
is_rsha_echo_timeout(rsh_echo_t *echo, time_t now)
{
    return is_rsha_timeout(echo->recv, now, echo->interval * echo->times);
}

STATIC void
rsha_echo_timeout(rsh_instance_t *instance, time_t now)
{
    if (is_rsh_fsm_run(instance) && is_rsha_echo_timeout(rshi_echo_get(instance), now)) {
        rshi_fsm_restart(instance, now);
    }
}

STATIC bool
is_rsha_busy_timeout(rsh_instance_t *instance, time_t now)
{
    return instance->echo.busy && is_rsha_timeout(instance->tm.busy, now, RSHA_ECHO_BUSY_TIMEOUT);
}

STATIC void
rsha_busy_timeout(rsh_instance_t *instance, time_t now)
{
    if (is_rsh_fsm_run(instance) && is_rsha_busy_timeout(instance, now)) {
        rshi_echo_set(instance, now, false);
    }
}

STATIC void
rsha_error_checker(rsh_instance_t *instance, time_t now)
{
    if (is_rsh_fsm_run(instance) && instance->peer_error > instance->peer_error_max) {
        rshi_fsm_restart(instance, now);
    }
}

STATIC bool
is_rsha_fsm_timeout(time_t fsm_time, time_t now)
{
    return is_rsha_timeout(fsm_time, now, os_second(RSHA_FSM_TIMEOUT));
}

STATIC void 
rsha_timer_handle(rsh_instance_t *instance, time_t now)
{
    static rsha_timer_handle_f *tigger[] = {
        rsha_init_tigger,
        rsha_registered_tigger,
        rsha_resolved_tigger,
        rsha_echo_tigger,
        rsha_echo_timeout,
        rsha_busy_timeout,
        rsha_error_checker,
    };
    int i;

    for (i=0; i<os_count_of(tigger); i++) {
        (*tigger[i])(instance, now);
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
