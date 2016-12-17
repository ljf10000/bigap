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
                    st->run.val[cmd][dir][type]);
            }
        }
    }

    return jobj;
error:
    jobj_put(jobj);

    return NULL;
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

    jobj_add(jobj, "idle", rshi_echo_o2j(&instance->idle));
    jobj_add(jobj, "busy", rshi_echo_o2j(&instance->busy));
    jobj_add(jobj, "st", __rshi_st_o2j(&instance->st));

    return jobj;
error:
    jobj_put(jobj);

    return NULL;
}

STATIC int
__rshi_j2o(rsh_instance_t *instance, jobj_t jobj)
{
    jobj_t jecho;
    int err;

    err = jrule_j2o(rsh_instance_jrules(), instance, jobj);
    if (err<0) {
        return err;
    }
    
    jecho = jobj_get(jobj, "echo");
    if (jecho) {
        rshi_echo_j2o(&instance->idle, jobj_get(jecho, "idle"));
        rshi_echo_j2o(&instance->busy, jobj_get(jecho, "busy"));
    } else {
        instance->idle.interval  = os_second(RSHA_ECHO_IDLE_INTERVAL);
        instance->idle.times     = RSHA_ECHO_IDLE_TIMES;
        instance->busy.interval  = os_second(RSHA_ECHO_BUSY_INTERVAL);
        instance->busy.times     = RSHA_ECHO_BUSY_TIMES;
    }

    return 0;
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
        os_free(instance->key);

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

STATIC int
__rshi_init(rsh_instance_t *instance, char *name, jobj_t jobj)
{
    int err;
    
    instance->seq_noack = instance->seq = rand() % 0xffff;
    instance->name  = os_strdup(name);
    instance->peer_error_max = RSHI_PEER_ERROR_MAX;

    err = __rshi_j2o(instance, jobj);
    if (err<0) {
        return err;
    }

    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
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
    
    instance->loop  = true;
    instance->jcfg  = jobj;

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

STATIC void
rshi_fsm_init(rsh_instance_t *instance)
{
    rshi_echo_clear(&instance->idle);
    rshi_echo_clear(&instance->busy);
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
    instance->fsm_time = now?now:time(NULL);
    
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
    rsh_secret_t *secret = rshi_secret_get(instance);
    rsh_msg_t *msg = rsha_msg;
    
    rsh_msg_encode(msg, secret->u.key32, secret->size);
}

STATIC void 
rshi_decode(rsh_instance_t *instance, int len)
{
    rsh_secret_t *secret = rshi_secret_get(instance);
    rsh_msg_t *msg = rsha_msg;

    rsh_msg_decode(msg, instance->md5, secret->u.key32, secret->size);
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
    int err, size = rsh_msg_size(msg), cmd = msg->cmd;
    
    rshi_encode(instance);
    
    err = io_sendto(instance->fd, msg, size, (sockaddr_t *)&proxy, sizeof(proxy));
    if (err<0) {
        goto error;
    }
    else if (err!=size) {
        err = -EIO; goto error;
    }
    
    return size;
error:
    rshi_st_send(instance, cmd, err<0);
    
    return err;
}

STATIC int
rshi_ack(rsh_instance_t *instance, int error, void *buf, int len)
{
    rsh_msg_t hdr;
    rsh_msg_t *msg = rsha_msg;
    int err;

    if (error) {
        os_objcpy(&hdr, msg);
    }
    
    os_objzero(msg);

    msg->flag   = RSH_F_ACK;
    msg->seq    = instance->seq;
    msg->error  = error;

    if (error) {
        msg->ack = instance->seq_peer_temp;
        
        instance->peer_error++;
        instance->error = error;

        rsh_hdr_hton(&hdr);
        rsh_msg_append(msg, &hdr, sizeof(hdr));
    } else {
        msg->ack = instance->seq_peer;
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
    jobj_t jobj = jobj_new_object();
    int err;
    
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
    
    if (false==os_hasflag(msg->flag, RSH_F_ACK)) {
        return rshi_ack_error(instance, -RSH_E_FLAG, 
            "echo reply[ack:0x%x] no ack flag", 
            msg->ack);
    }

    return 0;
}

STATIC int 
rshi_recv_nochecker(rsh_instance_t *instance, time_t now)
{
    return 0;
}

STATIC int 
rshi_noack_recv_checker(rsh_instance_t *instance, time_t now)
{
    rsh_msg_t *msg = rsha_msg;
    
    if (os_hasflag(msg->flag, RSH_F_ACK)) {
        return rshi_ack_error(instance, -RSH_E_FLAG, 
            "command with ack flag");
    }

    return 0;
}

STATIC int 
rshi_echo_recver(rsh_instance_t *instance, time_t now)
{
    rshi_echo_get(instance)->recv = now;
    
    return 0;
}

STATIC int 
rshi_command_recver(rsh_instance_t *instance, time_t now)
{
    rsh_msg_t *msg = rsha_msg;

    instance->command_time = now;
    rshi_ack_ok(instance);
    rshi_exec(instance, msg->body);

    return 0;
}

STATIC int 
rshi_updatekey_recver(rsh_instance_t *instance, time_t now)
{
    rsh_msg_t *msg = rsha_msg;

    rshi_ack_ok(instance);
    rshi_secret_setup(instance, rsh_msg_secret(msg), now);

    return 0;
}

STATIC int 
rshi_recv_checker(rsh_instance_t *instance, time_t now, rsh_msg_t *msg, int len)
{
    static int (*checker[RSH_CMD_END])(rsh_instance_t *instance, time_t now) = {
        [RSH_CMD_ECHO]      = rshi_echo_recv_checker,
        [RSH_CMD_COMMAND]   = rshi_noack_recv_checker,
        [RSH_CMD_UPDATEKEY] = rshi_noack_recv_checker,
    };
    
    int size = rsh_msg_size(msg);

    if (false==md5_eq(instance->md5, msg->md5)) {
        return rshi_ack_error(instance, -RSH_E_MD5, 
            "invalid md5");
    }
    else if (false==is_valid_rsh_cmd(msg->cmd)) {
        return rshi_ack_error(instance, -RSH_E_CMD, 
            "invalid cmd %d", 
            msg->cmd);
    }
    else if (len != size) {
        return rshi_ack_error(instance, -RSH_E_LEN, 
            "recv len %d not match msg size %d", 
            len, size);
    }
    else if (RSH_MAGIC != msg->magic) {
        return rshi_ack_error(instance, -RSH_E_MAGIC, 
            "invalid magic %d", 
            msg->magic);
    }
    else if (RSH_VERSION != msg->version) {
        return rshi_ack_error(instance, -RSH_E_VERSION, 
            "invalid version %d", 
            msg->version);
    }
    else if (os_macneq(msg->mac, rsha.mac)) {
        return rshi_ack_error(instance, -RSH_E_MAC, 
            "invalid mac %s", 
            os_macstring(msg->mac));
    }
    else if (msg->ack != instance->seq_noack) {
        return rshi_ack_error(instance, -RSH_E_ACK, 
            "ack:0x%x not match instance[seq_noack:0x%x]", 
            msg->ack,
            instance->seq_noack);
    }
    else {
        return (*checker[msg->cmd])(instance, now);
    }
}

int 
__rsha_recver(rsh_instance_t *instance, time_t now, int *pcmd)
{
    static int (*recver[RSH_CMD_END])(rsh_instance_t *instance, time_t now) = {
        [RSH_CMD_ECHO]      = rshi_echo_recver,
        [RSH_CMD_COMMAND]   = rshi_command_recver,
    };
    
    rsh_msg_t *msg = rsha_msg;
    int err, len;

    err = len = rshi_recv(instance);
    if (err<0) {
        goto error;
    }

    /*
    * now, msg maybe error, save peer msg to temp
    */
    instance->seq_peer_temp = msg->seq;
    
    err = rshi_recv_checker(instance, now, msg, len);
    if (err<0) {
        return err;
    }
    
    /*
    * now, msg is ok, save peer msg
    */
    instance->seq_peer = msg->seq;
    
    err = (*recver[msg->cmd])(instance, now);
error:
    *pcmd = is_valid_rsh_cmd(msg->cmd)?msg->cmd:RSH_CMD_UNKNOW;
    
    return err;
}

int 
rsha_recver(loop_watcher_t *watcher, time_t now)
{
    rsh_instance_t *instance = (rsh_instance_t *)watcher->user;
    int err, cmd = RSH_CMD_UNKNOW;

    err = __rsha_recver(instance, now, &cmd);
    
    rshi_st_recv(instance, cmd, err<0)++;

    return err;
}

int 
rshi_echo(rsh_instance_t *instance)
{
    rsh_msg_t *msg = rsha_msg;
    int err;

    os_objzero(msg);
    rsh_msg_fill(msg, rsha.mac, NULL, 0);

    msg->cmd    = RSH_CMD_ECHO;
    msg->flag   = 0;
    msg->seq    = instance->seq++;
    msg->ack    = instance->seq_peer; // first is 0
    
    return rshi_send(instance);
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

int 
rshi_register(rsh_instance_t *instance, time_t now)
{
    static char line[1+OS_FILE_LEN];
    jobj_t jobj = NULL;
    int err, len;

    char fcert[1+FCOOKIE_FILE_LEN] = {0};
    char  fkey[1+FCOOKIE_FILE_LEN] = {0};

    if (NULL==fcookie_cert(instance->cid, fcert) || 
        NULL==fcookie_key(instance->cid, fkey)) {
        err = -ENOCERT; goto error;
    }

    os_arrayzero(line);
    err = os_v_pgets(line, sizeof(line), 
            "curl"
                " -d '{\"mac\":\"%s\"}'"
                " -k --cert %s --key %s"
                " -s %s",
            rsha.macstring,
            fcert, fkey,
            instance->registry);
    if (err<0) {
        debug_error("curl err(%d)", err);
        err = -ECURLFAIL; goto error;
    }

    jobj = jobj_byjson(line);
    if (NULL==jobj) {
        debug_error("bad json=%s", line);
        err = -EBADJSON; goto error;
    }

    jobj_t jkey = jobj_get(jobj, "key");
    if (NULL==jkey) {
        err = -EBADJSON; goto error;
    }

    rsh_secret_t secret;
    char *key = jobj_get_string(jkey);
    
    err = rshi_secret_init(&secret, key);
    if (err<0) {
        goto error;
    }

    rshi_secret_setup(instance, &secret, now);
    instance->key = os_strdup(key);
    
    return rshi_fsm(instance, RSH_FSM_REGISTERED, now);
error:
    jobj_put(jobj);
    fcookie_put_file(fcert);
    fcookie_put_file(fkey);
    
    return err;
}

/******************************************************************************/
