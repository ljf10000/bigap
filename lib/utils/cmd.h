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

static inline cmd_table_t *
cmd_argv(char *command)
{
    static char line[1+OS_LINE_LEN];
    static cmd_table_t cmd;
    char *p;

    os_objzero(&cmd);
    os_arrayzero(line);
    os_strdcpy(line, command);

    for(p = os_strtok(line, __space);
        p;
        p = os_strtok(NULL, __space)) {
        //debug_trace("cmd.argv[%d]=%s", cmd.argc, p);
        
        cmd.argv[cmd.argc++] = p;
    }

    return &cmd;
}

static inline bool
__cmd_match(cmd_table_t *cmd, int argc, char *argv[])
{
    int i;

    if (argc!=cmd->argc) {
        return false;
    }

    for (i=0; i<argc; i++) {
        if (cmd->argv[i] && 0!=os_strcmp(argv[i], cmd->argv[i])) {
            return false;
        }
    }

    return true;
}

static inline int
__cmd_handle(int count, cmd_table_t cmd[], int argc, char *argv[], int (*usage)(void))
{
    int i;

    argc -= 1; argv += 1;
    
    for (i=0; i<count; i++) {
        if (__cmd_match(&cmd[i], argc, argv)) {
            return (*cmd[i].handle)(argc, argv);
        }
    }

    return (*usage)();
}

#define cmd_handle(_cmd, _argc, _argv, _usage)  \
    __cmd_handle(os_count_of(_cmd), _cmd, _argc, _argv, _usage)
/******************************************************************************/
typedef struct {
    char *module;
    
    int (*main)(int argc, char *argv[]);
} cmd_multi_table_t;

#define CMD_MULTI_ENTRY(_module, _main)     { .module = _module, .main = _main }

static inline int
__cmd_multi_handle(int count, cmd_multi_table_t multi[], int argc, char *argv[])
{
    int i;

    __cli_argv_dump(os_println, argc, argv);
    
    if (argc<2) {
        goto help;
    }

    char *app       = argv[0];
    char *module    = argv[1];

    argc -= 1; argv += 1;

    for (i=0; i<count; i++) {
        if (os_streq(module, multi[i].module)) {
            return (*multi[i].main)(argc, argv);
        }
    }

help:
    for (i=0; i<count; i++) {
        os_println("%s %s [COMMAND]", app, multi[i].module);
    }

    return -EFORMAT;
}

#define cmd_multi_handle(_multi, _argc, _argv) \
    __cmd_multi_handle(os_count_of(_multi), _multi, _argc, _argv)
/******************************************************************************/
typedef struct {
    int (*help)(int argc, char *argv[]);
    int (*showall)(void);
    int (*getter)(char *name);
    int (*setter)(char *name, char *value);
} cmd_op_t;

static inline int 
cmd_getsetter(int argc, char *argv[], cmd_op_t *op)
{
    int err = 0;
    int len;
    int var;
    int i;
    char *name;
    char *value;
    
    /*
    * display all
    */
    if (1==argc) {
        return (*op->showall)();
    }
    else if (2==argc) {
        char *name = argv[1];
        /*
        * help
        */
        if (0==strcmp("-h", name) || 0==strcmp("--help", name)) {
            return (*op->help)(argc, argv);
        }
        /*
        * get by name
        */
        else if (NULL==strchr(name, '=')) {
            return (*op->getter)(name);
        }
    }
    
    /*
    * set by name
    */
    for (i=1; i<argc; i++) {
        char line[1+OS_LINE_LEN] = {0};
        
        len = strlen(argv[i]);

        /*
        * check input length
        */
        if (len > OS_LINE_LEN) {
            os_println("argv[%d](%s) too long", i, argv[i]);

            return -1;
        }
        
        strcpy(line, argv[i]);

        /*
        * get name
        */
        name = line;
        value = strchr(line, '=');
        if (NULL==value) {
            os_println("argv[%d](%s) should as xxx=xxxx", i, argv[i]);

            return -1;
        }
        *value = 0; value++;

        err = (*op->setter)(name, value);
        if (err<0) {
            return err;
        }
    }
    
    return 0;
}

#endif /* defined(__APP__) || defined(__BOOT__) */
/******************************************************************************/
#endif /* __CMD_H_15a8b3427d7b4f05a0ec671c231b2fa3__ */
