/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#define _LINUX_LIST_H
#include <environment.h>

BENV_MAIN_INITER;

DECLARE_REAL_DEBUGGER;
DECLARE_REAL_JDEBUGGER;

/*
* bootargs/bootcmd is dirty???
*/
bool bootenv_dirty;

static void 
benv_boot_check(void) 
{
    benv_show_cookie();

    benv_check();
}

static void
benv_boot_save(void)
{
    if (bootenv_dirty) {
        if (os_streq("force", getenv("benvsave"))) {
            os_println("benv force save ...");
            {
                benv_save();
            }
            os_println("benv force save end");
        }
        
        os_println("bootenv update ..."); 
        {
            env_crc_update();
            saveenv(); /* include benv */
            bootenv_dirty = false;
        }
        os_println("bootenv update ok.");
    } else {
        os_println("benv normal save ...");
        {
            benv_save();
        }
        os_println("benv normal save end");
    }
}

static void
benv_boot_init(void)
{
#ifdef __BOOT__
    extern env_t *env_ptr;
    
    __benv_control->env = (benv_env_t *)(env_ptr->env);
#endif
}
/******************************************************************************/
