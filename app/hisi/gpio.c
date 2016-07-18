/*******************************************************************************
Copyright (c) 2016-2018, Supper Wali Technology. All rights reserved.
*******************************************************************************/
#if IS_PRODUCT_LTEFI_MD

static int
gpio_help(int argc, char *argv[])
{
#define USAGE_MODULE    __THIS_APPNAME " clock"
    os_eprintln(USAGE_MODULE "show");
    os_eprintln(USAGE_MODULE "save auto");
    os_eprintln(USAGE_MODULE "save");
    os_eprintln(USAGE_MODULE "sync");
#undef USAGE_MODULE

    return -EFORMAT;
}

static cmd_op_t gpio_op = {
    .help   = gpio_help,
    .showall= gpio_showall,
    .getter = gpio_getter,
    .setter = gpio_setter,
};

static int
gpio_getsetter(int argc, char *argv[])
{
    return cmd_getsetter(argc, argv, &gpio_op);
}

static int
gpio_main(int argc, char *argv[])
{
    return os_call(hisi_gpio_init, hisi_gpio_fini, gpio_getsetter, argc, argv);
}

#endif /* IS_PRODUCT_LTEFI_MD */

