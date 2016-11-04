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
__rt_load(void)
{
    os_pgeti(&runtime, "bootm rt");

    return 0;
}

static int
__rt_save(void)
{
    os_system("bootm rt=%d", runtime);

    return 0;
}
#elif IS_PRODUCT_PC || IS_PRODUCT_LTEFI_MD_PARTITION_B
static int
__rt_load(void)
{
    runtime = benv_mark(__benv_mark_runtime);
    
    return 0;
}

static int
__rt_save(void)
{
    benv_mark(__benv_mark_runtime) = runtime;

    return 0;
}
#endif

static int
rt_load(void)
{
    int err;

    benv_open();
    err = os_callv(benv_load, benv_save_nothing, __rt_load);
        debug_trace_error(err, "rt load");
    benv_close();
    
    return err;
}

static int
rt_save(void)
{
    int err;

    benv_open();
    err = os_callv(benv_load, benv_save_mark, __rt_save);
        debug_trace_error(err, "rt save");
    benv_close();

    return err;
}

static void 
__user(int signo)
{
    debug_trace("recive signo:%d", signo);

    if (SIGUSR2==signo) {
        runtime = 0;
    }

    rt_save();
}

static void 
__exit(int signo)
{
    debug_trace("recive signo:%d", signo);
    
    int err = rt_save();

    os_fini();
    
    exit(shell_error(err));
}

static int
__main(int argc, char *argv[])
{
    int i, err;

    (void)argc;
    (void)argv;
    
    rt_load();
    
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
    setup_signal_user(__user);
    setup_signal_exit(__exit);
    setup_signal_callstack(NULL);
    
    return os_main(__main, argc, argv);
}
/******************************************************************************/
