/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      cli
#endif

#include "utils.h"
#include "um/um.h"
#include "sm/sm.h"
#include "tm/tm.h"
#include "nsq/nsq.h"
#include "rsh/rsh.h"

OS_INITER;
/******************************************************************************/
static cli_client_t umc     = CLI_CLIENT_INITER("umd");
static cli_client_t smc     = CLI_CLIENT_INITER("smd");
static cli_client_t tmc     = CLI_CLIENT_INITER("tmd");
static cli_client_t nsqc    = CLI_CLIENT_INITER("nsqa");
static cli_client_t rshc    = CLI_CLIENT_INITER("rsha");

static struct {
    char *name;
    cli_client_t *client;
} cli[] = {
    { "umc",    &umc },
    { "smc",    &smc },
    { "tmc",    &tmc },
    { "nsqc",   &nsqc },
    { "rshc",   &rshc },
};

static int
cli_usage(int argc, char *argv[])
{
    int i;

    os_eprintln("cli usage:");
    for (i=0; i<os_count_of(cli); i++) {
        os_eprintln(__tab "cli %s", cli[i].name);
    }

    return -EHELP;
}

static cli_client_t *
cli_find(char *name)
{
    int i;

    if (NULL==name) {
        return NULL;
    }
    
    for (i=0; i<os_count_of(cli); i++) {
        if (os_streq(name, cli[i].name)) {
            return cli[i].client;
        }
    }

    return NULL;
}

STATIC int 
cli_main_helper(int argc, char *argv[])
{
    cli_client_t *client = cli_find(argv[1]);
    if (NULL==client) {
        return cli_usage(argc, argv);
    } else {
        return cli_request(client, argc-2, argv+2);
    }
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_ignore(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(cli_main_helper, argc, argv);
}

/******************************************************************************/
