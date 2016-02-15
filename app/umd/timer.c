#ifndef __THIS_APP
#define __THIS_APP      umd
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     "timer"
#endif

#define __DEAMON__
#include "umd.h"

typedef mv_t um_timer_handle_t(struct um_user *user, time_t now);

static inline void 
__try_aging(struct um_user *user, int type)
{
    if (__online_idle(user, type)) {
        __online_aging(user, type) -= UM_TICKS;
        
        if (__online_aging(user, type) <= 0) {
            debug_timeout("user(%s) type(%s) online aging",
                os_macstring(user->mac),
                flow_type_string(type));
            
            user_unbind(user, UM_DEAUTH_AGING);
        }
    }
}

static mv_t 
online_aging(struct um_user *user, time_t now)
{
    (void)now;
    
    if (have_bind(user)) {
        __try_aging(user, um_flow_type_wan);
        __try_aging(user, um_flow_type_lan);
    }
    
    return mv2_ok;
}

#if UM_USE_GC
static inline bool
__is_gc(struct um_user *user, time_t now)
{
    time_t noused = user->noused;

    bool is = (noused < now) && (now - noused > umd.gc);
    if (is) {
        debug_timeout("user(%s) gc(%u) noused(%u) now(%u) online gc",
            os_macstring(user->mac),
            umd.gc,
            noused,
            now);
    }

    return is;
}

mv_t 
umd_gc(struct um_user *user, time_t now)
{
    if (is_noused(user) && __is_gc(user, now)) {
        user_delete(user);
    }

    return mv2_ok;
}
#endif

static inline bool
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

static inline bool
is_online_timeout(struct um_user *user, time_t now)
{
    return __is_online_timeout(user, now, um_flow_type_wan)
        || __is_online_timeout(user, now, um_flow_type_lan);
}

static mv_t 
online_timeout(struct um_user *user, time_t now)
{
    /*
    * online timeout
    *   just for auth user
    */
    if (is_auth(user) && is_online_timeout(user, now)) {        
        user_deauth(user, UM_DEAUTH_ONLINETIME);
    }

    return mv2_ok;
}

static inline bool
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

static inline bool
is_online_reauth(struct um_user *user, time_t now)
{
    return __is_online_reauth(user, now, um_flow_type_wan)
        || __is_online_reauth(user, now, um_flow_type_lan);
}

static mv_t 
online_reauth(struct um_user *user, time_t now)
{
    /*
    * online reauth
    *   just for auth user
    */
    if (is_auth(user) && is_online_reauth(user, now)) {        
        user_reauth(user);
    }

    return mv2_ok;
}

static mv_t
timer_handle(struct um_user *user, time_t now)
{
    static um_timer_handle_t *table[] = {
        online_reauth,
        online_timeout,
        online_aging,
#if UM_USE_GC
        umd_gc, /* must last */
#endif
    };
    mv_u mv;
    int i;

    for (i=0; i<os_count_of(table); i++) {
        mv.v = (*table[i])(user, now);
        if (is_mv2_break(mv)) {
            return mv.v;
        }
    }

    return mv2_ok;
}

static void
timer_env_init(simpile_server_t *server)
{
    umd.gc = get_umd_gc_env();
}

static int
timer_server_init(simpile_server_t *server)
{
    timer_env_init(server);
    
    int fd = tm_fd(os_second(1000*UM_TICKS), os_nsecond(1000*UM_TICKS));
    if (fd<0) {
        return fd;
    }
    server->fd = fd;
    
    debug_ok("init timer server ok");
    
    return 0;
}

static int
timer_server_handle(simpile_server_t *server)
{
    uint32_t times = tm_fd_read(server->fd);
    time_t now = time(NULL);
    int i;
    
    mv_t cb(struct um_user *user)
    {
        return timer_handle(user, now);
    }
    
    for (i=0; i<times; i++) {
        umd.ticks++;
        
        um_user_foreach_safe(cb);
    }

    return 0;
}

simpile_server_t um_timer_server = {
    .fd     = INVALID_FD,

    .init   = timer_server_init,
    .handle = timer_server_handle,
};

/******************************************************************************/
