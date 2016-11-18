#ifndef __UTILS_H_6bb0dde2b3424f97a297b2b0e37d90aa__
#define __UTILS_H_6bb0dde2b3424f97a297b2b0e37d90aa__
/******************************************************************************/
#ifdef __FASTBOOT__
#define __BOOT__
#endif

#define STATIC

#ifdef __OPENWRT__
#   ifndef LINUX
#       define LINUX
#   endif
#endif

#ifdef __BUSYBOX__
#define __ALLINONE__
#endif

#if !defined(__GLIBC__) && !defined(__UCLIBC__)
#define __UCLIBC__
#endif

#ifdef __BOOT__
#   define __UCLIBC__
#   include <malloc.h>
#   include <common.h>
#   include <command.h>
#   include <linux/ctype.h>
#   include <linux/string.h>
#elif defined(__KERNEL__)
#   include <linux/moduleparam.h>
#   include <linux/module.h>
#   include <linux/skbuff.h>
#   include <linux/gfp.h>
#   include <linux/init.h>
#   include <linux/in.h>
#   include <linux/inet.h>
#   include <linux/netlink.h>
#   include <linux/rtc.h>
#   include <linux/ctype.h>
#   include <linux/string.h>
#   include <linux/time.h>
#   include <linux/errno.h>
#   include <linux/list.h>
#   include <net/sock.h>
#else /* __APP__ */
#   define __APP__
#   ifdef __BUSYBOX__
#       include "libbb.h"
#   endif
#   define RUN_AS_DEAMON        1
#   define RUN_AS_COMMAND       2
#   define RUN_AS_UNKNOW        (RUN_AS_DEAMON|RUN_AS_COMMAND)
#   ifndef __RUNAS__
#       ifdef __RUNAS_UNKNOW__
#           define __RUNAS__    RUN_AS_UNKNOW
#       elif defined(__DEAMON__)
#           define __RUNAS__    RUN_AS_DEAMON
#       else
#           define __RUNAS__    RUN_AS_COMMAND
#       endif
#   endif
#   include <stdint.h>
#   include <stdarg.h>
#   include <stdlib.h>
#   include <stdio.h>
#   include <signal.h>
#   include <string.h>
#   include <malloc.h>
#   include <assert.h>
#   include <unistd.h>
#   include <fcntl.h>
#   include <ctype.h>
#   include <errno.h>
#   include <grp.h>
#   include <pwd.h>
#   include <time.h>
#   include <math.h>
#   include <utime.h>
#   include <dirent.h>
#   include <syslog.h>
#   include <ucontext.h>
#   include <byteswap.h>
#   include <libgen.h>
#   include <netdb.h>
#   include <termios.h>
#   include <ulimit.h>
#   include <utmp.h>
#   include <sched.h>
#ifndef __OPENWRT__
#   include <fstab.h>
#endif
#   include <mntent.h>
#   include <limits.h>
#   include <dlfcn.h>
#   include <endian.h>
#   include <sys/types.h>
#   include <sys/stat.h>
#   include <sys/file.h>
#   include <sys/wait.h>
#   include <sys/ioctl.h>
#   include <sys/ipc.h>
#   include <sys/msg.h>
#   include <sys/sem.h>
#   include <sys/shm.h>
#   include <sys/socket.h>
#   include <sys/un.h>
#   include <sys/mman.h>
#   include <sys/time.h>
#   include <sys/times.h>
#   include <sys/timex.h>
#ifndef __OPENWRT__
#   include <sys/vtimes.h>
#   include <sys/vlimit.h>
#endif
#   include <sys/resource.h>
#ifndef __BUSYBOX__
#   include <sys/sysinfo.h>
#endif
#   include <sys/utsname.h>
#   include <sys/mount.h>
#   include <sys/epoll.h>
#   include <sys/timerfd.h>
#   include <sys/eventfd.h>
#   include <sys/signalfd.h>
#   include <sys/inotify.h>
#   include <sys/utsname.h>
#   include <netpacket/packet.h>
#   include <netinet/ether.h>
#   include <netinet/in.h>
#   include <netinet/ip.h>
#   include <netinet/tcp.h>
#   include <netinet/udp.h>
#   include <arpa/inet.h>
#   include <net/if.h>
#   include <linux/netlink.h>
#endif /* !__APP__ */

#include "config.h"
#include "utils/this.h"
#include "product/product.h"
#include "utils/type.h"
#include "utils/errno.h"
#include "utils/lanmbda.h"
#include "utils/buildin.h"
#include "utils/enum.h"
#include "utils/assert.h"
#include "utils/memory.h"
#include "utils/bits.h"
#include "utils/format.h"
#include "utils/string.h"
#include "utils/string2.h"
#include "utils/math.h"
#include "utils/list.h"
#include "utils/dlist.h"
#include "utils/hash.h"
#include "utils/db.h"
#include "utils/symbol.h"
#include "utils/file.h"
#include "utils/time.h"
#include "utils/env.h"
#include "utils/sem.h"
#include "utils/shm.h"
#include "utils/ak.h"
#include "utils/addr.h"
#include "utils/japi.h"
#include "utils/jlog.h"

/* here, support debug_xxx */
#include "utils/debug.h"
#include "utils/dump.h"
#include "utils/crc32.h"
#include "utils/autoarray.h"
#include "utils/sbuffer.h"
#include "utils/pipe.h"
#include "utils/pid.h"
#include "utils/if.h"
#include "utils/io.h"
#include "utils/kv.h"
#include "utils/command.h"
#include "utils/cli.h"
#include "utils/cmd.h"
#include "utils/timer.h"
#include "utils/slice.h"
#include "utils/blob.h"
#include "utils/loop.h"
#include "utils/dll.h"
#include "utils/md5.h"
#include "utils/base64.h"

#if 1   /* __EXTEND__ */
#include "utils/channel.h"
#include "utils/cqueue.h"
#include "utils/coroutine.h"
#include "utils/fd.h"
#endif  /* __EXTEND__ */


#include "oem/oem.h"
/******************************************************************************/
#ifdef __ALLINONE__
#define LIB_INITER  os_fake_declare
#else
#define LIB_INITER              \
    static os_constructor void  \
    __lib_constructor(void)     \
    {                           \
        lib_init();             \
    }                           \
                                \
    os_fake_declare             \
    /* end */
#endif

static inline int
lib_init(void)
{
    int err;

#ifdef __APP__
    srand(time(NULL));
    
    os_env_init();
#endif

    err = ak_init();
    if (err<0) {
        os_println(__THIS_APPNAME " ak init error:%d", err);
    }
    
    err = jlog_init();
    if (err<0) {
        os_println(__THIS_APPNAME " jlog init error:%d", err);
    }
    
    return 0;
}

static inline int
lib_fini(void)
{
    jlog_fini();
    ak_fini();

    return 0;
}

static inline int
os_init(void)
{
    int err;

    err = lib_init();
    if (err<0) {
        return err;
    }

#ifdef __APP__
    err = os_deamon_check();
    if (err<0) {
        return err;
    }
#endif

    return 0;
}

static inline int
os_fini(void)
{
#ifdef __APP__
    os_deamon_exit();
#endif

    lib_fini();
    
    return 0;
}

static inline int
os_main(int (*func)(int, char **), int argc, char *argv[])
{
    int err = os_call(os_init, os_fini, func, argc, argv);

    return shell_error(err);
}

typedef struct {
    char *name;
    int (*init)(void);
} os_initer_t;

#define INIT_ENTRY(_name, _init)    { .name = _name, .init = _init }

static inline int
os_initer(os_initer_t map[], int count)
{
    int i, err;

    for (i=0; i<count; i++) {
        err = (*map[i].init)();
        if (err<0) {
            debug_error("init %s error:%d", map[i].name, err);

            return err;
        }
    }

    debug_ok("init ok");

    return 0;
}

#if 0
Addr.h (lib\utils):    static byte mac[OS_MACSIZE] = {0};
Addr.h (lib\utils):    static char macstring[1+MACSTRINGLEN_L] = {0};
Addr.h (lib\utils):    static char line[1+OS_LINE_LEN];
Blob.h (lib\utils):    static blob_rule_t rule[BLOB_T_END] = {
Cmd.h (lib\utils):    static char line[1+OS_LINE_LEN];
Cmd.h (lib\utils):    static cmd_table_t cmd;
Coroutine.h (lib\utils):    static char *reserved_name[] = {
Crc32.h (lib\utils):    static uint32 table[256] = {
Enum.h (lib\utils):        static char *array[_end] = { _mapper(__ENUM_MAP_NAME) }; \
Fcookie.h (lib\fcookie):    static char tmp_file[1+FCOOKIE_FILE_LEN];
Fcookie.h (lib\fcookie):    static char *domains[] = {
Gpio.h (lib\hisi):    static hisi_gpio_t array[] = GPIO_INITER;
Haenv.h (lib\haenv):    static haenv_deft_t deft[] = HAENV_DEFAULTS;
Js.h (lib\js):    static js_register_f *registers[] = {
Loop.h (lib\utils):    static void (*map[LOOP_TYPE_END])(loop_watcher_t *watcher) = {
Md5.h (lib\utils):    static byte padding[64] = {
Nsq.h (lib\nsq):    static jrule_t rule[NSQ_IDENTIFY_END] = NSQ_IDENTIFY_RULE_INITER;
Oem.h (lib\oem):    static int type = OEM_T_END;
Oem.h (lib\oem):    static char *vendor;
Otp.h (lib\hisi):    static char string[2*OTP_SIZE+1];
Otp.h (lib\hisi):    static int otp_offset = 0;
Time.h (lib\utils):    static char current[1+FULLTIME_STRING_LEN];
Time.h (lib\utils):    static char current[1+FULLTIME_STRING_LEN];
Time.h (lib\utils):    static char current[1+FULLTIME_STRING_LEN];
#endif
/******************************************************************************/
#endif /* __UTILS_H_6bb0dde2b3424f97a297b2b0e37d90aa__ */
