/*******************************************************************************
Copyright (c) 2015-2016, xxx Networks. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      otp
#endif

#include "utils.h"
#include "otp/otp.h"
#include "benv/bcookie.h"

#if !(IS_PRODUCT_LTEFI_MD || IS_PRODUCT_PC)
#error "just support ltefi md/pc board"
#endif

OS_INITER;

static int
usage(void)
{
    os_eprintln(__THIS_NAME " custom  show");
    os_eprintln(__THIS_NAME " private show");

    return -EFORMAT;
}

static int
cmd_custom_show(int argc, char *argv[])
{
    return otp_custom_show();
}

static int
cmd_custom_write(int argc, char *argv[])
{
    return otp_custom_write((byte *)argv[2]);
}

static int
cmd_private_show(int argc, char *argv[])
{
    return otp_private_show();
}

static int
cmd_private_write(int argc, char *argv[])
{
    byte otp[OTP_SIZE];
    int count;
    
    count = os_hexstring2buf(argv[2], otp, OTP_SIZE);
    if (OTP_SIZE!=count) {
        return -EFORMAT;
    }
    
    return otp_private_write(otp);
}

#if 2==PRODUCT_VERSION || 0==PRODUCT_VERSION
static int
cmd_bcookie_show(int argc, char *argv[])
{
    int err;
    struct bcookie_otp botp = BCOOKIE_OBJ(BCOOKIE_OTP);
    
    err = bcookie_load((struct bcookie *)&botp, sizeof(botp));
    if (err<0) {
        return err;
    }

    os_println("%s", __otp_string(botp.custom));
    os_println("%s", __otp_string(botp.private));

    return 0;
}

static int
cmd_bcookie_write(int argc, char *argv[])
{
    int count;
    struct bcookie_otp botp = BCOOKIE_OBJ(BCOOKIE_OTP);
    
    count = os_hexstring2buf(argv[2], botp.custom, OTP_SIZE);
    if (count<0) {
        return count;
    }
    
    count = os_hexstring2buf(argv[3], botp.private, OTP_SIZE);
    if (count<0) {
        return count;
    }
    
    return bcookie_save((struct bcookie *)&botp, sizeof(botp));
}
#endif

static cmd_table_t cmd[] = {
    {
        .argc   = 2,
        .argv   = {"custom", "show"},
        .handle = cmd_custom_show,
    },
    {
        .argc   = 3,
        .argv   = {"custom", "write", NULL},
        .handle = cmd_custom_write,
    },
    {
        .argc   = 2,
        .argv   = {"private", "show"},
        .handle = cmd_private_show,
    },
    {
        .argc   = 3,
        .argv   = {"private", "write", NULL},
        .handle = cmd_private_write,
    },
#if 2==PRODUCT_VERSION || 0==PRODUCT_VERSION
    {
        .argc   = 2,
        .argv   = {"cookie", "show"},
        .handle = cmd_bcookie_show,
    },
    {
        .argc   = 4,
        .argv   = {"cookie", "write", NULL, NULL},
        .handle = cmd_bcookie_write,
    },
#endif
};

/*
* cmd have enabled when boot
*/
static int
__main(int argc, char *argv[])
{
    return cmd_handle(cmd, argc, argv, usage);
}

#ifndef __BUSYBOX__
#define otp_main  main
#endif

/*
* cmd have enabled when boot
*/
int otp_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);

    return os_main(__main, argc, argv);
}
/******************************************************************************/
