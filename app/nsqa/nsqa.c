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
nsqa_control_t nsqa;

static int
init_nsq_table(void)
{
    int hashsize = env_geti(OS_ENV(HASHSIZE), NSQ_HASHSIZE);
    
    return h1_init(&nsqa.table, hashsize);
}

static int
init_nsq_env(void)
{
    nsqa.conf   = env_gets(OS_ENV(CONFIG),  NSQ_CONF);
    nsqa.cache  = env_gets(OS_ENV(CACHE),   NSQ_CACHE);
    nsqa.script = env_gets(OS_ENV(SCRIPT),  NSQ_SCRIPT);
    nsqa.ticks  = env_geti(OS_ENV(TICKS),   NSQ_TICKS);
    
    return 0;
}

static int
__fini(void)
{
    os_fini();

    __loop_fini(&nsqa.loop);
    
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
        INIT_ENTRY("instance",  init_nsq_instance),
        INIT_ENTRY("msg",       init_nsq_msg),
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
    duk_context *ctx = js_init("main", argc, argv);
    if (NULL==ctx) {
        return -ENOMEM;
    }
    
    while(1) {
        
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
