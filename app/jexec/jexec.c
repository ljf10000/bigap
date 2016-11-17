/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      jexec
#endif

#include "utils.h"

OS_INITER;

static int 
jcallback(int error, char *outsring, char *errstring)
{
    os_println("stdout:%s", outsring);
    os_println("stderr:%s", errstring);
    os_println("errno:%d",  error);

    return 0;
}

/*
{
    "content": "content",               #must base64 encode
    "filename": "filename",
    "argument": [                       #option
        "arg1",
        "arg2",
        "arg3",
        ...
    ]
}
*/
static pipinfo_t jinfo = PIPINFO_INITER(NULL, jcallback);

static int
jmap(char *json)
{
    return 0;
}

static int
usage(int error)
{
    os_eprintln(__THIS_APPNAME " json");

    return error;
}

static int
__main(int argc, char *argv[])
{
    int err;

    if (2!=argc) {
        return usage(-EHELP);
    }
    
    err = jmap(argv[1]);
    if (err<0) {
        return err;
    }
    
    return os_pexecv(&jinfo);
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(__main, argc, argv);
}
/******************************************************************************/
