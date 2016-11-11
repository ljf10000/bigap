/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      nsqa
#endif
    
#ifndef __THIS_FILE
#define __THIS_FILE     cli
#endif
    
#define __DEAMON__
#include "nsqa.h"
/******************************************************************************/
/*
* remove {name}
*/
static int
handle_remove(char *args)
{
    char *name = args;
    
    return nsqi_remove(name);
}

/*
* insert {json}
*/
static int
handle_insert(char *args)
{
    char *json  = args;

    debug_cli("handle insert %s", json);
    
    return nsqi_insert(json);
}

static mv_t
show_user(struct um_user *user)
{
#if 0
    jobj_t obj = um_juser(user);
    
    if (obj) {
        cli_sprintf(__tab "%s" __crlf, jobj_json(obj));

        jobj_put(obj);
    }
#endif

    return mv2_ok;
}

static int
show_user_byjson(char *json)
{
    jobj_t obj      = NULL;
    jobj_t jmac     = NULL;
    jobj_t juser    = NULL;
    int err = 0;
#if 0
    obj = jobj_byjson(json);
    if (NULL==obj) {
        err = -EFILTER; goto error;
    }
    
    jmac = jobj_get(obj, "mac");
    if (NULL==obj) {
        err = -EFILTER; goto error;
    }
    
    char *macstring = jobj_get_string(jmac);
    if (false==is_good_macstring(macstring)) {
        err = -EBADMAC; goto error;
    }

    struct um_user *user = um_user_get(os_mac(macstring));
    if (NULL==user) {
        err = -ENOEXIST; goto error;
    }

    juser = um_juser(user);
    if (NULL==juser) {
        err = -ENOEXIST; goto error;
    }
    
    cli_sprintf(__tab "%s" __crlf, jobj_json(juser));
error:
    jobj_put(juser);
    jobj_put(obj);
#endif

    return err;
}

static int
show_stat(void)
{
    jobj_t obj = jobj_new_object();
    if (NULL==obj) {
        return -ENOMEM;
    }
#if 0
    jobj_add_string(obj, "mac", os_macstring(umd.basemac));
    jobj_add_i32(obj, "user", h1_count(&umd.table));
    jobj_add(obj, "flow", um_jflow());

    cli_sprintf(__tab "%s" __crlf, jobj_json(obj));
    
    jobj_put(obj);
#endif

    return 0;
}

static int
show_count(void)
{
    cli_sprintf("%d" __crlf, h1_count(&nsqa.table));
    
    return 0;
}

/*
* show [json]
*/
static int
handle_show(char *args)
{
    char *json = args; /* json maybe include space, not shift */
#if 0
    if (NULL==json) {
        return um_user_foreach(show_user, false);
    }
    else if (os_streq("stat", json)) {
        return show_stat();
    }
    else if (os_streq("count", json)) {
        return show_count();
    }
    else if (is_good_json(json)) {
        return show_user_byjson(json);
    }
    else {
        return -EFORMAT;
    }
#endif
}

static cli_table_t cli_table[] = {
    CLI_ENTRY("insert", handle_insert),
    CLI_ENTRY("remove", handle_remove),
    CLI_ENTRY("show",   handle_show),
};

static int
nsq_cli(loop_watcher_t *watcher)
{
    int err;
    
    err = clis_handle(watcher->fd, cli_table);
    
    os_loop_del_watcher(&nsqa.loop, watcher->fd);

    return err;
}

int
init_nsq_cli(void)
{
    int err;

    err = os_loop_add_CLI(&nsqa.loop, "nsqa", nsq_cli);
    if (err<0) {
        return err;
    }
    
    return 0;
}
/******************************************************************************/
