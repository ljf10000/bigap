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

static int 
cmd_reload(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    return ak_reload();
}

static int
usage(void)
{
    os_eprintln(__THIS_APPNAME " reload");

    return -EFORMAT;
}

static cmd_table_t cmd[] = {
    CMD_TABLE_ENTRY(cmd_reload, 1, "reload"),
};

static int
__main(int argc, char *argv[])
{
    return cmd_handle(cmd, argc, argv, usage);
}

#ifndef __BUSYBOX__
#define ak_main  main
#endif

int ak_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(__main, argc, argv);
}

/******************************************************************************/ 
