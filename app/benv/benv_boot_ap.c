/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#include "benv_boot_common.c"

#define __benv_start(_env)    (CFG_FLASH_BASE + BENV_START + benv_offset(_env))

void *benv_flash_read(void *flash, void *memory, int size)
{
	printf("%s, %s, L%d: %p, %p, %d\n", 
		__FILE__, __func__, __LINE__, (uchar *)flash, (uchar *)memory, size);

	return os_memcpy(memory, flash, size);
	//return memcpy(memory, flash, size);
}


int __benv_read(int env, bool mirror)
{
    void *buf;
    char *name;

    if (mirror) {
        buf = __benv_mirror;
        name = "mirror";
    } else {
        buf = __benv_env(env);
        name = "benv";
    }
    
    /*
    * emmc==>block
    */
    debug_io("read benv%d to %s ...", env, name);
	printf("%s, %s, L%d: %d, %d, %p, %p\n", 
		__FILE__, __func__, __LINE__, env, mirror, (void *)__benv_start(env), buf);

	benv_flash_read((void *)__benv_start(env), buf, BENV_SIZE);

    debug_io("read benv%d to %s ok.", env, name);
    
    return 0;
}

/*
* call it in fastboot
*/
static void
benv_boot(void)
{
    benv_control_fini();
    benv_init();
//    md_boot_init();//md only now
    
    benv_boot_check();
//    benv_boot_select();//md only now
    benv_boot_save();
}

/******************************************************************************/
