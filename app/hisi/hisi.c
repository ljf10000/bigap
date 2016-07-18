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
#include "gpio.c"
/******************************************************************************/
static cmd_multi_table_t multi[] = {
#if IS_PRODUCT_LTEFI_MD
    CMD_MULTI_INITER("dog",     dog_main),
    CMD_MULTI_INITER("gpio",    gpio_main),
    CMD_MULTI_INITER("clock",   clock_main),
#endif

#if (IS_PRODUCT_LTEFI_MD || IS_PRODUCT_PC)
    CMD_MULTI_INITER("otp",     otp_main),
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
    
    return os_call(hisi_init, hisi_fini, __main, argc, argv);
}
/******************************************************************************/
