/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      nsqc
#endif

#include "utils.h"

OS_INITER;

STATIC cli_client_t nsqc = CLI_CLIENT_INITER("nsqa");

STATIC int
nsqc_main_helper(int argc, char *argv[])
{
    return clic_argv_simple(&nsqc, argc-1, argv+1);
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_ignore(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(nsqc_main_helper, argc, argv);
}
/******************************************************************************/
