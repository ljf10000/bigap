/*******************************************************************************
Copyright (c) 2016-2018, Supper Wali Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      hisi
#endif

#include "hisi/hisi.h"

OS_INITER;

#include "clock.c"
#include "otp.c"
#include "dog.c"
/******************************************************************************/
static cmd_multi_table_t multi[] = {
#if IS_PRODUCT_LTEFI_MD
    {
        .module = "dog",
        .table  = cmd_dog,
        .count  = os_count_of(cmd_dog),
        .usage  = dog_usage,
        .init   = hisi_dog_init,
        .fini   = hisi_dog_fini,
    },
    {
        .module = "clock",
        .table  = cmd_clock,
        .count  = os_count_of(cmd_clock),
        .usage  = clock_usage,
        .init   = hisi_i2c_init,
        .fini   = hisi_i2c_fini,
    },
#endif

#if (IS_PRODUCT_LTEFI_MD || IS_PRODUCT_PC)
    {
        .module = "otp",
        .table  = cmd_otp,
        .count  = os_count_of(cmd_otp),
        .usage  = otp_usage,
        .init   = os_call_nothing,
        .fini   = os_call_nothing,
    },

#endif
};

#ifndef __BUSYBOX__
#define hisi_main  main
#endif

static int
__main(int argc, char *argv[])
{
    return cmd_multi_handle(multi, argc, argv);
}

/*
* dog have enabled when boot
*/
int hisi_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(__main, argc, argv);
}
/******************************************************************************/
