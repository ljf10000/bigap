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
#include "nsqa.h"
/******************************************************************************/
/*
* handle {mac} {json}
*/
STATIC int
nsqa_handle_name_topic_channel(
    int(*handle)(char *name, char *topic, char *channel), 
    int argc, 
    char *argv[]
)
{
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
    
    if (NULL==name && NULL==topic && NULL==channel) {
        return -EFORMAT;
    }

    return (*handle)(name, topic, channel);
}

/*
* remove {name}
*/
STATIC int
nsqa_handle_remove(cli_table_t *table, int argc, char *argv[])
{
    return nsqa_handle_name_topic_channel(nsqi_remove, argc, argv);
}

/*
* insert {json}
*/
STATIC int
nsqa_handle_insert(cli_table_t *table, int argc, char *argv[])
{
    char *json  = argv[1];
    
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
    return nsqa_handle_name_topic_channel(nsqi_show, argc, argv);
}

STATIC int
nsqa_cli(loop_watcher_t *watcher, time_t now)
{
    static cli_table_t tables[] = {
        CLI_TCP_ENTRY("insert", nsqa_handle_insert),
        CLI_TCP_ENTRY("remove", nsqa_handle_remove),
        CLI_TCP_ENTRY("show",   nsqa_handle_show),
    };

    return clis_handle(watcher->fd, tables);
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
