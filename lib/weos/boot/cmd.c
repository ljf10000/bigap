/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#if defined(__APP__) || defined(__BOOT__)
STATIC INLINE cmd_table_t *
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

STATIC INLINE bool
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

STATIC INLINE int
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

STATIC INLINE int
__cmd_multi_handle(int count, cmd_multi_table_t multi[], int argc, char *argv[])
{
    int i;

    char *app       = argv[0];
    char *module    = argv[1];

    if (argc<2) {
        goto help;
    }

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

STATIC INLINE int 
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
