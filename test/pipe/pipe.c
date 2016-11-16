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
callback(int err, simple_buffer_t *sbout, simple_buffer_t *sberr)
{
    os_println("stdout:%s", sbout->buf);
    os_println("stderr:%s", sberr->buf);
    os_println("errno:%d", err);

    return 0;
}

static int
run(char *line)
{
    pipinfo_t info = PIPEINFO_INITER(genv, callback);
    char *p;
    int err, count = 0;
    char *input = os_strdup(line);
    
    os_strtok_foreach(p, input, " ") {
        gargv[count++] = p;
    }

    if (os_file_exist(gargv[0])) {
        info.file = gargv[0];

        err = os_pexecv(info);
    } else {
        info.content = line;
        
        err = os_pexec(info, "%s", line);
    }
    
    os_free(input);

    return 0;
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
        
        os_println("get-line:%s", line);
        
        run(line);
    }

    return 0;
}
/******************************************************************************/
