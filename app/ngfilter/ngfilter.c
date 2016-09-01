/*******************************************************************************
Copyright (c) 2015-2016, xxx Networks. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      nglog
#endif

#include "utils.h"

OS_INITER;

enum {
    NGL_T_STRING,
    NGL_T_INT,
    NGL_T_DOUBLE,
    NGL_T_TIME,

    NGL_T_END
};

#define __NGL_A_LIST(_)   \
    _(NGL_A_remote_addr,    0, "$remote_addr"), \
    _(NGL_A_remote_user,    1, "$remote_user"), \
    _(NGL_A_time_local,     2, "$time_local"), \
    _(NGL_A_request,        3, "$request"), \
    _(NGL_A_http_host,      4, "$http_host"), \
    _(NGL_A_status,         5, "$status"), \
    _(NGL_A_upstream_status,        6, "$upstream_status"), \
    _(NGL_A_body_bytes_sent,        7, "$body_bytes_sent"), \
    _(NGL_A_http_referer,           8, "$http_referer"), \
    _(NGL_A_http_user_agent,        9, "$http_user_agent"), \
    _(NGL_A_ssl_protocol,           10, "$ssl_protocol"), \
    _(NGL_A_ssl_cipher,             11, "$ssl_cipher"), \
    _(NGL_A_upstream_addr,          12, "$upstream_addr"), \
    _(NGL_A_request_time,           13, "$request_time"), \
    _(NGL_A_upstream_response_time, 14, "$upstream_response_time"), \
    _(NGL_A_http_x_forwarded_for,   15, "$http_x_forwarded_for"), \
    /* end */

DECLARE_ENUM(ngla, __NGL_A_LIST, NGL_A_END);
// is_good_ngla
// ngla_string
// ngla_idx

static char ngl_format_deft[] = 
    "$remote_addr"          __blank
    "-"                     __blank
    "$remote_user"          __blank
    "[$time_local]"         __blank
    "\"$request\""          __blank
    "$status"               __blank
    "$body_bytes_sent"      __blank
    "\"$http_referer\""     __blank
    "\"$http_user_agent\""  __blank
    "\"$http_x_forwarded_for\"";

struct ngl_fmt_opt {
    char *name;
    int  type;
    int  border[2];
    bool ignore;
    
    union {
        int vi;
        double vd;
        char *vs;
    } v;
    
    union {
        void *pointer;
        uint32_t (*geti)(char *value);
        double (*getd)(char *value);
        char *(*gets)(char *value);
    } cb;
};

#define NGL_FMT_OPT(_name, _type, _border0, _border1, _cb) { \
    .name = _name,          \
    .type = _type,          \
    .border = {             \
        [0] = _border0,     \
        [1] = _border1,     \
    },                      \
    .cb = {                 \
        .pointer = _cb,     \
    },                      \
}

static int
ngl_fmt_uint(char *fmt)
{
    return atoi(fmt);
}

static int
ngl_fmt_double(char *fmt)
{
    return atof(fmt);
}

static char *
ngl_fmt_string(char *fmt)
{
    return fmt;
}

struct ngl_fmt_opt ngl_opt[NGL_A_END] = {
    [NGL_A_remote_addr] = NGL_FMT_OPT("$remote_addr",   NGL_T_STRING, 0, 0, NULL),
    [NGL_A_remote_user] = NGL_FMT_OPT("$remote_user",   NGL_T_STRING, 0, 0, ngl_fmt_string),
    [NGL_A_time_local]  = NGL_FMT_OPT("$time_local",    NGL_T_TIME, '[', ']', NULL),
    [NGL_A_request]     = NGL_FMT_OPT("$request",       NGL_T_STRING, '"', '"', ngl_fmt_string),
    [NGL_A_http_host]   = NGL_FMT_OPT("$http_host",     NGL_T_STRING, 0, 0, ngl_fmt_string),
    [NGL_A_status]      = NGL_FMT_OPT("$status",        NGL_T_INT, 0, 0, ngl_fmt_uint),
    [NGL_A_upstream_status] = NGL_FMT_OPT("$upstream_status",   NGL_T_INT, 0, 0, ngl_fmt_uint),
    [NGL_A_body_bytes_sent] = NGL_FMT_OPT("$body_bytes_sent",   NGL_T_INT, 0, 0, ngl_fmt_uint),
    [NGL_A_http_referer]    = NGL_FMT_OPT("$http_referer",      NGL_T_STRING, 0, 0, ngl_fmt_string),
    [NGL_A_http_user_agent] = NGL_FMT_OPT("$http_user_agent",   NGL_T_STRING, '"', '"', NULL),
    [NGL_A_ssl_protocol]    = NGL_FMT_OPT("$ssl_protocol",      NGL_T_STRING, 0, 0, ngl_fmt_string),
    [NGL_A_ssl_cipher]      = NGL_FMT_OPT("$ssl_cipher",        NGL_T_STRING, 0, 0, ngl_fmt_string),
    [NGL_A_upstream_addr]   = NGL_FMT_OPT("$upstream_addr",     NGL_T_STRING, 0, 0, NULL),
    [NGL_A_request_time]    = NGL_FMT_OPT("$request_time",      NGL_T_DOUBLE, 0, 0, ngl_fmt_double),
    [NGL_A_upstream_response_time]  = NGL_FMT_OPT("$upstream_response_time",    NGL_T_DOUBLE, 0, 0, NULL),
    [NGL_A_http_x_forwarded_for]    = NGL_FMT_OPT("$http_x_forwarded_for",  NGL_T_STRING, '"', '"', NULL),
};

static struct {
    struct ngl_fmt_opt opt[NGL_A_END];
    int count;
} ngl_config;

static char *ngl_fmt;

#define ngl_fmt_offset(_fmt)    ((_fmt) - ngl_fmt)

static int
ngl_fmt_next(char *fmt)
{
    switch(fmt[0]) {
        case ' ':
        case '\t':
            return ngl_fmt_next(fmt+1);
        case '$':
        case '-':
            return fmt;
        case '"':
            break;
        default:
            return -EFORMAT;
    }
}

static int
ngl_config_init(char *fmt)
{
    
}

static int
__main(int argc, char *argv[])
{
    if (1==argc) {
        ngl_fmt = ngl_format_deft;
    }
    else if (2==argc) {
        ngl_fmt = strdup(argv[1]);

        os_str_rstrim(ngl_fmt, NULL);
    }
    else {
        return -EFORMAT;
    }

    
    FILE *f = os_fopen(file_log, "r");
    if (NULL==f) {
        return -ENOEXIST;
    }

    char *line = (char *)os_malloc(1+OS_BIG_LEN);
    
    while(!os_feof(f)) {
        line[0] = 0;
        os_freadline(f, line, OS_BIG_LEN);
        
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

/*
* cmd have enabled when boot
*/
int allinone_main(int argc, char *argv[])
{
    return os_main(__main, argc, argv);
}
/******************************************************************************/
