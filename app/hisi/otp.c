/*******************************************************************************
Copyright (c) 2016-2018, Supper Wali Technology. All rights reserved.
*******************************************************************************/
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

static int
otp_main(int argc, char *argv[])
{
    return os_main(cmd_handle, cmd_otp, argc, argv, otp_usage);
}
#endif /* (IS_PRODUCT_LTEFI_MD || IS_PRODUCT_PC) */

