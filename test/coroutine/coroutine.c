/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      coroutine
#endif

#include "utils.h"

OS_INITER;

#define DELAY   (1000*100)
#define COUNT   1000
#define TIMES   10

STATIC int
__coroutine_something(void *data)
{
    char name[1+CO_NAME_LEN];
    int i;

    for (i=0; i<TIMES; i++) {
        os_saprintf(name, "%s.%d", co_self_name(), i);
        co_new(name, __coroutine_something, NULL, 0, 128, 0, false);
    }
    
    co_yield();

    return 0;
}

STATIC int 
__coroutine_fini(void) 
{
    co_fini();

    return 0;
}

STATIC int 
__coroutine_init(void) 
{
    co_init();

    return 0;
}

STATIC int 
__coroutine_main(int argc, char *argv[])
{
    co_id_t one;
    int i = 0;
    
    one = co_new("1", __coroutine_something, NULL, 0, 128, 0, false);

    debug_ok("MAIN start idle");
    co_idle();
    debug_ok("MAIN end idle");

    return 0;
}

int main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    int err = os_call(__coroutine_init, __coroutine_fini, __coroutine_main, argc, argv);

    return shell_error(err);
}

/******************************************************************************/
