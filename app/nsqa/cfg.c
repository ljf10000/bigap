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
    jobj_t jobj = JOBJ_MAPF(nsqa.conf, NSQ_JMAPPER);
    if (NULL==jobj) {
        return -EBADCONF;
    }

    char *mac = os_getbasemac();
    nsqa.cfg.hostname = os_strdup(mac);
    nsqa.cfg.client_id= os_strdup(mac);
    
    jobj_put(jobj);
    
    return 0;
}

/******************************************************************************/
