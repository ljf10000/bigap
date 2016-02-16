#ifndef __THIS_APP
#define __THIS_APP      umd
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     "cli"
#endif

#define __DEAMON__
#define __SIMPILE_RES__

#include "umd.h"

DECLARE_SIMPILE_RES;

/*
* online
*/
static int
handle_online(char *args)
{
    return 0;
}

/*
* offline
*/
static int
handle_offline(char *args)
{
    return 0;
}

#if UM_USE_MONITOR
/*
* enter {mac} {json}
*/
static int
handle_enter(char *args)
{
    char *mac   = args; os_cmd_shift(args);
    char *json  = args; os_cmd_shift(args);
    
    if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return -EFORMAT;
    }
        
    jobj_t obj = jobj(json);
    if (NULL==obj) {
        debug_trace("bad json %s", json);

        return -EFORMAT;
    }

    struct um_user *user = um_user_enter(os_mac(mac), obj);
    if (NULL==user) {
        return -ENOEXIST;
    }
    
    jobj_put(obj);
    
    return 0;
}

/*
* leave {mac}
*/
static int
handle_leave(char *args)
{
    char *mac = args; os_cmd_shift(args);

    if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return -EFORMAT;
    }
    
    um_user_leave(os_mac(mac));
    
    return 0;
}
#endif

/*
* bind {mac} {ip}
*/
static int
handle_bind(char *args)
{
    char *mac   = args; os_cmd_shift(args);
    char *ip    = args; os_cmd_shift(args);

    if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return -EFORMAT;
    }
    else if (false==is_good_ipstring(ip)) {
        debug_trace("bad ip %s", ip);

        return -EFORMAT;
    }

    um_user_bind(os_mac(mac), inet_addr(ip));

    return 0;
}

/*
* unbind {mac}
*/
static int
handle_unbind(char *args)
{
    char *mac = args; os_cmd_shift(args);

    if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return -EFORMAT;
    }
    
    um_user_unbind(os_mac(mac));
    
    return 0;
}

/*
* auth {mac} {group} {json}
*/
static int
handle_auth(char *args)
{
    char *mac   = args; os_cmd_shift(args);
    char *group = args; os_cmd_shift(args);
    char *json  = args; os_cmd_shift(args);
    
    if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return -EFORMAT;
    }
    
    int groupid = os_atoi(group);
    
    jobj_t obj = jobj(json);
    if (NULL==obj) {
        debug_trace("bad json %s", json);

        return -EFORMAT;
    }

    struct um_user *user = um_user_auth(os_mac(mac), groupid, obj);
    if (NULL==user) {
        return -ENOEXIST;
    }
    
    jobj_put(obj);
    
    return 0;
}

/*
* reauth {mac}
*/
static int
handle_reauth(char *args)
{
    char *mac = args; os_cmd_shift(args);
    
    if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return -EFORMAT;
    }
    
    um_user_reauth(os_mac(mac));
    
    return 0;
}

/*
* deauth {mac}
*/
static int
handle_deauth(char *args)
{
    char *mac   = args; os_cmd_shift(args);
    
    if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return -EFORMAT;
    }
    
    um_user_deauth(os_mac(mac), UM_DEAUTH_ADMIN);
    
    return 0;
}

static mv_t
show_user(struct um_user *user)
{
    jobj_t obj = um_juser(user);
    
    if (obj) {
        simpile_res_sprintf(__tab "%s" __crlf, jobj_string(obj));

        jobj_put(obj);
    }
    
    return mv2_ok;
}

static int
show_user_byjson(char *json)
{
    jobj_t obj = jobj(json);
    if (NULL==obj) {
        return -EFILTER;
    }
    
    jobj_t jmac = jobj_get(obj, "mac");
    if (NULL==obj) {
        return -EFILTER;
    }
    
    char *macstring = jobj_get_string(jmac);
    if (false==is_good_macstring(macstring)) {
        return -EBADMAC;
    }

    struct um_user *user = um_user_get(os_mac(macstring));
    if (NULL==user) {
        return -ENOEXIST;
    }

    jobj_t juser = um_juser(user);
    if (NULL==juser) {
        return -ENOEXIST;
    }
    
    simpile_res_sprintf("%s", jobj_string(juser));
    jobj_put(juser);
    jobj_put(obj);
    
    return 0;
}

static int
show_stat(void)
{
    jobj_t obj = jobj_new_object();
    if (NULL==obj) {
        return -ENOMEM;
    }

    jobj_add_string(obj, "mac", os_macstring(umd.basemac));
    jobj_add_int(obj, "user", h2_count(&umd.table));
    jobj_add(obj, "flow", um_jflow());

    simpile_res_sprintf("%s" __crlf, jobj_string(obj));
    
    jobj_put(obj);
    
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
        return um_user_foreach(show_user);
    }
    else if (is_good_json(json)) {
        return show_user_byjson(json);
    }
    else if (0==os_strcmp("stat", json)) {
        return show_stat();
    }
    else {
        return -EFORMAT;
    }
}

/*
* tag {mac} {key} [value]
*/
static int
handle_tag(char *args)
{
    char *mac   = args; os_cmd_shift(args);
    char *k     = args; os_cmd_shift(args);
    char *v     = args; os_cmd_shift(args);

    if (false==is_good_macstring(mac)) {
        debug_trace("bad mac %s", mac);

        return -EFORMAT;
    }

    struct um_tag *tag = NULL;
    if (NULL==v) {
        tag = um_tag_get(os_mac(mac), k);
        if (tag) {
            simpile_res_sprintf("%s" __crlf, tag->v);
        }
    } else {
        tag = um_tag_set(os_mac(mac), k, v);
    }
    
    return tag?0:-ENOEXIST;
}

#if UM_USE_GC
/*
* gc
*/
static int
handle_gc(char *args)
{
    time_t now = time(NULL);
    
    mv_t cb(struct um_user *user)
    {
        return umd_gc(user, now);
    }
    
    um_user_foreach_safe(cb);
    
    return 0;
}
#endif

static cmd_table_t cli_table[] = {
#if UM_USE_MONITOR
    CMD_ENTRY("enter",  handle_enter),
    CMD_ENTRY("leave",  handle_leave),
#endif
    CMD_ENTRY("bind",   handle_bind),
    CMD_ENTRY("unbind", handle_unbind),
    CMD_ENTRY("auth",   handle_auth),
    CMD_ENTRY("reauth", handle_reauth),
    CMD_ENTRY("deauth", handle_deauth),
    CMD_ENTRY("show",   handle_show),
    CMD_ENTRY("tag",    handle_tag),
#if UM_USE_GC
    CMD_ENTRY("gc",     handle_gc),
#endif
};

static int
cli_server_handle(simpile_server_t *server)
{
    return simpile_d_handle(server->fd, cli_table);
}

static int
cli_env_init(simpile_server_t *server)
{
    int err;
    
    err = get_umd_path_env(&server->addr.un);
    if (err) {
        return err;
    }

    return 0;
}

static int
cli_server_init(simpile_server_t *server)
{
    int err;
    
    err = cli_env_init(server);
    if (err) {
        return err;
    }

    err = simpile_u_server_init(server);
    if (err) {
        return err;
    }
    
    debug_ok("init cli server ok");
    
    return 0;
}

simpile_server_t um_cli_server = {
    .fd     = INVALID_FD,
    .addr   = OS_SOCKADDR_INITER(AF_UNIX),

    .init   = cli_server_init,
    .handle = cli_server_handle,
};
/******************************************************************************/
