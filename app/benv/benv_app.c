/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
OS_INITER;

static benv_env_t __benv;
BENV_CONTROL_INITER(&__benv);

int allinone_main(int argc, char *argv[])
{
    return __benv_main(argc, argv);
}
/******************************************************************************/
