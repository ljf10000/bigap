#ifndef __NSQA_H_138838ae69b44e039c63875789ba5889__
#define __NSQA_H_138838ae69b44e039c63875789ba5889__
/******************************************************************************/
#include "nsq/nsq.h"
/******************************************************************************/
#ifndef NSQ_HASHSIZE
#define NSQ_HASHSIZE        256
#endif
#define NSQ_HASHMASK        (NSQ_HASHSIZE-1)

#ifndef NSQ_PORT
#define NSQ_PORT            100
#endif

#ifndef NSQ_TICKS
#define NSQ_TICKS           PC_VAL(5, 5)   /* second */
#endif

#ifndef NSQ_CONF
#define NSQ_CONF            PC_FILE("/tmp/config", "nsqa.conf")
#endif

#ifndef NSQ_CACHE
#define NSQ_CACHE           "/tmp/cache"
#endif

#ifndef NSQ_SCRIPT
#define NSQ_SCRIPT          PC_FILE("/lib/script", "nsq.script")
#endif

#ifndef NSQ_USER_AGENT
#define NSQ_USER_AGENT      "weos-nsq-agent"
#endif

#ifndef NSQ_TOPIC
#define NSQ_TOPIC           "weos.nsq.topic.default"
#endif

enum {
    NSQA_SERVER_TIMER,    /* must first */
    NSQA_SERVER_CLI,
    
    NSQA_SERVER_INSTANCE,
    NSQA_SERVER_END = NSQA_SERVER_INSTANCE
};

typedef struct {
    char *hostname;
    char *client_id;
    
    char *script;
} nsq_config_t;

typedef struct {
    char *name;
    char *domain;
    char *cache;
    char *identify; // json
    
    nsq_buffer_t    sender;
    nsq_buffer_t    recver;
    os_sockaddr_t   addr;
    
    h1_node_t       node;
} nsq_instance_t;

typedef mv_t nsq_foreach_f(nsq_instance_t *instance);
typedef mv_t nsq_get_f(nsq_instance_t *instance);

typedef struct {
    int ticks;
    char *conf;
    char *cache;
    char *script;
    
    nsq_config_t cfg;

    loop_t loop;
    
    h1_table_t table;
} nsqa_control_t;

extern nsqa_control_t nsqa;
/******************************************************************************/
extern nsq_instance_t *
nsqi_entry(hash_node_t *node);

extern nsq_instance_t *
nsqi_hx_entry(h1_node_t *node);

extern hash_idx_t
nsqi_hash(char *name);

extern nsq_instance_t *
nsqi_create(void);

extern void
__nsqi_destroy(nsq_instance_t *instance);

#define nsqi_destroy(_instance) do{ \
    __nsqi_destroy(_instance);      \
    _instance = NULL;               \
}while(0)

extern void
nsqi_insert(nsq_instance_t *instance);

extern void
nsqi_remove(nsq_instance_t *instance);

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
