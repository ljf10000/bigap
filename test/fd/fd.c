/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      fd
#endif

#define __EXTEND__

#include "utils.h"

OS_INITER;

#define SERVER "server"
#define CLIENT "client"

static int
server(void *no_used)
{
    return 0;
}

static int
client(void *no_used)
{
    return 0;
}

static int 
as_server(int argc, char *argv[])
{
    fd_looper();
    co_new("server", server, NULL, 0, CO_PRI_DEFAULT, 0, false);
    co_idle();

    return 0;
}

static int
as_client(int argc, char *argv[])
{
    
    return 0;
}

static int
help(void)
{
    return 0;
}

static int 
__fini(void) 
{
    fd_fini();

    return 0;
}

static int 
__init(void) 
{    
    fd_init();

    return 0;
}

STATIC int
fd_main_helper(int argc, char *argv[])
{
    int err;
    
    if (argc < 2) {
        return help();
    }
    
    argc--; argv++;
    
    if (0==os_strcmp(SERVER, argv[0])) {
        err = as_server(argc, argv);
    }
    else if (0==os_strcmp(CLIENT, argv[0])) {
        err = as_client(argc, argv);
    }
    else {
        err = help();
    }

    return err;
}

int main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_ignore(NULL);
    setup_signal_callstack(NULL);
    
    int err = os_call(__init, __fini, fd_main_helper, argc, argv);

    return shell_error(err);
}

/******************************************************************************/
