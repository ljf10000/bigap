/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      umd
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     cli
#endif

#define __DEAMON__
#include "umd.h"

/*
* handle {mac} {json}
*/
static int
handle_mac_json(struct um_user *(*handle)(byte mac[], jobj_t obj), char *args)
{
    char *mac   = args; cli_shift(args);
    char *json  = args; cli_shift(args);
    jobj_t obj  = NULL;
    int err = 0;
    
    if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        err = -EFORMAT; goto error;
    }
        
    obj = jobj_byjson(json);
    if (NULL==obj) {
        debug_trace("bad json %s", json);

        err = -EFORMAT; goto error;
    }

    struct um_user *user = (*handle)(os_mac(mac), obj);
    if (NULL==user) {
        err = -EFORMAT; goto error;
    }

error:
    jobj_put(obj);
    
    return err;
}

/*
* handle {mac} {ip}
*/
static int
handle_mac_ip(struct um_user *(*handle)(byte mac[], uint32 ip), char *args)
{
    char *mac   = args; cli_shift(args);
    char *ip    = args; cli_shift(args);

    if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return -EFORMAT;
    }
    else if (false==is_good_ipstring(ip)) {
        debug_trace("bad ip %s", ip);

        return -EFORMAT;
    }

    struct um_user *user = (*handle)(os_mac(mac), inet_addr(ip));
    if (NULL==user) {
        return -ENOEXIST;
    }

    return 0;
}

/*
* handle {mac}
*/
static int
handle_mac(int (*handle)(byte mac[]), char *args)
{
    char *mac = args; cli_shift(args);

    if (is_good_macstring(mac)) {
        return (*handle)(os_mac(mac));
    } else {
        debug_trace("bad mac %s", mac);

        return -EFORMAT;
    }
}

/*
* create {mac}
*/
static int
handle_create(char *args)
{
    int create(byte mac[])
    {
        struct um_user *user = um_user_create(mac);
        
        return user?0:-ENOMEM;
    }
    
    return handle_mac(create, args);
}

/*
* delete {mac}
*/
static int
handle_delete(char *args)
{
    return handle_mac(um_user_delete, args);
}

/*
* block {mac}
*/
static int
handle_block(char *args)
{
    int block(byte mac[])
    {
        struct um_user *user = um_user_block(mac);

        return user?0:-ENOMEM;
    }
    
    return handle_mac(block, args);
}

/*
* unblock {mac}
*/
static int
handle_unblock(char *args)
{
    return handle_mac(um_user_unblock, args);
}

/*
* bind {mac} {ip}
*/
static int
handle_bind(char *args)
{
    return handle_mac_ip(um_user_bind, args);
}

/*
* unbind {mac}
*/
static int
handle_unbind(char *args)
{
    return handle_mac(um_user_unbind, args);
}

/*
* fake {mac} {ip}
*/
static int
handle_fake(char *args)
{
    if (UM_AUTO_FAKE==umd.cfg.autouser) {
        return handle_mac_ip(um_user_fake, args);
    } else {
        return -ENOSUPPORT;
    }
}

/*
* unfake {mac}
*/
static int
handle_unfake(char *args)
{
    if (UM_AUTO_FAKE==umd.cfg.autouser) {
        return handle_mac(um_user_unfake, args);
    } else {
        return -ENOSUPPORT;
    }
}

/*
* auth {mac} {group} {json}
*/
static int
handle_auth(char *args)
{
    char *mac   = args; cli_shift(args);
    char *group = args; cli_shift(args);
    char *json  = args; cli_shift(args);
    jobj_t obj = NULL;
    
    if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return -EFORMAT;
    }
    
    int groupid = os_atoi(group);
    int err = 0;
    
    obj = jobj_byjson(json);
    if (NULL==obj) {
        debug_trace("bad json %s", json);

        err = -EBADJSON; goto error;
    }

    struct um_user *user = um_user_auth(os_mac(mac), groupid, obj);
    if (NULL==user) {
        err = -ENOEXIST; goto error;
    }

error:
    jobj_put(obj);
    
    return err;
}

/*
* deauth {mac}
*/
static int
handle_deauth(char *args)
{
    int deauth(byte mac[])
    {
        um_user_deauth(mac, UM_DEAUTH_ADMIN);

        return 0;
    }
    
    return handle_mac(deauth, args);
}

/*
* reauth {mac}
*/
static int
handle_reauth(char *args)
{
    if (umd.cfg.reauthable) {
        return handle_mac(um_user_reauth, args);
    } else {
        return -ENOSUPPORT;
    }
}

static mv_t
show_user(struct um_user *user)
{
    jobj_t obj = um_juser(user);
    
    if (obj) {
        cli_sprintf(__tab "%s" __crlf, jobj_json(obj));

        jobj_put(obj);
    }
    
    return mv2_ok;
}

static int
show_user_byjson(char *json)
{
    jobj_t obj      = NULL;
    jobj_t jmac     = NULL;
    jobj_t juser    = NULL;
    int err = 0;
    
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
    
    return err;
}

static int
show_stat(void)
{
    jobj_t obj = jobj_new_object();
    if (NULL==obj) {
        return -ENOMEM;
    }

    jobj_add_string(obj, "mac", os_macstring(umd.basemac));
    jobj_add_i32(obj, "user", h2_count(&umd.table));
    jobj_add(obj, "flow", um_jflow());

    cli_sprintf(__tab "%s" __crlf, jobj_json(obj));
    
    jobj_put(obj);
    
    return 0;
}

static int
show_count(void)
{
    cli_sprintf("%d" __crlf, h2_count(&umd.table));
    
    return 0;
}

/*
* show [json]
*/
static int
handle_show(char *args)
{
    char *json = args; /* json maybe include space, not shift */

    if (NULL==json) {
        /*
        * TODO: unix socket, 数据报方式, 无法传送大量数据，需要修改机制
        */
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
}

/*
* sync {mac} {json}
*/
static int
handle_sync(char *args)
{
    if (umd.cfg.syncable) {
        return handle_mac_json(um_user_sync, args);
    } else {
        return -ENOSUPPORT;
    }
}

/*
* tag {mac} {key} [value]
*/
static int
handle_tag(char *args)
{
    char *mac   = args; cli_shift(args);
    char *k     = args; cli_shift(args);
    char *v     = args; cli_shift(args);

    if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return -EFORMAT;
    }

    struct um_tag *tag = NULL;
    if (NULL==v) {
        tag = um_tag_get(os_mac(mac), k);
        if (tag) {
            cli_sprintf("%s" __crlf, tag->v);
        }
    } else {
        tag = um_tag_set(os_mac(mac), k, v);
    }
    
    return tag?0:-ENOEXIST;
}

/*
* gc
*/
static int
handle_gc(char *args)
{
    if (umd.cfg.gc) {
        return um_user_foreach(umd_gc, true);
    } else {
        return -ENOSUPPORT;
    }
}

static cli_table_t cli_table[] = {
    CLI_ENTRY("create", handle_create),
    CLI_ENTRY("delete", handle_delete),
    
    CLI_ENTRY("block",  handle_block),
    CLI_ENTRY("unblock",handle_unblock),
    
    CLI_ENTRY("bind",   handle_bind),
    CLI_ENTRY("unbind", handle_unbind),
    
    CLI_ENTRY("fake",   handle_fake),
    CLI_ENTRY("unfake", handle_unfake),
    
    CLI_ENTRY("auth",   handle_auth),
    CLI_ENTRY("deauth", handle_deauth),
    CLI_ENTRY("reauth", handle_reauth),
    
    CLI_ENTRY("sync",   handle_sync),
    CLI_ENTRY("show",   handle_show),
    CLI_ENTRY("tag",    handle_tag),
    CLI_ENTRY("gc",     handle_gc),
};

static int
cli_handle(sock_server_t *server)
{
    return cli_d_handle(server->fd, cli_table);
}

static int
cli_init(sock_server_t *server)
{
    int err;
    
    set_abstract_path(&server->addr.un, OS_UNIX_PATH("umd"));

    err = cli_u_server_init(server);
    if (err<0) {
        return err;
    }
    
    debug_ok("init cli server ok");
    
    return 0;
}

sock_server_t um_cli_server = 
    SOCK_SERVER_INITER(UM_SERVER_CLI, AF_UNIX, cli_init, cli_handle);
/******************************************************************************/
