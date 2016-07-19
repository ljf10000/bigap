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

#if IS_PRODUCT_LTEFI_MD_PARTITION_A
static int
__rt_load(int idx)
{
    (void)idx;

    os_pgeti(&runtime, "bootm rt");

    return 0;
}

static int
__rt_save(int idx)
{
    (void)idx;
    
    os_system("bootm rt=%d", runtime);

    return 0;
}
#elif IS_PRODUCT_PC || IS_PRODUCT_LTEFI_MD_PARTITION_B
static int
__rt_load(int idx)
{
    (void)idx;
    
    runtime = benv_mark_get(__benv_mark_runtime);
    
    return 0;
}

static int
__rt_save(int idx)
{
    (void)idx;

    benv_mark_set(__benv_mark_runtime,  runtime);

    return 0;
}
#endif

static int
rt_load(int noused)
{
    int err;
    
    (void)noused;
    
    err = os_call_1(__benv_load, __benv_save_nothing, __rt_load, BENV_MARK);
        debug_trace_error(err, "rt load");

    return err;
}

static int
rt_save(int noused)
{
    int err;
    
    (void)noused;
    
    err = os_call_1(__benv_load, __benv_save, __rt_save, BENV_MARK);
        debug_trace_error(err, "rt save");

    return err;
}

static void 
__user(int signo)
{
    debug_trace("recive signo:%d", signo);

    if (SIGUSR2==signo) {
        runtime = 0;
    }

    os_call(benv_open, benv_close, rt_save, 0);
}

static void 
__exit(int signo)
{
    int err;

    debug_trace("recive signo:%d", signo);
    
    err = os_call(benv_open, benv_close, rt_save, 0);

    os_fini();
    
    exit(err);
}

static int
__main(int argc, char *argv[])
{
    int i, err;

    (void)argc;
    (void)argv;
    
    os_call(benv_open, benv_close, rt_load, 0);

    while(1) {
        sleep(1);

        runtime++;
        if ((RT_SAVECYCLE-1)==(runtime%RT_SAVECYCLE)) {
            os_call(benv_open, benv_close, rt_save, 0);
        }
    }

    return 0;
}

#ifndef __BUSYBOX__
#define rt_main  main
#endif

int rt_main(int argc, char *argv[])
{
    setup_signal_user(__user);
    setup_signal_exit(__exit);
    setup_signal_callstack(NULL);
    
    return os_main(__main, argc, argv);
}
/******************************************************************************/
