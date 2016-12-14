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
rsha_crypt(rsh_instance_t *instance, int len, aes_crypt_handle_t *cb)
{
    byte *in, *out;
    int i, count = AES_BLOCK_COUNT(len);
    
    for (i=0; i<count; i++) {
        in = out = rsha_buffer + i*AES_BLOCK_SIZE;
        
        (*cb)(in, out, instance->key32, 8*instance->keysize);
    }
}

STATIC void 
rsha_encrypt(rsh_instance_t *instance, int len)
{
    rsha_crypt(instance, len, aes_encrypt);
}

STATIC void 
rsha_decrypt(rsh_instance_t *instance, int len)
{
    rsha_crypt(instance, len, aes_decrypt);
}

STATIC int 
rsha_send(rsh_instance_t *instance)
{
    rsh_msg_t *msg = rsha_msg;
    int len = msg->len, err;

    rsh_msg_hton(msg);
    rsha_encrypt(instance, len);
    
    err = io_sendto(instance->fd, msg, len, (sockaddr_t *)&instance->server, sizeof(instance->server));
    if (err<0) {
        return err;
    }
    
    return 0;
}

int 
rsha_recver(loop_watcher_t *watcher, time_t now)
{
    rsh_msg_t *msg = rsha_msg;
    rsh_instance_t *instance = (rsh_instance_t *)watcher->user;
    sockaddr_in_t addr;
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

    rsha_decrypt(instance, len);
    rsh_msg_ntoh(msg);
    if (len != RSH_MSG_SIZE(msg->len)) {
        return -EBADSIZE;
    }

    return 0;
}

int 
rsha_echo(rsh_instance_t *instance)
{
    rsh_msg_t *msg = rsha_msg;

    rsh_msg_zfill(msg, rsha.mac);

    msg->cmd    = RSH_CMD_ECHO;
    msg->flag   = RSH_F_SYN;
    msg->seq    = instance->seq++;
    
    return rsha_send(instance);
}

int
rsha_ack(rsh_instance_t *instance, int error, char *error_string)
{
    rsh_msg_t *msg = rsha_msg;

    rsh_msg_sfill(msg, rsha.mac, error_string);

    msg->flag   = RSH_F_ACK;
    msg->seq    = 0;
    msg->ack    = instance->seq_peer;
    msg->error  = error;
    
    return rsha_send(instance);
}

STATIC jobj_t
rsha_ack_verror(rsh_instance_t *instance, const char *fmt, va_list args)
{
    jobj_t jobj = jobj_new_object();
    
    int err = jobj_vsprintf(jobj, "error", fmt, args);
    if (err<0) {
        return NULL;
    }

    return jobj;
}

int
rsha_ack_error(rsh_instance_t *instance, int error, const char *fmt, ...)
{
    va_list args;
    jobj_t jobj = NULL;
    int err;
    
    va_start(args, (char *)fmt);
    jobj = jobj_vsprintf(jobj_new_object(), fmt, args);
    va_end(args);

    if (NULL==jobj) {
        return -ENOMEM;
    }

    err = rsha_ack(instance, error, jobj_json(jobj));
    jobj_put(jobj);
    if (err<0) {
        return err;
    }
    
    return error;
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

    char *key = jobj_get_string(jkey);
    len = os_strlen(key);

    if (false==is_good_aes_key_size(len)) {
        err = -EBADKEY; goto error;
    }

    instance->key = os_strdup(key);
    instance->keysize = len/2;
    instance->key8 = (byte *)os_malloc(instance->keysize);
    if (NULL==instance->key8) {
        err = -ENOMEM; goto error;
    }
    
    instance->key32 = (uint32 *)os_malloc(instance->keysize);
    if (NULL==instance->key32) {
        err = -ENOMEM; goto error;
    }
    
    err = os_hex2bin(key, instance->key8, instance->keysize);
    if (err<0) {
        err = -EBADKEY; goto error;
    }

    aes_key_setup(instance->key8, instance->key32, len*8/2);

    return rshi_fsm(instance, RSH_FSM_REGISTERED, now);
error:
    jobj_put(jobj);
    fcookie_put_file(fcert);
    fcookie_put_file(fkey);
    
    return err;
}

/******************************************************************************/
