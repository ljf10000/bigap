/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      nsqa
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     cfg
#endif

#define __DEAMON__
#include "nsqa.h"
/******************************************************************************/
#define NSQ_JMAPPER(_)                  \
    _(&nsqa.cfg, string, script)        \
    _(&nsqa.cfg, string, hostname)      \
    _(&nsqa.cfg, string, client_id)     \
    /* end */

int
init_nsq_cfg(void)
{
    jobj_t jobj = JOBJ_MAPF(nsqa.conf, NSQ_JMAPPER);
    if (NULL==jobj) {
        return -EBADCONF;
    }
    
    jobj_put(jobj);
    
    return 0;
}

/******************************************************************************/
