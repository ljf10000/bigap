/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      jlogger
#endif

#define __JLOGGER__

#include "utils.h"

static int
usage(int error)
{
    os_eprintln(__THIS_APPNAME " {app} {file} {func} {pri} {json}");
    os_eprintln(__THIS_APPNAME " {app} {file} {func} {pri} {format} {values...}");

    return error;
}

static int 
__main(int argc, char *argv[])
{
    char *app   = get_argv(1);
    char *file  = get_argv(2);
    char *func  = get_argv(3);
    char *pri   = get_argv(4);
    char *json  = get_argv(5);
    char *format= json;
    
    if (argc < 6) {
        return usage(-EHELP);
    }

    switch(json[0]) {
        case '{':
            return __clogger(app, 
                NULL,   // sub
                file, 
                func, 
                0,      // line
                os_atoi(pri), 
                json);
        case '%':
            if (argc < 7) {
                return usage(-EHELP);
            }
            
            return __jformat(app,
                NULL,   // sub
                file, 
                func, 
                0,      // line
                os_atoi(pri), 
                format,
                argc-6,
                argv+6);
        default:
            return -EFORMAT;
    }
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(__main, argc, argv);
}
/******************************************************************************/
