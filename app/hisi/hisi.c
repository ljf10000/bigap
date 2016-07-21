/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
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
#include "dial.c"
#include "vcc.c"
#include "acc.c"
#include "led.c"
#include "voltage.c"
/******************************************************************************/
static cmd_multi_table_t multi[] = {
#if IS_PRODUCT_LTEFI_MD
    CMD_MULTI_ENTRY("dog",      dog_main),
    CMD_MULTI_ENTRY("gpio",     gpio_main),
    CMD_MULTI_ENTRY("clock",    clock_main),
    CMD_MULTI_ENTRY("dial",     dial_main),
    CMD_MULTI_ENTRY("vcc",      vcc_main),
    CMD_MULTI_ENTRY("acc",      acc_main),
    CMD_MULTI_ENTRY("voltage",  voltage_main),
#endif

#if (IS_PRODUCT_LTEFI_MD || IS_PRODUCT_PC)
    CMD_MULTI_ENTRY("otp",      otp_main),
#endif
};

#ifndef __BUSYBOX__
#define hisi_main  main
#endif

/*
* dog have enabled when boot
*/
int hisi_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_call(hisi_init, hisi_fini, cmd_multi_handle, multi, argc, argv);
}
/******************************************************************************/
