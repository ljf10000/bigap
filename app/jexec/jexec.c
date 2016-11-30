/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      jexec
#endif

#include "utils.h"

OS_INITER;

#define JSON_FORMAT                                     \
    __tab   "json format"                       __crlf  \
    __tab   "{"                                 __crlf  \
    __tab2      "\"content\": \"content\","     __crlf  \
    __tab2      "\"filename\": \"filename\","   __crlf  \
    __tab2      "\"argument\": ["               __crlf  \
    __tab3          "\"arg1\","                 __crlf  \
    __tab3          "\"arg2\","                 __crlf  \
    __tab3          "\"arg3\","                 __crlf  \
    __tab3          "..."                       __crlf  \
    __tab2      "]"                             __crlf  \
    __tab   "}"                                 __crlf  \
    /* end */

static int
usage(int error)
{
    os_eprintln(__THIS_APPNAME " json");
    os_eprintln(JSON_FORMAT);

    return error;
}

static int
__main(int argc, char *argv[])
{
    if (2!=argc) {
        return usage(-EHELP);
    }

    return os_pexec_json(argv[1], NULL);
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(__main, argc, argv);
}
/******************************************************************************/
