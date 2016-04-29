/*******************************************************************************
Copyright (c) 2015-2016, xxx Networks. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      rshd
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     rshd
#endif

#include "utils.h"
#include "rshd.h"

OS_INITER;
*******************************************************************************/
rsh_single_msg_t rsh_single;
rsh_config_t rsh_cfg = RSH_CONFIG_INITER;

rsh_t rsh = {
    .head   = {
        .list = LIST_HEAD_INIT(rsh.head.list),
    },

    .fd     = {INVALID_FD},
    
    .timer  = {
        [RSH_TIMER_APING]  = RSH_TIMER_INITER(RSH_APING_INTERVAL,    rsh_pinger),
        [RSH_TIMER_IPING]  = RSH_TIMER_INITER(RSH_IPING_INTERVAL,    rsh_pinger),
        [RSH_TIMER_LINK]   = RSH_TIMER_INITER(RSH_LINK_INTERVAL,     rsh_linker),
        [RSH_TIMER_TASK]   = RSH_TIMER_INITER(RSH_TASK_INTERVAL,     rsh_tasker),
        [RSH_TIMER_CONNECT]= RSH_TIMER_INITER(RSH_CONNECT_INTERVAL,  rsh_connecter),
    },
};

static int rsh_sigs[] = {SIGCHLD, os_signal_user_list, os_signal_exit_list, os_signal_callstack_list};

static int
rsh_signal(struct signalfd_siginfo *siginfo)
{
    int signo = siginfo->ssi_signo;
    
    switch(signo) {
        case SIGCHLD:
            rsh_task_wait(siginfo);
            break;
        case SIGUSR1: /* wan up */
            // todo:
            break;
        case SIGUSR2: /* wan down */
            // todo:
            break;
        case SIGINT:
        case SIGHUP:
        case SIGTERM:
        case SIGABRT:
            __sighandle_exit(signo);
            break;
        case SIGBUS:
        case SIGILL:
        case SIGFPE:
        case SIGSEGV:
            __sighandle_callstack(signo);
            break;
        default:
            os_do_nothing();
            break;
    }

    return 0;
}

static int
rsh_fd_handle(void)
{
    sockaddr_in_t from;
    int err, msglen, addrlen = sizeof(from);
    
    msglen = rsh_recvfrom(rsh.fd, &rsh_single, &from, &addrlen);
    if (msglen<0) {
        return msglen;
    }

    rsh_msg_t *msg = &rsh_single.msg;
    if (false==__is_good_rsh_msg(msg)) {
        return -EPROTOCOL;
    }
    
    int src = msg->src;
    int dst = msg->dst;
    /*
    * server
    *   other==>current
    *   cloud==>current
    */
    if (rsh_is_current(dst)) {
        if (is_rsh_ack(msg)) {
            switch(msg->cmd) {
                case RSH_CMD_ECHO:
                    rsh_slot_state(src, true);
                    return 0;
                default:
                    /* log */
                    return -ENOSUPPORT;
            }
        } else {
            switch(msg->mode) {
                case RSH_MODE_SYN:
                    return rsh_child(msg, &from);
                case RSH_MODE_ACK:
                    err = rsh_ack(msg, &from);
                    rsh_asyn(msg);
                    return err;
                case RSH_MODE_ASYN:
                    return rsh_asyn(msg);
            }
        }
    }
    /*
    * forward
    *   other==>cloud
    *   cloud==>other
    */
    else if ((rsh_is_other(src) && rsh_is_cloud(dst)) ||
             (rsh_is_cloud(src) && rsh_is_other(dst))) {
        return rsh_forward(msg, msglen, dst);      
    }
    /*
    * not support, here
    *   current==>cloud
    *   current==>other
    */
    else {
        // log
    }

    return 0;
}

static int 
rsh_handle(struct epoll_event *ev)
{
    int fd = ev->data.fd;

    if (fd==rsh.fd) {
        return rsh_fd_handle();
    }
    /*
    * pipe fd
    */
    else {
        return rsh_pipe_handle(fd);
    }
}

static int
rsh_echo_timer(void)
{
    int slot;

    rsh_foreach_slot(slot) {
        if (rsh_is_current(slot)) {
            continue;
        }

        rshd_echo_interval(slot)++;
        if (rshd_echo_interval(slot) >= rsh_slot_echo_interval(slot)) {
            rshd_echo_interval(slot) = 0;
            rsh_echo(slot);
            
            rshd_echo_times(slot)++;
            if (rshd_echo_times(slot) >= rsh_slot_echo_times(slot)) {
                rsh_slot_state(slot, false);
            }
        }
    }
}

static int
rsh_timer(uint32 times)
{
    int i;

    for (i=0; i<times; i++) {
        rsh_echo_timer();
    }
    
    return tm_trigger(times);
}

loop_t rsh_loop = LOOP_INITER(
    rsh_sigs,               os_count_of(rsh_sigs),  rsh_signal,
    os_second(RSH_TICKS),   os_nsecond(RSH_TICKS),  rsh_timer,
    rsh.fd,                 os_count_of(rsh.fd),    rsh_handle
);

static int
rsh_init(void)
{
    sockaddr_in_t server = OS_SOCKADDR_INET(rsh_current_ip, rsh_current_port);
    int fd, err;

    fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (fd<0) {
    	debug_error("socket error:%d", -errno);
        return -errno;
    }
    os_closexec(fd);
    
    err = bind(fd, (sockaddr_t *)&server, sizeof(server));
    if (err<0) {
        debug_error("bind error:%d", -errno);
        return -errno;
    }

    rsh.fd = fd;
    
    return 0;
}

static int
__main(int argc, char *argv[])
{
    int err = 0;
    
    err = rsh_load();
    if (err<0) {
        return err;
    }
    
    err = rsh_init();
    if (err<0) {
        return err;
    }
    
    err = rsh_timer_init();
    if (err<0) {
        return err;
    }

    err = os_loop_init(&rsh_loop);
    if (err<0) {
        return err;
    }
    
    return os_loop(&rsh_loop);
}

#ifndef __BUSYBOX__
#define rshd_main  main
#endif

int rshd_main(int argc, char **argv)
{
    return os_main(__main, argc, argv);
}
/******************************************************************************/
