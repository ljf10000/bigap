/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      rshc
#endif

#include "utils.h"
#include "rsh/rsh.h"

OS_INITER;

static cli_client_t rshc = CLI_CLIENT_INITER("rsha");

STATIC int 
rshc_main_helper(int argc, char *argv[])
{
    return cli_request(&rshc, argc-1, argv+1);
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_ignore(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(rshc_main_helper, argc, argv);
}
/******************************************************************************/
