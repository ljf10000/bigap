/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
ALWAYS_INLINE cli_t *
__this_cli_init(cli_t *cli)
{
    if (NULL==cli->b) {
        cli->b = (cli_buffer_t *)os_zalloc(1+CLI_BUFFER_LEN);
        if (NULL==cli->b) {
            return NULL;
        }
        
        __this_cli_addr->sun_family = AF_UNIX;
        __this_cli_addrlen = sizeof(sockaddr_un_t);
        __this_cli_fd = INVALID_FD;
    }

    return cli;
}

cli_t *
__this_cli(void)
{
    static cli_t __THIS_CLI;
    
    return __this_cli_init(&__THIS_CLI);
}

STATIC int
__clib_show(void)
{
    if (0==__clib_err && __clib_len && is_good_str(__clib_buf)) {
        __clib_cut(__clib_len);
        
        os_printf("%s", __clib_buf);

        os_objzero(__clib());
    }

    return __clib_err;
}

STATIC int
__clic_show(cli_client_t *clic)
{
    return clic->show?(*clic->show)():__clib_show();
}

STATIC int
__cli_reply(int err)
{
    cli_t *cli = __this_cli();
    int len;
    
    debug_cli("send reply[len=%d, err=%d]:%s", __clib_len, err, __clib_buf);

    __clib_err = err;
    
    if (__this_cli_tcp) {
        len = io_send(__this_cli_fd, __clib(), __clib_space);
    } else {
        len = io_sendto(__this_cli_fd, __clib(), __clib_space, 
                    (struct sockaddr *)__this_cli_addr, __this_cli_addrlen);
    }

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

    if (__this_cli_tcp && __clib_left < vsize) {
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
__clic_fd_helper(cli_client_t *clic, cli_table_t *table)
{
    bool tcp = os_hasflag(table->flag, CLI_F_TCP);
    int fd = INVALID_FD, err = 0;
    int type;

    fd = __this_cli_socket(tcp);
    if (fd<0) {
        debug_error("socket error:%d", -errno);
        return -errno;
    }
    
    int timeout = env_geti(OS_ENV(TIMEOUT), 0);
    timeout = (timeout>0)?timeout:table->timeout;
    struct timeval tm = OS_TIMEVAL_INITER(os_second(timeout), os_usecond(timeout));
    err = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tm, sizeof(tm));
    if (err<0) {
        debug_error("setsockopt SO_RCVTIMEO error:%d", -errno);
        return -errno;
    }
    
    sockaddr_un_t *client = &clic->client;
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

STATIC int
__clic_recv_tcp(cli_client_t *clic, int fd)
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
                __clic_show(clic);
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

STATIC int
__clic_recv_udp(cli_client_t *clic, int fd)
{
    cli_t *cli = __this_cli();
    int err = 0;

    err = __io_recv(fd, __clib(), CLI_BUFFER_LEN, 0);
    // err > hdr
    if (err >= (int)sizeof(cli_header_t)) {
        __clib_cut(err - sizeof(cli_header_t));
        
        return __clic_show(clic);
    }
    // 0 < err < hdr
    else if (err > 0) {
        return -ETOOSMALL;
    }
    // err < 0
    else {
        return err;
    }
}

STATIC int
__clic_recv(cli_client_t *clic, int fd, bool tcp)
{
    if (tcp) {
        return __clic_recv_tcp(clic, fd);
    } else {
        return __clic_recv_udp(clic, fd);
    }
}

int
__clic_request(cli_client_t *clic, cli_table_t *table, char *buf, int len)
{
    int fd = INVALID_FD, err = 0;
    bool tcp = os_hasflag(table->flag, CLI_F_TCP);
    bool syn = os_hasflag(table->flag, CLI_F_SYN);
    
    fd = __clic_fd(clic, table);
    if (fd<0) {
        return fd;
    }

    err = io_send(fd, buf, len);
    if (err<0) { /* yes, <0 */
        goto error;
    }

    if (__is_ak_debug_cli) {
        __os_dump_buffer(buf, len, NULL);
    }
    
    if (false==syn) {
        err = 0; goto error;
    }

    err = __clic_recv(clic, fd, tcp);
    if (err<0) {
        goto error;
    }
    
error:
    os_close(fd);
    
    return err;
}

int
clic_request(cli_client_t *clic, cli_table_t *table, int argc, char *argv[])
{
    char buf[1+OS_LINE_LEN];
    
    int len = argv_zip2bin(buf, sizeof(buf), argc, argv);
    
    return __clic_request(clic, table, buf, len);
}

int
__clis_fd(bool tcp, sockaddr_un_t *server)
{
    int fd = INVALID_FD, err = 0;

    fd = __this_cli_socket(tcp);
    if (fd<0) {
    	debug_error("socket error:%d", -errno);
        return -errno;
    }
    
    err = bind(fd, (sockaddr_t *)server, get_abstract_sockaddr_len(server));
    if (err<0) {
        debug_error("bind(%s) error:%d", get_abstract_path(server), -errno);
        
        return -errno;
    }

    if (tcp) {
        err = listen(fd, 0);
        if (err<0) {
            debug_error("listen(%s) error:%d", get_abstract_path(server), -errno);
            
            return -errno;
        }
    }
    
    return fd;
}

int
__cli_argv_handle(cli_table_t tables[], int count, int argc, char *argv[])
{
    int i, err, len;
    char *tag = argv[0];

    for (i=0; i<count; i++) {
        cli_table_t *table = &tables[i];

        if (os_strneq(table->tag, tag)) {
            continue;
        }

        bool tcp    = os_hasflag(table->flag, CLI_F_TCP);
        bool syn    = os_hasflag(table->flag, CLI_F_SYN);
        bool server = os_hasflag(table->flag, CLI_F_SERVER);

        if (server) {
            /*
            * this command use tcp
            */
            __this_cli_tcp = tcp;
        }
        
        err = (*table->cb)(table, argc, argv);
        
        if (server && syn) {
            len = __cli_reply(err);
            if (tcp && len > sizeof(cli_header_t)) {
                len = __cli_reply(err);
            }
        }
        
        return err;
    }
    
    return -ENOEXIST;
}

int
__clis_handle(int fd, cli_table_t tables[], int count)
{
    cli_t *cli = __this_cli();
    char buf[1+OS_LINE_LEN] = {0};
    char *argv[CLI_ARGC] = {NULL};
    int err, argc;

    __clib_clear();

    if (__this_cli_tcp) {
        err = __io_recv(fd, buf, sizeof(buf), 0);
    } else {
        err = __io_recvfrom(fd, buf, sizeof(buf), 0, (sockaddr_t *)__this_cli_addr, &__this_cli_addrlen);
        if (is_abstract_sockaddr(__this_cli_addr)) {
            set_abstract_sockaddr_len(__this_cli_addr, __this_cli_addrlen);
        }
    }
    if (err<0) { /* yes, <0 */
        return err;
    }
    else if (0==err) {
        return 0;
    }
    buf[err] = 0;

    if (__is_ak_debug_cli) {
        __os_dump_buffer(buf, err, NULL);
    }

    argc = argv_unzipbin(buf, os_count_of(argv), argv);
    if (argc<0) {
        __cli_reply(argc);

        return argc;
    }
    else if (0==argc) {
        cli_table_t *table = &tables[0];

        err = (tables[0].cb)(table, argc, argv);

        __cli_reply(err);

        return err;
    }
    
    if (__is_ak_debug_cli) {
        __argv_dump(os_println, argc, argv);
    }

    /*
    * save this fd, for __cli_reply
    */
    __this_cli_fd = fd;
    
    err = __cli_argv_handle(tables, count, argc, argv);
    debug_cli("action:%s, error:%d, len:%d, buf:%s", 
        tables->tag,
        __clib_err,
        __clib_len,
        __clib_buf);
    
    return err;
}
/******************************************************************************/
