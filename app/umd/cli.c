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
#define __THIS_USAGE \
    "umc usage:"                            __crlf \
    __tab " associate   {mac} {ifname}"     __crlf \
    __tab " diassociate {mac}"              __crlf \
    __tab " bind   {mac} {ip}"              __crlf \
    __tab " unbind {mac}"                   __crlf \
    __tab " fake   {mac} {ip}"              __crlf \
    __tab " unfake {mac}"                   __crlf \
    __tab " auth   {mac} {group} {json}"    __crlf \
    __tab " deauth {mac}"                   __crlf \
    __tab " reauth {mac}"                   __crlf \
    __tab " tag    {mac} {key} [value]"     __crlf \
    __tab " gc     {user|conn}"             __crlf \
    __tab " show [stat | json]"             __crlf \
    /* end */
#include "umd.h"
/******************************************************************************/
/*
* handle {mac} {json}
*/
STATIC int
umd_handle_mac_json(umd_user_t *(*handle)(byte mac[], jobj_t obj), int argc, char *argv[])
{
    char *mac   = argv[1];
    char *json  = argv[2];
    jobj_t obj  = NULL;
    int err = 0;
    
    if (3!=argc) {
        return cli_help(-EFORMAT);
    }
    else if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return cli_help(-EFORMAT);
    }
    else if (false==is_good_json(json)) {
        debug_trace("bad json %s", json);

        return cli_help(-EFORMAT);
    }

    obj = jobj_byjson(json);
    if (NULL==obj) {
        debug_trace("bad json %s", json);

        err = cli_help(-EFORMAT); goto error;
    }

    umd_user_t *user = (*handle)(os_mac(mac), obj);
    if (NULL==user) {
        err = cli_help(-EFORMAT); goto error;
    }

error:
    jobj_put(obj);
    
    return err;
}

/*
* handle {mac} {ip}
*/
STATIC int
umd_handle_mac_ip(umd_user_t *(*handle)(byte mac[], uint32 ip), int argc, char *argv[])
{
    char *mac   = argv[0];
    char *ip    = argv[1];
    
    if (3!=argc) {
        return cli_help(-EFORMAT);
    }
    else if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return cli_help(-EFORMAT);
    }
    else if (false==is_good_ipstring(ip)) {
        debug_trace("bad ip %s", ip);

        return cli_help(-EFORMAT);
    }

    umd_user_t *user = (*handle)(os_mac(mac), os_ipaddr(ip));
    if (NULL==user) {
        return -ENOEXIST;
    }

    return 0;
}

/*
* handle {mac}
*/
STATIC int
umd_handle_mac(int (*handle)(byte mac[]), int argc, char *argv[])
{
    char *mac = argv[1];
    
    if (2!=argc) {
        return cli_help(-EFORMAT);
    }
    else if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return cli_help(-EFORMAT);
    }

    return (*handle)(os_mac(mac));
}

/*
* create {mac}
*/
STATIC int
umd_handle_create(cli_table_t *table, int argc, char *argv[])
{
    int create(byte mac[])
    {
        umd_user_t *user = umd_user_create(mac);
        
        return user?0:-ENOMEM;
    }
    
    return umd_handle_mac(create, argc, argv);
}

/*
* delete {mac}
*/
STATIC int
umd_handle_delete(cli_table_t *table, int argc, char *argv[])
{
    return umd_handle_mac(umd_user_delete, argc, argv);
}

/*
* block {mac}
*/
STATIC int
umd_handle_block(cli_table_t *table, int argc, char *argv[])
{
    int block(byte mac[])
    {
        umd_user_t *user = umd_user_block(mac);

        return user?0:-ENOMEM;
    }
    
    return umd_handle_mac(block, argc, argv);
}

/*
* unblock {mac}
*/
STATIC int
umd_handle_unblock(cli_table_t *table, int argc, char *argv[])
{
    return umd_handle_mac(umd_user_unblock, argc, argv);
}

/*
* associate {mac} {ath}
*/
STATIC int
umd_handle_associate(cli_table_t *table, int argc, char *argv[])
{
    char *mac   = argv[1];
    char *ath   = argv[2];
    
    if (3!=argc) {
        return cli_help(-EFORMAT);
    }
    else if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return cli_help(-EFORMAT);
    }

    umd_user_t *user = umd_user_create(os_mac(mac));
    if (NULL==user) {
        return -ENOMEM;
    }

    // todo: save ath
    
    return 0;
}

/*
* diassociate {mac}
*/
STATIC int
umd_handle_diassociate(cli_table_t *table, int argc, char *argv[])
{
    return umd_handle_mac(umd_user_delete, argc, argv);
}

/*
* bind {mac} {ip}
*/
STATIC int
umd_handle_bind(cli_table_t *table, int argc, char *argv[])
{
    return umd_handle_mac_ip(umd_user_bind, argc, argv);
}

/*
* unbind {mac}
*/
STATIC int
umd_handle_unbind(cli_table_t *table, int argc, char *argv[])
{
    return umd_handle_mac(umd_user_unbind, argc, argv);
}

/*
* fake {mac} {ip}
*/
STATIC int
umd_handle_fake(cli_table_t *table, int argc, char *argv[])
{
    if (UMD_FAKEABLE==umd.cfg.fakeable) {
        return umd_handle_mac_ip(umd_user_fake, argc, argv);
    } else {
        return -ENOSUPPORT;
    }
}

/*
* unfake {mac}
*/
STATIC int
umd_handle_unfake(cli_table_t *table, int argc, char *argv[])
{
    if (UMD_FAKEABLE==umd.cfg.fakeable) {
        return umd_handle_mac(umd_user_unfake, argc, argv);
    } else {
        return -ENOSUPPORT;
    }
}

/*
* auth {mac} {group} {json}
*/
STATIC int
umd_handle_auth(cli_table_t *table, int argc, char *argv[])
{
    char *mac   = argv[1];
    char *group = argv[2];
    char *json  = argv[3];
    jobj_t obj = NULL;
    
    if (4!=argc) {
        return cli_help(-EFORMAT);
    }
    else if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return cli_help(-EFORMAT);
    }
    else if (false==is_good_json(json)) {
        debug_trace("bad json %s", json);

        return cli_help(-EFORMAT);
    }
    
    int groupid = os_atoi(group);
    int err = 0;
    
    obj = jobj_byjson(json);
    if (NULL==obj) {
        debug_trace("bad json %s", json);

        err = cli_help(-EBADJSON); goto error;
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
umd_handle_deauth(cli_table_t *table, int argc, char *argv[])
{
    int deauth(byte mac[])
    {
        umd_user_deauth(mac, UMD_DEAUTH_ADMIN);

        return 0;
    }
    
    return umd_handle_mac(deauth, argc, argv);
}

/*
* reauth {mac}
*/
STATIC int
umd_handle_reauth(cli_table_t *table, int argc, char *argv[])
{
    if (umd.cfg.reauthable) {
        return umd_handle_mac(umd_user_reauth, argc, argv);
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
        err = cli_help(-EFILTER); goto error;
    }
    
    if (NULL!=(jmac = jobj_get(obj, "mac"))) {
        string = jobj_get_string(jmac);
        if (false==is_good_macstring(string)) {
            err = cli_help(-EBADMAC); goto error;
        }

        user = umd_user_get(os_mac(string));
    }
    else if (NULL!=(jip = jobj_get(obj, "ip"))) {
        string = jobj_get_string(jip);
        if (false==is_good_ipstring(string)) {
            err = cli_help(-EBADIP); goto error;
        }

        user = umd_user_getbyip(os_ipaddr(string));
    }
    else {
        err = cli_help(-EFILTER); goto error;
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
    jobj_add_i32(obj, "user", h2_count(&umd.head.user));
    jobj_add(obj, "flow", umd_jflow());

    cli_sprintf(__tab "%s" __crlf, jobj_json(obj));
    
    jobj_put(obj);
    
    return 0;
}

STATIC int
umd_show_count(void)
{
    cli_sprintf("%d" __crlf, h2_count(&umd.head.user));
    
    return 0;
}

/*
* show [json]
*/
STATIC int
umd_handle_show(cli_table_t *table, int argc, char *argv[])
{
    switch(argc) {
        case 1:
            return umd_user_foreach(umd_show_user, false);
        case 2: {
            char *json = argv[1];

            if (os_streq("stat", json)) {
                return umd_show_stat();
            }
            else if (os_streq("count", json)) {
                return umd_show_count();
            }
            else if (is_good_json(json)) {
                return umd_show_user_byjson(json);
            }
            else {
                return cli_help(-EHELP);
            }
        }
        default:
            return cli_help(-EHELP);
    }
}

/*
* sync {mac} {json}
*/
STATIC int
umd_handle_sync(cli_table_t *table, int argc, char *argv[])
{
    if (umd.cfg.syncable) {
        return umd_handle_mac_json(umd_user_sync, argc, argv);
    } else {
        return -ENOSUPPORT;
    }
}

/*
* tag {mac} {key} [value]
*/
STATIC int
umd_handle_tag(cli_table_t *table, int argc, char *argv[])
{
    char *mac   = argv[1];
    char *k     = argv[2];
    char *v     = argv[3];

    if (3!=argc && 4!=argc) {        
        return cli_help(-EFORMAT);
    }
    else if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return cli_help(-EFORMAT);
    }

    umd_user_tag_t *tag = NULL;
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
umd_handle_gc(cli_table_t *table, int argc, char *argv[])
{
    char *target = argv[1];

    int id = umd_gc_target_getidbyname(target);

    switch(id) {
        case umd_gc_target_user:
            if (umd.cfg.gcuser) {
                return umd_user_foreach(umd_user_gc, true);
            } else {
                return -ENOSUPPORT;
            }

        case umd_gc_target_conn:
            if (umd.cfg.connectable && umd.cfg.gcconn) {
                return umd_conn_foreach(umd_conn_gc, true);
            } else {
                return -ENOSUPPORT;
            }
        default:
            return cli_help(-EFORMAT);
    }
}

STATIC int
umd_cli(loop_watcher_t *watcher, time_t now)
{
    static cli_table_t tables[] = {
        /*
        * help must first
        */
        CLI_TCP_ENTRY("help",   cli_handle_help),
        
        CLI_TCP_ENTRY("create", umd_handle_create),
        CLI_TCP_ENTRY("delete", umd_handle_delete),
        
        CLI_TCP_ENTRY("block",  umd_handle_block),
        CLI_TCP_ENTRY("unblock",umd_handle_unblock),
        
        CLI_TCP_ENTRY("associate",      umd_handle_associate),
        CLI_TCP_ENTRY("diassociate",    umd_handle_diassociate),
        
        CLI_TCP_ENTRY("bind",   umd_handle_bind),
        CLI_TCP_ENTRY("unbind", umd_handle_unbind),
        
        CLI_TCP_ENTRY("fake",   umd_handle_fake),
        CLI_TCP_ENTRY("unfake", umd_handle_unfake),
        
        CLI_TCP_ENTRY("auth",   umd_handle_auth),
        CLI_TCP_ENTRY("deauth", umd_handle_deauth),
        CLI_TCP_ENTRY("reauth", umd_handle_reauth),
        
        CLI_TCP_ENTRY("sync",   umd_handle_sync),
        CLI_TCP_ENTRY("show",   umd_handle_show),
        CLI_TCP_ENTRY("tag",    umd_handle_tag),
        CLI_TCP_ENTRY("gc",     umd_handle_gc),
    };

    return cli_response(watcher->fd, tables);
}

STATIC int
umd_cli_init(sock_server_t *server)
{
    int err;

    err = os_loop_cli_tcp(&umd.loop, umd_cli, server);
    if (err<0) {
        debug_ok("add loop cli error:%d", err);
        
        return err;
    }
    
    debug_ok("init cli server ok");
    
    return 0;
}

sock_server_t umd_cli_server = 
    SOCK_USERVER_INITER(UMD_SERVER_CLI, "umd", umd_cli_init, NULL);
/******************************************************************************/
