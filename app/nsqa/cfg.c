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
    /* end */

int
init_nsq_cfg(void)
{
    jobj_t jobj = JOBJ_MAPF(nsqa.env.conf, NSQ_JMAPPER);
    if (NULL==jobj) {
        return -EBADCONF;
    }

    char *mac = os_getbasemac();
    os_strdcpy(nsqa.cfg.hostname, mac);
    os_strdcpy(nsqa.cfg.client_id, mac);
    
    jobj_put(jobj);
    
    return 0;
}

/******************************************************************************/
