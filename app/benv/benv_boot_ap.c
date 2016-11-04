/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#include "benv_boot_common.c"
//#include <environment.h>

#define __benv_start(_env)    (BENV_START + benv_offset(_env))

#if 1
int __benv_read(int env, bool mirror)
{
	return 0;
}
int __benv_write(int env, int idx)
{
	return 0;
}
#else
int __benv_read(int env, bool mirror)
{
    /*
    * emmc==>block
    */
    debug_io("read benv%d ...", env);
    if (BENV_SIZE != __flash_read(__benv_start(env), __benv_env(env), BENV_SIZE)) {
         os_println("read benv%d error", env);
        
        return -EIO;
    }
    debug_io("read benv%d ok.", env);
    
    /*
    * block==>mirror
    */
    os_memcpy(__benv_mirror(env), __benv_env(env), BENV_SIZE);

    return 0;
}

int __benv_write(int env, int idx)
{
    debug_io("save benv%d ...", env);
    if (BENV_SIZE != __flash_write(__benv_start(env), __benv_env(env), BENV_SIZE)) {
         os_println("save benv%d error", env);

        return -EIO;
    }
    debug_io("save benv%d ok.", env);
    
    os_memcpy(__benv_mirror(env), __benv_env(env), BENV_SIZE);
    
    return 0;
}
#endif
/*
* call it in fastboot
*/
static void
benv_boot(void)
{
    benv_boot_init();
    benv_init();
//    md_boot_init();//
    
    benv_boot_check();
//    benv_boot_select();//
    benv_boot_save();
}

/******************************************************************************/
