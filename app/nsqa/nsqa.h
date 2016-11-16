#ifndef __NSQA_H_138838ae69b44e039c63875789ba5889__
#define __NSQA_H_138838ae69b44e039c63875789ba5889__
/******************************************************************************/
#include "nsq/nsq.h"
/******************************************************************************/
#ifndef NSQ_HASHSIZE
#define NSQ_HASHSIZE        256
#endif
#define NSQ_HASHMASK        (NSQ_HASHSIZE-1)

enum {
    NSQA_SERVER_TIMER,    /* must first */
    NSQA_SERVER_CLI,
    
    NSQA_SERVER_INSTANCE,
    NSQA_SERVER_END = NSQA_SERVER_INSTANCE
};

typedef struct {
    char hostname[1+MACSTRINGLEN_L];
    char client_id[1+MACSTRINGLEN_L];
    
    char *script;
} nsq_config_t;

#define NSQA_CFG_INITER  {  \
    .script = NSQ_SCRIPT,   \
}   /* end */

/*
1. if content:exist, filename:exist, then
        cache must memory/forever
{
    "type": "sh/js",                    #default: sh
    "content": "content",               #must base64 encode
    "filename": "filename",
    "url": "file url for get",
    "md5": "md5 string",
    "argument": [                       #option
        "arg1",
        "arg2",
        "arg3",
        ...
    ],
    "cache": "none/memory/forever",     #default: none
    "scope": "global/instance/topic",   #default: topic
    "slot": SLOT-NUMBER                 #default: 0
    "seq": SEQ,                         #must exist
    "id": "GUID",                       #must exist
    "reply": "COMMAND",                 #option

    "instance": {
        "name": "instance name",        #must exist
        "cache": "global cache path",   #must exist
        "topic": "topic"                #must exist
    }
}
*/

typedef struct {
    int     fd;
    int     fsm;
    uint32  rdy;
    time_t  fsm_time;
    bool    loop;
    bool    auth;
    
    char *name;
    char *domain;
    char *topic;
    char *identify; // json
    char *channel;
        
    byte msgid[NSQ_MSGID_SIZE]; // last message id
    int  error;                 // last error
    
    nsq_buffer_t    bsender;
    nsq_buffer_t    brecver;
    
    sockaddr_in_t   server;
    sockaddr_in_t   client;

    jobj_t          jinstance;
    jobj_t          jauth;

    h1_node_t       node;
} 
nsq_instance_t;

#define is_nsq_fsm_init(_instance)          (NSQ_FSM_INIT==(_instance)->fsm)
#define is_nsq_fsm_resolved(_instance)      (NSQ_FSM_RESOLVED==(_instance)->fsm)
#define is_nsq_fsm_connected(_instance)     (NSQ_FSM_CONNECTED==(_instance)->fsm)
#define is_nsq_fsm_handshaked(_instance)    (NSQ_FSM_HANDSHAKED==(_instance)->fsm)
#define is_nsq_fsm_identifying(_instance)   (NSQ_FSM_IDENTIFYING==(_instance)->fsm)
#define is_nsq_fsm_identified(_instance)    (NSQ_FSM_IDENTIFIED==(_instance)->fsm)
#define is_nsq_fsm_authing(_instance)       (NSQ_FSM_AUTHING==(_instance)->fsm)
#define is_nsq_fsm_authed(_instance)        (NSQ_FSM_AUTHED==(_instance)->fsm)
#define is_nsq_fsm_subscribing(_instance)   (NSQ_FSM_SUBSCRIBING==(_instance)->fsm)
#define is_nsq_fsm_subscribed(_instance)    (NSQ_FSM_SUBSCRIBED==(_instance)->fsm)
#define is_nsq_fsm_run(_instance)           (NSQ_FSM_RUN==(_instance)->fsm)

static inline bool
is_nsq_resolve_ok(nsq_instance_t *instance)
{
    return INADDR_NONE != instance->server.sin_addr.s_addr;
}

static inline nsq_msg_t *
nsq_recver_msg(nsq_instance_t *instance)
{
    return (nsq_msg_t *)instance->brecver.buf;
}

static inline nsq_msg_t *
nsq_sender_msg(nsq_instance_t *instance)
{
    return (nsq_msg_t *)instance->bsender.buf;
}

typedef mv_t nsq_foreach_f(nsq_instance_t *instance);
typedef mv_t nsq_get_f(nsq_instance_t *instance);

typedef struct {
    struct {
        uint32 ticks;
    } st;
    
    struct {
        uint32 ticks;
        char *conf;
        char *cache;
    } env;
    
    nsq_config_t cfg;
    
    loop_t loop;
    
    h1_table_t table;
} nsqa_control_t;

extern nsqa_control_t nsqa;

#define NSQA_INITER         {   \
    .cfg    = NSQA_CFG_INITER,  \
    .loop   = LOOP_INITER,      \
}   /* end */
/******************************************************************************/
extern int
nsqi_insert(char *json);

extern int
nsqi_remove(char *name);

extern nsq_instance_t *
nsqi_get(char *name);

extern int
nsqi_foreach(nsq_foreach_f *foreach, bool safe);

extern int
nsqi_identify(nsq_instance_t *instance, char *json);

extern int
nsqi_fsm(nsq_instance_t *instance, int fsm);
/******************************************************************************/
static inline int 
nsq_send(nsq_instance_t *instance)
{
    return nsqb_send(instance->fd, &instance->bsender);
}

extern int 
nsq_recv(nsq_instance_t *instance);

extern int 
nsq_MAGIC(nsq_instance_t *instance);

extern int
nsq_IDENTIFY(nsq_instance_t *instance);

extern int
nsq_SUB(nsq_instance_t *instance);

extern int
nsq_RDY(nsq_instance_t *instance);

extern int
nsq_FIN(nsq_instance_t *instance);

extern int
nsq_NOP(nsq_instance_t *instance);

extern int
nsq_CLS(nsq_instance_t *instance);

extern int
nsq_REQ(nsq_instance_t *instance);

extern int
ndq_try_rdy(nsq_instance_t *instance);

extern int 
nsq_recver(struct loop_watcher *watcher, time_t now);

extern int
nsq_resolve(nsq_instance_t *instance);

extern int
nsq_connect(nsq_instance_t *instance);

extern int
nsq_handshake(nsq_instance_t *instance);

extern int
nsq_identify(nsq_instance_t *instance);

extern int
nsq_auth(nsq_instance_t *instance);

extern int
nsq_subscrib(nsq_instance_t *instance);

extern int
nsq_run(nsq_instance_t *instance);

extern int
nsq_confuse(nsq_instance_t *instance);

extern int
nsq_script(nsq_instance_t *instance, char *json);
/******************************************************************************/
extern int
init_nsq_timer(void);

extern int
init_nsq_cli(void);

extern int
init_nsq_cfg(void);
/******************************************************************************/
#endif /* __NSQA_H_138838ae69b44e039c63875789ba5889__ */
