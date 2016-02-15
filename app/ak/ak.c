#ifndef __THIS_APP
#define __THIS_APP      ak
#endif

#include "utils.h"

OS_INITER;

#ifdef __BUSYBOX__
DECLARE_REAL_DEBUGGER;
DECLARE_REAL_AK;
DECLARE_REAL_FLOCK;
DECLARE_REAL_DEAMON;
DECLARE_REAL_OEM;
#endif

static int 
cmd_reload(int argc, char *argv[])
{
    return ak_reload();
}

static int
usage(void)
{
    os_eprintln(__THIS_NAME " reload");

    return -EFORMAT;
}

static simpile_cmd_t cmd[] = {
    {
        .argc   = 1,
        .argv   = {"reload"},
        .handle = cmd_reload,
    },
};

static int
__main(int argc, char *argv[])
{
    return simpile_cmd_handle(cmd, argc, argv, usage);
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
