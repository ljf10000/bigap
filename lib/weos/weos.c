/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      libweos
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     libweos
#endif

#include "utils.h"
LIB_INITER;
/******************************************************************************/
#include "pipe.c"
#include "timer.c"
#include "channel.c"
#include "cqueue.c"
#include "coroutine.c"
#include "fd.c"
#include "json.c"
#include "loop.c"
#include "blob.c"
#include "slice.c"
#include "file.c"
#include "dll.c"
#include "jlog.c"
#include "ak.c"
/******************************************************************************/
int
__os_system(char *cmd)
{
    int err;

    err = system(cmd);
        debug_shell("%s error:%d", cmd, err);
    if (127==err || -1==err) {
        return -ESYSTEM;
	} else {
        return __os_wait_error(err);
	}
}


/******************************************************************************/
