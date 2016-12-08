/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      umc
#endif

#include "utils.h"
#include "um/um.h"

OS_INITER;

static cli_client_t umc = CLI_CLIENT_INITER("umd");

STATIC int 
umc_main_helper(int argc, char *argv[])
{
    return cli_request(&umc, argc-1, argv+1);
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_ignore(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(umc_main_helper, argc, argv);
}

/******************************************************************************/
