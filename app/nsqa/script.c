/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      nsqa
#endif
        
#ifndef __THIS_FILE
#define __THIS_FILE     script
#endif

#define __DEAMON__
#include "nsqa.h"
/******************************************************************************/
int
nsq_script(nsq_instance_t *instance, char *json)
{
    jobj_t jscript = NULL;

    jscript = jobj_byjson(json);
    if (NULL==jscript) {
        return 0;
    }

    jobj_t jinstance = jobj_new_object();
    jobj_add(jinstance, NSQ_INSTANCE_NAME_NAME,     jobj_new_string(instance->name));
    jobj_add(jinstance, NSQ_INSTANCE_TOPIC_NAME,    jobj_new_string(instance->topic));
    jobj_add(jinstance, "cache", jobj_new_string(nsqa.env.cache));
    jobj_add(jscript, "instance", jinstance);
    
    os_p_system(NSQ_SCRIPT " '%s'", jobj_json(jscript));

    jobj_put(jscript);
    
    return 0;
}
/******************************************************************************/
