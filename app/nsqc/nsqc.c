/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      nsqc
#endif

#include "utils.h"

OS_INITER;

STATIC cli_client_t nsqc = CLI_CLIENT_INITER("nsqa");

#define nsqc_handle(_action, _argc, _argv) \
    clic_sync_handle(&nsqc, _action, _argc, _argv)

STATIC int
nsqc_usage(int error)
{
    os_eprintln(__THIS_APPNAME " insert {json}");
    os_eprintln(__THIS_APPNAME " remove {name|*} {topic|*} {channel|*}");
    os_eprintln(__THIS_APPNAME " show [{name|*} {topic|*} {channel|*}]");

    return error;
}

/*
* ACTION name topic channel
*/
STATIC int
nsqc_handle_name_topic_channel(char *action, int argc, char *argv[], bool allow_all_empty)
{
    if (argc < 1) {
        return nsqc_usage(-EINVAL0);
    }
    else if (argc > 3) {
        return nsqc_usage(-EINVAL1);
    }

    char *name      = argv[0];
    char *topic     = (argc>1)?argv[1]:NULL;
    char *channel   = (argc>2)?argv[2]:NULL;

    if (os_str_is_wildcard(name, os_iswildcard)) {
        name = NULL;
    }
    
    if (os_str_is_wildcard(topic, os_iswildcard)) {
        topic = NULL;
    }
    
    if (os_str_is_wildcard(channel, os_iswildcard)) {
        channel = NULL;
    }

    if (false==allow_all_empty && NULL==name && NULL==topic && NULL==channel) {
        return nsqc_usage(-EINVAL2);
    }
    else if (os_strlen(name) > NSQ_NAMESIZE) {
        return nsqc_usage(-ETOOBIG);
    }
    else if (os_strlen(topic) > NSQ_NAMESIZE) {
        return nsqc_usage(-ETOOBIG);
    }
    else if (os_strlen(channel) > NSQ_NAMESIZE) {
        return nsqc_usage(-ETOOBIG);
    }
    else {
        return nsqc_handle(action, argc, argv);
    }
}

STATIC int
nsqc_cmd_insert(int argc, char *argv[])
{
    char *json = argv[0];
    
    if (1!=argc) {
        return nsqc_usage(-EINVAL0);
    }
    else if (false==is_good_json(json)) {
        return nsqc_usage(-EBADJSON);
    }
    
    return nsqc_handle("insert", argc, argv);
}

STATIC int
nsqc_cmd_remove(int argc, char *argv[])
{
    return nsqc_handle_name_topic_channel("remove", argc, argv, false);
}

STATIC int
nsqc_cmd_show(int argc, char *argv[])
{
    return nsqc_handle_name_topic_channel("remove", argc, argv, true);
}

STATIC cli_table_t nsqc_table[] = {
    CLI_ENTRY("insert", nsqc_cmd_insert),
    CLI_ENTRY("remove", nsqc_cmd_remove),
    CLI_ENTRY("show",   nsqc_cmd_show),
};

STATIC int
nsqc_command(int argc, char *argv[])
{
    int err;

    err = cli_argv_handle(nsqc_table, os_count_of(nsqc_table), argc, argv);
    if (err<0) {
        debug_error("%s error:%d", argv[0], err);

        return err;
    }

    return 0;
}

STATIC int
nsqc_main_helper(int argc, char *argv[])
{
    int err;
    
    if (1==argc) {
        return nsqc_usage(-EHELP);
    }
    
    err = nsqc_command(argc-1, argv+1);
    if (err<0) {
        /* just log, NOT return */
    }

    return err;
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(nsqc_main_helper, argc, argv);
}
/******************************************************************************/
