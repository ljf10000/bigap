#ifndef __CMD_H_15a8b3427d7b4f05a0ec671c231b2fa3__
#define __CMD_H_15a8b3427d7b4f05a0ec671c231b2fa3__
/******************************************************************************/
#if defined(__APP__) || defined(__BOOT__)
#ifndef CMD_ARGV_COUNT
#define CMD_ARGV_COUNT  16
#endif

#define get_argv(_idx)  ((_idx<argc)?argv[_idx]:NULL)

typedef struct {
    int (*handle)(int argc, char *argv[]);
    
    int argc;
    char *argv[CMD_ARGV_COUNT];
} cmd_table_t;

#define CMD_TABLE_ENTRY(_handle, _argc, _arg, _args...)  { \
    .handle = _handle,              \
    .argc   = _argc,                \
    .argv   = { _arg, ##_args },    \
}   /* end */

STATIC INLINE cmd_table_t *
cmd_argv(char *command);

STATIC INLINE int
__cmd_handle(int count, cmd_table_t cmd[], int argc, char *argv[], int (*usage)(void));

#define cmd_handle(_cmd, _argc, _argv, _usage)  \
    __cmd_handle(os_count_of(_cmd), _cmd, _argc, _argv, _usage)
/******************************************************************************/
typedef struct {
    char *module;
    
    int (*main)(int argc, char *argv[]);
} cmd_multi_table_t;

#define CMD_MULTI_ENTRY(_module, _main)     { .module = _module, .main = _main }

STATIC INLINE int
__cmd_multi_handle(int count, cmd_multi_table_t multi[], int argc, char *argv[]);

#define cmd_multi_handle(_multi, _argc, _argv) \
    __cmd_multi_handle(os_count_of(_multi), _multi, _argc, _argv)
/******************************************************************************/
typedef struct {
    int (*help)(int argc, char *argv[]);
    int (*showall)(void);
    int (*getter)(char *name);
    int (*setter)(char *name, char *value);
} cmd_op_t;

STATIC INLINE int 
cmd_getsetter(int argc, char *argv[], cmd_op_t *op);

#endif /* defined(__APP__) || defined(__BOOT__) */
/******************************************************************************/
#endif /* __CMD_H_15a8b3427d7b4f05a0ec671c231b2fa3__ */
