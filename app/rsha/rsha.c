/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      rsha
#endif

#define __DEAMON__

#include "utils.h"
#include "rsh/rsh.h"

OS_INITER;
/******************************************************************************/
typedef struct {
    char *ipstring;
    uint32 ip;      /* network sort */
    int port;       /* network sort */
} rsh_slot_t;

typedef struct {
    char *config;       /* local config file */
    char *cloud;        /* cloud config file */
    
    char *basemac;
    char *request;
    char *response;
    
    int current;        /* current slot */
    int master;
    
    struct {
        int interval;   /* seconds */
        int times;
    } echo;
    
    rsh_slot_t slot[RSH_SLOT_END];
} rsh_config_t;

#define RSH_CONFIG_INITER               {   \
    .echo = {                               \
        .interval   = RSH_ECHO_INTERVAL,    \
        .times      = RSH_ECHO_TIMES,       \
    },                                      \
}

typedef struct {
    rsh_config_t cfg;

    struct {
        char *request;
        int size;
        
        bool sig;
    } echo;
        
    int fd;
} rsh_agent_t;

static rsh_agent_t rsh = {      \
    .cfg= RSH_CONFIG_INITER,    \
    .fd = INVALID_FD,           \
};
static char rsh_buffer[64*1024];

#define rsh_config                  rsh.cfg.config
#define rsh_cloud                   rsh.cfg.cloud
#define rsh_request                 rsh.cfg.request
#define rsh_response                rsh.cfg.response
#define rsh_basemac                 rsh.cfg.basemac

#define rsh_echo_times              rsh.cfg.echo.times
#define rsh_echo_interval           rsh.cfg.echo.interval
#define rsh_echo_request            rsh.echo.request
#define rsh_echo_size               rsh.echo.size
#define rsh_echo_sig                rsh.echo.sig

#define rsh_slot(_slot)             (&rsh.cfg.slot[_slot])
#define rsh_slot_ip(_slot)          rsh_slot(_slot)->ip
#define rsh_slot_ipstring(_slot)    rsh_slot(_slot)->ipstring
#define rsh_slot_port(_slot)        rsh_slot(_slot)->port
#define RSH_SLOT_ADDR(_slot)        OS_SOCKADDR_INET(rsh_slot_ip(_slot), rsh_slot_port(_slot))

#define rsh_master                  rsh.cfg.master
#define rsh_master_ip               rsh_slot_ip(rsh_master)
#define rsh_master_ipstring         rsh_slot_ipstring(rsh_master)
#define rsh_master_port             rsh_slot_port(rsh_master)

#define rsh_current                 rsh.cfg.current
#define rsh_current_ip              rsh_slot_ip(rsh_current)
#define rsh_current_ipstring        rsh_slot_ipstring(rsh_current)
#define rsh_current_port            rsh_slot_port(rsh_current)

#define rsh_cloud_ip                rsh_slot_ip(RSH_SLOT_CLOUD)
#define rsh_cloud_ipstring          rsh_slot_ipstring(RSH_SLOT_CLOUD)
#define rsh_cloud_port              rsh_slot_port(RSH_SLOT_CLOUD)
#define RSH_CLOUD_ADDR              RSH_SLOT_ADDR(RSH_SLOT_CLOUD)

static inline bool
is_cloud_ready(void)
{
    return rsh_cloud && rsh_cloud_ip && rsh_cloud_ipstring && rsh_cloud_port;
}
/******************************************************************************/
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
    uint32 ip;
    
    jobj_t jip = jobj_get(jslot, "ip");
    if (NULL==jip) {
        debug_error("invalid load slot[%d].ip", slot);
        
        return -ENOEXIST;
    }
    char *ipstring = jobj_get_string(jip);
    if (is_good_ipstring(ipstring)) {
        ip = inet_addr(ipstring);
    } else if (is_local_rsh_slot(slot)) {
        debug_error("bad load slot[%d].ip", slot);
        
        return -EBADCFG;
    } else {
        struct hostent *h = gethostbyname(ipstring);
        if (NULL==h) {
            return 0;
        }

        ip = *(uint32 *)(h->h_addr);
    }
    rsh_slot_ip(slot) = ip;
    rsh_slot_ipstring(slot) = os_strdup(os_ipstring(ip));

    jobj_t jport = jobj_get(jslot, "port");
    int port = jport?jobj_get_i32(jport):RSH_PORT;
    rsh_slot_port(slot) = htons(port);

    debug_config("rsh.config.slot[%d].ip=%s", slot, ipstring);
    debug_config("rsh.config.slot[%d].port=%d", slot, port);
    
    return 0;
}

static inline int
__load_config(jobj_t jcfg)
{
    int err, slot, count;
    
    jobj_t jslot = jobj_get(jcfg, "slot");
    count = jarray_length(jslot);
    if (RSH_SLOT_COUNT != count) {
        debug_error("rsh.config.slot.count(%d) != RSH_SLOT_COUNT", count);
        
        return -EBADCFG;
    }
    debug_config("rsh.config.slot.count=%d", count);
    
    for (slot=0; slot<RSH_SLOT_CLOUD; slot++) {
        jobj_t jobj = jarray_get(jslot, slot);
        if (NULL==jobj) {
            debug_error("get rsh.config.slot[%d] error", slot);
            
            return -ENOEXIST;
        }

        err = load_slot(jobj, slot);
        if (err<0) {
            return err;
        }
    }

    jobj_t jecho = jobj_get(jcfg, "echo");
    if (jecho) {
        jobj_t jtimes = jobj_get(jecho, "times");
        if (jtimes) {
            rsh_echo_times = jobj_get_i32(jtimes);
        }
        
        jobj_t jinterval = jobj_get(jecho, "interval");
        if (jinterval) {
            rsh_echo_interval = jobj_get_i32(jinterval);
        }
    }
    
    debug_config("rsh.config.echo.times=%d", rsh_echo_times);
    debug_config("rsh.config.echo.interval=%d", rsh_echo_interval);
    
    return 0;
}

static inline int
load_config(void)
{
    int err;
    jobj_t jcfg = NULL;
    
    jcfg = jobj_byfile(rsh_config);
    if (NULL==jcfg) {
        debug_error("invalid rsh.config");
        
        err = -EBADCFG; goto error;
    }
    
    err = __load_config(jcfg);
error:
    jobj_put(jcfg);
    
    return err;
}

static inline int
load_cloud(void)
{
    int err = 0;
    jobj_t jcfg = NULL;

    if (is_cloud_ready()) {
        return 0;
    }
    
    jcfg = jobj_byfile(rsh_cloud);
    if (NULL==jcfg) {
        debug_error("invalid rsh.cloud");
        
        err = -EBADCFG; goto error;
    }
    
    err = load_slot(jcfg, RSH_SLOT_CLOUD);
error:
    jobj_put(jcfg);
    
    return err;
}

static inline int
init_cfg(void)
{
    int err;
    jobj_t jcfg = NULL;
    
#if RSH_SLOT_MULTI
    rsh_current = env_geti(ENV_RSH_SLOT_LOCAL, RSH_SLOT_MASTER);
    rsh_master  = env_geti(ENV_RSH_SLOT_MASTER, RSH_SLOT_MASTER);
#else
    rsh_current = RSH_SLOT_MASTER;
    rsh_master  = RSH_SLOT_MASTER;
#endif
    debug_config("rsh.current=%d", rsh_current);
    debug_config("rsh.master=%d", rsh_master);
    
    rsh_request = env_gets(ENV_RSH_REQUEST_SCRIPT,  RSH_REQUEST_SCRIPT);
    debug_config("rsh.request=%s", rsh_request);
    
    rsh_response= env_gets(ENV_RSH_RESPONSE_SCRIPT, RSH_RESPONSE_SCRIPT);
    debug_config("rsh.response=%s", rsh_response);
    
    rsh_config  = env_gets(ENV_RSH_CONFIG, RSH_CONFIG_FILE);
    debug_config("rsh.config=%s", rsh_config);
    
    rsh_cloud   = env_gets(ENV_RSH_CLOUD_CONFIG, RSH_CLOUD_CONFIG_FILE);
    debug_config("rsh.cloud=%s", rsh_cloud);

    /*
    * get basemac
    */
    char *basemac = os_getbasemac();
    if (NULL==basemac) {
        debug_error("get basemac error");
        
        return -EBADMAC;
    }
    rsh_basemac = os_strdup(basemac);
    debug_config("rsh.basemac=%s", rsh_basemac);
    
    /*
    * init echo-request
    */
    os_asprintf(&rsh_echo_request, RSH_ECHO_REQUEST, RSH_VERSION, rsh_basemac);
    if (NULL==rsh_echo_request) {
        debug_error("init echo request error");
        
        return -ENOMEM;
    }
    rsh_echo_size = os_strlen(rsh_echo_request);
    debug_config("rsh.echo.request=%s, size=%d", rsh_echo_request, rsh_echo_size);
    
    err = load_config();
    if (err<0) {
        return err;
    }

    /*
    * maybe failed(cloud-config NOT exist)
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

        err = io_sendto(rsh.fd, buf, size, (sockaddr_t *)&cloud, sizeof(cloud));
    }

    return err;
}

static int
__echo(void)
{    
    return __send(rsh_echo_request, rsh_echo_size);
}

static int
__exec(jobj_t jrequest, int slot)
{
    int err = 0;
    
    if (slot==rsh_current) {
            debug_proto("%s '%s' &", rsh_response, rsh_buffer);
        err = os_system("%s '%s' &", rsh_response, rsh_buffer);
    } else {
            debug_proto("%s '%s' &", rsh_request, rsh_buffer);
        err = os_system("%s '%s' &", rsh_request, rsh_buffer);
    }

    return err;
}

static int
__command(jobj_t jrequest, char *cmd)
{
    jobj_t jseq = jobj_get(jrequest, "seq");
    if (NULL==jseq) {
        debug_proto("no-found request.seq");
        
        return -EBADPROTO;
    }
    
    jobj_t jslot = jobj_get(jrequest, "slot");
    if (NULL==jslot) {
        debug_proto("no-found request.slot");
        
        return -EBADPROTO;
    }
    int slot = jobj_get_i32(jslot);
    if (false==is_local_rsh_slot(slot)) {
        debug_proto("invalid request.slot:%d", slot);
        
        return -EBADSLOT;
    }
    
    jobj_t jstdin = jobj_get(jrequest, "stdin");
    if (NULL==jstdin) {
        debug_proto("no-found request.stdin");
        
        return -EBADPROTO;
    }
    char *stdin = jobj_get_string(jstdin);
    if (NULL==stdin) {
        debug_proto("invalid request.stdin");
        
        return -EBADPROTO;
    }
    
    jobj_t jmode = jobj_get(jrequest, "mode");
    if (NULL==jmode) {
        debug_proto("no-found request.mode");
        
        return -EBADPROTO;
    }
    char *modestring = jobj_get_string(jmode);
    if (NULL==modestring) {
        debug_proto("invalid request.mode");
        
        return -EBADPROTO;
    }
    
    int mode = rsh_mode_ops()->getid(modestring);
    switch(mode) {
        case RSH_MODE_ASYN: /* down */
        case RSH_MODE_REFLECT:
            return __exec(jrequest, slot);
        case RSH_MODE_ACK: /* down */
        case RSH_MODE_SYN: /* down */
        default:
            debug_proto("no-support request.mode:%s", modestring);
            
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
        debug_proto("no-found request.version");
        
        return -EBADPROTO;
    }
    int version = jobj_get_i32(jversion);
    if (version!=RSH_VERSION) {
        debug_proto("invalid request.version:%d, proto.version=%d", version, RSH_VERSION);
        
        return -EBADVERSION;
    }

    /*
    * check mac
    */
    jobj_t jmac = jobj_get(jrequest, "mac");
    if (NULL==jmac) {
        debug_proto("no-found request.mac");
        
        return -EBADPROTO;
    }
    char *mac = jobj_get_string(jmac);
    if (false==os_streq(mac, rsh_basemac)) {
        debug_proto("invalid request.mac:%s, basemac=%s", mac, rsh_basemac);
        
        return -ENOMATCH;
    }

    /*
    * check cmd
    */
    jobj_t jcmd = jobj_get(jrequest, "cmd");
    if (NULL==jcmd) {
        debug_proto("no-found request.cmd");
        
        return -EBADPROTO;
    }
    char *cmd = jobj_get_string(jcmd);
    if (os_streq(cmd, "echo")) {
        /*
        * now, NOT handle echo-response
        */
        debug_proto("no-support request.cmd:echo from cloud");
        
        return -ENOSUPPORT;
    }
    else if (os_streq(cmd, "command")) {
        return __command(jrequest, cmd);
    }
    else {
        debug_proto("invalid request.cmd:%s", cmd);
        
        return -EBADPROTO;
    }
}

static int
__handle(struct sockaddr_in *client)
{
    int err = 0;
    struct sockaddr_in cloud = RSH_CLOUD_ADDR;
    
    if (false==os_objeq(client, &cloud)) {
        debug_proto("invalid request.source(not cloud), drop it");
        
        return -ENOMATCH;
    }
    
    jobj_t jrequest = jobj_byjson(rsh_buffer);
    if (NULL==jrequest) {
        debug_proto("invalid request");
        
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
        debug_error("init config error:%d", err);
        
        return err;
    }
    
    err = init_net();
    if (err<0) {
        debug_error("init net error:%d", err);
        
        return err;
    }
    
    err = init_timer();
    if (err<0) {
        debug_error("init timer error:%d", err);
        
        return err;
    }
    
    debug_ok("init ok");
    
    return 0;
}

static void
__signal(int sig)
{
    switch(sig) {
        case SIGALRM: /* timer */
        case SIGUSR1: /* ipcp up */
            rsh_echo_sig = true;

            break;
        default:
            break;
    }
}

static int
__signal_bottom(void)
{
    if (rsh_echo_sig) {
        rsh_echo_sig = false;
        
        __echo();
    }

    return 0;
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
    int len;
    socklen_t addrlen;
    struct sockaddr_in client;
    
    len = io_recvfrom(rsh.fd, rsh_buffer, sizeof(rsh_buffer), RSH_TICKS, (sockaddr_t *)&client, &addrlen);
    if (len<0) {
        return len;
    }
    rsh_buffer[len] = 0;
    
    return __handle(&client);
}

int __main(int argc, char *argv[])
{
    __echo();
    
    while(1) {
        __service();
        
        __signal_bottom();
    }
    
    return 0;
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(__exit);
    setup_signal_timer(__signal);
    setup_signal_user(__signal);
    setup_signal_callstack(NULL);
    
    int err = os_call(__init, __fini, __main, argc, argv);

    return shell_error(err);
}


/******************************************************************************/
