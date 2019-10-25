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

    rsh_msg_padzero(msg);
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

    if (__is_ak_debug_proto && __is_ak_debug_packet) {
        os_println("recv msg size:%d", len);
        os_dump_buffer(msg, len);
    }
    
    if (false==is_good_rsh_msg_size(len)) {
        debug_proto("recv bad msg size:%d", len);

        return -EBADSIZE;
    }
    else if (false==is_rshi_server_address(instance, &addr)) {
        debug_proto("recv from fake server %s:%d",
            unsafe_ipstring(addr.sin_addr.s_addr),
            ntohs(addr.sin_port));
        
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
    sockaddr_in_t proxy = OS_SOCKADDR_INET(instance->ip, instance->sin_port);
    rsh_over_t over = {
        .cmd    = msg->cmd,
    };
    int size    = rsh_msg_size(msg);
    int err;

    debug_proto("instance %s send %s size:%d to %s:%d ...", 
        instance->sp, rsh_cmd_getnamebyid(over.cmd), size,
        unsafe_ipstring(instance->ip), instance->port);

    rshi_encode(instance);

    err = io_sendto(instance->fd, msg, size, (sockaddr_t *)&proxy, sizeof(proxy));
    if (err<0) {
        goto error;
    }

    err = size;
error:
    rshi_send_over(instance, &over, err<0);

    debug_packet("instance %s send %s size:%d to %s:%d error:%d", 
        instance->sp, rsh_cmd_getnamebyid(over.cmd), size, 
        unsafe_ipstring(instance->ip), instance->port, err);

    return err;
}

STATIC int
rshi_ack(rsh_instance_t *instance, int error, void *buf, int len)
{
    rsh_msg_t *msg = rsha_msg;
    rsh_msg_t old = *msg;
    int err;

    if (is_rsh_msg_ack(msg)) {
        return 0;
    }

    os_println("ack error:%d", error);

    /*
    * zero first
    */
    os_objzero(msg);

    msg->cmd        = old.cmd;
    msg->version    = old.version;
    msg->flag       = RSH_F_ACK;
    msg->error      = error;
    msg->hmacsize   = instance->sec.hmacsize;
    msg->seq        = instance->seq;
    msg->ack        = old.seq;
    
    if (error) {
        instance->peer_error++;
        instance->error = error;

        rsh_hdr_hton(&old);
        rsh_msg_append(msg, &old, sizeof(old));
    }

    rsh_msg_fill(msg, rsha.mac, buf, len);

    if (error && buf) {
        debug_error("ack error:%s", buf);
         os_println("ack error:%s", buf);
    }
    
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

    json = jobj_json(jobj);
    err = jscript_exec(json);

    jobj_put(jobj);

    return err;
}

STATIC int 
rshi_ack_checker(rsh_instance_t *instance, time_t now)
{
    rsh_msg_t *msg = rsha_msg;
    uint32 offset = os_seq_offset(instance->seq, msg->ack);

    if (offset > 1024) {
        debug_proto("recv bad msg, ack:%u not match instance seq:%u", msg->ack, instance->seq);
        
        return -EBADSEQ;
    } else {
        return 0;
    }
}

STATIC int 
rshi_handshake_checker(rsh_instance_t *instance, time_t now)
{
    rsh_msg_t *msg = rsha_msg;
    
    if (false==is_rsh_msg_ack(msg)) {
        return rshi_ack_error(instance, -RSH_E_FLAG, 
            "time[%s] handshake reply ack[0x%x] without ack flag", 
            unsafe_fulltime_string(now),
            msg->ack);
    }
    else if (msg->ack != instance->seq) {
        return rshi_ack_error(instance, -RSH_E_ACK, 
            "time[%s] handshake reply ack[0x%x] not match instance[seq:0x%x]", 
            unsafe_fulltime_string(now),
            msg->ack,
            instance->seq);
    }

    return 0;
}

STATIC int 
rshi_handshake_handle(rsh_instance_t *instance, time_t now)
{
    rsh_msg_t *msg = rsha_msg;
    
    // save peer's seq
    instance->seq_peer = msg->seq;
    debug_proto("instance %s handshake ok. save peer seq:%u", instance->sp, instance->seq_peer);

    // fsm ==> handshaked
    rshi_fsm(instance, RSH_FSM_HANDSHAKED, now);

    // update statistics
    instance->handshake.recv = now;
    instance->handshake.recvs++;

    return 0;
}

STATIC int 
rshi_echo_checker(rsh_instance_t *instance, time_t now)
{
    rsh_msg_t *msg = rsha_msg;
    
    if (false==is_rsh_msg_ack(msg)) {
        return rshi_ack_error(instance, -RSH_E_FLAG, 
            "time[%s] echo reply ack[0x%x] without ack flag", 
            unsafe_fulltime_string(now),
            msg->ack);
    }
    
    return rshi_ack_checker(instance, now);
}

STATIC int 
rshi_echo_handle(rsh_instance_t *instance, time_t now)
{
    rsh_echo_t *echo = rshi_echo_get(instance);
    
    echo->recv = now;
    echo->recvs++;

    debug_proto("recv echo ack");

    return 0;
}

STATIC int 
rshi_command_checker(rsh_instance_t *instance, time_t now)
{
    rsh_msg_t *msg = rsha_msg;
    
    if (is_rsh_msg_ack(msg)) {
        return rshi_ack_error(instance, -RSH_E_FLAG, 
            "time[%s] command with ack flag",
            unsafe_fulltime_string(now));
    }
    
    return rshi_ack_checker(instance, now);
}

STATIC int 
rshi_command_handle(rsh_instance_t *instance, time_t now)
{
    rsh_msg_t *msg = rsha_msg;
    int err;

    // save peer's seq
    instance->seq_peer = msg->seq;
    
    rshi_echo_set(instance, now, true);
    os_println("rshi_exec ...");
    err = rshi_exec(instance, (char *)rsh_msg_body(msg));
    os_println("rshi_exec error:%d", err);
    rshi_ack(instance, err, NULL, 0);
    
    debug_proto("recv command");

    return err;
}

STATIC int 
rshi_recv_checker(rsh_instance_t *instance, time_t now, rsh_msg_t *msg, int len)
{
    static int (*checker[RSH_CMD_END])(rsh_instance_t *instance, time_t now) = {
        [RSH_CMD_HANDSHAKE]     = rshi_handshake_checker,
        [RSH_CMD_ECHO]          = rshi_echo_checker,
        [RSH_CMD_COMMAND]       = rshi_command_checker,
    };
    int size = rsh_msg_size(msg);

    if (instance->sec.hmacsize != msg->hmacsize) {
        return rshi_ack_error(instance, -RSH_E_HMAC, 
            "time[%s] invalid hmac size",
            unsafe_fulltime_string(now));
    }
    else if (false==os_memeq(instance->sec.hmac, msg->hmac, instance->sec.hmacsize)) {
        return rshi_ack_error(instance, -RSH_E_HMAC, 
            "time[%s] invalid hmac",
            unsafe_fulltime_string(now));
    }
    else if (false==is_valid_rsh_cmd(msg->cmd)) {
        return rshi_ack_error(instance, -RSH_E_CMD, 
            "time[%s] invalid cmd[%d]",
            unsafe_fulltime_string(now),
            msg->cmd);
    }
    else if (len != size) {
        return rshi_ack_error(instance, -RSH_E_LEN, 
            "time[%s] recv len[%d] not match msg size[%d]", 
            unsafe_fulltime_string(now),
            len, size);
    }
    else if (RSH_VERSION != msg->version) {
        return rshi_ack_error(instance, -RSH_E_VERSION, 
            "time[%s] invalid version[%d]", 
            unsafe_fulltime_string(now),
            msg->version);
    }
    else if (false==os_maceq(msg->mac, rsha.mac)) {
        return rshi_ack_error(instance, -RSH_E_MAC, 
            "time[%s] invalid mac[%s]", 
            unsafe_fulltime_string(now),
            unsafe_macstring(msg->mac));
    }
#if 0
    else if (msg->seq != instance->seq_peer) {
        return rshi_ack_error(instance, -RSH_E_SEQ, 
            "time[%s] seq[0x%x] not match instance[seq_peer:0x%x]", 
            unsafe_fulltime_string(now),
            msg->seq,
            instance->seq_peer);
    }
    else if (msg->ack != instance->seq_noack) {
        return rshi_ack_error(instance, -RSH_E_ACK, 
            "time[%s] ack[0x%x] not match instance[seq_noack:0x%x]", 
            unsafe_fulltime_string(now),
            msg->ack,
            instance->seq_noack);
    }
#endif
    else {
        return (*checker[msg->cmd])(instance, now);
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
    static int (*recver[RSH_CMD_END])(rsh_instance_t *instance, time_t now) = {
        [RSH_CMD_HANDSHAKE]     = rshi_handshake_handle,
        [RSH_CMD_ECHO]          = rshi_echo_handle,
        [RSH_CMD_COMMAND]       = rshi_command_handle,
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

    err = (*recver[msg->cmd])(instance, now);

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
    msg->seq        = instance->seq;
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
    msg->seq        = instance->seq++;
    msg->ack        = instance->seq_peer;
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
    debug_proto("instance %s resolve %s ...", instance->sp, instance->proxy);
    
    instance->ip = os_ipdns(instance->proxy);
    if (is_good_ipaddr(instance->ip)) {
        debug_proto("instance %s resolve %s ok", instance->sp, instance->proxy);
        
        rshi_fsm(instance, RSH_FSM_RESOLVED, now);
    }

    return 0;
}
/******************************************************************************/
