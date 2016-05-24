/*******************************************************************************
Copyright (c) 2016-2018, Supper Wali Technology. All rights reserved.
*******************************************************************************/
#ifdef BIGAP
#ifndef __THIS_APP
#define __THIS_APP      cmd_benv
#endif

#include "benv/benv.h"

static int 
do_benv (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    return __benv_main(argc, argv);
}

U_BOOT_CMD(
    benv,  CONFIG_SYS_MAXARGS, 0,  do_benv,
    "get/set benv",
    "\n"
    "    benv name ==> show env by name\n"
    "    benv name1=value1 name2=value2 ... ==> set env by name and value"
);

static inline int
__bversion(int argc, char *argv[])
{
    int i, err;
    benv_version_t version;

    err = __benv_version_atoi(&version, argv[1]);
    if (err<0) {
        return err;
    }
    
    for (i=0; i<OS_FIRMWARE_COUNT; i++) {
        os_objcpy(benv_rootfs_version(i), &version);
        os_objcpy(benv_kernel_version(i), &version);
    }

    benv_dirty_byidx(BENV_OS);
}

static int 
do_bversion (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    return os_call(__benv_init, __benv_fini, __bversion, argc, argv);
}

U_BOOT_CMD(
    bversion,  CONFIG_SYS_MAXARGS, 0,  do_bversion,
    "set bversion",
    "\n"
    "    bversion version\n"
    "    bversion version\n"
);

static int 
do_endian (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    uint32 v = 0xaabbccdd;  
    byte c = *(byte *)(&v);  

    switch(c) {
        case 0xaa:
            os_println("big"); 
            break;
        case 0xdd:
            os_println("little"); 
            break;
        default:
            os_println("unknow"); 
            break;
    }

    return 0;
}

U_BOOT_CMD(
    endian,  CONFIG_SYS_MAXARGS, 0,  do_endian,
    "endian test",
    "\n"
    "    endian test\n"
    "    endian test\n"
);

#endif
/******************************************************************************/
