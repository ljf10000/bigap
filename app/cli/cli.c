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
typedef struct {
    char *name;
    cli_client_t *client;
} cli_entry_t;

/*
* insert new cli here !!!
*/
#define DECLARE_X(_) \
    _(umc,  umd)    \
    _(smc,  smd)    \
    _(tmc,  tmd)    \
    _(nsqc, nsqa)   \
    _(rshc, rsha)   \
    /* end */

#define X_CLIENT(_cname, _sname)    static cli_client_t _cname = CLI_CLIENT_INITER(#_sname); os_fake_declare
#define X_ENTRY(_cname, _sname)     { #_cname, &_cname },

DECLARE_X(X_CLIENT);
static cli_entry_t cli[] = { DECLARE_X(X_ENTRY) };

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
