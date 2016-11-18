/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      pipe
#endif

#include "utils.h"

OS_INITER;

static char *genv[] = {
    "SB=SBSBSBSBSBSBSBSBSBSB",
    "SB=SB",
    NULL,
};

static char *gargv[1024];

static int 
callback(int error, char *outsring, char *errstring)
{
    os_println("stdout:%s", outsring);
    os_println("stderr:%s", errstring);
    os_println("errno:%d", error);

    return 0;
}

static int
run(char *line)
{
    pipinfo_t info = PIPINFO_INITER(genv, callback);

    return os_pexecline(&info, line);
}

int main(int argc, char *argv[])
{
    int err = 0;
    char buf[1024];
    char *line;
    
    while(1) {
        os_memzero(buf, sizeof(buf));
        
        line = fgets(buf, sizeof(buf)-1, stdin);
        if (false==is_good_str(line) || '\n'==line[0]) {
            continue;
        }

        os_str_strim_both(line, NULL);
        
        run(line);
    }

    return 0;
}
/******************************************************************************/
