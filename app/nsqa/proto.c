/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      nsqa
#endif
        
#ifndef __THIS_FILE
#define __THIS_FILE     loop
#endif
        
#define __DEAMON__
#include "nsqa.h"
/******************************************************************************/
static int
identifying_response_handle(nsq_instance_t *instance, time_t now)
{
    nsq_msg_t *msg = nsq_recver_msg(instance);
    
    if (NSQ_E_OK==instance->error) {
        nsqi_fsm(instance, NSQ_FSM_IDENTIFIED);
    }
    else if (instance->auth) {
        jobj_put(instance->jauth);
        
        instance->jauth = jobj_byjson(msg->body);
        if (NULL==instance->jauth) {
            return -EPROTO;
        }

        nsqi_fsm(instance, NSQ_FSM_IDENTIFIED);
    }
    
    return 0;
}

static int
subscribing_response_handle(nsq_instance_t *instance, time_t now)
{
    if (NSQ_E_OK==instance->error) {
        nsqi_fsm(instance, NSQ_FSM_SUBSCRIBED);
    }
    
    return 0;
}

static int
run_response_handle(nsq_instance_t *instance, time_t now)
{
    nsq_msg_t *msg = nsq_recver_msg(instance);
    int err;
    
    return 0;
}

static int
run_message_handle(nsq_instance_t *instance, time_t now)
{
    nsq_msg_t *msg = nsq_recver_msg(instance);
    int err = 0;

    err = nsq_script_init(&instance->script, msg->body);
    if (err<0) {
        goto error;
    }
    
    err = nsq_script_exec(&instance->script);
    if (err<0) {
        goto error;
    }

    nsq_FIN(instance);
    ndq_try_rdy(instance);
    
    return 0;
error:
    nsq_REQ(instance);
    
    return err;
}

static int
pre_handle(nsq_instance_t *instance)
{
    switch(instance->fsm) {
        case NSQ_FSM_INIT:      // down
        case NSQ_FSM_RESOLVED:  // down
        case NSQ_FSM_CONNECTED: // down
            debug_proto("instance[%s] not support recv @fsm[%s]",
                instance->name, 
                nsq_fsm_string(instance->fsm));

            return -EPROTO;
    }

    return 0;
}

static int 
frame_error_handle(nsq_instance_t *instance, time_t now)
{
    return -EPROTO;
}

static int 
frame_response_handle(nsq_instance_t *instance, time_t now)
{
    nsq_msg_t *msg = nsq_recver_msg(instance);
    int err;
    
    if (is_nsq_response_heartbeat(msg)) {
        return nsq_NOP(instance);
    }

    instance->error = nsq_error_idx(msg->body);
    if (is_valid_nsq_error(instance->error)) {
        return -EPROTO;
    }

    switch(instance->fsm) {
        case NSQ_FSM_HANDSHAKED: // down
        case NSQ_FSM_IDENTIFIED: // down
        case NSQ_FSM_AUTHING:    // down
        case NSQ_FSM_AUTHED:     // down
        case NSQ_FSM_SUBSCRIBED: // down
            debug_proto("instance[%s] not support recv @fsm[%s]",
                instance->name, 
                nsq_fsm_string(instance->fsm));
            
            err = -EPROTO;

            break;
        case NSQ_FSM_IDENTIFYING:
            err = identifying_response_handle(instance, now);

            break;
        case NSQ_FSM_SUBSCRIBING:
            err = subscribing_response_handle(instance, now);

            break;
        case NSQ_FSM_RUN:
            err = run_response_handle(instance, now);
            
            break;
        default:
            err = -EPROTO;

            break;
    }

    return -EPROTO;
}

static int 
frame_message_handle(nsq_instance_t *instance, time_t now)
{
    nsq_msg_t *msg = nsq_recver_msg(instance);
    int err;

    if (NSQ_FSM_RUN != instance->fsm) {
        debug_proto("instance[%s] not support recv @fsm[%s]",
            instance->name, 
            nsq_fsm_string(instance->fsm));

        return -EPROTO;
    }
    
    err = run_message_handle(instance, now);
    if (err<0) {
        return err;
    }
    
    return 0;
}

static int 
frame_handle(nsq_instance_t *instance, time_t now)
{
    nsq_msg_t *msg = nsq_recver_msg(instance);
    int err;

    switch(msg->type) {
        case NSQ_FRAME_ERROR:
            err = frame_error_handle(instance, now);

            break;
        case NSQ_FRAME_RESPONSE:
            err = frame_response_handle(instance, now);
            
            break;
        case NSQ_FRAME_MESSAGE:
            err = frame_message_handle(instance, now);
            
            break;
        default:
            err = -EPROTO;

            break;
    }

    return err;
}

int 
nsq_recver(struct loop_watcher *watcher, time_t now)
{
    nsq_instance_t *instance = (nsq_instance_t *)watcher->user;
    int err;

    err = pre_handle(instance);
    if (err<0) {
        goto confused;
    }

    err = nsq_recv(instance);
    if (err<0) {
        goto confused;
    }

    err = frame_handle(instance, now);
    if (err<0) {
        goto confused;
    }

confused:
    nsq_confuse(instance);
    
    return err;
}

int
nsq_resolve(nsq_instance_t *instance)
{
    if (is_nsq_resolve_ok(instance)) {
        return 0;
    }
    
    debug_proto("instance[%s] resolve ...", instance->name);
    
    uint32 ip = inet_addr(instance->domain);
    if (INADDR_NONE != ip) {
        instance->server.sin_addr.s_addr = ip;
    } else {
        struct hostent *p = gethostbyname(instance->domain);
        if (NULL==p) {
            return -EDNS;
        }

        instance->server.sin_addr.s_addr = *(uint32 *)(p->h_addr);
    }
    
    debug_proto("instance[%s] resolve ok.", instance->name);

    nsqi_fsm(instance, NSQ_FSM_RESOLVED);


    return 0;
}

int
nsq_connect(nsq_instance_t *instance)
{
    int err;
    
    debug_proto("instance[%s] connect ...", instance->name);
    
    err = connect(instance->fd, (sockaddr_t *)&instance->server, sizeof(instance->server));
    if (instance->fd<0) {
        debug_error("instance %s connect error:%d", instance->name, -errno);
        
        return -errno;
    }

    nsqi_fsm(instance, NSQ_FSM_CONNECTED);

    debug_proto("instance[%s] connect ok.", instance->name);

    return 0;
}

int
nsq_handshake(nsq_instance_t *instance)
{
    int err;

    debug_proto("instance[%s] handshaking ...", instance->name);
    
    err = nsq_MAGIC(instance);
    if (err<0) {
        return err;
    }

    debug_proto("instance[%s] handshaking ok.", instance->name);

    nsqi_fsm(instance, NSQ_FSM_HANDSHAKED);
    
    return 0;
}

int
nsq_identify(nsq_instance_t *instance)
{
    int err;
    
    debug_proto("instance[%s] identifying ...", instance->name);
    
    err = nsq_IDENTIFY(instance);
    if (err<0) {
        return err;
    }
    
    debug_proto("instance[%s] identifying ok.", instance->name);

    nsqi_fsm(instance, NSQ_FSM_IDENTIFYING);

    return 0;
}

int
nsq_auth(nsq_instance_t *instance)
{
    // support auth later, 
    // just authed, now
    nsqi_fsm(instance, NSQ_FSM_AUTHED);

    return 0;
}

int
nsq_subscrib(nsq_instance_t *instance)
{
    int err;
    
    debug_proto("instance[%s] subscribing ...", instance->name);
    
    err = nsq_SUB(instance);
    if (err<0) {
        return err;
    }
    
    debug_proto("instance[%s] subscribing ok.", instance->name);

    nsqi_fsm(instance, NSQ_FSM_SUBSCRIBING);
    
    return 0;
}

int
nsq_run(nsq_instance_t *instance)
{
    int err;
    
    debug_proto("instance[%s] run ...", instance->name);
    
    err = nsq_RDY(instance);
    if (err<0) {
        return err;
    }
    
    debug_proto("instance[%s] run ok.", instance->name);

    nsqi_fsm(instance, NSQ_FSM_RUN);
    
    return 0;
}

int
nsq_confuse(nsq_instance_t *instance)
{
    if (is_good_fd(instance->fd)) {
        nsq_CLS(instance);
        
        os_close(instance->fd);

        nsqi_fsm(instance, NSQ_FSM_INIT);
        instance->rdy = NSQ_RDY;
    }
    
    return -EPROTO;
}

int 
nsq_recv(nsq_instance_t *instance)
{
    nsq_msg_t *msg = nsq_recver_msg(instance);

    int err = nsqb_recv(instance->fd, &instance->brecver);
    
    debug_proto("instance[%s] recv frame-%s @%s",
        instance->name, 
        nsq_frame_string(msg->type),
        nsq_fsm_string(instance->fsm));

    return err;
}

int 
nsq_MAGIC(nsq_instance_t *instance)
{
    int err;

    err = nsqb_MAGIC(&instance->bsender);
    if (err<0) {
        return err;
    }

    return nsq_send(instance);
}

int
nsq_IDENTIFY(nsq_instance_t *instance)
{
    int err;

    err = nsqb_IDENTIFY(&instance->bsender, instance->identify);
    if (err<0) {
        return err;
    }

    return nsq_send(instance);
}

int
nsq_SUB(nsq_instance_t *instance)
{
    int err;

    err = nsqb_SUB(&instance->bsender, instance->topic, instance->channel);
    if (err<0) {
        return err;
    }

    return nsq_send(instance);
}

int
nsq_RDY(nsq_instance_t *instance)
{
    int err;

    err = nsqb_RDY(&instance->bsender, instance->rdy);
    if (err<0) {
        return err;
    }

    return nsq_send(instance);
}

int
nsq_FIN(nsq_instance_t *instance)
{
    int err;

    err = nsqb_FIN(&instance->bsender, nsq_recver_msg(instance)->id);
    if (err<0) {
        return err;
    }

    return nsq_send(instance);
}

int
nsq_NOP(nsq_instance_t *instance)
{
    int err;

    err = nsqb_NOP(&instance->bsender);
    if (err<0) {
        return err;
    }

    return nsq_send(instance);
}

int
nsq_CLS(nsq_instance_t *instance)
{
    int err;

    err = nsqb_CLS(&instance->bsender);
    if (err<0) {
        return err;
    }

    return nsq_send(instance);
}

int
nsq_REQ(nsq_instance_t *instance)
{
    int err;

    err = nsqb_REQ(&instance->bsender, nsq_recver_msg(instance)->id, 0);
    if (err<0) {
        return err;
    }

    return nsq_send(instance);
}

int
ndq_try_rdy(nsq_instance_t *instance)
{
    instance->rdy--;
    
    if (instance->rdy > NSQ_RDY_MIN) {
        return 0;
    } else {
        instance->rdy = NSQ_RDY;
        
        return nsq_RDY(instance);
    }
}

/******************************************************************************/
