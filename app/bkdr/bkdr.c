/*******************************************************************************
Copyright (c) 2015-2016, xxx Networks. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      bkdr
#endif

#include "utils.h"

OS_INITER;

static int
cmd_hex(int argc, char *argv[])
{
    char *hex   = argv[1];
    char *mod   = argv[2];
    byte bin[1+OS_HUGE_LEN];
    int len;
    
    len = os_hextobin(hex, bin, sizeof(bin));
    if (len<0) {
        os_eprintln("too long hex");

        return -ETOOBIG;
    }
    
    os_println("%d", os_bin_bkdr(bin, len)%os_atoi(mod));

    return 0;
}

static int
cmd_str(int argc, char *argv[])
{
    char *str   = argv[1];
    char *mod   = argv[2];
    
    os_println("%d", os_str_bkdr(str)%os_atoi(mod));

    return 0;
}

static cmd_table_t cmd[] = {
    {
        .argc   = 3,
        .argv   = {"hex", NULL, NULL},
        .handle = cmd_hex,
    },
    {
        .argc   = 3,
        .argv   = {"str", NULL, NULL},
        .handle = cmd_str,
    },
};

static int
usage(void)
{
    os_eprintln(__THIS_APP_NAME " hex {hex} {MOD}");
    os_eprintln(__THIS_APP_NAME " str {str} {MOD}");

    return -EFORMAT;
}

/*
* cmd have enabled when boot
*/
static int
__main(int argc, char *argv[])
{
    int err;

    err = cmd_handle(cmd, argc, argv, usage);
    
    return err;
}

#ifndef __BUSYBOX__
#define bkdr_main  main
#endif

int bkdr_main(int argc, char *argv[])
{
    return os_main(__main, argc, argv);
}
/******************************************************************************/
