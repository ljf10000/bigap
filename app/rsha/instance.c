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
__rshi_hashsp(char *sp)
{
    return hash_bybuf(sp, os_strlen(sp), RSHI_SPHASHSIZE - 1);
}

STATIC hash_idx_t
__rshi_nodehashsp(hash_node_t *node)
{
    rsh_instance_t *instance = __rshi_hx_entry(node);

    return __rshi_hashsp(instance->sp);
}

STATIC jobj_t
__rshi_a3_cmd_dir_type_o2j(uint32 val[RSHIST_TYPE_END], void (*jadd)(jobj_t, int, uint32))
{
    jobj_t jobj = jobj_new_object();
    if (jobj) {
        int i;

        for (i=0; i<RSHIST_TYPE_END; i++) {
            jadd(jobj, i, val[i]);
        }
    }

    return jobj;
}

STATIC jobj_t
__rshi_a3_cmd_dir_o2j(uint32 val[RSHIST_DIR_END][RSHIST_TYPE_END], void (*jadd)(jobj_t, int, uint32))
{
    jobj_t jobj = jobj_new_object();
    if (jobj) {
        int i;

        for (i=0; i<RSHIST_DIR_END; i++) {
            jobj_add(jobj, rshist_dir_getnamebyid(i), __rshi_a3_cmd_dir_type_o2j(val[i], jadd));
        }
    }

    return jobj;
}

STATIC jobj_t
__rshi_a3_cmd_o2j(uint32 val[RSH_CMD_END][RSHIST_DIR_END][RSHIST_TYPE_END], void (*jadd)(jobj_t, int, uint32))
{
    jobj_t jobj = jobj_new_object();
    if (jobj) {
        int i;

        for (i=0; i<RSH_CMD_END; i++) {
            jobj_add(jobj, rsh_cmd_getnamebyid(i), __rshi_a3_cmd_dir_o2j(val[i], jadd));
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

STATIC void
__rshi_tm_a3_jadd(jobj_t jobj, int idx, uint32 val)
{
    jobj_add_string(jobj, rshist_type_getnamebyid(idx), os_fulltime_string(val));
}

STATIC jobj_t
__rshi_tm_o2j(rshi_tm_t *tm)
{
    jobj_t jobj = jobj_new_object();
    if (jobj) {
        jobj_add(jobj, "fsm", __rshi_tm_fsm_o2j(tm->fsm));
        jobj_add(jobj, "cmd", __rshi_a3_cmd_o2j(tm->cmd, __rshi_tm_a3_jadd));
        jobj_add_string(jobj, "busy", os_fulltime_string(tm->busy));
    }

    return jobj;
}

STATIC void
__rshi_st_a3_jadd(jobj_t jobj, int idx, uint32 val)
{
    jobj_add_u32(jobj, rshist_type_getnamebyid(idx), val);
}

STATIC jobj_t
__rshi_st_o2j(rshi_st_t *st)
{
    return __rshi_a3_cmd_o2j(st->val, __rshi_st_a3_jadd);
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
        jobj_add_string(jobj, "sp", instance->sp);
        
        jrule_o2j(rsh_instance_jrules(), instance, jobj);
        
        jobj_add(jobj, "echo", __rshi_echo_o2j(&instance->echo));
        jobj_add(jobj, "st", __rshi_st_o2j(&instance->st));
        jobj_add(jobj, "tm", __rshi_tm_o2j(&instance->tm));
    }
    
    return jobj;
}

STATIC int
__rshi_echo_j2o(rsh_instance_t *instance, jobj_t jobj)
{
    jobj_t jecho = jobj_get(jobj, "echo");
    if (jecho) {
        int i;
        
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
__rshi_sec_j2o(rsh_instance_t *instance, jobj_t jobj)
{
    int err;
    rshi_sec_t *sec = &instance->sec;
    
    jobj_t jsec = jobj_get(jobj, "sec");
    if (NULL==jsec) {
        return -EBADJSON;
    }

    jj_byeq(sec, jsec, crypt, rsh_crypt_getidbyname);
    jj_byeq(sec, jsec, hmactype, sha2_getidbyname);
    sec->hmacsize = SHA_DIGEST_SIZE(sec->hmactype);

    __jj_string(sec, jsec, pmk, pmkstring);
    err = rsh_key_hex2bin(rshi_pmk(instance), sec->pmkstring);
    if (err<0) {        
        return err;
    }
    
    __jj_string(sec, jsec, key, keystring);
    err = rsh_key_hex2bin(rshi_key(instance), sec->keystring);
    if (err<0) {
        return err;
    }

    return 0;
}

STATIC int
__rshi_j2o(rsh_instance_t *instance, jobj_t jobj)
{
    int err;
    
    err = __rshi_sec_j2o(instance, jobj);
    if (err<0) {
        debug_error("bad instance %s's sec");
        
        return err;
    }
    
    err = __rshi_echo_j2o(instance, jobj);
    if (err<0) {
        debug_error("bad instance %s's echo");
        
        return err;
    }
    
    err = jrule_j2o(rsh_instance_jrules(), instance, jobj);
    if (err<0) {
        return err;
    }
    
    // check
    if (NULL==instance->proxy) {
        return -EBADCONF;
    }
    else if (0==instance->port) {
        return -EBADCONF;
    }

    // repair
    if (0==instance->handshake.interval) {
        instance->handshake.interval = 30;
    }
    
    return 0;
}

STATIC int
__rshi_add_watcher(rsh_instance_t *instance)
{
    int err, fd;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd<0) {
        debug_error("instance %s socket error:%d", instance->sp, -errno);
        
        return fd;
    }
    os_closexec(fd);
    instance->fd = fd;
    debug_entry("instance %s socket %d ok", instance->sp, fd);

#if 1
    sockaddr_in_t client = OS_SOCKADDR_INET(INADDR_ANY, htons(0));
    err = bind(fd, (sockaddr_t *)&client, sizeof(client));
    if (err<0) {
        debug_error("instance %s bind error:%d", instance->sp, -errno);
        
        return -errno;
    }
#endif

    int on = 1;
    err = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (err<0) {
        debug_error("instance %s reuse error:%d", instance->sp, -errno);
        
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
        debug_entry("instance %s close socket %d ok", instance->sp, instance->fd);
        os_loop_del_watcher(&rsha.loop, instance->fd);
        os_close(instance->fd);
    }
}

STATIC int
__rshi_destroy(rsh_instance_t *instance)
{
    if (instance) {
        os_free(instance->sp);
        os_free(instance->proxy);
        os_free(instance->registry);
        os_free(instance->cache);
        os_free(instance->flash);
        os_free(instance->sec.keystring);
        os_free(instance->sec.pmkstring);

        jobj_put(instance->jcfg);
        
        __rshi_del_watcher(instance);

        os_free(instance);
    }

    return 0;
}

STATIC int
__rshi_init(rsh_instance_t *instance, char *sp, jobj_t jobj)
{
    int err;
    
    instance->fd    = INVALID_FD;
    instance->sp    = os_strdup(sp);
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
__rshi_create(char *sp, jobj_t jobj)
{
    rsh_instance_t *instance = (rsh_instance_t *)os_zalloc(sizeof(rsh_instance_t));
    if (NULL==instance) {
        goto error;
    }
    
    int err = __rshi_init(instance, sp, jobj);
    if (err<0) {
        goto error;
    }
    
    return instance;
error:
    __rshi_destroy(instance);

    return NULL;
}

STATIC int
__rshi_insert(rsh_instance_t *instance)
{
    return h1_add(&rsha.head.instance, &instance->node.instance, __rshi_nodehashsp);
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

STATIC int
rshi_fsm_init(rsh_instance_t *instance)
{
    int i, err, fd;

    __rshi_del_watcher(instance);

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
            instance->sp,
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
        instance->sp,
        rsh_fsm_getnamebyid(old),
        rsh_fsm_getnamebyid(fsm));

    return 0;
}

int
rshi_insert(char *sp, char *json)
{
    jobj_t jobj = NULL;
    int err;

    if (rshi_getbysp(sp)) {
        debug_entry("instance %s exist.", sp);
        
        err = -EEXIST; goto error;
    }
    
    jobj = jobj_byjson(json);
    if (NULL==jobj) {
        err = -EBADJSON; goto error;
    }
    
    rsh_instance_t *instance = __rshi_create(sp, jobj);
    if (NULL==instance) {
        err = -ENOMEM; goto error;
    }

    err = __rshi_insert(instance);
        debug_ok_error(err, "insert %s", sp);
    if (err<0) {
        goto error;
    }
    
    return 0;
error:
    jobj_put(jobj);

    return err;
}

int
rshi_remove(char *sp)
{
    int err;
    
    rsh_instance_t *instance = rshi_getbysp(sp);
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
rshi_getbysp(char *sp)
{
    hash_idx_t dhash(void)
    {
        return __rshi_hashsp(sp);
    }
    
    bool eq(hash_node_t *node)
    {
        rsh_instance_t *instance = __rshi_hx_entry(node);
        
        return os_streq(sp, instance->sp);
    }
    
    return __rshi_h1_entry(h1_find(&rsha.head.instance, dhash, eq));
}

int
rshi_show(char *sp)
{
    if (sp) {
        return __rshi_show(rshi_getbysp(sp));
    } else {
        return rshi_foreach(__rshi_show_cb, false);
    }
}
/******************************************************************************/
