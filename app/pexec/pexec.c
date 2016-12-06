/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      pexec
#endif

#include "utils.h"

OS_INITER;

static int
usage(int error)
{
    os_eprintln(__THIS_APPNAME " insert {name} delay(second) interval(second) limit command");
    os_eprintln(__THIS_APPNAME " remove {name}");
    os_eprintln(__THIS_APPNAME " show [name]");

    return error;
}

STATIC int
pexec_main_helper(int argc, char *argv[])
{
    int err;
    
    if (1==argc) {
        return usage(-EHELP);
    }

    err = command(argc-1, argv+1);
    if (err<0) {
        /* just log, NOT return */
    }

    return err;
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_ignore(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(pexec_main_helper, argc, argv);
}
/******************************************************************************/
