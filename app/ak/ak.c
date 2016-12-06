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

STATIC void 
ak_show_helper(char *app, char *key) 
{
    mv_t foreach(char *name, char *k, uint32 v)
    {
        if ((NULL==app || os_streq(app, name)) &&
            (NULL==key || os_streq(key, k))) {
            os_println(__tab "%s.%s=%u", name, k, v);
        }

        return mv2_ok;
    }
    
    ak_foreach(foreach);

    return 0;
}

STATIC void 
ak_json_helper(char *app, char *key) 
{
    jobj_t jobj, japp, jval;
    ak_t *ak;
    int err;
    
    jobj = jobj_new_object();
    if (NULL==jobj) {
        return;
    }

    mv_t foreach(char *name, char *k, uint32 v)
    {
        if ((NULL==app || os_streq(app, name)) &&
            (NULL==key || os_streq(key, k))) {
            japp = jobj_get(jobj, name);
            if (NULL==japp) {
                japp = jobj_new_object();
                if (NULL==japp) {
                    return mv2_break(-ENOMEM);
                }
                
                jobj_add(jobj, name, japp);
            }

            if (NULL==jobj_get(japp, k)) {
                err = jobj_add_u32(japp, k, v);
                if (err<0) {
                    return mv2_break(err);
                }
            }
        }
        
        return mv2_ok;
    }

    ak_foreach(foreach);

    os_println("%s", jobj_json(jobj));

    jobj_put(jobj);
}

STATIC int 
ak_cmd_reload(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    ak_load();
    ak_show_helper(NULL, NULL);

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
ak_cmd_set(int argc, char *argv[])
{
    char *app = argv[1];
    char *key = argv[2];
    char *value = argv[3];

    akid_t id = __ak_getbynameEx(app, key);
    if (INVALID_AKID==id) {
        return -EINVAL;
    }

    char *end = NULL;
    uint32 v = os_strtoul(value, &end, 0);

    int err = __ak_set(id, v);
    if (err<0) {
        return err;
    }
    
    return 0;
}

STATIC int 
ak_cmd_show(int argc, char *argv[])
{
    return ak_handle_app_key(argc, argv, ak_show_helper);
}

STATIC int 
ak_cmd_json(int argc, char *argv[])
{
    return ak_handle_app_key(argc, argv, ak_json_helper);
}

STATIC int
ak_usage(void)
{
    os_eprintln(__THIS_APPNAME " reload");
    os_eprintln(__THIS_APPNAME " load");
    os_eprintln(__THIS_APPNAME " set {app} {key} {value}");
    os_eprintln(__THIS_APPNAME " show [app] [key]");
    os_eprintln(__THIS_APPNAME " json [app] [key]");

    return -EFORMAT;
}

STATIC int
ak_main_helper(int argc, char *argv[])
{
    static cmd_table_t cmd[] = {
        CMD_TABLE_ENTRY(ak_cmd_reload,  1, "reload"),
        CMD_TABLE_ENTRY(ak_cmd_load,    1, "load"),
        CMD_TABLE_ENTRY(ak_cmd_set,     4, "set", NULL, NULL, NULL),
        CMD_TABLE_ENTRY(ak_cmd_show,    3, "show", NULL, NULL),
        CMD_TABLE_ENTRY(ak_cmd_json,    3, "json", NULL, NULL),
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
