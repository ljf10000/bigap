/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifdef BIGAP
#ifndef __THIS_APP
#define __THIS_APP      cmd_benv
#endif

#include "benv/benv.h"

static int 
do_benv (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    return __benv_main(argc, argv);
}

U_BOOT_CMD(
    benv,  CFG_MAXARGS, 0,  do_benv,
    "get/set benv",
    "\n"
    "    benv name ==> show env by name\n"
    "    benv name1=value1 name2=value2 ... ==> set env by name and value"
);

#endif
/******************************************************************************/
