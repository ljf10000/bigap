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

STATIC cmd_table_t ak_cmd[] = {
    CMD_TABLE_ENTRY(ak_cmd_reload, 1, "reload"),
};

STATIC int
__ak_main(int argc, char *argv[])
{
    return cmd_handle(ak_cmd, argc, argv, ak_usage);
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(__ak_main, argc, argv);
}

/******************************************************************************/ 
