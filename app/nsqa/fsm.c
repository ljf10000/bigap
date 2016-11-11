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
int
nsq_fsm_change(nsq_instance_t *instance, int fsm)
{
    int old = instance->fsm;
    
    if (is_good_nsq_fsm(fsm)) {
        instance->fsm = fsm;
        instance->fsm_time = time(NULL);
        
        debug_entry("change instance(%s)'fsm from %s to %s",
            instance->name,
            nsq_fsm_string(old),
            nsq_fsm_string(fsm));

        return 0;
    } else {
        debug_error("try change instance(%s)'fsm from %s to invalid %d",
            instance->name,
            nsq_fsm_string(old),
            fsm);

        return -EBADFSM;
    }
}
/******************************************************************************/
