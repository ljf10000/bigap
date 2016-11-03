#ifndef __NSQA_H_138838ae69b44e039c63875789ba5889__
#define __NSQA_H_138838ae69b44e039c63875789ba5889__
/******************************************************************************/
#include "nsq/nsq.h"
/******************************************************************************/
#ifndef NSQ_HASHSIZE
#define NSQ_HASHSIZE        256
#endif

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
#define NSQ_USER_AGENT      "weos/nsqa"
#endif

typedef struct {
    char *conf;
    char *script;
} nsq_config_t;

typedef struct {
    char *name;
    char *domain;
    char *cache;
    
    nsq_buffer_t    sender;
    nsq_buffer_t    recver;
    nsq_identity_t  identify;
    os_sockaddr_t   server;    
    h1_node_t       node;
} nsq_instance_t;

struct nsqa {
    nsq_config_t cfg;

    h1_table_t instance;
};

extern struct nsqa nsqa;

/******************************************************************************/
#endif /* __NSQA_H_138838ae69b44e039c63875789ba5889__ */
