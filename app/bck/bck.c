/*******************************************************************************
Copyright (c) 2016-2018, Supper Wali Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      bck
#endif

#include "utils.h"
#include "benv/benv.h"
#include "reg/reg.h"
#if 1==PRODUCT_VERSION
#include "../bootm/bootm.h"
#endif
#if !(IS_PRODUCT_LTEFI_MD || IS_PRODUCT_PC)
#error "just support ltefi md/pc board"
#endif

OS_INITER;

static int fd_boot      = INVALID_FD;
static int fd_bootenv   = INVALID_FD;
static int fd_cmdline   = INVALID_FD;

#define AT_CHECK_DEBUG  0
#if AT_CHECK_DEBUG
#define at_check_println(_fmt, _args...)    os_println(_fmt, ##_args)
#else
#define at_check_println(_fmt, _args...)    os_do_nothing()
#endif

static int
__fini(void)
{
    os_close(fd_cmdline);
    os_close(fd_bootenv);
    os_close(fd_boot);

    os_fini();

    return 0;
}

static int
__init(void)
{
    int fd, err;
    int permit = S_IRUSR | S_IRGRP;
        
    err = os_init();
    if (err<0) {
        return err;
    }

    do {
        fd = open(DEV_BOOT, O_RDONLY, permit);
        if (false==is_good_fd(fd)) {
            trace_error(-errno, "open " DEV_BOOT);
            
            sleep(1);
        }
    }while(false==is_good_fd(fd));
    fd_boot = fd;

    fd = open(DEV_BOOTENV, O_RDONLY, permit);
    if (false==is_good_fd(fd)) {
        trace_error(-errno, "open " OS_DEV_BOOTENV);
        
        return -errno;
    }
    fd_bootenv = fd;

    fd = open(OS_PROC_CMDLINE, O_RDONLY, permit);
    if (false==is_good_fd(fd)) {
        trace_error(-errno, "open " OS_PROC_CMDLINE);
        
        return -errno;
    }
    fd_cmdline = fd;

    return 0;
}

static inline int
hacked(int obj, int reason)
{
    os_println("system is hacked(%d.%d), reboot...", obj, reason);
    
    __os_system("reboot &");
    
    return -EHACKED;
}

static int
check_boot(void)
{
#if 2==PRODUCT_VERSION
    static os_reg_t dog_enable[] = AT_REG_DOG_ENABLE;
#endif
    static struct {
        char *addr;
        int size;
    } cookie[] = {
#if 2==PRODUCT_VERSION
        {
            .addr = (char *)dog_enable,
            .size = sizeof(dog_enable),
        },
#endif
        {
            .addr = CONFIG_BOOTARGS,
            .size = sizeof(CONFIG_BOOTARGS),
        },
    };

    at_check_println("before check boot");
    
    static char boot[BENV_BOOT_SIZE];
    if (BENV_BOOT_SIZE != read(fd_boot, boot, BENV_BOOT_SIZE)) {
        trace_error(-errno, "read boot");

        return -errno;
    }
    
    char *p     = boot;
    char *end   = boot + BENV_BOOT_SIZE;
    int i;
    
    for (i=0; i<os_count_of(cookie); i++) {
        p = os_memmem(p, end - p, cookie[i].addr, cookie[i].size);
        if (NULL==p) {
            return hacked(1, i);
        }
    }
    at_check_println("after  check boot");
    
    return 0;
}

static int
check_bootenv(void)
{
#if 1==PRODUCT_VERSION
    struct {
        char init[sizeof(AT_DEFT_INIT)-1];
    } cookie = {
        {0},
    }, deft = {
        AT_DEFT_INIT,
    };
#elif 2==PRODUCT_VERSION
    benv_cookie_t deft = BENV_DEFT_COOKIE;
    benv_cookie_t cookie;
#endif

    at_check_println("before check bootenv");
    
    if (sizeof(cookie) != read(fd_bootenv, &cookie, sizeof(cookie))) {
        trace_error(-errno, "read benv cookie");

        return -errno;
    }

    if (false==os_objeq(&cookie, &deft)) {
        return hacked(2, 0);
    }

    at_check_println("after  check bootenv");

    return 0;
}


static int
check_partition(void)
{
    char line[1+OS_LINE_LEN] = {0};
    
    at_check_println("before check partition");
    
    read(fd_cmdline, line, OS_LINE_LEN);
    
    if (NULL==os_strstr(line, CONFIG_BOOTARGS_BODY)) {
        return hacked(3, 0);
    }

    at_check_println("after  check partition");

    return 0;
}

/*
* cmd have enabled when boot
*/
static int
__main(int argc, char *argv[])
{
    static int (*check[])(void) = {
        check_boot,
        check_bootenv,
        check_partition,
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

#ifndef __BUSYBOX__
#define bck_main  main
#endif

/*
* cmd have enabled when boot
*/
int bck_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_call(__init, __fini, __main, argc, argv);
}
/******************************************************************************/
