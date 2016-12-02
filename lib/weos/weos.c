/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      libweos
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     libweos
#endif

#define __RUNAS__       1 
#define __LIB__
#include "utils.h"
#include "benv/bcookie.h"
LIB_INITER;
/******************************************************************************/
#include "app/autoarray.c"
#include "app/bcookie.c"
#include "app/bits.c"
#if USE_APP_BLOB
#include "app/blob.c"
#endif
#if USE_APP_CHANNEL
#include "app/channel.c"
#endif
#if USE_APP_COROUTINE
#include "app/coroutine.c"
#endif
#if USE_APP_CQUEUE
#include "app/cqueue.c"
#endif
#include "app/dll.c"
#include "app/env.c"
#if USE_APP_FD
#include "app/fd.c"
#endif
#include "app/file.c"
#include "app/intf.c"
#include "app/io.c"
#include "app/jlog.c"
#include "app/json.c"
#include "app/loop.c"
#include "app/pipe.c"
#if USE_APP_SLICE
#include "app/slice.c"
#endif
#include "app/this.c"
#include "app/timer.c"
/******************************************************************************/
