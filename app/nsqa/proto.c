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
normal_recver(nsq_msg_t *msg)
{
    int id;

    id = nsq_error_idx(msg->body);
    if (false==is_good_nsq_error(id)) {
        return -EPROTO;
    }
    else if (NSQ_E_OK!=id) {
        return -EPROTO;
    }

    return 0;
}

static int
identifying_recver(nsq_instance_t *instance, time_t now)
{
    nsq_msg_t *msg = nsq_recver_msg(instance);
    int err;

    err = normal_recver(msg);
    if (err<0) {
        return err;
    }

    nsqi_fsm(instance, NSQ_FSM_IDENTIFIED);

    return 0;
}

static int
subscribing_recver(nsq_instance_t *instance, time_t now)
{
    nsq_msg_t *msg = nsq_recver_msg(instance);
    int err;

    err = normal_recver(msg);
    if (err<0) {
        return err;
    }

    nsqi_fsm(instance, NSQ_FSM_SUBSCRIBED);

    return 0;
}

static int
run_recver(nsq_instance_t *instance, time_t now)
{
    nsq_msg_t *msg = nsq_recver_msg(instance);
    int err;
    
    return 0;
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
nsq_confuse(nsq_instance_t *instance)
{
    if (is_good_fd(instance->fd)) {
        os_close(instance->fd);

        nsqi_fsm(instance, NSQ_FSM_INIT);
        instance->rdy = NSQ_RDY;
    }
    
    return 0;
}

static int
recv_pre(nsq_instance_t *instance)
{
    switch(instance->fsm) {
        case NSQ_FSM_INIT:      // down
        case NSQ_FSM_RESOLVED:  // down
        case NSQ_FSM_CONNECTED: // down
        case NSQ_FSM_HANDSHAKED:// down
            debug_proto("instance[%s] not support recv @fsm[%s]",
                instance->name, 
                nsq_fsm_string(instance->fsm));

            return -EBADFSM;
        case NSQ_FSM_IDENTIFYING:
        case NSQ_FSM_IDENTIFIED:
        case NSQ_FSM_SUBSCRIBING:
        case NSQ_FSM_SUBSCRIBED:
        case NSQ_FSM_RUN:
            return 0;
        default:
            debug_proto("instance[%s] not support recv @fsm[%d]",
                instance->name, 
                instance->fsm);

            return -EBADFSM;
    }
}

static int
recv_post(nsq_instance_t *instance)
{
    nsq_msg_t *msg = nsq_recver_msg(instance);
    int err;

    if (false==is_good_nsq_frame(msg->type)) {
        return -EPROTO;
    }
    
    switch(instance->fsm) {
        case NSQ_FSM_IDENTIFIED:
        case NSQ_FSM_SUBSCRIBED:
            if (is_nsq_msg_heartbeat(msg)) {
                
            }
            
            break;
        case NSQ_FSM_IDENTIFYING: // down
        case NSQ_FSM_SUBSCRIBING:
            if (is_nsq_frame_message(msg->type)) {
                return -EPROTO;
            }

            break;
        case NSQ_FSM_RUN:
            
            
            break;
        default:
            err = -EBADFSM;

            break;
    }

    return err;
}

static int
common_recver(nsq_msg_t *msg)
{
    int id;


    return 0;
}

static int 
recver_handle(nsq_instance_t *instance, time_t now)
{
    int err;
    
    switch(instance->fsm) {
        case NSQ_FSM_IDENTIFYING:
            err = identifying_recver(instance, now);

            break;
        case NSQ_FSM_SUBSCRIBING:
            err = subscribing_recver(instance, now);

            break;
        case NSQ_FSM_RUN:
            err = run_recver(instance, now);

            break;
        default:
            err = -EBADFSM;

            break;
    }

    return err;
}

int 
nsq_recver(struct loop_watcher *watcher, time_t now)
{
    nsq_instance_t *instance = (nsq_instance_t *)watcher->user;
    int err;

    err = recv_pre(instance);
    if (err<0) {
        goto confused;
    }

    err = nsq_recv(instance);
    if (err<0) {
        goto confused;
    }

    err = recv_post(instance);
    if (err<0) {
        goto confused;
    }

    err = recver_handle(instance, now);
    if (err<0) {
        goto confused;
    }

confused:
    nsq_confuse(instance);
    
    return err;
}

/******************************************************************************/
