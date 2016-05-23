/*******************************************************************************
Copyright (c) 2016-2018, Supper Wali Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      ak
#endif

#include "utils.h"

OS_REAL_INITER;

static int 
cmd_reload(int argc, char *argv[])
{
    ak_println("cmd_reload");
    
    return ak_reload();
}

static int
usage(void)
{
    os_eprintln(__THIS_APP_NAME " reload");

    return -EFORMAT;
}

static cmd_table_t cmd[] = {
    {
        .argc   = 1,
        .argv   = {"reload"},
        .handle = cmd_reload,
    },
};

static int
__main(int argc, char *argv[])
{
    return cmd_handle(cmd, argc, argv, usage);
}

#ifndef __BUSYBOX__
#define appkey_main  main
#endif

int appkey_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(__main, argc, argv);
}

/******************************************************************************/ 
