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
STATIC nsq_instance_t *
__nsqi_hx_entry(hash_node_t *node)
{
    return hx_entry(node, nsq_instance_t, node, 0);
}

STATIC nsq_instance_t *
__nsqi_h1_entry(h1_node_t *node)
{
    return h1_entry(node, nsq_instance_t, node);
}

STATIC hash_idx_t
__nsqi_hash(char *name, char *topic, char *channel)
{
    bkdr_t bkdr = 0;

    bkdr = os_str_bkdr_push(bkdr, name);
    bkdr = os_str_bkdr_push(bkdr, topic);
    bkdr = os_str_bkdr_push(bkdr, channel);

    return (hash_idx_t)bkdr & NSQ_HASHMASK;
}

STATIC hash_idx_t 
__ndqi_nhash(hash_node_t *node)
{
    nsq_instance_t *instance = __nsqi_hx_entry(node);
    
    return __nsqi_hash(instance->name, instance->topic, instance->channel);
}

STATIC int
__nsqi_destroy(nsq_instance_t *instance)
{
    int i;
    
    if (instance) {
        os_free(instance->name);
        os_free(instance->topic);
        os_free(instance->channel);

        os_free(instance->domain);
        os_free(instance->script);

        jobj_put(instance->jinstance);
        jobj_put(instance->jauth);
        
        nsqb_fini(&instance->brecver);
        nsqb_fini(&instance->bsender);

        if (instance->loop) {
            os_loop_del_watcher(&nsqa.loop, instance->fd);
            instance->loop = false;
        }
        
        os_close(instance->fd);
        
        os_free(instance);
    }

    return 0;
}

STATIC nsq_instance_t *
__nsqi_create(char *name, char *topic, char *channel, jobj_t jobj)
{
    nsq_instance_t *instance = NULL;
    jobj_t jval, jidentify;
    int fd, err;
    
    instance = (nsq_instance_t *)os_zalloc(sizeof(nsq_instance_t));
    if (NULL==instance) {
        goto error;
    }
    
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd<0) {
        debug_error("instance %s socket error:%d", instance->name, -errno);
        
        goto error;
    }
    // os_noblock(fd);
    os_closexec(fd);
    instance->fd = fd;

    err = os_loop_add_normal(&nsqa.loop, fd, nsq_recver, instance);
    if (err<0) {
        goto error;
    }
    instance->loop      = true;

    instance->rdy       = NSQ_RDY;
    instance->fsm       = NSQ_FSM_INIT;
    instance->jinstance = jobj;

    instance->name      = os_strdup(name);
    instance->topic     = os_strdup(topic);
    instance->channel   = os_strdup(channel);

    jval = jobj_get(jobj, NSQ_INSTANCE_DOMAIN_NAME);
    instance->domain    = os_strdup(jobj_get_string(jval));

    jval = jobj_get(jobj, NSQ_INSTANCE_SCRIPT_NAME);
    instance->script    = os_strdup(jobj_get_string(jval));

    jval = jobj_get(jobj, NSQ_INSTANCE_PORT_NAME);
    instance->port = jobj_get_int(jval);
    sockaddr_in_t *server   = &instance->server;
    server->sin_family      = AF_INET;
    server->sin_port        = htons(instance->port);
    server->sin_addr.s_addr = INADDR_NONE;

    sockaddr_in_t *client   = &instance->client;
    client->sin_family      = AF_INET;

    jidentify = jobj_get(jobj, NSQ_INSTANCE_IDENTIFY_NAME);
    instance->jidentify = jidentify;

    jval = jobj_get(jidentify, NSQ_IDENTIFY_FEATURE_NEGOTIATION_NAME);
    instance->auth      = jobj_get_bool(jval);

    nsqb_init(&instance->brecver, NSQ_SEND_BUFFER_SIZE);
    nsqb_init(&instance->bsender, NSQ_RECV_BUFFER_SIZE);
    
    return instance;
error:
    __nsqi_destroy(instance);

    return NULL;
}

STATIC int
__nsqi_insert(nsq_instance_t *instance)
{
    return h1_add(&nsqa.table, &instance->node, __ndqi_nhash);
}

STATIC int
__nsqi_remove(nsq_instance_t *instance)
{
    return h1_del(&nsqa.table, &instance->node);
}

STATIC int
__nsqi_show(nsq_instance_t *instance)
{
    return 0;
}

STATIC int
nsqi_handle_name_topic_channel_by_special(
    int (*pre)(nsq_instance_t *instance),
    int (*handle)(nsq_instance_t *instance),
    int (*post)(nsq_instance_t *instance),
    char *name, char *topic, char *channel
)
{
    int err;
    
    nsq_instance_t *instance = nsqi_get(name, topic, channel);
    if (NULL==instance) {
        return -ENOEXIST;
    }

    if (pre) {
        err = (*pre)(instance);
        if (err<0) {
            return err;
        }
    }

    if (handle) {
        err = (*handle)(instance);
        if (err<0) {
            return err;
        }
    }
    
    if (post) {
        err = (*post)(instance);
        if (err<0) {
            return err;
        }
    }
    
    return 0;
}

STATIC int
nsqi_handle_name_topic_channel_by_wildcard(
    int (*pre)(nsq_instance_t *instance),
    int (*handle)(nsq_instance_t *instance, int pre_error),
    int (*post)(nsq_instance_t *instance),
    char *name, char *topic, char *channel
)
{
    mv_t foreach(nsq_instance_t *instance)
    {
        int err = 0;
        
        if ((NULL==name || os_streq(name, instance->name)) &&
            (NULL==topic || os_streq(topic, instance->topic)) &&
            (NULL==channel || os_streq(channel, instance->channel))) {
            if (pre) {
                err = (*pre)(instance);
                if (err<0) {
                    return mv2_go(err);
                }
            }
            
            if (handle) {
                err = (*handle)(instance, err);
                if (err<0) {
                    return mv2_go(err);
                }
            }
            
            if (post) {
                err = (*post)(instance);
                if (err<0) {
                    return mv2_go(err);
                }
            }
        }

        return mv2_ok;
    }
    
    return nsqi_foreach(foreach, true);
}

STATIC int
nsqi_remove_by_special(char *name, char *topic, char *channel)
{
    return nsqi_handle_name_topic_channel_by_special(
                NULL, __nsqi_remove, __nsqi_destroy,
                name, topic, channel);
}

STATIC int
nsqi_remove_by_wildcard(char *name, char *topic, char *channel)
{
    int notify(nsq_instance_t *instance, int pre_error)
    {
        cli_sprintf("instance %s.%s.%s remove %s\n",
            instance->name,
            instance->topic,
            instance->channel,
            __ok_string(0==pre_error));

        return 0;
    }
    
    return nsqi_handle_name_topic_channel_by_wildcard(
                __nsqi_remove, notify, __nsqi_destroy,
                name, topic, channel);
}

STATIC int
nsqi_show_by_special(char *name, char *topic, char *channel)
{
    return nsqi_handle_name_topic_channel_by_special(
                NULL, __nsqi_show, NULL,
                name, topic, channel);
}

STATIC int
nsqi_show_by_wildcard(char *name, char *topic, char *channel)
{
    int notify(nsq_instance_t *instance, int pre_error)
    {
        return __nsqi_show(instance);
    }
    
    return nsqi_handle_name_topic_channel_by_wildcard(
                NULL, notify, NULL,
                name, topic, channel);
}

int
nsqi_fsm(nsq_instance_t *instance, int fsm)
{
    int old = instance->fsm;
    
    if (is_good_nsq_fsm(fsm)) {
        instance->fsm = fsm;
        instance->fsm_time = time(NULL);
        
        debug_entry("change instance(%s)'fsm from %s to %s",
            instance->name,
            nsq_fsm_getnamebyid(old),
            nsq_fsm_getnamebyid(fsm));

        return 0;
    } else {
        debug_error("try change instance(%s)'fsm from %s to invalid %d",
            instance->name,
            nsq_fsm_getnamebyid(old),
            fsm);

        return -EBADFSM;
    }
}

int
nsqi_insert(jobj_t jobj)
{
    int err;

    char *name = jobj_get_string(jobj_get(jobj, NSQ_INSTANCE_NAME_NAME));
    if (NULL==name) {
        debug_cli("no-found " NSQ_INSTANCE_NAME_NAME);
        
        return -EBADJSON;
    }

    char *topic = jobj_get_string(jobj_get(jobj, NSQ_INSTANCE_TOPIC_NAME));
    if (NULL==name) {
        debug_cli("no-found " NSQ_INSTANCE_TOPIC_NAME);
        
        return -EBADJSON;
    }

    char *channel = jobj_get_string(jobj_get(jobj, NSQ_INSTANCE_CHANNEL_NAME));
    if (NULL==name) {
        debug_cli("no-found " NSQ_INSTANCE_CHANNEL_NAME);
        
        return -EBADJSON;
    }

    if (nsqi_get(name, topic, channel)) {
        debug_entry("instance %s.%s.%s exist.", name, topic, channel);
        
        return -EEXIST;
    }
    
    nsq_instance_t *instance = __nsqi_create(name, topic, channel, jobj);
    if (NULL==instance) {
        return -ENOMEM;
    }

    err = __nsqi_insert(instance);
    if (err<0) {
        debug_entry("insert %s error:%d", name, err);
        
        return err;
    }
    
    return 0;
}

int
nsqi_insert_byjson(char *json)
{
    jobj_t jobj = NULL;
    int err = 0;
    
    jobj = jobj_byjson(json);
    if (NULL==jobj) {
        err = -EBADJSON; goto error;
    }

    err = nsqi_insert(jobj);
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
nsqi_remove(char *name, char *topic, char *channel)
{
    if (name && topic && channel) {
        return nsqi_remove_by_special(name, topic, channel);
    } else {
        return nsqi_remove_by_wildcard(name, topic, channel);
    }
}

int
nsqi_foreach(mv_t (*foreach)(nsq_instance_t *instance), bool safe)
{
    mv_t node_foreach(h1_node_t *node)
    {
        return (*foreach)(__nsqi_h1_entry(node));
    }

    if (safe) {
        return h1_foreach_safe(&nsqa.table, node_foreach);
    } else {
        return h1_foreach(&nsqa.table, node_foreach);
    }
}

nsq_instance_t *
nsqi_get(char *name, char *topic, char *channel)
{
    hash_idx_t dhash(void)
    {
        return __nsqi_hash(name, topic, channel);
    }
    
    bool eq(hash_node_t *node)
    {
        nsq_instance_t *instance = __nsqi_hx_entry(node);
        
        return os_streq(name, instance->name)
            && os_streq(topic, instance->topic)
            && os_streq(channel, instance->channel);
    }

    return __nsqi_h1_entry(h1_find(&nsqa.table, dhash, eq));
}

int
nsqi_show(char *name, char *topic, char *channel)
{
    if (name && topic && channel) {
        return nsqi_show_by_special(name, topic, channel);
    } else {
        return nsqi_show_by_wildcard(name, topic, channel);
    }
}
/******************************************************************************/
