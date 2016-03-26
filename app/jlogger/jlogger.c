/*******************************************************************************
Copyright (c) 2015-2016, xxx Networks. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      jlogger
#endif

#ifndef ENV_JLOG_SUB
#define ENV_JLOG_SUB    "__JLOG_SUB__"
#endif

#define __JLOGGER__

#include "utils.h"

OS_INITER;

static char sub[1+OS_APPNAMELEN];

static int
usage(int error)
{
    os_eprintln(__THIS_APP_NAME " pri app json");

    return error;
}

static int 
__main(int argc, char *argv[])
{
    char *app, *obj;
    int err, pri;
    
    if (4 != argc) {
        return usage(-EHELP);
    }

    pri = os_atoi(argv[1]);
    app = argv[2];
    obj = argv[3];
    
    env_copy(ENV_JLOG_SUB, __empty, sub);
    
    err = __jclogger(app, sub, NULL, 0, 0, pri, obj);
    
    return err;
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
