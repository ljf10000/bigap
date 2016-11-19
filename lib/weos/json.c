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
                japi_println("bool=%d", var.d);
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

static int
__jrule_repair(jobj_t jobj, jrule_t *rule, char *name, jobj_t jval)
{
    int err = 0;
    
    switch(rule->type) {
        case jtype_bool: {
            bool vb = (rule->flag & JRULE_DYNAMIC)?(*rule->deft.fb)():rule->deft.b;
            
        }   break;
        case jtype_int: {
            int32 vi = (rule->flag & JRULE_DYNAMIC)?(*rule->deft.fi)():rule->deft.i;
            
            err = jobj_add_i32(jobj, name, vi);
            
        }   break;
        case jtype_string: {
            char *vs = (rule->flag & JRULE_DYNAMIC)?(*rule->deft.fs)():rule->deft.s;
            
            err = jobj_add_string(jobj, name, vs);
            
        }   break;
        case jtype_object: {
            if (rule->deft.fo) {
                err = (*rule->deft.fo)(jval);
            }
            
        }   break;
        case jtype_array: {
            if (rule->deft.fa) {
                err = (*rule->deft.fa)(jval);
            }
            
        }   break;
        default:
            err = -ENOSUPPORT;
    }

    return err;
}

int
jrule_check(jrule_ops_t *ops)
{
    int id, err;
    char *name;
    jobj_t jval;
    jrule_t *rule;
    
    for (id=0; id<ops->count; id++) {
        rule = &ops->rule[id];

        if (os_hasflag(rule->flag, JRULE_CONST | JRULE_DYNAMIC)) {
            return -EBADRULE;
        }
        
        switch(rule->type) {
            case jtype_object:  // down
            case jtype_array:
                if (NULL==rule->deft.v) {
                    return -EBADRULE;
                }

                break;
            case jtype_bool:    // down
            case jtype_int:     // down
            case jtype_string:
                if (os_hasflag(rule->flag, JRULE_DYNAMIC) && NULL==rule->deft.v) {
                    return -EBADRULE;
                }
                
                break;
            case jtype_null:    // down
            case jtype_double:
            default:
                return -EBADRULE;
        }
    }

    return 0;
}

int
jrule_repair(jobj_t jobj, jrule_t *rule, char *name, jobj_t jval)
{
    /*
    * dynamic/const, 
    *   delete it from jobj
    *   set default
    */
    if ((JRULE_CONST | JRULE_DYNAMIC) & rule->flag) {
        jobj_del(jobj, name);
        
        return __jrule_repair(jobj, rule, name, jval);
    }
    /*
    * no value, set default
    */
    else if (NULL==jval) {
        return __jrule_repair(jobj, rule, name, jval);
    }

    return 0;
}

int
jrule_apply(jobj_t jobj, jrule_ops_t *ops, jrule_apply_f *apply)
{
    int id, err;
    char *name;
    jobj_t jval;
    jrule_t *rule;
    
    for (id=0; id<ops->count; id++) {
        if (false==(*ops->is_good_id)(id)) {
            return -EBADRULE;
        }
        
        name = (*ops->getnamebyid)(id);
        if (NULL==name) {
            return -EBADRULE;
        }
        rule = &ops->rule[id];
        
        jval = jobj_get(jobj, name);
        if (NULL==jval) {
            if (os_hasflag(rule->flag, JRULE_MUST)) {
                return -ENOMUST;
            }
        }
        else if (jobj_type(jval) != rule->type) {
            return -EBADJTYPE;
        }
        else if (apply) {
            err = (*apply)(jobj, rule, name, jval);
            if (err<0) {
                return err;
            }
        }
    }

    return 0;
}

#endif
/******************************************************************************/
