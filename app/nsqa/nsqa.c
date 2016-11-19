/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      nsqa
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     nsqa
#endif

#define __DEAMON__
#include "nsqa.h"

OS_INITER;
/******************************************************************************/
nsqa_control_t nsqa = NSQA_INITER;

#define NSQ_JMAPPER(_)                  \
    _(&nsqa.cfg, string, script)        \
    /* end */

STATIC int
init_nsq_cfg(void)
{
    jobj_t jobj = JOBJ_MAPF(nsqa.env.conf, NSQ_JMAPPER);
    if (NULL==jobj) {
        return -EBADCONF;
    }

    char *mac = os_getbasemac();
    os_strdcpy(nsqa.cfg.hostname, mac);
    os_strdcpy(nsqa.cfg.client_id, mac);

    jobj_put(jobj);
    
    return 0;
}

STATIC int
init_nsq_table(void)
{
    int hashsize = env_geti(OS_ENV(HASHSIZE), NSQ_HASHSIZE);
    
    return h1_init(&nsqa.table, hashsize);
}

STATIC int
init_nsq_env(void)
{
    nsqa.env.conf   = env_gets(OS_ENV(CONFIG),  NSQ_CONF);
    nsqa.env.ticks  = env_geti(OS_ENV(TICKS),   NSQ_TICKS);
    nsqa.env.cache  = env_gets(OS_ENV(CACHE),   NSQ_CACHE);
    nsqa.env.flash  = env_gets(OS_ENV(FLASH),   NSQ_FLASH);
    
    return 0;
}

static int
__fini(void)
{
    os_fini();

    os_loop_fini(&nsqa.loop);
    
    return 0;
}

static int
__init(void)
{
    static os_initer_t map[] = {
        INIT_ENTRY("os",        os_init),
        INIT_ENTRY("env",       init_nsq_env),
        INIT_ENTRY("table",     init_nsq_table),
        INIT_ENTRY("timer",     init_nsq_timer),
        INIT_ENTRY("cli",       init_nsq_cli),
        INIT_ENTRY("cfg",       init_nsq_cfg),
    };

    return os_initer(map, os_count_of(map));
}

static void
__signal(int sig)
{
    switch(sig) {
        case SIGALRM: /* timer */
        case SIGUSR1: /* ipcp up */
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

int __main(int argc, char *argv[])
{
#if 0
    duk_context *ctx = js_init("main", argc, argv);
    if (NULL==ctx) {
        return -ENOMEM;
    }
#endif

    return os_loop(&nsqa.loop);
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(__exit);
    setup_signal_user(__signal);
    setup_signal_callstack(NULL);
    
    int err = os_call(__init, __fini, __main, argc, argv);

    return shell_error(err);
}
/******************************************************************************/
