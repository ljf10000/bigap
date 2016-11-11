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
typedef mv_t nsq_timer_handle_f(nsq_instance_t *instance, time_t now);

static mv_t
resolve_timer(nsq_instance_t *instance, time_t now)
{
    if (is_nsq_fsm_init(instance)) {
        nsq_resolve(instance);
    }
    
    return mv2_ok;
}

static mv_t
connect_timer(nsq_instance_t *instance, time_t now)
{
    if (is_nsq_fsm_resolved(instance)) {
        nsq_connect(instance);
    }

    return mv2_ok;
}

static mv_t
handshake_timer(nsq_instance_t *instance, time_t now)
{
    if (is_nsq_fsm_connected(instance)) {
        nsq_handshake(instance);
    }

    return mv2_ok;
}

static mv_t
identify_timer(nsq_instance_t *instance, time_t now)
{
    if (is_nsq_fsm_handshaked(instance)) {
        nsq_identify(instance);
    }

    return mv2_ok;
}

static mv_t
subscrib_timer(nsq_instance_t *instance, time_t now)
{
    if (is_nsq_fsm_identified(instance)) {
        nsq_subscrib(instance);
    }

    return mv2_ok;
}

static inline bool
is_fsm_timeout(time_t fsm_time, time_t now)
{
    return  fsm_time && 
            now && 
            fsm_time < now && 
            (now-fsm_time) >= os_second(NSQ_MSG_TIMEOUT);
}

static mv_t
fsm_timeout(nsq_instance_t *instance, time_t now)
{
    int err;
    
    switch(instance->fsm) {
        case NSQ_FSM_HANDSHAKING:
            if (is_fsm_timeout(instance->fsm_time, now)) {
                err = nsq_handshake(instance);
            }
            
            break;
        case NSQ_FSM_IDENTIFYING:
            if (is_fsm_timeout(instance->fsm_time, now)) {
                err = nsq_identify(instance);
            }
            
            break;
        case NSQ_FSM_SUBSCRIBING:
            if (is_fsm_timeout(instance->fsm_time, now)) {
                err = nsq_subscrib(instance);
            }
            
            break;
        case NSQ_FSM_RUN:
            // todo
            
            break;
    }
    
    return mv2_ok;
}

static mv_t 
timer_handle(nsq_instance_t *instance, time_t now)
{
    static nsq_timer_handle_f *handler[] = {
        resolve_timer,
        connect_timer,
        handshake_timer,
        identify_timer,
        subscrib_timer,
        fsm_timeout,
    };
    
    mv_u mv;
    int i;

    for (i=0; i<os_count_of(handler); i++) {
        mv.v = (*handler[i])(instance, now);
        if (is_mv2_break(mv)) {
            return mv.v;
        }
    }

    return mv2_ok;
}

static int
nsq_timer(loop_watcher_t *watcher, time_t now)
{
    mv_t cb(nsq_instance_t *instance)
    {
        return timer_handle(instance, now);
    }
    
    nsqi_foreach(cb, true);
    
    nsqa.st.ticks++;
    
    return 0;
}

int
init_nsq_timer(void)
{
    struct itimerspec iticks = OS_ITIMESPEC_INITER(os_second(nsqa.env.ticks), os_usecond(nsqa.env.ticks));
    int err;
    
    err = os_loop_add_timer(&nsqa.loop, nsq_timer, &iticks);
    if (err<0) {
        debug_error("add loop timer error:%d", err);
        
        return err;
    }

    return 0;
}

/******************************************************************************/
