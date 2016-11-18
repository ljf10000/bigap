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
typedef void um_timer_handle_t(struct um_user *user, time_t now);

STATIC void 
__try_aging(struct um_user *user, int type)
{
    if (__online_idle(user, type)) {
        __online_aging(user, type) -= umd.cfg.ticks;
        
        if (__online_aging(user, type) <= 0) {
            debug_timeout("user(%s) type(%s) online aging",
                os_macstring(user->mac),
                flow_type_string(type));
            
            user_unbind(user, UM_DEAUTH_AGING);
        }
    }
}

STATIC void 
online_aging(struct um_user *user, time_t now)
{
    (void)now;
    
    if (is_user_have_bind(user)) {
        __try_aging(user, um_flow_type_wan);
        __try_aging(user, um_flow_type_lan);
    }
}

STATIC bool
__is_gc(struct um_user *user, time_t now)
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

int umd_gc(struct um_user *user)
{
    if (is_user_noused(user)) {
        um_user_debug("gc", user, __is_ak_debug_gc);
        
        user_delete(user);
    }

    return 0;
}

STATIC void
gc_auto(struct um_user *user, time_t now)
{
    if (__is_gc(user, now)) {
        umd_gc(user);
    }
}

STATIC bool
__is_online_timeout(struct um_user *user, time_t now, int type)
{
    time_t max      = __online_max(user, type);
    time_t uptime   = __online_uptime(user, type);

    bool is = max && (uptime < now) && (now - uptime > max);
    if (is) {
        debug_timeout("user(%s) type(%s) max(%u) uptime(%u) now(%u) online timeout",
            os_macstring(user->mac),
            flow_type_string(type),
            max,
            uptime,
            now);
    }

    return is;
}

STATIC bool
is_online_timeout(struct um_user *user, time_t now)
{
    return __is_online_timeout(user, now, um_flow_type_wan)
        || __is_online_timeout(user, now, um_flow_type_lan);
}

STATIC void 
online_timeout(struct um_user *user, time_t now)
{
    /*
    * online timeout
    *   just for auth user
    */
    if (is_user_auth(user) && is_online_timeout(user, now)) {        
        user_deauth(user, UM_DEAUTH_ONLINETIME);
    }
}

STATIC bool
__is_online_reauth(struct um_user *user, time_t now, int type)
{
    time_t uptime   = __online_uptime(user, type);
    time_t reauth   = __online_reauth(user, type);
    
    bool is = reauth && (uptime < now) && (now - uptime > reauth);
    if (is) {
        debug_timeout("user(%s) type(%s) reauth(%u) uptime(%u) now(%u) online reauth",
            os_macstring(user->mac),
            flow_type_string(type),
            reauth,
            uptime,
            now);
    }

    return is;
}

STATIC bool
is_online_reauth(struct um_user *user, time_t now)
{
    return __is_online_reauth(user, now, um_flow_type_wan)
        || __is_online_reauth(user, now, um_flow_type_lan);
}

STATIC void 
online_reauth(struct um_user *user, time_t now)
{
    /*
    * online reauth
    *   just for auth user
    */
    if (umd.cfg.reauthable 
            && is_user_auth(user) 
            && is_online_reauth(user, now)) {        
        user_reauth(user);
    }
}

STATIC bool
is_fake_timeout(struct um_user *user, time_t now)
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
fake_timeout(struct um_user *user, time_t now)
{
    if (is_user_fake(user) && is_fake_timeout(user, now)) {        
        user_unfake(user, UM_DEAUTH_ONLINETIME);
    }
}

STATIC void
timer_handle(struct um_user *user, time_t now)
{
    static um_timer_handle_t *handler[] = {
        fake_timeout,
        //online_reauth,
        //online_timeout,
        //online_aging,
        
        // gc_auto, // keep last
    };
    
    int i;

    for (i=0; i<os_count_of(handler); i++) {
        (*handler[i])(user, now);
    }
}

STATIC int
timer_server_init(sock_server_t *server)
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
timer_server_handle(sock_server_t *server)
{
    uint32 times = tm_fd_read(server->fd);
    time_t now = time(NULL);
    int i;

    mv_t cb(struct um_user *user)
    {
        //os_println("timer user[%s]=%p", os_macstring(user->mac), user);
        
        timer_handle(user, now);

        return mv2_ok;
    }
    
    for (i=0; i<times; i++) {
        umd.ticks++;
        
        um_user_foreach(cb, true);
    }

    return 0;
}

sock_server_t um_timer_server = 
    SOCK_SERVER_INITER(UM_SERVER_TIMER, 0, timer_server_init, timer_server_handle);
/******************************************************************************/
