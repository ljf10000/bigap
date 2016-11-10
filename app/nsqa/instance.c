/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      nsqa
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     instance
#endif

#define __DEAMON__
#include "nsqa.h"
/******************************************************************************/
static nsq_instance_t *
__entry(hash_node_t *node)
{
    if (node) {
        return h1_entry(node, nsq_instance_t, node);
    } else {
        return NULL;
    }
}

static nsq_instance_t *
__hentry(h1_node_t *node)
{
    if (node) {
        return h1_entry(node, nsq_instance_t, node);
    } else {
        return NULL;
    }
}

static hash_idx_t
__hash(char *name)
{
    return hash_bybuf(name, os_strlen(name), NSQ_HASHMASK);
}

static nsq_instance_t *
__create(void)
{
    nsq_instance_t *instance = (nsq_instance_t *)os_zalloc(sizeof(nsq_instance_t));
    if (NULL==instance) {
        return NULL;
    }
    
    return instance;
}

static void
____destroy(nsq_instance_t *instance)
{
    int i;
    
    if (instance) {
        os_free(instance->name);
        os_free(instance->domain);
        os_free(instance->cache);
        os_free(instance->identify);
    }
}

#define __destroy(_instance)    do{ \
    ____destroy(_instance);         \
    _instance = NULL;               \
}while(0)

static void
__insert(nsq_instance_t *instance)
{
    hash_idx_t nhash(hash_node_t *node)
    {
        return __hash(__entry(node)->name);
    }
    
    h1_add(&nsqa.table, &instance->node, nhash);
}

static void
__remove(nsq_instance_t *instance)
{
    h1_del(&nsqa.table, &instance->node);
}

static nsq_instance_t *
__get(char *name)
{
    hash_idx_t dhash(void)
    {
        return __hash(name);
    }
    
    bool eq(hash_node_t *node)
    {
        return os_streq(name, __entry(node)->name);
    }

    return __hentry(h1_find(&nsqa.table, dhash, eq));
}

static int
__foreach(nsq_foreach_f *foreach, bool safe)
{
    mv_t node_foreach(h1_node_t *node)
    {
        return (*foreach)(__hentry(node));
    }

    if (safe) {
        return h1_foreach_safe(&nsqa.table, node_foreach);
    } else {
        return h1_foreach(&nsqa.table, node_foreach);
    }
}

static int
__identify(nsq_instance_t *instance, jobj_t jobj)
{
    nsq_identify_rule_t *rule = nsq_identify_rule();
    char *string;
    int id;

    jobj_foreach(jobj, k, v) {
        id = nsq_identify_idx(k);
        if (false==is_good_nsq_identify(id) ||
            jobj_type(v) != rule[id].jtype  ||
            NSQ_IDENTIFY_TYPE_CLOUD!=rule[id].type) {
            /*
            * del invalid key
            */
            jobj_del(jobj, k);
        }
    }

    jobj_add_string(jobj, NSQ_IDENTIFY_USER_AGENT_NAME, NSQ_USER_AGENT);
    jobj_add_string(jobj, NSQ_IDENTIFY_CLIENT_ID_NAME,  nsqa.cfg.client_id);
    jobj_add_string(jobj, NSQ_IDENTIFY_HOSTNAME_NAME,   nsqa.cfg.hostname);

    for (id=0; id<NSQ_IDENTIFY_END; id++) {
        string = nsq_identify_string(id);
        
        if (NSQ_IDENTIFY_TYPE_CLOUD==rule[id].type &&
            NULL==jobj_get(jobj, string)) {
            /*
            * no value, set default
            */
            switch(rule[id].jtype) {
                case jtype_bool:
                    jobj_add_bool(jobj, string, rule[id].deft.b);
                    
                    break;
                case jtype_int:
                    jobj_add_i32(jobj, string, rule[id].deft.i);
                    
                    break;
                case jtype_string:
                    jobj_add_string(jobj, string, rule[id].deft.s);
                    
                    break;
            }
        }
    }

    os_free(instance->identify);
    instance->identify = jobj_json(jobj);
    
    return 0;
}

nsq_instance_t *
nsqi_insert(char *json)
{
    nsq_instance_t *instance = NULL;
    
    jobj_t jobj = jobj_byjson(json);
    if (NULL==jobj) {
        return -EBADJSON;
    }

    
}

int
init_nsq_instance(void)
{
   return 0;
}

/******************************************************************************/
