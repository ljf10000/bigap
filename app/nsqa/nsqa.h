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
    char *name;
    char *domain;
    char *topic;
    char *identify; // json
    
    char *cache;

    jobj_t jobj;
    
    nsq_buffer_t    sender;
    nsq_buffer_t    recver;
    sockaddr_in_t   addr;
    
    h1_node_t       node;
} nsq_instance_t;

typedef mv_t nsq_foreach_f(nsq_instance_t *instance);
typedef mv_t nsq_get_f(nsq_instance_t *instance);

typedef struct {
    int ticks;
    char *conf;
    char *cache;
    
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
init_nsq_timer(void);

extern int
init_nsq_cli(void);

extern int
init_nsq_cfg(void);

extern int
init_nsq_instance(void);

extern int
init_nsq_msg(void);

/******************************************************************************/
#endif /* __NSQA_H_138838ae69b44e039c63875789ba5889__ */
