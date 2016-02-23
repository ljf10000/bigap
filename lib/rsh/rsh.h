#ifndef __RSH_H_cd4ac08f199c4732a4decf3ae976b791__
#define __RSH_H_cd4ac08f199c4732a4decf3ae976b791__
/******************************************************************************/
#include "utils.h"
/******************************************************************************/
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
    return is_good_enum(slot, RSH_SLOT_END);
}

#ifndef RSH_TASK_COUNT
#define RSH_TASK_COUNT          128
#endif

#ifndef RSH_RUNTIME
#define RSH_RUNTIME             300 /* second */
#endif

#ifndef RSH_TICKS
#define RSH_TICKS               1000 /* ms */
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
#ifndef RSH_COOKIE0
#define RSH_COOKIE0             32
#endif

#ifndef RSH_COOKIE1
#define RSH_COOKIE1             18
#endif

#ifndef RSH_COOKIE2
#define RSH_COOKIE2             1
#endif

#ifndef RSH_COOKIE3
#define RSH_COOKIE3             23
#endif

#ifndef RSH_VERSION
#define RSH_VERSION             1
#endif

#ifndef RSH_ALIGN
#define RSH_ALIGN               4
#endif

static inline int
rsh_align(int size)
{
    return OS_ALIGN(size, RSH_ALIGN);
}

/*
* cloud:
*   client
*   server(master/slave)
*
* dev:
*   cmd
*   agent(master/slave, current/target/source/other)
*/
enum {
    /*
    * client==>server(master)==>server(slave)==>agent(master)==>[agent(slave)==>]shell
    * cmd==>agent(current)==>agent(target)==>shell
    */
    RSH_CMD_SHELL       = 0,

    /*
    * agent(master)==>server(slave)
    * agent(current)==>agent(other)
    *
    * zero body
    */
    RSH_CMD_ECHO        = 1,
    RSH_CMD_ACTIVE      = 2,
    
    RSH_CMD_END
};

static inline bool
is_good_rsh_cmd(int cmd)
{
    return is_good_enum(cmd, RSH_CMD_END);
}

static inline bool
is_rsh_zero_cmd(int cmd)
{
    return RSH_CMD_ECHO==cmd;
}

enum {
    RSH_MODE_SYN,
    RSH_MODE_ACK,
    RSH_MODE_ASYN,

    RSH_MODE_END
};

static inline bool
is_rsh_good_mode(int mode)
{
    return is_good_enum(mode, RSH_MODE_END);
}

enum {
    RSH_F_ACK   = 0x01,
    
    RSH_F_XXX
};

typedef struct {
    byte     mac[6];
    uint8_t  src;       /* src slot     */
    uint8_t  dst;       /* dst slot     */
    
    uint8_t  cookie0;   /* RSH_COOKIE0  */
    uint8_t  mode;      /* RSH_F_XXX    */
    uint8_t  cookie1;   /* RSH_COOKIE1  */
    uint8_t  flag;      /* RSH_F_XXX    */
    
    uint8_t  version;   /* RSH_VERSION  */
    uint8_t  cookie2;   /* RSH_COOKIE2  */
    uint8_t  cmd;       /* RSH_CMD_END  */
    uint8_t  cookie3;   /* RSH_COOKIE3  */
    
    uint16_t len;       /* msg length, NOT include header, NOT include '\0' */
    union {
        int16_t maxrun;
        int16_t error;
    } u;

    uint32_t id;        /* msg id       */

    union {
        char data[0];
        char *buffer[0];
    } body;
} 
rsh_msg_t;

#define RSH_MSG_FLAG(_flag)     (((rand() & 0xff) & ~(_flag)) | (_flag))
#define RSH_MSG_INITER(_src, _dst, _cmd, _mode, _flag, _maxrun, _id)  { \
    .src        = _src,         \
    .dst        = _dst,         \
    .cookie0    = RSH_COOKIE0,  \
    .cookie1    = RSH_COOKIE1,  \
    .cookie2    = RSH_COOKIE2,  \
    .cookie3    = RSH_COOKIE3,  \
    .mode       = _mode,        \
    .flag       = (_flag)?RSH_MSG_FLAG(_flag):0, \
    .cmd        = _cmd,         \
    .id         = _id,          \
    .u          = {             \
        .maxrun = _maxrun,      \
    },                          \
}

#ifndef RSH_MSG_SIZE
#define RSH_MSG_SIZE            (64*1024 - 14/*eth*/ - 4/*vlan*/ - 20/*ip*/ - 16/*udp*/ - 8/*pad*/)
#endif

#define RSH_MSG_DSIZE           (RSH_MSG_SIZE - sizeof(rsh_msg_t))

typedef union {
    byte buf[RSH_MSG_SIZE];
    
    rsh_msg_t msg;
} rsh_single_msg_t;

typedef struct {
    rsh_msg_t msg;
    
    char *buffer;
} rsh_multi_msg_t;

static inline void rsh_cookie_set(rsh_msg_t *msg)
{
    msg->cookie0 = RSH_COOKIE0;
    msg->cookie1 = RSH_COOKIE1;
    msg->cookie2 = RSH_COOKIE2;
    msg->cookie3 = RSH_COOKIE3;
}

static inline void is_good_rsh_cookie(rsh_msg_t *msg)
{
    return RSH_COOKIE0==msg->cookie0
        && RSH_COOKIE1==msg->cookie1
        && RSH_COOKIE2==msg->cookie2
        && RSH_COOKIE3==msg->cookie3;
}

/*
* network/host sort
*/
static inline bool
__is_good_rsh_msg(rsh_msg_t *msg)
{
    if (RSH_VERSION!=msg->version) {
        return false;
    }
    else if (false==is_good_rsh_cookie(msg)) {
        return false;
    }
    else if (false==is_good_rsh_cmd(msg->cmd)) {
        return false;
    }
    else if (false==is_rsh_good_mode(msg->mode)) {
        return false;
    }
    else if (msg->src == msg->dst) {
        return false;
    }
    else if (false==is_good_rsh_slot(msg->src)) {
        return false;
    }
    else if (false==is_good_rsh_slot(msg->dst)) {
        return false;
    }
    else if (false==is_good_mac(msg->mac)) {
        return false;
    }
    else {
        return true;
    }
}

/*
* host sort
*/
static inline bool
is_good_rsh_msg(rsh_msg_t *msg)
{
    return __is_good_rsh_msg(msg) && msg->len <= RSH_MSG_DSIZE;
}

static inline bool
is_rsh_ack(rsh_msg_t *msg)
{
    return os_hasflag(msg->flag, RSH_F_ACK);
}

static inline int
rsh_msg_size(rsh_msg_t *msg)
{
    return sizeof(rsh_msg_t) + msg->len;
}

static inline void
rsh_m_release(rsh_msg_t *msg)
{
    os_free(msg->body.buffer[0]);
}

static inline void
rsh_msg_header_hton(rsh_msg_t *msg)
{
    msg->id     = htonl(msg->id);
    
    msg->len    = htons(msg->len);
    msg->u.error= htons(msg->u.error);
}
#define rsh_msg_header_ntoh(_msg)   rsh_msg_header_hton(_msg)

static inline int
rsh_msg_hton(rsh_msg_t *msg)
{
    if (false==is_good_rsh_msg(msg)) {
        return -EFORMAT;
    }

    rsh_msg_header_hton(msg);

    return 0;
}

static inline int
rsh_msg_ntoh(rsh_msg_t *msg)
{
    rsh_msg_header_ntoh(msg);
    
    if (false==is_good_rsh_msg(msg)) {
        return -EFORMAT;
    }

    return 0;
}
/******************************************************************************/
typedef struct {
    uint32_t ip;    /* network sort */
    int port;       /* network sort */
    
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
    
    byte mac[6], r[2];
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
#define rsh_mac                         rsh_cfg.mac
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
    debug_config("slot[%d]"                     __blank
        "config:"                               __blank
            "ip[%s] port[%d]"                   __blank
            "times[%d] interval[%d]"            __blank
        "echo:"                                 __blank
            "times[%d] interval[%d] alive[%d]"  __blank
            "connect[%llu] disconnect[%llu]"    __blank
            "send-ok[%llu] send-fail[%llu]"     __blank
            "recv-ok[%llu] recv-fail[%llu]",
        slot,
        os_ipstring(rsh_slot_ip(slot)), ntohs(rsh_slot_port(slot)),
        rsh_slot_echo_times(slot), rsh_slot_echo_interval(slot),
        rshd_echo_times(slot), rshd_echo_interval(slot), rshd_slot_alive(slot),
        rshd_echo_send(slot, 0), rshd_echo_send(slot, 1),
        rshd_echo_recv(slot, 0), rshd_echo_recv(slot, 1));
}
/******************************************************************************/
static inline void
rsh_msg_init(
    rsh_msg_t *msg, 
    int src,
    int dst,
    int cmd, 
    int mode,
    int flag,
    int maxrun, 
    uint32_t id
)
{
    os_objzero(msg);

    rsh_cookie_set(msg);
    os_maccpy(msg->mac, rsh_mac);
    msg->version= RSH_VERSION;

    msg->src    = src;
    msg->dst    = dst;
    msg->cmd    = cmd;
    msg->mode   = mode;
    msg->flag   = flag?RSH_MSG_FLAG(flag):0;
    msg->id     = id;
    msg->u.maxrun = maxrun;
}

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
    os_getmac_bystring(rsh_cfg.mac, basemac);

    rsh_cfg.config = env_gets(ENV_RSH_CONFIG, RSH_CONFIG_FILE);
    jcfg = jobj_file(rsh_cfg.config);
    if (NULL==jcfg) goto error;

#if RSH_SLOT_MULTI
    jobj_t jcurrent = jobj_get(jcfg, "current");
    if (NULL==jcurrent) goto error;
    rsh_current = jobj_get_int(jcurrent);
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
        uint32_t ip = inet_addr(ipstring);
        if (rsh_is_local(slot) && INADDR_NONE==ip) goto error;
        rsh_slot_ip(slot) = htonl(ip);

        jobj_t jport = jobj_get(obj, "port");
        if (NULL==jport) goto error;
        uint16_t port = jobj_get_int(jport);
        rsh_slot_port(slot) = htons(port);

        jobj_t jecho = jobj_get(obj, "echo");
        if (NULL==jecho) goto error;
        
        jobj_t jtimes = jobj_get(jecho, "times");
        if (NULL==jtimes) goto error;
        rsh_slot_echo_times(slot) = jobj_get_int(jtimes);
        
        jobj_t jinterval = jobj_get(jecho, "interval");
        if (NULL==jinterval) goto error;
        rsh_slot_echo_interval(slot) = jobj_get_int(jinterval);
        
        rsh_slot_dump(slot);
    }

    err = 0;
error:
    jobj_put(jcfg);

    return err;
}
/******************************************************************************/
#endif /* __RSH_H_cd4ac08f199c4732a4decf3ae976b791__ */
