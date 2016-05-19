/*******************************************************************************
Copyright (c) 2015-2016, xxx Networks. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      rsha
#endif

#include "utils.h"
#include "rsh/rsh.h"

OS_INITER;

typedef struct {
    char *echo;

    
} rsh_agent_t;

static rsh_config_t rsh_cfg     = RSH_CONFIG_INITER;
static rsh_agent_t  rsha;

#ifndef __BUSYBOX__
#define rsha_main  main
#endif

int rsha_main(int argc, char **argv)
{
    return os_main(__main, argc, argv);
}


/******************************************************************************/
