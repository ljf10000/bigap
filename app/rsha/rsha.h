#ifndef __RSHA_H_693922c61a254f70bc3b1aeb930f449c__
#define __RSHA_H_693922c61a254f70bc3b1aeb930f449c__
/******************************************************************************/
#include "rsh/rsh.h"

#ifndef RSHA_CACHE
#define RSHA_CACHE                  "/tmp/cache/rsh"
#endif

#ifndef RSHA_FLASH
#define RSHA_FLASH                  "/data/cache/rsh"
#endif

#ifndef RSHA_TICKS
#define RSHA_TICKS                  1000    // ms
#endif

#ifndef RSHA_ECHO_IDLE_INTERVAL
#define RSHA_ECHO_IDLE_INTERVAL     PC_VAL(10*1000, 60*1000)    // ms
#endif

#ifndef RSHA_ECHO_IDLE_TIMES
#define RSHA_ECHO_IDLE_TIMES        PC_VAL(3, 3)
#endif

#ifndef RSHA_ECHO_BUSY_INTERVAL
#define RSHA_ECHO_BUSY_INTERVAL     PC_VAL(2*1000, 5*1000)      // ms
#endif

#ifndef RSHA_ECHO_BUSY_TIMES
#define RSHA_ECHO_BUSY_TIMES        PC_VAL(3, 3)
#endif

#ifndef RSHA_ECHO_BUSY_TIMEOUT
#define RSHA_ECHO_BUSY_TIMEOUT      PC_VAL(300*1000, 150*1000)  // ms
#endif

#ifndef RSHA_FSM_TIMEOUT
#define RSHA_FSM_TIMEOUT            PC_VAL(5*1000, 10*1000)
#endif

#ifndef RSHI_NAMEHASHSIZE
#define RSHI_NAMEHASHSIZE           PC_VAL(256, 256)
#endif

#ifndef RSHI_NAMESIZE
#define RSHI_NAMESIZE               32
#endif

#ifndef RSHI_PEER_ERROR_MAX
#define RSHI_PEER_ERROR_MAX         PC_VAL(8, 32)
#endif
/******************************************************************************/
#if 1
#define RSHIST_DIR_ENUM_MAPPER(_)       \
    _(RSHIST_DIR_SEND,  0, "send"),     \
    _(RSHIST_DIR_RECV,  1, "recv"),     \
    /* end */
DECLARE_ENUM(rshist_dir, RSHIST_DIR_ENUM_MAPPER, RSHIST_DIR_END);

static inline enum_ops_t *rshist_dir_ops(void);
static inline bool is_good_rshist_dir(int id);
static inline char *rshist_dir_getnamebyid(int id);
static inline int rshist_dir_getidbyname(const char *name);

#define RSHIST_DIR_SEND     RSHIST_DIR_SEND
#define RSHIST_DIR_RECV     RSHIST_DIR_RECV
#define RSHIST_DIR_END      RSHIST_DIR_END
#endif

#if 1
#define RSHIST_TYPE_ENUM_MAPPER(_)          \
    _(RSHIST_TYPE_OK,       0, "ok"),       \
    _(RSHIST_TYPE_ERROR,    1, "error"),    \
    /* end */
DECLARE_ENUM(rshist_type, RSHIST_TYPE_ENUM_MAPPER, RSHIST_TYPE_END);

static inline enum_ops_t *rshist_type_ops(void);
static inline bool is_good_rshist_type(int id);
static inline char *rshist_type_getnamebyid(int id);
static inline int rshist_type_getidbyname(const char *name);

#define RSHIST_TYPE_OK      RSHIST_TYPE_OK
#define RSHIST_TYPE_ERROR   RSHIST_TYPE_ERROR
#define RSHIST_TYPE_END     RSHIST_TYPE_END

#define rshist_type(_is_error)  ((_is_error)?RSHIST_TYPE_ERROR:RSHIST_TYPE_OK)
#endif

#if 1
#define RSH_ECHO_ENUM_MAPPER(_)     \
    _(RSH_ECHO_IDLE,    0, "idle"), \
    _(RSH_ECHO_BUSY,    1, "busy"), \
    /* end */
DECLARE_ENUM(rsh_echo, RSH_ECHO_ENUM_MAPPER, RSH_ECHO_END);

static inline enum_ops_t *rsh_echo_ops(void);
static inline bool is_good_rsh_echo(int id);
static inline char *rsh_echo_getnamebyid(int id);
static inline int rsh_echo_getidbyname(const char *name);

#define RSH_ECHO_IDLE   RSH_ECHO_IDLE
#define RSH_ECHO_BUSY   RSH_ECHO_BUSY
#define RSH_ECHO_END    RSH_ECHO_END
#endif

/*
* 1. agent
*   init==>registered==>resolved==>run
*
* 2. proxy
*   init==>registered==>run
*/
#if 1
#define RSH_FSM_ENUM_MAPPER(_)                  \
    _(RSH_FSM_INIT,         0, "init"),         \
    _(RSH_FSM_REGISTERED,   1, "registered"),   \
    _(RSH_FSM_RESOLVED,     2, "resolved"),     \
    _(RSH_FSM_RUN,          3, "run"),          \
    /* end */
DECLARE_ENUM(rsh_fsm, RSH_FSM_ENUM_MAPPER, RSH_FSM_END);

static inline enum_ops_t *rsh_fsm_ops(void);
static inline bool is_good_rsh_fsm(int id);
static inline char *rsh_fsm_getnamebyid(int id);
static inline int rsh_fsm_getidbyname(const char *name);

#define RSH_FSM_INIT            RSH_FSM_INIT
#define RSH_FSM_REGISTERED      RSH_FSM_REGISTERED
#define RSH_FSM_RESOLVED        RSH_FSM_RESOLVED
#define RSH_FSM_RUN             RSH_FSM_RUN
#define RSH_FSM_END             RSH_FSM_END

#define is_rsh_fsm_init(_instance)          (RSH_FSM_INIT==(_instance)->fsm)
#define is_rsh_fsm_retgistered(_instance)   (RSH_FSM_REGISTERED==(_instance)->fsm)
#define is_rsh_fsm_resolved(_instance)      (RSH_FSM_RESOLVED==(_instance)->fsm)
#define is_rsh_fsm_run(_instance)           (RSH_FSM_RUN==(_instance)->fsm)
#endif

/*
    1. rsha json config
    {
        "name": "NAME",                 // must
        "proxy": "PROXY",               // must
        "registry": "REGISTRY",         // must
        "cid": CID,                     // must
        "port": PORT,                   // must
        
        "cache": "CACHE-PATH",          // option
        "flash": "FLASH-PATH",          // option
        "echo": {                       // option
            "idle": {
                "interval": INTERVAL,
                "times", TIMES
            },
            "busy": {
                "interval": INTERVAL,
                "times", TIMES
            }
        }
    }
    
    2. rsh register
    2.1 input json
    {
        "mac": "DEV-BASE-MAC",
        "crypt": "aes",
        "hmactype": "hmac type:sha224/256/384/512",
        "seq": AGENT-BEGIN-SEQ
    }
    
    2.2 output json
    {
        "key": "KEY-HEX-STRING",
        "crypt": "aes",
        "hmactype": "hmac type:sha224/256/384/512",
        "seq": PROXY-BEGIN-SEQ,
        "debug": true/false
    }
*/

typedef struct {
    uint32 interval;
    uint32 times;
    time_t send;
    time_t recv;
} rsh_echo_t;

#if 1
#define RSH_ECHO_JRULE_MAPPER(_) \
    _(offsetof(rsh_echo_t, interval), interval, "interval", \
            u32, sizeof(uint32), 0,                     \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(rsh_echo_t, times), times, "times",      \
            u32, sizeof(uint32), 0,                     \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(rsh_echo_t, send), send, "send",         \
            time, sizeof(time_t), 0,                    \
            JRULE_VAR_TIME,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(rsh_echo_t, recv), recv, "recv",         \
            time, sizeof(time_t), 0,                    \
            JRULE_VAR_TIME,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    /* end */
DECLARE_JRULER(rsh_echo, RSH_ECHO_JRULE_MAPPER);

static inline jrule_t *rsh_echo_jrules(void);

static inline int
__rshi_echo_j2o(rsh_echo_t *echo, jobj_t jobj)
{
    return jrule_j2o(rsh_echo_jrules(), echo, jobj);
}

static inline jobj_t
__rshi_echo_o2j(rsh_echo_t *echo)
{
    return jrule_o2j_ex(rsh_echo_jrules(), echo);
}
#endif

typedef struct {
    uint32 val[RSH_CMD_END][RSHIST_DIR_END][RSHIST_TYPE_END];
} rshi_st_t;

typedef struct {
    time_t fsm[RSH_FSM_END];
    time_t update[RSH_UPDATE_END][RSHIST_DIR_END][RSHIST_TYPE_END];
    time_t command[RSHIST_DIR_END][RSHIST_TYPE_END];
    time_t echo[RSHIST_DIR_END][RSHIST_TYPE_END];
    time_t busy;
} rshi_tm_t;

typedef struct {
    char *hex;
    rsh_key_t key[2];
    uint32 current;
    uint32 update;
} rshi_key_t;

typedef struct {
    char *name;
    char *proxy;
    char *registry;
    char *cache;
    char *flash;
    uint32 cid; // cert id

    rsh_echo_t echo[RSH_ECHO_END];
    bool echo_busy;

    jobj_t jcfg;
    
    int fd;
    int port;
    int fsm;
    int error;
    int crypt;
    int hmactype;
    int hmacsize;
    
    uint32 ip;
    time_t fsm_time[RSH_FSM_END];

    bool   debug;
    uint32 seq; 
    uint32 seq_noack;
    uint32 seq_peer;

    rshi_tm_t tm;
    rshi_st_t st;
    rshi_key_t key;
    
    byte hmac[SHA512_DIGEST_SIZE];
    uint32 peer_error;
    uint32 peer_error_max;

    struct {
        h1_node_t instance;
    } node;
} 
rsh_instance_t;

#define rshi_seq_rand()     ((rand() % 0xffffff) + (rand() % 0xffff) + (rand() % 0xff))

#if 1
#define RSH_INSTANCE_JRULE_MAPPER(_) \
    _(offsetof(rsh_instance_t, cid), cid, "cid",        \
            int, sizeof(int), JRULE_F_MUST,             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(rsh_instance_t, port), port, "port",     \
            int, sizeof(int), JRULE_F_MUST,             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(rsh_instance_t, error), error, "error",  \
            int, sizeof(int), 0,                        \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(rsh_instance_t, peer_error_max), peer_error_max, "peer_error_max", \
            u32, sizeof(uint32), 0,                     \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(rsh_instance_t, peer_error), peer_error, "peer_error", \
            u32, sizeof(uint32), 0,                     \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(rsh_instance_t, seq), seq, "seq",        \
            u32, sizeof(uint32), 0,                     \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(rsh_instance_t, seq_noack), seq_noack, "seq_noack", \
            u32, sizeof(uint32), 0,                     \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(rsh_instance_t, seq_peer), seq_peer, "seq_peer", \
            u32, sizeof(uint32), 0,                     \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(rsh_instance_t, ip), ip, "ip",           \
            ip, sizeof(int), 0,                         \
            JRULE_VAR_IP,                               \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(rsh_instance_t, fsm), fsm, "fsm",        \
            enum, sizeof(int), 0,                       \
            JRULE_VAR_ENUM(rsh_fsm_ops),                \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(rsh_instance_t, proxy), proxy, "proxy",  \
            string, sizeof(char *), JRULE_F_MUST,       \
            JRULE_VAR_STRDUP,                           \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(rsh_instance_t, registry), registry, "registry", \
            string, sizeof(char *), JRULE_F_MUST,       \
            JRULE_VAR_STRDUP,                           \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    _(offsetof(rsh_instance_t, cache), cache, "cache",  \
            string, sizeof(char *), 0,                  \
            JRULE_VAR_STRDUP,                           \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_STRING(RSHA_CACHE)),              \
    _(offsetof(rsh_instance_t, flash), flash, "flash",  \
            string, sizeof(char *), 0,                  \
            JRULE_VAR_STRDUP,                           \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_STRING(RSHA_FLASH)),              \
    _(offsetof(rsh_instance_t, key), key, "key",        \
            string, sizeof(char *), 0,                  \
            JRULE_VAR_STRDUP,                           \
            JRULE_VAR_NULL,                             \
            JRULE_VAR_NULL),                            \
    /* end */
DECLARE_JRULER(rsh_instance, RSH_INSTANCE_JRULE_MAPPER);

static inline jrule_t *rsh_instance_jrules(void);
#endif

#define rshi_st(_instance, _cmd, _dir, _type) \
    (_instance)->st.val[_cmd][_dir][_type]
#define rshi_st_recv(_instance, _cmd, _is_error) \
    rshi_st(_instance, _cmd, RSHIST_DIR_RECV, rshist_type(_is_error))
#define rshi_st_send(_instance, _cmd, _is_error) \
    rshi_st(_instance, _cmd, RSHIST_DIR_SEND, rshist_type(_is_error))

#define rshi_tm_update(_instance, _update, _dir, _type) \
    (_instance)->tm.update[_update][_dir][_type]
#define rshi_tm_update_recv(_instance, _update, _is_error) \
    rshi_tm_update(_instance, _update, RSHIST_DIR_RECV, rshist_type(_is_error))
#define rshi_tm_update_send(_instance, _update, _is_error) \
    rshi_tm_update(_instance, _update, RSHIST_DIR_SEND, rshist_type(_is_error))

#define rshi_tm_command(_instance, _dir, _type) \
    (_instance)->tm.command[_dir][_type]
#define rshi_tm_command_recv(_instance, _is_error) \
    rshi_tm_command(_instance, RSHIST_DIR_RECV, rshist_type(_is_error))
#define rshi_tm_command_send(_instance, _is_error) \
    rshi_tm_command(_instance, RSHIST_DIR_SEND, rshist_type(_is_error))

#define rshi_tm_echo(_instance, _dir, _type) \
    (_instance)->tm.echo[_dir][_type]
#define rshi_tm_echo_recv(_instance, _is_error) \
    rshi_tm_echo(_instance, RSHIST_DIR_RECV, rshist_type(_is_error))
#define rshi_tm_echo_send(_instance, _is_error) \
    rshi_tm_echo(_instance, RSHIST_DIR_SEND, rshist_type(_is_error))

static inline void
rshi_send_over(rsh_instance_t *instance, rsh_over_t *over, bool is_error)
{
    rshi_st_send(instance, over->cmd, is_error)++;
    
    switch(over->cmd) {
        case RSH_CMD_ECHO:
            rshi_tm_echo_send(instance, is_error)++;
            
            break;
        case RSH_CMD_COMMAND:
            rshi_tm_command_send(instance, is_error)++;
            
            break;
        case RSH_CMD_UPDATE:
            if (is_good_rsh_update(over->update)) {
                rshi_tm_update_send(over->update, instance, is_error)++;
            }
            
            break;
    }
}

static inline void
rshi_recv_over(rsh_instance_t *instance, rsh_over_t *over, bool is_error)
{
    rshi_st_recv(instance, over->cmd, is_error)++;
    
    switch(over->cmd) {
        case RSH_CMD_ECHO:
            rshi_tm_echo_recv(instance, is_error)++;
            
            break;
        case RSH_CMD_COMMAND:
            rshi_tm_command_recv(instance, is_error)++;
            
            break;
        case RSH_CMD_UPDATE:
            if (is_good_rsh_update(over->update)) {
                rshi_tm_update_recv(over->update, instance, is_error)++;
            }
            
            break;
    }
}

static inline bool
is_rshi_server_address(rsh_instance_t *instance, sockaddr_in_t *addr)
{
    return instance->ip==addr->sin_addr.s_addr && instance->port==addr->sin_port;
}

static inline void
rshi_echo_clear(rsh_echo_t *echo)
{
    echo->send = 0;
    echo->recv = 0;
}

static inline rsh_echo_t *
rshi_echo_get(rsh_instance_t *instance)
{
    return &instance->echo[instance->echo_busy];
}

extern rsh_echo_t *
rshi_echo_set(rsh_instance_t *instance, time_t now, bool busy);

static inline rsh_key_t *
rshi_key_get(rsh_instance_t *instance)
{
    return &instance->key.key[instance->key.current];
}

static inline rsh_key_t *
rshi_key_pre(rsh_instance_t *instance)
{
    return &instance->key.key[!instance->key.current];
}

extern int
rshi_key_init(rsh_key_t *key, char *keystring);

extern rsh_key_t *
rshi_key_setup(rsh_instance_t *instance, rsh_key_t *key, time_t now);

typedef struct {
    int fd;
    uint32 ticks;

    char buffer[RSH_MSG_ALLSIZE];
    char macstring[1+MACSTRINGLEN];
    byte mac[OS_MACSIZE];
    
    loop_t loop;
    
    struct {
        h1_table_t instance;
    } head;
} rsha_control_t;

extern rsha_control_t rsha;
extern rsh_msg_t *rsha_msg;
extern byte rsha_buffer[];
/******************************************************************************/
extern int
rshi_fsm(rsh_instance_t *instance, int fsm, time_t now);

static inline int
rshi_fsm_restart(rsh_instance_t *instance, time_t now)
{
    return rshi_fsm(instance, RSH_FSM_INIT, now);
}

extern int
rshi_insert(jobj_t jobj);

extern int
rshi_remove(char *name);

extern int
rshi_foreach(mv_t (*foreach)(rsh_instance_t *instance), bool safe);

extern rsh_instance_t *
rshi_getbyname(char *name);

extern int
rshi_show(char *name);

extern int 
rsha_recver(loop_watcher_t *watcher, time_t now);

extern int 
rshi_echo(rsh_instance_t *instance, time_t now);

extern int 
rshi_resolve(rsh_instance_t *instance, time_t now);

extern int 
rshi_run(rsh_instance_t *instance, time_t now);

extern int 
rshi_register(rsh_instance_t *instance, time_t now);

extern int
init_rsha_cli(void);

extern int
init_rsha_timer(void);
/******************************************************************************/
#endif /* __RSHA_H_693922c61a254f70bc3b1aeb930f449c__ */
