/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#define _LINUX_LIST_H
#include <environment.h>

BENV_CONTROL_INITER(NULL);

DECLARE_REAL_DEBUGGER;
DECLARE_REAL_JDEBUGGER;

static void
benv_boot_init(void)
{
#ifdef __BOOT__
    extern env_t *env_ptr;
    
    __benv_control->env = (benv_env_t *)(env_ptr->env);
#endif
}
/******************************************************************************/
