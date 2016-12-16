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

#ifndef RSHA_FSM_TIMEOUT
#define RSHA_FSM_TIMEOUT            PC_VAL(5*1000, 10*1000)
#endif

#ifndef RSHA_ECHO_BUSY_TIMES
#define RSHA_ECHO_BUSY_TIMES        PC_VAL(3, 3)
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

static inline enum_ops_t *rshist_dir_ops(void);
static inline bool is_good_rshist_dir(int id);
static inline char *rshist_dir_getnamebyid(int id);
static inline int rshist_dir_getidbyname(const char *name);

#define RSHIST_TYPE_OK      RSHIST_TYPE_OK
#define RSHIST_TYPE_ERROR   RSHIST_TYPE_ERROR
#define RSHIST_TYPE_END     RSHIST_TYPE_END
#endif

#if 1
#define RSH_FSM_ENUM_MAPPER(_)                  \
    _(RSH_FSM_INIT,         1, "init"),         \
    _(RSH_FSM_REGISTERED,   2, "registered"),   \
    _(RSH_FSM_RESOLVED,     3, "resolved"),     \
    _(RSH_FSM_RUN,          4, "run"),          \
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
    
    2. rsh register json
    {
        "key": "KEY"
    }
*/

typedef struct {
    uint32 interval;
    uint32 times;
    time_t send;
    time_t recv;
} rsh_echo_t;

typedef struct {
    char *name;
    char *proxy;
    char *registry;
    char *cache;
    char *flash;
    uint32 cid; // cert id

    rsh_echo_t idle;
    rsh_echo_t busy;
    bool echo_busy;
    
    jobj_t jcfg;
    
    int fd;
    int port;
    int fsm;
    int echo_state;
    int error;
    uint32 ip;
    uint32 fsm_time;
    uint32 command_time;
    
    bool loop;
    uint32 seq; 
    uint32 seq_noack;
    uint32 seq_peer;
    
    char *key;
    byte *key8;
    uint32 *key32;
    uint32 keysize;

    uint32 peer_error;
    uint32 peer_error_max;
    
    struct {
        struct {
            uint32 val[RSH_CMD_END][RSHIST_DIR_END][RSHIST_TYPE_END];
        } run;
    } st;

    struct {
        h1_node_t instance;
    } node;
} 
rsh_instance_t;

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
    /* end */
DECLARE_JRULER(rsh_instance, RSH_INSTANCE_JRULE_MAPPER);

static inline jrule_t *rsh_instance_jrules(void);
#endif

#define rshi_st_run(_instance, _cmd, _dir, _type) \
    (_instance)->st.run.val[_cmd][_dir][_type]
#define rshi_st_run_recv_error(_instance, _cmd) \
    rshi_st_run(_instance, _cmd, RSHIST_DIR_RECV, RSHIST_TYPE_ERROR)

#define rshi_unknow_recv_ok(_instance) \
    rshi_st_run(_instance, RSH_CMD_UNKNOW, RSHIST_DIR_RECV, RSHIST_TYPE_OK)
#define rshi_unknow_recv_error(_instance) \
    rshi_st_run(_instance, RSH_CMD_UNKNOW, RSHIST_DIR_RECV, RSHIST_TYPE_ERROR)

#define rshi_command_recv_ok(_instance) \
    rshi_st_run(_instance, RSH_CMD_COMMAND, RSHIST_DIR_RECV, RSHIST_TYPE_OK)
#define rshi_command_recv_error(_instance) \
    rshi_st_run(_instance, RSH_CMD_COMMAND, RSHIST_DIR_RECV, RSHIST_TYPE_ERROR)

#define rshi_echo_recv_ok(_instance) \
    rshi_st_run(_instance, RSH_CMD_ECHO, RSHIST_DIR_RECV, RSHIST_TYPE_OK)
#define rshi_echo_recv_error(_instance) \
    rshi_st_run(_instance, RSH_CMD_ECHO, RSHIST_DIR_RECV, RSHIST_TYPE_ERROR)
#define rshi_echo_send_ok(_instance) \
    rshi_st_run(_instance, RSH_CMD_ECHO, RSHIST_DIR_SEND, RSHIST_TYPE_OK)
#define rshi_echo_send_error(_instance) \
    rshi_st_run(_instance, RSH_CMD_ECHO, RSHIST_DIR_SEND, RSHIST_TYPE_ERROR)

static inline bool
is_rshi_server_address(rsh_instance_t *instance, sockaddr_in_t *addr)
{
    return instance->ip==addr->sin_addr.s_addr && instance->port==addr->sin_port;
}

static inline uint32
rshi_seq(rsh_instance_t *instance)
{
    return (instance->seq_noack = instance->seq++);
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
    if (instance->echo_busy) {
        return &instance->busy;
    } else {
        return &instance->idle;
    }
}

static inline rsh_echo_t *
rshi_echo_set(rsh_instance_t *instance, bool busy)
{
    if (instance->echo_busy != busy) {
        rsh_echo_t *old = rshi_echo_get(instance);
        instance->echo_busy = busy;
        rsh_echo_t *new = rshi_echo_get(instance);

        new->send = old->send;
        new->recv = old->recv;
    }
    
    return rshi_echo_get(instance);
}

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
rshi_echo(rsh_instance_t *instance);

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
