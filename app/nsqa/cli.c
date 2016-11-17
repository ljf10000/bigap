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
static int
handle_name_topic_channel(int(*handle)(char *name, char *topic, char *channel), char *args)
{
    char *name      = args;             cli_shift(args);
    char *topic     = name?args:NULL;   cli_shift(args);
    char *channel   = topic?args:NULL;

    if (os_str_is_wildcard(name, __iswildcard)) {
        name = NULL;
    }
    
    if (os_str_is_wildcard(topic, __iswildcard)) {
        topic = NULL;
    }
    
    if (os_str_is_wildcard(channel, __iswildcard)) {
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
static int
handle_remove(char *args)
{
    return handle_name_topic_channel(nsqi_remove, args);
}

/*
* insert {json}
*/
static int
handle_insert(char *args)
{
    char *json  = args;
    
    return nsqi_insert(json);
}

static mv_t
show_user(nsq_instance_t *instance)
{
    return mv2_ok;
}

static int
show_stat(void)
{
    return 0;
}

static int
show_count(void)
{
    cli_sprintf("%d" __crlf, h1_count(&nsqa.table));
    
    return 0;
}

/*
* show [json]
*/
static int
handle_show(char *args)
{
    return handle_name_topic_channel(nsqi_show, args);
}

static cli_table_t cli_table[] = {
    CLI_ENTRY("insert", handle_insert),
    CLI_ENTRY("remove", handle_remove),
    CLI_ENTRY("show",   handle_show),
};

static int
nsq_cli(loop_watcher_t *watcher, time_t now)
{
    int err;
    
    err = clis_handle(watcher->fd, cli_table);
    
    os_loop_del_watcher(&nsqa.loop, watcher->fd);

    return err;
}

int
init_nsq_cli(void)
{
    int err;

    err = os_loop_add_CLI(&nsqa.loop, "nsqa", nsq_cli);
    if (err<0) {
        return err;
    }
    
    return 0;
}
/******************************************************************************/
