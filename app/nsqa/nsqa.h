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

typedef struct {
    int fd;
    int fsm;
    time_t fsm_time;
    
    bool loop;
    
    char *name;
    char *domain;
    char *topic;
    char *identify; // json
    char *channel;
    
    char *cache;
    
    byte msgid[NSQ_MSGID_SIZE]; // last message id
    
    nsq_buffer_t    bsender;
    nsq_buffer_t    brecver;
    
    sockaddr_in_t   server;
    sockaddr_in_t   client;
    
    jobj_t          jinstance;
    h1_node_t       node;
} 
nsq_instance_t;

#define is_nsq_fsm_init(_instance)          (NSQ_FSM_INIT==(_instance)->fsm)
#define is_nsq_fsm_resolved(_instance)      (NSQ_FSM_RESOLVED==(_instance)->fsm)
#define is_nsq_fsm_connected(_instance)     (NSQ_FSM_CONNECTED==(_instance)->fsm)
#define is_nsq_fsm_handshaking(_instance)   (NSQ_FSM_HANDSHAKING==(_instance)->fsm)
#define is_nsq_fsm_handshaked(_instance)    (NSQ_FSM_HANDSHAKED==(_instance)->fsm)
#define is_nsq_fsm_identifying(_instance)   (NSQ_FSM_IDENTIFYING==(_instance)->fsm)
#define is_nsq_fsm_identified(_instance)    (NSQ_FSM_IDENTIFIED==(_instance)->fsm)
#define is_nsq_fsm_subscribing(_instance)   (NSQ_FSM_SUBSCRIBING==(_instance)->fsm)
#define is_nsq_fsm_subscribed(_instance)    (NSQ_FSM_SUBSCRIBED==(_instance)->fsm)
#define is_nsq_fsm_run(_instance)           (NSQ_FSM_RUN==(_instance)->fsm)

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
/******************************************************************************/
extern int
nsq_fsm_change(nsq_instance_t *instance, int fsm);

extern int 
nsq_recver(struct loop_watcher *watcher, time_t now);
/******************************************************************************/
extern int
nsq_resolve(nsq_instance_t *instance);

extern int
nsq_connect(nsq_instance_t *instance);

extern int
nsq_handshake(nsq_instance_t *instance);

extern int
nsq_identify(nsq_instance_t *instance);

extern int
nsq_subscrib(nsq_instance_t *instance);
/******************************************************************************/
extern int
init_nsq_timer(void);

extern int
init_nsq_cli(void);

extern int
init_nsq_cfg(void);

extern int
init_nsq_instance(void);
/******************************************************************************/
#endif /* __NSQA_H_138838ae69b44e039c63875789ba5889__ */
