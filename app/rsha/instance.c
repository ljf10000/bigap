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
__rshi_tm_o2j(rsh_instance_t *instance)
{
    jobj_t jobj = NULL, jval;
    int i;
    
    jobj = jobj_new_object();
    if (NULL==jobj) {
        return NULL;
    }
    
    for (i=0; i<RSH_FSM_END; i++) {
        jobj_add_string(jobj, rsh_fsm_getnamebyid(i), os_fulltime_string(instance->fsm_time[i]));
    }

    return jobj;
}

STATIC jobj_t
__rshi_st_o2j(rshi_st_t *st)
{
    jobj_t jobj = NULL, jrun, jcmd, jdir;
    int cmd, dir, type;
    
    jobj = jobj_new_object();
    if (NULL==jobj) {
        return NULL;
    }
    
    jrun = jobj_new_object();
    if (NULL==jrun) {
        goto error;
    }
    jobj_add(jobj, "run", jrun);

    for (cmd=0; cmd<RSH_CMD_END; cmd++) {
        jcmd = jobj_new_object();
        if (NULL==jcmd) {
            goto error;
        }
        jobj_add(jrun, rsh_cmd_getnamebyid(cmd), jcmd);
        
        for (dir=0; dir<RSHIST_DIR_END; dir++) {
            jdir = jobj_new_object();
            if (NULL==jdir) {
                goto error;
            }
            jobj_add(jcmd, rshist_dir_getnamebyid(dir), jdir);
            
            for (type=0; type<RSHIST_TYPE_END; type++) {
                jobj_add_u32(jdir, rshist_type_getnamebyid(type), 
                    st->val[cmd][dir][type]);
            }
        }
    }

    return jobj;
error:
    jobj_put(jobj);

    return NULL;
}

STATIC jobj_t
rshi_echo_o2j(rsh_instance_t *instance)
{
    jobj_t jobj = NULL;
    int i, err;
    
    jobj = jobj_new_object();
    if (NULL==jobj) {
        return NULL;
    }

    for (i=0; i<RSH_ECHO_END; i++) {
        jobj_add(jobj, rsh_echo_getnamebyid(i), __rshi_echo_o2j(&instance->echo[i]));
    }

    return jobj;
}

STATIC jobj_t
__rshi_o2j(rsh_instance_t *instance)
{
    jobj_t jobj = NULL;
    int err;
    
    jobj = jobj_new_object();
    if (NULL==jobj) {
        return NULL;
    }
    
    err = jrule_o2j(rsh_instance_jrules(), instance, jobj);
    if (err<0) {
        goto error;
    }

    jobj_add(jobj, "echo", rshi_echo_o2j(instance));
    jobj_add(jobj, "st", __rshi_st_o2j(&instance->st));
    jobj_add(jobj, "tm", __rshi_tm_o2j(&instance->tm));

    return jobj;
error:
    jobj_put(jobj);

    return NULL;
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
            __rshi_echo_j2o(&instance->echo[i], jobj_get(jecho, rsh_echo_getnamebyid(i)));
        }
    } else {
        rsh_echo_t *echo;

        echo = &instance->echo[RSH_ECHO_IDLE];
        echo->interval  = os_second(RSHA_ECHO_IDLE_INTERVAL);
        echo->times     = RSHA_ECHO_IDLE_TIMES;
        
        echo = &instance->echo[RSH_ECHO_BUSY];
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
    if (instance->echo_busy != busy) {
        rsh_echo_t *old = rshi_echo_get(instance);
        instance->echo_busy = busy;
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
        rshi_echo_clear(&instance->echo[i]);
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

STATIC void 
rshi_encode(rsh_instance_t *instance)
{
    rsh_msg_t *msg = rsha_msg;

    rsh_msg_encode(msg, rsh_msg_len(msg), 
        rshi_key_get(instance), 
        msg->hmac, msg->hmacsize);
}

STATIC void 
rshi_decode(rsh_instance_t *instance, int len)
{
    rsh_msg_t *msg = rsha_msg;

    rsh_msg_decode(msg, len, 
        rshi_key_get(instance), 
        instance->hmac, instance->hmacsize);
}

STATIC int 
rshi_recv(rsh_instance_t *instance)
{
    rsh_msg_t *msg = rsha_msg;
    sockaddr_in_t addr = OS_SOCKADDR_INET(0, 0);
    socklen_t addrlen = sizeof(addr);
    int err, len;

    err = len = __io_recvfrom(instance->fd, msg, RSH_MSG_ALLSIZE, 0,
        (sockaddr_t *)&addr, &addrlen);
    if (err<0) {
        return err;
    }
    else if (false==is_good_rsh_msg_size(len)) {
        return -EBADSIZE;
    }
    else if (false==is_rshi_server_address(instance, &addr)) {
        return -EFAKESEVER;
    }
    
    rshi_decode(instance, len);
    
    return len;
}

STATIC int 
rshi_send(rsh_instance_t *instance)
{
    rsh_msg_t *msg = rsha_msg;
    sockaddr_in_t proxy = OS_SOCKADDR_INET(instance->ip, instance->port);
    rsh_msg_t rawmsg = {
        .cmd    = msg->cmd,
        .update = rsh_msg_update_type(msg),
    };
    int size    = rsh_msg_size(msg);
    int err;
    
    rshi_encode(instance);
    
    err = io_sendto(instance->fd, msg, size, (sockaddr_t *)&proxy, sizeof(proxy));
    if (err<0) {
        goto error;
    }
    else if (err!=size) {
        err = -EIO; goto error;
    }
    
    err = size;
error:
    rshi_send_over(instance, &rawmsg, err<0);
    
    return err;
}

STATIC int
rshi_ack(rsh_instance_t *instance, int error, void *buf, int len)
{
    rsh_msg_t emsg;
    rsh_msg_t *msg = rsha_msg;
    uint32 seq_peer = msg->seq;
    int err;
    
    if (error) {
        os_objcpy(&emsg, msg);
    }

    /*
    * zero first
    */
    os_objzero(msg);

    msg->flag       = RSH_F_ACK;
    msg->error      = error;
    msg->seq        = instance->seq;
    msg->hmacsize   = instance->hmacsize;
    msg->ack        = seq_peer;
    
    if (error) {
        instance->peer_error++;
        instance->error = error;

        rsh_hdr_hton(&emsg);
        rsh_msg_append(msg, &emsg, sizeof(emsg));
    }

    rsh_msg_fill(msg, rsha.mac, buf, len);
    
    return rshi_send(instance);
}

STATIC int
rshi_ack_ok(rsh_instance_t *instance)
{
    return rshi_ack(instance, 0, NULL, 0);
}

STATIC int
rshi_ack_error(rsh_instance_t *instance, int error, const char *fmt, ...)
{
    va_list args;
    jobj_t jobj = NULL;
    rsh_msg_t *msg = rsha_msg;
    int err;

    if (is_rsh_msg_ack(msg)) {
        /*
        * not report ack's error
        */
        return 0;
    }
    
    jobj = jobj_new_object();
    
    va_start(args, (char *)fmt);
    err = jobj_vsprintf(jobj, "error", fmt, args);
    va_end(args);
    if (err<0) {
        goto error;
    }

    char *json = jobj_json(jobj);
    err = rshi_ack(instance, error, json, os_strlen(json));
    if (err<0) {
        goto error;
    }

    err = error;
error:
    jobj_put(jobj);

    return err;
}

STATIC int
rshi_exec(rsh_instance_t *instance, char *json)
{
    jobj_t jobj = NULL;
    int err;

    jobj = jobj_byjson(json);
    if (NULL==jobj) {
        return -EBADJSON;
    }
    
    jobj_t jinstance = jobj_new_object();
    
    jobj_add(jinstance, "name",     jobj_new_string(instance->name));
    jobj_add(jinstance, "cache",    jobj_new_string(instance->cache));
    jobj_add(jobj, "instance", jinstance);

    err = jscript_exec(jobj_json(jobj));
    
    jobj_put(jobj);

    return err;
}

STATIC int 
rshi_echo_recv_checker(rsh_instance_t *instance, time_t now)
{
    rsh_msg_t *msg = rsha_msg;
    
    if (false==is_rsh_msg_ack(msg)) {
        return rshi_ack_error(instance, -RSH_E_FLAG, 
            "time[%s] echo reply ack[0x%x] without ack flag", 
            os_fulltime_string(now),
            msg->ack);
    }

    return 0;
}

STATIC int 
rshi_noack_recv_checker(rsh_instance_t *instance, time_t now)
{
    rsh_msg_t *msg = rsha_msg;
    
    if (is_rsh_msg_ack(msg)) {
        return rshi_ack_error(instance, -RSH_E_FLAG, 
            "time[%s] command with ack flag",
            os_fulltime_string(now));
    }

    return 0;
}

STATIC int 
rshi_update_recv_checker(rsh_instance_t *instance, time_t now)
{
    rsh_msg_t *msg = rsha_msg;
    rsh_update_t *update = rsh_msg_update(msg);
    
    if (is_rsh_msg_ack(msg)) {
        return rshi_ack_error(instance, -RSH_E_FLAG, 
            "time[%s] update with ack flag",
            os_fulltime_string(now));
    }
    else if (false==is_good_rsh_update(update->type)) {
        return rshi_ack_error(instance, -RSH_E_FLAG, 
            "time[%s] invalid update[%d]",
            os_fulltime_string(now),
            update->type);
    }
    
    return 0;
}

STATIC void 
rshi_echo_recver(rsh_instance_t *instance, time_t now)
{
    rshi_echo_get(instance)->recv = now;
}

STATIC void 
rshi_command_recver(rsh_instance_t *instance, time_t now)
{
    rsh_msg_t *msg = rsha_msg;

    rshi_echo_set(instance, now, true);
    rshi_exec(instance, rsh_msg_body(msg));
}

STATIC void 
rshi_update_recver(rsh_instance_t *instance, time_t now)
{
    rsh_msg_t *msg = rsha_msg;

    instance->update_time = now;
    rshi_key_setup(instance, rsh_msg_key(msg), now);
}

STATIC int 
rshi_recv_checker(rsh_instance_t *instance, time_t now, rsh_msg_t *msg, int len)
{
    static int (*checker[RSH_CMD_END])(rsh_instance_t *instance, time_t now) = {
        [RSH_CMD_ECHO]      = rshi_echo_recv_checker,
        [RSH_CMD_COMMAND]   = rshi_noack_recv_checker,
        [RSH_CMD_UPDATE]    = rshi_update_recv_checker,
    };
    
    int size = rsh_msg_size(msg);

    if (instance->hmacsize != msg->hmacsize) {
        return rshi_ack_error(instance, -RSH_E_HMAC, 
            "time[%s] invalid hmac size",
            os_fulltime_string(now));
    }
    else if (false==os_memeq(instance->hmac, msg->hmac, instance->hmacsize)) {
        return rshi_ack_error(instance, -RSH_E_HMAC, 
            "time[%s] invalid hmac",
            os_fulltime_string(now));
    }
    else if (false==is_valid_rsh_cmd(msg->cmd)) {
        return rshi_ack_error(instance, -RSH_E_CMD, 
            "time[%s] invalid cmd[%d]",
            os_fulltime_string(now),
            msg->cmd);
    }
    else if (len != size) {
        return rshi_ack_error(instance, -RSH_E_LEN, 
            "time[%s] recv len[%d] not match msg size[%d]", 
            os_fulltime_string(now),
            len, size);
    }
    else if (RSH_VERSION != msg->version) {
        return rshi_ack_error(instance, -RSH_E_VERSION, 
            "time[%s] invalid version[%d]", 
            os_fulltime_string(now),
            msg->version);
    }
    else if (false==os_maceq(msg->mac, rsha.mac)) {
        return rshi_ack_error(instance, -RSH_E_MAC, 
            "time[%s] invalid mac[%s]", 
            os_fulltime_string(now),
            os_macstring(msg->mac));
    }
    else if (msg->seq != instance->seq_peer) {
        return rshi_ack_error(instance, -RSH_E_SEQ, 
            "time[%s] seq[0x%x] not match instance[seq_peer:0x%x]", 
            os_fulltime_string(now),
            msg->seq,
            instance->seq_peer);
    }
    else if (msg->ack != instance->seq_noack) {
        return rshi_ack_error(instance, -RSH_E_ACK, 
            "time[%s] ack[0x%x] not match instance[seq_noack:0x%x]", 
            os_fulltime_string(now),
            msg->ack,
            instance->seq_noack);
    }
    else {
        return (*checker[msg->cmd])(instance, now);
    }
}

STATIC void 
rshi_recv_ok(rsh_instance_t *instance, time_t now)
{
    rsh_msg_t *msg = rsha_msg;
    
    /*
    * now, msg is ok, save next peer msg
    */
    instance->seq_peer = 1 + msg->seq;

    if (false==is_rsh_msg_ack(msg)) {
        rshi_ack_ok(instance);
    }
}

int 
rsha_recver(loop_watcher_t *watcher, time_t now)
{
    static void (*recver[RSH_CMD_END])(rsh_instance_t *instance, time_t now) = {
        [RSH_CMD_ECHO]      = rshi_echo_recver,
        [RSH_CMD_COMMAND]   = rshi_command_recver,
        [RSH_CMD_UPDATE]    = rshi_update_recver,
    };
    
    rsh_instance_t *instance = (rsh_instance_t *)watcher->user;
    rsh_msg_t *msg = rsha_msg;
    rsh_msg_t rawmsg = {
        .cmd    = RSH_CMD_UNKNOW,
        .update = RSH_UPDATE_END,
    };
    int err, len;

    err = len = rshi_recv(instance);
    if (err<0) {
        goto error;
    }
    rawmsg.cmd      = msg->cmd;
    rawmsg.update   = rsh_msg_update_type(msg);
    
    err = rshi_recv_checker(instance, now, msg, len);
    if (err<0) {
        goto error;
    }

    rshi_recv_ok(instance, now);
    
    (*recver[msg->cmd])(instance, now);
error:
    rshi_recv_over(instance, &rawmsg, err<0);

    return err;
}

int 
rshi_echo(rsh_instance_t *instance, time_t now)
{
    rsh_msg_t *msg = rsha_msg;
    int err;

    /*
    * zero first
    */
    os_objzero(msg);
    rsh_msg_fill(msg, rsha.mac, NULL, 0);

    msg->cmd        = RSH_CMD_ECHO;
    msg->flag       = 0;
    msg->seq        = instance->seq++;
    msg->ack        = instance->seq_peer; // first is 0
    msg->hmacsize   = instance->hmacsize;
    
    err = rshi_send(instance);
    if (err<0) {
        return err;
    }

    rshi_echo_get(instance)->send = now;

    return 0;
}

int
rshi_resolve(rsh_instance_t *instance, time_t now)
{
    instance->ip = os_ipdns(instance->proxy);
    if (is_good_ipaddr(instance->ip)) {
        rshi_fsm(instance, RSH_FSM_RESOLVED, now);
    }

    return 0;
}

int 
rshi_run(rsh_instance_t *instance, time_t now)
{
    return rshi_fsm(instance, RSH_FSM_RUN, now);
}

STATIC jobj_t 
rshi_register_pre(rsh_instance_t *instance)
{
    jobj_t jobj = jobj_new_object();
    if (NULL==jobj) {
        return NULL;
    }

    instance->crypt     = RSH_CRYPT_AES;
    instance->hmactype  = SHA_256;
    instance->seq = instance->seq_noack = rshi_seq_rand();

    jobj_add_string(jobj, "mac", rsha.macstring);
    jobj_add_string(jobj, "crypt", rsh_crypt_getnamebyid(instance->crypt));
    jobj_add_string(jobj, "hmactype", sha2_getnamebyid(instance->hmactype));
    jobj_add_u32(jobj, "seq",  instance->seq);

    return jobj;
error:
    jobj_put(jobj);

    return NULL;
}

STATIC int 
rshi_register_post(rsh_instance_t *instance, jobj_t jobj, time_t now)
{
    int err;

    /*
    * 1. get key
    */
    jobj_t jkey = jobj_get(jobj, "key");
    if (NULL==jkey) {
        return -EBADJSON;
    }

    rsh_key_t key;
    char *keyhex = jobj_get_string(jkey);
    
    err = rshi_key_init(&key, keyhex);
    if (err<0) {
        return err;
    }

    rshi_key_setup(instance, &key, now);
    instance->key.hex = os_strdup(keyhex);

    /*
    * 2. get crypt
    */
    jobj_t jcrypt = jobj_get(jobj, "crypt");
    if (NULL==jcrypt) {
        return -EBADJSON;
    }
    int crypt = rsh_crypt_getidbyname(jobj_get_string(jcrypt));
    if (false==is_good_rsh_crypt(crypt)) {
        return -EBADJSON;
    }
    instance->crypt = crypt;

    /*
    * 3. get hmac type
    */
    jobj_t jhmactype = jobj_get(jobj, "hmactype");
    if (NULL==jhmactype) {
        return -EBADJSON;
    }
    int hmactype = sha2_getidbyname(jobj_get_string(jhmactype));
    if (false==is_good_sha2(hmactype)) {
        return -EBADJSON;
    }
    instance->hmactype = hmactype;
    instance->hmacsize = SHA_DIGEST_SIZE(hmactype);
    
    /*
    * 4. get seq
    */
    jobj_t jseq = jobj_get(jobj, "seq");
    if (NULL==jseq) {
        return -EBADJSON;
    }
    instance->seq_peer = jobj_get_u32(jseq);
    
    /*
    * 5. get seq
    */
    jobj_t jdebug = jobj_get(jobj, "debug");
    if (NULL==jdebug) {
        return -EBADJSON;
    }
    instance->debug = jobj_get_bool(jdebug);
    
    return 0;
}

int 
rshi_register(rsh_instance_t *instance, time_t now)
{
    static char line[1+OS_FILE_LEN];
    jobj_t jinput = NULL, joutput = NULL;
    int err, len;

    /*
    * 1. prepare cert
    */
    char fcert[1+FCOOKIE_FILE_LEN] = {0};
    char  fkey[1+FCOOKIE_FILE_LEN] = {0};

    if (NULL==fcookie_cert(instance->cid, fcert) || 
        NULL==fcookie_key(instance->cid, fkey)) {
        err = -ENOCERT; goto error;
    }

    /*
    * 2. prepare input
    */
    jinput = rshi_register_pre(instance);
    if (NULL==jinput) {
        err = -ENOMEM; goto error;
    }
    
    /*
    * 3. do register
    */
    os_arrayzero(line);
    err = os_v_pgets(line, sizeof(line), 
            "curl"
                " -d '%s'"
                " -k --cert %s --key %s"
                " -s %s",
            jobj_json(jinput),
            fcert, fkey,
            instance->registry);
    if (err<0) {
        debug_error("curl err(%d)", err);
        err = -ECURLFAIL; goto error;
    }
    
    joutput = jobj_byjson(line);
    if (NULL==joutput) {
        debug_error("bad json=%s", line);
        err = -EBADJSON; goto error;
    }
    
    /*
    * 4. analysis output
    */
    err = rshi_register_post(instance, joutput, now);
    if (err<0) {
        goto error;
    }
    
    /*
    * 5. switch fsm
    */
    err = rshi_fsm(instance, RSH_FSM_REGISTERED, now);
error:
    jobj_put(jinput);
    jobj_put(joutput);
    fcookie_put_file(fcert);
    fcookie_put_file(fkey);
    
    return err;
}

/******************************************************************************/
