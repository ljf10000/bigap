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

OS_INITER;

struct um_control umd = {
    .hash_size = {
        [UM_USER_NIDX_MAC]  = UMD_HASHSIZE,
        [UM_USER_NIDX_IP]   = UMD_HASHSIZE,
    },

    .cfg = {
        .lan = UM_LAN_INITER,
    },
};

static int
init_base(void)
{
    char *basemac = os_getbasemac();
    if (NULL==basemac) {
        return -EBADBASEMAC;
    }
    __os_getmac_bystring(umd.basemac, basemac);

    umd.ether_type_ip  = htons(ETHERTYPE_IP);
    umd.ether_type_vlan= htons(ETHERTYPE_VLAN);
    umd.ether_type_all = htons(ETH_P_ALL);

    return 0;
}

static void
init_event(void)
{
    os_system(UMD_SCRIPT_EVENT " init &");
}

static void
fini_event(void)
{
    if (false==umd.deinit) {
        umd.deinit = true;
        
        os_system(UMD_SCRIPT_EVENT " fini &");
    }
}

static int
__fini(void)
{
    fini_event();
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

    err = init_base();
    if (err<0) {
        return err;
    }
    
    err = init_cfg();
    if (err<0) {
        return err;
    }
    
    err = h2_init(&umd.table, umd.hash_size);
    if (err<0) {
        return err;
    }
    
    err = __cli_server_init(umd.server, umd.server_count);
    if (err<0) {
        return err;
    }

    init_event();

    return 0;
}

static int
__main(int argc, char **argv)
{
    int err = 0;

    update_limit_test();

    while(1) {
        __cli_server_run(umd.server, umd.server_count);
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
