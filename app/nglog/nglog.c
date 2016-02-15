/*******************************************************************************
Copyright (c) 2012-2015, xxx Networks. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      nglog
#endif

#include "utils.h"
#include "http/user_agent.h"
#include "http/uv.h"
#include "http/pv.h"

OS_INITER;

static int
usage(char *self)
{
#if USE_USAGE
    os_fprintln(stderr, "%s nginx-log-file pv-file uv-file", self);
#endif

    return -EFORMAT;
}

#ifndef MAX_UV
#define MAX_UV  (1024*1024)
#endif

struct uv_item {
    byte mac[6];
};

static struct {
    struct uv_item db[MAX_UV];
    int count;
} uv[UA_END];

static inline struct uv_item *
uv_find(int os, byte mac[])
{
    int i;

    for (i=0; i<uv[os].count; i++) {
        struct uv_item *item = &uv[os].db[i];
        if (os_maceq(item->mac, mac)) {
            return item;
        }
    }

    return NULL;
}

static inline void
uv_insert(int os, byte mac[])
{
    os_maccpy(uv[os].db[uv[os].count].mac, mac);
    
    if (uv[os].count<MAX_UV) {
        uv[os].count++;
    }
}

static inline void
uv_update(int os, char *macstring)
{
    byte *mac = os_mac(macstring);
    
    if (NULL==uv_find(os, mac)) {
        uv_insert(os, mac);
    }
}

static inline void
uv_sync(char *file)
{
    FILE *f = os_fopen(file, "w");
    int i, os;

    for (os=0; os<UA_END; os++) {
        for (i=0; i<uv[os].count; i++) {
            os_fprintln(f, "%s %s", user_agent_string(os), os_macstring(uv[os].db[i].mac));
        }
    }

    os_fclose(f);
}

#ifndef MAX_PV
#define MAX_PV  (1024*1024)
#endif

struct pv_item {
    char *url;
    int count;
};

static struct {
    struct pv_item db[MAX_PV];
    int count;
} pv[UA_END];

static inline struct pv_item *
pv_find(int os, char *url)
{
    int i;

    for (i=0; i<pv[os].count; i++) {
        struct pv_item *item = &pv[os].db[i];
        
        if (0==os_strcmp(url, item->url)) {
            return item;
        }
    }

    return NULL;
}

static inline void
pv_insert(int os, char *url)
{
    pv[os].db[pv[os].count].url = strdup(url);
    pv[os].db[pv[os].count].count = 0;
    if (pv[os].count<MAX_PV) {
        pv[os].count++;
    }
}

static inline void
pv_update(int os, char *url)
{
    struct pv_item *item = pv_find(os, url);
    if (NULL==item) {
        pv_insert(os, url);
    } else {
        item->count++;
    }
}

static inline void
pv_sync(char *file)
{
    FILE *f = os_fopen(file, "w");
    int i, os;

    for (os=0; os<UA_END; os++) {
        for (i=0; i<pv[os].count; i++) {
            os_fprintln(f, "%s %d %s", 
                user_agent_string(os), 
                pv[os].db[i].count, 
                pv[os].db[i].url);
        }
    }

    os_fclose(f);
}

static char *
get_user(char *mac)
{
    mac = strchr(mac, '+');
    if (NULL==mac) {
        return NULL;
    }

    return mac+1;
}

static char *
get_url(char *url)
{
    url = strchr(url, ' ');
    if (NULL==url) {
        return NULL;
    }
    url++;
    
    char *p = strchr(url, ' ');
    if (NULL==p) {
        return NULL;
    }
    *p = 0;

    p = strchr(url, '?');
    if (p) {
        *p = 0;
    }

    return url;
}

/*
* 1: nginx-log-file
* 2: pv-file
* 3: uv-file
*/
static int
__main(int argc, char *argv[])
{
    char *file_log  = argv[1];
    char *file_pv   = argv[2];
    char *file_uv   = argv[3];
    
    if (4!=argc) {
        return usage(argv[0]);
    }

    FILE *f = os_fopen(file_log, "r");
    if (NULL==f) {
        return -ENOEXIST;
    }

    char *line = (char *)os_malloc(1+OS_HUGE_LEN);
    
    while(!os_feof(f)) {
        line[0] = 0;
        os_freadline(f, line, OS_HUGE_LEN);
        
        if (__is_blank_line(line)) {
            continue;
        }
        
        debug_trace("line=%s", line);
        char *mac = line, *url, *agent, *p;

//mac:192.168.0.64+a4:eb:d3:cd:6e:2d - - [07/Jan/2016:11:09:13 +0800] "GET /?width=20&height=20 HTTP/1.1" 200 2201 "-" "Dalvik/1.6.0 (Linux; U; Android 4.1.2; SCH-I759 Build/JZO54K)" -
//==>
//mac:192.168.0.64+a4:eb:d3:cd:6e:2d
//p:- - [07/Jan/2016:11:09:13 +0800] "GET /?width=20&height=20 HTTP/1.1" 200 2201 "-" "Dalvik/1.6.0 (Linux; U; Android 4.1.2; SCH-I759 Build/JZO54K)" -
        p = strchr(mac, ' ');
        if (NULL==p) {
            continue;
        }
        *p++ = 0;
        
        mac = get_user(mac);
        if (NULL==mac) {
            continue;
        }
        debug_trace("mac=%s", mac);
        
//p:- - [07/Jan/2016:11:09:13 +0800] "GET /?width=20&height=20 HTTP/1.1" 200 2201 "-" "Dalvik/1.6.0 (Linux; U; Android 4.1.2; SCH-I759 Build/JZO54K)" -
//==>
//url:GET /?width=20&height=20 HTTP/1.1
//p: 200 2201 "-" "Dalvik/1.6.0 (Linux; U; Android 4.1.2; SCH-I759 Build/JZO54K)" -
        url = strchr(p, '"');
        if (NULL==url) {
            continue;
        }
        url++;
        
        p = strchr(url, '"');
        if (NULL==p) {
            continue;
        }
        *p++ = 0;
        
        url = get_url(url);
        if (NULL==url) {
            continue;
        }
        debug_trace("url=%s", url);
        
//p: 200 2201 "-" "Dalvik/1.6.0 (Linux; U; Android 4.1.2; SCH-I759 Build/JZO54K)" -
//==>
//agent:Dalvik/1.6.0 (Linux; U; Android 4.1.2; SCH-I759 Build/JZO54K)" -
//p: -
        p = strchr(p, '"');
        if (NULL==p) {
            continue;
        }
        p++;
        
        p = strchr(p, '"');
        if (NULL==p) {
            continue;
        }
        p++;
        
        p = strchr(p, '"');
        if (NULL==p) {
            continue;
        }
        p++;
        
        agent = p;
        p = strchr(agent, '"');
        if (NULL==p) {
            continue;
        }
        *p++ = 0;
        
        int os = user_agent_get(agent);
        debug_trace("os=%s", user_agent_string(os));

        uv_update(os, mac);
        pv_update(os, url);
    }

    uv_sync(file_uv);
    pv_sync(file_pv);

    return 0;
}

#ifndef __BUSYBOX__
#define nglog_main  main
#endif

/*
* cmd have enabled when boot
*/
int nglog_main(int argc, char *argv[])
{
    return os_main(__main, argc, argv);
}
/******************************************************************************/
