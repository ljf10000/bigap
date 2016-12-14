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
int 
rsha_send(rsh_instance_t *instance)
{
    rsh_msg_t *msg = rsha_msg;
    int len = msg->len, err;

    rsh_msg_hton(msg);
    
    err = io_sendto(instance->fd, msg, len, (sockaddr_t *)&instance->server, sizeof(instance->server));
    if (err<0) {
        return err;
    }
    
    return 0;
}

int 
rsha_recver(loop_watcher_t *watcher, time_t now)
{
    rsh_instance_t *instance = (rsh_instance_t *)watcher->user;

    return 0;
}

int 
rsha_echo(rsh_instance_t *instance)
{
    rsh_msg_t *msg = rsha_msg;

    os_objzero(msg);

    msg->cmd    = RSH_CMD_ECHO;
    msg->flag   = RSH_F_SYN;
    msg->seq    = instance->seq++;

    rsh_msg_bfill(msg, rsha.mac, NULL, 0);
    
    return rsha_send(instance);
}

int 
rsha_ack(rsh_instance_t *instance)
{
    rsh_msg_t *msg = rsha_msg;

    msg->flag   = RSH_F_ACK;
    msg->seq    = 0;
    msg->ack    = instance->seq_peer;
    msg->error  = 0;

    rsh_msg_bfill(msg, rsha.mac, NULL, 0);
    
    return 0;
}

int 
rsh_resolve(rsh_instance_t *instance, time_t now)
{
    return rshi_fsm(instance, RSH_FSM_RESOLVED, now);
}

int 
rsh_run(rsh_instance_t *instance, time_t now)
{
    return rshi_fsm(instance, RSH_FSM_RUN, now);
}

int 
rsh_register(rsh_instance_t *instance, time_t now)
{
    static char line[1+OS_FILE_LEN];
    jobj_t jobj = NULL;
    int err, len;

    char cert[1+FCOOKIE_FILE_LEN] = {0};
    char  key[1+FCOOKIE_FILE_LEN] = {0};

    if (NULL==fcookie_cert(instance->cid, cert) || 
        NULL==fcookie_key(instance->cid, key)) {
        err = -ENOCERT; goto error;
    }

    os_arrayzero(line);
    err = os_v_pgets(line, sizeof(line), 
            "curl"
                " -d '{"
                    "\"mac\":\"%s\""
                "}'"
                " -k --cert %s --key %s"
                " -s %s",
            rsha.macstring,
            cert, key);
    if (err<0) {
        debug_error("curl err(%d)", err);
        err = -ECURLFAIL; goto error;
    }

    jobj_t jobj = jobj_byjson(line);
    if (NULL==jobj) {
        debug_error("bad json=%s", line);
        err = -EBADJSON; goto error;
    }

    jobj_t jkey = jobj_get(jobj, "key");
    if (NULL==jkey) {
        err = -EBADJSON; goto error;
    }

    char *key = jobj_get_string(jkey);
    len = os_strlen(key);

    if (false==is_good_aes_key_size(len)) {
        err = -EBADKEY; goto error;
    }

    instance->key = os_strdup(key);
    instance->key8 = (byte *)os_malloc(len/2);
    if (NULL==instance->key8) {
        err = -ENOMEM; goto error;
    }
    
    instance->key32 = (uint32 *)os_malloc(len/2);
    if (NULL==instance->key32) {
        err = -ENOMEM; goto error;
    }

    err = os_hex2bin(key, instance->key8, len/2);
    if (err<0) {
        err = -EBADKEY; goto error;
    }

    aes_key_setup(instance->key8, instance->key32, len*8/2);

    return rshi_fsm(instance, RSH_FSM_REGISTERED, now);
error:
    jobj_put(jobj);
    fcookie_put_file(cert);
    fcookie_put_file(key);
    
    return err;
}

/******************************************************************************/
