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
typedef void nsqa_timer_handle_f(nsq_instance_t *instance, time_t now);

STATIC void
nsqa_init_tigger(nsq_instance_t *instance, time_t now)
{
    if (is_nsq_fsm_init(instance)) {
        nsq_resolve(instance);      // ==> resolved
    }
}

STATIC void
nsqa_resolved_tigger(nsq_instance_t *instance, time_t now)
{
    if (is_nsq_fsm_resolved(instance)) {
        nsq_connect(instance);      // ==> connected
    }
}

STATIC void
nsqa_connected_tigger(nsq_instance_t *instance, time_t now)
{
    if (is_nsq_fsm_connected(instance)) {
        nsq_handshake(instance);    // ==> handshaked
    }
}

STATIC void
nsqa_handshaked_tigger(nsq_instance_t *instance, time_t now)
{
    if (is_nsq_fsm_handshaked(instance)) {
        nsq_identify(instance);     // ==> identifying
    }
}

STATIC void
nsqa_identified_tigger(nsq_instance_t *instance, time_t now)
{
    if (is_nsq_fsm_identified(instance)) {
        nsq_auth(instance);         // ==> authing
    }
}

STATIC void
nsqa_authed_tigger(nsq_instance_t *instance, time_t now)
{
    if (is_nsq_fsm_authed(instance)) {
        nsq_subscrib(instance);     // ==> subscribing
    }
}

STATIC void
nsqa_subscribed_tigger(nsq_instance_t *instance, time_t now)
{
    if (is_nsq_fsm_subscribed(instance)) {
        nsq_run(instance);          // ==> run
    }
}

STATIC bool
is_nsqa_fsm_timeout(time_t fsm_time, time_t now)
{
    return  fsm_time && 
            now && 
            fsm_time < now && 
            (now-fsm_time) >= os_second(NSQ_MSG_TIMEOUT);
}

STATIC void
nsqa_identifying_timeout(nsq_instance_t *instance, time_t now)
{
    if (is_nsq_fsm_identifying(instance) && is_nsqa_fsm_timeout(instance->fsm_time, now)) {
        nsq_identify(instance); // re-send identify
    }
}

STATIC void
nsqa_subscribing_timeout(nsq_instance_t *instance, time_t now)
{
    if (is_nsq_fsm_subscribing(instance) && is_nsqa_fsm_timeout(instance->fsm_time, now)) {
        nsq_subscrib(instance); // re-send sub
    }
}

// keep sort
STATIC nsqa_timer_handle_f *nsqa_handler[] = {
    nsqa_init_tigger,
    nsqa_resolved_tigger,
    nsqa_connected_tigger,
    nsqa_handshaked_tigger,
    nsqa_identified_tigger,
    nsqa_authed_tigger,
    nsqa_subscribed_tigger,
    
    nsqa_identifying_timeout,
    nsqa_subscribing_timeout,
};

STATIC void 
nsqa_timer_handle(nsq_instance_t *instance, time_t now)
{
    int i;

    for (i=0; i<os_count_of(nsqa_handler); i++) {
        (*nsqa_handler[i])(instance, now);
    }
}

STATIC int
nsqa_timer(loop_watcher_t *watcher, time_t now)
{
    mv_t cb(nsq_instance_t *instance)
    {
        nsqa_timer_handle(instance, now);

        return mv2_ok;
    }
    
    nsqi_foreach(cb, true);
    
    nsqa.st.ticks++;
    
    return 0;
}

int
init_nsqa_timer(void)
{
    struct itimerspec iticks = OS_ITIMESPEC_INITER(os_second(nsqa.env.ticks), os_usecond(nsqa.env.ticks));
    int err;
    
    err = os_loop_add_timer(&nsqa.loop, nsqa_timer, &iticks);
    if (err<0) {
        debug_error("add loop timer error:%d", err);
        
        return err;
    }

    return 0;
}

/******************************************************************************/
