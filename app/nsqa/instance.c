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
nsq_instance_t *
nsqi_entry(h1_node_t *node)
{
    if (node) {
        return h1_entry(node, nsq_instance_t, node);
    } else {
        return NULL;
    }
}

hash_idx_t
nsqi_hash(char *name)
{
    return hash_bybuf(name, os_strlen(name), NSQ_HASHMASK);
}

nsq_instance_t *
nsqi_create(void)
{
    nsq_instance_t *instance = (nsq_instance_t *)os_zalloc(sizeof(nsq_instance_t));
    if (NULL==instance) {
        return NULL;
    }
    
    return instance;
}

void
__nsqi_destroy(nsq_instance_t *instance)
{
    int i;
    
    if (instance) {
        os_free(instance->name);
        os_free(instance->domain);
        os_free(instance->cache);
        os_free(instance->identify);
    }
}

void
nsqi_insert(nsq_instance_t *instance)
{
    hash_idx_t nhash(hash_node_t *node)
    {
        return nsqi_hash(nsqi_entry(node)->name);
    }
    
    h1_add(&nsqa.table, &instance->node, nsqi_hash);
}

void
nsqi_remove(nsq_instance_t *instance)
{
    h1_del(&nsqa.table, &instance->node);
}

nsq_instance_t *
nsqi_get(char *name)
{
    hash_idx_t dhash(void)
    {
        return nsqi_hash(name);
    }
    
    bool eq(hash_node_t *node)
    {
        return os_streq(name, nsqi_entry(node)->name);
    }

    return nsqi_entry(h1_find(&nsqa.table, dhash, eq));
}

int
nsqi_foreach(nsq_foreach_f *foreach, bool safe)
{
    mv_t node_foreach(h1_node_t *node)
    {
        return (*foreach)(nsqi_entry(node));
    }

    if (safe) {
        return h2_foreach_safe(&nsqa.table, node_foreach);
    } else {
        return h2_foreach(&nsqa.table, node_foreach);
    }
}

int
nsqi_identify(nsq_instance_t *instance, char *json)
{
    nsq_identify_rule_t *rule = nsq_identify_rule();
    char *string;
    int id;
    
    jobj_t jobj = jobj_byjson(json);
    if (NULL==jobj) {
        return -EBADJSON;
    }

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
    jobj_put(jobj);
    
    return 0;
}

int
init_nsq_instance(void)
{
   return 0;
}

/******************************************************************************/
