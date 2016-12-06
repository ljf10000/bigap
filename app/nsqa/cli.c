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
nsqa_handle_name_topic_channel(int(*handle)(char *name, char *topic, char *channel), char *args)
{
    char *name      = args;             cli_shift(args);
    char *topic     = name?args:NULL;   cli_shift(args);
    char *channel   = topic?args:NULL;

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
nsqa_handle_remove(char *args)
{
    return nsqa_handle_name_topic_channel(nsqi_remove, args);
}

/*
* insert {json}
*/
STATIC int
nsqa_handle_insert(char *args)
{
    char *json  = args;
    
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
nsqa_handle_show(char *args)
{
    return nsqa_handle_name_topic_channel(nsqi_show, args);
}

STATIC int
nsqa_cli(loop_watcher_t *watcher, time_t now)
{
    static cli_table_t table[] = {
        CLI_ENTRY("insert", nsqa_handle_insert),
        CLI_ENTRY("remove", nsqa_handle_remove),
        CLI_ENTRY("show",   nsqa_handle_show),
    };
    int err;

    err = clis_handle(watcher->fd, table);
    
    os_loop_del_watcher(&nsqa.loop, watcher->fd);

    return err;
}

int
init_nsqa_cli(void)
{
    int err;

    err = os_loop_add_CLI(&nsqa.loop, "nsqa", nsqa_cli);
    if (err<0) {
        return err;
    }
    
    return 0;
}
/******************************************************************************/
