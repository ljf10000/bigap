/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      libweos
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     libweos
#endif

#define __LIB__
#include "utils.h"
LIB_INITER;
/******************************************************************************/

/******************************************************************************/
#include "blob.c"
#include "channel.c"
#include "coroutine.c"
#include "cqueue.c"
#include "dll.c"
#include "fd.c"
#include "file.c"
#include "json.c"
#include "loop.c"
#include "pipe.c"
#include "slice.c"
#include "timer.c"
/******************************************************************************/
cli_t *
__this_cli(void)
{
    static cli_t cli = CLI_INITER;
    
    if (NULL==cli->b) {
        cli->b = (cli_buffer_t *)os_zalloc(1+CLI_BUFFER_LEN);
    }
    
    return &cli;
}



/******************************************************************************/
