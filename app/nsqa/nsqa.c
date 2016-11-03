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
