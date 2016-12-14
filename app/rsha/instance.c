/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      rsha
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     instance
#endif

#define __DEAMON__
#include "rsha.h"
/******************************************************************************/
#if 1
DECLARE_DB_H2(&rsha.head.instance, __rshi, rsh_instance_t, node.instance);

static inline rsh_instance_t *
__rshi_hx_entry(hash_node_t *node, hash_idx_t nidx);

static inline rsh_instance_t *
__rshi_h2_entry(h2_node_t *node);

static inline int
__rshi_foreach(mv_t (*foreach)(rsh_instance_t *entry), bool safe);
#endif

STATIC hash_idx_t
__rshi_hashname(char *name)
{
    return hash_bybuf(name, os_strlen(name), RSHI_NAMEHASHSIZE - 1);
}

STATIC hash_idx_t
__rshi_hashaddr(sockaddr_in_t *addr)
{
    bkdr_t bkdr = 0;

    bkdr = os_bin_bkdr_push(bkdr, &addr->sin_addr.s_addr, sizeof(addr->sin_addr.s_addr));
    bkdr = os_bin_bkdr_push(bkdr, &addr->sin_port, sizeof(addr->sin_port));
    
    return bkdr & (RSHI_ADDRHASHSIZE - 1);
}

STATIC hash_idx_t
__rshi_nodehashname(hash_node_t *node)
{
    rsh_instance_t *entry = __rshi_hx_entry(node, RSHI_NIDX_NAME);

    return __rshi_hashname(entry->name);
}

STATIC hash_idx_t
__rshi_nodehashaddr(hash_node_t *node)
{
    rsh_instance_t *entry = __rshi_hx_entry(node, RSHI_NIDX_ADDR);

    return __rshi_hashaddr(&entry->server);
}

STATIC int
__rshi_destroy(rsh_instance_t *instance)
{
    if (instance) {
        os_free(instance->name);
        os_free(instance->proxy);
        os_free(instance->registry);

        os_free(instance->key);
        os_free(instance->key8);
        os_free(instance->key32);

        jobj_put(instance->jcfg);
        
        if (instance->loop) {
            os_loop_del_watcher(&rsha.loop, instance->fd);
            
            instance->loop = false;
            instance->fd = INVALID_FD;
        }

        os_free(instance);
    }

    return 0;
}

STATIC void
__rshi_jecho(rsh_echo_t *echo, jobj_t jval)
{
    jj_u32(echo, jval, interval);
    jj_u32(echo, jval, times);
}

STATIC rsh_instance_t *
__rshi_create(char *name, jobj_t jobj)
{
    rsh_instance_t *instance = NULL;
    rsh_echo_t *echo;
    jobj_t jval;
    int i, fd, err;
    
    instance = (rsh_instance_t *)os_zalloc(sizeof(rsh_instance_t));
    if (NULL==instance) {
        goto error;
    }
    instance->name = os_strdup(name);

    echo = &instance->echo[RSH_ECHO_STATE_IDLE];
    echo->interval  = os_second(RSHA_ECHO_IDLE_INTERVAL);
    echo->times     = RSHA_ECHO_IDLE_TIMES;
    
    echo = &instance->echo[RSH_ECHO_STATE_BUSY];
    echo->interval  = os_second(RSHA_ECHO_BUSY_INTERVAL);
    echo->times     = RSHA_ECHO_BUSY_TIMES;
    
    jj_string(instance, jobj, proxy);
    if (NULL==instance->proxy) {
        goto error;
    }
    
    jj_string(instance, jobj, registry);
    if (NULL==instance->registry) {
        goto error;
    }
    
    jj_u32(instance, jobj, seq);
    instance->seq_noack = instance->seq;
    
    jj_u32(instance, jobj, cid);
    if (false==is_good_os_cert(instance->cid)) {
        goto error;
    }
    

    __jj_u32(instance, jobj, port, server.sin_port);
    if (0==instance->server.sin_port) {
        goto error;
    }
    
    jobj_t jecho = jobj_get(jobj, "echo");
    if (jecho) {
        for (i=0; i<RSH_ECHO_STATE_END; i++) {
            jval = jobj_get(jecho, rsh_echo_state_getnamebyid(i));
            if (jval) {
                __rshi_jecho(&instance->echo[i], jval);
            }
        }
    }
    
    fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (fd<0) {
        debug_error("instance %s socket error:%d", instance->name, -errno);
        
        goto error;
    }
    os_closexec(fd);
    instance->fd = fd;

    instance->client.sin_family = AF_INET;
    instance->client.sin_addr.s_addr = INADDR_ANY;
    instance->client.sin_port = 0;
    err = bind(fd, (sockaddr_t *)&instance->client, sizeof(instance->client));
    if (err<0) {
        debug_error("bind instance %s error:%d", instance->name, -errno);
        return -errno;
    }
    
    err = os_loop_add_normal(&rsha.loop, fd, rsha_recver, instance);
    if (err<0) {
        goto error;
    }
    instance->loop  = true;
    instance->jcfg  = jobj;

    return instance;
error:
    __rshi_destroy(instance);

    return NULL;
}

STATIC int
__rshi_insert(rsh_instance_t *instance)
{
    static hash_node_calc_f *nhash[RSHI_NIDX_END] = {
        [RSHI_NIDX_NAME]    = __rshi_nodehashname, 
        [RSHI_NIDX_ADDR]    = __rshi_nodehashaddr,
    };
    
    return h2_add(&rsha.head.instance, &instance->node.instance, nhash);
}

STATIC int
__rshi_remove(rsh_instance_t *instance)
{
    return h2_del(&rsha.head.instance, &instance->node.instance);
}

STATIC int
__rshi_show(rsh_instance_t *instance)
{
    if (NULL==instance) {
        return -ENOEXIST;
    }

    return 0;
}

STATIC mv_t 
__rshi_show_cb(rsh_instance_t *instance)
{
    __rshi_show(instance);

    return mv2_ok;
}

int
rshi_fsm_init(rsh_instance_t *instance)
{
    int i;

    for (i=0; i<RSH_ECHO_STATE_END; i++) {
        rshi_echo_clear(&instance->echo[i]);
    }
}

int
rshi_fsm(rsh_instance_t *instance, int fsm, time_t now)
{
    int old = instance->fsm;
    
    if (is_good_rsh_fsm(fsm)) {
        instance->fsm = fsm;
        instance->fsm_time = now?now:time(NULL);
        
        debug_entry("change instance(%s)'fsm from %s to %s",
            instance->name,
            rsh_fsm_getnamebyid(old),
            rsh_fsm_getnamebyid(fsm));

        return 0;
    } else {
        debug_error("try change instance(%s)'fsm from %s to invalid %d",
            instance->name,
            rsh_fsm_getnamebyid(old),
            fsm);

        return -EBADFSM;
    }
}

int
rshi_insert(jobj_t jobj)
{
    int err;

    char *name = jobj_get_string(jobj_get(jobj, "name"));
    if (NULL==name) {
        debug_cli("no-found name");
        
        return -EBADJSON;
    }

    if (rshi_getbyname(name)) {
        debug_entry("instance %s exist.", name);
        
        return -EEXIST;
    }
    
    rsh_instance_t *instance = __rshi_create(name, jobj);
    if (NULL==instance) {
        return -ENOMEM;
    }

    err = __rshi_insert(instance);
    if (err<0) {
        debug_entry("insert %s error:%d", name, err);
        
        return err;
    }
    
    return 0;
}

int
rshi_remove(char *name)
{
    int err;
    
    rsh_instance_t *instance = rshi_getbyname(name);
    if (NULL==instance) {
        return -ENOEXIST;
    }

    err = __rshi_remove(instance);
    if (err<0) {
        return err;
    }

    __rshi_destroy(instance);

    return 0;
}

int
rshi_foreach(mv_t (*foreach)(rsh_instance_t *instance), bool safe)
{
    return __rshi_foreach(foreach, safe);
}

rsh_instance_t *
rshi_getbyname(char *name)
{
    hash_idx_t dhash(void)
    {
        return __rshi_hashname(name);
    }
    
    bool eq(hash_node_t *node)
    {
        rsh_instance_t *instance = __rshi_hx_entry(node);
        
        return os_streq(name, instance->name);
    }
    
    return __rshi_h2_entry(h2_find(&rsha.head.instance, dhash, eq));
}

rsh_instance_t *
rshi_getbyaddr(sockaddr_in_t *addr)
{
    hash_idx_t dhash(void)
    {
        return __rshi_hashaddr(addr);
    }
    
    bool eq(hash_node_t *node)
    {
        rsh_instance_t *instance = __rshi_hx_entry(node);
        
        return addr->sin_addr.s_addr==instance->server.sin_addr.s_addr
            && addr->sin_port==instance->server.sin_port;
    }
    
    return __rshi_h2_entry(h2_find(&rsha.head.instance, dhash, eq));
}

int
rshi_show(char *name)
{
    if (name) {
        return __rshi_show(rshi_getbyname(name));
    } else {
        return rshi_foreach(__rshi_show_cb, false);
    }
}
/******************************************************************************/
