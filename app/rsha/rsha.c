/*******************************************************************************
Copyright (c) 2015-2016, xxx Networks. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      rsha
#endif

#include "utils.h"
#include "rsh/rsh.h"

OS_INITER;

typedef struct {
    char *ipstring;
    uint32 ip;      /* network sort */
    int port;       /* network sort */
} rsh_slot_t;

typedef struct {
    int interval;   /* seconds */
    int times;
} rsh_echo_t;

typedef struct {
    char *config;       /* local config file */
    char *cloud;        /* cloud config file */
    char *basemac;
    char *request;
    char *response;
    
    int current;        /* current slot */
    int master;
    
    rsh_slot_t slot[RSH_SLOT_END];
    rsh_echo_t echo;
} rsh_config_t;

#define RSH_CONFIG_INITER               {   \
    .echo = {                               \
        .interval   = RSH_ECHO_INTERVAL,    \
        .times      = RSH_ECHO_TIMES,       \
    },                                      \
}

typedef struct {
    char *echo_request;
    int echo_size;
    
    int fd;
    bool echo;
} rsh_agent_t;

static rsh_config_t rsh_cfg = RSH_CONFIG_INITER;
static rsh_agent_t  rsh = {
    .fd = INVALID_FD,
    .usr= INVALID_VALUE,
};
static char rsh_buffer[64*1024];
static int rsh_size;

#define rsh_keepalive_times         rsh_cfg.echo.times
#define rsh_keepalive_interval      rsh_cfg.echo.interval

#define rsh_slot(_slot)                 (&rsh_cfg.slot[_slot])
#define rsh_slot_ip(_slot)              rsh_slot(_slot)->ip
#define rsh_slot_ipstring(_slot)        rsh_slot(_slot)->ipstring
#define rsh_slot_port(_slot)            rsh_slot(_slot)->port
#define RSH_SLOT_ADDR(_slot)            OS_SOCKADDR_INET(rsh_slot_ip(_slot), rsh_slot_port(_slot))

#define rsh_master                  rsh_cfg.master
#define rsh_master_ip               rsh_slot_ip(rsh_master)
#define rsh_master_ipstring         rsh_slot_ipstring(rsh_master)
#define rsh_master_port             rsh_slot_port(rsh_master)

#define rsh_current                 rsh_cfg.current
#define rsh_current_ip              rsh_slot_ip(rsh_current)
#define rsh_current_ipstring        rsh_slot_ipstring(rsh_current)
#define rsh_current_port            rsh_slot_port(rsh_current)

#define rsh_cloud_ip                rsh_slot_ip(RSH_SLOT_CLOUD)
#define rsh_cloud_ipstring          rsh_slot_ipstring(RSH_SLOT_CLOUD)
#define rsh_cloud_port              rsh_slot_port(RSH_SLOT_CLOUD)
#define RSH_CLOUD_ADDR              RSH_SLOT_ADDR(RSH_SLOT_CLOUD)

#define rsh_request                 rsh_cfg.request
#define rsh_response                rsh_cfg.response

static inline bool
is_cloud_ready(void)
{
    return rsh_cfg.cloud && rsh_cloud_ip && rsh_cloud_ipstring && rsh_cloud_port;
}

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
            "ip[%s] port[%d]",
        slot,
        rsh_slot_ipstring(slot), ntohs(rsh_slot_port(slot)));
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
    sockaddr_in_t server = RSH_SLOT_ADDR(slot);
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

/*
{
    "slot": [
        {
            "ip": "IP",
            "port": PORT
        },
        ...
        {
            "ip": "IP",
            "port": PORT
        }
    ],
    
    "echo": {
        "interval": INTERVAL,
        "times": TIMES,
    }
}
*/

static inline int
load_slot(jobj_t jslot, int slot)
{
    int err;
    
    jobj_t jip = jobj_get(jslot, "ip");
    if (NULL==jip) {
        return -ENOEXIST;
    }
    char *ipstring = jobj_get_string(jslot);
    uint32 ip = inet_addr(ipstring);
    if (INADDR_NONE==ip && rsh_is_local(slot)) {
        return -EBADCFG;
    }
    rsh_slot_ip(slot) = htonl(ip);
    rsh_slot_ipstring(slot) = os_strdup(ipstring);

    jobj_t jport = jobj_get(jslot, "port");
    if (NULL==jport) {
        rsh_slot_port(slot) = htons(RSH_PORT);
    } else {
        int port = jobj_get_i32(jport);
        rsh_slot_port(slot) = htons(port);
    }
        
    return 0;
}

static inline int
__load_config(jobj_t jcfg)
{
    int err, slot;
    
    jobj_t jslot = jobj_get(jcfg, "slot");
    if (RSH_SLOT_COUNT!=jarray_length(jslot)) {
        return -EBADCFG;
    }
    
    for (slot=0; slot<RSH_SLOT_END; slot++) {
        jobj_t jobj = jarray_get(jslot, slot);
        if (NULL==jobj) {
            return -ENOEXIST;
        }

        err = load_slot(jobj, slot);
        if (err<0) {
            return err;
        }
    }
    
    jobj_t jecho = jobj_get(jcfg, "echo");
    if (NULL==jecho) {
        return -ENOEXIST;
    }
    
    jobj_t jtimes = jobj_get(jecho, "times");
    if (NULL==jtimes) {
        return -ENOEXIST;
    }
    rsh_keepalive_times = jobj_get_i32(jtimes);
    
    jobj_t jinterval = jobj_get(jecho, "interval");
    if (NULL==jinterval) {
        return -ENOEXIST;
    }
    rsh_keepalive_interval = jobj_get_i32(jinterval);
    
    return 0;
}

static inline int
load_config(void)
{
    int err;
    jobj_t jobj = NULL;
    
    rsh_cfg.config = env_gets(ENV_RSH_CONFIG, RSH_CONFIG_FILE);
    jobj = jobj_file(rsh_cfg.config);
    if (NULL==jobj) {
        err = -EBADCFG; goto error;
    }
    
    err = __load_config(jobj);
error:
    jobj_put(jobj);
    
    return err;
}

static inline int
load_cloud(void)
{
    int err = 0;
    jobj_t jobj = NULL;

    if (NULL==rsh_cfg.cloud) {
        rsh_cfg.cloud = env_gets(ENV_RSH_CLOUD_CONFIG, RSH_CLOUD_CONFIG_FILE);
    }

    if (false==is_cloud_ready()) {
        jobj = jobj_file(rsh_cfg.cloud);
        if (NULL==jobj) {
            err = -EBADCFG; goto error;
        }
        
        err = load_slot(jobj, RSH_SLOT_CLOUD);
    }
error:
    jobj_put(jobj);
    
    return err;
}

static inline int
init_cfg(void)
{
    int err;
    jobj_t jcfg = NULL;
    char basemac[1+OS_LINE_LEN] = {0};
    
#if RSH_SLOT_MULTI
    rsh_current = env_geti(ENV_RSH_SLOT_LOCAL, RSH_SLOT_MASTER);
    rsh_master  = env_geti(ENV_RSH_SLOT_MASTER, RSH_SLOT_MASTER);
#else
    rsh_current = RSH_SLOT_MASTER;
    rsh_master  = RSH_SLOT_MASTER;
#endif

    /*
    * get basemac
    */
    os_pgets(basemac, OS_LINE_LEN, SCRIPT_PRODUCT_BASEMAC);
    if (false==is_good_macstring(basemac)) {
        return -EBADMAC;
    }
    rsh_cfg.basemac = os_strdup(basemac);
    
    /*
    * init echo-request
    */
    os_asprintf(&rsh.echo_request, RSH_ECHO_REQUEST, RSH_VERSION, basemac);
    if (NULL==rsh.echo_request) {
        return -EBADMAC;
    }
    rsh.echo_size = 1 + os_strlen(rsh.echo_request);
    
    err = load_config();
    if (err<0) {
        return err;
    }

    /*
    * maybe cloud-config NOT exist
    */
    load_cloud();

    return 0;
}

static int 
init_timer(void)
{
    return setup_timer(RSH_ECHO_INTERVAL, 0);
}

static int
init_net(void)
{
    int err;

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd<0) {
    	debug_error("socket error:%d", -errno);
        return -errno;
    }
    os_closexec(fd);

    rsh.fd = fd;

    return 0;
}

static int
__send(char *buf, int size)
{
    int err = 0;

    load_cloud();
    
    if (is_cloud_ready()) {
        struct sockaddr_in cloud = RSH_CLOUD_ADDR;
        
        err = io_sendto(rsh.fd, buf, size, &cloud, sizeof(cloud));
    }

    return err;
}

static int
__echo(void)
{
    return __send(rsh.echo_request, rsh.echo_size);
}

static int
__exec(jobj_t jrequest, int slot)
{
    int err = 0;
    
    if (slot==rsh_current) {
        err = os_system("%s '%s' &", rsh_response, rsh_buffer);
    } else {
        err = os_system("%s '%s' &", rsh_request, rsh_buffer);
    }

    return err;
}

static int
__command(jobj_t jrequest, char *cmd)
{
    jobj_t jseq = jobj_get(jrequest, "seq");
    if (NULL==jseq) {
        return -EBADPROTO;
    }
    
    jobj_t jslot = jobj_get(jrequest, "slot");
    if (NULL==jslot) {
        return -EBADPROTO;
    }
    int slot = jobj_get_i32(jslot);
    if (false==is_good_rsh_slot(slot)) {
        return -EBADSLOT;
    }
    
    jobj_t jstdin = jobj_get(jrequest, "stdin");
    if (NULL==jstdin) {
        return -EBADPROTO;
    }
    char *stdin = jobj_get_string(jstdin);
    if (NULL==stdin) {
        return -EBADPROTO;
    }
    
    jobj_t jmode = jobj_get(jrequest, "mode");
    if (NULL==jmode) {
        return -EBADPROTO;
    }
    char *modestring = jobj_get_string(jmode);
    if (NULL==modestring) {
        return -EBADPROTO;
    }
    
    int mode = rsh_mode_idx(modestring);
    switch(mode) {
        case RSH_MODE_ASYN: /* down */
        case RSH_MODE_REFLECT:
            return __exec(jrequest, slot);
        case RSH_MODE_ACK: /* down */
        case RSH_MODE_SYN: /* down */
        default:
            return -ENOSUPPORT;
    }
}

static int
__proto(jobj_t jrequest)
{
    /*
    * check version
    */
    jobj_t jversion = jobj_get(jrequest, "version");
    if (NULL==jversion) {
        return -EBADPROTO;
    }
    int version = jobj_get_i32(jversion);
    if (version!=RSH_VERSION) {
        return -EBADVERSION;
    }

    /*
    * check mac
    */
    jobj_t jmac = jobj_get(jrequest, "mac");
    if (NULL==jmac) {
        return -EBADPROTO;
    }
    char *mac = jobj_get_string(jmac);
    if (false==os_streq(mac, rsh_cfg.basemac)) {
        return -ENOMATCH;
    }

    /*
    * check cmd
    */
    jobj_t jcmd = jobj_get(jrequest, "cmd");
    if (NULL==jcmd) {
        return -EBADPROTO;
    }
    char *cmd = jobj_get_string(jcmd);
    if (os_streq(cmd, "echo")) {
        /*
        * now, NOT handle echo-response
        */
        return -ENOSUPPORT;
    }
    else if (os_streq(cmd, "command")) {
        return __command(jrequest, cmd);
    }
    else {
        return -EBADPROTO;
    }
}

static int
__handle(struct sockaddr_in *client)
{
    int err = 0;
    struct sockaddr_in cloud = RSH_CLOUD_ADDR;
    
    if (false==os_objeq(client, &cloud)) {
        return -ENOMATCH;
    }
    
    jobj_t jrequest = jobj(rsh_buffer);
    if (NULL==jrequest) {
        return -EBADJSON;
    }

    err = __proto(jrequest);
    jobj_put(jrequest);
    if (err<0) {
        return err;
    }

    return 0;
}

static int
__fini(void)
{
    os_fini();

    return 0;
}

static int
__init(void)
{
    int err;
    
    err = os_init();
    if (err<0) {
        return err;
    }

    err = init_cfg();
    if (err<0) {
        return err;
    }

    err = init_net();
    if (err<0) {
        return err;
    }

    return 0;
}

static void
__signal(int sig)
{
    switch(sig) {
        case SIGALRM: /* timer */
        case SIGUSR1: /* ipcp up */
            rsh.echo = true;

            break;
        default:
            break;
    }
}

static int
__signal_bottom(void)
{
    if (rsh.echo) {
        rsh.echo = false;
        
        __echo();
    }
}

static void
__exit(int sig)
{
    __fini();
    
    exit(sig);
}

static int
__service(void)
{
    int len, addrlen;
    struct sockaddr_in client;
    
    len = io_recvfrom(rsh.fd, rsh_buffer, sizeof(rsh_buffer), RSH_TICKS, &client, &addrlen);
    if (len<0) {
        return len;
    }
    rsh_buffer[len] = 0;
    
    return __handle(&client);
}

int __main(int argc, char *argv[])
{
    while(1) {
        __service();
        
        __signal_bottom();
    }
    
    return 0;
}

#ifndef __BUSYBOX__
#define rsha_main  main
#endif

int rsha_main(int argc, char **argv)
{
    setup_signal_exit(__exit);
    setup_signal_timer(__signal);
    setup_signal_user(__signal);
    setup_signal_callstack(NULL);
    
    return os_main(__init, __fini, __main, argc, argv);
}


/******************************************************************************/
