/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#if IS_PRODUCT_LTEFI_MD
static xcc_t acc = {
    .name       = "acc",
    .timeout    = ACC_TIMEOUT,
    .interval   = ACC_INTERVAL,
    .path       = ACC_PATH,
    .script     = ACC_SCRIPT,

    .map = {
        {
            .name   = GPIO_NAME_MON_ACC,
        },
        {
            .name   = GPIO_NAME_SHUT_DELAY,
            .value  = 0,
        },
    },

    .count = 1 + ACC_GPIO_COUNT,
};

static int
acc_usage(void)
{
    return xcc_usage(&acc);
}

static int
cmd_acc_monitor(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    return xcc_monitor(&acc);
}

static cmd_table_t cmd_acc[] = {
    CMD_TABLE_ENTRY(cmd_acc_monitor, 1, "monitor"),
};

static int
acc_main(int argc, char *argv[])
{
    int err = os_call(hisi_gpio_init, hisi_gpio_fini, cmd_handle, cmd_acc, argc, argv, acc_usage);

    return shell_error(err);
}
#endif /* IS_PRODUCT_LTEFI_MD */

