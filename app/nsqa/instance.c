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

static void
____destroy(nsq_instance_t *instance)
{
    int i;
    
    if (instance) {
        os_free(instance->name);
        os_free(instance->domain);
        os_free(instance->cache);
        os_free(instance->topic);
        os_free(instance->identify);

        jobj_put(instance->jinstance);
        
        nsqb_fini(&instance->brecver);
        nsqb_fini(&instance->bsender);

        if (instance->loop) {
            os_loop_del_watcher(&nsqa.loop, instance->fd);
            instance->loop = false;
        }
        
        os_close(instance->fd);
        
        os_free(instance);
    }
}

#define __destroy(_instance)    do{ \
    ____destroy(_instance);         \
    _instance = NULL;               \
}while(0)

static nsq_instance_t *
__create(char *name, jobj_t jobj)
{
    nsq_instance_t *instance = NULL;
    jobj_t jval;
    int fd, port, err;
    
    instance = (nsq_instance_t *)os_zalloc(sizeof(nsq_instance_t));
    if (NULL==instance) {
        goto error;
    }
    
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd<0) {
        debug_error("instance %s socket error:%d", instance->name, -errno);
        
        goto error;
    }
    instance->fd        = fd;

    err = os_loop_add_normal(&nsqa.loop, fd, nsq_recver, instance);
    if (err<0) {
        goto error;
    }
    instance->loop      = true;
    
    instance->fsm       = NSQ_FSM_INIT;
    instance->jinstance = jobj;
    
    instance->name      = os_strdup(name);
    os_asprintf(&instance->cache, "%s/%s", nsqa.env.cache, name);

    jval = jobj_get(jobj, NSQ_INSTANCE_DOMAIN_NAME);
    instance->domain    = os_strdup(jobj_get_string(jval));

    jval = jobj_get(jobj, NSQ_INSTANCE_TOPIC_NAME);
    instance->topic     = os_strdup(jobj_get_string(jval));
    instance->channel   = nsqa.cfg.hostname;
    
    jval = jobj_get(jobj, NSQ_INSTANCE_IDENTIFY_NAME);
    instance->identify  = os_strdup(jobj_json(jval));

    jval = jobj_get(jobj, NSQ_INSTANCE_PORT_NAME);
    port = jobj_get_bool(jval);
    sockaddr_in_t *server   = &instance->server;
    server->sin_family      = AF_INET;
    server->sin_port        = htons(port);
    server->sin_addr.s_addr = INADDR_NONE;

    sockaddr_in_t *client   = &instance->client;
    client->sin_family      = AF_INET;
    
    nsqb_init(&instance->brecver, NSQ_SEND_BUFFER_SIZE);
    nsqb_init(&instance->bsender, NSQ_RECV_BUFFER_SIZE);
    
    return instance;
error:
    __destroy(instance);

    return NULL;
}

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

static int
__nsqi_insert(jobj_t jobj)
{
    int err;

    char *name = jobj_get_string(jobj_get(jobj, NSQ_INSTANCE_NAME_NAME));
    if (NULL==name) {
        debug_cli("no-found key " NSQ_INSTANCE_NAME_NAME);
        
        return -EBADJSON;
    }
    
    if (nsqi_get(name)) {
        debug_entry("instance %s exist.", name);
        
        return -EEXIST;
    }

    nsq_instance_t *instance = __create(name, jobj);
    if (NULL==instance) {
        return -ENOMEM;
    }

    err = __insert(instance);
    if (err<0) {
        debug_entry("insert %s error:%d", name, err);
        
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
    if (NULL==jobj) {
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
nsqi_remove(char *name)
{
    int err;
    
    if (NULL==name) {
        return -EINVAL0;
    }

    nsq_instance_t *instance = nsqi_get(name);
    if (NULL==instance) {
        return -ENOEXIST;
    }

    err = __remove(instance);
    if (err<0) {
        return err;
    }

    __destroy(instance);

    return 0;
}

int
nsqi_foreach(nsq_foreach_f *foreach, bool safe)
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

nsq_instance_t *
nsqi_get(char *name)
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
/******************************************************************************/
