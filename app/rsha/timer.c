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
        rshi_resolve(instance, now);     // ==> resolved
    }
}

STATIC bool
is_rsha_handshake_tigger(rshi_handshake_t *handshake, time_t now)
{
    return 0==handshake->sends || is_rsha_timeout(handshake->send, now, handshake->interval);
}

STATIC void
rsha_resolved_tigger(rsh_instance_t *instance, time_t now)
{
    rshi_handshake_t *handshake = &instance->handshake;
    
    if (is_rsh_fsm_resolved(instance) && is_rsha_handshake_tigger(handshake, now)) {
        debug_proto("instance %s handshake ...", instance->sp);
        
        rshi_handshake(instance, now);
    }
}

STATIC void
rsha_handshaked_tigger(rsh_instance_t *instance, time_t now)
{
    if (is_rsh_fsm_handshaked(instance)) {
        rshi_fsm(instance, RSH_FSM_RUN, now);
    }
}

STATIC bool
is_rsha_echo_tigger(rsh_echo_t *echo, time_t now)
{
    return 0==echo->sends || is_rsha_timeout(echo->send, now, echo->interval);
}

STATIC void
rsha_echo_tigger(rsh_instance_t *instance, time_t now)
{   
    if (is_rsh_fsm_run(instance) && is_rsha_echo_tigger(rshi_echo_get(instance), now)) {
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
        debug_proto("fsm restart: echo timeout");
            
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
        debug_proto("fsm restart: peer error[%d] > max peer error[%d]",
            instance->peer_error,
            instance->peer_error_max);

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
        rsha_resolved_tigger,
        rsha_handshaked_tigger,
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
