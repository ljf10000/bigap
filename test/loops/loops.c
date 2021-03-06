/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      loops
#endif

#define __DEAMON__
#include "utils.h"

OS_INITER;

struct server {
    int fd;
    int type;
    os_sockaddr_t addr;
};

static struct {
    struct server tcp, udp, timer;

    char buf[1024];
} loops = {
    .tcp = {
        .type = SOCK_STREAM,
        .addr = OS_SOCKADDR_ABSTRACT_INITER("loops.tcp"),
    },
    .udp = {
        .type = SOCK_DGRAM,
        .addr = OS_SOCKADDR_ABSTRACT_INITER("loops.udp"),
    },
};

static loop_t loop = LOOP_INITER;

static int timer_cb(loop_watcher_t *watcher, time_t now)
{
    static uint32 count = 0;

    os_println("timer:%u time:%u", count++, (uint32)now);
    
    return 0;
}

static int signal_cb(loop_watcher_t *watcher, struct signalfd_siginfo *p, time_t now)
{
    os_println( "signo=%d, "
                "errno=%d, "
                "code=%d, "
                "pid=%d, "
                "uid=%d, "
                "fd=%d, "
                "tid=%d, "
                "band=%d, "
                "overrun=%d, "
                "trapno=%d, "
                "status=%d, "
                "int=%d",
        p->ssi_signo,
        p->ssi_errno,
        p->ssi_code,
        p->ssi_pid,
        p->ssi_uid,
        p->ssi_fd,
        p->ssi_tid,
        p->ssi_band,
        p->ssi_overrun,
        p->ssi_trapno,
        p->ssi_status,
        p->ssi_int);

    return 0;
}

static int normal_cb(loop_watcher_t *watcher, time_t now)
{
    int err, len;
    os_sockaddr_t addr;
    sockaddr_len_t addrlen = sizeof(addr.un);
    
    len = __io_recvfrom(watcher->fd, loops.buf, sizeof(loops.buf), 0, (sockaddr_t *)&addr.c, &addrlen);
    if (len<0) {
        debug_error("normal recv error:%d", -errno);

        return -errno;
    }
    loops.buf[len] = 0;
    
    os_println("normal recv:%s", loops.buf);

    err = io_sendto(watcher->fd, loops.buf, len, (sockaddr_t *)&addr.c, addrlen);
    if (err<0) {
        debug_error("normal send error:%d", -errno);

        return -errno;
    }
    
    return 0;
}

static int son_cb(loop_watcher_t *watcher, time_t now)
{
    int err, len;

    len = __io_recv(watcher->fd, loops.buf, sizeof(loops.buf), 0);
    if (len<0) {
        debug_error("son recv error:%d", -errno);

        return -errno;
    }
    loops.buf[len] = 0;
    
    os_println("son recv:%s", loops.buf);

    err = io_send(watcher->fd, loops.buf, len);
    if (err<0) {
        debug_error("son send error:%d", -errno);

        return -errno;
    }

    return 0;
}

static int
usock(struct server *s)
{
    int fd, err;
    
    fd = socket(AF_UNIX, s->type, 0);
    if (fd<0) {
    	debug_error("socket error:%d", -errno);
        return -errno;
    }
    os_closexec(fd);
    
    err = bind(fd, &s->addr.c, get_abstract_sockaddr_len(&s->addr.un));
    if (err<0) {
        debug_error("bind error:%d", -errno);
        return -errno;
    }

    int size = 1+CLI_BUFFER_LEN;
    err = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
    if (err<0) {
        debug_error("setsockopt error:%d", -errno);
        return -errno;
    }

    s->fd = fd;
    
    return 0;
}

static int
tcp(void)
{
    int err;
    
    usock(&loops.tcp);

    err = listen(loops.tcp.fd, 0);
    if (err<0) {
        debug_error("listen error:%d", -errno);
        
        return err;
    }
    
    return 0;    
}

static int
udp(void)
{
    usock(&loops.udp);

    return 0;
}

STATIC int
loops_main_helper(int argc, char *argv[])
{
    struct itimerspec tm = OS_ITIMESPEC_INITER(5, 0);
    int sigs[] = {
        os_signal_user_list,
        os_signal_exit_list,
        os_signal_callstack_list
    };
    int err;
    
    err = udp();
    if (err<0) {
        return err;
    }
    
    err = tcp();
    if (err<0) {
        return err;
    }

    err = os_loop_add_signal(&loop, signal_cb, sigs, os_count_of(sigs));
    if (err<0) {
        debug_error("add loop signal error:%d", err);
        
        return err;
    }
    
    err = os_loop_add_timer(&loop, timer_cb, &tm);
    if (err<0) {
        debug_error("add loop timer error:%d", err);
        
        return err;
    }
    
    err = os_loop_add_normal(&loop, loops.udp.fd, normal_cb, NULL);
    if (err<0) {
        debug_error("add loop normal error:%d", err);
        
        return err;
    }

    err = os_loop_add_father(&loop, loops.tcp.fd, son_cb, true, NULL);
    if (err<0) {
        debug_error("add loop father error:%d", err);
        
        return err;
    }

    err = os_loop(&loop);
    if (err<0) {
        debug_error("loop run error:%d", err);
        
        return err;
    }
    
    return 0;
}

int main(int argc, char *argv[])
{
    os_println("main");
    int err = os_main(loops_main_helper, argc, argv);

    return shell_error(err);
}

/******************************************************************************/
