/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      sem
#endif

#define __COMMAND__
#include "utils.h"
#include "haenv/haenv.h"

OS_INITER;

static os_sem_t sem;

static int
cmd_lock(int argc, char *argv[])
{
    char *key   = argv[1];
    char *wait  = argv[2];
    int err;
    
    err = os_sem_create(&sem, os_atoi(key));
    if (err<0) {
        return err;
    }
    
    os_sem_lock(&sem);

    sleep(os_atoi(wait));

    return 0;
}

static int
cmd_unlock(int argc, char *argv[])
{
    char *key   = argv[1];
    int err;
    
    err = os_sem_create(&sem, os_atoi(key));
    if (err<0) {
        return err;
    }
    
    os_sem_unlock(&sem);

    return 0;
}

static cmd_table_t cmd[] = {
    CMD_TABLE_ENTRY(cmd_lock, 3, "lock", NULL, NULL),
    CMD_TABLE_ENTRY(cmd_unlock, 2, "unlock", NULL),
};

static int
usage(void)
{
    os_eprintln(__THIS_APPNAME " lock {key} {wait}");
    os_eprintln(__THIS_APPNAME " unlock {key}");

    return -EFORMAT;
}

/*
* cmd have enabled when boot
*/
static int
__main(int argc, char *argv[])
{
    int err;

    err = cmd_handle(cmd, argc, argv, usage);
    
    return err;
}

int allinone_main(int argc, char *argv[])
{
    return os_main(__main, argc, argv);
}
/******************************************************************************/
