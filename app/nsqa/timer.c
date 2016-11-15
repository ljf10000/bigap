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
typedef void nsq_timer_handle_f(nsq_instance_t *instance, time_t now);

static void
init_tigger(nsq_instance_t *instance, time_t now)
{
    if (is_nsq_fsm_init(instance)) {
        nsq_resolve(instance);      // ==> resolved
    }
}

static void
resolved_tigger(nsq_instance_t *instance, time_t now)
{
    if (is_nsq_fsm_resolved(instance)) {
        nsq_connect(instance);      // ==> connected
    }
}

static void
connected_tigger(nsq_instance_t *instance, time_t now)
{
    if (is_nsq_fsm_connected(instance)) {
        nsq_handshake(instance);    // ==> handshaked
    }
}

static void
handshaked_tigger(nsq_instance_t *instance, time_t now)
{
    if (is_nsq_fsm_handshaked(instance)) {
        nsq_identify(instance);     // ==> identifying
    }
}

static void
identified_tigger(nsq_instance_t *instance, time_t now)
{
    if (is_nsq_fsm_identified(instance)) {
        nsq_auth(instance);         // ==> authing
    }
}

static void
authed_tigger(nsq_instance_t *instance, time_t now)
{
    if (is_nsq_fsm_authed(instance)) {
        nsq_subscrib(instance);     // ==> subscribing
    }
}

static void
subscribed_tigger(nsq_instance_t *instance, time_t now)
{
    if (is_nsq_fsm_subscribed(instance)) {
        nsq_run(instance);          // ==> run
    }
}

static inline bool
is_fsm_timeout(time_t fsm_time, time_t now)
{
    return  fsm_time && 
            now && 
            fsm_time < now && 
            (now-fsm_time) >= os_second(NSQ_MSG_TIMEOUT);
}

static void
identifying_timeout(nsq_instance_t *instance, time_t now)
{
    if (is_nsq_fsm_identifying(instance) && is_fsm_timeout(instance->fsm_time, now)) {
        nsq_identify(instance); // re-send identify
    }
}

static void
subscribing_timeout(nsq_instance_t *instance, time_t now)
{
    if (is_nsq_fsm_subscribing(instance) && is_fsm_timeout(instance->fsm_time, now)) {
        nsq_subscrib(instance); // re-send sub
    }
}

static void 
timer_handle(nsq_instance_t *instance, time_t now)
{
    // keep sort
    static nsq_timer_handle_f *handler[] = {
        init_tigger,
        resolved_tigger,
        connected_tigger,
        handshaked_tigger,
        identified_tigger,
        authed_tigger,
        subscribed_tigger,
        
        identifying_timeout,
        subscribing_timeout,
    };
    
    int i;

    for (i=0; i<os_count_of(handler); i++) {
        (*handler[i])(instance, now);
    }
}

static int
nsq_timer(loop_watcher_t *watcher, time_t now)
{
    mv_t cb(nsq_instance_t *instance)
    {
        timer_handle(instance, now);

        return mv2_ok;
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
