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
    rshi_tm_send(instance, over->cmd, is_error) = time(NULL);
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
    
    debug_proto("instance %s send %s size:%d to %s:%d", 
        instance->sp, rsh_cmd_getnamebyid(over.cmd), size,
        os_ipstring(instance->ip), ntohs(instance->port));

    rshi_encode(instance);
    
    err = io_sendto(instance->fd, msg, size, (sockaddr_t *)&proxy, sizeof(proxy));
    if (err<0) {
        goto error;
    }
    
    err = size;
error:
    rshi_send_over(instance, &over, err<0);

    debug_packet("instance %s send %s error:%d", 
        instance->sp, rsh_cmd_getnamebyid(over.cmd), err);
    
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
    
    jobj_add(jinstance, "sp",       jobj_new_string(instance->sp));
    jobj_add(jinstance, "cache",    jobj_new_string(instance->cache));
    jobj_add(jobj, "instance", jinstance);

    err = jscript_exec(jobj_json(jobj));
    
    jobj_put(jobj);

    return err;
}

STATIC int 
rshi_handshake_recv_checker(rsh_instance_t *instance, time_t now)
{
    rsh_msg_t *msg = rsha_msg;
    
    if (false==is_rsh_msg_ack(msg)) {
        return rshi_ack_error(instance, -RSH_E_FLAG, 
            "time[%s] handshake reply ack[0x%x] without ack flag", 
            os_fulltime_string(now),
            msg->ack);
    }

    return 0;
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
rshi_handshake_recver(rsh_instance_t *instance, time_t now)
{
    rshi_fsm(instance, RSH_FSM_HANDSHAKED, now);
    
    instance->handshake.recv = now;
    instance->handshake.recvs++;
}

STATIC void 
rshi_echo_recver(rsh_instance_t *instance, time_t now)
{
    rsh_echo_t *echo = rshi_echo_get(instance);
    
    echo->recv = now;
    echo->recvs++;
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
        [RSH_CMD_HANDSHAKE]     = rshi_handshake_recv_checker,
        [RSH_CMD_ECHO]          = rshi_echo_recv_checker,
        [RSH_CMD_COMMAND]       = rshi_noack_recv_checker,
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
rshi_recv_over(rsh_instance_t *instance, time_t now, rsh_over_t *over, bool is_error)
{
    rshi_st_recv(instance, over->cmd, is_error)++;
    rshi_tm_recv(instance, over->cmd, is_error) = now;
}

int 
rsha_recver(loop_watcher_t *watcher, time_t now)
{
    static void (*recver[RSH_CMD_END])(rsh_instance_t *instance, time_t now) = {
        [RSH_CMD_HANDSHAKE]     = rshi_handshake_recver,
        [RSH_CMD_ECHO]          = rshi_echo_recver,
        [RSH_CMD_COMMAND]       = rshi_command_recver,
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
    rshi_recv_over(instance, now, &over, err<0);

    return err;
}

int 
rshi_handshake(rsh_instance_t *instance, time_t now)
{
    rsh_msg_t *msg = rsha_msg;
    int err;
    
    /*
    * zero first
    */
    os_objzero(msg);
    rsh_msg_fill(msg, rsha.mac, NULL, 0);

    msg->cmd        = RSH_CMD_HANDSHAKE;
    msg->flag       = 0;
    msg->seq        = instance->seq;
    msg->ack        = 0;
    msg->hmacsize   = instance->sec.hmacsize;
    
    instance->handshake.send = now;
    instance->handshake.sends++;
    
    err = rshi_send(instance);
    if (err<0) {
        return err;
    }
    
    return 0;
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

    rsh_echo_t *echo = rshi_echo_get(instance);
    echo->send = now;
    echo->sends++;

    return 0;
}

int
rshi_resolve(rsh_instance_t *instance, time_t now)
{
    instance->ip = os_ipdns(instance->proxy);
    if (is_good_ipaddr(instance->ip)) {
        rshi_fsm(instance, RSH_FSM_RESOLVED, now);
    }
    debug_entry("instance %s resolve %s ok", instance->sp, instance->proxy);

    return 0;
}

int 
rshi_run(rsh_instance_t *instance, time_t now)
{
    return rshi_fsm(instance, RSH_FSM_RUN, now);
}

/******************************************************************************/
