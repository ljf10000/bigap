/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      tmc
#endif

#include "utils.h"
#include "tm/tm.h"

OS_INITER;

static cli_client_t tmc = CLI_CLIENT_INITER("tmd");

STATIC int
tmc_main_helper(int argc, char *argv[])
{
    return clic_argv_simple(&tmc, argc-1, argv+1);
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_ignore(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(tmc_main_helper, argc, argv);
}
/******************************************************************************/
