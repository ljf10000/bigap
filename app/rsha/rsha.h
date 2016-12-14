#ifndef __RSHA_H_693922c61a254f70bc3b1aeb930f449c__
#define __RSHA_H_693922c61a254f70bc3b1aeb930f449c__
/******************************************************************************/
#include "rsh/rsh.h"

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

#ifndef RSHI_ADDRHASHSIZE
#define RSHI_ADDRHASHSIZE           PC_VAL(256, 256)
#endif

#ifndef RSHI_NAMESIZE
#define RSHI_NAMESIZE               32
#endif
/******************************************************************************/
#if 1
#define RSHIST_DIR_ENUM_MAPPER(_)       \
    _(RSHIST_DIR_SEND,   0, "send"),    \
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

#if 1
#define RSH_ECHO_STATE_ENUM_MAPPER(_)       \
    _(RSH_ECHO_STATE_IDLE,  0, "idle"),     \
    _(RSH_ECHO_STATE_BUSY,  1, "busy"),     \
    /* end */
DECLARE_ENUM(rsh_echo_state, RSH_ECHO_STATE_ENUM_MAPPER, RSH_ECHO_STATE_END);

static inline enum_ops_t *rsh_echo_state_ops(void);
static inline bool is_good_rsh_echo_state(int id);
static inline char *rsh_echo_state_getnamebyid(int id);
static inline int rsh_echo_state_getidbyname(const char *name);

#define RSH_ECHO_STATE_IDLE     RSH_ECHO_STATE_IDLE
#define RSH_ECHO_STATE_BUSY     RSH_ECHO_STATE_BUSY
#define RSH_ECHO_STATE_END      RSH_ECHO_STATE_END
#endif

enum {
    RSHI_NIDX_NAME,
    RSHI_NIDX_ADDR,
    
    RSHI_NIDX_END
};
#define RSHI_HASHSIZE_INITER { \
    [RSHI_NIDX_NAME]    = RSHI_NAMEHASHSIZE, \
    [RSHI_NIDX_ADDR]    = RSHI_ADDRHASHSIZE, \
}   /* end */

/*
    json config
    
    {
        "name": "NAME",
        "proxy": "PROXY",
        "register": "REGISTER",
        "cid": CID,
        "port": PORT,
        "seq": SEQ-BEGIN-NUMBER,
        "echo": {
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
*/

typedef struct {
    uint32 interval;
    uint32 times;
    uint32 send;
    uint32 recv;
} rsh_echo_t;

typedef struct {
    char *name;
    char *proxy;
    char *register;
    uint32 cid; // cert id

    sockaddr_in_t client, server;
    rsh_echo_t echo[RSH_ECHO_STATE_END];
    jobj_t jcfg;
    
    int fd;
    int fsm;
    int echo_state;
    int error;
    uint32 fsm_time;
    
    bool loop;
    uint32 seq; 

    char *key;
    byte *key8;
    uint32 key8size;
    uint32 *key32;
    uint32 key32size;

    struct {
        struct {
            uint32 val[RSH_CMD_END][RSHIST_DIR_END][RSHIST_TYPE_END];
        } run;
    } st;

    struct {
        h2_node_t instance;
    } node;
} 
rsh_instance_t;

static inline rsh_echo_t *
rshi_echo(rsh_instance_t *instance)
{
    return &instance->echo[instance->echo_state];
}

static inline void
rshi_echo_clear(rsh_echo_t *echo)
{
    echo->miss  = 0;
    echo->ticks = 0;
}

static inline rsh_echo_t *
rshi_echo_switch(rsh_instance_t *instance, int state)
{
    rshi_echo_clear(rshi_echo(instance));

    instance->echo_state = state;

    return rshi_echo(instance);
}

typedef struct {
    int fd;

    char rbuffer[RSH_MSG_ALLSIZE];
    char wbuffer[RSH_MSG_ALLSIZE];

    loop_t loop;
    uint32 ticks;
    
    struct {
        h2_table_t instance;
    } head;
} rsha_control_t;

extern rsha_control_t rsha;

static inline rsh_msg_t *
rsha_rmsg(void)
{
    return (rsh_msg_t *)rsha.rbuffer;
}

static inline rsh_msg_t *
rsha_wmsg(void)
{
    return (rsh_msg_t *)rsha.wbuffer;
}
/******************************************************************************/
extern int
rshi_fsm_init(rsh_instance_t *instance);

extern int
rshi_fsm(rsh_instance_t *instance, int fsm);

extern int
rshi_insert(jobj_t jobj);

extern int
rshi_remove(char *name);

extern int
rshi_foreach(mv_t (*foreach)(rsh_instance_t *instance), bool safe);

extern rsh_instance_t *
rshi_getbyname(char *name);

extern rsh_instance_t *
rshi_getbyaddr(sockaddr_in_t *addr);

extern int
rshi_show(char *name);

extern int 
rsha_recver(loop_watcher_t *watcher, time_t now);

extern int 
rsha_echo(rsh_instance_t *instance);

extern int 
rsha_ack(rsh_instance_t *instance);

extern int
init_rsha_cli(void);

extern int
init_rsha_timer(void);
/******************************************************************************/
#endif /* __RSHA_H_693922c61a254f70bc3b1aeb930f449c__ */
