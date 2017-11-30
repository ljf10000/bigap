/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
jlog_control_t __THIS_JLOG = JLOG_CONTROL_INITER;

jlog_control_t *
__this_jlogger(void)
{
    return &__THIS_JLOG;
}

jobj_t
__jlog_add_header(
    jobj_t obj, 
    const char *app, 
    const char *sub, 
    const char *type,
    const char *file, 
    const char *func, 
    uint32 line, 
    uint32 PRI
)
{
    jobj_t header = NULL;
    int pri     = LOG_PRI(PRI);
    int level   = JLOG_LEVEL(PRI);
    int err;
    
    if (NULL==obj) {
        obj = jobj_new_object();
        if (NULL==obj) {
            __debug_error("create obj error");
            
            goto error;
        }
    }

    header = jobj_new_object();
    if (NULL==header) {
        __debug_error("create obj header error");
            
        goto error;
    }
    jobj_add(obj, JLOG_KEY_HEADER, header);

    err = jobj_add_string(header, JLOG_KEY_APP, app);
    if (err<0) {
        __debug_error("add app %s error", app);
        
        goto error;
    }
    
    err = jobj_add_string(header, JLOG_KEY_SUB, sub);
    if (err<0) {
        __debug_error("add sub %s error", sub);
        
        goto error;
    }
    
    err = jobj_add_string(header, JLOG_KEY_TYPE, type);
    if (err<0) {
        __debug_error("add type %s error", type);
        
        goto error;
    }
    
    err = jobj_add_string(header, JLOG_KEY_FILE, file);
    if (err<0) {
        __debug_error("add file %s error", file);
        
        goto error;
    }
    
    err = jobj_add_string(header, JLOG_KEY_FUNC, func);
    if (err<0) {
        __debug_error("add func %s error", func);
        
        goto error;
    }
    
    if (line) {
        err = jobj_add_u32(header, JLOG_KEY_LINE, line);
        if (err<0) {
            __debug_error("add line %d error", line);
            
            goto error;
        }
    }

    if (level) {
        char *name = __ak_debug_getname(level);
        
        err = jobj_add_string(header, JLOG_KEY_LEVEL, name);
        if (err<0) {
            __debug_error("add level %s error", name);
            
            goto error;
        }
    }
    
    err = jobj_add_u32(header, JLOG_KEY_PRI, pri);
    if (err<0) {
        __debug_error("add pri %d error", pri);
        
        goto error;
    }
    
    err = jobj_add_string(header, JLOG_KEY_TIME, unsafe_fulltime_string(time(NULL)));
    if (err<0) {
        __debug_error("add time error");
        
        goto error;
    }
        
    return obj;
error:
    jobj_put(obj);

    return NULL;
}

STATIC int
__jlog_socket_init(const char *app, const char *sub, int family)
{
    int fd, err, addrlen;
    
    fd = socket(family, SOCK_DGRAM, (AF_UNIX==family)?0:IPPROTO_UDP);
    if (fd<0) {
        __debug_trace("socket error:%d", -errno);
        
        return -errno;
    }
    os_closexec(fd);
        
#if USE_JLOG_BIND
    os_sockaddr_t client = OS_SOCKADDR_INITER(family);
    
    if (AF_UNIX==family) {
        os_saprintf(get_abstract_path(&client.un), "/tmp/.jlog.%s.%s.%d.unix", app, sub?sub:"-", getpid());

        addrlen = get_abstract_sockaddr_len(&client.un);
    }
    else {
        addrlen = sizeof(sockaddr_in_t);
    }
    
    err = bind(fd, &client.c, addrlen);
    if (err<0) {
        __debug_trace("bind error:%d", -errno);
        err = -errno; goto error;
    }
#endif

#if USE_JLOG_CONNECT
    if (AF_UNIX==family) {
        addrlen = get_abstract_sockaddr_len(__jlog_userver());
    } else {
        addrlen = sizeof(sockaddr_in_t);
    }
    
    err = connect(fd, __jlog_server(), addrlen);
    if (err<0) {
        __debug_trace("connect jlog error:%d", -errno);
        err = -errno; goto error;
    }
#endif

    __this_jlogger()->fd = fd;

    return fd;
error:
    os_close(fd);

    return err;
}

int
__jlog_socket(const char *app, const char *sub, int family)
{
    int fd = __this_jlogger()->fd;
    if (is_good_fd(fd)) {
        return fd;
    } else {
        return __jlog_socket_init(app, sub, family);
    }
}

void
__jlog_close(void)
{
    os_close(__this_jlogger()->fd);
}

int
__jlog_init(void)
{
    int family, err;

    if (__this_jlogger()->inited) {
        return 0;
    }

    __this_jlogger()->inited = true;
    
    family  = __jlog_server()->sa_family
            = __this_jlogger()->family
            = env_geti(OS_ENV(FAMILY), JLOG_FAMILY);

    switch(family) {
        case AF_UNIX: {
            err = __env_copy(OS_ENV(UNIX), JLOG_UNIX, 
                    get_abstract_path(__jlog_userver()),
                    abstract_path_size);
            if (err<0) {
                return err;
            }
            
        }   break;
        case AF_INET: {
            uint32 ip;
            char ipaddress[32] = {0};
            sockaddr_in_t *iserver = __jlog_iserver();
            
            iserver->sin_port = env_geti(OS_ENV(PORT), JLOG_PORT);
            __debug_ok("get port:%d", iserver->sin_port);
            
            err = env_copy(OS_ENV(SERVER), JLOG_SERVER, ipaddress);
            if (err<0) {
                __debug_error("get jlog ip error:%d", -errno);
                
                return err;
            } else {
                __debug_ok("get jlog ip:%s", ipaddress);
            }

            ip = os_ipaddr(ipaddress);
            if (false==is_good_ipaddr(ip)) {
                return -EFORMAT;
            }
            
            iserver->sin_addr.s_addr = ip;
            
        }   break;
        default:
            return -ENOSUPPORT;
    }

    return 0;
}
/******************************************************************************/
