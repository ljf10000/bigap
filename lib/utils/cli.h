#ifndef __CLI_H_277ca663cad74dd5ad59851d69c58e0c__
#define __CLI_H_277ca663cad74dd5ad59851d69c58e0c__
/******************************************************************************/
#define CLI_SOCK_TYPE       SOCK_STREAM
#define CLI_REPLY_END       __cli_reply
#define CLI_BUFFER_LEN      PC_VAL(OS_PAGE_LEN, OS_BIG_LEN)

#ifndef CLI_TIMEOUT
#define CLI_TIMEOUT         5000 /* ms */
#endif

#if defined(__APP__) || defined(__BOOT__)
typedef struct {
    char *tag;

    bool syn; /* just for server, client not use it */
    
    union {
        void *cb;
        int (*line_cb)(char *args);
        int (*argv_cb)(int argc, char *argv[]);
    } u;
} cli_table_t;

#define __CLI_ENTRY(_tag, _syn, _cb)   { \
    .tag    = _tag,         \
    .syn    = _syn,         \
    .u      = {             \
        .cb = _cb,          \
    },                      \
}

#define CLI_ENTRY(_tag, _cb)    __CLI_ENTRY(_tag, true, _cb)

#define __cli_argv_dump(_dump, _argc, _argv) ({ \
    int i;                                  \
                                            \
    for (i=0; i<_argc; i++) {               \
        _dump("function:%s argv[%d]=%s",    \
            __func__, i, _argv[i]);         \
    }                                       \
                                            \
    _argv[0];                               \
})

#define cli_argv_dump(_argc, _argv)         __cli_argv_dump(debug_trace, _argc, _argv)

#define __cli_line_next(_args)              os_str_next_byifs(_args, __space)
#define __cli_shift(_args, _count)      do{ \
    int i, count = (_count);                \
                                            \
    for (i=0; i<count; i++) {               \
        _args = __cli_line_next(_args);     \
    }                                       \
}while(0)

#define cli_shift(_args)                do{ \
    _args = __cli_line_next(_args);         \
}while(0)

EXTERN int
cli_argv_handle(cli_table_t tables[], int count, int argc, char *argv[]);

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
    int fd;

    sockaddr_un_t   addr;
    socklen_t       addrlen;

    cli_buffer_t    *b;
} cli_t;

#define DECLARE_FAKE_CLI        extern cli_t __THIS_CLI
#define DECLARE_REAL_CLI        cli_t __THIS_CLI;

#ifdef __ALLINONE__
#   define DECLARE_CLI          DECLARE_FAKE_CLI
#else
#   define DECLARE_CLI          DECLARE_REAL_CLI
#endif

DECLARE_FAKE_CLI;

extern cli_t *
__this_cli(void);

static inline cli_buffer_t *
__clib(void)
{
    return __this_cli()->b;
}

#define __clib_err      __clib()->hdr.err
#define __clib_len      __clib()->hdr.len
#define __clib_buf      __clib()->buf
#define __clib_cursor   (__clib_buf  + __clib_len)
#define __clib_space    (sizeof(cli_header_t) + __clib_len)
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
    __clib_len = 0;
    
    __clib_cut(0);
}

extern int
__clib_show(void);

extern int
__cli_reply(int err);

extern int
cli_vsprintf(const char *fmt, va_list args);

extern int
cli_sprintf(const char *fmt, ...);

#if 1
typedef struct {
    int timeout;
    
    sockaddr_un_t server, client;
} cli_client_t;

#define CLI_CLIENT_INITER(_server_PATH)             {   \
    .server     = OS_SOCKADDR_ABSTRACT(_server_PATH),   \
    .client     = OS_SOCKADDR_UNIX(__zero),             \
    .timeout    = CLI_TIMEOUT,                          \
}   /* end */

#define CLI_CLIENT_UNIX     "/tmp/." __THIS_APPNAME ".%d.unix"

extern int
__clic_fd(cli_client_t *clic);

extern int
__clic_recv(int fd, int timeout);

extern int
__clic_handle(bool syn, char *buf, cli_client_t *clic);

extern int
clic_handle(cli_client_t *clic, bool syn, char *action, int argc, char *argv[]);

#define clic_sync_handle(_client, _action, _argc, _argv) \
    clic_handle(_client, true, _action, _argc, _argv)

#define clic_async_handle(_client, _action, _argc, _argv) \
    clic_handle(_client, false, _action, _argc, _argv)
#endif
/******************************************************************************/
#if 1
extern int
__clis_fd(sockaddr_un_t *server);

#define __clis_FD(_name)    ({  \
    sockaddr_un_t server = OS_SOCKADDR_ABSTRACT(_name); \
                                \
    __clis_fd(&server);         \
})  /* end */

extern int
cli_line_handle(
    cli_table_t tables[],
    int count,
    char *tag,
    char *args,
    int (*reply)(int err),
    int (*reply_end)(int err)
);

extern int
__clis_handle(int fd, cli_table_t *table, int count);

#define clis_handle(_fd, _table) \
    __clis_handle(_fd, _table, os_count_of(_table))
#endif
#endif /* __APP__ */

#ifdef __BOOT__
#include "weos/boot/cli.c"
#endif
/******************************************************************************/
#endif /* __CLI_H_277ca663cad74dd5ad59851d69c58e0c__ */
