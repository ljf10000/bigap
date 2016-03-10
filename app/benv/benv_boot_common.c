/*******************************************************************************
Copyright (c) 2015-2016, xxx Networks. All rights reserved.
*******************************************************************************/
#define _LINUX_LIST_H
#include <environment.h>

static benv_ops_t __ops[]     = BENV_DEFT_OPS;
static benv_cache_t __cache[os_count_of(__ops)];
benv_control_t benv_control     = BENV_CONTROL_DEFT(NULL, __ops, __cache);

extern env_t *env_ptr;
DECLARE_REAL_DEBUGGER;
DECLARE_REAL_JSDEBUGGER;

/*
* call it in fastboot
*/
static void
benv_boot_init(void)
{
    __benv_control->env = (benv_env_t *)(env_ptr->env);
    
    __THIS_DEBUG    = benv_mark_pointer(__benv_mark_debug);
    __THIS_JSDEBUG  = benv_mark_pointer(__benv_mark_jsdebug);
}

/******************************************************************************/
