#ifndef __CMD_H_15a8b3427d7b4f05a0ec671c231b2fa3__
#define __CMD_H_15a8b3427d7b4f05a0ec671c231b2fa3__
/******************************************************************************/
#if defined(__APP__) || defined(__BOOT__)
#ifndef __COMMAND_COUNT
#define __COMMAND_COUNT         (2*32)
#endif

enum {
    __COMMAND_F_HIDE = 0x01,
};

/*
* ����ģʽ
*  (1): ����֮��û�й�����ϵ�������������
*       os_do_command ��Ҫѭ����������ƥ�������
*       Ŀǰ��֧��
*
*  (2): ����֮���й�����ϵ������������ϣ���Ҫ�г�ȫ����Ч�������
*       os_do_command ֻ�����״�ƥ�䵽������
*       Ŀǰ֧�ֵ�ģʽ
*/

struct command_item {
    /*
    * list����Ԫ��ʵ�ʸ���
    * �����ʼ����do_command �ڲ���ɨ��list�Զ�����
    */
    int count;
    
    /*
    * �����б��������������ʽ
    *   �������� xxx -a address -v value1 value2, ��
    *       list[0] ��ʼ��Ϊ "-a"��     ����ؼ���
    *       list[1] ��ʼ��Ϊ "address"��"-a"�Ĳ���
    *       list[2] ��ʼ��Ϊ "-v"��     ����ؼ���
    *       list[3] ��ʼ��Ϊ "value1"�� "-v"�ĵ�һ������
    *       list[4] ��ʼ��Ϊ "value2"�� "-v"�ĵڶ�������
    *       listʣ��ΪNULL
    *
    * os_do_command ����� list �Զ����ɰ�����Ϣ��Ҫ��
    *   (1)����������һ������(main's argc>1)������޲������Զ����ɲ���ӡ����
    *   (2)ִ�� "xxx -h" ���Զ����ɲ���ӡ��������������ʾ��������
    *   (3)ִ�� "xxx --" ���Զ����ɲ���ӡ������������������
    */
    char *list[__COMMAND_COUNT+1];
    
    /*
    * ����������İ�����Ϣ, ����Ϊ NULL
    */
    char *help;
    
    /*
    * ����ִ�к���
    *   argc, argv �Ǵ� main ���������, argv[0] �ǳ�����
    */
    int (*func)(int argc, char *argv[]);
    
    /*
    * �����ʶ
    *   __COMMAND_F_HIDE ��λ��ʾ����Ϊ��������
    *       ִ�� "xxx" �� "xxx -h" ʱ����ʾ������Ϣ����ʵ�ʿ�ִ��
    *       ִ�� "xxx --" ����ʾ������Ϣ, "xxx --"������help��Ϣ��
    */
    uint32_t flag;

    /*
    * ������Ч�Կ��ƣ��� command_ctrl is_use_mask/bit ���ʹ��
    *
    * ��ʹ��32λ�������粻���ø�Ϊ64λ
    */
    uint32_t mask;
};

struct command_ctrl {
    struct command_item *cmd;   /* array of (struct command_item) */
    int count;                  /* count of cmd         */
    /*
    * ��� is_use_mask Ϊ�٣��� bit �� command��mask ��������
    * ��� is_use_mask Ϊ�棬�� ��ÿ������ִ��������� ((1<<ctrl's bit) & cmd's mask)
    *   ���Խ��Ϊ�棬�����������Ч
    *   ���򣬴���������Ч(�����ɰ���������ִ��)
    * Ŀǰ��Ҫ���ڹ����ض���Ʒ�������
    */
    bool is_use_mask;
    int bit;
};

#define __COMMAND_ARG_HELPS     "-h"
#define __COMMAND_ARG_HELPL     "-help"
#define __COMMAND_ARG_HIDE      "--"

#define __COMMAND_MASK_ALL      0xffffffff

#define __COMMAND_CTRL_INITER(_cmd, _is_use_mask, _bit) { \
    .cmd        = _cmd,             \
    .count      = os_count_of(_cmd),\
    .is_use_mask= _is_use_mask,     \
    .bit        = _bit,             \
}
#define COMMAND_CTRL_INITER(_cmd) \
        __COMMAND_CTRL_INITER(_cmd, false, 0)

static inline struct command_item *
__command_getbyindex(struct command_ctrl *ctrl, int idx)
{
    return &ctrl->cmd[idx];
}


static inline int 
__command_list_count(struct command_item *cmd)
{
    int i;

    for (i=0; i<__COMMAND_COUNT && cmd->list[i]; i++) {
        ;
    }

    return i;
}

static inline void 
__command_init_one(struct command_ctrl *ctrl, int idx)
{
    struct command_item *cmd = __command_getbyindex(ctrl, idx);
    
    /*
    * step:1
    *
    * get command count
    */
    cmd->count = __command_list_count(cmd);

    /*
    * step:2
    *
    * rewrite command mask
    */
    if (ctrl->is_use_mask && 0 == cmd->mask) {
        cmd->mask = __COMMAND_MASK_ALL;
    }
}


static inline void 
__command_init(struct command_ctrl *ctrl)
{
    int i;
    
    for (i=0; i<ctrl->count; i++) {
        __command_init_one(ctrl, i);
    }
}

static inline bool 
__is_command_used(struct command_ctrl *ctrl, int idx)
{
    struct command_item *cmd = __command_getbyindex(ctrl, idx);
    
    return (false == ctrl->is_use_mask || os_hasbit(cmd->mask, ctrl->bit));
}

static inline bool 
__is_command_hide(struct command_item *cmd)
{
    return os_hasflag(cmd->flag, __COMMAND_F_HIDE);
}


static inline void 
__commond_do_help_one(struct command_item *cmd)
{
    int i;

    /* 
    * __tab
    */
    os_printf(__tab);
    
    for (i=0; i<cmd->count; i++) {
        if (cmd->list[i]) {
            os_printf( "%s ", cmd->list[i]);
        }
    }
    
    /* 
    * help string
    */
    os_printf( __crlf __tab2 "%s" __crlf, cmd->help?cmd->help:"......");
}


static inline void 
__commond_help_one(struct command_ctrl *ctrl, int idx, bool show_hide)
{
    struct command_item *cmd = __command_getbyindex(ctrl, idx);
    
    if (false == __is_command_used(ctrl, idx)) {
        return;
    }
    
    /*
    * is hide command, and not show_hide
    */
    if (__is_command_hide(cmd) && false==show_hide) {
        return;
    }
    
    /*
    * then, make help
    */
    __commond_do_help_one(cmd);
}


static inline void 
__commond_help(struct command_ctrl *ctrl, char *name, bool show_hide)
{
    int i;
    
    /* 
    * help head
    */
    os_println("%s:", name);
    
    for (i=0; i<ctrl->count; i++) {
        __commond_help_one(ctrl, i, show_hide);
    }
}

static inline bool 
__command_match(int argc, char *argv[], struct command_ctrl *ctrl, int idx)
{
    int i;
    struct command_item *cmd = __command_getbyindex(ctrl, idx);

    if ((argc-1)!=cmd->count) {
        return false;
    }
    /*
    * not match current
    */
    else if (false==__is_command_used(ctrl, idx)) {
        return false;
    }
    
    for (i=0; i<cmd->count; i++) {
        if (cmd->list[i]
            /* 
            * begin with '-', need compare 
            */
            && '-' == cmd->list[i][0]
            /* 
            * main's argv != cmd's arg
            */
            && 0 != os_strcmp(argv[i+1], cmd->list[i])) {
            return false;
        }
    }
    
    return (i == cmd->count);
}

/* os_do_command ����

struct command_item commands[] = {
    {
        .list = {"-r", "-a", "address"},    // xxx -r -a address
        .func = reg_read,                   // command function
        .help = "read from the address.",   // command help
    },
    {
        .list = {"-w", "-a", "address", "-v", "value"}, // xxx -w -a address -v value
        .func = reg_write,                      // command function
        .help = "write value to the address.",  // command help
        .flag = __COMMAND_F_HIDE,                 // hide command
    },
};

struct command_ctrl ctrl = __COMMAND_CTRL_INITER(commands, true, 0xffffffff);

os_do_command(argc, argv, &ctrl);

*/
static inline int 
os_do_command(int argc, char *argv[], struct command_ctrl *ctrl)
{
    int i;
    bool show_hide = false;
    
    __command_init(ctrl);

    if (1==argc) {
        goto show_help;
    }
    else if (2 == argc) {
        char *help = argv[1];

        /*
        * "xxx -h"
        * "xxx -help"
        * "xxx --"
        */
        if (0 == os_strcmp(help, __COMMAND_ARG_HELPS)   ||
            0 == os_strcmp(help, __COMMAND_ARG_HELPL)) {
            goto show_help;
        }
        else if (0 == os_strcmp(help, __COMMAND_ARG_HIDE)) {
            show_hide = true;
            goto show_help;
        }
    }

    for (i=0; i<ctrl->count; i++) {
        if (__command_match(argc, argv, ctrl, i)) {
            struct command_item *cmd = __command_getbyindex(ctrl, i);
            
            return (*cmd->func)(argc, argv);
        }
    }
    
show_help:
    __commond_help(ctrl, argv[0], show_hide);
    
    return -EFORMAT;
}
/******************************************************************************/
typedef struct {
    char *tag;

    bool syn; /* just for server, client not use it */
    
    union {
        void *cb;
        int (*line_cb)(char *args);
        int (*argv_cb)(int argc, char *argv[]);
    } u;
} cmd_table_t;

#define __CMD_ENTRY(_tag, _syn, _cb)   { \
    .tag    = _tag,         \
    .syn    = _syn,         \
    .u      = {             \
        .cb = _cb,          \
    },                      \
}

#define CMD_ENTRY(_tag, _cb)    __CMD_ENTRY(_tag, true, _cb)

#define __os_cmd_dump_argv(_dump, _argc, _argv) ({ \
    int i;                                  \
                                            \
    for (i=0; i<_argc; i++) {               \
        _dump("function:%s argv[%d]=%s",    \
            __func__, i, _argv[i]);         \
    }                                       \
                                            \
    _argv[0];                               \
})

#define os_cmd_trace_argv(_argc, _argv)     __os_cmd_dump_argv(debug_trace, _argc, _argv)

#define __cmd_line_next(_args)              os_str_next_byifs(_args, ' ')
#define __os_cmd_shift(_args, _count)   do{ \
    int i, count = (_count);                \
                                            \
    for (i=0; i<count; i++) {               \
        _args = __cmd_line_next(_args);     \
    }                                       \
}while(0)

#define os_cmd_shift(_args)             do{ \
    _args = __cmd_line_next(_args);         \
}while(0)

static int
cmd_line_handle(cmd_table_t map[], int count, char *tag, char *args, int (*after)(void))
{
    int i, err;
    
    for (i=0; i<count; i++) {
        cmd_table_t *table = &map[i];
        
        if (0==os_strcmp(table->tag, tag)) {
            err = (*table->u.line_cb)(args);
            
            if (table->syn && after) {
                int len = (*after)();
                
                debug_trace("send len:%d", len);
            }

            return err;
        }
    }
    
    return -ENOEXIST;
}

static int
cmd_argv_handle(cmd_table_t map[], int count, int argc, char *argv[])
{
    int i;

    if (argc < 1) {
        return -EINVAL0;
    }
    
    for (i=0; i<count; i++) {
        cmd_table_t *table = &map[i];
        
        if (0==os_strcmp(table->tag, argv[0])) {
            return (table->u.argv_cb)(argc-1, argv+1);
        }
    }

    return -ENOEXIST;
}
/******************************************************************************/
#define SIMPILE_CMD_ARGV_COUNT  16

typedef struct {
    int (*handle)(int argc, char *argv[]);
    
    int argc;
    char *argv[SIMPILE_CMD_ARGV_COUNT];
} simpile_cmd_t;

static inline simpile_cmd_t *
simpile_cmd_argv(char *command)
{
    static char line[1+OS_LINE_LEN];
    static simpile_cmd_t cmd;
    char *p;

    os_objzero(&cmd);
    os_arrayzero(line);
    os_strdcpy(line, command);

    for(p = os_strtok(line, " ");
        p;
        p = os_strtok(NULL, " ")) {
        //debug_trace("cmd.argv[%d]=%s", cmd.argc, p);
        
        cmd.argv[cmd.argc++] = p;
    }

    return &cmd;
}

static inline bool
__simpile_cmd_match(simpile_cmd_t *cmd, int argc, char *argv[])
{
    int i;

    if (argc!=cmd->argc) {
        return false;
    }

    for (i=0; i<argc; i++) {
        if (cmd->argv[i] && os_strcmp(argv[i], cmd->argv[i])) {
            return false;
        }
    }

    return true;
}

static inline int
__simpile_cmd_handle(int count, simpile_cmd_t cmd[], int argc, char *argv[], int (*usage)(void))
{
    int i;

    for (i=0; i<count; i++) {
        if (__simpile_cmd_match(&cmd[i], argc, argv)) {
            return (*cmd[i].handle)(argc, argv);
        }
    }

    return (*usage)();
}

#define simpile_cmd_handle(_cmd, _argc, _argv, _usage)  \
    __simpile_cmd_handle(os_count_of(_cmd), _cmd, _argc-1, _argv+1, _usage)
#endif /* defined(__APP__) || defined(__BOOT__) */
/******************************************************************************/
#endif /* __CMD_H_15a8b3427d7b4f05a0ec671c231b2fa3__ */
