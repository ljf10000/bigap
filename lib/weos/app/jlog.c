/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
STATIC sockaddr_un_t *
__jlog_userver(void)
{
    return &__this_jlogger()->server.un;
}

STATIC sockaddr_in_t *
__jlog_iserver(void)
{
    return &__this_jlogger()->server.in;
}

STATIC sockaddr_t *
__jlog_server(void)
{
    return &__this_jlogger()->server.c;
}

STATIC jobj_t
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
    
    err = jobj_add_string(header, JLOG_KEY_TIME, os_fulltime_string(time(NULL)));
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
__jlog_socket(const char *app, const char *sub, int family)
{
    int fd, err, addrlen;
    
    fd = __this_jlogger()->fd;
    if (is_good_fd(fd)) {
        return fd;
    }
    
    fd = socket(family, SOCK_DGRAM, (AF_UNIX==family)?0:IPPROTO_UDP);
    if (fd<0) {
        __debug_trace("socket error:%d", -errno);
        
        return -errno;
    }
    os_closexec(fd);
        
#if use_jlog_bind
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

#if use_jlog_connect
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
__jlog(jobj_t obj, const char *app, const char *sub, uint32 PRI)
{
    int fd, len, err;
    
    char *json = NULL;
    
    if (NULL==obj) {
        __debug_error("nil obj");
        
        return -EINVAL0;
    }

    json = jobj_json(obj);
    if (NULL==json) {
        __debug_error("__jlog obj==>json failed");
        
        return -ENOMEM;
    }

    if (LOG_DEBUG==LOG_PRI(PRI) && __is_ak_debug(JLOG_LEVEL(PRI))) {
        os_eprintln(__tab "%s", json);
    }
    
    len = os_strlen(json);
    if (len<=0) {
        __debug_error("__jlog obj==>json(bad)");
        
        return -EINVAL1;
    }
    
    bool try = false;
    int family = __this_jlogger()->family;
    
try_again:
    fd = __jlog_socket(app, sub, family);
    if (false==is_good_fd(fd)) {
        return fd;
    }

#if use_jlog_connect
    err = send(fd, json, len, 0);
#else
    err = sendto(fd, json, len, 0, __jlog_server(), os_sockaddr_len(__jlog_userver()));
#endif
    if (false==try && EBADF==err) {
        /*
        * if jlogd re-start
        *   re-create client socket
        */
        try = true;

        __this_jlogger()->fd = INVALID_FD;
        
        goto try_again;
    }
    else if (err<0) { /* yes, <0 */
        __debug_trace("__jlog write error:%d", -errno);
        
        return -errno;
    }
    else if (err != len) {
        __debug_trace("__jlog write count(%d) < length(%d)", err, len);
        
        return -EIO;
    }
    
    return 0;
}

int
__jvlogger(
    const char *app, 
    const char *sub, 
    const char *file, 
    const char *func, 
    uint32 line, 
    uint32 PRI, 
    const char *fmt, 
    va_list args
)
{
    jobj_t obj = NULL;
    int err;
    
    obj = __jlog_add_header(NULL, app, sub, JLOG_KEY_TYPE_C, file, func, line, PRI);
    if (NULL==obj) {
        err = -ENOMEM; goto error;
    }
    
    err = jobj_vprintf(obj, fmt, args);
    if (err<0) {
        goto error;
    }
    
    err = __jlog(obj, app, sub, PRI);

error:
    jobj_put(obj);

    return err;
}

int
__dvlogger(
    const char *app, 
    const char *sub, 
    const char *file, 
    const char *func, 
    uint32 line, 
    uint32 PRI, 
    const char *fmt, 
    va_list args
)
{
    jobj_t obj = NULL;
    int err;
    
    obj = __jlog_add_header(NULL, app, sub, JLOG_KEY_TYPE_C, file, func, line, PRI);
    if (NULL==obj) {
        err = -ENOMEM; goto error;
    }

    err = jobj_vsprintf(obj, "__debugger__", fmt, args);
    if (err<0) {
        goto error;
    }
    
    err = __jlog(obj, app, sub, PRI);

error:
    jobj_put(obj);
    
    return err;
}

int
__jlogger(
    const char *app, 
    const char *sub, 
    const char *file, 
    const char *func, 
    uint32 line, 
    uint32 PRI, 
    const char *fmt, 
    ...
)
{
    int err = 0;
    va_list args;
    
    va_start(args, (char *)fmt);
    err = __jvlogger(app, sub, file, func, line, PRI, fmt, args);
    va_end(args);
    
    return err;
}

int
__dlogger(
    const char *app, 
    const char *sub, 
    const char *file, 
    const char *func, 
    uint32 line, 
    uint32 PRI, 
    const char *fmt, 
    ...
)
{
    int err = 0;
    va_list args;
    
    va_start(args, (char *)fmt);
    err = __dvlogger(app, sub, file, func, line, PRI, fmt, args);
    va_end(args);
    
    return err;
}

int
__clogger(
    const char *app, 
    const char *sub, 
    const char *file, 
    const char *func, 
    uint32 line,
    uint32 PRI,
    char *json
)
{
    jobj_t obj = NULL;
    int err;

    obj = jobj_byjson(json);
    if (NULL==obj) {
        err = -EBADJSON; goto error;
    }

    obj = __jlog_add_header(obj, app, sub, JLOG_KEY_TYPE_SH, file, func, line, PRI);
    if (NULL==obj) {
        err = -EFORMAT; goto error;
    }
    
    err = __jlog(obj, app, sub, PRI);
error:
    jobj_put(obj);

    return err;
}

int
__jformat(
    const char *app, 
    const char *sub, 
    const char *file, 
    const char *func, 
    uint32 line, 
    uint32 PRI, 
    const char *fmt, 
    int argc,
    char *argv[]
)
{
    int err = 0;
    
    jobj_t obj = __jlog_add_header(NULL, app, sub, JLOG_KEY_TYPE_SH, file, func, line, PRI);
    if (NULL==obj) {
        err = -ENOMEM; goto error;
    }
    
    err = jobj_exec(obj, fmt, argc, argv);
    if (err<0) {
        goto error;
    }
    
    err = __jlog(obj, app, sub, PRI);

error:
    jobj_put(obj);

    return err;
}

int
__jlog_env_init(void)
{
    int family, err;
    
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

            ip = inet_addr(ipaddress);
            if (INADDR_NONE==ip) {
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
