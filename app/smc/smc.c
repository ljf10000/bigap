/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      smc
#endif

#include "utils.h"
#include "sm/sm.h"

OS_INITER;

static cli_client_t smc = CLI_CLIENT_INITER("smd");

STATIC int 
smc_main_helper(int argc, char *argv[])
{
    return clic_argv_simple(&smc, argc-1, argv+1);
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_ignore(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(smc_main_helper, argc, argv);
}
/******************************************************************************/
