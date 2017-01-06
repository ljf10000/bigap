/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      libweos
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     libweos
#endif

#ifndef __BUSYBOX__
#define __LIB__
#endif

#define __RUNAS_UNKNOW__

#include "utils.h"
#include "benv/bcookie.h"

akid_t __THIS_only_debugger;
akid_t __THIS_only_jdebugger;

akid_t __THIS_DEBUG;
akid_t __THIS_JDEBUG;

LIB_INITER;
/******************************************************************************/
#include "weos/boot/addr.c"
#include "weos/boot/aes.c"
#include "weos/boot/ak.c"
#include "weos/boot/autoarray.c"
#include "weos/boot/base64.c"
#include "weos/boot/bcookie.c"
#include "weos/boot/benv.c"
#include "weos/boot/bits.c"
#if USE_MOD_BLOB
#include "weos/boot/blob.c"
#endif
#if USE_MOD_CHANNEL
#include "weos/boot/channel.c"
#endif
#if USE_MOD_CQUEUE
#include "weos/boot/cqueue.c"
#endif
#include "weos/boot/cmd.c"
#include "weos/boot/crc32.c"
#include "weos/boot/db.c"
#include "weos/boot/dlist.c"
#include "weos/boot/dump.c"
#include "weos/boot/env.c"
#include "weos/boot/format.c"
#include "weos/boot/hash.c"
#include "weos/boot/hmac.c"
#include "weos/boot/list.c"
#include "weos/boot/md5.c"
#include "weos/boot/rc4.c"
#include "weos/boot/sbuffer.c"
#include "weos/boot/sha2.c"
#if USE_MOD_SLICE
#include "weos/boot/slice.c"
#endif
#include "weos/boot/string.c"
/******************************************************************************/
#include "weos/app/ak.c"
#include "weos/app/bcookie.c"
#include "weos/app/cli.c"
#if USE_MOD_COROUTINE
#include "weos/app/coroutine.c"
#endif
#include "weos/app/debug.c"
#include "weos/app/dll.c"
#include "weos/app/env.c"
#if USE_MOD_FCOOKIE
#include "weos/app/fcookie.c"
#endif
#if USE_MOD_FD
#include "weos/app/fd.c"
#endif
#include "weos/app/file.c"
#include "weos/app/format.c"
#include "weos/app/intf.c"
#include "weos/app/io.c"
#include "weos/app/jlog.c"
#include "weos/app/jscript.c"
#include "weos/app/json.c"
#include "weos/app/loop.c"
#if USE_MOD_NSQ
#include "weos/app/nsq.c"
#endif
#include "weos/app/pipe.c"
#include "weos/app/sem.c"
#include "weos/app/shm.c"
#include "weos/app/this.c"
#include "weos/app/time.c"
#include "weos/app/timer.c"
/******************************************************************************/
