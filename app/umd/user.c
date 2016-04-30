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
    if (o) {                                                    \
        limit_online(_user, _TYPE)->_field = jobj_get_i32(o);   \
    }                                                           \
}while(0)

#define update_online(_user, _obj, _field)  do{ \
    __update_online(_user, _obj, lan, _field);  \
    __update_online(_user, _obj, wan, _field);  \
}while(0)

#define __update_flow(_user, _obj, _TYPE, _DIR, _field) do{ \
    jobj_t o = jobj_get_leaf(_obj, #_TYPE, "flow", #_DIR, #_field, NULL); \
    if (o) {                                                        \
        limit_flow(_user, _TYPE, _DIR)->_field = jobj_get_i64(o); \
    }                                                               \
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
    if (o) {                                                        \
        limit_rate(_user, _TYPE, _DIR)->_field = jobj_get_i32(o);   \
    }                                                               \
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
    struct um_user user = {
        .ip = umd.intf[UM_INTF_TC].ip,
        .head = { 
            .tag = LIST_HEAD_INIT(user.head.tag),
        },
    };
    os_maccpy(user.mac, umd.intf[UM_INTF_TC].mac);
    
    jobj_t obj = jobj_file("./auth.json");
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

typedef int event_cb_t(struct um_user *user);

static int
__cb(struct um_user *user, char *action, int (*cb)(jobj_t obj))
{
    static char json[1+OS_FILE_LEN];
    int err = 0;
    
    jobj_t juser = um_juser(user);
    if (NULL==juser) {
        return -ENOEXIST;
    }

    os_arrayzero(json);
    os_v_pgetb(json, OS_FILE_LEN, 
        UMD_SCRIPT_EVENT " %s '%s'",
        action,
        jobj_json(juser));
    jobj_put(juser);
    
    if (cb) {
        jobj_t obj = jobj(json);
        if (NULL==obj) {
            return -EBADJSON;
        }
        
        err = (*cb)(obj);

        jobj_put(obj);
    }
    
    return err;
}

static int
delete_cb(struct um_user *user)
{
    return __cb(user, "delete", NULL);
}

static int
create_cb(struct um_user *user)
{
    return __cb(user, "create", NULL);
}

#if UM_USE_MONITOR
static int
enter_cb(struct um_user *user)
{
    return __cb(user, "enter", NULL);
}

static int
leave_cb(struct um_user *user)
{
    return __cb(user, "leave", NULL);
}
#endif

static int
bind_cb(struct um_user *user)
{
    return __cb(user, "bind", NULL);
}

static int
unbind_cb(struct um_user *user)
{
    return __cb(user, "unbind", NULL);
}

static int
auth_cb(struct um_user *user)
{
    return __cb(user, "auth", NULL);
}

static int
reauth_cb(struct um_user *user)
{
    int cb(jobj_t obj)
    {
        update_limit(user, obj);

        return 0;
    }
    
    return __cb(user, "reauth", cb);
}

static int
deauth_cb(struct um_user *user)
{
    return __cb(user, "deauth", NULL);
}

static inline hash_idx_t
hashmac(byte mac[])
{
    return hash_bybuf(mac, OS_MACSIZE, UMD_HASHMASK);
}

static inline hash_idx_t
haship(uint32 ip)
{
    return hash_bybuf((byte *)&ip, sizeof(ip), UMD_HASHMASK);
}

static inline struct um_user *
getuser_bynode(h2_node_t *node)
{
    return hash_entry(node, struct um_user, node);
}

static inline struct um_user *
getuser_bynidx(hash_node_t *node, hash_idx_t nidx)
{
    return hx_entry(node, struct um_user, node, nidx);
}

static hash_idx_t
nodehashmac(hash_node_t *node)
{
    struct um_user *user = getuser_bynidx(node, UM_USER_NIDX_MAC);

    return hashmac(user->mac);
}

static hash_idx_t
nodehaship(hash_node_t *node)
{
    struct um_user *user = getuser_bynidx(node, UM_USER_NIDX_IP);

    return haship(user->ip);
}

static inline void
__tag_free(struct um_tag *tag)
{
    if (tag) {
        os_free(tag->k);
        os_free(tag->v);
        os_free(tag);
    }
}

static inline struct um_tag *
__tag_new(char *k, char *v)
{
    struct um_tag *tag = (struct um_tag *)os_malloc(sizeof(*tag));
    if (NULL==tag) {
        return NULL;
    }
    INIT_LIST_HEAD(&tag->tag);

    tag->k = strdup(k);
    tag->v = strdup(v);

    if (NULL==tag->k || NULL==tag->v) {
        __tag_free(tag); 

        return NULL;
    }
    
    return tag;
}

static inline struct um_tag *
__tag_get(struct um_user *user, char *k)
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
__tag_insert(struct um_user *user, char *k, char *v, bool update_if_exist)
{
    struct um_tag *tag = NULL;

    if (NULL==user) {
        return NULL;
    }

    tag = __tag_get(user, k);
    if (NULL==tag) {
        tag = __tag_new(k, v);
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
            tag->v = strdup(v);
        }
    }

    return tag;
}

static inline struct um_tag *
__tag_set(struct um_user *user, char *k, char *v)
{
    return __tag_insert(user, k, v, true);
}

static inline void
__user_tag_clear(struct um_user *user)
{
    struct um_tag *p, *n;
    
    list_for_each_entry_safe(p, n, &user->head.tag, tag) {
        __tag_free(p);
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

    __online_idle(user, um_flow_type_lan) = UMD_IDLE;
    __online_idle(user, um_flow_type_wan) = UMD_IDLE;
    
    flow_reset(user, um_flow_type_lan);
    flow_reset(user, um_flow_type_wan);

    __user_tag_clear(user);
    
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

    __user_tag_clear(user);
    
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

    if (is_noused(user)) {
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
__set_group(struct um_user *user, int group)
{
    if (NULL==user) {
        return;
    }
    else if (group==user->group) {
        return;
    }
    
    user->group = group;
}

static void
__set_reason(struct um_user *user, int reason)
{
    if (NULL==user) {
        return;
    }
    else if (reason==user->reason) {
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

static int
__user_delete(struct um_user *user, event_cb_t *cb)
{
    if (NULL==user) {
        return -ENOEXIST;
    }

    __user_debug("before-user-delete", user);
    
    if (cb) {
        (*cb)(user);
    }
    
    __user_tag_clear(user);
    __user_remove(user);
    os_free(user);
    
    return 0;
}

static struct um_user *
__user_create(byte mac[], event_cb_t *cb)
{
    struct um_user *user;

    user = (struct um_user *)os_zalloc(sizeof(*user));
    if (NULL==user) {
        return NULL;
    }
    os_maccpy(user->mac, mac);
    
    INIT_LIST_HEAD(&user->head.tag);

    __set_state(user, UM_STATE_NONE);
    lan_online(user);
    user->create = user->noused = time(NULL);
    
    __user_insert(user);

    if (cb) {
        (*cb)(user);
    }

    __user_debug("after-user-create", user);
    
    return user;
}

static int
__user_deauth(struct um_user *user, int reason, event_cb_t *cb)
{
    if (NULL==user) {
        return -ENOEXIST;
    }
    else if (false==is_auth(user)) {
        return 0;
    }

    __user_debug("before-user-deauth", user);
    /*
    * 1. save reason
    * 2. callback
    * 3. deauth
    */
    __set_reason(user, reason);

    if (cb) {
        (*cb)(user);
    }
    
    /*
    * auth==>bind
    */
    __set_state(user, UM_STATE_BIND);
    __set_group(user, 0);
    
    wan_offline(user);

    __user_debug("after-user-deauth", user);
    
    return 0;
}

static int
__user_unbind(struct um_user *user, int reason, event_cb_t *cb)
{
    int err = 0;

    err = __user_deauth(user, reason, deauth_cb);
    if (err<0) {
        return err;
    }

    if (false==is_bind(user)) {
        return 0;
    }

    __user_debug("before-user-unbind", user);
    
    /*
    * 1. save reason
    * 2. callback
    * 3. unbind
    */
    __set_reason(user, reason);

    if (cb) {
        (*cb)(user);
    }

    __set_ip(user, 0);
    /*
    * bind==>connect
    */
    __set_state(user, UM_STATE_NONE);
    
    lan_offline(user);

    __user_debug("after-user-unbind", user);
    
    return 0;
}

#if UM_USE_MONITOR
static int
__user_leave(struct um_user *user, event_cb_t *cb)
{
    if (NULL==user) {
        return -ENOEXIST;
    }

    // do leave
    user->flags &= ~UM_F_MONITOR;

    if (is_noused(user)) {
        user->noused = time(NULL);
    }

    if (cb) {
        (*cb)(user);
    }

    return 0;
}

static struct um_user *
__user_enter(struct um_user *user, jobj_t obj, event_cb_t *cb)
{
    if (NULL==user) {
        return NULL;
    }

    // do enter
    user->flags |= UM_F_MONITOR;
    
    if (cb) {
        (*cb)(user);
    }

    return user;
}
#endif

static struct um_user *
__user_bind(struct um_user *user, uint32 ip, event_cb_t *cb)
{
    if (NULL==user) {
        return NULL;
    }

    if (have_bind(user)) {
        if (ip==user->ip) {
            return user;
        } else {
            __user_unbind(user, UM_DEAUTH_AUTO, unbind_cb);
        }
    }

    __user_debug("before-user-bind", user);

    /*
    * 1. unbind
    * 2. callback
    */
    __set_ip(user, ip);
    __set_state(user, UM_STATE_BIND);
    lan_online(user);
    update_aging(user, true);

    if (cb) {
        (*cb)(user);
    }

    __user_debug("after-user-bind", user);

    return user;
}

static int
__user_reauth(struct um_user *user, event_cb_t *cb)
{
    if (NULL==user) {
        return -ENOEXIST;
    }
    else if (false==is_auth(user)) {
        return 0;
    }

    __user_debug("before-user-reauth", user);

    /*
    * 1. callback
    */
    if (cb) {
        (*cb)(user);
    }

    __user_debug("after-user-reauth", user);

    return 0;
}

static struct um_user *
__user_clone(struct um_user *user)
{
    struct um_user *new = (struct um_user *)os_malloc(sizeof(*new));
    if (new) {
        os_objcpy(new, user);
    }

    return new;
}

static struct um_user *
__user_rollback(struct um_user *dst, struct um_user *src)
{
    os_objcpy(dst, src);
    os_free(src);

    return dst;
}

static struct um_user *
__user_auth(struct um_user *user, int group, jobj_t obj, event_cb_t *cb)
{
    int err;
    
    if (NULL==user) {
        return NULL;
    }

    if (false==have_bind(user)) {
        char ipaddress[1+OS_LINE_LEN];
        
        err = os_v_pgets(ipaddress, OS_LINE_LEN, UMD_SCRIPT_IP " %s", os_macstring(user->mac));
        if (err<0) {
            return NULL;
        }
        else if (false==is_good_ipstring(ipaddress)) {
            return NULL;
        }
        else {
            __user_bind(user, inet_addr(ipaddress), bind_cb);
        }
    }

    __user_debug("before-user-auth", user);

    /*
    * 1. clone
    * 2. auth by clone
    * 3. callback
    * 4. rollback if cb failed
    */
    struct um_user backup = *user;

    /*
    * bind==>auth
    */
    __set_state(user, UM_STATE_AUTH);
    __set_group(user, group);
    __set_reason(user, UM_DEAUTH_NONE);
    
    update_limit(user, obj);
    wan_online(user);
    update_aging(user, true);
    
    if (cb) {
        err = (*cb)(user);
        if (err<0) {
            /*
            * rollback
            */
            *user = backup;

            debug_trace("user auth-cb err(%d), rollback", err);

            return NULL;
        }
    }

    __user_debug("after-user-auth", user);

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
        struct um_user *user = getuser_bynidx(node, UM_USER_NIDX_MAC);
        
        return os_maceq(user->mac, mac);
    }
    
    h2_node_t *node = h2_find(&umd.table, UM_USER_NIDX_MAC, dhash, eq);

    return node?getuser_bynode(node):NULL;
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
    return __user_create(mac, NULL);
}

int user_delete(struct um_user *user)
{
    return __user_delete(user, delete_cb);
}

#if UM_USE_MONITOR
static struct um_user *
user_enter(struct um_user *user, jobj_t obj)
{
    return __user_enter(user, obj, enter_cb);
}

static int
user_leave(struct um_user *user)
{
    return __user_leave(user, leave_cb);
}
#endif

static struct um_user *
user_bind(struct um_user *user, uint32 ip)
{
    return __user_bind(user, ip, bind_cb);
}

int user_unbind(struct um_user *user, int reason)
{
    if (is_valid_deauth_reason(reason)) {
        return __user_unbind(user, reason, unbind_cb);
    } else {
        return -EBADREASON;
    }
}

static struct um_user *
user_auth(struct um_user *user, int group, jobj_t obj)
{
    return __user_auth(user, group, obj, auth_cb);
}

int user_reauth(struct um_user *user)
{
    return __user_reauth(user, reauth_cb);
}

int user_deauth(struct um_user *user, int reason)
{
    if (is_valid_deauth_reason(reason)) {
        return __user_deauth(user, reason, deauth_cb);
    } else {
        return -EBADREASON;
    }
}

static int
user_foreach(um_foreach_f *foreach, bool safe)
{
    mv_t node_foreach(h2_node_t *node)
    {
        return (*foreach)(getuser_bynode(node));
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
    return __tag_get(__user_get(mac), k);
}

struct um_tag *
um_tag_set(byte mac[], char *k, char *v)
{
    return __tag_set(__user_get(mac), k, v);
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

#if UM_USE_MONITOR
struct um_user *
um_user_enter(byte mac[], jobj_t obj)
{
    return user_enter(um_user_create(mac), obj);
}

int um_user_leave(byte mac[])
{
    return user_leave(user_get(mac));
}
#endif

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
um_user_auth(byte mac[], int group, jobj_t obj)
{
    return user_auth(um_user_create(mac), group, obj);
}

int um_user_reauth(byte mac[])
{
    return user_reauth(user_get(mac));
}

int um_user_deauth(byte mac[], int reason)
{
    return user_deauth(user_get(mac), reason);
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
        struct um_user *user = getuser_bynidx(node, UM_USER_NIDX_IP);

        return ip==user->ip;
    }
    
    h2_node_t *node = h2_find(&umd.table, UM_USER_NIDX_IP, dhash, eq);
    if (node) {
        return getuser_bynode(node);
    }

    return NULL;
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

    jobj_add_i32(obj, "max",            __online_max(user, type));
    jobj_add_i32(obj, "idle",           __online_idle(user, type));
    jobj_add_i32(obj, "aging",          __online_aging(user, type));
    jobj_add_i32(obj, "numerator",      __online_numerator(user, type));
    jobj_add_i32(obj, "denominator",    __online_denominator(user, type));
    
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
    
    jobj_add_i64(obj, "max",          __flow_max(user, type, dir));
    jobj_add_i64(obj, "now",          __flow_now(user, type, dir));
    jobj_add_i64(obj, "numerator",    __flow_numerator(user, type, dir));
    jobj_add_i64(obj, "denominator",  __flow_denominator(user, type, dir));

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
    
    jobj_add_i32(obj, "max", __rate_max(user, type, dir));
    jobj_add_i32(obj, "avg", __rate_avg(user, type, dir));

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

    jobj_add_string(obj, "mac",     os_macstring(user->mac));
    jobj_add_string(obj, "ip",      os_ipstring(user->ip));
    jobj_add_string(obj, "state",   user_state_string(user->state));
    jobj_add_string(obj, "reason",  deauth_reason_string(user->reason));
    
    jobj_add_string(obj, "create",  os_fulltime_string(&user->create));
    jobj_add_string(obj, "noused",  os_fulltime_string(&user->noused));
    
    jobj_add_i32(obj,   "group",    user->group);
    jobj_add_bool(obj,  "monitor",  is_monitor(user));
    
    jobj_add(obj, "tag",    juser_tag(user));
    jobj_add(obj, "limit",  juser_limit(user));

    return obj;
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
    if (__have_bind(filter->state) && filter->state != user->state) {
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
