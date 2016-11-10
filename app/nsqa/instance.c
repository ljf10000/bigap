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
__create(char *name, jobj_t jobj)
{
    jobj_t jval;
    
    nsq_instance_t *instance = (nsq_instance_t *)os_zalloc(sizeof(nsq_instance_t));
    if (NULL==instance) {
        return NULL;
    }
    instance->jobj      = jobj;
    instance->name      = os_strdup(name);
    os_asprintf(&instance->cache, "%s/%s", nsqa.cache, name);

    jval = jobj_get(jobj, NSQ_INSTANCE_DOMAIN_NAME);
    instance->domain    = os_strdup(jobj_get_string(jval));

    jval = jobj_get(jobj, NSQ_INSTANCE_TOPIC_NAME);
    instance->topic     = os_strdup(jobj_get_string(jval));
    
    jval = jobj_get(jobj, NSQ_INSTANCE_PORT_NAME);
    int port = jobj_get_bool(jval);
    sockaddr_in_t *server  = &instance->addr.in;
    server->sin_family  = AF_INET;
    server->sin_port    = htons(port);
    
    jval = jobj_get(jobj, NSQ_INSTANCE_IDENTIFY_NAME);
    instance->identify  = os_strdup(jobj_json(jval));

    nsqb_init(&instance->recver, NSQ_OUT_BUFFER_SIZE);
    nsqb_init(&instance->sender, 1+OS_PAGE_LEN);
    
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

static int
__insert(nsq_instance_t *instance)
{
    hash_idx_t nhash(hash_node_t *node)
    {
        return __hash(__entry(node)->name);
    }
    
    return h1_add(&nsqa.table, &instance->node, nhash);
}

static int
__remove(nsq_instance_t *instance)
{
    return h1_del(&nsqa.table, &instance->node);
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
__nsqi_insert(jobj_t jobj)
{
    jobj_t jval;
    char *name;
    int err;

    jval = jobj_get(jobj, NSQ_INSTANCE_NAME_NAME);
    if (NULL==jval) {
        return -EBADJSON;
    }
    
    name = jobj_get_string(jval);
    if (NULL==name) {
        return -EBADJSON;
    }
    
    if (__get(name)) {
        return -EBADJSON;
    }

    nsq_instance_t *instance = __create(name, jobj);
    if (NULL==instance) {
        return -EEXIST;
    }

    err = __insert(instance);
    if (err<0) {
        return err;
    }
    
    return 0;
}

int
nsqi_insert(char *json)
{
    jobj_t jobj = NULL;
    int err = 0;
    
    jobj = jobj_byjson(json);
    if (jobj) {
        err = -EBADJSON; goto error;
    }

    err = __nsqi_insert(jobj);
    if (err<0) {
        goto error;
    }

error:
    if (err<0) {
        jobj_put(jobj);
    }
    
    return err;
}

int
init_nsq_instance(void)
{
   return 0;
}

/******************************************************************************/
