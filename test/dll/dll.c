/******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      testdll
#endif

#define OS_EXTEND

#include "utils.h"

OS_INITER;

static int
dll_printf(void)
{
    libval_t params[] = {
        [0] = LIBVALp_INITER("libc %s number %d" __crlf),
        [1] = LIBVALp_INITER("printf"),
        [2] = LIBVAL4_INITER(100),
    };
    libproto_t proto = LIBPROTO_INITER(int, params);

    return os_libcall("libc.so.6", "printf", &proto);
}

static int
dll_memory(void)
{
    void *pointer = NULL;
    
    {
        libval_t params[] = {
            [0] = LIBVAL4_INITER(1024),
        };
        libproto_t proto = LIBPROTO_INITER(void *, params);
        
        os_libcall("libc.so.6", "malloc", &proto);
        pointer = (void *)proto.result.u.p;

        os_println("libc malloc %p", pointer);
    }
    
    {
        libval_t params[] = {
            [0] = LIBVAL4_INITER(pointer),
        };
        libproto_t proto = __LIBPROTO_INITER(0, params);
        
        os_libcall("libc.so.6", "free", &proto);
    }
    
    return 0;
}

static int
dll_tm(void)
{
    time_t t;
    struct tm *tm;

    {
        libval_t params[] = {
            [0] = LIBVALp_INITER(NULL),
        };
        libproto_t proto = LIBPROTO_INITER(time_t, params);

        os_libcall("libc.so.6", "time", &proto);

        t = (time_t)proto.result.u.b4;
    }

    {
        libval_t params[] = {
            [0] = LIBVALp_INITER(&t),
        };
        libproto_t proto = LIBPROTO_INITER(struct tm *, params);
        
        os_libcall("libc.so.6", "gmtime", &proto);

        tm = (struct tm *)proto.result.u.p;
    }

    os_println("time is %s", __fulltime_string(tm, __os_fulltime_ifs));

    return 0;
}

static int
__main(int argc, char *argv[])
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
    
    return os_main(__main, argc, argv);
}

/******************************************************************************/
