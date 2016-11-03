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
    nsq_config_t cfg;
    
    loop_t loop;
    
    h1_table_t table;
} nsqa_t;

extern nsqa_t nsqa;
/******************************************************************************/
#endif /* __NSQA_H_138838ae69b44e039c63875789ba5889__ */
