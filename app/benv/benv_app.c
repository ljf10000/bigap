/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
OS_INITER;

benv_env_t ____benv_env[BENV_COUNT];
char ____benv_zero[BENV_COUNT*BENV_SIZE];
BENV_CONTROL_INITER(____benv_env);

int allinone_main(int argc, char *argv[])
{
    return __benv_main(argc, argv);
}
/******************************************************************************/
