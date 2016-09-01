/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
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

    os_dump_line(0, bin, len, NULL);
    
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
    CMD_TABLE_ENTRY(cmd_hex, 3, "hex", NULL, NULL),
    CMD_TABLE_ENTRY(cmd_str, 3, "str", NULL, NULL),
};

static int
usage(void)
{
    os_eprintln(__THIS_APPNAME " hex {hex} {MOD}");
    os_eprintln(__THIS_APPNAME " str {str} {MOD}");

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

int allinone_main(int argc, char *argv[])
{
    return os_main(__main, argc, argv);
}
/******************************************************************************/
