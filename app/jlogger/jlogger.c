/*******************************************************************************
Copyright (c) 2015-2016, xxx Networks. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      jlogger
#endif

#define __JLOGGER__

#include "utils.h"

OS_INITER;

static int
usage(int error)
{
    os_eprintln(__THIS_APP_NAME " {pri} {app} {json} [sub] [file] [func] [line]");

    return error;
}

static int 
__main(int argc, char *argv[])
{
    if (argc < 4) {
        return usage(-EHELP);
    }

    char *pri   = get_argv(1);
    char *app   = get_argv(2);
    char *json  = get_argv(3);
    char *sub   = get_argv(4);
    char *file  = get_argv(5);
    char *func  = get_argv(6);
    char *line  = get_argv(7);
    
    return __clogger(app, sub, file, func, os_atoi(line), os_atoi(pri), json);
}

#ifndef __BUSYBOX__
#define jlogger_main  main
#endif

int jlogger_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(__main, argc, argv);
}
/******************************************************************************/
