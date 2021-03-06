/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      rt
#endif

#define __DEAMON__

#include "utils.h"
#include "benv/benv.h"

OS_INITER;

#if !(IS_PRODUCT_LTEFI_MD || IS_PRODUCT_PC)
#error "just support ltefi md/pc board"
#endif

BENV_INITER;

enum {
    RT_SAVECYCLE = 3141,
};

static int runtime;
static int savecycle;

#if IS_PRODUCT_LTEFI_MD_PARTITION_A
STATIC int
rt_load(void)
{
    os_pgeti(&runtime, "bootm rt");

    return 0;
}

STATIC int
rt_save(void)
{
    os_system("bootm rt=%d", runtime);

    return 0;
}
#elif IS_PRODUCT_PC || IS_PRODUCT_LTEFI_MD_PARTITION_B
STATIC int
rt_load(void)
{
    benv_load();
    runtime = benv_mark(__benv_mark_runtime);
    
    return 0;
}

STATIC int
rt_save(void)
{
    benv_load();
    benv_mark(__benv_mark_runtime) = runtime;
    benv_save_mark();
    
    return 0;
}
#endif

STATIC int
rt_fini(void)
{
    rt_save();
    
    benv_close();
    
    os_fini();

    return 0;
}

STATIC int
rt_init(void)
{
    int err;
    
    err = os_init();
    if (err<0) {
        return err;
    }

    err = benv_open();
    if (err<0) {
        return err;
    }

    err = rt_load();
    if (err<0) {
        return err;
    }
    
    return 0;
}

STATIC void 
rt_user(int signo)
{
    debug_trace("recive signo:%d", signo);

    if (SIGUSR2==signo) {
        runtime = 0;
    }

    rt_save();
}

STATIC void 
rt_exit(int sig)
{
    rt_fini();
    
    exit(sig);
}

STATIC int
rt_main_helper(int argc, char *argv[])
{
    int i, err;

    (void)argc;
    (void)argv;

    savecycle = env_geti("RT_SAVECYCLE", RT_SAVECYCLE);
        
    while(1) {
        sleep(1);

        runtime++;
        if ((RT_SAVECYCLE-1)==(runtime%RT_SAVECYCLE)) {
            rt_save();
        }
    }

    return 0;
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_user(rt_user);
    setup_signal_exit(rt_exit);
    setup_signal_ignore(NULL);
    setup_signal_callstack(NULL);
    
    return os_call(rt_init, rt_fini, rt_main_helper, argc, argv);
}
/******************************************************************************/
