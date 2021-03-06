/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      bck
#endif

#include "utils.h"
#if IS_PRODUCT_LTEFI_MD_PARTITION_A
#include "../bootm/bootm.h"
#elif IS_PRODUCT_LTEFI_MD_PARTITION_B
#include "benv/benv.h"
#endif
#include "hisi/hisi.h"
#if !(IS_PRODUCT_LTEFI_MD || IS_PRODUCT_PC)
#error "just support ltefi md/pc board"
#endif

OS_INITER;

static int fd_boot      = INVALID_FD;
static int fd_bootenv   = INVALID_FD;
static int fd_cmdline   = INVALID_FD;

#define BCK_PRINT       0

#if BCK_PRINT
#define bck_println(_fmt, _args...)    os_println(_fmt, ##_args)
#else
#define bck_println(_fmt, _args...)    os_do_nothing()
#endif

STATIC int
bck_fini(void)
{
    os_close(fd_cmdline);
    os_close(fd_bootenv);
    os_close(fd_boot);

    os_fini();

    return 0;
}

STATIC int
bck_init(void)
{
    int fd, err;
    int permit = S_IRUSR | S_IRGRP;
        
    err = os_init();
    if (err<0) {
        return err;
    }

    do {
        fd = open(PRODUCT_DEV_BOOT, O_RDONLY, permit);
        if (false==is_good_fd(fd)) {
            trace_error(-errno, "open " PRODUCT_DEV_BOOT);
            
            sleep(1);
        }
    }while(false==is_good_fd(fd));
    fd_boot = fd;

    fd = open(PRODUCT_DEV_BOOTENV, O_RDONLY, permit);
    if (false==is_good_fd(fd)) {
        trace_error(-errno, "open " PRODUCT_DEV_BOOTENV);
        
        return -errno;
    }
    fd_bootenv = fd;

    fd = open(PRODUCT_PROC_CMDLINE, O_RDONLY, permit);
    if (false==is_good_fd(fd)) {
        trace_error(-errno, "open " PRODUCT_PROC_CMDLINE);
        
        return -errno;
    }
    fd_cmdline = fd;

    return 0;
}

STATIC int
bck_hacked(int obj, int reason)
{
    os_println("system is hacked(%d.%d), reboot...", obj, reason);
    
    os_system_helper("reboot &");
    
    return -EHACKED;
}

STATIC int
bck_check_boot(void)
{
#if IS_PRODUCT_LTEFI_MD_PARTITION_B
    os_reg_t dog_enable[] = OS_DOG_ENABLE;
#endif
    benv_cookie_t deft = BENV_DEFT_COOKIE;
    struct {
        char *addr;
        int size;
    } cookie[] = {
#if IS_PRODUCT_LTEFI_MD_PARTITION_B
        {
            .addr = (char *)dog_enable,
            .size = sizeof(dog_enable),
        },
#endif
        {
            .addr = CONFIG_BOOTARGS,
            .size = sizeof(CONFIG_BOOTARGS),
        },
        /*
        * just find FIXED in boot
        */
        {
            .addr = &deft,
            .size = BENV_COOKIE_FIXED,
        }
    };

    bck_println("before check boot");
    
    static char boot[PRODUCT_BOOT_SIZE];
    if (PRODUCT_BOOT_SIZE != read(fd_boot, boot, PRODUCT_BOOT_SIZE)) {
        trace_error(-errno, "read boot");

        return -errno;
    }
    
    char *p     = boot;
    char *end   = boot + PRODUCT_BOOT_SIZE;
    int i;
    
    for (i=0; i<os_count_of(cookie); i++) {
        p = os_memmem(p, end - p, cookie[i].addr, cookie[i].size);
        if (NULL==p) {
            return bck_hacked(1, i);
        }
    }
    bck_println("after  check boot");
    
    return 0;
}

STATIC int
bck_check_bootenv(void)
{
#if IS_PRODUCT_LTEFI_MD_PARTITION_A
    struct {
        char init[sizeof(AT_DEFT_INIT)-1];
    } cookie = {
        {0},
    }, deft = {
        AT_DEFT_INIT,
    };
#elif IS_PRODUCT_PC || IS_PRODUCT_LTEFI_MD_PARTITION_B
    benv_cookie_t cookie;
    benv_cookie_t deft = BENV_DEFT_COOKIE;
#endif

    bck_println("before check bootenv");
    
    if (sizeof(cookie) != read(fd_bootenv, &cookie, BENV_COOKIE_FIXED)) {
        trace_error(-errno, "read benv cookie");

        return -errno;
    }
    
    if (false==os_memeq(&cookie, &deft, BENV_COOKIE_FIXED)) {
        return bck_hacked(2, 0);
    }

    bck_println("after  check bootenv");

    return 0;
}

STATIC int
bck_check_partition(void)
{
    char line[1+OS_LINE_LEN] = {0};
    
    bck_println("before check partition");
    
    read(fd_cmdline, line, OS_LINE_LEN);
    
    if (NULL==os_strstr(line, PRODUCT_BOOTARGS_BODY)) {
        return bck_hacked(3, 0);
    }

    bck_println("after  check partition");

    return 0;
}

/*
* cmd have enabled when boot
*/
STATIC int
bck_main_helper(int argc, char *argv[])
{
    static int (*check[])(void) = {
        bck_check_boot,
        bck_check_bootenv,
        bck_check_partition,
    };
    
    int i, err;

    for (i=0; i<os_count_of(check); i++) {
        err = (*check[i])();
        if (err<0) {
            return err;
        }
    }
    
    return 0;
}

/*
* cmd have enabled when boot
*/
int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_ignore(NULL);
    setup_signal_callstack(NULL);
    
    int err = os_call(bck_init, bck_fini, bck_main_helper, argc, argv);

    return shell_error(err);
}
/******************************************************************************/
