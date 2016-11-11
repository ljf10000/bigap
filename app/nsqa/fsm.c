/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      nsqa
#endif
        
#ifndef __THIS_FILE
#define __THIS_FILE     fsm
#endif

#define __DEAMON__
#include "nsqa.h"
/******************************************************************************/
int nsq_fsm_change(nsq_instance_t *instance, int fsm)
{
    instance->fsm = fsm;

    return 0;
}

int nsq_fsm_init(nsq_instance_t *instance)
{
    int err;
    
    instance->fsm = NSQ_FSM_INIT;

    // todo: fsm timer

    return 0;
}

int nsq_fsm_resolved(nsq_instance_t *instance)
{
    instance->fsm = NSQ_FSM_RESOLVED;


    return 0;
}

int nsq_fsm_resolved(nsq_instance_t *instance)
{
    instance->fsm = NSQ_FSM_RESOLVED;


    return 0;
}


/******************************************************************************/
