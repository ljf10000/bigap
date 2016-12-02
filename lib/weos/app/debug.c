/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#if __BACKTRACE__==BACKTRACE_by_libunwind
#include <libunwind.h>

void 
__sighandle_callstack(int signo)
{
    unw_cursor_t    cursor;
    unw_context_t   context;

    unw_getcontext(&context);
    unw_init_local(&cursor, &context);

    while (unw_step(&cursor) > 0) {
        unw_word_t  offset, pc;
        char        fname[128];

        unw_get_reg(&cursor, UNW_REG_IP, &pc);

        fname[0] = '\0';
        (void) unw_get_proc_name(&cursor, fname, sizeof(fname), &offset);

        printf ("%p : (%s+0x%x) [%p]\n", (void *)pc, fname, offset, (void *)pc);
    }

    exit(signo);
}

#elif __BACKTRACE__==BACKTRACE_by_libc_backtrace
#include <execinfo.h>

void 
__sighandle_callstack(int signo)
{
    void *array[BACKTRACE_ASIZE] = {NULL};
    int i, count, fd;
    int pid = getpid();
    uint32 now = (uint32)time(NULL);
    
    /*
    * create app trace file
    */
    fd = os_v_open(O_WRONLY | O_CREAT | O_TRUNC, BACKTRACE_FILE_TRACE);
    count = backtrace(array, os_count_of(array));    
    backtrace_symbols_fd(array, count, fd);
    close(fd);
    
    os_println("backtrace array count(%d)", count);
    for (i=0; i<count; i++) {
        os_println(__tab "backtrace array[%d]=%p", i, array[i]);
    }
    os_println("backtrace array end");
    
    /*
    * copy app maps file
    */
    os_system("cp /proc/%d/maps " BACKTRACE_FILE_MAPS, pid);
    os_system("chmod +w " BACKTRACE_FILE_MAPS);
    
    /*
    * locate bug file & line
    */
{
    char lib[256];
    unsigned long base_address = 0;
    unsigned long dead_address = (unsigned long)array[3];
    
    STREAM stream = os_v_popen("r", 
        "awk -F\"[- ]\" '{if($1<=\"%lx\" && $2>=\"%lx\") print$1, $NF}' "
        BACKTRACE_FILE_MAPS,
        dead_address,
        dead_address);
    fscanf(stream, "%lx %s", &base_address, lib);
    
    if (NULL==strstr(lib, ".so.")) {
        /*
        * exe
        */
        os_system("addr2line -e ./%s 0x%lx > " BACKTRACE_FILE_LOCATE, __THIS_APPNAME, dead_address);
    } else {
        /*
        * lib
        */
        os_system("addr2line -e %s 0x%lx > " BACKTRACE_FILE_LOCATE, lib, dead_address - base_address);
    }
}

/*
    $(APP) is exe or lib's filename
    $(ADDRESS) is dead address, format as AABBCCDD
    
    readelf -s $(APP) | sort -k 2 | awk 'BEGIN{address="00000000";name=""}{if($2<="$(ADDRESS)"){address=$2;name=$8}}END{print address,name}'
*/
    exit(signo);
}
#endif /* BACKTRACE_TYPE */

void
__setup_signal(void (*handle)(int signo), int sigs[], int count)
{
    struct sigaction s = {
        .sa_flags   = 0,
        .sa_handler = handle,
    };
    int i;
    
	for (i=0; i<count; i++) {
        sigaction(sigs[i], &s, NULL);
	}
}

/******************************************************************************/
