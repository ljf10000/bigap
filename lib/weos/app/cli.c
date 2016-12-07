/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
ALWAYS_INLINE void
__this_cli_init(cli_t *cli)
{
    if (NULL==cli->b) {
        cli->b = (cli_buffer_t *)os_zalloc(1+CLI_BUFFER_LEN);
        if (NULL==cli->b) {
            return NULL;
        }
        
        cli->addr.sun_family = AF_UNIX;
        cli->addrlen = sizeof(sockaddr_un_t);
        cli->fd = INVALID_FD;
    }
}

cli_t *
__this_cli(void)
{
    static cli_t __THIS_CLI;
    
    cli_t *cli = &__THIS_CLI;

    __this_cli_init();
    
    return cli;
}

int
__clib_show(void)
{
    if (0==__clib_err && __clib_len && is_good_str(__clib_buf)) {
        __clib_cut(__clib_len);
        
        os_printf("%s", __clib_buf);

        os_objzero(__clib());
    }

    return __clib_err;
}

int
__cli_reply(int err)
{
    cli_t *cli = __this_cli();
    int len;
    
    debug_cli("send reply[len=%d, err=%d]:%s", __clib_len, err, __clib_buf);

    __clib_err = err;
    len = io_send(cli->fd, __clib(), __clib_space);
    __clib_clear();
    
    return len;
}

int
cli_vsprintf(const char *fmt, va_list args)
{
    va_list copy;

    va_copy(copy, args);
    int vsize = os_vsprintf_size((char *)fmt, copy);
    va_end(copy);

    if (__clib_left < vsize) {
        __cli_reply(0);
    }

    if (__clib_left < vsize) {
        return -ENOSPACE;
    }

    uint32 left = __clib_left;
    int len = os_vsnprintf(__clib_cursor, left, fmt, args);
    if (len<0) {
        return -errno;
    }
    else if (len > left) {
        return -ENOSPACE;
    }

    __clib_len += len;
    __clib_cursor[0] = 0;

    return len;
}

int
cli_sprintf(const char *fmt, ...)
{
    va_list args;

    va_start(args, (char *)fmt);
    int len = cli_vsprintf(fmt, args);
    va_end(args);

    return len;
}

int
__clic_fd(cli_client_t *clic)
{
    int fd = INVALID_FD, err = 0;
    int type;
    
    sockaddr_un_t *client = &clic->client;
    abstract_path_sprintf(client, CLI_CLIENT_UNIX, getpid());
    clic->timeout = env_geti(OS_ENV(TIMEOUT), CLI_TIMEOUT);

    fd = socket(AF_UNIX, CLI_SOCK_TYPE, 0);
    if (fd<0) {
        debug_error("socket error:%d", -errno);
        return -errno;
    }
    os_closexec(fd);

    struct timeval timeout = OS_TIMEVAL_INITER(os_second(clic->timeout), os_usecond(clic->timeout));
    
    err = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
    if (err<0) {
        debug_error("setsockopt SO_RCVTIMEO error:%d", -errno);
        return -errno;
    }

    err = bind(fd, (sockaddr_t *)client, get_abstract_sockaddr_len(client));
    if (err<0) {
        debug_error("bind(%s) error:%d", get_abstract_path(client), -errno);
        return -errno;
    }

    sockaddr_un_t *server = &clic->server;
    err = connect(fd, (sockaddr_t *)server, get_abstract_sockaddr_len(server));
    if (err<0) {
        debug_error("connect(%s) error:%d", get_abstract_path(server), -errno);
        return -errno;
    }

    return fd;
}

int
__clic_recv(int fd, int timeout)
{
    cli_t *cli = __this_cli();
    int err = 0, len;

    while(1) {
        len = sizeof(cli_header_t);
        err = __io_recv(fd, __clib(), len, 0);
        if (err==len) {
            // is last
            if (0==__clib_len) {
                return __clib_err;
            }

            len = __clib_len;
            err = __io_recv(fd, __clib_buf, len, 0);
            if (err==len) {
                __clib_show();
            } else {
                goto error;
            }
        } else {
error:
            if (err > len) {
                return -ETOOBIG;
            }
            else if (err>0) {
                return -ETOOSMALL;
            }
            // err = 0
            else if (0==err) {
                return __clib_err;
            }
            else if (EINTR==errno) {
                continue;
            }
            else if (EAGAIN==errno) {
                /*
                * timeout
                */
                return -ETIMEOUT;
            }
            else {
                return -errno;
            }
        }
    }
    
    return 0;
}

int
__clic_handle(bool syn, char *buf, cli_client_t *clic)
{
    int fd = INVALID_FD, err = 0;

    fd = __clic_fd(clic);
    if (fd<0) {
        return fd;
    }

    int len = os_strlen(buf);
    err = io_send(fd, buf, len);
    if (err<0) { /* yes, <0 */
        goto error;
    }
    debug_cli("send repuest[%d]:%s", len, buf);

    if (false==syn) {
        err = 0; goto error;
    }

    err = __clic_recv(fd, clic->timeout);
    if (err<0) {
        goto error;
    }
    
error:
    os_close(fd);
    
    return err;
}

int
clic_handle(cli_client_t *clic, bool syn, char *action, int argc, char *argv[])
{
    char buf[1+OS_LINE_LEN] = {0};
    int i, len;

    len = os_saprintf(buf, "%s", action);
    for (i=0; i<argc; i++) {
        len += os_snprintf(buf + len, OS_LINE_LEN - len, " %s", argv[i]);
    }

    return __clic_handle(syn, buf, clic);
}

int
__clis_fd(sockaddr_un_t *server)
{
    int fd = INVALID_FD, err = 0;
    
    fd = socket(AF_UNIX, CLI_SOCK_TYPE, 0);
    if (fd<0) {
    	debug_error("socket error:%d", -errno);
        return -errno;
    }
    os_closexec(fd);
    
    err = bind(fd, (sockaddr_t *)server, get_abstract_sockaddr_len(server));
    if (err<0) {
        debug_error("bind(%s) error:%d", get_abstract_path(server), -errno);
        
        return -errno;
    }

    err = listen(fd, 0);
    if (err<0) {
        debug_error("listen(%s) error:%d", get_abstract_path(server), -errno);
        
        return -errno;
    }

    return fd;
}

int
cli_line_handle(
    cli_table_t tables[],
    int count,
    char *tag,
    char *args,
    int (*reply)(int err),
    int (*reply_end)(int err)
)
{
    int i, len, err;

    for (i=0; i<count; i++) {
        cli_table_t *table = &tables[i];

        if (os_strneq(table->tag, tag)) {
            continue;
        }

        err = (*table->u.line_cb)(args);
        
        if (table->syn && reply) {
            len = (*reply)(err);
            debug_cli("send len:%d", len);

            if (len > sizeof(cli_header_t) && reply_end) {
                len = (*reply_end)(err);
            }
        }

        return err;
    }
    
    return -ENOEXIST;
}

int
__clis_handle(int fd, cli_table_t *table, int count)
{
    cli_t *cli = __this_cli();
    char buf[1+OS_LINE_LEN] = {0};
    int err;

    __this_cli()->fd = fd;
    __clib_clear();
    
    err = __io_recv(fd, buf, sizeof(buf), 0);
        debug_cli("recv request[%d]:%s", err, buf);
    if (err<0) { /* yes, <0 */
        return err;
    }
    else if (0==err) {
        return 0;
    }
    buf[err] = 0;

    char *method = buf;
    char *args   = buf;

    os_str_strim_both(method, NULL);
    os_str_reduce(method, NULL);
    cli_shift(args);

    err = cli_line_handle(table, count, method, args, __cli_reply, CLI_REPLY_END);
    
    debug_cli("action:%s %s, error:%d, len:%d, buf:%s", 
        method, args?args:__empty,
        __clib_err,
        __clib_len,
        __clib_buf);

    return err;
}
/******************************************************************************/
