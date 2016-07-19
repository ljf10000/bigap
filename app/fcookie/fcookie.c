/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      fcookie
#endif

#include "utils.h"
#include "fcookie/fcookie.h"

OS_INITER;

/*
* cmd have enabled when boot
*/
static int
__main(int argc, char *argv[])
{
    int type;

    if (argc<3) {
        return EFORMAT;
    }

    type = os_atoi(argv[1]);

    switch(type) {
        case FCOOKIE_FILE: {
            int id = os_atoi(argv[2]);
            char *file = fcookie_file(id);
            if (NULL==file) {
                return ENOEXIST;
            }

            os_println("%s", file);
        }   break;
        case FCOOKIE_DOMAIN: {
            char *domain = argv[2];
            
        }   break;
        default:
            return EFORMAT;
    }

    return 0;
}

#ifndef __BUSYBOX__
#define fcookie_main  main
#endif

/*
* cmd have enabled when boot
*/
int fcookie_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(__main, argc, argv);
}
/******************************************************************************/
