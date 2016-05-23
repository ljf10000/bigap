/*******************************************************************************
Copyright (c) 2016-2018, Supper Wali Technology. All rights reserved.
*******************************************************************************/
OS_INITER;

static benv_env_t __benv        = BENF_DEFT;
static benv_ops_t __ops[]       = BENV_DEFT_OPS;
static benv_cache_t __cache[os_count_of(__ops)];
benv_control_t benv_control     = BENV_CONTROL_DEFT(&__benv, __ops, __cache);

#ifndef __BUSYBOX__
#define benv_main  main
#endif

int benv_main(int argc, char *argv[])
{
    return __benv_main(argc, argv);
}
/******************************************************************************/
