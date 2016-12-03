/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      ak
#endif

/*
* for reload
*/
#define __DEAMON__

#include "utils.h"

OS_REAL_INITER;

STATIC int 
ak_cmd_reload(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    return ak_reload();
}

STATIC int
ak_usage(void)
{
    os_eprintln(__THIS_APPNAME " reload");

    return -EFORMAT;
}

STATIC int
ak_main_helper(int argc, char *argv[])
{
    static cmd_table_t cmd[] = {
        CMD_TABLE_ENTRY(ak_cmd_reload, 1, "reload"),
    };

    return cmd_handle(cmd, argc, argv, ak_usage);
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(ak_main_helper, argc, argv);
}

/******************************************************************************/ 
