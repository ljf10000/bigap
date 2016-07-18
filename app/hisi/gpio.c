/*******************************************************************************
Copyright (c) 2016-2018, Supper Wali Technology. All rights reserved.
*******************************************************************************/
#if IS_PRODUCT_LTEFI_MD

static int
clock_usage(void)
{
#define USAGE_MODULE    __THIS_APPNAME " clock"
    os_eprintln(USAGE_MODULE "show");
    os_eprintln(USAGE_MODULE "save auto");
    os_eprintln(USAGE_MODULE "save");
    os_eprintln(USAGE_MODULE "sync");
#undef USAGE_MODULE

    return -EFORMAT;
}

static cmd_table_t cmd_gpio[] = {
    {
        .argc   = 1,
        .argv   = {"show"},
        .handle = cmd_clock_show,
    },
    {
        .argc   = 2,
        .argv   = {"save", "auto"},
        .handle = cmd_clock_save_auto,
    },
    {
        .argc   = 1,
        .argv   = {"save"},
        .handle = cmd_clock_save_manual,
    },
    {
        .argc   = 1,
        .argv   = {"sync"},
        .handle = cmd_clock_sync,
    },
};
#endif /* IS_PRODUCT_LTEFI_MD */

