/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#define _LINUX_LIST_H
#include <environment.h>

static benv_ops_t __ops[]     = BENV_DEFT_OPS;
static benv_cache_t __cache[os_count_of(__ops)];
benv_control_t benv_control     = BENV_CONTROL_DEFT(NULL, __ops, __cache);

extern env_t *env_ptr;
DECLARE_REAL_DEBUGGER;
DECLARE_REAL_JDEBUGGER;

static void
benv_boot_init(void)
{
#ifdef __BOOT__
    __benv_control->env = (benv_env_t *)(env_ptr->env);
#endif
}
/******************************************************************************/
