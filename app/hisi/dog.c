/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#if IS_PRODUCT_LTEFI_MD
static int
cmd_dog_enable(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    return hisi_dog_foreach(hisi_dog_enable);
}

static int
cmd_dog_disable(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    hisi_dog_foreach(hisi_dog_clear);
    
    return hisi_dog_foreach(hisi_dog_disable);
}

static int
cmd_dog_clear(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    return hisi_dog_foreach(hisi_dog_clear);
}

static int
cmd_dog_reset(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    return hisi_dog_foreach(hisi_dog_reset);
}

static int
cmd_dog_get_timeout(int argc, char *argv[])
{
    int i, err, timeout[PRODUCT_DOG_COUNT];

    (void)argc;
    (void)argv;

    for (i=0; i<PRODUCT_DOG_COUNT; i++) {
        err = hisi_dog_get_timeout(i, &timeout[i]);
        if (err<0) {
            return err;
        }
    }
    
    for (i=1; i<PRODUCT_DOG_COUNT; i++) {
        if (timeout[i-1] != timeout[i]) {
            debug_error("dog%d's timeout(%d) != dog%d's timeout(%d)",
                i-1,
                timeout[i-1],
                i,
                timeout[i]);
            
            return INVALID_VALUE;
        }
    }

    os_println("%d", timeout[0]);
    
    return 0;
}

static int
cmd_dog_set_timeout(int argc, char *argv[])
{
    int timeout;
    char *end = NULL;
    char *string = argv[1];
    
    timeout = os_strtol(string, &end, 0);
    if (false==os_strton_is_good_end(end) || timeout <= 0) {
        os_println("bad timeout:%s", string);
        
        return -EFORMAT;
    } else {
        return hisi_dog_foreach(hisi_dog_set_timeout, timeout);
    }
}

static int
dog_usage(void)
{
#define USAGE_MODULE    __THIS_APPNAME " dog "
    os_eprintln(USAGE_MODULE "enable");
    os_eprintln(USAGE_MODULE "disable");
    os_eprintln(USAGE_MODULE "clear");
    os_eprintln(USAGE_MODULE "reset");
    os_eprintln(USAGE_MODULE "timeout [timeout(ms)]");
#undef USAGE_MODULE

    return -EFORMAT;
}

static cmd_table_t cmd_dog[] = {
    CMD_TABLE_ENTRY(cmd_dog_enable, 1, "enable"),
    CMD_TABLE_ENTRY(cmd_dog_disable, 1, "disable"),
    CMD_TABLE_ENTRY(cmd_dog_clear, 1, "clear"),
    CMD_TABLE_ENTRY(cmd_dog_reset, 1, "reset"),
    CMD_TABLE_ENTRY(cmd_dog_get_timeout, 1, "timeout"),
    CMD_TABLE_ENTRY(cmd_dog_set_timeout, 2, "timeout", NULL),
    CMD_TABLE_ENTRY(cmd_dog_disable, 1, "disable"),
};

static int
dog_main(int argc, char *argv[])
{
    return os_call(hisi_dog_init, hisi_dog_fini, cmd_handle, cmd_dog, argc, argv, dog_usage);
}

#endif /* IS_PRODUCT_LTEFI_MD */

