/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      nsqa
#endif
    
#ifndef __THIS_FILE
#define __THIS_FILE     cli
#endif
    
#define __DEAMON__
#define __THIS_USAGE \
    "nsqc usage:"                                       __crlf \
    __tab "nsqc insert {json}"                          __crlf \
    __tab "nsqc remove {name|*} {topic|*} {channel|*}"  __crlf \
    __tab "nsqc show [{name|*} {topic|*} {channel|*}]"  __crlf \
    /* end */
#include "nsqa.h"
/******************************************************************************/
/*
* handle {mac} {json}
*/
STATIC int
nsqa_handle_name_topic_channel(
    int(*handle)(char *name, char *topic, char *channel), 
    int argc, 
    char *argv[],
    bool allow_all_empty
)
{
    if (argc < 2) {
        return cli_help(-EINVAL0);
    }
    else if (argc > 4) {
        return cli_help(-EINVAL1);
    }

    char *name      = argv[1];
    char *topic     = (argc>2)?argv[2]:NULL;
    char *channel   = (argc>3)?argv[3]:NULL;

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
        return cli_help(-EINVAL2);
    }
    else if (os_strlen(name) > NSQ_NAMESIZE) {
        return cli_help(-ETOOBIG);
    }
    else if (os_strlen(topic) > NSQ_NAMESIZE) {
        return cli_help(-ETOOBIG);
    }
    else if (os_strlen(channel) > NSQ_NAMESIZE) {
        return cli_help(-ETOOBIG);
    }

    return (*handle)(name, topic, channel);
}

/*
* remove {name}
*/
STATIC int
nsqa_handle_remove(cli_table_t *table, int argc, char *argv[])
{
    return nsqa_handle_name_topic_channel(nsqi_remove, argc, argv, false);
}

/*
* insert {json}
*/
STATIC int
nsqa_handle_insert(cli_table_t *table, int argc, char *argv[])
{
    char *json = argv[1];
    
    if (2!=argc) {
        return cli_help(-EINVAL0);
    }
    else if (false==is_good_json(json)) {
        return cli_help(-EBADJSON);
    }
    
    return nsqi_insert_byjson(json);
}

STATIC mv_t
nsqa_show_user(nsq_instance_t *instance)
{
    return mv2_ok;
}

STATIC int
nsqa_show_stat(void)
{
    return 0;
}

STATIC int
nsqa_show_count(void)
{
    cli_sprintf("%d" __crlf, h1_count(&nsqa.table));
    
    return 0;
}

/*
* show [json]
*/
STATIC int
nsqa_handle_show(cli_table_t *table, int argc, char *argv[])
{
    return nsqa_handle_name_topic_channel(nsqi_show, argc, argv, true);
}

STATIC int
nsqa_cli(loop_watcher_t *watcher, time_t now)
{
    static cli_table_t tables[] = {
        /*
        * help must first
        */
        CLI_TCP_ENTRY("help",   cli_handle_help),
        
        CLI_TCP_ENTRY("insert", nsqa_handle_insert),
        CLI_TCP_ENTRY("remove", nsqa_handle_remove),
        CLI_TCP_ENTRY("show",   nsqa_handle_show),
    };

    return cli_response(watcher->fd, tables);
}

int
init_nsqa_cli(void)
{
    int err;

    err = os_loop_cli_tcp(&nsqa.loop, nsqa_cli, NULL);
    if (err<0) {
        return err;
    }
    
    return 0;
}
/******************************************************************************/
