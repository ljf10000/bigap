/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      umd
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     user
#endif

#define __DEAMON__
#include "umd.h"
/******************************************************************************/
umd_limit_t *
umd_limit_get(umd_user_t *user, int type)
{
    return &user->limit[type];
}

umd_limit_online_t *
umd_limit_onliner(umd_user_t *user, int type)
{
    return &umd_limit_get(user, type)->online;
}

umd_limit_flow_t *
umd_limit_flower(umd_user_t *user, int type, int dir)
{
    return &umd_limit_get(user, type)->flow[dir];
}

umd_limit_rate_t *
umd_limit_rater(umd_user_t *user, int type, int dir)
{
    return &umd_limit_get(user, type)->rate[dir];
}

void
umd_update_aging_helper(umd_user_t *user, int type, bool debug)
{
    umd_online_aging(user, type) = umd_online_idle(user, type);

    if (debug) {
        debug_timeout("update %s aging to %d",
            umd_flow_type_getnamebyid(type),
            umd_online_aging(user, type));
    }
}

void
umd_update_aging(umd_user_t *user, bool debug)
{
    umd_update_aging_helper(user, umd_flow_type_wan, debug);
    umd_update_aging_helper(user, umd_flow_type_lan, debug);
}

void
umd_user_dump(char *tag, umd_user_t *user)
{
    jobj_t obj = umd_juser(user);

    os_println("\t%s:%s", tag, jobj_json(obj));

    jobj_put(obj);
}

void
umd_user_debug_helper(char *tag, umd_user_t *user, bool debug)
{
    if (debug) {
        jobj_t obj = umd_juser(user);
        
        jdebug("%o", tag, obj);

        jobj_put(obj);
    }
}
/******************************************************************************/
#define umd_update_online_helper(_user, _obj, _TYPE, _field)    do{ \
    jobj_t o = jobj_get_leaf(_obj, #_TYPE, "online", #_field, NULL);\
                                                                    \
    umd_limit_online(_user, _TYPE)->_field = o?jobj_get_i32(o):0;   \
}while(0)

#define umd_update_online(_user, _obj, _field)      do{ \
    umd_update_online_helper(_user, _obj, lan, _field); \
    umd_update_online_helper(_user, _obj, wan, _field); \
}while(0)

#define umd_update_flow_helper(_user, _obj, _TYPE, _DIR, _field)    do{     \
    jobj_t o = jobj_get_leaf(_obj, #_TYPE, "flow", #_DIR, #_field, NULL);   \
                                                                            \
    umd_limit_flow(_user, _TYPE, _DIR)->_field = o?jobj_get_i64(o):0;       \
}while(0)

#define umd_updata_flow(_user, _obj, _field)    do{ \
    umd_update_flow_helper(_user, _obj, lan, up, _field);    \
    umd_update_flow_helper(_user, _obj, wan, up, _field);    \
    umd_update_flow_helper(_user, _obj, lan, down, _field);  \
    umd_update_flow_helper(_user, _obj, wan, down, _field);  \
    umd_update_flow_helper(_user, _obj, lan, all, _field);   \
    umd_update_flow_helper(_user, _obj, wan, all, _field);   \
}while(0)

#define umd_update_rate_helper(_user, _obj, _TYPE, _DIR, _field)    do{     \
    jobj_t o = jobj_get_leaf(_obj, #_TYPE, "rate", #_DIR, #_field, NULL);   \
                                                                            \
    umd_limit_rate(_user, _TYPE, _DIR)->_field = o?jobj_get_i32(o):0;       \
}while(0)

#define umd_updata_rate(_user, _obj, _field)    do{ \
    umd_update_rate_helper(_user, _obj, lan, up, _field);    \
    umd_update_rate_helper(_user, _obj, wan, up, _field);    \
    umd_update_rate_helper(_user, _obj, lan, down, _field);  \
    umd_update_rate_helper(_user, _obj, wan, down, _field);  \
    umd_update_rate_helper(_user, _obj, lan, all, _field);   \
    umd_update_rate_helper(_user, _obj, wan, all, _field);   \
}while(0)

STATIC void
umd_update_limit(umd_user_t *user, jobj_t obj)
{
    umd_update_online(user, obj, max);
    umd_update_online(user, obj, idle);
    umd_update_online(user, obj, numerator);
    umd_update_online(user, obj, denominator);
    
    umd_updata_flow(user, obj, max);
    umd_updata_flow(user, obj, numerator);
    umd_updata_flow(user, obj, denominator);
    
    umd_updata_rate(user, obj, max);
    umd_updata_rate(user, obj, avg);
}

void umd_update_limit_test(void)
{
#if UM_TEST & UM_TEST_JSON
    umd_ingress_t *ingress = &umd.cfg.instance.ingress[0];
    umd_user_t user = {
        .ip = ingress->ip,
        .head = { 
            .tag = DLIST_INITER(user.head.tag),
        },
    };
    os_maccpy(user.mac, ingress->mac);
    
    jobj_t obj = jobj_byfile("./auth.json");
    if (NULL==obj) {
        os_println("read auth.json failed");

        exit(1);
    }
    
    umd_update_limit(&user, obj);
    umd_update_aging(&user, true);
    jobj_put(obj);

    obj = umd_juser(&user);
    os_println("%s", jobj_json(obj));
    os_system("echo '%s' | jq .", jobj_json(obj));
    jobj_put(obj);
#endif
}

typedef int umd_event_cb_t(umd_user_t *user, char *action);

STATIC int
__umduser_ev(umd_user_t *user, char *action)
{
    jobj_t juser = umd_juser(user);
    if (NULL==juser) {
        return -ENOEXIST;
    }

    os_system("%s %s '%s' &", 
        umd.cfg.script_event,
        action,
        jobj_json(juser));
    jobj_put(juser);

    return 0;
}

STATIC int
__umduser_ev_call(umd_event_cb_t *ev, umd_user_t *user, char *action)
{
    int err = 0;
    
    if (ev) {
        err = (*ev)(user, action);
    }

    return err;
}

STATIC hash_idx_t
__umduser_hashmac(byte mac[])
{
    return os_macbkdr(mac) & (umd.cfg.machashsize - 1);
}

STATIC hash_idx_t
__umduser_haship(uint32 ip)
{
    return os_ipbkdr(ip) & (umd.cfg.iphashsize - 1);
}
#if 1
DECLARE_DB_H2(&umd.head.user, __umduser, umd_user_t, node.user);

static inline umd_user_t *
__umduser_h2_entry(h2_node_t *node);

static inline umd_user_t *
__umduser_hx_entry(hash_node_t *node, hash_idx_t nidx);

static inline int 
__umduser_foreach(mv_t (*foreach)(umd_user_t *user), bool safe);
#endif

STATIC hash_idx_t
umd_nodehashmac(hash_node_t *node)
{
    umd_user_t *user = __umduser_hx_entry(node, UM_USER_NIDX_MAC);

    return __umduser_hashmac(user->mac);
}

STATIC hash_idx_t
umd_nodehaship(hash_node_t *node)
{
    umd_user_t *user = __umduser_hx_entry(node, UM_USER_NIDX_IP);

    return __umduser_haship(user->ip);
}

STATIC void
umduser_tag_free(umd_user_tag_t *tag)
{
    if (tag) {
        os_free(tag->k);
        os_free(tag->v);
        os_free(tag);
    }
}

STATIC umd_user_tag_t *
umduser_tag_new(char *k, char *v)
{
    umd_user_tag_t *tag = (umd_user_tag_t *)os_zalloc(sizeof(*tag));
    if (NULL==tag) {
        return NULL;
    }

    tag->k = os_strdup(k);
    tag->v = os_strdup(v);

    if (NULL==tag->k || NULL==tag->v) {
        umduser_tag_free(tag); 

        return NULL;
    }
    
    return tag;
}

STATIC umd_user_tag_t *
umduser_tag_get(umd_user_t *user, char *k)
{
    if (NULL==user) {
        return NULL;
    }

    bool eq(dlist_node_t *p)
    {
        umd_user_tag_t *tag = dlist_entry(p, umd_user_tag_t, node.tag);

        return os_streq(k, tag->k);
    }
    
    dlist_node_t *node = dlist_find(&user->head.tag, eq);

    return dlist_entry(node, umd_user_tag_t, node.tag);
}

STATIC umd_user_tag_t *
umduser_tag_insert(umd_user_t *user, char *k, char *v, bool update_if_exist)
{
    umd_user_tag_t *tag = NULL;

    if (NULL==user) {
        return NULL;
    }

    tag = umduser_tag_get(user, k);
    if (NULL==tag) {
        tag = umduser_tag_new(k, v);
        if (NULL==tag) {
            return NULL;
        }

        dlist_add(&user->head.tag, &tag->node.tag);
    }
    else {
        if (false==update_if_exist) {
            return NULL;
        }
        else if (0!=os_strcmp(tag->v, v)) {
            os_free(tag->v);
            tag->v = os_strdup(v);
        }
    }

    return tag;
}

STATIC umd_user_tag_t *
umduser_tag_set(umd_user_t *user, char *k, char *v)
{
    return umduser_tag_insert(user, k, v, true);
}

STATIC void
umduser_tag_clear(umd_user_t *user)
{
    mv_t foreach(dlist_node_t *p)
    {
        umd_user_tag_t *tag = dlist_entry(p, umd_user_tag_t, node.tag);

        dlist_del(&user->head.tag, &tag->node.tag);
        
        umduser_tag_free(tag);

        return mv2_ok;
    }

    dlist_foreach_safe(&user->head.tag, foreach);
}

STATIC void
umd_flow_reset(umd_user_t *user, int type)
{
    int i;

    for (i=0; i<umd_flow_dir_end; i++) {
        umd_flow_now(user, type, i) = 0;
    }
}

STATIC void
umd_lan_online(umd_user_t *user)
{
    /*
    * lan, offline==>online
    *   1. update lan uptime
    *   2. clear  wan uptime
    *   3. clear lan/wan downtime
    *   4. reset lan/wan idle
    *   5. reset lan/wan flow
    *   6. reset tag
    */
    umd_online_uptime(user, umd_flow_type_lan)     = time(NULL);
    umd_online_uptime(user, umd_flow_type_wan)     = 0;
    
    umd_online_downtime(user, umd_flow_type_lan)   = 0;
    umd_online_downtime(user, umd_flow_type_wan)   = 0;

    umd_online_idle(user, umd_flow_type_lan) = umd.cfg.idle;
    umd_online_idle(user, umd_flow_type_wan) = umd.cfg.idle;
    
    umd_flow_reset(user, umd_flow_type_lan);
    umd_flow_reset(user, umd_flow_type_wan);

    umduser_tag_clear(user);
    
    debug_event("user %s lan online", os_macstring(user->mac));
}

STATIC void
umd_wan_online(umd_user_t *user)
{
    /*
    * wan, offline==>online
    *   1. update wan uptime
    *   2. clear lan/wan downtime
    *   3. reset wan flow
    *   4. reset tag
    */
    umd_online_uptime(user, umd_flow_type_wan)     = time(NULL);
    
    umd_online_downtime(user, umd_flow_type_lan)   = 0;
    umd_online_downtime(user, umd_flow_type_wan)   = 0;

    umd_flow_reset(user, umd_flow_type_wan);

    umduser_tag_clear(user);
    
    debug_event("user %s wan online", os_macstring(user->mac));
}

STATIC void
umd_lan_offline(umd_user_t *user)
{
    /*
    * lan, online==>offline
    *   1. update lan downtime
    *      and keep others
    */
    umd_online_downtime(user, umd_flow_type_lan)   = time(NULL);

    if (is_user_noused(user)) {
        user->noused = time(NULL);
    }

    debug_event("user %s lan offline", os_macstring(user->mac));
}

STATIC void
umd_wan_offline(umd_user_t *user)
{
    /*
    * wan, online==>offline
    *   1. update wan downtime
    *   2. clear  lan downtime
    *      and keep others
    */
    umd_online_downtime(user, umd_flow_type_wan)   = time(NULL);
    umd_online_downtime(user, umd_flow_type_lan)   = 0;
    
    debug_event("user %s wan offline", os_macstring(user->mac));
}

STATIC umd_user_t *
__umduser_remove(umd_user_t *user)
{
    int err;
    
    if (NULL==user) {
        debug_bug("user nil");
        
        return NULL;
    }
    /*
    * not in list
    */
    else if (false==in_hx_table(&user->node.user)) {
        debug_bug("user not in list");
        
        return user;
    }

    h2_del(&umd.head.user, &user->node.user);

    return user;
}

STATIC umd_user_t *
__umduser_insert(umd_user_t *user)
{
    static hash_node_calc_f *ncalc[UM_USER_NIDX_END] = {
        [UM_USER_NIDX_MAC]  = umd_nodehashmac, 
        [UM_USER_NIDX_IP]   = umd_nodehaship
    };
    
    if (NULL==user) {
        debug_bug("user nil");
        
        return NULL;
    }
    /*
    * have in list
    */
    else if (in_hx_table(&user->node.user)) {
        debug_bug("user have in list");
        
        __umduser_remove(user);
    }

    h2_add(&umd.head.user, &user->node.user, ncalc);
    
    return user;
}

STATIC void
umd_set_user_group(umd_user_t *user, int group)
{
    if (NULL==user) {
        debug_bug("user nil");
        
        return;
    }
    else if (group==user->group) {
        debug_bug("same group %d", group);
        
        return;
    }
    
    user->group = group;
}

STATIC void
umd_set_user_reason(umd_user_t *user, int reason)
{
    if (NULL==user) {
        debug_bug("user nil");
        
        return;
    }
    else if (reason==user->reason) {
        debug_bug("same reason %s", umd_deauth_reason_getnamebyid(reason));
        
        return;
    }

    debug_entry("user(%s) state(%s) reason change %s==>%s", 
        os_macstring(user->mac),
        umd_user_state_getnamebyid(user->state),
        umd_deauth_reason_getnamebyid(user->reason), 
        umd_deauth_reason_getnamebyid(reason));
    
    user->reason = reason;
}

STATIC void
umd_set_user_ip(umd_user_t *user, uint32 ip)
{
    if (NULL==user) {
        debug_bug("user nil");
        
        return;
    }
    else if (ip==user->ip) {
        debug_bug("same ip %s", unsafe_ipstring(ip));
        
        return;
    }
    
    __umduser_remove(user);
    user->ip = ip;
    __umduser_insert(user);
}

STATIC void
umd_set_user_state(umd_user_t *user, int state)
{
    if (NULL==user) {
        debug_bug("user nil");
        
        return;
    }
    else if (state==user->state) {
        debug_bug("same state %s", umd_user_state_getnamebyid(state));
        
        return;
    }

    debug_entry("user %s state change %s==>%s", 
        os_macstring(user->mac), 
        umd_user_state_getnamebyid(user->state), 
        umd_user_state_getnamebyid(state));

    user->state = state;
}

STATIC void
umd_user_debug(char *tag, umd_user_t *user)
{
    umd_user_debug_helper(tag, user, __is_ak_debug_entry && __is_ak_debug_event);
}

#define umd_user_ev_call(_ev, _user)     __umduser_ev_call(_ev, _user, __user_debug_call_tag)

#define umd_user_debug_call_helper(_pos, _tag, _user, _body)   do{ \
    char *__user_debug_call_tag = _tag;             \
    if (is_position_head(_pos)) {                   \
        umd_user_debug("before-user-" _tag, _user); \
    }                                               \
    _body                                           \
    if (is_position_tail(_pos)) {                   \
        umd_user_debug("after-user-" _tag, _user);  \
    }                                               \
}while(0)

#define umd_user_debug_head_call(_tag, _user, _body) \
    umd_user_debug_call_helper(OS_POSITION_HEAD, _tag, _user, _body)

#define umd_user_debug_tail_call(_tag, _user, _body) \
    umd_user_debug_call_helper(OS_POSITION_TAIL, _tag, _user, _body)

#define umd_user_debug_call(_tag, _user, _body) \
    umd_user_debug_call_helper(OS_POSITION_ALL, _tag, _user, _body)

STATIC int
__umduser_delete(umd_user_t *user, umd_event_cb_t *ev)
{
    if (NULL==user) {
        return -ENOEXIST;
    }

    umd_user_debug_head_call("delete", user, {
        umd_user_ev_call(ev, user);
        umduser_tag_clear(user);
        __umduser_remove(user);
        os_free(user);
    });

    return 0;
}

STATIC umd_user_t *
__umduser_create(byte mac[], umd_event_cb_t *ev)
{
    umd_user_t *user;

    user = (umd_user_t *)os_zalloc(sizeof(*user));
    if (NULL==user) {
        return NULL;
    }
    dlist_init(&user->head.tag);

    os_maccpy(user->mac, mac);

    umd_user_debug_tail_call("create", user, {
        umd_set_user_state(user, UMD_STATE_NONE);
        umd_lan_online(user);
        user->create = user->noused = time(NULL);
        __umduser_insert(user);
        umd_user_ev_call(ev, user);
    });
    
    return user;
}

STATIC int
__umduser_deauth(umd_user_t *user, int reason, umd_event_cb_t *ev)
{
    if (NULL==user) {
        return -ENOEXIST;
    }
    // must @auth
    else if (false==is_user_auth(user)) {
        return 0;
    }

    // auth==>bind
    umd_user_debug_call("deauth", user, {
        umd_set_user_reason(user, reason);
        umd_user_ev_call(ev, user);
        
        umd_set_user_state(user, UMD_STATE_BIND);
        umd_set_user_group(user, 0);
        umd_wan_offline(user);
    });

    return 0;
}

STATIC int
__umduser_unfake(umd_user_t *user, int reason, umd_event_cb_t *ev)
{
    if (NULL==user) {
        return -ENOEXIST;
    }
    // must @fake
    else if (false==is_user_fake(user)) {
        return 0;
    }

    // fake==>bind
    umd_user_debug_call("unfake", user, {
        umd_set_user_reason(user, reason);
        umd_user_ev_call(ev, user);
        
        umd_set_user_state(user, UMD_STATE_BIND);
        umd_wan_offline(user);
    });

    return 0;
}

STATIC int
__umduser_unbind(umd_user_t *user, int reason, umd_event_cb_t *ev)
{
    if (NULL==user) {
        return -ENOEXIST;
    }

    // try auth/fake==>bind
    __umduser_deauth(user, reason, __umduser_ev);
    __umduser_unfake(user, reason, __umduser_ev);

    // must @bind
    if (false==is_user_bind(user)) {
        return 0;
    }

    // bind==>none
    umd_user_debug_call("unbind", user, {
        umd_set_user_reason(user, reason);
        umd_user_ev_call(ev, user);
        
        umd_set_user_ip(user, 0);
        umd_set_user_state(user, UMD_STATE_NONE);
        umd_lan_offline(user);
    });

    return 0;
}

STATIC umd_user_t *
__umduser_block(umd_user_t *user, umd_event_cb_t *ev)
{
    if (NULL==user) {
        return NULL;
    }

    // try auth/fake/bind==>none
    __umduser_unbind(user, UMD_DEAUTH_BLOCK, __umduser_ev);

    // must @none
    if (false==is_user_none(user)) {
        return NULL;
    }

    // none==>block
    umd_user_debug_call("block", user, {
        umd_set_user_reason(user, UMD_DEAUTH_BLOCK);
        umd_user_ev_call(ev, user);
        
        umd_set_user_state(user, UMD_STATE_BLOCK);
    });

    return user;
}
/******************************************************************************/
STATIC umd_user_t *
__umduser_unblock(umd_user_t *user, umd_event_cb_t *ev)
{
    if (NULL==user) {
        return NULL;
    }
    // must @block
    else if (false==is_user_block(user)) {
        return NULL;
    }

    // block==>none
    umd_user_debug_call("unblock", user, {
        umd_set_user_state(user, UMD_STATE_NONE);
        
        umd_user_ev_call(ev, user);
    });

    return user;
}

STATIC umd_user_t *
__umduser_bind(umd_user_t *user, uint32 ip, umd_event_cb_t *ev)
{
    if (NULL==user) {
        return NULL;
    }
    // not support bind @block
    else if (is_user_block(user)) {
        return NULL;
    }
    else if (is_user_have_bind(user)) {
        if (ip==user->ip) {
            // same ip @bind/auth/fake, just return user
            return user;
        } else {
            // diff ip @bind/auth/fake, first unbind user(==>none)
            __umduser_unbind(user, UMD_DEAUTH_AUTO, __umduser_ev);
        }
    }

    // must @none
    if (false==is_user_none(user)) {
        return NULL;
    }
    
    // none==>bind
    umd_user_debug_call("bind", user, {
        umd_set_user_ip(user, ip);
        umd_set_user_state(user, UMD_STATE_BIND);
        umd_lan_online(user);
        umd_update_aging(user, true);
        umd_user_ev_call(ev, user);
    });

    return user;
}

STATIC umd_user_t *
__umduser_fake(umd_user_t *user, uint32 ip, umd_event_cb_t *ev)
{
    if (NULL==user) {
        return NULL;
    }
    // not support fake @block
    else if (is_user_block(user)) {
        return NULL;
    }
    else if (is_user_have_bind(user)) {
        if (ip==user->ip) {
            // same ip @bind/auth/fake, just return user
            return user;
        } else {
            // diff ip @bind/auth/fake, first unbind user(==>none)
            __umduser_unbind(user, UMD_DEAUTH_AUTO, __umduser_ev);
        }
    }
    
    umd_user_debug_call("fake", user, {
        umd_set_user_ip(user, ip);
        umd_set_user_state(user, UMD_STATE_FAKE);
        umd_lan_online(user);
        umd_update_aging(user, true);
        user->faketime = time(NULL);
        umd_user_ev_call(ev, user);
    });

    return user;
}

STATIC umd_user_t *
__umduser_reauth(umd_user_t *user, umd_event_cb_t *ev)
{
    if (NULL==user) {
        return NULL;
    }
    // not support reauth @block
    else if (is_user_block(user)) {
        return NULL;
    }
    // must @auth
    else if (false==is_user_auth(user)) {
        return NULL;
    }
    
    umd_user_debug_call("reauth", user, {
        umd_user_ev_call(ev, user);
    });

    return user;
}

STATIC umd_user_t *
__umduser_auth(umd_user_t *user, int group, jobj_t obj, umd_event_cb_t *ev)
{
    if (NULL==user) {
        return NULL;
    }
    // not support auth @block
    else if (is_user_block(user)) {
        return NULL;
    }
    else if (is_user_none(user)) {
        char ipaddress[1+OS_LINE_LEN];
        
        int err = os_v_pgets(ipaddress, OS_LINE_LEN, 
                        "%s %s", 
                        umd.cfg.script_getipbymac,
                        os_macstring(user->mac));
        if (err<0 || false==is_good_ipstring(ipaddress)) {
            // no found user mac @none
            return NULL;
        } else {
            // bind user @none
            __umduser_bind(user, os_ipaddr(ipaddress), __umduser_ev);
        }
    }
    
    // must @bind/fake/auth
    if (false==is_user_have_bind(user)) {
        return NULL;
    }
    
    umd_user_debug_call("auth", user, {
        umd_set_user_state(user, UMD_STATE_AUTH);
        umd_set_user_group(user, group);
        umd_set_user_reason(user, UMD_DEAUTH_NONE);
        umd_update_limit(user, obj);
        umd_wan_online(user);
        umd_update_aging(user, true);
        umd_user_ev_call(ev, user);
    });

    return user;
}

STATIC umd_user_t *
__umduser_sync(umd_user_t *user, jobj_t obj, umd_event_cb_t *ev)
{
    if (NULL==user) {
        return NULL;
    }
    
    umd_user_debug_call("sync", user, {
        umd_touser(user, obj);
        
        umd_user_ev_call(ev, user);
    });

    return user;
}

STATIC umd_user_t *
__umduser_get(byte mac[])
{
    hash_idx_t dcalc(void)
    {
        return __umduser_hashmac(mac);
    }
    
    bool eq(hash_node_t *node)
    {
        umd_user_t *user = __umduser_hx_entry(node, UM_USER_NIDX_MAC);
        
        return os_maceq(user->mac, mac);
    }
    
    return __umduser_h2_entry(h2_find(&umd.head.user, UM_USER_NIDX_MAC, dcalc, eq));
}

STATIC umd_user_t *
umduser_get(byte mac[])
{
    umd_user_t *user = __umduser_get(mac);
    if (NULL==user) {
        debug_entry("no-found user %s", os_macstring(mac));
    }

    return user;
}

STATIC umd_user_t *
umduser_create(byte mac[])
{
    return __umduser_create(mac, __umduser_ev);
}

int umduser_delete(umd_user_t *user)
{
    return __umduser_delete(user, __umduser_ev);
}

STATIC umd_user_t *
umduser_block(umd_user_t *user)
{
    return __umduser_block(user, __umduser_ev);
}

int umduser_unblock(umd_user_t *user)
{
    return __umduser_unblock(user, __umduser_ev)?0:-ENOPERM;
}

STATIC umd_user_t *
umduser_bind(umd_user_t *user, uint32 ip)
{
    return __umduser_bind(user, ip, __umduser_ev);
}

int umduser_unbind(umd_user_t *user, int reason)
{
    if (is_valid_umd_deauth_reason(reason)) {
        return __umduser_unbind(user, reason, __umduser_ev);
    } else {
        return -EBADREASON;
    }
}

STATIC umd_user_t *
umduser_fake(umd_user_t *user, uint32 ip)
{
    return __umduser_fake(user, ip, __umduser_ev);
}

int umduser_unfake(umd_user_t *user, int reason)
{
    if (is_valid_umd_deauth_reason(reason)) {
        return __umduser_unfake(user, reason, __umduser_ev);
    } else {
        return -EBADREASON;
    }
}

STATIC umd_user_t *
umduser_auth(umd_user_t *user, int group, jobj_t obj)
{
    return __umduser_auth(user, group, obj, __umduser_ev);
}

int umduser_deauth(umd_user_t *user, int reason)
{
    if (is_valid_umd_deauth_reason(reason)) {
        return __umduser_deauth(user, reason, __umduser_ev);
    } else {
        return -EBADREASON;
    }
}

umd_user_t *
umduser_reauth(umd_user_t *user)
{
    return __umduser_reauth(user, __umduser_ev);
}

STATIC umd_user_t *
umduser_sync(umd_user_t *user, jobj_t obj)
{
    return __umduser_sync(user, obj, __umduser_ev);
}

umd_user_tag_t *
umd_user_tag_get(byte mac[], char *k)
{
    return umduser_tag_get(umduser_get(mac), k);
}

umd_user_tag_t *
umd_user_tag_set(byte mac[], char *k, char *v)
{
    return umduser_tag_set(umduser_get(mac), k, v);
}

int umd_user_delete(byte mac[])
{
    return umduser_delete(umduser_get(mac));
}

umd_user_t *
umd_user_create(byte mac[])
{
    umd_user_t *user = umduser_get(mac);

    return user?user:umduser_create(mac);
}

int umd_user_diassociate(byte mac[])
{
    return umduser_delete(umduser_get(mac));
}

umd_user_t *
umd_user_associate(byte mac[], char *ath, char *ssid)
{
    umd_user_t *user = umduser_get(mac);
    if (user) {
        // check and save current bssid
    }
    
    user = umduser_create(mac);
    os_strcpy(user->ath, ath);
    
    // todo: save first bssid
    //guoshuai-ssid
    char buf[128];
    memset(buf,0,sizeof(buf));
    
    int len = strlen(ssid);
    memcpy(buf, ssid, len);
    user->ssid = strdup(buf);
    
    return user;
}

umd_user_t *
umd_user_block(byte mac[])
{
    return umduser_block(umd_user_create(mac));
}

int umd_user_unblock(byte mac[])
{
    return umduser_unblock(umduser_get(mac));
}

umd_user_t *
umd_user_bind(byte mac[], uint32 ip)
{
    return umduser_bind(umd_user_create(mac), ip);
}

int umd_user_unbind(byte mac[])
{
    return umduser_unbind(umduser_get(mac), UMD_DEAUTH_INITIATIVE);
}

umd_user_t *
umd_user_fake(byte mac[], uint32 ip)
{
    return umduser_fake(umd_user_create(mac), ip);
}

int umd_user_unfake(byte mac[])
{
    return umduser_unfake(umduser_get(mac), UMD_DEAUTH_INITIATIVE);
}

umd_user_t *
umd_user_auth(byte mac[], int group, jobj_t obj)
{
    return umduser_auth(umd_user_create(mac), group, obj);
}

int umd_user_deauth(byte mac[], int reason)
{
    return umduser_deauth(umduser_get(mac), reason);
}

int umd_user_reauth(byte mac[])
{
    return umduser_reauth(umduser_get(mac))?0:-ENOPERM;
}

umd_user_t *
umd_user_sync(byte mac[], jobj_t obj)
{
    return umduser_sync(umd_user_create(mac), obj);
}

int umd_user_foreach(mv_t (*foreach)(umd_user_t *user), bool safe)
{
    return __umduser_foreach(foreach, safe);
}

umd_user_t *
umd_user_get(byte mac[])
{
    return umduser_get(mac);
}

umd_user_t *
umd_user_getbyip(uint32 ip)
{
    hash_idx_t dcalc(void)
    {
        return __umduser_haship(ip);
    }
    
    bool eq(hash_node_t *node)
    {
        umd_user_t *user = __umduser_hx_entry(node, UM_USER_NIDX_IP);

        return ip==user->ip;
    }
    
    return __umduser_h2_entry(h2_find(&umd.head.user, UM_USER_NIDX_IP, dcalc, eq));
}

int umd_user_delbyip(uint32 ip)
{
    return umduser_delete(umd_user_getbyip(ip));
}

STATIC jobj_t
umd_juser_online(umd_user_t *user, int type)
{
    jobj_t obj = jobj_new_object();
    time_t time;

    jobj_add_u32(obj, "max",            umd_online_max(user, type));
    jobj_add_u32(obj, "idle",           umd_online_idle(user, type));
    jobj_add_i32(obj, "aging",          umd_online_aging(user, type));
    jobj_add_u32(obj, "numerator",      umd_online_numerator(user, type));
    jobj_add_u32(obj, "denominator",    umd_online_denominator(user, type));
    
    time = umd_online_uptime(user, type);
    if (time) {
        jobj_add_string(obj, "uptime", unsafe_fulltime_string(time));
    }

    time = umd_online_downtime(user, type);
    if (time) {
        jobj_add_string(obj, "downtime", unsafe_fulltime_string(time));
    }
    
    return obj;
}

STATIC jobj_t
umd_juser_flow_helper(umd_user_t *user, int type, int dir)
{
    jobj_t obj = jobj_new_object();
    
    jobj_add_u64(obj, "max",          umd_flow_max(user, type, dir));
    jobj_add_u64(obj, "now",          umd_flow_now(user, type, dir));
    jobj_add_u64(obj, "numerator",    umd_flow_numerator(user, type, dir));
    jobj_add_u64(obj, "denominator",  umd_flow_denominator(user, type, dir));

    return obj;
}

STATIC jobj_t
umd_juser_flow(umd_user_t *user, int type)
{
    jobj_t obj = jobj_new_object();
    int dir;

    for (dir=0; dir<umd_flow_dir_end; dir++) {
        jobj_add(obj, umd_flow_dir_getnamebyid(dir), umd_juser_flow_helper(user, type, dir));
    }

    return obj;
}

STATIC jobj_t
umd_juser_rate_helper(umd_user_t *user, int type, int dir)
{
    jobj_t obj = jobj_new_object();
    
    jobj_add_u32(obj, "max", umd_rate_max(user, type, dir));
    jobj_add_u32(obj, "avg", umd_rate_avg(user, type, dir));

    return obj;
}

STATIC jobj_t
umd_juser_rate(umd_user_t *user, int type)
{
    jobj_t obj = jobj_new_object();
    int dir;

    for (dir=0; dir<umd_flow_dir_end; dir++) {
        jobj_add(obj, umd_flow_dir_getnamebyid(dir), umd_juser_rate_helper(user, type, dir));
    }

    return obj;
}

STATIC jobj_t
umd_juser_limit_helper(umd_user_t *user, int type)
{
    jobj_t obj = jobj_new_object();

    jobj_add(obj, "online", umd_juser_online(user, type));
    jobj_add(obj, "flow", umd_juser_flow(user, type));
    jobj_add(obj, "rate", umd_juser_rate(user, type));

    return obj;
}

STATIC jobj_t
umd_juser_limit(umd_user_t *user)
{
    jobj_t obj = jobj_new_object();

    jobj_add(obj, umd_flow_type_getnamebyid(umd_flow_type_lan), umd_juser_limit_helper(user, umd_flow_type_lan));
    jobj_add(obj, umd_flow_type_getnamebyid(umd_flow_type_wan), umd_juser_limit_helper(user, umd_flow_type_wan));
    
    return obj;
}

STATIC jobj_t
umd_juser_tag(umd_user_t *user)
{
    jobj_t obj = jobj_new_object();

    mv_t foreach(dlist_node_t *node)
    {
        umd_user_tag_t *tag = dlist_entry(node, umd_user_tag_t, node.tag);

        jobj_add_string(obj, tag->k, tag->v);

        return mv2_ok;
    }

    dlist_foreach(&user->head.tag, foreach);

    return obj;
}

jobj_t umd_juser(umd_user_t *user)
{
    jobj_t obj = jobj_new_object();

    jobj_add_string(obj, "mac",             os_macstring(user->mac));
    jobj_add_string(obj, "bssid_first",     os_macstring(user->bssid_first));
    jobj_add_string(obj, "bssid_current",   os_macstring(user->bssid_current));
    jobj_add_string(obj, "ssid",    user->ssid);
    jobj_add_string(obj, "ip",      unsafe_ipstring(user->ip));
    jobj_add_string(obj, "state",   umd_user_state_getnamebyid(user->state));
    jobj_add_string(obj, "reason",  umd_deauth_reason_getnamebyid(user->reason));
    
    jobj_add_string(obj, "create",  unsafe_fulltime_string(user->create));
    jobj_add_string(obj, "noused",  unsafe_fulltime_string(user->noused));
    jobj_add_string(obj, "faketime",unsafe_fulltime_string(user->faketime));
    jobj_add_string(obj, "hitime",  unsafe_fulltime_string(user->hitime));
    
    jobj_add_i32(obj,   "group",    user->group);

    jobj_add(obj, "tag",    umd_juser_tag(user));
    jobj_add(obj, "limit",  umd_juser_limit(user));

    return obj;
}

STATIC void
umd_touser_base(umd_user_t *user, jobj_t juser)
{
    jj_mac(user, juser, mac);
    jj_mac(user, juser, bssid_first);
    jj_ip(user, juser, ip);

    jj_byeq(user, juser, state, umd_user_state_getidbyname);
    jj_byeq(user, juser, reason, umd_deauth_reason_getidbyname);

    jj_time(user, juser, create);
    jj_time(user, juser, noused);
    jj_time(user, juser, hitime);
    
    jj_i32(user, juser, group);
}

STATIC void
umd_touser_tag(umd_user_t *user, jobj_t juser)
{
    jobj_t jtag;
    
    jtag = jobj_get(juser, "tag");
    if (jtag) {
        jobj_foreach(jtag, k, v) {
            if (jtype_string==jobj_type(v)) {
                umduser_tag_set(user, k, jobj_get_string(v));
            }
        }
    }
}

STATIC void
umd_touser_flow(umd_limit_flow_t *flow, jobj_t jflow)
{
    jj_u64(flow, jflow, max);
    jj_u64(flow, jflow, now);
    jj_u64(flow, jflow, numerator);
    jj_u64(flow, jflow, denominator);
}

STATIC void
umd_touser_rate(umd_limit_rate_t *rate, jobj_t jrate)
{
    jj_u32(rate, jrate, max);
    jj_u32(rate, jrate, avg);
}

STATIC void
umd_touser_online(umd_limit_online_t *online, jobj_t jonline)
{
    jj_u32(online, jonline, max);
    jj_u32(online, jonline, idle);
    jj_u32(online, jonline, numerator);
    jj_u32(online, jonline, denominator);
    
    jj_i32(online, jonline, aging);

    jj_time(online, jonline, uptime);
    jj_time(online, jonline, downtime);
}

STATIC void
umd_touser_intf(umd_limit_t *intf, jobj_t jintf)
{
    jobj_t jobj;
    int dir;
    
    jobj_t jonline = jobj_get(jintf, "online");
    if (jonline) {
        umd_touser_online(&intf->online, jonline);
    }
    
    jobj = jobj_get(jintf, "flow");
    if (jobj) {
        for (dir=0; dir<umd_flow_dir_end; dir++) {
            jobj_t jflow = jobj_get(jobj, umd_flow_dir_getnamebyid(dir));
            if (jflow) {
                umd_touser_flow(&intf->flow[dir], jflow);
            }
        }
    }
    
    jobj = jobj_get(jintf, "rate");
    if (jobj) {
        for (dir=0; dir<umd_flow_dir_end; dir++) {
            jobj_t jrate = jobj_get(jobj, umd_flow_dir_getnamebyid(dir));
            if (jrate) {
                umd_touser_rate(&intf->rate[dir], jrate);
            }
        }
    }
}

STATIC void
umd_touser_limit(umd_user_t *user, jobj_t juser)
{
    int type;
    
    jobj_t jlimit = jobj_get(juser, "limit");
    if (jlimit) {
        for (type=0; type<umd_flow_type_end; type++) {
            jobj_t jintf = jobj_get(jlimit, umd_flow_type_getnamebyid(type));
            if (jintf) {
                umd_touser_intf(&user->limit[type], jintf);
            }
        }
    }
}

umd_user_t *umd_touser(umd_user_t *user, jobj_t juser)
{
    umd_touser_base(user, juser);
    umd_touser_tag(user, juser);
    umd_touser_limit(user, juser);
    
    return user;
}

STATIC bool
umd_match_mac(byte umac[], byte fmac[], byte mask[])
{
    if (is_good_mac(fmac)) {
        if (is_zero_mac(mask)) {
            /*
            * mac NOT zero
            * macmask zero
            *
            * use mac filter
            */
            if (false==os_maceq(umac, fmac)) {
                return false;
            }
        } else {
            /*
            * mac NOT zero
            * macmask NOT zero
            *
            * use mac/macmask filter
            */
            if (false==os_macmaskmach(umac, fmac, mask)) {
                return false;
            }
        }
    }

    return true;
}

STATIC bool
umd_match_ip(uint32 uip, uint32 fip, uint32 mask)
{
    if (fip) {
        if (0==mask) {
            /*
            * ip NOT zero
            * ipmask zero
            *
            * use ip filter
            */
            if (uip != fip) {
                return false;
            }
        } else {
            /*
            * ip NOT zero
            * ipmask NOT zero
            *
            * use ip/ipmask filter
            */
            if (false==os_ipmatch(uip, fip, mask)) {
                return false;
            }
        }
    }

    return true;
}

STATIC bool
umd_match_user(umd_user_t *user, umd_user_filter_t *filter)
{
    if (__is_user_have_bind(filter->state) && filter->state != user->state) {
        return false;
    }
    
    if (false==umd_match_mac(user->mac, filter->mac, filter->macmask)) {
        return false;
    }
    
    if (false==umd_match_ip(user->ip, filter->ip, filter->ipmask)) {
        return false;
    }
    
    /* all matched */
    return true;
}

int umd_user_delby(umd_user_filter_t *filter)
{
    mv_t foreach(umd_user_t *user)
    {
        if (umd_match_user(user, filter)) {
            umduser_delete(user);
        }

        return mv2_ok;
    }
    
    return umd_user_foreach(foreach, true);
}

int umd_user_getbyfilter(umd_user_filter_t *filter, mv_t (*get)(umd_user_t *user))
{
    mv_t foreach(umd_user_t *user)
    {
        if (umd_match_user(user, filter)) {
            return (*get)(user);
        } else {
            return mv2_ok;
        }
    }
    
    return umd_user_foreach(foreach, false);
}

STATIC void 
__umduser_online_aging(umd_user_t *user, int type)
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
umduser_online_aging(umd_user_t *user, time_t now)
{
    (void)now;
    
    if (is_user_have_bind(user)) {
        __umduser_online_aging(user, umd_flow_type_wan);
        __umduser_online_aging(user, umd_flow_type_lan);
    }
}

mv_t 
umd_user_gc(umd_user_t *user)
{
    if (is_user_noused(user)) {
        umd_user_debug_helper("gc", user, __is_ak_debug_gc);
        
        umduser_delete(user);
    }

    return 0;
}

STATIC bool
is_umduser_gc(umd_user_t *user, time_t now)
{
    time_t noused = user->noused;

    bool is = umd.cfg.gcuser 
        && (noused < now) 
        && (now - noused > umd.cfg.gcuser);

    if (is) {
        debug_gc("user(%s) gc(%u) noused(%u) now(%u) online gc",
            os_macstring(user->mac),
            umd.cfg.gcuser,
            noused,
            now);
    }

    return is;
}

STATIC void
umduser_gc_auto(umd_user_t *user, time_t now)
{
    if (is_umduser_gc(user, now)) {
        umd_user_gc(user);
    }
}

STATIC bool
__is_umduser_online_timeout(umd_user_t *user, time_t now, int type)
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
is_umduser_online_timeout(umd_user_t *user, time_t now)
{
    return __is_umduser_online_timeout(user, now, umd_flow_type_wan)
        || __is_umduser_online_timeout(user, now, umd_flow_type_lan);
}

STATIC void 
umduser_online_timeout(umd_user_t *user, time_t now)
{
    /*
    * online timeout
    *   just for auth user
    */
    if (is_user_auth(user) && is_umduser_online_timeout(user, now)) {        
        umduser_deauth(user, UMD_DEAUTH_ONLINETIME);
    }
}

STATIC bool
__is_umduser_online_reauth(umd_user_t *user, time_t now, int type)
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
is_umduser_online_reauth(umd_user_t *user, time_t now)
{
    return __is_umduser_online_reauth(user, now, umd_flow_type_wan)
        || __is_umduser_online_reauth(user, now, umd_flow_type_lan);
}

STATIC void 
umduser_online_reauth(umd_user_t *user, time_t now)
{
    /*
    * online reauth
    *   just for auth user
    */
    if (umd.cfg.reauthable 
            && is_user_auth(user) 
            && is_umduser_online_reauth(user, now)) {        
        umduser_reauth(user);
    }
}

STATIC bool
is_umduser_fake_timeout(umd_user_t *user, time_t now)
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
umduser_fake_timeout(umd_user_t *user, time_t now)
{
    if (is_user_fake(user) && is_umduser_fake_timeout(user, now)) {        
        umduser_unfake(user, UMD_DEAUTH_ONLINETIME);
    }
}
//guoshuai-hostapd-data
#ifdef __OPENWRT__
#ifdef __BUSYBOX__
STATIC int
get_brstamac_stats(int s, const u_int8_t macaddr[IEEE80211_ADDR_LEN])
{
	int err;
	struct iwreq iwr;
	struct ieee80211req_sta_stats stats;
	const struct ieee80211_nodestats *ns = &stats.is_stats;
	const char *sep;

	(void) memset(&iwr, 0, sizeof(iwr));
	(void) strncpy(iwr.ifr_name, ifr.ifr_name, sizeof(iwr.ifr_name));
	iwr.ifr_name[sizeof(iwr.ifr_name) - 1] = '\0';
	iwr.u.data.pointer = (void *) &stats;
	iwr.u.data.length = sizeof(stats);
	memcpy(stats.is_u.macaddr, macaddr, 18);

	err = ioctl(s, IEEE80211_IOCTL_STA_STATS, &iwr);
	if (err < 0) {
		debug_flow("ioctl error:%d", err);
		return err;
	}

	#define STAT(x,fmt) \
		brstaflow.x = ns->ns_##x;

	STAT(rx_data, "%u");
	STAT(rx_bytes, "%llu");
	STAT(tx_data, "%u");
	STAT(tx_bytes, "%llu");

	brstaflow.all_bytes = brstaflow.rx_bytes + brstaflow.tx_bytes;
	brstaflow.all_data = brstaflow.rx_data + brstaflow.tx_data;

	return 0;
}
STATIC void
umduser_update_brsta(umd_user_t *user)
{
	umd_flow_now(user, umd_flow_type_lan, umd_flow_dir_up) = brstaflow.rx_bytes;
	umd_flow_now(user, umd_flow_type_wan, umd_flow_dir_up) = brstaflow.rx_bytes;
	umd_flow_now(user, umd_flow_type_lan, umd_flow_dir_down) = brstaflow.tx_bytes;
	umd_flow_now(user, umd_flow_type_wan, umd_flow_dir_down) = brstaflow.tx_bytes;
	umd_flow_now(user, umd_flow_type_lan, umd_flow_dir_all) = brstaflow.all_bytes;
	umd_flow_now(user, umd_flow_type_wan, umd_flow_dir_all) = brstaflow.all_bytes;

	umd_update_aging(user, true);
}

STATIC int
umduser_brflow_update(umd_user_t *user, time_t now)
{
	int fd, err;
	fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (fd < 0) {
			debug_flow("socket error:%d", -errno);
			return -errno;
		}
	strncpy(ifr.ifr_name, "ath0", sizeof (ifr.ifr_name));
	os_strcpy( brstaflow.brstamac, os_macstring(user->mac));

	const struct ether_addr *ea = ether_aton(brstaflow.brstamac);
	if (ea != NULL)
	{
		err = get_brstamac_stats(fd, ea->ether_addr_octet);
		if (err<0) { /* yes, <0 */
			return err;
		}
	}
	
	umduser_update_brsta(user);
    
	return 0;
}
#endif
#endif
//end

STATIC void
umduser_timer_handle(umd_user_t *user, time_t now)
{
    static umd_user_timer_f *handler[] = {
        umduser_fake_timeout,
        umduser_online_reauth,
        umduser_online_timeout,
        umduser_online_aging,
        //guoshuai-hostapd-data
        #ifdef __OPENWRT__
        #ifdef __BUSYBOX__
        umduser_brflow_update,
        #endif
        #endif
        //end
        umduser_gc_auto, // keep last
    };
    
    int i;

    for (i=0; i<os_count_of(handler); i++) {
        (*handler[i])(user, now);
    }
}

int
umd_user_timer(loop_watcher_t *watcher, time_t now)
{
    mv_t foreach(umd_user_t *user)
    {
        umduser_timer_handle(user, now);

        return mv2_ok;
    }
    
    umd_user_foreach(foreach, true);

    return 0;
}

/******************************************************************************/
