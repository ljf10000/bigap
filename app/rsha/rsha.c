/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      rsha
#endif

#define __DEAMON__
#include "rsha.h"

OS_INITER;
/******************************************************************************/
rsha_control_t rsha;

/******************************************************************************/
static int
rsha_fini(void)
{
    os_fini();

    return 0;
}

static int
rsha_init(void)
{
    int err;
    
    err = os_init();
    if (err<0) {
        return err;
    }

    err = os_loop_init(&rsha.loop);
    if (err<0) {
        debug_error("init loop error:%d", err);
        
        return err;
    }

    err = init_rsha_timer();
    if (err<0) {
        debug_error("init timer error:%d", err);
        
        return err;
    }

    int hash_size[RSHI_NIDX_END] = RSHI_HASHSIZE_INITER;
    err = h2_init(&rsha.head.instance, hash_size);
    if (err<0) {
        debug_error("init instance hash:%d", err);
        
        return err;
    }
    
    debug_ok("init ok");
    
    return 0;
}

static void
rsha_exit(int sig)
{
    rsha_fini();
    
    exit(sig);
}

STATIC int
rsha_main_helper(int argc, char *argv[])
{
    return os_loop(&rsha.loop);
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(rsha_exit);
    setup_signal_ignore(NULL);
    setup_signal_callstack(NULL);

    int err = os_call(rsha_init, rsha_fini, rsha_main_helper, argc, argv);

    return shell_error(err);
}
/******************************************************************************/
