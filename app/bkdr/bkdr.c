/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      bkdr
#endif

#include "utils.h"

OS_INITER;

STATIC int
bkdr_cmd_hex(int argc, char *argv[])
{
    char *hex   = argv[1];
    char *mod   = argv[2];
    byte bin[1+OS_HUGE_LEN];
    int len;
    
    len = os_hex2bin(hex, bin, sizeof(bin));
    if (len<0) {
        os_eprintln("too long hex");

        return -ETOOBIG;
    }

    os_dump_line(0, bin, len, NULL);
    
    os_println("%d", os_bin_bkdr(bin, len)%os_atoi(mod));

    return 0;
}

STATIC int
bkdr_cmd_str(int argc, char *argv[])
{
    char *str   = argv[1];
    char *mod   = argv[2];
    
    os_println("%d", os_str_bkdr(str)%os_atoi(mod));

    return 0;
}

STATIC cmd_table_t bkdr_cmd[] = {
    CMD_TABLE_ENTRY(bkdr_cmd_hex, 3, "hex", NULL, NULL),
    CMD_TABLE_ENTRY(bkdr_cmd_str, 3, "str", NULL, NULL),
};

STATIC int
bkdr_usage(void)
{
    os_eprintln(__THIS_APPNAME " hex {hex} {MOD}");
    os_eprintln(__THIS_APPNAME " str {str} {MOD}");

    return -EFORMAT;
}

/*
* cmd have enabled when boot
*/
STATIC int
bkdr_main_helper(int argc, char *argv[])
{
    int err;

    err = cmd_handle(bkdr_cmd, argc, argv, bkdr_usage);
    
    return err;
}

int allinone_main(int argc, char *argv[])
{
    return os_main(bkdr_main_helper, argc, argv);
}
/******************************************************************************/
