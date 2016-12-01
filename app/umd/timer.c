/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      umd
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     timer
#endif

#define __DEAMON__
#include "umd.h"

typedef void um_timer_handle_t(umd_user_t *user, time_t now);

STATIC void 
umd_try_aging(umd_user_t *user, int type)
{
    if (umd_online_idle(user, type)) {
        umd_online_aging(user, type) -= umd.cfg.ticks;
        
        if (umd_online_aging(user, type) <= 0) {
            debug_timeout("user(%s) type(%s) online aging",
                os_macstring(user->mac),
                umd_flow_type_getnamebyid(type));
            
            umduser_unbind(user, UMD_DEAUTH_AGING);
        }
    }
}

STATIC void 
umd_timer_online_aging(umd_user_t *user, time_t now)
{
    (void)now;
    
    if (is_user_have_bind(user)) {
        umd_try_aging(user, umd_flow_type_wan);
        umd_try_aging(user, umd_flow_type_lan);
    }
}

STATIC bool
is_umd_gc(umd_user_t *user, time_t now)
{
    time_t noused = user->noused;

    bool is = umd.cfg.gc && (noused < now) && (now - noused > umd.cfg.gc);
    if (is) {
        debug_gc("user(%s) gc(%u) noused(%u) now(%u) online gc",
            os_macstring(user->mac),
            umd.cfg.gc,
            noused,
            now);
    }

    return is;
}

int umd_gc(umd_user_t *user)
{
    if (is_user_noused(user)) {
        umd_user_debug_helper("gc", user, __is_ak_debug_gc);
        
        umduser_delete(user);
    }

    return 0;
}

STATIC void
umd_timer_gc_auto(umd_user_t *user, time_t now)
{
    if (is_umd_gc(user, now)) {
        umd_gc(user);
    }
}

STATIC bool
__is_umd_timer_online_timeout(umd_user_t *user, time_t now, int type)
{
    time_t max      = umd_online_max(user, type);
    time_t uptime   = umd_online_uptime(user, type);

    bool is = max && (uptime < now) && (now - uptime > max);
    if (is) {
        debug_timeout("user(%s) type(%s) max(%u) uptime(%u) now(%u) online timeout",
            os_macstring(user->mac),
            umd_flow_type_getnamebyid(type),
            max,
            uptime,
            now);
    }

    return is;
}

STATIC bool
is_umd_timer_online_timeout(umd_user_t *user, time_t now)
{
    return __is_umd_timer_online_timeout(user, now, umd_flow_type_wan)
        || __is_umd_timer_online_timeout(user, now, umd_flow_type_lan);
}

STATIC void 
umd_timer_online_timeout(umd_user_t *user, time_t now)
{
    /*
    * online timeout
    *   just for auth user
    */
    if (is_user_auth(user) && is_umd_timer_online_timeout(user, now)) {        
        umduser_deauth(user, UMD_DEAUTH_ONLINETIME);
    }
}

STATIC bool
__is_umd_timer_online_reauth(umd_user_t *user, time_t now, int type)
{
    time_t uptime   = umd_online_uptime(user, type);
    time_t reauth   = umd_online_reauthor(user, type);
    
    bool is = reauth && (uptime < now) && (now - uptime > reauth);
    if (is) {
        debug_timeout("user(%s) type(%s) reauth(%u) uptime(%u) now(%u) online reauth",
            os_macstring(user->mac),
            umd_flow_type_getnamebyid(type),
            reauth,
            uptime,
            now);
    }

    return is;
}

STATIC bool
is_umd_timer_online_reauth(umd_user_t *user, time_t now)
{
    return __is_umd_timer_online_reauth(user, now, umd_flow_type_wan)
        || __is_umd_timer_online_reauth(user, now, umd_flow_type_lan);
}

STATIC void 
umd_timer_online_reauth(umd_user_t *user, time_t now)
{
    /*
    * online reauth
    *   just for auth user
    */
    if (umd.cfg.reauthable 
            && is_user_auth(user) 
            && is_umd_timer_online_reauth(user, now)) {        
        umduser_reauth(user);
    }
}

STATIC bool
is_umd_timer_fake_timeout(umd_user_t *user, time_t now)
{
    time_t faketime = user->faketime;
    uint32 fake = umd.cfg.fake;
    
    bool is = fake && (faketime < now) && (now - faketime > umd.cfg.fake);
    if (is) {
        debug_timeout("user(%s) faketime(%u) now(%u) timeout",
            os_macstring(user->mac),
            faketime,
            now);
    }

    return is;
}

STATIC void 
umd_timer_fake_timeout(umd_user_t *user, time_t now)
{
    if (is_user_fake(user) && is_umd_timer_fake_timeout(user, now)) {        
        umduser_unfake(user, UMD_DEAUTH_ONLINETIME);
    }
}

STATIC void
umd_timer_handle(umd_user_t *user, time_t now)
{
    static um_timer_handle_t *handler[] = {
        umd_timer_fake_timeout,
        umd_timer_online_reauth,
        umd_timer_online_timeout,
        umd_timer_online_aging,
        
        umd_timer_gc_auto, // keep last
    };
    
    int i;

    for (i=0; i<os_count_of(handler); i++) {
        (*handler[i])(user, now);
    }
}

STATIC int
umd_timer_server_init(sock_server_t *server)
{    
    int fd = tm_fd(os_second(1000*umd.cfg.ticks), os_nsecond(1000*umd.cfg.ticks));
    if (fd<0) {
        return fd;
    }
    server->fd = fd;
    
    debug_ok("init timer server ok");
    
    return 0;
}

STATIC int
umd_timer_server_handle(sock_server_t *server)
{
    uint32 times = tm_fd_read(server->fd);
    time_t now = time(NULL);
    int i;
    
    mv_t cb(umd_user_t *user)
    {
        umd_timer_handle(user, now);

        return mv2_ok;
    }
    
    for (i=0; i<times; i++) {
        umd.ticks++;
        
        umd_user_foreach(cb, true);
    }

    return 0;
}

sock_server_t umd_timer_server = 
    SOCK_SERVER_INITER(UMD_SERVER_TIMER, 0, umd_timer_server_init, umd_timer_server_handle);
/******************************************************************************/
