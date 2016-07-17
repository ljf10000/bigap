/*******************************************************************************
Copyright (c) 2016-2018, Supper Wali Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      hisi
#endif

#include "hisi/hisi.h"

OS_INITER;
/******************************************************************************/
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
#define USAGE_MODULE    __THIS_APPNAME " dog"
    os_eprintln(USAGE_MODULE "enable");
    os_eprintln(USAGE_MODULE "disable");
    os_eprintln(USAGE_MODULE "clear");
    os_eprintln(USAGE_MODULE "reset");
    os_eprintln(USAGE_MODULE "timeout [timeout(ms)]");
#undef USAGE_MODULE

    return -EFORMAT;
}

static cmd_table_t cmd_dog[] = {
    {
        .argc   = 1,
        .argv   = {"enable"},
        .handle = cmd_dog_enable,
    },
    {
        .argc   = 1,
        .argv   = {"disable"},
        .handle = cmd_dog_disable,
    },
    {
        .argc   = 1,
        .argv   = {"clear"},
        .handle = cmd_dog_clear,
    },
    {
        .argc   = 1,
        .argv   = {"reset"},
        .handle = cmd_dog_reset,
    },
    {
        .argc   = 1,
        .argv   = {"timeout"},
        .handle = cmd_dog_get_timeout,
    },
    {
        .argc   = 2,
        .argv   = {"timeout", NULL},
        .handle = cmd_dog_set_timeout,
    },
};
#endif /* IS_PRODUCT_LTEFI_MD */
/******************************************************************************/
#if (IS_PRODUCT_LTEFI_MD || IS_PRODUCT_PC)
static int
cmd_otp_custom_show(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    return otp_custom_show();
}

static int
cmd_otp_custom_write(int argc, char *argv[])
{
    return otp_custom_write((byte *)argv[2]);
}

static int
cmd_otp_private_show(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    return otp_private_show();
}

static int
cmd_otp_private_write(int argc, char *argv[])
{
    byte otp[OTP_SIZE];
    int count;
    
    count = os_hextobin(argv[2], otp, OTP_SIZE);
    if (OTP_SIZE!=count) {
        return -EFORMAT;
    }
    
    return otp_private_write(otp);
}

#if IS_PRODUCT_PC || IS_PRODUCT_LTEFI_MD_PARTITION_B
static int
cmd_otp_bcookie_show(int argc, char *argv[])
{
    int err;
    struct bcookie_otp botp = BCOOKIE_OBJ(BCOOKIE_OTP);
    
    (void)argc;
    (void)argv;
    
    err = bcookie_load((struct bcookie *)&botp, sizeof(botp));
    if (err<0) {
        return err;
    }

    os_println("%s", __otp_string(botp.custom));
    os_println("%s", __otp_string(botp.private));

    return 0;
}

static int
cmd_otp_bcookie_write(int argc, char *argv[])
{
    int count;
    struct bcookie_otp botp = BCOOKIE_OBJ(BCOOKIE_OTP);
    
    count = os_hextobin(argv[2], botp.custom, OTP_SIZE);
    if (count<0) {
        return count;
    }
    
    count = os_hextobin(argv[3], botp.private, OTP_SIZE);
    if (count<0) {
        return count;
    }
    
    return bcookie_save((struct bcookie *)&botp, sizeof(botp));
}
#endif

static int
otp_usage(void)
{
#define USAGE_MODULE    __THIS_APPNAME " otp"
    os_eprintln(__THIS_APPNAME " custom  show");
    os_eprintln(__THIS_APPNAME " private show");
#undef USAGE_MODULE

    return -EFORMAT;
}

static cmd_table_t cmd_otp[] = {
    {
        .argc   = 2,
        .argv   = {"custom", "show"},
        .handle = cmd_otp_custom_show,
    },
    {
        .argc   = 3,
        .argv   = {"custom", "write", NULL},
        .handle = cmd_otp_custom_write,
    },
    {
        .argc   = 2,
        .argv   = {"private", "show"},
        .handle = cmd_otp_private_show,
    },
    {
        .argc   = 3,
        .argv   = {"private", "write", NULL},
        .handle = cmd_otp_private_write,
    },
#if IS_PRODUCT_PC || IS_PRODUCT_LTEFI_MD_PARTITION_B
    {
        .argc   = 2,
        .argv   = {"cookie", "show"},
        .handle = cmd_otp_bcookie_show,
    },
    {
        .argc   = 4,
        .argv   = {"cookie", "write", NULL, NULL},
        .handle = cmd_otp_bcookie_write,
    },
#endif
};
#endif /* (IS_PRODUCT_LTEFI_MD || IS_PRODUCT_PC) */
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
#endif

#if (IS_PRODUCT_LTEFI_MD || IS_PRODUCT_PC)
    {
        .module = "otp",
        .table  = cmd_otp,
        .count  = os_count_of(cmd_otp),
        .usage  = otp_usage,
        .init   = NULL,
        .fini   = NULL,
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
