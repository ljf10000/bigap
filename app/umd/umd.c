/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      umd
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     umd
#endif

#define __DEAMON__
#include "umd.h"
#define static
struct um_control umd = UMD_INITER;

static int
init_pre(void)
{
    int i;
    char *basemac = os_getbasemac();
    if (NULL==basemac) {
        return -EBADBASEMAC;
    }
    __os_getmac_bystring(umd.basemac, basemac);

    umd.ether_type_ip  = htons(ETHERTYPE_IP);
    umd.ether_type_vlan= htons(ETHERTYPE_VLAN);
    umd.ether_type_all = htons(ETH_P_ALL);

    for (i=0; i<os_count_of(umd.lan); i++) {
        umd.lan[i].ip   = inet_addr(umd.lan[i].ipstring);
        umd.lan[i].mask = inet_addr(umd.lan[i].maskstring);
    }

    umd.conf = env_gets(OS_ENV(CONF), UMD_CONF);
    
    return 0;
}

static int
init_post(void)
{
    int err;
    int hash_size[UM_USER_NIDX_END] = {
        [UM_USER_NIDX_MAC] = umd.cfg.machashsize,
        [UM_USER_NIDX_IP]  = umd.cfg.iphashsize,
    };
    
    err = h2_init(&umd.table, hash_size);
    if (err<0) {
        return err;
    }

    err = sock_servers_init(umd.server, umd.server_count);
    if (err<0) {
        return err;
    }

    os_system("%s init &", umd.cfg.script_event);

    return 0;
}

static int
__fini(void)
{
    if (false==umd.deinit) {
        umd.deinit = true;
        
        os_system("%s fini &", umd.cfg.script_event);
    }
    
    os_fini();

    return 0;
}

static void 
__exit(int sig)
{
    __fini();
    
    exit(sig);
}

extern int init_cfg(void);

static int
__init(void)
{
    int err;

    err = os_init();
    if (err<0) {
        return err;
    }

    err = init_pre();
    if (err<0) {
        return err;
    }

    err = init_cfg();
    if (err<0) {
        return err;
    }

    err = init_post();
    if (err<0) {
        return err;
    }
    
    return 0;
}

static int
__main(int argc, char **argv)
{
    int err = 0;

    update_limit_test();

    while(1) {
        sock_servers_run(umd.server, umd.server_count);
    }
    
    return err;
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(__exit);
    setup_signal_callstack(NULL);
    
    int err = os_call(__init, __fini, __main, argc, argv);

    return shell_error(err);
}
/******************************************************************************/
