/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      jscript
#endif

#include "utils.h"

OS_INITER;
/******************************************************************************/
STATIC int
jscript_usage(int argc, char *argv[])
{
    os_eprintln(__THIS_APPNAME);
    os_eprintln(__tab __THIS_APPNAME " json [args1 args2 args3]");

    return -EINVAL;
}

STATIC int
jscript_main_helper(int argc, char *argv[])
{
    char *json = NULL;
    int err;

    /*
    * get input json
    */
    switch(argc) {
        case 1:
            json = os_readfd(0);
            if (NULL==json) {
                return -ENOMEM;
            }
            
            break;
        case 2:
            json = argv[1];
            
            break;
        default:
            return jscript_usage(argc, argv);
    }
    
    return jscript_exec(json);
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_ignore(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(jscript_main_helper, argc, argv);
}
/******************************************************************************/
