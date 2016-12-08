#ifndef __CLI_H_277ca663cad74dd5ad59851d69c58e0c__
#define __CLI_H_277ca663cad74dd5ad59851d69c58e0c__
/******************************************************************************/
#ifndef CLI_BUFFER_LEN
#define CLI_BUFFER_LEN      PC_VAL(OS_PAGE_LEN, OS_BIG_LEN)
#endif

#ifndef CLI_TIMEOUT
#define CLI_TIMEOUT         5000 /* ms */
#endif

#ifndef CLI_ARGC
#define CLI_ARGC            PC_VAL(32, 16)
#endif

#ifndef CLI_F_DEAMON
#define CLI_F_DEAMON        DEAMON_VAL(CLI_F_SERVER, 0)
#endif

typedef struct cli_table_s cli_table_t;

typedef int cli_handle_f(cli_table_t *table, int argc, char *argv[]);
typedef int cli_show_f(char *buf, int len);

enum {
    CLI_F_SYN   = 0x01,
    CLI_F_TCP   = 0x02,
    CLI_F_SERVER= 0x04,
};

#if defined(__APP__) || defined(__BOOT__)
struct cli_table_s {
    char *tag;

    uint32 flag;
    uint32 timeout;

    cli_handle_f *cb;
};

#define __CLI_ENTRY(_tag, _flag, _timeout, _cb)   { \
    .tag    = _tag,     \
    .flag   = _flag | CLI_F_DEAMON, \
    .timeout= _timeout, \
    .cb     = _cb,      \
}   /* end */

#define __CLI_TCP_ENTRY(_tag, _timeout, _cb) \
    __CLI_ENTRY(_tag, CLI_F_SYN | CLI_F_TCP, _timeout, _cb)
#define __CLI_UDP_ENTRY(_tag, _timeout, _cb) \
    __CLI_ENTRY(_tag, CLI_F_SYN, _timeout, _cb)

#define CLI_TCP_ENTRY(_tag, _cb) \
    __CLI_TCP_ENTRY(_tag, CLI_TIMEOUT, _cb)
#define CLI_UDP_ENTRY(_tag, _cb) \
    __CLI_UDP_ENTRY(_tag, CLI_TIMEOUT, _cb)

#endif /* defined(__APP__) || defined(__BOOT__) */
/******************************************************************************/
#ifdef __APP__

typedef struct {
    uint32 len;
    uint32 flag;
    uint32 type;
     int32 err;
} cli_header_t;

typedef struct {
    cli_header_t hdr;
    
    char buf[0];
} cli_buffer_t;

typedef struct {
    int fd;     // just for server
    bool tcp;   // just for server
    
    sockaddr_un_t   addr;
    socklen_t       addrlen;

    cli_buffer_t    *b;
} cli_t;

extern cli_t *
__this_cli(void);

#define __this_cli_tcp          __this_cli()->tcp
#define __this_cli_fd           __this_cli()->fd
#define __this_cli_addr         (&__this_cli()->addr)
#define __this_cli_addrlen      __this_cli()->addrlen

#define __this_cli_type_string(_tcp)    (_tcp)?"tcp":"udp"

static inline int 
__this_cli_socket(bool tcp)
{
    int fd = socket(AF_UNIX, tcp?SOCK_STREAM:SOCK_DGRAM, 0);
    if (is_good_fd(fd)) {
        os_closexec(fd);
    }

    return fd;
}

static inline cli_buffer_t *
__clib(void)
{
    return __this_cli()->b;
}

#define __clib_err      __clib()->hdr.err
#define __clib_len      __clib()->hdr.len
#define __clib_buf      __clib()->buf
#define __clib_cursor   (__clib_buf  + __clib_len)
#define __clib_space    ((uint32)sizeof(cli_header_t) + __clib_len)
#define __clib_SIZE     (CLI_BUFFER_LEN - sizeof(cli_header_t))
#define __clib_left     ((__clib_SIZE>__clib_len)?(__clib_SIZE - __clib_len):0)

static inline void
__clib_cut(uint32 len)
{
    __clib_buf[len] = 0;
}

static inline void
__clib_clear(void) 
{
    __clib_err = 0;
    __clib_len = 0;
    
    __clib_cut(0);
}

extern int
cli_vsprintf(const char *fmt, va_list args);

extern int
cli_sprintf(const char *fmt, ...);

#define cli_help(_error, _usage)    ({ \
    cli_sprintf(_usage);    \
                            \
    _error;                 \
})  /* end */

typedef struct {
    int timeout;    // todo: not use it
    
    sockaddr_un_t server, client;

    cli_show_f *show;
} cli_client_t;

#define CLI_CLIENT_INITER(_server_PATH) {   \
    .server     = OS_SOCKADDR_ABSTRACT(_server_PATH),   \
    .client     = OS_SOCKADDR_UNIX(__zero),             \
    .timeout    = CLI_TIMEOUT,                          \
}   /* end */

#define CLI_CLIENT_UNIX     "/tmp/." __THIS_APPNAME ".%d.unix"

extern int
__clic_fd_helper(cli_client_t *clic, cli_table_t *table);

static inline int
__clic_fd(cli_client_t *clic, cli_table_t *table)
{
    abstract_path_sprintf(&clic->client, CLI_CLIENT_UNIX, getpid());
    
    return __clic_fd_helper(clic, table);
}
/******************************************************************************/
extern int
__clis_fd(bool tcp, sockaddr_un_t *server);

#define __clis_FD(_tcp, _name)    ({    \
    sockaddr_un_t server = OS_SOCKADDR_ABSTRACT(_name); \
                                        \
    __clis_fd(_tcp, &server);           \
})  /* end */

extern int
__cli_response(int fd, cli_table_t tables[], int count);

#define cli_response(_fd, _tables) \
    __cli_response(_fd, _tables, os_count_of(_tables))

extern int
cli_request(cli_client_t *clic, int argc, char *argv[]);

#endif /* __APP__ */

#ifdef __BOOT__
#include "weos/boot/cli.c"
#endif
/******************************************************************************/
#endif /* __CLI_H_277ca663cad74dd5ad59851d69c58e0c__ */
