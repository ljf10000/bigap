/*******************************************************************************
Copyright (c) 2015-2016, xxx Networks. All rights reserved.
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include "http/user_agent.h"

typedef struct {
    char *ip;
    char *mac;
    char *user;
    char *time;
    char *zone;
    char *method;
    char *url;
    char *version;
    char *status;
    char *body_bytes_sent;
    char *referer;
    char *agent;
    char *xforwarded;
} nglog_t;

#if 0
#define nglog_println(_fmt, _args...)   printf(_fmt "\n", ##_args)
#else
#define nglog_println(_fmt, _args...)   do{}while(0)
#endif

static char *
cut_url_param(char *url)
{
    char *p = strchr(url, '?');
    if (p) {
        *p = 0;
    }

    return url;
}

static void
nglog_handle(nglog_t *log)
{
    log->url    = cut_url_param(log->url);
    log->referer= cut_url_param(log->referer);
    log->agent  = user_agent_string(user_agent_get(log->agent));
}

static void
nglog_show(nglog_t *log)
{
    printf(
        "%s %s %s "
        "%s %s %s "
        "%s %s %s "
        "%s %s %s "
        "%s\n",
        log->ip, log->mac, log->user, 
        log->time, log->zone, log->method,
        log->url, log->version, log->status,
        log->body_bytes_sent, log->referer, log->agent,
        log->xforwarded);
}

static int
usage(void)
{
    fprintf(stderr, "nglog2 [nginx-log-file]\n");

    return EINVAL;
}

#define good(_p, _end)  (_p && *_p && _p<=_end)
#define next(_p, _ch, _end)     ({  \
    if (!good(_p, _end)) {          \
        continue;                   \
    } else {                        \
        _p = strchr(_p, _ch);       \
        if (!good(_p, _end)) {      \
            continue;               \
        }                           \
        *_p++ = 0;                  \
    }                               \
                                    \
    _p;                             \
})

#define LINE_LEN            (256*1024-1)  
/*
* 1: nginx-log-file
* 2: pv-file
* 3: uv-file
*/
int
main(int argc, char *argv[])
{
    char *file = NULL, *p;
    FILE *f = NULL;
    nglog_t log = {NULL};
    char *line, *end;
    int len;
    
    if (2==argc) {
        if (0==strcmp("--help", argv[1])) {
            return usage();
        }

        file = argv[1];
    }
    else if (1!=argc) {
        return usage();
    }

    if (file) {
        f = fopen(file, "r");
        if (NULL==f) {
            return ENOENT;
        }
    } else {
        f = stdin;
    }

    line = (char *)malloc(1+LINE_LEN);
    
    while(!feof(f)) {
        line[0] = 0;
        fgets(line, LINE_LEN, f);

        if (0==line[0] || '\n'==line[0] || ('\r'==line[0] && '\n'==line[1])) {
            continue;
        }
        
        len = strlen(line);
        end = line + len - 1;
        while(end>line && ('\n'== *end || '\r'== *end || ' '== *end || '\t'== *end)) {
            *end-- = 0;
        }

        nglog_println("begin=%p, end=%p", line, end);
        
        memset(&log, 0, sizeof(log));
        nglog_println("line=%s", line);
        
        log.ip = p = line;
        next(p, '+', end);
        nglog_println("p=%p, ip=%s", p, log.ip);

        log.mac = p;
        next(p, ' ', end);
        nglog_println("p=%p, mac=%s", p, log.mac);

        /*
        * skip '-'
        */
        next(p, '-', end);
        next(p, ' ', end);

        log.user = p;
        next(p, ' ', end);
        nglog_println("p=%p, user=%s", p, log.user);

        next(p, '[', end);
        log.time = p;
        next(p, ' ', end);
        next(p, '+', end);
        nglog_println("p=%p, time=%s", p, log.time);
        log.zone = p;
        next(p, ']', end);
        next(p, ' ', end);
        nglog_println("p=%p, zone=%s", p, log.zone);
        
        next(p, '"', end);
        log.method = p;
        next(p, ' ', end);
        nglog_println("p=%p, method=%s", p, log.method);
        log.url = p;
        next(p, ' ', end);
        nglog_println("p=%p, url=%s", p, log.url);
        log.version = p;
        next(p, '"', end);
        next(p, ' ', end);
        nglog_println("p=%p, version=%s", p, log.version);

        log.status = p;
        next(p, ' ', end);
        nglog_println("p=%p, status=%s", p, log.status);

        log.body_bytes_sent = p;
        next(p, ' ', end);
        nglog_println("p=%p, body_bytes_sent=%s", p, log.body_bytes_sent);
        
        next(p, '"', end);
        log.referer= p;
        next(p, '"', end);
        next(p, ' ', end);
        nglog_println("p=%p, referer=%s", p, log.referer);
        
        next(p, '"', end);
        log.agent= p;
        next(p, '"', end);
        next(p, ' ', end);
        nglog_println("p=%p, agent=%s", p, log.agent);
        
        log.xforwarded= p;
        nglog_println("p=%p, xforwarded=%s", p, log.xforwarded);

        nglog_handle(&log);
        nglog_show(&log);
    }

    if (file) {
        fclose(f);
    }
    if (line) {
        free(line);
    }
    
    return 0;
}
/******************************************************************************/
