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

    for (i=0; i<count; i++) {
        if (__cmd_match(&cmd[i], argc, argv)) {
            return (*cmd[i].handle)(argc, argv);
        }
    }

    return (*usage)();
}

#define cmd_handle(_cmd, _argc, _argv, _usage)  \
    __cmd_handle(os_count_of(_cmd), _cmd, _argc-1, _argv+1, _usage)
#endif /* defined(__APP__) || defined(__BOOT__) */
/******************************************************************************/
#endif /* __CMD_H_15a8b3427d7b4f05a0ec671c231b2fa3__ */
