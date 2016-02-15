/*******************************************************************************
Copyright (c) 2015-2016, xxx Networks. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      rshc
#endif

#include "utils.h"
#include "rsh/rsh.h"

OS_INITER;
/*
* rsha: reverse shell agent, on work net
* rshd: reverse shell server, on internet
* rshc: reverse shell client, on person pc
*/

#ifndef __BUSYBOX__
#define rshd_main  main
#endif

int rshd_main(int argc, char **argv)
{
    return os_main(__main, argc, argv);
}


/******************************************************************************/
