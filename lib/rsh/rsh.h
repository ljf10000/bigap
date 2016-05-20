#ifndef __RSH_H_cd4ac08f199c4732a4decf3ae976b791__
#define __RSH_H_cd4ac08f199c4732a4decf3ae976b791__
/******************************************************************************/
#include "utils.h"
/******************************************************************************/
#ifndef RSH_VERSION
#define RSH_VERSION             1
#endif

#ifndef RSH_TICKS
#define RSH_TICKS               1000 /* ms */
#endif

#ifndef ENV_RSH_CONFIG
#define ENV_RSH_CONFIG          "__RSH_CONFIG__"
#endif

#ifndef RSH_SLOT_COUNT
#define RSH_SLOT_COUNT          2
#endif

#ifndef RSH_SLOT_MASTER
#define RSH_SLOT_MASTER         1
#endif

#ifndef RSH_PORT
#define RSH_PORT                8740
#endif

#ifndef RSH_CONFIG_FILE
#define RSH_CONFIG_FILE         "/tmp/.rsh.config"
#endif

#ifndef RSH_RESOLV_FILE
#define RSH_RESOLV_FILE         "/tmp/.rsh.resolv"
#endif

#ifndef RSHD_UNIX
#define RSHD_UNIX               "/tmp/.rsh.unix"
#endif

#ifndef RSHC_UNIX
#define RSHC_UNIX               "/tmp/.rsh.%d.unix"
#endif

#ifndef RSH_SLOT_FILE
#define RSH_SLOT_FILE           "/tmp/.rsh.%d.slot"
#endif

#ifndef RSH_SLOT_SCRIPT
#define RSH_SLOT_SCRIPT         "/usr/sbin/.rsh.slot.script"
#endif

#define RSH_SLOT_CLOUD          0
#define RSH_SLOT_LOCAL          1
#define RSH_SLOT_MULTI          (RSH_SLOT_COUNT>1)
#define RSH_SLOT_END            (1+RSH_SLOT_COUNT)
#define rsh_foreach_slot(_slot) for(_slot=0; _slot<RSH_SLOT_END; _slot++)

static inline bool
is_good_rsh_local_slot(int slot)
{
    return IS_GOOD_VALUE(slot, RSH_SLOT_LOCAL, RSH_SLOT_END);
}

static inline bool
is_good_rsh_slot(int slot)
{
    return IS_GOOD_ENUM(slot, RSH_SLOT_END);
}

#ifndef RSH_TASK_COUNT
#define RSH_TASK_COUNT          128
#endif

#ifndef RSH_RUNTIME
#define RSH_RUNTIME             300 /* second */
#endif

#ifndef RSH_DELAY_MIN
#define RSH_DELAY_MIN           (500*1000)  /* us */
#endif

#ifndef RSH_DELAY_MAX
#define RSH_DELAY_MAX           (100*1000)  /* us */
#endif

#ifndef RSH_TASK_INTERVAL
#define RSH_TASK_INTERVAL       (10*1000/RSH_TICKS)
#endif

#ifndef RSH_LINK_INTERVAL
#define RSH_LINK_INTERVAL       (300*1000/RSH_TICKS)
#endif

#ifndef RSH_ECHO_LOCAL_INTERVAL
#define RSH_ECHO_LOCAL_INTERVAL (3*1000/RSH_TICKS)
#endif

#ifndef RSH_ECHO_LOCAL_TIMES
#define RSH_ECHO_LOCAL_TIMES    3
#endif

#ifndef RSH_ECHO_CLOUD_INTERVAL
#define RSH_ECHO_CLOUD_INTERVAL (10*1000/RSH_TICKS)
#endif

#ifndef RSH_ECHO_CLOUD_TIMES
#define RSH_ECHO_CLOUD_TIMES    3
#endif

#define RSH_ECHO_TIMES(_slot)       (RSH_SLOT_CLOUD==(_slot)?RSH_ECHO_CLOUD_TIMES:RSH_ECHO_LOCAL_TIMES)
#define RSH_ECHO_INTERVAL(_slot)    (RSH_SLOT_CLOUD==(_slot)?RSH_ECHO_CLOUD_INTERVAL:RSH_ECHO_LOCAL_INTERVAL)
/******************************************************************************/
/*
cmd:command/echo
mode:asyn/syn/ack/reflect
url:
    http(s)://domain:port/service
    tcp://domain:port

request(echo):  agent==>cloud
response(echo): agent<==cloud
{
    "version": VERSION,
    "mac":"MAC",
    "cmd":"echo",
}

request(shell, asyn): 
    agent<==cloud
    rshc==>xinetd
{
    "version": VERSION,
    "mac":"MAC",
    "slot":SLOT,
    "cmd":"command",
    "mode":"asyn",
    "seq":SEQ,
    "stdin":"STDIN"
}

request(shell, ack): 
    agent<==cloud
    rshc==>xinetd
{
    "version": VERSION,
    "mac":"MAC",
    "slot":SLOT,
    "cmd":"command",
    "mode":"asyn",
    "seq":SEQ,
    "stdin":"STDIN"
}

response(shell, ack): 
    agent==>cloud
    rshc<==xinetd
{
    "version": VERSION,
    "mac":"MAC",
    "slot":SLOT,
    "cmd":"command",
    "mode":"ack",
    "seq":SEQ,
    "stdin":"STDIN"
}

request(shell, reflect): agent<==cloud
{
    "version": VERSION,
    "mac":"MAC",
    "slot":SLOT,
    "cmd":"command",
    "mode":"syn",
    "seq":SEQ,
    "stdin":"STDIN",
    "redirect": {
        "url":"URL"
    }
}

response(shell, reflect): xinetd==>cloud
{
    "version": VERSION,
    "mac":"MAC",
    "slot":SLOT,
    "cmd":"command",
    "mode":"syn",
    "seq":SEQ,
    "stdin":"STDIN",
    "redirect": {
        "url":"URL"
    },
    
    "stdout":"STDOUT",
    "stderr":"STDERR",
    "errno":ERRNO
}

request(shell, syn): rshc==>xinetd
{
    "version": VERSION,
    "mac":"MAC",
    "slot":SLOT,
    "cmd":"command",
    "mode":"syn",
    "seq":SEQ,
    "stdin":"STDIN",
}

response(shell, syn): rshc<==xinetd
{
    "version": VERSION,
    "mac":"MAC",
    "slot":SLOT,
    "cmd":"command",
    "mode":"syn",
    "seq":SEQ,
    "stdin":"STDIN",

    "stdout":"STDOUT",
    "stderr":"STDERR",
    "errno":ERRNO
}

echo: cloud==(echo-request)==>agent==(echo-response)==>cloud
asyn: cloud==(asyn-request)==>agent==(asyn-request)==>xinetd
                                |
                                |
                                ==(asyn-request)==>fork
reflect:cloud==(reflect-request)==>agent==(reflect-request)==>xinetd==(reflect-response)==>cloud
                                |
                                |
                                ==(reflect-request)==>fork==(reflect-response)==>cloud
syn: rshc==(syn-request)==>xinetd==(syn-response)==>rshc
*/

#define __XLIST_RSH_CMD(_)              \
    _(RSH_CMD_COMMAND,  0, "command"),  \
    _(RSH_CMD_ECHO,     1, "echo"),     \
    /* end */

static inline bool is_good_rsh_cmd(int cmd);
static inline char *rsh_cmd_string(int cmd);
static inline int rsh_cmd_idx(char *cmd_string);
DECLARE_ENUM(rsh_cmd, __XLIST_RSH_CMD, RSH_CMD_END);

#if 1 /* just for sourceinsight */
#define RSH_CMD_COMMAND     RSH_CMD_COMMAND
#define RSH_CMD_ECHO        RSH_CMD_ECHO
#define RSH_CMD_END         RSH_CMD_END
#endif /* just for sourceinsight */

#define __XLIST_RSH_CMD(_)          \
    _(RSH_MODE_SYN,     0, "syn"),  \
    _(RSH_MODE_ACK,     1, "ack"),  \
    _(RSH_MODE_ASYN,    2, "asyn"), \
    /* end */

static inline bool is_good_rsh_mode(int mode);
static inline char *rsh_mode_string(int mode);
static inline int rsh_mode_idx(char *mode_string);
DECLARE_ENUM(rsh_mode, __XLIST_RSH_CMD, RSH_MODE_END);

#if 1 /* just for sourceinsight */
#define RSH_MODE_SYN    RSH_MODE_SYN
#define RSH_MODE_ACK    RSH_MODE_ACK
#define RSH_MODE_ASYN   RSH_MODE_ASYN
#define RSH_MODE_END    RSH_MODE_END
#endif /* just for sourceinsight */

/******************************************************************************/
typedef struct {
    uint32 ip;      /* network sort */
    int port;
    
    int interval;   /* seconds */
    int times;
} rsh_slot_t;

#define RSH_SLOT_INITER             {   \
    .port       = RSH_PORT,             \
    .interval   = RSH_ECHO_INTERVAL(0), \
    .times      = RSH_ECHO_TIMES(0),    \
}

typedef struct {
    char *config;       /* config file */
    char *basemac;
    
    int current;        /* current slot */
    
    rsh_slot_t slot[RSH_SLOT_END];
} rsh_config_t;

#define RSH_CONFIG_INITER {     \
    .config = RSH_CONFIG_FILE,  \
    .slot   = {                 \
        [0 ... RSH_SLOT_END] = RSH_SLOT_INITER, \
    },                          \
}

extern rsh_config_t rsh_cfg;
#define rsh_slot(_slot)                 (&rsh_cfg.slot[_slot])
#define rsh_slot_ip(_slot)              rsh_slot(_slot)->ip
#define rsh_slot_port(_slot)            rsh_slot(_slot)->port
#define rsh_slot_echo_times(_slot)      rsh_slot(_slot)->times
#define rsh_slot_echo_interval(_slot)   rsh_slot(_slot)->interval
#define rsh_slot_addr(_slot)            OS_SOCKADDR_INET(rsh_slot_ip(_slot), rsh_slot_port(_slot))

#define rsh_current                 rsh_cfg.current
#define rsh_current_ip              rsh_slot_ip(rsh_current)
#define rsh_current_port            rsh_slot_port(rsh_current)
#define rsh_current_addr            rsh_slot_addr(rsh_current)

#define rsh_cloud_ip                rsh_slot_ip(RSH_SLOT_CLOUD)
#define rsh_cloud_port              rsh_slot_port(RSH_SLOT_CLOUD)
#define rsh_cloud_addr              rsh_slot_addr(RSH_SLOT_CLOUD)

/*
* all   := cloud + local
* local := current + other
* local := master  + slave
*/
#define rsh_is_cloud(_slot)         (RSH_SLOT_CLOUD==(_slot))
#define rsh_is_local(_slot)         (false==rsh_is_cloud(_slot))
#define rsh_is_current(_slot)       ((_slot)==rsh_current)
#define rsh_is_other(_slot)         (rsh_is_local(_slot) && false==rsh_is_current(_slot))
#define rsh_is_master(_slot)        (RSH_SLOT_MASTER==(_slot))
#define rsh_is_slave(_slot)         (false==rsh_is_master(_slot))

static inline void
__rsh_slot_state(int slot, bool alive)
{
    rshd_slot_alive(slot) = alive;

    os_v_fseti(alive, RSH_SLOT_FILE, slot);
    os_system(RSH_SLOT_SCRIPT " %d %d &", slot, alive);
}

static inline void
rsh_slot_state(int slot, bool alive)
{
    alive = !!alive;

    if (false==alive && rshd_slot_alive(slot)) {
        rshd_slot_disconnect(slot)++;

        __rsh_slot_state(slot, alive);
    }
    else if (alive && false==rshd_slot_alive(slot)) {
        rshd_slot_connect(slot)++;

        __rsh_slot_state(slot, alive);
    }
}

static inline void
rsh_slot_dump(int slot)
{
    debug_config("slot[%d]"                     __space
        "config:"                               __space
            "ip[%s] port[%d]"                   __space
            "times[%d] interval[%d]"            __space
        "echo:"                                 __space
            "times[%d] interval[%d] alive[%d]"  __space
            "connect[%llu] disconnect[%llu]"    __space
            "send-ok[%llu] send-fail[%llu]"     __space
            "recv-ok[%llu] recv-fail[%llu]",
        slot,
        os_ipstring(rsh_slot_ip(slot)), ntohs(rsh_slot_port(slot)),
        rsh_slot_echo_times(slot), rsh_slot_echo_interval(slot),
        rshd_echo_times(slot), rshd_echo_interval(slot), rshd_slot_alive(slot),
        rshd_echo_send(slot, 0), rshd_echo_send(slot, 1),
        rshd_echo_recv(slot, 0), rshd_echo_recv(slot, 1));
}
/******************************************************************************/
static inline int
rsh_connect(int fd, int slot)
{
    sockaddr_in_t server = OS_SOCKADDR_INET(rsh_slot_ip(slot), rsh_slot_port(slot));
    
    return connect(fd, &server, sizeof(server));
}

static inline int
rsh_disconnect(int fd)
{
    sockaddr_t unspec = OS_SOCKADDR_UNSPEC();

    return connect(fd, &unspec, sizeof(unspec));
}

static inline int
rsh_recvfrom(int fd, rsh_single_msg_t *single, sockaddr_in_t *from, int *addrlen)
{
    int msglen = recvfrom(fd, single, sizeof(*single), 0, from, addrlen);
    if (msglen<0) {
        return -errno;
    }
    else if (msglen==sizeof(rsh_single)) {
        return -ETOOBIG;
    }

    rsh_msg_t *msg = &single->msg;
    int err = rsh_msg_ntoh(msg);
    if (err<0) {
        return err;
    }
    else if (msglen!=rsh_msg_size(msg)) {
        return -ENOMATCH;
    }
    
    return msglen;
}

static inline int
rsh_m_sendto(int fd, rsh_msg_t *msg, int slot)
{
    int size = rsh_msg_size(msg);
    struct iovec iov[] = {
        OS_IOVEC_INITER(msg, sizeof(*msg)),
        OS_IOVEC_INITER(msg->body.buffer, msg->len),
    };
    
    rsh_msg_hton(msg);
    rsh_connect(fd, slot);
    int len = io_writev(fd, iov, os_count_of(iov));
    rsh_disconnect(fd);

    if (len<0) {
        return len;
    }
    else if (len==size) {
        /*
        * send ok, clean cache
        */
    }
    else if (len>0) {
        /*
        * send some data, reconnect
        */
    }
    
    return len;
}

static inline int
rsh_s_sendto(int fd, rsh_msg_t *msg, int slot)
{
    sockaddr_in_t server = rsh_slot_addr(slot);
    int size = rsh_msg_size(msg);

    int len = io_sendto(fd, msg, size, &server, sizeof(server));
    if (len<0) {
        return len;
    }
    else if (len==size) {
        /*
        * send ok, clean cache
        */
    }
    else if (len>0) {
        /*
        * send some data, reconnect
        */
    }
    
    return len;
}
/******************************************************************************/
/*
{
    "current": CURRENT,
    
    "slot": [
        {
            "ip": "IP",
            "port": PORT,
            "echo": {
                "interval": INTERVAL,
                "times": TIMES,
            },
        },
        ...
        {
            "ip": "IP",
            "port": PORT,
            "echo": {
                "interval": INTERVAL,
                "times": TIMES,
            },
        }
    ],
}
*/
static inline int
rsh_load(void)
{
    int slot, err = -EFORMAT, val;
    jobj_t obj, jcfg = NULL;
    char basemac[1+OS_LINE_LEN] = {0};
    
    /*
    * maybe not found basemac
    */
    os_pgets(basemac, OS_LINE_LEN, SCRIPT_PRODUCT_BASEMAC);
    rsh_cfg.basemac = os_strdup(basemac);
    
    rsh_cfg.config = env_gets(ENV_RSH_CONFIG, RSH_CONFIG_FILE);
    jcfg = jobj_file(rsh_cfg.config);
    if (NULL==jcfg) goto error;

#if RSH_SLOT_MULTI
    jobj_t jcurrent = jobj_get(jcfg, "current");
    if (NULL==jcurrent) goto error;
    rsh_current = jobj_get_i32(jcurrent);
    if (false==is_good_rsh_local_slot(rsh_current)) goto error;
#else
    rsh_current = RSH_SLOT_LOCAL;
#endif

    jobj_t jslot = jobj_get(jcfg, "slot");
    if (RSH_SLOT_COUNT!=jarray_length(jslot)) goto error;
    
    for (slot=0; slot<RSH_SLOT_END; slot++) {
        obj = jarray_get(jslot, slot);
        if (NULL==obj) goto error;

        jobj_t jip = jobj_get(obj, "ip");
        if (NULL==jip) goto error;
        char *ipstring = jobj_get_string(obj);
        uint32 ip = inet_addr(ipstring);
        if (rsh_is_local(slot) && INADDR_NONE==ip) goto error;
        rsh_slot_ip(slot) = htonl(ip);

        jobj_t jport = jobj_get(obj, "port");
        if (NULL==jport) goto error;
        uint16 port = jobj_get_i32(jport);
        rsh_slot_port(slot) = htons(port);

        jobj_t jecho = jobj_get(obj, "echo");
        if (NULL==jecho) goto error;
        
        jobj_t jtimes = jobj_get(jecho, "times");
        if (NULL==jtimes) goto error;
        rsh_slot_echo_times(slot) = jobj_get_i32(jtimes);
        
        jobj_t jinterval = jobj_get(jecho, "interval");
        if (NULL==jinterval) goto error;
        rsh_slot_echo_interval(slot) = jobj_get_i32(jinterval);
        
        rsh_slot_dump(slot);
    }

    err = 0;
error:
    jobj_put(jcfg);

    return err;
}
/******************************************************************************/
#endif /* __RSH_H_cd4ac08f199c4732a4decf3ae976b791__ */
