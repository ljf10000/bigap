/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      rsha
#endif
    
#ifndef __THIS_FILE
#define __THIS_FILE     proto
#endif
    
#define __DEAMON__
#include "rsha.h"
/******************************************************************************/
STATIC void 
rshi_encode(rsh_instance_t *instance)
{
    rsh_msg_t *msg = rsha_msg;

    rsh_msg_encode(msg, rsh_msg_len(msg), 
        rshi_pmk(instance), rshi_key(instance), 
        msg->hmac, msg->hmacsize);
}

STATIC void 
rshi_decode(rsh_instance_t *instance, int len)
{
    rsh_msg_t *msg = rsha_msg;

    rsh_msg_decode(msg, len, 
        rshi_pmk(instance), rshi_key(instance), 
        instance->sec.hmac, instance->sec.hmacsize);
}

STATIC int 
rshi_recv(rsh_instance_t *instance)
{
    rsh_msg_t *msg = rsha_msg;
    sockaddr_in_t addr = OS_SOCKADDR_INET(0, 0);
    sockaddr_len_t addrlen = sizeof(addr);
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

STATIC void
rshi_send_over(rsh_instance_t *instance, rsh_over_t *over, bool is_error)
{
    rshi_st_send(instance, over->cmd, is_error)++;
    
    switch(over->cmd) {
        case RSH_CMD_ECHO:
            rshi_tm_echo_send(instance, is_error)++;
            
            break;
        case RSH_CMD_COMMAND:
            rshi_tm_command_send(instance, is_error)++;
            
            break;
    }
}

STATIC int 
rshi_send(rsh_instance_t *instance)
{
    rsh_msg_t *msg = rsha_msg;
    sockaddr_in_t proxy = OS_SOCKADDR_INET(instance->ip, instance->port);
    rsh_over_t over = {
        .cmd    = msg->cmd,
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
    rshi_send_over(instance, &over, err<0);
    
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
    msg->hmacsize   = instance->sec.hmacsize;
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
    rshi_exec(instance, (char *)rsh_msg_body(msg));
}

STATIC int 
rshi_recv_checker(rsh_instance_t *instance, time_t now, rsh_msg_t *msg, int len)
{
    static int (*checker[RSH_CMD_END])(rsh_instance_t *instance, time_t now) = {
        [RSH_CMD_ECHO]      = rshi_echo_recv_checker,
        [RSH_CMD_COMMAND]   = rshi_noack_recv_checker,
    };
    
    int size = rsh_msg_size(msg);

    if (instance->sec.hmacsize != msg->hmacsize) {
        return rshi_ack_error(instance, -RSH_E_HMAC, 
            "time[%s] invalid hmac size",
            os_fulltime_string(now));
    }
    else if (false==os_memeq(instance->sec.hmac, msg->hmac, instance->sec.hmacsize)) {
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

STATIC void
rshi_recv_over(rsh_instance_t *instance, rsh_over_t *over, bool is_error)
{
    rshi_st_recv(instance, over->cmd, is_error)++;
    
    switch(over->cmd) {
        case RSH_CMD_ECHO:
            rshi_tm_echo_recv(instance, is_error)++;
            
            break;
        case RSH_CMD_COMMAND:
            rshi_tm_command_recv(instance, is_error)++;
            
            break;
    }
}

int 
rsha_recver(loop_watcher_t *watcher, time_t now)
{
    static void (*recver[RSH_CMD_END])(rsh_instance_t *instance, time_t now) = {
        [RSH_CMD_ECHO]      = rshi_echo_recver,
        [RSH_CMD_COMMAND]   = rshi_command_recver,
    };
    
    rsh_instance_t *instance = (rsh_instance_t *)watcher->user;
    rsh_msg_t *msg = rsha_msg;
    rsh_over_t over = {
        .cmd    = RSH_CMD_UNKNOW,
    };
    int err, len;

    err = len = rshi_recv(instance);
    if (err<0) {
        goto error;
    }
    over.cmd    = msg->cmd;

    err = rshi_recv_checker(instance, now, msg, len);
    if (err<0) {
        goto error;
    }

    (*recver[msg->cmd])(instance, now);

    rshi_recv_ok(instance, now);
error:
    rshi_recv_over(instance, &over, err<0);

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
    msg->hmacsize   = instance->sec.hmacsize;
    
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

    instance->sec.crypt     = RSH_CRYPT_AES;
    instance->sec.hmactype  = SHA_256;
    instance->seq = instance->seq_noack = rshi_seq_rand();

    jobj_add_string(jobj, "service", "rsh");
    jobj_add_string(jobj, "product", "TODO:PRODUCT");
    jobj_add_string(jobj, "mac", rsha.macstring);
    jobj_add_u32(jobj, "seq",  instance->seq);

    return jobj;
error:
    jobj_put(jobj);

    return NULL;
}

STATIC int 
rshi_register_post(rsh_instance_t *instance, jobj_t jobj, time_t now)
{
    jobj_t jval;
    char *string;
    rsh_key_t key;
    int err, val;

    /*
    * 1. get pmk
    */
    jval = jobj_get(jobj, "pmk");
    if (NULL==jval) {
        return -EBADJSON;
    }
    string = jobj_get_string(jval);
    err = rsh_key_hex2bin(&key, string);
    if (err<0) {
        return err;
    }
    rsh_key_setup(rshi_pmk(instance), &key);
    instance->sec.pmkstring = os_strdup(string);
    
    /*
    * 2. get key
    */
    jval = jobj_get(jobj, "key");
    if (NULL==jval) {
        return -EBADJSON;
    }
    string = jobj_get_string(jval);
    err = rsh_key_hex2bin(&key, string);
    if (err<0) {
        return err;
    }
    rshi_key_setup(instance, &key);
    instance->sec.keystring = os_strdup(string);

    /*
    * 2. get crypt
    */
    jval = jobj_get(jobj, "crypt");
    if (NULL==jval) {
        return -EBADJSON;
    }
    val = rsh_crypt_getidbyname(jobj_get_string(jval));
    if (false==is_good_rsh_crypt(val)) {
        return -EBADJSON;
    }
    instance->sec.crypt = val;

    /*
    * 3. get hmac type
    */
    jval = jobj_get(jobj, "hmactype");
    if (NULL==jval) {
        return -EBADJSON;
    }
    val = sha2_getidbyname(jobj_get_string(jval));
    if (false==is_good_sha2(val)) {
        return -EBADJSON;
    }
    else if (SHA_224==val) {
        return -EBADJSON;
    }
    instance->sec.hmactype = val;
    instance->sec.hmacsize = SHA_DIGEST_SIZE(val);
    
    /*
    * 4. get seq
    */
    jval = jobj_get(jobj, "seq");
    if (NULL==jval) {
        return -EBADJSON;
    }
    instance->seq_peer = jobj_get_u32(jval);
    
    /*
    * 5. get seq
    */
    jval = jobj_get(jobj, "debug");
    if (NULL==jval) {
        return -EBADJSON;
    }
    instance->debug = jobj_get_bool(jval);
    
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
    char   fcert[1+FCOOKIE_FILE_LEN] = {0};
    char    fkey[1+FCOOKIE_FILE_LEN] = {0};
    char fcacert[1+FCOOKIE_FILE_LEN] = {0};

    if (NULL==fcookie_cert(instance->cid, fcert)    || 
        NULL==fcookie_key(instance->cid, fkey)      ||
        NULL==fcookie_cacert(instance->cid, fcacert)) {
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
                " --cert %s --key %s --cacert %s"
                " -s %s",
            jobj_json(jinput),
            fcert, fkey, fcacert,
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
