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
#define NSQ_JOBJ_LOADER(_)              \
    _(&nsqa.cfg, string, script)        \
    _(&nsqa.cfg, string, hostname)      \
    _(&nsqa.cfg, string, client_id)     \
    /* end */

int
init_nsq_cfg(void)
{
    DECLARE_JOBJ_LOADER(NSQ_JOBJ_LOADER);
    
    static jobj_loader_f *map[] = JOBJ_MAPPER(NSQ_JOBJ_LOADER);
    int err;
    
    jobj_t jcfg = jobj_byfile(nsqa.conf);
    if (NULL==jcfg) {
        debug_error("bad %s", nsqa.conf);
        
        return -EBADCONF;
    }
    
    err = jobj_load(jcfg, map, os_count_of(map));
    if (err<0) {
        return err;
    }
    
    return 0;
}

/******************************************************************************/
