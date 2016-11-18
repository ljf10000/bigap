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
#define __update_online(_user, _obj, _TYPE, _field)         do{ \
    jobj_t o = jobj_get_leaf(_obj, #_TYPE, "online", #_field, NULL); \
                                                                \
    limit_online(_user, _TYPE)->_field = o?jobj_get_i32(o):0;   \
}while(0)

#define update_online(_user, _obj, _field)  do{ \
    __update_online(_user, _obj, lan, _field);  \
    __update_online(_user, _obj, wan, _field);  \
}while(0)

#define __update_flow(_user, _obj, _TYPE, _DIR, _field) do{ \
    jobj_t o = jobj_get_leaf(_obj, #_TYPE, "flow", #_DIR, #_field, NULL); \
                                                            \
    limit_flow(_user, _TYPE, _DIR)->_field = o?jobj_get_i64(o):0; \
}while(0)

#define updata_flow(_user, _obj, _field)        do{ \
    __update_flow(_user, _obj, lan, up, _field);    \
    __update_flow(_user, _obj, wan, up, _field);    \
    __update_flow(_user, _obj, lan, down, _field);  \
    __update_flow(_user, _obj, wan, down, _field);  \
    __update_flow(_user, _obj, lan, all, _field);   \
    __update_flow(_user, _obj, wan, all, _field);   \
}while(0)

#define __update_rate(_user, _obj, _TYPE, _DIR, _field)         do{ \
    jobj_t o = jobj_get_leaf(_obj, #_TYPE, "rate", #_DIR, #_field, NULL); \
                                                                    \
    limit_rate(_user, _TYPE, _DIR)->_field = o?jobj_get_i32(o):0;   \
}while(0)

#define updata_rate(_user, _obj, _field)        do{ \
    __update_rate(_user, _obj, lan, up, _field);    \
    __update_rate(_user, _obj, wan, up, _field);    \
    __update_rate(_user, _obj, lan, down, _field);  \
    __update_rate(_user, _obj, wan, down, _field);  \
    __update_rate(_user, _obj, lan, all, _field);   \
    __update_rate(_user, _obj, wan, all, _field);   \
}while(0)

static void
update_limit(struct um_user *user, jobj_t obj)
{
    update_online(user, obj, max);
    update_online(user, obj, idle);
    update_online(user, obj, numerator);
    update_online(user, obj, denominator);
    
    updata_flow(user, obj, max);
    updata_flow(user, obj, numerator);
    updata_flow(user, obj, denominator);
    
    updata_rate(user, obj, max);
    updata_rate(user, obj, avg);
}

void update_limit_test(void)
{
#if UM_TEST & UM_TEST_JSON
    struct um_intf *intf = &umd.cfg.instance.intf[0];
    struct um_user user = {
        .ip = intf->ip,
        .head = { 
            .tag = LIST_HEAD_INIT(user.head.tag),
        },
    };
    os_maccpy(user.mac, intf->mac);
    
    jobj_t obj = jobj_byfile("./auth.json");
    if (NULL==obj) {
        os_println("read auth.json failed");

        exit(1);
    }
    
    update_limit(&user, obj);
    update_aging(&user, true);
    jobj_put(obj);

    obj = um_juser(&user);
    os_println("%s", jobj_json(obj));
    os_system("echo '%s' | jq .", jobj_json(obj));
    jobj_put(obj);
#endif
}

typedef int event_cb_t(struct um_user *user, char *action);

static int
__ev(struct um_user *user, char *action)
{
    jobj_t juser = um_juser(user);
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

static int
__ev_call(event_cb_t *ev, struct um_user *user, char *action)
{
    int err = 0;
    
    if (ev) {
        err = (*ev)(user, action);
    }

    return err;
}

static inline hash_idx_t
hashmac(byte mac[])
{
    return hash_bybuf(mac, OS_MACSIZE, umd.cfg.machashsize - 1);
}

static inline hash_idx_t
haship(uint32 ip)
{
    return hash_bybuf((byte *)&ip, sizeof(ip), umd.cfg.iphashsize - 1);
}

static inline struct um_user *
user_hx_entry(h2_node_t *node)
{
    if (node) {
        return hash_entry(node, struct um_user, node);
    } else {
        return NULL;
    }
}

static inline struct um_user *
user_entry(hash_node_t *node, hash_idx_t nidx)
{
    return hx_entry(node, struct um_user, node, nidx);
}

static hash_idx_t
nodehashmac(hash_node_t *node)
{
    struct um_user *user = user_entry(node, UM_USER_NIDX_MAC);

    return hashmac(user->mac);
}

static hash_idx_t
nodehaship(hash_node_t *node)
{
    struct um_user *user = user_entry(node, UM_USER_NIDX_IP);

    return haship(user->ip);
}

static inline void
tag_free(struct um_tag *tag)
{
    if (tag) {
        os_free(tag->k);
        os_free(tag->v);
        os_free(tag);
    }
}

static inline struct um_tag *
tag_new(char *k, char *v)
{
    struct um_tag *tag = (struct um_tag *)os_malloc(sizeof(*tag));
    if (NULL==tag) {
        return NULL;
    }
    INIT_LIST_HEAD(&tag->tag);

    tag->k = strdup(k);
    tag->v = strdup(v);

    if (NULL==tag->k || NULL==tag->v) {
        tag_free(tag); 

        return NULL;
    }
    
    return tag;
}

static inline struct um_tag *
tag_get(struct um_user *user, char *k)
{
    struct um_tag *tag;

    if (NULL==user) {
        return NULL;
    }
    
    list_for_each_entry(tag, &user->head.tag, tag) {
        if (os_streq(k, tag->k)) {
            return tag;
        }
    }

    return NULL;
}

static inline struct um_tag *
tag_insert(struct um_user *user, char *k, char *v, bool update_if_exist)
{
    struct um_tag *tag = NULL;

    if (NULL==user) {
        return NULL;
    }

    tag = tag_get(user, k);
    if (NULL==tag) {
        tag = tag_new(k, v);
        if (NULL==tag) {
            return NULL;
        }

        list_add(&tag->tag, &user->head.tag);
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

static inline struct um_tag *
tag_set(struct um_user *user, char *k, char *v)
{
    return tag_insert(user, k, v, true);
}

static inline void
tag_clear(struct um_user *user)
{
    struct um_tag *p, *n;
    
    list_for_each_entry_safe(p, n, &user->head.tag, tag) {
        tag_free(p);
    }
}

static void
flow_reset(struct um_user *user, int type)
{
    int i;

    for (i=0; i<um_flow_dir_end; i++) {
        __flow_now(user, type, i) = 0;
    }
}

static void
lan_online(struct um_user *user)
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
    __online_uptime(user, um_flow_type_lan)     = time(NULL);
    __online_uptime(user, um_flow_type_wan)     = 0;
    
    __online_downtime(user, um_flow_type_lan)   = 0;
    __online_downtime(user, um_flow_type_wan)   = 0;

    __online_idle(user, um_flow_type_lan) = umd.cfg.idle;
    __online_idle(user, um_flow_type_wan) = umd.cfg.idle;
    
    flow_reset(user, um_flow_type_lan);
    flow_reset(user, um_flow_type_wan);

    tag_clear(user);
    
    debug_event("user %s lan online", os_macstring(user->mac));
}

static void
wan_online(struct um_user *user)
{
    /*
    * wan, offline==>online
    *   1. update wan uptime
    *   2. clear lan/wan downtime
    *   3. reset wan flow
    *   4. reset tag
    */
    __online_uptime(user, um_flow_type_wan)     = time(NULL);
    
    __online_downtime(user, um_flow_type_lan)   = 0;
    __online_downtime(user, um_flow_type_wan)   = 0;

    flow_reset(user, um_flow_type_wan);

    tag_clear(user);
    
    debug_event("user %s wan online", os_macstring(user->mac));
}

static void
lan_offline(struct um_user *user)
{
    /*
    * lan, online==>offline
    *   1. update lan downtime
    *      and keep others
    */
    __online_downtime(user, um_flow_type_lan)   = time(NULL);

    if (is_user_noused(user)) {
        user->noused = time(NULL);
    }

    debug_event("user %s lan offline", os_macstring(user->mac));
}

static void
wan_offline(struct um_user *user)
{
    /*
    * wan, online==>offline
    *   1. update wan downtime
    *   2. clear  lan downtime
    *      and keep others
    */
    __online_downtime(user, um_flow_type_wan)   = time(NULL);
    __online_downtime(user, um_flow_type_lan)   = 0;
    
    debug_event("user %s wan offline", os_macstring(user->mac));
}

static struct um_user *
__user_remove(struct um_user *user)
{
    if (NULL==user) {
        debug_bug("user nil");
        
        return NULL;
    }
    /*
    * not in list
    */
    else if (false==in_hx_table(&user->node)) {
        debug_bug("user not in list");
        
        return user;
    }

    h2_del(&umd.table, &user->node);

    return user;
}

static struct um_user *
__user_insert(struct um_user *user)
{
    static hash_node_calc_f *nhash[UM_USER_NIDX_END] = {
        [UM_USER_NIDX_MAC]  = nodehashmac, 
        [UM_USER_NIDX_IP]   = nodehaship
    };
    
    if (NULL==user) {
        debug_bug("user nil");
        
        return NULL;
    }
    /*
    * have in list
    */
    else if (in_hx_table(&user->node)) {
        debug_bug("user have in list");
        
        __user_remove(user);
    }

    h2_add(&umd.table, &user->node, nhash);
    
    return user;
}

static void
libc_set_group(struct um_user *user, int group)
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

static void
__set_reason(struct um_user *user, int reason)
{
    if (NULL==user) {
        debug_bug("user nil");
        
        return;
    }
    else if (reason==user->reason) {
        debug_bug("same reason %s", deauth_reason_string(reason));
        
        return;
    }

    debug_trace("user(%s) state(%s) reason change %s==>%s", 
        os_macstring(user->mac),
        user_state_string(user->state),
        deauth_reason_string(user->reason), 
        deauth_reason_string(reason));
    
    user->reason = reason;
}

static void
__set_ip(struct um_user *user, uint32 ip)
{
    if (NULL==user) {
        debug_bug("user nil");
        
        return;
    }
    else if (ip==user->ip) {
        debug_bug("same ip %s", os_ipstring(ip));
        
        return;
    }
    
    __user_remove(user);
    user->ip = ip;
    __user_insert(user);
}

static void
__set_state(struct um_user *user, int state)
{
    if (NULL==user) {
        debug_bug("user nil");
        
        return;
    }
    else if (state==user->state) {
        debug_bug("same state %s", user_state_string(state));
        
        return;
    }

    debug_trace("user %s state change %s==>%s", 
        os_macstring(user->mac), 
        user_state_string(user->state), 
        user_state_string(state));

    user->state = state;
}


static void
__user_debug(char *tag, struct um_user *user)
{
    um_user_debug(tag, user, __is_ak_debug_entry && __is_ak_debug_event);
}

#define ev_call(_ev, _user)     __ev_call(_ev, _user, __user_debug_call_tag)

#define __user_debug_call(_pos, _tag, _user, _body)   do{ \
    char *__user_debug_call_tag = _tag;             \
    if (is_position_head(_pos)) {                   \
        __user_debug("before-user-" _tag, _user);   \
    }                                               \
    _body                                           \
    if (is_position_tail(_pos)) {                   \
        __user_debug("after-user-" _tag, _user);    \
    }                                               \
}while(0)

#define user_debug_head_call(_tag, _user, _body) \
    __user_debug_call(OS_POSITION_HEAD, _tag, _user, _body)

#define user_debug_tail_call(_tag, _user, _body) \
    __user_debug_call(OS_POSITION_TAIL, _tag, _user, _body)

#define user_debug_call(_tag, _user, _body) \
    __user_debug_call(OS_POSITION_ALL, _tag, _user, _body)

static int
__user_delete(struct um_user *user, event_cb_t *ev)
{
    if (NULL==user) {
        return -ENOEXIST;
    }

    user_debug_head_call("delete", user, {
        ev_call(ev, user);
        tag_clear(user);
        __user_remove(user);
        os_free(user);
    });

    return 0;
}

static struct um_user *
__user_create(byte mac[], event_cb_t *ev)
{
    struct um_user *user;

    user = (struct um_user *)os_zalloc(sizeof(*user));
    if (NULL==user) {
        return NULL;
    }
    os_maccpy(user->mac, mac);
    
    INIT_LIST_HEAD(&user->head.tag);

    user_debug_tail_call("create", user, {
        __set_state(user, UM_STATE_NONE);
        lan_online(user);
        user->create = user->noused = time(NULL);
        __user_insert(user);
        ev_call(ev, user);
    });
    
    return user;
}

static int
__user_deauth(struct um_user *user, int reason, event_cb_t *ev)
{
    if (NULL==user) {
        return -ENOEXIST;
    }
    // must @auth
    else if (false==is_user_auth(user)) {
        return 0;
    }

    // auth==>bind
    user_debug_call("deauth", user, {
        __set_reason(user, reason);
        ev_call(ev, user);
        
        __set_state(user, UM_STATE_BIND);
        libc_set_group(user, 0);
        wan_offline(user);
    });

    return 0;
}

static int
__user_unfake(struct um_user *user, int reason, event_cb_t *ev)
{
    if (NULL==user) {
        return -ENOEXIST;
    }
    // must @fake
    else if (false==is_user_fake(user)) {
        return 0;
    }

    // fake==>bind
    user_debug_call("unfake", user, {
        __set_reason(user, reason);
        ev_call(ev, user);
        
        __set_state(user, UM_STATE_BIND);
        wan_offline(user);
    });

    return 0;
}

static int
__user_unbind(struct um_user *user, int reason, event_cb_t *ev)
{
    if (NULL==user) {
        return -ENOEXIST;
    }

    // try auth/fake==>bind
    __user_deauth(user, reason, __ev);
    __user_unfake(user, reason, __ev);

    // must @bind
    if (false==is_user_bind(user)) {
        return 0;
    }

    // bind==>none
    user_debug_call("unbind", user, {
        __set_reason(user, reason);
        ev_call(ev, user);
        
        __set_ip(user, 0);
        __set_state(user, UM_STATE_NONE);
        lan_offline(user);
    });

    return 0;
}

static struct um_user *
__user_block(struct um_user *user, event_cb_t *ev)
{
    if (NULL==user) {
        return NULL;
    }

    // try auth/fake/bind==>none
    __user_unbind(user, UM_DEAUTH_BLOCK, __ev);

    // must @none
    if (false==is_user_none(user)) {
        return NULL;
    }

    // none==>block
    user_debug_call("block", user, {
        __set_reason(user, UM_DEAUTH_BLOCK);
        ev_call(ev, user);
        
        __set_state(user, UM_STATE_BLOCK);
    });

    return user;
}
/******************************************************************************/
static struct um_user *
__user_unblock(struct um_user *user, event_cb_t *ev)
{
    if (NULL==user) {
        return NULL;
    }
    // must @block
    else if (false==is_user_block(user)) {
        return NULL;
    }

    // block==>none
    user_debug_call("unblock", user, {
        __set_state(user, UM_STATE_NONE);
        
        ev_call(ev, user);
    });

    return user;
}

static struct um_user *
__user_bind(struct um_user *user, uint32 ip, event_cb_t *ev)
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
            __user_unbind(user, UM_DEAUTH_AUTO, __ev);
        }
    }

    // must @none
    if (false==is_user_none(user)) {
        return NULL;
    }
    
    // none==>bind
    user_debug_call("bind", user, {
        __set_ip(user, ip);
        __set_state(user, UM_STATE_BIND);
        lan_online(user);
        update_aging(user, true);
        ev_call(ev, user);
    });

    return user;
}

static struct um_user *
__user_fake(struct um_user *user, uint32 ip, event_cb_t *ev)
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
            __user_unbind(user, UM_DEAUTH_AUTO, __ev);
        }
    }
    
    user_debug_call("fake", user, {
        __set_ip(user, ip);
        __set_state(user, UM_STATE_FAKE);
        lan_online(user);
        update_aging(user, true);
        user->faketime = time(NULL);
        ev_call(ev, user);
    });

    return user;
}

static struct um_user *
__user_reauth(struct um_user *user, event_cb_t *ev)
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
    
    user_debug_call("reauth", user, {
        ev_call(ev, user);
    });

    return user;
}

static struct um_user *
__user_auth(struct um_user *user, int group, jobj_t obj, event_cb_t *ev)
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
            __user_bind(user, inet_addr(ipaddress), __ev);
        }
    }
    
    // must @bind/fake/auth
    if (false==is_user_have_bind(user)) {
        return NULL;
    }
    
    user_debug_call("auth", user, {
        __set_state(user, UM_STATE_AUTH);
        libc_set_group(user, group);
        __set_reason(user, UM_DEAUTH_NONE);
        update_limit(user, obj);
        wan_online(user);
        update_aging(user, true);
        ev_call(ev, user);
    });

    return user;
}

static struct um_user *
__user_sync(struct um_user *user, jobj_t obj, event_cb_t *ev)
{
    if (NULL==user) {
        return NULL;
    }
    
    user_debug_call("sync", user, {
        um_touser(user, obj);
        
        ev_call(ev, user);
    });

    return user;
}

static struct um_user *
__user_get(byte mac[])
{
    hash_idx_t dhash(void)
    {
        return hashmac(mac);
    }
    
    bool eq(hash_node_t *node)
    {
        struct um_user *user = user_entry(node, UM_USER_NIDX_MAC);
        
        return os_maceq(user->mac, mac);
    }
    
    return user_hx_entry(h2_find(&umd.table, UM_USER_NIDX_MAC, dhash, eq));
}

static struct um_user *
user_get(byte mac[])
{
    struct um_user *user = __user_get(mac);
    if (NULL==user) {
        debug_trace("no-found user %s", os_macstring(mac));
    }

    return user;
}

static struct um_user *
user_create(byte mac[])
{
    return __user_create(mac, __ev);
}

int user_delete(struct um_user *user)
{
    return __user_delete(user, __ev);
}

static struct um_user *
user_block(struct um_user *user)
{
    return __user_block(user, __ev);
}

int user_unblock(struct um_user *user)
{
    return __user_unblock(user, __ev)?0:-ENOPERM;
}

static struct um_user *
user_bind(struct um_user *user, uint32 ip)
{
    return __user_bind(user, ip, __ev);
}

int user_unbind(struct um_user *user, int reason)
{
    if (is_valid_deauth_reason(reason)) {
        return __user_unbind(user, reason, __ev);
    } else {
        return -EBADREASON;
    }
}

static struct um_user *
user_fake(struct um_user *user, uint32 ip)
{
    return __user_fake(user, ip, __ev);
}

int user_unfake(struct um_user *user, int reason)
{
    if (is_valid_deauth_reason(reason)) {
        return __user_unfake(user, reason, __ev);
    } else {
        return -EBADREASON;
    }
}

static struct um_user *
user_auth(struct um_user *user, int group, jobj_t obj)
{
    return __user_auth(user, group, obj, __ev);
}

int user_deauth(struct um_user *user, int reason)
{
    if (is_valid_deauth_reason(reason)) {
        return __user_deauth(user, reason, __ev);
    } else {
        return -EBADREASON;
    }
}

struct um_user *
user_reauth(struct um_user *user)
{
    return __user_reauth(user, __ev);
}

static struct um_user *
user_sync(struct um_user *user, jobj_t obj)
{
    return __user_sync(user, obj, __ev);
}

static int
user_foreach(um_foreach_f *foreach, bool safe)
{
    mv_t node_foreach(h2_node_t *node)
    {
        return (*foreach)(user_hx_entry(node));
    }

    if (safe) {
        return h2_foreach_safe(&umd.table, node_foreach);
    } else {
        return h2_foreach(&umd.table, node_foreach);
    }
}

struct um_tag *
um_tag_get(byte mac[], char *k)
{
    return tag_get(__user_get(mac), k);
}

struct um_tag *
um_tag_set(byte mac[], char *k, char *v)
{
    return tag_set(__user_get(mac), k, v);
}

int um_user_delete(byte mac[])
{
    return user_delete(__user_get(mac));
}

struct um_user *
um_user_create(byte mac[])
{
    struct um_user *user = __user_get(mac);

    return user?user:user_create(mac);
}

struct um_user *
um_user_block(byte mac[])
{
    return user_block(um_user_create(mac));
}

int um_user_unblock(byte mac[])
{
    return user_unblock(user_get(mac));
}

struct um_user *
um_user_bind(byte mac[], uint32 ip)
{
    return user_bind(um_user_create(mac), ip);
}

int um_user_unbind(byte mac[])
{
    return user_unbind(user_get(mac), UM_DEAUTH_INITIATIVE);
}

struct um_user *
um_user_fake(byte mac[], uint32 ip)
{
    return user_fake(um_user_create(mac), ip);
}

int um_user_unfake(byte mac[])
{
    return user_unfake(user_get(mac), UM_DEAUTH_INITIATIVE);
}

struct um_user *
um_user_auth(byte mac[], int group, jobj_t obj)
{
    return user_auth(um_user_create(mac), group, obj);
}

int um_user_deauth(byte mac[], int reason)
{
    return user_deauth(user_get(mac), reason);
}

int um_user_reauth(byte mac[])
{
    return user_reauth(user_get(mac))?0:-ENOPERM;
}

struct um_user *
um_user_sync(byte mac[], jobj_t obj)
{
    return user_sync(um_user_create(mac), obj);
}

int um_user_foreach(um_foreach_f *foreach, bool safe)
{
    return user_foreach(foreach, safe);
}

struct um_user *
um_user_get(byte mac[])
{
    return __user_get(mac);
}

struct um_user *
um_user_getbyip(uint32 ip)
{
    hash_idx_t dhash(void)
    {
        return haship(ip);
    }
    
    bool eq(hash_node_t *node)
    {
        struct um_user *user = user_entry(node, UM_USER_NIDX_IP);

        return ip==user->ip;
    }
    
    return user_hx_entry(h2_find(&umd.table, UM_USER_NIDX_IP, dhash, eq));
}

int um_user_delbyip(uint32 ip)
{
    return user_delete(um_user_getbyip(ip));
}

static jobj_t
juser_online(struct um_user *user, int type)
{
    jobj_t obj = jobj_new_object();
    time_t time;

    jobj_add_u32(obj, "max",            __online_max(user, type));
    jobj_add_u32(obj, "idle",           __online_idle(user, type));
    jobj_add_i32(obj, "aging",          __online_aging(user, type));
    jobj_add_u32(obj, "numerator",      __online_numerator(user, type));
    jobj_add_u32(obj, "denominator",    __online_denominator(user, type));
    
    time = __online_uptime(user, type);
    if (time) {
        jobj_add_string(obj, "uptime", os_fulltime_string(&time));
    }

    time = __online_downtime(user, type);
    if (time) {
        jobj_add_string(obj, "downtime", os_fulltime_string(&time));
    }
    
    return obj;
}

static jobj_t
__juser_flow(struct um_user *user, int type, int dir)
{
    jobj_t obj = jobj_new_object();
    
    jobj_add_u64(obj, "max",          __flow_max(user, type, dir));
    jobj_add_u64(obj, "now",          __flow_now(user, type, dir));
    jobj_add_u64(obj, "numerator",    __flow_numerator(user, type, dir));
    jobj_add_u64(obj, "denominator",  __flow_denominator(user, type, dir));

    return obj;
}

static jobj_t
juser_flow(struct um_user *user, int type)
{
    jobj_t obj = jobj_new_object();
    int dir;

    for (dir=0; dir<um_flow_dir_end; dir++) {
        jobj_add(obj, flow_dir_string(dir), __juser_flow(user, type, dir));
    }

    return obj;
}

static jobj_t
__juser_rate(struct um_user *user, int type, int dir)
{
    jobj_t obj = jobj_new_object();
    
    jobj_add_u32(obj, "max", __rate_max(user, type, dir));
    jobj_add_u32(obj, "avg", __rate_avg(user, type, dir));

    return obj;
}

static jobj_t
juser_rate(struct um_user *user, int type)
{
    jobj_t obj = jobj_new_object();
    int dir;

    for (dir=0; dir<um_flow_dir_end; dir++) {
        jobj_add(obj, flow_dir_string(dir), __juser_rate(user, type, dir));
    }

    return obj;
}

static jobj_t
__juser_limit(struct um_user *user, int type)
{
    jobj_t obj = jobj_new_object();

    jobj_add(obj, "online", juser_online(user, type));
    jobj_add(obj, "flow", juser_flow(user, type));
    jobj_add(obj, "rate", juser_rate(user, type));

    return obj;
}

static jobj_t
juser_limit(struct um_user *user)
{
    jobj_t obj = jobj_new_object();

    jobj_add(obj, flow_type_string(um_flow_type_lan), __juser_limit(user, um_flow_type_lan));
    jobj_add(obj, flow_type_string(um_flow_type_wan), __juser_limit(user, um_flow_type_wan));
    
    return obj;
}

static jobj_t
juser_tag(struct um_user *user)
{
    jobj_t obj = jobj_new_object();
    struct um_tag *tag;
    
    list_for_each_entry(tag, &user->head.tag, tag) {
        jobj_add_string(obj, tag->k, tag->v);
    }

    return obj;
}

jobj_t um_juser(struct um_user *user)
{
    jobj_t obj = jobj_new_object();

    jobj_add_string(obj, "mac",             os_macstring(user->mac));
    jobj_add_string(obj, "bssid_first",     os_macstring(user->bssid_first));
    jobj_add_string(obj, "bssid_current",   os_macstring(user->bssid_current));
    jobj_add_string(obj, "ssid",    user->ssid);
    jobj_add_string(obj, "ip",      os_ipstring(user->ip));
    jobj_add_string(obj, "state",   user_state_string(user->state));
    jobj_add_string(obj, "reason",  deauth_reason_string(user->reason));
    
    jobj_add_string(obj, "create",  os_fulltime_string(&user->create));
    jobj_add_string(obj, "noused",  os_fulltime_string(&user->noused));
    jobj_add_string(obj, "faketime",os_fulltime_string(&user->faketime));
    jobj_add_string(obj, "hitime",  os_fulltime_string(&user->hitime));
    
    jobj_add_i32(obj,   "group",    user->group);

    jobj_add(obj, "tag",    juser_tag(user));
    jobj_add(obj, "limit",  juser_limit(user));

    return obj;
}

static void
touser_base(struct um_user *user, jobj_t juser)
{
    jj_mac(user, juser, mac);
    jj_mac(user, juser, bssid_first);
    jj_ip(user, juser, ip);

    jj_byeq(user, juser, state, user_state_idx);
    jj_byeq(user, juser, reason, deauth_reason_idx);

    jj_time(user, juser, create);
    jj_time(user, juser, noused);
    jj_time(user, juser, hitime);
    
    jj_i32(user, juser, group);
}

static void
touser_tag(struct um_user *user, jobj_t juser)
{
    jobj_t jtag;
    
    jtag = jobj_get(juser, "tag");
    if (jtag) {
        jobj_foreach(jtag, k, v) {
            if (jtype_string==jobj_type(v)) {
                tag_set(user, k, jobj_get_string(v));
            }
        }
    }
}

static void
touser_flow(struct um_limit_flow *flow, jobj_t jflow)
{
    jj_u64(flow, jflow, max);
    jj_u64(flow, jflow, now);
    jj_u64(flow, jflow, numerator);
    jj_u64(flow, jflow, denominator);
}

static void
touser_rate(struct um_limit_rate *rate, jobj_t jrate)
{
    jj_u32(rate, jrate, max);
    jj_u32(rate, jrate, avg);
}

static void
touser_online(struct um_limit_online *online, jobj_t jonline)
{
    jj_u32(online, jonline, max);
    jj_u32(online, jonline, idle);
    jj_u32(online, jonline, numerator);
    jj_u32(online, jonline, denominator);
    
    jj_i32(online, jonline, aging);

    jj_time(online, jonline, uptime);
    jj_time(online, jonline, downtime);
}

static void
touser_intf(struct um_limit *intf, jobj_t jintf)
{
    jobj_t jobj;
    int dir;
    
    jobj_t jonline = jobj_get(jintf, "online");
    if (jonline) {
        touser_online(&intf->online, jonline);
    }
    
    jobj = jobj_get(jintf, "flow");
    if (jobj) {
        for (dir=0; dir<um_flow_dir_end; dir++) {
            jobj_t jflow = jobj_get(jobj, flow_dir_string(dir));
            if (jflow) {
                touser_flow(&intf->flow[dir], jflow);
            }
        }
    }
    
    jobj = jobj_get(jintf, "rate");
    if (jobj) {
        for (dir=0; dir<um_flow_dir_end; dir++) {
            jobj_t jrate = jobj_get(jobj, flow_dir_string(dir));
            if (jrate) {
                touser_rate(&intf->rate[dir], jrate);
            }
        }
    }
}

static void
touser_limit(struct um_user *user, jobj_t juser)
{
    int type;
    
    jobj_t jlimit = jobj_get(juser, "limit");
    if (jlimit) {
        for (type=0; type<um_flow_type_end; type++) {
            jobj_t jintf = jobj_get(jlimit, flow_type_string(type));
            if (jintf) {
                touser_intf(&user->limit[type], jintf);
            }
        }
    }
}

struct um_user *um_touser(struct um_user *user, jobj_t juser)
{
    touser_base(user, juser);
    touser_tag(user, juser);
    touser_limit(user, juser);
    
    return user;
}

static inline bool
match_mac(byte umac[], byte fmac[], byte mask[])
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

static inline bool
match_ip(uint32 uip, uint32 fip, uint32 mask)
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

static bool
match_user(struct um_user *user, struct um_user_filter *filter)
{
    if (__is_user_have_bind(filter->state) && filter->state != user->state) {
        return false;
    }
    
    if (false==match_mac(user->mac, filter->mac, filter->macmask)) {
        return false;
    }
    
    if (false==match_ip(user->ip, filter->ip, filter->ipmask)) {
        return false;
    }
    
    /* all matched */
    return true;
}

int um_user_delby(struct um_user_filter *filter)
{
    mv_t cb(struct um_user *user)
    {
        if (match_user(user, filter)) {
            user_delete(user);
        }

        return mv2_ok;
    }
    
    return um_user_foreach(cb, true);
}

int um_user_getby(struct um_user_filter *filter, um_get_f *get)
{
    mv_t cb(struct um_user *user)
    {
        if (match_user(user, filter)) {
            return (*get)(user);
        } else {
            return mv2_ok;
        }
    }
    
    return um_user_foreach(cb, false);
}
/******************************************************************************/
