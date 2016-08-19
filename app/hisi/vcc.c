/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#if IS_PRODUCT_LTEFI_MD

static xcc_t vcc = {
    .name       = "vcc",
    .timeout    = VCC_TIMEOUT,
    .interval   = VCC_INTERVAL,
    .path       = VCC_PATH,
    .script     = VCC_SCRIPT,

    .map = {
        {
            .name   = GPIO_NAME_MON_VCC,
        },
        {
            .name   = GPIO_NAME_SHUT_FORCE,
            .value  = 1,
        },
        {
            .name   = GPIO_NAME_SHUT_DELAY,
            .value  = 0,
        },
    },

    .count = 1 + VCC_GPIO_COUNT,
};

static int
vcc_usage(void)
{
    return xcc_usage(&vcc);
}

static int
cmd_vcc_monitor(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    return xcc_monitor(&vcc);
}

static cmd_table_t cmd_vcc[] = {
    CMD_TABLE_ENTRY(cmd_vcc_monitor, 1, "monitor"),
};

static int
vcc_main(int argc, char *argv[])
{
    int err = os_call(hisi_gpio_init, hisi_gpio_fini, cmd_handle, cmd_vcc, argc, argv, vcc_usage);

    return shell_error(err);
}

#endif /* IS_PRODUCT_LTEFI_MD */

