/*******************************************************************************
Copyright (c) 2015-2016, xxx Networks. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      haenv
#endif

#include "utils.h"
#include "haenv/haenv.h"

OS_INITER;

static int
cmd_init(int argc, char *argv[])
{
    int err;

    err = haenv_gc(false);
    if (err<0) {
        os_eprintln("gc error:%d", err);
        
        return err;
    }

    err = haenv_export();
    if (err<0) {
        os_eprintln("export error:%d", err);
        
        return err;
    }

    return 0;
}

static int
cmd_gc(int argc, char *argv[])
{
    return haenv_gc(true);
}

static int
cmd_get(int argc, char *argv[])
{
    char *k = argv[1];
    int err = 0;

    haenv_entry_t *e = haenv_find(k);
    if (NULL==e) {
        return -ENOEXIST;
    }
    
    os_println("%s", haee_value(e));

    return 0;
}

static int
cmd_set(int argc, char *argv[])
{
    char *k = argv[1];
    char *v = argv[2];
    int err = 0;
    
    err = haenv_append(k, v);
    if (err<0) {
        os_eprintln("append error:%d", err);
        
        return err;
    }

    err = haenv_flush();
    if (err<0) {
        os_eprintln("flush error:%d", err);
        
        return err;
    }

    err = haenv_export();
    if (err<0) {
        os_eprintln("export error:%d", err);
        
        return err;
    }

    return err;
}

static int
cmd_erase(int argc, char *argv[])
{
    int err;

    err = haenv_erase();
    if (err<0) {
        os_eprintln("erase error:%d", err);
        
        return err;
    }

    return 0;
}

static cmd_table_t cmd[] = {
    {
        .argc   = 3,
        .argv   = {"set", NULL, NULL},
        .handle = cmd_set,
    },
    {
        .argc   = 2,
        .argv   = {"get", NULL},
        .handle = cmd_get,
    },
    {
        .argc   = 1,
        .argv   = {"init"},
        .handle = cmd_init,
    },
    {
        .argc   = 1,
        .argv   = {"gc"},
        .handle = cmd_gc,
    },
    {
        .argc   = 1,
        .argv   = {"erase"},
        .handle = cmd_erase,
    },
};

static int
usage(void)
{
    os_eprintln(__THIS_APP_NAME " set {key}");
    os_eprintln(__THIS_APP_NAME " get {key} {value}");

    return -EFORMAT;
}

static void 
__exit(int signo)
{
    int err = haenv_fini();
    os_fini();
    
    exit(err);
}

static inline int
__init(void)
{
    int err = 0;
    
    setup_signal_exit(__exit);
    setup_signal_callstack(__exit);

    return haenv_init();
}

static inline int
__fini(void)
{
    return haenv_fini();
}

static int
__handle(int argc, char *argv[])
{
    int err;
    
    err = haenv_load();
    if (err<0) {
        return err;
    }
    
    err = haenv_check();
    if (err<0) {
        err = haenv_repaire();
        if (err<0) {
            return err;
        }
    }
    
    return cmd_handle(cmd, argc, argv, usage);
}

/*
* cmd have enabled when boot
*/
static int
__main(int argc, char *argv[])
{
    int err;

    haenv_lock();
    err = __handle(argc, argv);
    haenv_unlock();

    return err;
}

#ifndef __BUSYBOX__
#define haenv_main  main
#endif

int haenv_main(int argc, char *argv[])
{
    return os_call(__init, __fini, __main, argc, argv);
}
/******************************************************************************/
