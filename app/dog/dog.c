/*******************************************************************************
Copyright (c) 2016-2018, Supper Wali Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      dog
#endif

#include "utils.h"
#include "hi_unf_wdg.h"

OS_INITER;

#if 0
HI_S32 HI_UNF_WDG_Init(HI_VOID);
HI_S32 HI_UNF_WDG_DeInit(HI_VOID);
HI_S32 HI_UNF_WDG_Enable(HI_U32 u32WdgNum);
HI_S32 HI_UNF_WDG_Disable(HI_U32 u32WdgNum);
HI_S32 HI_UNF_WDG_GetTimeout(HI_U32 u32WdgNum, HI_U32 *pu32Value);
HI_S32 HI_UNF_WDG_SetTimeout(HI_U32 u32WdgNum, HI_U32 u32Value);
HI_S32 HI_UNF_WDG_Clear(HI_U32 u32WdgNum);
HI_S32 HI_UNF_WDG_Reset(HI_U32 u32WdgNum);
#endif

#define method(_method, _dog, _args...)  ({ \
    int __err;                              \
                                            \
    __err = HI_UNF_WDG_##_method(_dog, ##_args); \
        debug_trace_error(__err, "dog%d " #_method, _dog); \
                                            \
    __err;                                  \
})

static int
enable(int dog)
{
    return method(Enable, dog);
}

static int
disable(int dog)
{
    return method(Disable, dog);
}

static int
clear(int dog)
{
    return method(Clear, dog);
}

static int
reset(int dog)
{
    return method(Reset, dog);
}

static int
get_timeout(int dog, int *timeout)
{
    return method(GetTimeout, dog, timeout);
}

static int
set_timeout(int dog, int timeout)
{
    return method(SetTimeout, dog, timeout);
}

#define foreach(_method, _args...)   ({ \
    int __i, __err;                     \
                                        \
    for (__i=0; __i<DOG_COUNT; __i++) { \
        __err = _method(__i, ##_args);  \
        if (__err) {                    \
            return __err;               \
        }                               \
    }                                   \
                                        \
    0;                                  \
})

static int
cmd_enable(int argc, char *argv[])
{
    return foreach(enable);
}

static int
cmd_disable(int argc, char *argv[])
{
    foreach(clear);
    
    return foreach(disable);
}

static int
cmd_clear(int argc, char *argv[])
{
    return foreach(clear);
}

static int
cmd_reset(int argc, char *argv[])
{
    return foreach(reset);
}

static int
cmd_get_timeout(int argc, char *argv[])
{
    int i, err, timeout[DOG_COUNT];

    for (i=0; i<DOG_COUNT; i++) {
        err = get_timeout(i, &timeout[i]);
        if (err<0) {
            return err;
        }
    }
    
    for (i=1; i<DOG_COUNT; i++) {
        if (timeout[i-1] != timeout[i]) {
            debug_error("dog%d's timeout(%d) != dog%d's timeout(%d)",
                i-1,
                timeout[i-1],
                i,
                timeout[i]);
            
            return INVALID_VALUE;
        }
    }

    os_println("%d", timeout[0]);
    
    return 0;
}

static int
cmd_set_timeout(int argc, char *argv[])
{
    int timeout;
    char *end = NULL;
    char *string = argv[1];
    
    timeout = os_strtol(string, &end, 0);
    if (false==os_strton_is_good_end(end) || timeout <= 0) {
        os_println("bad timeout:%s", string);
        
        return -EFORMAT;
    } else {
        return foreach(set_timeout, timeout);
    }
}

static cmd_table_t cmd[] = {
    {
        .argc   = 1,
        .argv   = {"enable"},
        .handle = cmd_enable,
    },
    {
        .argc   = 1,
        .argv   = {"disable"},
        .handle = cmd_disable,
    },
    {
        .argc   = 1,
        .argv   = {"clear"},
        .handle = cmd_clear,
    },
    {
        .argc   = 1,
        .argv   = {"reset"},
        .handle = cmd_reset,
    },
    {
        .argc   = 1,
        .argv   = {"timeout"},
        .handle = cmd_get_timeout,
    },
    {
        .argc   = 2,
        .argv   = {"timeout", NULL},
        .handle = cmd_set_timeout,
    },
};

static int
usage(void)
{
    os_eprintln(__THIS_APP_NAME " enable");
    os_eprintln(__THIS_APP_NAME " disable");
    os_eprintln(__THIS_APP_NAME " clear");
    os_eprintln(__THIS_APP_NAME " reset");
    os_eprintln(__THIS_APP_NAME " timeout [timeout(ms)]");

    return -EFORMAT;
}

#define __method(_method)           ({  \
    int __err = HI_UNF_WDG_##_method(); \
    if (__err) {                        \
        debug_error(#_method " error:%d", __err); \
    }                                   \
                                        \
    __err;                              \
})  /* end */

static int
__fini(void)
{
    __method(DeInit);
    
    os_fini();
    
    return 0;
}

static int
__init(void)
{
    int err;
    
    err = os_init();
    if (err<0) {
        return err;
    }
    
    do {
        err = __method(Init);
        if (err<0) {
            sleep(1);
        }
    } while(err);
    
    return 0;
}

#ifndef __BUSYBOX__
#define dog_main  main
#endif

static int
__main(int argc, char *argv[])
{
    return cmd_handle(cmd, argc, argv, usage);
}

/*
* dog have enabled when boot
*/
int dog_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_call(__init, __fini, __main, argc, argv);
}
/******************************************************************************/
