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

#define NSQ_IDENTIFY_FEATURE_NEGOTIATION_NAME   "feature_negotiation"

typedef struct {
    char *client_id;
    char *hostname;
    char *user_agent;
    bool feature_negotiation;
    int msg_timeout;
    int heartbeat_interval;
    int output_buffer_size;
    int output_buffer_timeout;
} nsq_identify_t;

#if USE_JRULE
#define NSQ_IDENTIFY_JRULE_MAPPER(_) \
    _(offsetof(nsq_identify_t, client_id), client_id, "client_id", \
            string, sizeof(char *), JRULE_MUST,         \
            JRULE_VAR_STRASSIGN,                        \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(nsq_identify_t, hostname), hostname, "hostname", \
            string, sizeof(char *), JRULE_MUST,         \
            JRULE_VAR_STRASSIGN,                        \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(nsq_identify_t, user_agent), user_agent, "user_agent", \
            string, sizeof(char *), 0,                  \
            JRULE_VAR_STRASSIGN,                        \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_STRING(NSQ_USER_AGENT)),          \
    _(offsetof(nsq_identify_t, feature_negotiation), feature_negotiation, NSQ_IDENTIFY_FEATURE_NEGOTIATION_NAME, \
            bool, sizeof(bool), 0,                      \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_BOOL(NSQ_NEGOTIATION)),           \
    _(offsetof(nsq_identify_t, feature_negotiation), feature_negotiation, NSQ_IDENTIFY_FEATURE_NEGOTIATION_NAME, \
            int, sizeof(int), 0,                        \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_INT(NSQ_HEARBEAT_INTERVAL)),      \
    _(offsetof(nsq_identify_t, output_buffer_size), output_buffer_size, "output_buffer_size", \
            int, sizeof(int), 0,                        \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_INT(NSQ_SEND_BUFFER_SIZE)),       \
    _(offsetof(nsq_identify_t, output_buffer_timeout), output_buffer_timeout, "output_buffer_timeout", \
            int, sizeof(int), 0,                        \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_INT(NSQ_SEND_BUFFER_TIMEOUT)),    \
    _(offsetof(nsq_identify_t, msg_timeout), msg_timeout, "msg_timeout", \
            int, sizeof(int), 0,                        \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_INT(NSQ_MSG_TIMEOUT)),            \
    /* end */

DECLARE_JRULER(nsq_identify, NSQ_IDENTIFY_JRULE_MAPPER);

static inline jrule_t *nsq_identify_jrules(void);
#endif

#define NSQ_INSTANCE_NAME_NAME      "name"
#define NSQ_INSTANCE_DOMAIN_NAME    "domain"
#define NSQ_INSTANCE_PORT_NAME      "port"
#define NSQ_INSTANCE_TOPIC_NAME     "topic"
#define NSQ_INSTANCE_CHANNEL_NAME   "channel"
#define NSQ_INSTANCE_SCRIPT_NAME    "script"
#define NSQ_INSTANCE_IDENTIFY_NAME  "identify"

typedef struct {
    int     fd;
    int     fsm;
    int     port;
    uint32  rdy;
    time_t  fsm_time;
    bool    loop;
    bool    auth;
    
    char *name;
    char *domain;
    char *topic;
    char *channel;

    char *script;   // self exec script

    byte msgid[NSQ_MSGID_SIZE]; // last message id
    int  error;                 // last error

    nsq_identify_t  identify;

    nsq_buffer_t    bsender;
    nsq_buffer_t    brecver;

    sockaddr_in_t   server;
    sockaddr_in_t   client;

    jobj_t          jidentify;
    jobj_t          jinstance;
    jobj_t          jauth;

    h1_node_t       node;
} 
nsq_instance_t;

#if USE_JRULE
#define NSQ_INSTANCE_JRULE_MAPPER(_) \
    _(offsetof(nsq_instance_t, fsm), fsm, "fsm",        \
            enum, sizeof(int), 0,                       \
            JRULE_VAR_ENUM(nsq_fsm_ops_getter),         \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(nsq_instance_t, rdy), rdy, "rdy",        \
            u32, sizeof(uint), 0,                       \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(nsq_instance_t, fsm_time), fsm_time, "fsm_time", \
            time, sizeof(time_t), 0,                    \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(nsq_instance_t, auth), auth, "auth",     \
            bool, sizeof(bool), 0,                      \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(nsq_instance_t, name), name, NSQ_INSTANCE_NAME_NAME, \
            string, sizeof(char *), JRULE_MUST,         \
            JRULE_VAR_STRASSIGN,                        \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(nsq_instance_t, domain), domain, NSQ_INSTANCE_DOMAIN_NAME, \
            string, sizeof(char *), JRULE_MUST,         \
            JRULE_VAR_STRASSIGN,                        \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(nsq_instance_t, topic), topic, NSQ_INSTANCE_TOPIC_NAME, \
            string, sizeof(char *), JRULE_MUST,         \
            JRULE_VAR_STRASSIGN,                        \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(nsq_instance_t, channel), channel, NSQ_INSTANCE_CHANNEL_NAME, \
            string, sizeof(char *), JRULE_MUST,         \
            JRULE_VAR_STRASSIGN,                        \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(nsq_instance_t, script), script, NSQ_INSTANCE_SCRIPT_NAME, \
            string, sizeof(char *), 0,                  \
            JRULE_VAR_STRASSIGN,                        \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(nsq_instance_t, port), port, NSQ_INSTANCE_PORT_NAME, \
            int, sizeof(int), 0,                        \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_INT(NSQ_PORT)),                   \
    _(offsetof(nsq_instance_t, identify), identify, NSQ_INSTANCE_IDENTIFY_NAME, \
            object, sizeof(nsq_identify_t), JRULE_MUST, \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_RULES(nsq_identify_jrules)),      \
    /* end */

DECLARE_JRULER(nsq_instance, NSQ_INSTANCE_JRULE_MAPPER);

static inline jrule_t *nsq_instance_jrules(void);
#endif

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
        char *flash;
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
nsqi_remove(char *name, char *topic, char *channel);

extern nsq_instance_t *
nsqi_get(char *name, char *topic, char *channel);

extern int
nsqi_show(char *name, char *topic, char *channel);

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
/******************************************************************************/
#endif /* __NSQA_H_138838ae69b44e039c63875789ba5889__ */
