/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      haenv
#endif

#include "utils.h"
#include "haenv/haenv.h"

OS_INITER;

STATIC int
haenv_load_helper(bool check)
{
    int err;
    
    err = haenv_load();
    if (err<0) {
        return err;
    }

    if (check) {
        err = haenv_check();
        if (err<0) {
            os_eprintln("check error:%d", err);
            
            err = haenv_repaire();
            if (err<0) {
                os_eprintln("repaire error:%d", err);
                
                return err;
            }
        }
    }
    
    return 0;
}

STATIC int
haenv_cmd_init(int argc, char *argv[])
{
    int err;

    err = haenv_load_helper(true);
    if (err<0) {
        return err;
    }
    
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

STATIC int
haenv_cmd_gc(int argc, char *argv[])
{
    int err;
    
    err = haenv_load_helper(true);
    if (err<0) {
        return err;
    }
    
    return haenv_gc(true);
}

STATIC int
haenv_cmd_get(int argc, char *argv[])
{
    char *k = argv[1];
    int err = 0;
    
    err = haenv_load_helper(true);
    if (err<0) {
        return err;
    }

    haenv_entry_t *e = haenv_find(k);
    if (NULL==e) {
        return -ENOEXIST;
    }
    
    os_println("%s", haee_value(e));

    return 0;
}

STATIC int
haenv_cmd_set(int argc, char *argv[])
{
    char *k = argv[1];
    char *v = argv[2];
    int err = 0;
    
    err = haenv_load_helper(true);
    if (err<0) {
        return err;
    }

    err = haenv_gc(false);
    if (err<0) {
        return err;
    }
    
    haenv_entry_t *e = haenv_find(k);
    if (e && os_streq(v, haee_value(e))) {
        return 0;
    }
    
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

STATIC int
haenv_cmd_clean(int argc, char *argv[])
{
    int err;
    
    err = haenv_load_helper(false);
    if (err<0) {
        return err;
    }

    err = haenv_clean();
    if (err<0) {
        os_eprintln("clean error:%d", err);
        
        return err;
    }

    return 0;
}

STATIC int
haenv_cmd_deft(int argc, char *argv[])
{
    int err;
    
    err = haenv_load_helper(false);
    if (err<0) {
        return err;
    }

    err = haenv_deft();
    if (err<0) {
        os_eprintln("deft error:%d", err);
        
        return err;
    }

    return 0;
}

STATIC cmd_table_t haenv_cmd[] = {
    CMD_TABLE_ENTRY(haenv_cmd_set, 3, "set", NULL, NULL),
    CMD_TABLE_ENTRY(haenv_cmd_get, 2, "get", NULL),
    CMD_TABLE_ENTRY(haenv_cmd_init, 1, "init"),
    CMD_TABLE_ENTRY(haenv_cmd_gc, 1, "gc"),
    CMD_TABLE_ENTRY(haenv_cmd_clean, 1, "clean"),
    CMD_TABLE_ENTRY(haenv_cmd_deft, 1, "deft"),
};

STATIC int
haenv_usage(void)
{
    os_eprintln(__THIS_APPNAME " set {key}");
    os_eprintln(__THIS_APPNAME " get {key} {value}");

    return -EFORMAT;
}

STATIC void 
haenv_exit_helper(int signo)
{
    int err;

    haenv_unlock();
    err = haenv_fini();
    os_fini();
    
    exit(err);
}

STATIC int
haenv_init_helper(void)
{
    int err = 0;
    
    setup_signal_exit(haenv_exit_helper);
    setup_signal_callstack(haenv_exit_helper);
    
    err = os_init();
    if (err<0) {
        return err;
    }

    err = haenv_init();
    if (err<0) {
        return err;
    }

    return 0;
}

STATIC int
haenv_fini_helper(void)
{
    haenv_fini();

    return os_fini();
}

/*
* cmd have enabled when boot
*/
STATIC int
haenv_main_helper(int argc, char *argv[])
{
    int err;

    haenv_lock();
    err = cmd_handle(haenv_cmd, argc, argv, haenv_usage);
    haenv_unlock();
    
    return err;
}

int allinone_main(int argc, char *argv[])
{
    int err = os_call(haenv_init_helper, haenv_fini_helper, haenv_main_helper, argc, argv);

    return shell_error(err);
}
/******************************************************************************/
