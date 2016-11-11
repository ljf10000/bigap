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
static bool
is_resolve_ok(nsq_instance_t *instance)
{
    return INADDR_NONE != instance->server.sin_addr.s_addr;
}

static int
handshaked_recver(nsq_instance_t *instance, time_t now)
{
    int err;

    err = nsqb_recv(instance->fd, &instance->brecver);
    if (err<0) {
        return err;
    }

    
    return 0;
}

static int
identifying_recver(nsq_instance_t *instance, time_t now)
{
    return 0;
}

static int
subscribing_recver(nsq_instance_t *instance, time_t now)
{
    return 0;
}

static int
run_recver(nsq_instance_t *instance, time_t now)
{
    return 0;
}

int
nsq_resolve(nsq_instance_t *instance)
{
    if (false==is_resolve_ok(instance)) {
        in_addr_t ip = inet_addr(instance->domain);
        if (INADDR_NONE != ip) {
            instance->server.sin_addr.s_addr = ip;
        } else {
            struct hostent *p = gethostbyname(instance->domain);
            if (NULL==p) {
                return -EDNS;
            }

            instance->server.sin_addr.s_addr = *(uint32 *)(p->h_addr);
        }

        nsq_fsm_change(instance, NSQ_FSM_RESOLVED);
    }

    return 0;
}

int
nsq_connect(nsq_instance_t *instance)
{
    int err;
    
    err = connect(instance->fd, (sockaddr_t *)&instance->server, sizeof(instance->server));
    if (instance->fd<0) {
        debug_error("instance %s connect error:%d", instance->name, -errno);
        
        return -errno;
    }

    nsq_fsm_change(instance, NSQ_FSM_CONNECTED);

    return 0;
}

int
nsq_handshake(nsq_instance_t *instance)
{
    int err;
    
    err = nsqb_MAGIC(&instance->bsender);
    if (err<0) {
        return err;
    }

    err = nsqb_send(instance->fd, &instance->bsender);
    if (err<0) {
        return err;
    }

    nsq_fsm_change(instance, NSQ_FSM_HANDSHAKING);
    
    return 0;
}

int
nsq_identify(nsq_instance_t *instance)
{
    int err;
    
    err = nsqb_IDENTIFY(&instance->bsender, instance->identify);
    if (err<0) {
        return err;
    }

    err = nsqb_send(instance->fd, &instance->bsender);
    if (err<0) {
        return err;
    }

    nsq_fsm_change(instance, NSQ_FSM_IDENTIFYING);

    return 0;
}

int
nsq_subscrib(nsq_instance_t *instance)
{
    int err;
    
    err = nsqb_SUB(&instance->bsender, instance->topic, instance->channel);
    if (err<0) {
        return err;
    }

    err = nsqb_send(instance->fd, &instance->bsender);
    if (err<0) {
        return err;
    }

    nsq_fsm_change(instance, NSQ_FSM_SUBSCRIBING);
    
    return 0;
}

int 
nsq_recver(struct loop_watcher *watcher, time_t now)
{
    nsq_instance_t *instance = (nsq_instance_t *)watcher->user;
    int err;
    
    switch(instance->fsm) {
        case NSQ_FSM_INIT:      /* down */
        case NSQ_FSM_RESOLVED:  /* down */
        case NSQ_FSM_CONNECTED: /* down */
        case NSQ_FSM_HANDSHAKED:/* down */
        case NSQ_FSM_IDENTIFIED:/* down */
        case NSQ_FSM_SUBSCRIBED:
            return -EBADFSM;
        default:
            return -EBADFSM;
    }

    err = nsqb_recv(instance->fd, &instance->brecver);
    if (err<0) {
        return err;
    }
    time_t now = time(NULL);
    
    switch(instance->fsm) {
        case NSQ_FSM_HANDSHAKING:
            err = handshaked_recver(instance, now);
            
            break;
        case NSQ_FSM_IDENTIFYING:
            err = identifying_recver(instance, now);

        case NSQ_FSM_SUBSCRIBING:
            err = subscribing_recver(instance, now);
            
            break;
        case NSQ_FSM_RUN:
            err = run_recver(instance, now);
            
            break;
    }
    
    return 0;
}

/******************************************************************************/
