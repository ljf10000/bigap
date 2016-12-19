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
__clib_show(int (*show)(char *buf, int len))
{
    if (__clib_len && is_good_str(__clib_buf)) {
        __clib_cut(__clib_len);

        if (show) {
            (*show)(__clib_buf, __clib_len);
        }
        else if (0==__clib_err) {
            os_printf("%s", __clib_buf);
        }
        else {
            os_eprintf("%s", __clib_buf);
        }
    }

    int err = __clib_err;
    
    os_objzero(__clib());
    
    return err;
}

STATIC int
__cli_reply(int err)
{
    cli_t *cli = __this_cli();
    int len, fd = __this_cli_fd;
    
    __clib_err = err;
    
    if (__this_cli_tcp) {
        len = io_send(fd, __clib(), __clib_space);
    } else {
        len = io_sendto(fd, __clib(), __clib_space, 
                    (struct sockaddr *)__this_cli_addr, __this_cli_addrlen);
    }

    if (__is_ak_debug_cli) {
        os_println("send %s reply[fd=%d pkt=%d/%d len=%d, err=%d]:\n%s",
            __this_cli_type_string(__this_cli_tcp),
            fd, __clib_space, len,
            __clib_len, 
            __clib_err, 
            __clib_buf);
        
        os_dump_buffer(__clib(), __clib_space);
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
        
        if (__is_ak_debug_cli) {
            os_println("recv tcp reply-header[fd=%d pkt=%d/%d len=%d, err=%d]", 
                fd, len, err,
                __clib_len,
                __clib_err);
            
            os_dump_buffer(__clib(), err);
        }
        
        if (err==len) {
            // is last
            if (0==__clib_len) {
                return __clib_err;
            }

            len = __clib_len;
            err = __io_recv(fd, __clib_buf, len, 0);
            if (__is_ak_debug_cli) {
                os_println("recv tcp reply[fd=%d pkt=%d/%d len=%d, err=%d]\n%s", 
                    fd, len, err,
                    __clib_len, 
                    __clib_err,
                    __clib_buf);
                
                os_dump_buffer(__clib_buf, err);
            }
            
            if (err==len) {
                err = __clib_show(clic->show);
            } else {
                goto error;
            }
        } 
        else {
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
    if (__is_ak_debug_cli) {
        os_println("recv udp reply[fd=%d pkt=%d/%d len=%d, err=%d]\n%s", 
            fd, CLI_BUFFER_LEN, err, 
            __clib_len, 
            __clib_err, 
            __clib_buf);
        
        os_dump_buffer(__clib(), err);
    }
    // err > hdr
    if (err >= (int)sizeof(cli_header_t)) {
        __clib_cut(err - sizeof(cli_header_t));

        return __clib_show(clic->show);
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
__cli_request(cli_client_t *clic, cli_table_t *table, int argc, char *argv[])
{
    char buf[1+OS_LINE_LEN];
    bool tcp = os_hasflag(table->flag, CLI_F_TCP);
    bool syn = os_hasflag(table->flag, CLI_F_SYN);
    int fd = INVALID_FD, err = 0;
    int len = argv_zip2bin(buf, sizeof(buf), argc, argv);

    fd = __clic_fd(clic, table);
    if (fd<0) {
        return fd;
    }

    if (__is_ak_debug_cli) {
        os_println("table %s[flag=0x%x timeout=%d]", 
            table->tag, 
            table->flag, 
            table->timeout);
    }

    err = io_send(fd, buf, len);
    if (err<0) { /* yes, <0 */
        goto error;
    }

    if (__is_ak_debug_cli) {
        os_println("send %s request[fd=%d pkt=%d/%d]", 
            __this_cli_type_string(tcp), 
            fd, len, err);
        
        os_dump_buffer(buf, len);
    }
    
    if (false==syn) {
        err = 0; goto error;
    }

    if (tcp) {
        err = __clic_recv_tcp(clic, fd);
    } else {
        err = __clic_recv_udp(clic, fd);
    }
    if (err<0) {
        goto error;
    }
    
error:
    os_close(fd);
    
    return err;
}

int
cli_request(cli_client_t *clic, int argc, char *argv[])
{
    cli_table_t table = __CLI_ENTRY("@default", CLI_F_SYN | CLI_F_TCP, CLI_TIMEOUT, NULL);

    return __cli_request(clic, &table, argc, argv);
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

STATIC int
__clis_table_handle(cli_table_t *table, int argc, char *argv[])
{
    int err, len;
    char *tag = argv[0];

    if (__is_ak_debug_cli) {
        os_println("table %s[flag=0x%x timeout=%d]", 
            table->tag, 
            table->flag, 
            table->timeout);
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

STATIC int
__clis_argv_handle(cli_table_t tables[], int count, int argc, char *argv[])
{
    int i, err, len;
    char *tag = argv[0];

    for (i=0; i<count; i++) {
        cli_table_t *table = &tables[i];

        if (false==os_streq(table->tag, tag)) {
            continue;
        }

        return __clis_table_handle(table, argc, argv);
    }
    
    return -ENOEXIST;
}

int
__cli_response(int fd, cli_table_t tables[], int count)
{
    cli_t *cli = __this_cli();
    char buf[1+OS_LINE_LEN] = {0};
    char *argv[CLI_ARGC] = {NULL};
    int err, argc;

    __clib_clear();

    /*
    * save this fd/tcp, for __cli_reply
    */
    __this_cli_tcp = os_hasflag(tables[0].flag, CLI_F_TCP);
    __this_cli_fd = fd;
    
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
        os_println("recv %s request[fd=%d pkt=%d/%d]", 
            __this_cli_type_string(__this_cli_tcp), 
            fd, (int)sizeof(buf), err);
        
        os_dump_buffer(buf, err);
    }

    argc = argv_unzipbin(buf, os_count_of(argv), argv);
    if (argc<0) {
        __cli_reply(argc);

        return argc;
    }
    else if (0==argc) {
        /*
        * tables[0] is help
        */
        return __clis_table_handle(&tables[0], argc, argv);
    }
    
    if (__is_ak_debug_cli) {
        os_println("recv %s request[fd=%d argc=%d]", 
            __this_cli_type_string(__this_cli_tcp), 
            fd, argc);
        
        __argv_dump(os_println, argc, argv);
    }
    
    err = __clis_argv_handle(tables, count, argc, argv);
    debug_cli("action:%s, error:%d, len:%d, buf:%s", 
        tables->tag,
        __clib_err,
        __clib_len,
        __clib_buf);
    
    return err;
}
/******************************************************************************/
