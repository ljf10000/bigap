/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
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
    os_eprintln(__THIS_APPNAME " {pri} {app} {json}");
    os_eprintln(__THIS_APPNAME " {pri} {app} {format} {values...}");
    os_eprintln(__tab "set env " ENV_JSUB  " to submodule");
    os_eprintln(__tab "set env " ENV_JFILE " to filename");
    os_eprintln(__tab "set env " ENV_JFUNC " to funcname");
    os_eprintln(__tab "set env " ENV_JLINE " to logline");

    return error;
}

static int 
__main(int argc, char *argv[])
{
    char *pri   = get_argv(1);
    char *app   = get_argv(2);
    char *json  = get_argv(3);
    char *format= json;
    
    if (argc < 4) {
        return usage(-EHELP);
    }
    
    char *sub   = env_gets(ENV_JSUB, NULL);
    char *file  = env_gets(ENV_JFILE, NULL);
    char *func  = env_gets(ENV_JFUNC, NULL);
    char *line  = env_gets(ENV_JLINE, NULL);

    switch(json[0]) {
        case '{':
            return __clogger(app, 
                sub, 
                file, 
                func, 
                os_atoi(line), 
                os_atoi(pri), 
                json);
        case '%':
            if (argc < 5) {
                return usage(-EHELP);
            }
            
            return __jformat(app,
                sub, 
                file, 
                func, 
                os_atoi(line), 
                os_atoi(pri), 
                format,
                argc-2,
                argv+2);
        default:
            return -EFORMAT;
    }
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
