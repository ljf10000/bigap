#ifndef __UTILS_H_6bb0dde2b3424f97a297b2b0e37d90aa__
#define __UTILS_H_6bb0dde2b3424f97a297b2b0e37d90aa__
/******************************************************************************/
#ifdef __FASTBOOT__
#define __BOOT__
#endif

#ifdef __BOOT__
#   include <malloc.h>
#   include <command.h>
#   include <common.h>
#   include <linux/ctype.h>
#   include <linux/string.h>
#   ifndef __THIS_DEBUG
#       define __THIS_DEBUG    __boot_debuger
#   endif
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
#   include <fstab.h>
#   include <mntent.h>
#   include <limits.h>
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
#   include <sys/vtimes.h>
#   include <sys/resource.h>
#   include <sys/vlimit.h>
#   include <sys/sysinfo.h>
#   include <sys/utsname.h>
#   include <sys/mount.h>
#   include <sys/epoll.h>
#   include <sys/timerfd.h>
#   include <sys/eventfd.h>
#   include <sys/signalfd.h>
#   include <sys/inotify.h>
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
#include "utils/type.h"
#include "utils/errno.h"
#include "utils/buildin.h"
#include "utils/enum.h"
#include "utils/bits.h"
#include "utils/assert.h"
#include "utils/memory.h"
#include "utils/string.h"
#include "utils/string2.h"
#include "utils/math.h"
#include "utils/list.h"
#include "utils/dlist.h"
#include "utils/hash.h"
#include "utils/db.h"
#include "utils/symbol.h"
#include "utils/format.h"
#include "utils/file.h"
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
#include "utils/time.h"
#include "utils/crc32.h"
#include "utils/addr.h"
#include "utils/pid.h"
#include "utils/if.h"
#include "utils/io.h"
#include "utils/kv.h"
#include "utils/cmd.h"
#include "utils/slice.h"
#include "utils/blob.h"
#include "utils/timer.h"
#include "utils/autoarray.h"
#include "utils/channel.h"
#include "utils/cqueue.h"
#include "utils/coroutine.h"
#include "utils/fd.h"
#include "utils/loop.h"

#include "oem/oem.h"
/******************************************************************************/
#define OS_INITER       \
    DECLARE_DEAMON;     \
    DECLARE_FLOCK;      \
    DECLARE_JLOG;       \
    DECLARE_OEM;        \
    DECLARE_AK;         \
    DECLARE_DEBUGGER /* last */ \
    /* end */

static inline int
os_init(void)
{
    int err;

    srand(time(NULL));

    err = ak_init();
    if (err<0) {
        return err;
    }
    
    err = jlog_init();
    if (err<0) {
        return err;
    }

    err = os_deamon_check();
    if (err<0) {
        return err;
    }
    
    return 0;
}

static inline int
os_fini(void)
{
    os_deamon_exit();
    jlog_fini();
    ak_fini();

    return 0;
}

static inline int
os_main(int (*func)(int, char **), int argc, char **argv)
{
    return os_call(os_init, os_fini, func, argc, argv);
}
/******************************************************************************/
#endif /* __UTILS_H_6bb0dde2b3424f97a297b2b0e37d90aa__ */
