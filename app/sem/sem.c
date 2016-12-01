/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      sem
#endif

#include "utils.h"
#include "haenv/haenv.h"

OS_INITER;

STATIC os_sem_t sem_control;

STATIC int
sem_cmd_lock(int argc, char *argv[])
{
    char *key   = argv[1];
    char *wait  = argv[2];
    int err;
    
    err = os_sem_create(&sem_control, os_atoi(key));
    if (err<0) {
        return err;
    }
    
    os_sem_lock(&sem_control);

    sleep(os_atoi(wait));

    return 0;
}

STATIC int
sem_cmd_unlock(int argc, char *argv[])
{
    char *key   = argv[1];
    int err;
    
    err = os_sem_create(&sem_control, os_atoi(key));
    if (err<0) {
        return err;
    }
    
    os_sem_unlock(&sem_control);

    return 0;
}

STATIC cmd_table_t sem_cmd[] = {
    CMD_TABLE_ENTRY(sem_cmd_lock, 3, "lock", NULL, NULL),
    CMD_TABLE_ENTRY(sem_cmd_unlock, 2, "unlock", NULL),
};

STATIC int
sem_usage(void)
{
    os_eprintln(__THIS_APPNAME " lock {key} {wait}");
    os_eprintln(__THIS_APPNAME " unlock {key}");

    return -EFORMAT;
}

/*
* cmd have enabled when boot
*/
STATIC int
sem_main_helper(int argc, char *argv[])
{
    int err;

    err = cmd_handle(sem_cmd, argc, argv, sem_usage);
    
    return err;
}

int allinone_main(int argc, char *argv[])
{
    return os_main(sem_main_helper, argc, argv);
}
/******************************************************************************/
