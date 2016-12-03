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
    
    ak_load();
    ak_show(NULL, NULL);

    return 0;
}

STATIC int 
ak_cmd_load(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    ak_load();

    return 0;
}

STATIC int 
ak_handle_app_key(int argc, char *argv[], void (*handle)(char *app, char *key))
{
    char *app = argv[1];
    char *key = argv[2];

    if (app && 0==app[1] && os_iswildcard(app[0])) {
        app = NULL;
    }

    if (key && 0==key[1] && os_iswildcard(key[0])) {
        key = NULL;
    }

    (*handle)(app, key);

    return 0;
}

STATIC int 
ak_cmd_insert(int argc, char *argv[])
{
    char *app = argv[1];
    char *key = argv[2];
    char *value = argv[3];
    
    char *end = NULL;
    uint32 v = os_strtoul(value, &end, 0);
    
    if (INVALID_AKID==ak_insert(app, key, v)) {
        return -EINVAL;
    }

    return 0;
}

STATIC int 
ak_cmd_show(int argc, char *argv[])
{
    return ak_handle_app_key(argc, argv, ak_show);
}

STATIC int 
ak_cmd_jshow(int argc, char *argv[])
{
    return ak_handle_app_key(argc, argv, ak_jshow);
}

STATIC int
ak_usage(void)
{
    os_eprintln(__THIS_APPNAME " reload");
    os_eprintln(__THIS_APPNAME " load");
    os_eprintln(__THIS_APPNAME " insert {app} {key} {value}");
    os_eprintln(__THIS_APPNAME " show [app] [key]");
    os_eprintln(__THIS_APPNAME " jshow [app] [key]");

    return -EFORMAT;
}

STATIC int
ak_main_helper(int argc, char *argv[])
{
    static cmd_table_t cmd[] = {
        CMD_TABLE_ENTRY(ak_cmd_reload,  1, "reload"),
        CMD_TABLE_ENTRY(ak_cmd_load,    1, "load"),
        CMD_TABLE_ENTRY(ak_cmd_insert,  4, "insert", NULL, NULL, NULL),
        CMD_TABLE_ENTRY(ak_cmd_show,    3, "show", NULL, NULL),
        CMD_TABLE_ENTRY(ak_cmd_jshow,   3, "jshow", NULL, NULL),
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
