/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      nsqa
#endif

#define __DEAMON__
#include "nsqa.h"

OS_INITER;
/******************************************************************************/
nsqa_t nsqa;

static nsq_instance_t *
nsqi_entry(h1_node_t *node)
{
    if (node) {
        return h1_entry(node, nsq_instance_t, node);
    } else {
        return NULL;
    }
}

static hash_idx_t
nsqi_hash(char *name)
{
    return hash_bybuf(name, os_strlen(name), NSQ_HASHMASK);
}

static nsq_instance_t *
nsqi_create(void)
{
    nsq_instance_t *instance = (nsq_instance_t *)os_zalloc(sizeof(nsq_instance_t));
    if (NULL==instance) {
        return NULL;
    }
    
    return instance;
}

static void
__nsqi_destroy(nsq_instance_t *instance)
{
    if (instance) {
        os_free(instance->name);
        os_free(instance->domain);
        os_free(instance->cache);
    }
}

#define nsqi_destroy(_instance) do{ \
    __nsqi_destroy(_instance);      \
    _instance = NULL;               \
}while(0)

static void
nsqi_insert(nsq_instance_t *instance)
{
    hash_idx_t nhash(hash_node_t *node)
    {
        return nsqi_hash(nsqi_entry(node)->name);
    }
    
    h1_add(&nsqa.table, &instance->node, nsqi_hash);
}

static void
nsqi_remove(nsq_instance_t *instance)
{
    h1_del(&nsqa.table, &instance->node);
}

static nsq_instance_t *
nsqi_get(char *name)
{
    hash_idx_t dhash(void)
    {
        return nsqi_hash(name);
    }
    
    bool eq(hash_node_t *node)
    {
        return os_streq(name, nsqi_entry(node)->name);
    }

    return nsqi_entry(h1_find(&nsqa.table, dhash, eq));
}

static int
nsqi_foreach(nsq_foreach_f *foreach, bool safe)
{
    mv_t node_foreach(h1_node_t *node)
    {
        return (*foreach)(nsqi_entry(node));
    }

    if (safe) {
        return h2_foreach_safe(&nsqa.table, node_foreach);
    } else {
        return h2_foreach(&nsqa.table, node_foreach);
    }
}

static int
__fini(void)
{
    os_fini();

    return 0;
}

static int
__init(void)
{
    int err;
    
    err = os_init();
    if (err<0) {
        return err;
    }

    h1_init(&nsqa.table, NSQ_HASHSIZE);
    
    debug_ok("init ok");
    
    return 0;
}

static void
__signal(int sig)
{
    switch(sig) {
        case SIGALRM: /* timer */
        case SIGUSR1: /* ipcp up */
            rsh_echo_sig = true;

            break;
        default:
            break;
    }
}

static void
__exit(int sig)
{
    __fini();
    
    exit(sig);
}

static int
__service(void)
{
    int len;
    socklen_t addrlen;
    struct sockaddr_in client;
    
    len = io_recvfrom(rsh.fd, rsh_buffer, sizeof(rsh_buffer), RSH_TICKS, (struct sockaddr *)&client, &addrlen);
    if (len<0) {
        return len;
    }
    rsh_buffer[len] = 0;
    
    return __handle(&client);
}

static int
__js_init(void)
{
    duk_context *ctx = NULL;

    duk_context *ctx = js_init("main", argc, argv);
}

int __main(int argc, char *argv[])
{
    duk_context *ctx = js_init("main", argc, argv);
    if (NULL==ctx) {
        return -ENOMEM;
    }
    
    while(1) {
        __service();
        
        __signal_bottom();
    }
    
    return 0;
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(__exit);
    setup_signal_timer(__signal);
    setup_signal_user(__signal);
    setup_signal_callstack(NULL);
    
    int err = os_call(__init, __fini, __main, argc, argv);

    return shell_error(err);
}
/******************************************************************************/
