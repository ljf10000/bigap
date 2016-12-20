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
DECLARE_DB_H1(&rsha.head.instance, __rshi, rsh_instance_t, node.instance);

static inline rsh_instance_t *
__rshi_hx_entry(hash_node_t *node);

static inline rsh_instance_t *
__rshi_h1_entry(h1_node_t *node);

static inline int
__rshi_foreach(mv_t (*foreach)(rsh_instance_t *entry), bool safe);
#endif

STATIC hash_idx_t
__rshi_hashname(char *name)
{
    return hash_bybuf(name, os_strlen(name), RSHI_NAMEHASHSIZE - 1);
}

STATIC hash_idx_t
__rshi_nodehashname(hash_node_t *node)
{
    rsh_instance_t *instance = __rshi_hx_entry(node);

    return __rshi_hashname(instance->name);
}

STATIC jobj_t
__rshi_tm_dir_type_o2j(time_t tm[RSHIST_TYPE_END])
{
    jobj_t jobj = jobj_new_object();
    if (jobj) {
        int i;
        
        for (i=0; i<RSHIST_TYPE_END; i++) {
            jobj_add_string(jobj, 
                rshist_type_getnamebyid(i), 
                os_fulltime_string(tm[i]));
        }
    }
    
    return jobj;
}

STATIC jobj_t
__rshi_tm_dir_o2j(time_t tm[RSHIST_DIR_END][RSHIST_TYPE_END])
{
    jobj_t jobj = jobj_new_object();
    if (jobj) {
        int i;
        
        for (i=0; i<RSHIST_DIR_END; i++) {
            jobj_add(jobj, rshist_dir_getnamebyid(i), __rshi_tm_dir_type_o2j(tm[i]));
        }
    }
    
    return jobj;
}

STATIC jobj_t
__rshi_tm_fsm_o2j(time_t fsm[RSH_FSM_END])
{
    jobj_t jobj = jobj_new_object();
    if (jobj) {
        int i;
        
        for (i=0; i<RSH_FSM_END; i++) {
            jobj_add_string(jobj, rsh_fsm_getnamebyid(i), os_fulltime_string(fsm[i]));
        }
    }

    return jobj;
}

STATIC jobj_t
__rshi_tm_update_o2j(time_t update[RSH_UPDATE_END][RSHIST_DIR_END][RSHIST_TYPE_END])
{
    jobj_t jobj = jobj_new_object();
    if (jobj) {
        int i;
        
        for (i=0; i<RSH_UPDATE_END; i++) {
            jobj_add(jobj, rsh_update_getnamebyid(i), __rshi_tm_dir_o2j(update[i]));
        }
    }

    return jobj;
}

STATIC jobj_t
__rshi_tm_echo_o2j(time_t echo[RSHIST_DIR_END][RSHIST_TYPE_END])
{
    return __rshi_tm_dir_o2j(echo);
}

STATIC jobj_t
__rshi_tm_command_o2j(time_t command[RSHIST_DIR_END][RSHIST_TYPE_END])
{
    return __rshi_tm_dir_o2j(command);
}

STATIC jobj_t
__rshi_tm_o2j(rshi_tm_t *tm)
{
    jobj_t jobj = jobj_new_object();
    if (jobj) {
        jobj_add(jobj, "fsm", __rshi_tm_fsm_o2j(tm->fsm));
        jobj_add(jobj, "update", __rshi_tm_update_o2j(tm->update));
        jobj_add(jobj, "command", __rshi_tm_command_o2j(tm->command));
        jobj_add(jobj, "echo", __rshi_tm_echo_o2j(tm->echo));
        jobj_add_string(jobj, "busy", os_fulltime_string(tm->busy));
    }

    return jobj;
}

STATIC jobj_t
__rshi_st_cmd_dir_type_o2j(uint32 val[RSHIST_TYPE_END])
{
    jobj_t jobj = jobj_new_object();
    if (jobj) {
        int i;

        for (i=0; i<RSHIST_TYPE_END; i++) {
            jobj_add_u32(jobj, rshist_type_getnamebyid(i), val[i]);
        }
    }

    return jobj;
}

STATIC jobj_t
__rshi_st_cmd_dir_o2j(uint32 val[RSHIST_DIR_END][RSHIST_TYPE_END])
{
    jobj_t jobj = jobj_new_object();
    if (jobj) {
        int i;

        for (i=0; i<RSHIST_DIR_END; i++) {
            jobj_add(jobj, rshist_dir_getnamebyid(i), __rshi_st_cmd_dir_type_o2j(val[i]));
        }
    }

    return jobj;
}

STATIC jobj_t
__rshi_st_cmd_o2j(uint32 val[RSH_CMD_END][RSHIST_DIR_END][RSHIST_TYPE_END])
{
    jobj_t jobj = jobj_new_object();
    if (jobj) {
        int i;

        for (i=0; i<RSH_CMD_END; i++) {
            jobj_add(jobj, rsh_cmd_getnamebyid(i), __rshi_st_cmd_dir_o2j(val[i]));
        }
    }

    return jobj;
}

STATIC jobj_t
__rshi_st_o2j(rshi_st_t *st)
{
    return __rshi_st_cmd_o2j(st->val);
}

STATIC jobj_t
__rshi_echo_o2j(rshi_echo_t *echo)
{
    jobj_t jobj = jobj_new_object();
    if (jobj) {
        int i;
        
        for (i=0; i<RSH_ECHO_END; i++) {
            jobj_add(jobj, rsh_echo_getnamebyid(i), rsh_echo_o2j(&echo->echo[i]));
        }

        jobj_add_bool(jobj, "busy", echo->busy);
    }

    return jobj;
}

STATIC jobj_t
__rshi_o2j(rsh_instance_t *instance)
{
    jobj_t jobj = jobj_new_object();
    if (jobj) {
        jrule_o2j(rsh_instance_jrules(), instance, jobj);
        
        jobj_add(jobj, "echo", __rshi_echo_o2j(&instance->echo));
        jobj_add(jobj, "st", __rshi_st_o2j(&instance->st));
        jobj_add(jobj, "tm", __rshi_tm_o2j(&instance->tm));
    }
    
    return jobj;
}

STATIC int
__rshi_j2o(rsh_instance_t *instance, jobj_t jobj)
{
    jobj_t jecho;
    int i, err;

    err = jrule_j2o(rsh_instance_jrules(), instance, jobj);
    if (err<0) {
        return err;
    }
    
    jecho = jobj_get(jobj, "echo");
    if (jecho) {
        for (i=0; i<RSH_ECHO_END; i++) {
            rsh_echo_j2o(rshi_echo_getby(instance, i), jobj_get(jecho, rsh_echo_getnamebyid(i)));
        }
    } else {
        rsh_echo_t *echo;

        echo = rshi_echo_getby(instance, RSH_ECHO_IDLE);
        echo->interval  = os_second(RSHA_ECHO_IDLE_INTERVAL);
        echo->times     = RSHA_ECHO_IDLE_TIMES;
        
        echo = rshi_echo_getby(instance, RSH_ECHO_BUSY);
        echo->interval  = os_second(RSHA_ECHO_BUSY_INTERVAL);
        echo->times     = RSHA_ECHO_BUSY_TIMES;
    }

    return 0;
}

STATIC int
__rshi_add_watcher(rsh_instance_t *instance)
{
    int err, fd;

    fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (fd<0) {
        debug_error("instance %s socket error:%d", instance->name, -errno);
        
        return fd;
    }
    os_closexec(fd);
    instance->fd = fd;
    
    sockaddr_in_t client = OS_SOCKADDR_INET(INADDR_ANY, 0);
    err = bind(fd, (sockaddr_t *)&client, sizeof(client));
    if (err<0) {
        debug_error("bind instance %s error:%d", instance->name, -errno);
        
        return -errno;
    }

    err = os_loop_add_normal(&rsha.loop, fd, rsha_recver, instance);
    if (err<0) {
        return err;
    }
    
    return 0;
}

STATIC void
__rshi_del_watcher(rsh_instance_t *instance)
{
    if (is_good_fd(instance->fd)) {
        os_loop_del_watcher(&rsha.loop, instance->fd);
        os_close(instance->fd);
    }
}

STATIC int
__rshi_destroy(rsh_instance_t *instance)
{
    if (instance) {
        os_free(instance->name);
        os_free(instance->proxy);
        os_free(instance->registry);
        os_free(instance->cache);
        os_free(instance->flash);
        os_free(instance->key.hex);

        jobj_put(instance->jcfg);
        
        __rshi_del_watcher(instance);

        os_free(instance);
    }

    return 0;
}

STATIC int
__rshi_init(rsh_instance_t *instance, char *name, jobj_t jobj)
{
    int err;
    
    instance->fd    = INVALID_FD;
    instance->name  = os_strdup(name);
    instance->peer_error_max = RSHI_PEER_ERROR_MAX;
    
    err = __rshi_j2o(instance, jobj);
    if (err<0) {
        return err;
    }
    instance->jcfg  = jobj;

    rshi_fsm_restart(instance, time(NULL));

    return 0;
}

STATIC rsh_instance_t *
__rshi_create(char *name, jobj_t jobj)
{
    rsh_instance_t *instance = (rsh_instance_t *)os_zalloc(sizeof(rsh_instance_t));
    if (NULL==instance) {
        return NULL;
    }
    
    int err = __rshi_init(instance, name, jobj);
    if (err<0) {
        __rshi_destroy(instance);

        return NULL;
    }
    
    return instance;
}

STATIC int
__rshi_insert(rsh_instance_t *instance)
{
    return h1_add(&rsha.head.instance, &instance->node.instance, __rshi_nodehashname);
}

STATIC int
__rshi_remove(rsh_instance_t *instance)
{
    return h1_del(&rsha.head.instance, &instance->node.instance);
}

STATIC int
__rshi_show(rsh_instance_t *instance)
{
    if (NULL==instance) {
        return -ENOEXIST;
    }

    jobj_t jobj = __rshi_o2j(instance);
    if (NULL==jobj) {
        return -EOBJ2JSON;
    }
    
    cli_sprintf("%s", jobj_json(jobj));
    jobj_put(jobj);
    
    return 0;
}

STATIC mv_t 
__rshi_show_cb(rsh_instance_t *instance)
{
    __rshi_show(instance);

    return mv2_ok;
}

rsh_echo_t *
rshi_echo_set(rsh_instance_t *instance, time_t now, bool busy)
{
    if (instance->echo.busy != busy) {
        rsh_echo_t *old = rshi_echo_get(instance);
        instance->echo.busy = busy;
        rsh_echo_t *new = rshi_echo_get(instance);

        new->send = old->send;
        new->recv = old->recv;
    }

    if (busy) {
        instance->tm.busy = now;
    }
    
    return rshi_echo_get(instance);
}

int
rshi_key_init(rsh_key_t *key, char *keystring)
{
    int err, len = os_strlen(keystring), size = len/2;
    
    if (len%2 || false==is_good_aes_key_size(size)) {
        err = -EBADKEY; goto error;
    }

    err = os_hex2bin(keystring, key->u.key, size);
    if (err<0) {
        err = -EBADKEY; goto error;
    }
    key->size = size;
    
    return 0;
error:

    return err;
}

rsh_key_t *
rshi_key_setup(rsh_instance_t *instance, rsh_key_t *key, time_t now)
{
    instance->key.current = !instance->key.current;

    rsh_key_t *current = rshi_key_get(instance);

    current->size = key->size;
    aes_key_setup(key->u.key, current->u.key32, 8*key->size);

    instance->key.update++;
    
    return current;
}

STATIC int
rshi_fsm_init(rsh_instance_t *instance)
{
    int i, err, fd;

    if (is_good_fd(instance->fd)) {
        os_loop_del_watcher(&rsha.loop, instance->fd);
        os_close(instance->fd);
    }

    for (i=0; i<RSH_ECHO_END; i++) {
        rshi_echo_clear(rshi_echo_getby(instance, i));
    }

    err = __rshi_add_watcher(instance);
    if (err<0) {
        return err;
    }

    return 0;
}

int
rshi_fsm(rsh_instance_t *instance, int fsm, time_t now)
{
    int old = instance->fsm;
    
    if (false==is_good_rsh_fsm(fsm)) {
        debug_error("try change instance(%s)'fsm from %s to invalid %d",
            instance->name,
            rsh_fsm_getnamebyid(old),
            fsm);

        return -EBADFSM;
    }
    
    instance->fsm = fsm;
    instance->tm.fsm[fsm] = now;
    
    switch(fsm) {
        case RSH_FSM_INIT:
            rshi_fsm_init(instance);
            break;
        default:
            break;
    }

    debug_entry("change instance(%s)'fsm from %s to %s",
        instance->name,
        rsh_fsm_getnamebyid(old),
        rsh_fsm_getnamebyid(fsm));

    return 0;
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
    
    return __rshi_h1_entry(h1_find(&rsha.head.instance, dhash, eq));
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
