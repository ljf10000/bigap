/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      dll
#endif

#include "utils.h"

OS_INITER;

STATIC int
dll_printf(void)
{
    libval_t params[] = {
        [0] = LIBVAL_S_INITER(LIBVAL_IN, "libc %s number %d" __crlf),
        [1] = LIBVAL_S_INITER(LIBVAL_IN, "printf"),
        [2] = LIBVAL_I4_INITER(LIBVAL_IN, 100),
    };
    libproto_t proto = LIBPROTO_INITER(int, params);

    return os_libcall("libc.so.6", "printf", &proto);
}

STATIC int
dll_memory(void)
{
    void *pointer = NULL;
    
    {
        libval_t params[] = {
            [0] = LIBVAL_I4_INITER(LIBVAL_IN, 1024),
        };
        libproto_t proto = LIBPROTO_INITER(void *, params);

        os_libcall("libc.so.6", "malloc", &proto);
        pointer = (void *)proto.result.u.p;

        os_println("libc malloc %p", pointer);
    }
    
    {
        libval_t params[] = {
            [0] = LIBVAL_P_INITER(LIBVAL_IN, pointer),
        };
        libproto_t proto = __LIBPROTO_INITER(0, params);
        
        os_libcall("libc.so.6", "free", &proto);
    }
    
    return 0;
}

STATIC int
dll_tm(void)
{
    time_t t;
    struct tm *tm;

    {
        libval_t params[] = {
            [0] = LIBVAL_P_INITER(LIBVAL_IN, NULL),
        };
        libproto_t proto = LIBPROTO_INITER(time_t, params);

        os_libcall("libc.so.6", "time", &proto);

        t = (time_t)proto.result.u.u4;
    }

    {
        libval_t params[] = {
            [0] = LIBVAL_P_INITER(LIBVAL_IN, &t),
        };
        libproto_t proto = LIBPROTO_INITER(struct tm *, params);
        
        os_libcall("libc.so.6", "gmtime", &proto);

        tm = (struct tm *)proto.result.u.p;
    }

    os_println("time is %s", __fulltime_string(tm, __os_fulltime_ifs));

    return 0;
}

STATIC int
dll_main(int argc, char *argv[])
{
    int i, err = 0;

    int (*array[])(void) = {
        dll_printf,
        dll_memory,
        dll_tm,
    };

    for (i=0; i<os_count_of(array); i++) {
        err = (*array[i])();
        if (err<0) {
            return err;
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(dll_main, argc, argv);
}

/******************************************************************************/
