/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifdef __APP__

void
jobj_add(jobj_t obj, char *k, jobj_t v)
{
    if (obj && v) {
        switch(jobj_type(obj)) {
            case jtype_object:
                if (k) {
                    json_object_object_add(obj, k, v);
                }

                break;
            case jtype_array:
                if (NULL==k) {
                    json_object_array_add(obj, v);
                } else {
                    json_object_array_put_idx(obj, (int)(uintptr_t)k, v);
                }
                
                break;
            default:
                break;
        }       
    }
}

int
jobj_vsprintf(jobj_t obj, const char *key, const char *fmt, va_list args)
{
    int len, err = 0;
    char *line = (char *)os_zalloc(1+OS_BIG_LEN);
    if (NULL==line) {
        err = -ENOMEM; goto error;
    }

    len = os_vsnprintf(line, OS_BIG_LEN, fmt, args);
    if (os_snprintf_is_full(OS_BIG_LEN, len)) {
        err = -ENOSPACE;
    } else {
        err = jobj_add_string(obj, key, line);
	}

error:
    if (line) {
        os_free(line);
    }

    return err;
}

int
jobj_sprintf(jobj_t obj, const char *key, const char *fmt, ...)
{
    va_list args;
    int err;
    
    va_start(args, fmt);
    err = jobj_vsprintf(obj, key, fmt, args);
    va_end(args);
    
    return err;    
}

jobj_t
jobj_byjson(char *json)
{
    int err = 0;
    jtok_t tok = NULL;
    jobj_t obj = NULL;

    if (false==is_good_str(json)) {
        return NULL;
    }
    
    tok = jtok_new();
    if (NULL==tok) {
        japi_println("create tok error");
        
        err = -ENOMEM; goto error;
    }
    
    jtok_set_flags(tok, JSON_TOKENER_STRICT);
    
    obj = jtok_parse_ex(tok, json, os_strlen(json));
    if (NULL==tok) {
        japi_println("tok parase error");
        
        err = -ENOMEM; goto error;
    }
    
error:
    jtok_free(tok);

    return obj;
}

int
jobj_add_json(jobj_t obj, char *key, char *value)
{
    int err = 0;
    jobj_t new = NULL;

    if (NULL==key) {
        return -EKEYNULL;
    }
    else if (false==is_good_str(value)) {
        return 0;
    }

    new = jobj_byjson(value);
    if (NULL==new) {
        return -EBADJSON;
    }

    /*
    * new is add to obj,
    *   needn't to free new
    */
    jobj_add(obj, key, new);

    return 0;
}

jobj_t
jarray_clone(jobj_t jobj, bool keepsort)
{
    int i, count = jarray_length(jobj);
    jobj_t jsub;
    jobj_t jnew = jobj_new_array();
    
    for (i=0; i<count; i++) {
        jsub = jarray_get(jobj, i);
        if (jsub) {
            if (keepsort) {
                jarray_set(jnew, i, jobj_clone(jsub, keepsort));
            }
            else if (jtype_null!=jobj_type(jsub)) {
                jarray_add(jnew, jobj_clone(jsub, keepsort));
            }
        }
    }

    return jnew;
}

jobj_t
jobj_clone(jobj_t jobj, bool keepsort)
{
    jobj_t jv;
    jobj_t jnew = jobj_new_object();
    if (NULL==jnew) {
        return NULL;
    }

    jobj_foreach(jobj, k, v) {
        switch(jobj_type(v)) {
            case jtype_bool:
                jobj_add_bool(jnew, k, jobj_get_bool(v));
                
                break;
            case jtype_double:
                jobj_add_f64(jnew, k, jobj_get_f64(v));
                
                break;
            case jtype_int:
                jobj_add_i32(jnew, k, jobj_get_i32(v));
                
                break;
            case jtype_string:
                jobj_add_string(jnew, k, jobj_get_string(v));
                
                break;
            case jtype_object:
                jv = jobj_clone(v, keepsort);

                jobj_add(jnew, k, jv);
                
                break;
            case jtype_array:
                jv = jarray_clone(v, keepsort);

                jobj_add(jnew, k, jv);
                
                break;
            case jtype_null:
            default:
                goto error;
        }

        
    }

    return jnew;
error:
    jobj_put(jnew);
    
    return NULL;
}

bool
jobj_format_is_valid(int c, int valid[], int count)
{
    int i;

    for (i=0; i<count; i++) {
        if (c!=valid[i]) {
            return false;
        }
    }

    return true;
}

int
jobj_exec(jobj_t obj, const char *fmt, int argc, char *argv[])
{
    char *key;
    jvar_t var;
    int err = 0, idx = 0, count = 0;

    if (os_strlen(fmt)%2) {
        japi_println("bad format:%s", fmt);
        
        return -EFORMAT;
    }
    
    char *p = (char *)fmt;
    while(*p) {
        if ('%' == *p++) {
            key = argv[idx++];
        } else {
            japi_println("bad format:%s", fmt);
            
            return -EFORMAT;
        }

        switch(*p++) {
            case jfmt_bool:
                var.b = os_atoi(argv[idx++]);
                japi_println("bool=%d", var.b);
                err = jobj_add_bool(obj, key, var.b);
                
                break;
            case jfmt_int:
                var.d = os_atoi(argv[idx++]);
                japi_println("int=%d", var.d);
                err = jobj_add_i32(obj, key, var.d);

                break;
            case jfmt_long:
                var.l = os_atoll(argv[idx++]);
                japi_println("int64=%lld", var.l);
                err = jobj_add_i64(obj, key, var.l);
                
                break;
            case jfmt_double:
                var.f = os_atof(argv[idx++]);
                japi_println("float64=%lf", var.f);
                err = jobj_add_f64(obj, key, var.f);
                
                break;
            case jfmt_string:
                var.s = argv[idx++];
                if (NULL==var.s) {
                    var.s = __empty;
                }
                japi_println("string=%s", var.s);
                err = jobj_add_string(obj, key, var.s);
                
                break;
            case jfmt_json:
                var.j = argv[idx++];
                if (NULL==var.j) {
                    var.j = "{}";
                }
                japi_println("json=%s", var.j);
                jobj_add_json(obj, key, var.j);

                break;
            default:
                japi_println("no support format:%s", fmt);
                
                return -ENOSUPPORT;
        }

        if (err<0) {
            return err;
        }

        count++;
    }

    if (2*count != argc) {
        japi_println("bad format:%s", fmt);
        
        return -EFORMAT;
    }
    
    return 0;
}

int
jobj_vprintf(jobj_t obj, const char *fmt, va_list args)
{
    char *key, *p;
    jvar_t var;
    int err = 0;

    if (os_strlen(fmt)%2) {
        japi_println("bad format:%s", fmt);
        
        return -EFORMAT;
    }
    
    p = (char *)fmt;
    while(*p) {
        if ('%' == *p++) {
            key = va_arg(args, char *);
        } else {
            japi_println("bad format:%s", fmt);
            
            return -EFORMAT;
        }

        switch(*p++) {
            case jfmt_bool:
                var.b = va_arg(args, bool);
                japi_println("bool=%d", var.b);
                err = jobj_add_bool(obj, key, var.b);
                
                break;
            case jfmt_int:
                var.d = va_arg(args, int32);
                japi_println("int=%d", var.d);
                err = jobj_add_i32(obj, key, var.d);

                break;
            case jfmt_long:
                var.l = va_arg(args, int64);
                japi_println("int64=%lld", var.l);
                err = jobj_add_i64(obj, key, var.l);
                
                break;
            case jfmt_double:
                var.f = va_arg(args, float64);
                japi_println("float64=%lf", var.f);
                err = jobj_add_f64(obj, key, var.f);
                
                break;
            case jfmt_string:
                var.s = va_arg(args, char *);
                if (NULL==var.s) {
                    var.s = __empty;
                }
                japi_println("string=%s", var.s);
                err = jobj_add_string(obj, key, var.s);
                
                break;
            case jfmt_array:
                var.a = va_arg(args, jobj_t);
                if (NULL==var.a) {
                    var.a = jobj_new_object();
                }
                japi_println("array=%s", jobj_json(var.a));
                jobj_add(obj, key, var.a);

                break;
            case jfmt_object:
                var.o = va_arg(args, jobj_t);
                if (NULL==var.o) {
                    var.o = jobj_new_object();
                }
                japi_println("object=%s", jobj_json(var.o));
                jobj_add(obj, key, var.o);

                break;
            case jfmt_json:
                var.j = va_arg(args, char *);
                if (NULL==var.j) {
                    var.j = "{}";
                }
                japi_println("json=%s", var.j);
                jobj_add_json(obj, key, var.j);

                break;
            default:
                japi_println("no support format:%s", fmt);
                
                return -ENOSUPPORT;
        }

        if (err<0) {
            return err;
        }
    }

    return 0;
}

int
jobj_printf(jobj_t obj, const char *fmt, ...)
{
    int err = 0;
    va_list args;
    
    va_start(args, (char *)fmt);
    err = jobj_vprintf(obj, fmt, args);
    va_end(args);
    
    return err;
}

jobj_t
jobj_voprintf(const char *fmt, va_list args)
{
    jobj_t obj = jobj_new_object();
    if (NULL==obj) {
        return NULL;
    }

    int err = jobj_vprintf(obj, fmt, args);
    if (err<0) {
        return obj;
    }
    
    return obj;
}

jobj_t
jobj_oprintf(const char *fmt, ...)
{
    jobj_t obj;
    va_list args;
    
    va_start(args, (char *)fmt);
    obj = jobj_voprintf(fmt, args);
    va_end(args);
    
    return obj;
}

bool
is_good_json(char *json)
{
    jobj_t obj = jobj_byjson(json);
    if (obj) {
        jobj_put(obj);

        return true;
    } else {
        return false;
    }
}

jobj_t
jobj_get_vleaf(jobj_t obj, va_list args)
{
    jobj_t root = obj, leaf = NULL;
    char *key = va_arg(args, char *);
    
    while(key) {
        root = leaf = jobj_get(root, key);

        key = va_arg(args, char *);
    }
    
    return leaf;
}

jobj_t
jobj_get_leaf(jobj_t obj, ...)
{
    jobj_t leaf;
    va_list args;
    
    va_start(args, (char *)obj);
    leaf = jobj_get_vleaf(obj, args);
    va_end(args);
    
    return leaf;
}

jobj_t 
__jobj_map(jobj_t jobj, jobj_mapper_f *map[], int count)
{
    int i, err = 0;

    if (jobj) {
        for (i=0; i<count; i++) {
            err = (*map[i])(jobj);
            if (err<0) {
                return NULL;
            }
        }
    }
    
    return jobj;
}

#if USE_JRULE
jrule_t *
jrule_getbyname(jrule_t *rules, char *name)
{
    jrule_t *rule;

    JRULE_FOREACH(rule, rules) {
        if (os_streq(name, rule->name)) {
            return rule;
        }
    }

    return NULL;
}

bool
is_good_jrule_flag(int flag)
{
    static int flags[] = { JURLE_VALID_FLAGS };
    int i;

    for (i=0; i<os_count_of(flags); i++) {
        if (os_hasflag(flag, flags[i])) {
            return true;
        }
    }

    return false;
}

STATIC int
__jrule_selfcheck(jrule_t *rule)
{
    if (false==is_good_jrule_flag(rule->flag)) {
        return -EBADRULE;
    }

    switch(rule->type) {
        case jtype_bool:    // down
        case jtype_int:     // down
        case jtype_double:  // down
        case jtype_i32:     // down
        case jtype_u32:     // down
        case jtype_f32:     // down
        case jtype_i64:     // down
        case jtype_u64:     // down
        case jtype_f64:
            if (os_hasflag(rule->flag, JRULE_CHECKER) && NULL==rule->serialize.check) {
                return -EBADRULE;
            }
            
            break;
        case jtype_enum:
            if (NULL==rule->serialize.get_enum_ops) {
                return -EBADRULE;
            }
            
            break;
        case jtype_string:
            if (NULL==rule->unserialize.j2o) {
                return -EBADRULE;
            }

            break;
        case jtype_object:
            if (NULL==rule->deft.get_rules) {
                return -EBADRULE;
            }
            
            break;
        case jtype_array:
            if (NULL==rule->serialize.o2j ||
                NULL==rule->unserialize.j2o ||
                NULL==rule->deft.get_rules) {
                return -EBADRULE;
            }

            break;
        case jtype_null: // down
        default:
            return -EBADRULE;
    }

    return 0;
}

int
jrule_selfcheck(jrule_t *rules)
{
    jrule_t *rule;
    int err;
    
    JRULE_FOREACH(rule, rules) {
        err = __jrule_selfcheck(rule);
        if (err<0) {
            return err;
        }
    }

    return 0;
}

STATIC int
__jrule_o2j(jrule_t *rule, void *obj, jobj_t jobj)
{
    char *member = JRULE_OBJ_MEMBER_ADDRESS(rule, obj);
    jobj_t jval;
    int err;
    
    switch(rule->type) {
        case jtype_bool:
            jobj_add_bool(jobj, rule->name, *(bool *)member);
            break;
        case jtype_int:
            jobj_add_int(jobj, rule->name, *(int *)member);
            break;
        case jtype_double:
            jobj_add_double(jobj, rule->name, *(double *)member);
            break;
        case jtype_i32:
            jobj_add_i32(jobj, rule->name, *(int32 *)member);
            break;
        case jtype_u32:
            jobj_add_u32(jobj, rule->name, *(uint32 *)member);
            break;
        case jtype_f32:
            jobj_add_f32(jobj, rule->name, *(float32 *)member);
            break;
        case jtype_i64:
            jobj_add_i64(jobj, rule->name, *(int64 *)member);
            break;
        case jtype_u64:
            jobj_add_u64(jobj, rule->name, *(uint64 *)member);
            break;
        case jtype_f64:
            jobj_add_f64(jobj, rule->name, *(float64 *)member);
            break;
        case jtype_enum: {
            enum_ops_t *ops = rule->serialize.get_enum_ops();

            jobj_add_string(jobj, rule->name, ops->getname(*(int *)member));
        }   break;
        case jtype_string:
            jobj_add_string(jobj, rule->name, member);

            break;
        case jtype_object:
            jval = jobj_new_object();
            jobj_add(jobj, rule->name, jval);
            
            err = jrule_o2j(rule->deft.get_rules(), member, jval);
            if (err<0) {
                return err;
            }

            break;
        case jtype_array:
            err = rule->serialize.o2j(rule, obj, jobj);
            if (err<0) {
                return err;
            }

            break;
        case jtype_null: // down
        default:
            return -EBADRULE;
    }

    return 0;
}

#define JRULE_AUTOMIC_J2O(_long_type, _short_type, _member) do{ \
    _long_type v = jobj_get_##_short_type(jval);    \
                                                    \
    if (border) {                                   \
        err = JRULE_BORDER_CHECK(v, rule, _member); \
        if (err<0) {                                \
            return err;                             \
        }                                           \
    }                                               \
                                                    \
    if (obj) {                                      \
        *(_long_type *)member = v;                  \
    }                                               \
} while(0)

STATIC int
__jrule_j2o(jrule_t *rule, void *obj, jobj_t jobj)
{
    jobj_t jval = jobj_get(jobj, rule->name);
    int jtype = jobj_type(jval);
    int err;
    
    if (os_hasflag(rule->flag, JRULE_DROP)) {
        debug_trace("drop json key %s", rule->name);
        
        jobj_del(jobj, rule->name);
        
        return 0;
    }
    else if (os_hasflag(rule->flag, JRULE_MUST)) {
        debug_trace("no-found json key %s", rule->name);
        
        return -ENOEXIST;
    }
    else if (os_hasflag(rule->flag, JRULE_CHECKER)) {
        err = (*rule->serialize.check)(rule, jobj);
        if (err<0) {
            return 0;
        }
    }

    bool border = os_hasflag(rule->flag, JRULE_BORDER);
    char *member = JRULE_OBJ_MEMBER_ADDRESS(rule, obj);

    switch(rule->type) {
        case jtype_bool:
            if (jtype_bool != jtype) {
                return -EBADJSON;
            }

            if (obj) {
                *(bool *)member = jobj_get_bool(jval);
            }
            
            break;
        case jtype_int:
            if (jtype_int != jtype) {
                return -EBADJSON;
            }
            
            JRULE_AUTOMIC_J2O(int, int, i);
            
            break;
        case jtype_double:
            if (jtype_double != jtype) {
                return -EBADJSON;
            }
            
            JRULE_AUTOMIC_J2O(double, double, d);
            
            break;
        case jtype_i32:
            if (jtype_int != jtype) {
                return -EBADJSON;
            }
            
            JRULE_AUTOMIC_J2O(int32, i32, i32);
            
            break;
        case jtype_u32:
            if (jtype_int != jtype) {
                return -EBADJSON;
            }
            
            JRULE_AUTOMIC_J2O(uint32, u32, u32);
            
            break;
        case jtype_f32:
            if (jtype_double != jtype) {
                return -EBADJSON;
            }
            
            JRULE_AUTOMIC_J2O(float32, f32, f32);
            
            break;
        case jtype_i64:
            if (jtype_int != jtype) {
                return -EBADJSON;
            }
            
            JRULE_AUTOMIC_J2O(int64, i64, i64);
            
            break;
        case jtype_u64:
            if (jtype_int != jtype) {
                return -EBADJSON;
            }
            
            JRULE_AUTOMIC_J2O(uint64, u64, u64);
            
            break;
        case jtype_f64:
            if (jtype_double != jtype) {
                return -EBADJSON;
            }
            
            JRULE_AUTOMIC_J2O(float64, f64, f64);
            
            break;
        case jtype_enum: {
            if (jtype_string != jtype) {
                return -EBADJSON;
            }
            
            enum_ops_t *ops = rule->serialize.get_enum_ops();
            int id = ops->getid(jobj_get_string(jval));
            if (false==ops->is_good(id)) {
                return -EBADENUM;
            }
                
            if (obj) {
                *(int *)member = id;
            }
            
        }   break;
        case jtype_string:
            if (jtype_string != jtype) {
                return -EBADJSON;
            }
            
            if (obj) {
                err = rule->serialize.j2o(rule, obj, jobj);
                if (err<0) {
                    return err;
                }
            }
            
            break;
        case jtype_object:
            if (jtype_object != jtype) {
                return -EBADJSON;
            }
            
            if (obj) {
                err = jrule_j2o(rule->deft.get_rules(), member, jval);
                if (err<0) {
                    return err;
                }
            }
            
            break;
        case jtype_array:
            if (jtype_array != jtype) {
                return -EBADJSON;
            }
            
            if (obj) {
                err = rule->unserialize.j2o(rule, obj, jobj);
                if (err<0) {
                    return err;
                }
            }
            
            break;
        case jtype_null: // down
        default:
            return -EBADRULE;
    }

    return 0;
}
#undef JRULE_AUTOMIC_J2O

STATIC int
jrules_apply(jrule_t *rules, void *obj, jobj_t jobj, int (*apply)(jrule_t *rule, void *obj, jobj_t jobj))
{
    jrule_t *rule;
    int err;

    JRULE_FOREACH(rule, rules) {
        err = (*apply)(rule, obj, jobj);
        if (err<0) {
            return 0;
        }
    }
    
    return 0;
}

int
jrule_o2j(jrule_t *rules, void *obj, jobj_t jobj)
{
    if (NULL==rules) {
        return -EINVAL0;
    }
    else if (NULL==obj) {
        return -EINVAL1;
    }
    else if (NULL==jobj) {
        return -EINVAL2;
    }
    
    return jrules_apply(rules, obj, jobj, __jrule_o2j);
}

int
jrule_j2o(jrule_t *rules, void *obj, jobj_t jobj)
{
    if (NULL==rules) {
        return -EINVAL3;
    }
    else if (NULL==jobj) {
        return -EINVAL4;
    }
    
    return jrules_apply(rules, obj, jobj, __jrule_j2o);
}
#endif /* USE_JRULE */

#endif
/******************************************************************************/
