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
#define __CLI_TCP__     0
#include "umd.h"

/*
* handle {mac} {json}
*/
STATIC int
umd_handle_mac_json(umd_user_t *(*handle)(byte mac[], jobj_t obj), char *args)
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

    umd_user_t *user = (*handle)(os_mac(mac), obj);
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
STATIC int
umd_handle_mac_ip(umd_user_t *(*handle)(byte mac[], uint32 ip), char *args)
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

    umd_user_t *user = (*handle)(os_mac(mac), inet_addr(ip));
    if (NULL==user) {
        return -ENOEXIST;
    }

    return 0;
}

/*
* handle {mac}
*/
STATIC int
umd_handle_mac(int (*handle)(byte mac[]), char *args)
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
STATIC int
umd_handle_create(char *args)
{
    int create(byte mac[])
    {
        umd_user_t *user = umd_user_create(mac);
        
        return user?0:-ENOMEM;
    }
    
    return umd_handle_mac(create, args);
}

/*
* delete {mac}
*/
STATIC int
umd_handle_delete(char *args)
{
    return umd_handle_mac(umd_user_delete, args);
}

/*
* block {mac}
*/
STATIC int
umd_handle_block(char *args)
{
    int block(byte mac[])
    {
        umd_user_t *user = umd_user_block(mac);

        return user?0:-ENOMEM;
    }
    
    return umd_handle_mac(block, args);
}

/*
* unblock {mac}
*/
STATIC int
umd_handle_unblock(char *args)
{
    return umd_handle_mac(umd_user_unblock, args);
}

/*
* bind {mac} {ip}
*/
STATIC int
umd_handle_bind(char *args)
{
    return umd_handle_mac_ip(umd_user_bind, args);
}

/*
* unbind {mac}
*/
STATIC int
umd_handle_unbind(char *args)
{
    return umd_handle_mac(umd_user_unbind, args);
}

/*
* fake {mac} {ip}
*/
STATIC int
umd_handle_fake(char *args)
{
    if (UMD_AUTO_FAKE==umd.cfg.autouser) {
        return umd_handle_mac_ip(umd_user_fake, args);
    } else {
        return -ENOSUPPORT;
    }
}

/*
* unfake {mac}
*/
STATIC int
umd_handle_unfake(char *args)
{
    if (UMD_AUTO_FAKE==umd.cfg.autouser) {
        return umd_handle_mac(umd_user_unfake, args);
    } else {
        return -ENOSUPPORT;
    }
}

/*
* auth {mac} {group} {json}
*/
STATIC int
umd_handle_auth(char *args)
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

    umd_user_t *user = umd_user_auth(os_mac(mac), groupid, obj);
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
STATIC int
umd_handle_deauth(char *args)
{
    int deauth(byte mac[])
    {
        umd_user_deauth(mac, UMD_DEAUTH_ADMIN);

        return 0;
    }
    
    return umd_handle_mac(deauth, args);
}

/*
* reauth {mac}
*/
STATIC int
umd_handle_reauth(char *args)
{
    if (umd.cfg.reauthable) {
        return umd_handle_mac(umd_user_reauth, args);
    } else {
        return -ENOSUPPORT;
    }
}

STATIC mv_t
umd_show_user(umd_user_t *user)
{
    jobj_t obj = umd_juser(user);
    
    if (obj) {
        cli_sprintf(__tab "%s" __crlf, jobj_json(obj));

        jobj_put(obj);
    }
    
    return mv2_ok;
}

STATIC int
umd_show_user_byjson(char *json)
{
    jobj_t obj      = NULL;
    jobj_t jmac     = NULL;
    jobj_t jip      = NULL;
    jobj_t juser    = NULL;
    char *string    = NULL;
    umd_user_t *user = NULL;
    int err = 0;
    
    obj = jobj_byjson(json);
    if (NULL==obj) {
        err = -EFILTER; goto error;
    }
    
    if (NULL!=(jmac = jobj_get(obj, "mac"))) {
        string = jobj_get_string(jmac);
        if (false==is_good_macstring(string)) {
            err = -EBADMAC; goto error;
        }

        user = umd_user_get(os_mac(string));
    }
    else if (NULL!=(jip = jobj_get(obj, "ip"))) {
        string = jobj_get_string(jip);
        if (false==is_good_ipstring(string)) {
            err = -EBADIP; goto error;
        }

        user = umd_user_getbyip(inet_addr(string));
    }
    else {
        err = -EFILTER; goto error;
    }
    
    if (NULL==user) {
        err = -ENOEXIST; goto error;
    }
    
    juser = umd_juser(user);
    if (NULL==juser) {
        err = -ENOEXIST; goto error;
    }
    
    cli_sprintf(__tab "%s" __crlf, jobj_json(juser));
error:
    jobj_put(juser);
    jobj_put(obj);
    
    return err;
}

STATIC int
umd_show_stat(void)
{
    jobj_t obj = jobj_new_object();
    if (NULL==obj) {
        return -ENOMEM;
    }

    jobj_add_string(obj, "mac", os_macstring(umd.basemac));
    jobj_add_i32(obj, "user", h2_count(&umd.table));
    jobj_add(obj, "flow", umd_jflow());

    cli_sprintf(__tab "%s" __crlf, jobj_json(obj));
    
    jobj_put(obj);
    
    return 0;
}

STATIC int
umd_show_count(void)
{
    cli_sprintf("%d" __crlf, h2_count(&umd.table));
    
    return 0;
}

/*
* show [json]
*/
STATIC int
umd_handle_show(char *args)
{
    char *json = args; /* json maybe include space, not shift */
    
    if (NULL==json) {
        /*
        * TODO: unix socket, 数据报方式, 无法传送大量数据，需要修改机制
        */
        return umd_user_foreach(umd_show_user, false);
    }
    else if (os_streq("stat", json)) {
        return umd_show_stat();
    }
    else if (os_streq("count", json)) {
        return umd_show_count();
    }
    else if (is_good_json(json)) {
        return umd_show_user_byjson(json);
    }
    else {
        return -EFORMAT;
    }
}

/*
* sync {mac} {json}
*/
STATIC int
umd_handle_sync(char *args)
{
    if (umd.cfg.syncable) {
        return umd_handle_mac_json(umd_user_sync, args);
    } else {
        return -ENOSUPPORT;
    }
}

/*
* tag {mac} {key} [value]
*/
STATIC int
umd_handle_tag(char *args)
{
    char *mac   = args; cli_shift(args);
    char *k     = args; cli_shift(args);
    char *v     = args; cli_shift(args);

    if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return -EFORMAT;
    }

    umd_tag_t *tag = NULL;
    if (NULL==v) {
        tag = umd_user_tag_get(os_mac(mac), k);
        if (tag) {
            cli_sprintf("%s" __crlf, tag->v);
        }
    } else {
        tag = umd_user_tag_set(os_mac(mac), k, v);
    }
    
    return tag?0:-ENOEXIST;
}

/*
* gc
*/
STATIC int
umd_handle_gc(char *args)
{
    if (umd.cfg.gc) {
        return umd_user_foreach(umd_gc, true);
    } else {
        return -ENOSUPPORT;
    }
}

STATIC cli_table_t umd_cli_table[] = {
    CLI_ENTRY("create", umd_handle_create),
    CLI_ENTRY("delete", umd_handle_delete),
    
    CLI_ENTRY("block",  umd_handle_block),
    CLI_ENTRY("unblock",umd_handle_unblock),
    
    CLI_ENTRY("bind",   umd_handle_bind),
    CLI_ENTRY("unbind", umd_handle_unbind),
    
    CLI_ENTRY("fake",   umd_handle_fake),
    CLI_ENTRY("unfake", umd_handle_unfake),
    
    CLI_ENTRY("auth",   umd_handle_auth),
    CLI_ENTRY("deauth", umd_handle_deauth),
    CLI_ENTRY("reauth", umd_handle_reauth),
    
    CLI_ENTRY("sync",   umd_handle_sync),
    CLI_ENTRY("show",   umd_handle_show),
    CLI_ENTRY("tag",    umd_handle_tag),
    CLI_ENTRY("gc",     umd_handle_gc),
};

STATIC int
umd_cli_handle(sock_server_t *server)
{
    return clis_handle(server->fd, umd_cli_table);
}

STATIC int
umd_cli_init(sock_server_t *server)
{
    int fd;
    
    fd = __clis_fd(&server->addr.un);
    if (fd<0) {
        return fd;
    }
    server->fd = fd;
    
    debug_ok("init cli server ok");
    
    return 0;
}

sock_server_t umd_cli_server = 
    SOCK_USERVER_INITER(UMD_SERVER_CLI, "umd", umd_cli_init, umd_cli_handle);
/******************************************************************************/
