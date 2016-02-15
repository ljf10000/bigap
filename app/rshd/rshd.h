#ifndef __RSHD_H_2b0325d9311a40b6ab74422f0ccf9d36__
#define __RSHD_H_2b0325d9311a40b6ab74422f0ccf9d36__
/******************************************************************************/
#include "rsh/rsh.h"
/******************************************************************************/

enum {
    /*
    * active ping
    */
    TIMER_APING,
    
    /*
    * inactive ping
    */
    TIMER_IPING,
    
    /*
    * check (3g) link
    */
    TIMER_LINK,
    
    /*
    * check task over
    */
    TIMER_TASK,
    
    TIMER_CONNECT,

    TIMER_END
};

typedef struct {
    int interval;
    tm_callback_f *cb;
    tm_node_t node;
} rsh_timer_t;
#define RSHA_TIMER_INITER(_interval, _cb) { \
    .interval   = _interval,    \
    .cb         = _cb,          \
}   /* end */

typedef int rsh_handle_f(void);

typedef struct {
    int fd; /* udp */
    
    int ping;
    int timeout;
    
    uint32_t id;
    uint32_t taskid;

    rsh_timer_t timer[TIMER_END];

    struct {
        struct {
            int interval;   /* seconds */
            int times;
            
            uint64_t send[2];   /* 0: send ok, 1: send fail */
            uint64_t recv[2];   /* 0: send ok, 1: send fail */
        } echo;
        
        bool alive;
        uint64_t disconnect;
        uint64_t connect;
    } st[RSH_SLOT_END];
    
    struct clist_table table;
}
rsh_t;

extern rsh_t rsh;
#define rshd_echo_times(_slot)          rsh.st[_slot].times
#define rshd_echo_interval(_slot)       rsh.st[_slot].interval
#define rshd_echo_send(_slot, _state)   rsh.st[_slot].send[_state]
#define rshd_echo_recv(_slot, _state)   rsh.st[_slot].recv[_state]
#define rshd_slot_alive(_slot)          rsh.st[_slot].alive
#define rshd_slot_disconnect(_slot)     rsh.st[_slot].disconnect
#define rshd_slot_connect(_slot)        rsh.st[_slot].connect

extern loop_t rsh_loop;
extern rsh_single_msg_t rsh_single;
/******************************************************************************/
static inline int
rsh_echo(int slot)
{
    int err = 0;
    
    if (rsh_slot_ip(slot)) {
        rsh_msg_t msg = RSH_MSG_INITER(
                            rsh_current, 
                            slot, 
                            RSH_CMD_ECHO,
                            RSH_MODE_ACK, 
                            0, 
                            0,
                            rsh.id++);

        err = rsh_s_sendto(rsh.fd, &msg, slot);
        
        rshd_echo_send(slot, (err>0)?OS_OK:OS_FAIL)++;
    }

    return err;
}

static int
rsh_asyn(rsh_msg_t *msg)
{
    return os_system("%s &", msg->body.buffer);
}

static inline int
rsh_ack(rsh_msg_t *msg, sockaddr_in_t *client)
{
    rsh_msg_t ack = *msg;
    
    os_swap_value(ack.src, ack.dst);
    ack.flag |= RSH_F_ACK;
    ack.len = htonl(0);

    return io_sendto(rsh_agent_fd, msg, sizeof(*msg), client, sizeof(*client));
}

static inline int
rsh_forward(rsh_msg_t *msg, int msglen, int dst)
{
    sockaddr_in_t cloud = rsh_cloud_addr;
    
    int len = sendto(rsh.fd, msg, msglen, 0, &cloud, sizeof(cloud));
    if (len<0) {
        return -errno;
    }
    else if (len!=msglen) {
        // log
    }

    return 0;
}
/******************************************************************************/
extern mv_t rsh_pinger(tm_node_t *timer);
extern mv_t rsh_linker(tm_node_t *timer);
extern mv_t rsh_tasker(tm_node_t *timer);
extern mv_t rsh_connecter(tm_node_t *timer);
extern int rsh_timer_init(void);

extern int rsh_cli(void);
extern int rsh_child(rsh_msg_t *msg, sockaddr_in_t *client);
extern int rsh_pipe_handle(int fd);
extern int rsh_task_wait(struct signalfd_siginfo *siginfo);
/******************************************************************************/
#endif /* __RSHD_H_2b0325d9311a40b6ab74422f0ccf9d36__ */
