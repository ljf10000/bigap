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

STATIC int
init_nsqa_cfg(void)
{
    jobj_t jobj = JOBJ_MAPFILE(nsqa.env.conf, NSQA_CFG_JMAPPER);
    if (NULL==jobj) {
        return -EBADCONF;
    }

    char *mac = os_getmacby(SCRIPT_GETBASEMAC);
    os_strdcpy(nsqa.cfg.hostname, mac);
    os_strdcpy(nsqa.cfg.client_id, mac);

    jobj_put(jobj);
    
    return 0;
}

STATIC int
init_nsqa_table(void)
{
    int hashsize = env_geti(OS_ENV(HASHSIZE), NSQ_HASHSIZE);
    
    return h1_init(&nsqa.table, hashsize);
}

STATIC int
init_nsqa_env(void)
{
    nsqa.env.conf   = env_gets(OS_ENV(CONFIG),  NSQ_CONF);
    nsqa.env.ticks  = env_geti(OS_ENV(TICKS),   NSQ_TICKS);
    nsqa.env.cache  = env_gets(OS_ENV(CACHE),   NSQ_CACHE);
    nsqa.env.flash  = env_gets(OS_ENV(FLASH),   NSQ_FLASH);
    
    return 0;
}

STATIC int
nsqa_fini(void)
{
    os_fini();

    os_loop_fini(&nsqa.loop);
    
    return 0;
}

STATIC int
nsqa_init(void)
{
    static os_initer_t map[] = {
        INIT_ENTRY("os",        os_init),
        INIT_ENTRY("env",       init_nsqa_env),
        INIT_ENTRY("table",     init_nsqa_table),
        INIT_ENTRY("timer",     init_nsqa_timer),
        INIT_ENTRY("cli",       init_nsqa_cli),
        INIT_ENTRY("cfg",       init_nsqa_cfg),
    };

    return os_initer(map, os_count_of(map));
}

STATIC void
nsqa_signal(int sig)
{
    switch(sig) {
        case SIGALRM: /* timer */
        case SIGUSR1: /* ipcp up */
            break;
        default:
            break;
    }
}

STATIC void
nsqa_exit(int sig)
{
    nsqa_fini();
    
    exit(sig);
}

STATIC int
nsqa_main_helper(int argc, char *argv[])
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
    setup_signal_exit(nsqa_exit);
    setup_signal_ignore(NULL);
    setup_signal_user(nsqa_signal);
    setup_signal_callstack(NULL);
    
    int err = os_call(nsqa_init, nsqa_fini, nsqa_main_helper, argc, argv);

    return shell_error(err);
}
/******************************************************************************/
