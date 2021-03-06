/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      libjs
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     libc
#endif

#ifndef __BUSYBOX__
#define __LIB__
#endif

#define __DEAMON__

#include "utils.h"
#include "js.h"
/******************************************************************************/
int
libc_get_sockaddr(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    os_sockaddr_t *p = (os_sockaddr_t *)obj; os_objzero(p);
    
    p->c.sa_family = js_get_obj_int(ctx, idx, "family");
    
    switch(p->c.sa_family) {
        case AF_INET:
            p->in.sin_addr.s_addr = js_get_obj_uint(ctx, idx, "ip");
            p->in.sin_port = js_get_obj_int(ctx, idx, "port");

            break;
        case AF_UNIX: {
            bool abstract = js_get_obj_bool(ctx, idx, "abstract");
            
            js_copy_obj_string(ctx, idx, "path",
                p->un.sun_path + !!abstract,
                sizeof(p->un.sun_path) - !!abstract);
        }   break;
        case AF_PACKET:
            p->ll.sll_protocol  = js_get_obj_int(ctx, idx, "protocol");
            p->ll.sll_ifindex   = js_get_obj_int(ctx, idx, "ifindex");
            p->ll.sll_hatype    = js_get_obj_int(ctx, idx, "hatype");
            p->ll.sll_pkttype   = js_get_obj_int(ctx, idx, "pkttype");
            p->ll.sll_halen     = js_get_obj_int(ctx, idx, "halen");

            js_copy_obj_buffer(ctx, idx, "addr", (char *)p->ll.sll_addr, 8);

            break;
        case AF_NETLINK:
            p->nl.nl_pid = js_get_obj_int(ctx, idx, "pid");
            p->nl.nl_groups = js_get_obj_int(ctx, idx, "groups");

            break;
        default:
            return -ENOSUPPORT;
    }

    return 0;
}

int
libc_set_sockaddr(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    os_sockaddr_t *p = (os_sockaddr_t *)obj;
    
    js_set_obj_int(ctx, idx, "family", p->c.sa_family);
    
    switch(p->c.sa_family) {
        case AF_INET:
            js_set_obj_uint(ctx, idx, "ip", p->in.sin_addr.s_addr);
            js_set_obj_uint(ctx, idx, "port", p->in.sin_port);
            
            break;
        case AF_UNIX: {
            bool abstract = is_abstract_sockaddr(&p->un);

            js_set_obj_bool(ctx, idx, "abstract", abstract);
            js_set_obj_string(ctx, idx, "path", p->un.sun_path + !!abstract);
            
        }   break;
        case AF_PACKET:
            js_set_obj_int(ctx, idx, "protocol", p->ll.sll_protocol);
            js_set_obj_uint(ctx, idx, "ifindex", p->ll.sll_ifindex);
            js_set_obj_int(ctx, idx, "hatype", p->ll.sll_hatype);
            js_set_obj_int(ctx, idx, "pkttype", p->ll.sll_pkttype);
            js_set_obj_int(ctx, idx, "halen", p->ll.sll_halen);
            js_set_obj_buffer(ctx, idx, "addr", p->ll.sll_addr, 8);
            
            break;
        case AF_NETLINK:
            js_set_obj_int(ctx, idx, "pid", p->nl.nl_pid);
            js_set_obj_uint(ctx, idx, "groups", p->nl.nl_groups);
            
            break;
        default:
            return -ENOSUPPORT;
    }

    return 0;
}

int
libc_get_linger(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct linger *p = (struct linger *)obj; os_objzero(p);
    
    p->l_onoff   = js_get_obj_bool(ctx, idx, "onoff");
    p->l_linger  = js_get_obj_uint(ctx, idx, "linger");

    return 0;
}

int
libc_set_linger(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct linger *p = (struct linger *)obj;
    
    js_set_obj_bool(ctx, idx, "onoff", p->l_onoff);
    js_set_obj_uint(ctx, idx, "linger", p->l_linger);

    return 0;
}

int
libc_get_dirent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct dirent *p = (struct dirent *)obj; os_objzero(p);
    
    p->d_type   = js_get_obj_int(ctx, idx, "type");
    p->d_ino    = js_get_obj_int(ctx, idx, "ino");
    p->d_off    = js_get_obj_uint(ctx, idx, "off");
    js_copy_obj_string(ctx, idx, "name", p->d_name, sizeof(p->d_name));

    return 0;
}

int
libc_set_dirent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct dirent *p = (struct dirent *)obj;
    
    js_set_obj_int(ctx, idx, "type", p->d_type);
    js_set_obj_int(ctx, idx, "ino", p->d_ino);
    js_set_obj_uint(ctx, idx, "off", p->d_off);
    js_set_obj_string(ctx, idx, "name", p->d_name);

    return 0;
}

int
libc_get_stat(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct stat *p = (struct stat *)obj; os_objzero(p);
    
    p->st_dev   = js_get_obj_uint(ctx, idx, "dev");
    p->st_rdev  = js_get_obj_uint(ctx, idx, "rdev");
    p->st_ino   = js_get_obj_uint(ctx, idx, "ino");
    p->st_mode  = js_get_obj_uint(ctx, idx, "mode");
    p->st_nlink = js_get_obj_uint(ctx, idx, "nlink");
    p->st_uid   = js_get_obj_uint(ctx, idx, "uid");
    p->st_gid   = js_get_obj_uint(ctx, idx, "gid");
    p->st_atime = js_get_obj_uint(ctx, idx, "atime");
    p->st_mtime = js_get_obj_uint(ctx, idx, "mtime");
    p->st_ctime = js_get_obj_uint(ctx, idx, "ctime");
    p->st_size  = js_get_obj_int(ctx, idx, "size");
    p->st_blksize = js_get_obj_int(ctx, idx, "blksize");
    p->st_blocks= js_get_obj_int(ctx, idx, "blocks");

    return 0;
}

int
libc_set_stat(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct stat *p = (struct stat *)obj;
    
    js_set_obj_uint(ctx, idx, "dev", p->st_dev);
    js_set_obj_uint(ctx, idx, "rdev", p->st_rdev);
    js_set_obj_uint(ctx, idx, "ino", p->st_ino);
    js_set_obj_uint(ctx, idx, "mode", p->st_mode);
    js_set_obj_uint(ctx, idx, "nlink", p->st_nlink);
    js_set_obj_uint(ctx, idx, "uid", p->st_uid);
    js_set_obj_uint(ctx, idx, "gid", p->st_gid);
    js_set_obj_uint(ctx, idx, "atime", p->st_atime);
    js_set_obj_uint(ctx, idx, "mtime", p->st_mtime);
    js_set_obj_uint(ctx, idx, "ctime", p->st_ctime);
    js_set_obj_int(ctx, idx, "size", p->st_size);
    js_set_obj_int(ctx, idx, "blksize", p->st_blksize);
    js_set_obj_int(ctx, idx, "blocks", p->st_blocks);

    return 0;
}

int
libc_get_if_nameindex(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct if_nameindex *p = (struct if_nameindex *)obj; os_objzero(p);
    
    p->if_index = js_get_obj_uint(ctx, idx, "index");
    p->if_name  = js_get_obj_string(ctx, idx, "name", NULL);

    return 0;
}

int
libc_set_if_nameindex(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct if_nameindex *p = (struct if_nameindex *)obj;
    
    js_set_obj_uint(ctx, idx, "index", p->if_index);
    js_set_obj_string(ctx, idx, "name", p->if_name);

    return 0;
}

int
libc_set_hostent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct hostent *p = (struct hostent *)obj;
    int i;
    
    js_set_obj_string(ctx, idx, "name", p->h_name);
    js_set_obj_int(ctx, idx, "addrtype", p->h_addrtype);
    js_set_obj_int(ctx, idx, "length", p->h_length);
    js_set_obj_uint(ctx, idx, "addr", *(uint32 *)(p->h_addr));
    
    duk_push_object(ctx);
    for (i=0; p->h_aliases[i]; i++) {
        js_set_array_string(ctx, -1, i, p->h_aliases[i]);
    }
    duk_put_prop_string(ctx, idx, "aliases");
    
    duk_push_object(ctx);
    for (i=0; p->h_addr_list[0]; i++) {
        js_set_array_uint(ctx, -1, i, *(uint32 *)p->h_addr_list[i]);
    }
    duk_put_prop_string(ctx, idx, "addr_list");

    return 0;
}

int
libc_set_servent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct servent *p = (struct servent *)obj;
    char *a;
    int i;
    
    js_set_obj_string(ctx, idx, "name", p->s_name);
    js_set_obj_int(ctx, idx, "port", p->s_port);
    js_set_obj_string(ctx, idx, "proto", p->s_proto);
    
    duk_push_object(ctx);
    for (i=0, a=p->s_aliases[0]; a; i++, a++) {
        js_set_array_string(ctx, -1, i, a);
    }
    duk_put_prop_string(ctx, idx, "aliases");

    return 0;
}

int
libc_set_protoent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct protoent *p = (struct protoent *)obj;
    char *a;
    int i;
    
    js_set_obj_string(ctx, idx, "name", p->p_name);
    js_set_obj_int(ctx, idx, "proto", p->p_proto);
    
    duk_push_object(ctx);
    for (i=0, a=p->p_aliases[0];a; i++, a++) {
        js_set_array_string(ctx, -1, i, a);
    }
    duk_put_prop_string(ctx, idx, "aliases");

    return 0;
}

int
libc_set_netent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct netent *p = (struct netent *)obj;
    int i;
        
    js_set_obj_string(ctx, idx, "n_name", p->n_name);
    js_set_obj_int(ctx, idx, "n_addrtype", p->n_addrtype);
    js_set_obj_int(ctx, idx, "n_net", p->n_net);

    duk_push_object(ctx);
    for (i=0; p->n_aliases[i]; i++) {
        js_set_array_string(ctx, -1, i, p->n_aliases[i]);
    }
    duk_put_prop_string(ctx, idx, "n_aliases");

    return 0;
}

int
libc_get_utimbuf(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct utimbuf *p = (struct utimbuf *)obj; os_objzero(p);
    
    p->actime   = js_get_obj_uint(ctx, idx, "actime");
    p->modtime  = js_get_obj_uint(ctx, idx, "modtime");

    return 0;
}

int
libc_set_utimbuf(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct utimbuf *p = (struct utimbuf *)obj;
    
    js_set_obj_uint(ctx, idx, "actime", p->actime);
    js_set_obj_uint(ctx, idx, "modtime", p->modtime);

    return 0;
}

int
libc_get_timeval(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timeval *p = (struct timeval *)obj; os_objzero(p);
    
    p->tv_sec   = js_get_obj_uint(ctx, idx, "sec");
    p->tv_usec  = js_get_obj_uint(ctx, idx, "usec");

    return 0;
}

int
libc_set_timeval(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timeval *p = (struct timeval *)obj;
    
    js_set_obj_uint(ctx, idx, "sec", p->tv_sec);
    js_set_obj_uint(ctx, idx, "usec", p->tv_usec);

    return 0;
}

int
libc_get_itimerval(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct itimerval *p = (struct itimerval *)obj; os_objzero(p);

    js_obj_get(ctx, idx, libc_get_timeval, &p->it_interval, "interval");
    js_obj_get(ctx, idx, libc_get_timeval, &p->it_value, "value");

    return 0;
}

int
libc_set_itimerval(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct itimerval *p = (struct itimerval *)obj;
    
    js_obj_set(ctx, idx, libc_set_timeval, &p->it_interval, "interval");
    js_obj_set(ctx, idx, libc_set_timeval, &p->it_value, "value");

    return 0;
}

int
libc_get_timespec(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timespec *p = (struct timespec *)obj; os_objzero(p);
    
    p->tv_sec   = js_get_obj_uint(ctx, idx, "sec");
    p->tv_nsec  = js_get_obj_uint(ctx, idx, "nsec");

    return 0;
}

int
libc_set_timespec(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timespec *p = (struct timespec *)obj;
    
    js_set_obj_uint(ctx, idx, "sec", p->tv_sec);
    js_set_obj_uint(ctx, idx, "nsec", p->tv_nsec);

    return 0;
}

int
libc_get_itimerspec(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct itimerspec *p = (struct itimerspec *)obj; os_objzero(p);

    js_obj_get(ctx, idx, libc_get_timespec, &p->it_interval, "interval");
    js_obj_get(ctx, idx, libc_get_timespec, &p->it_value, "value");

    return 0;
}

int
libc_set_itimerspec(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct itimerspec *p = (struct itimerspec *)obj;

    js_obj_set(ctx, idx, libc_set_timespec, &p->it_interval, "interval");
    js_obj_set(ctx, idx, libc_set_timespec, &p->it_value, "value");

    return 0;
}

#if js_LIBC_LINUX
int
libc_get_signalfd_siginfo(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct signalfd_siginfo *p = (struct signalfd_siginfo *)obj; os_objzero(p);
    
    p->ssi_signo    = js_get_obj_uint(ctx, idx, "signo");
    p->ssi_errno    = js_get_obj_int(ctx, idx, "errno");
    p->ssi_code     = js_get_obj_int(ctx, idx, "code");
    p->ssi_pid      = js_get_obj_uint(ctx, idx, "pid");
    p->ssi_uid      = js_get_obj_uint(ctx, idx, "uid");
    p->ssi_fd       = js_get_obj_int(ctx, idx, "fd");
    p->ssi_tid      = js_get_obj_uint(ctx, idx, "tid");
    p->ssi_band     = js_get_obj_uint(ctx, idx, "band");
    p->ssi_overrun  = js_get_obj_uint(ctx, idx, "overrun");
    p->ssi_trapno   = js_get_obj_uint(ctx, idx, "trapno");
    p->ssi_status   = js_get_obj_int(ctx, idx, "status");
    p->ssi_int      = js_get_obj_int(ctx, idx, "int");

    return 0;
}

int
libc_set_signalfd_siginfo(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct signalfd_siginfo *p = (struct signalfd_siginfo *)obj;
    
    js_set_obj_uint(ctx, idx, "signo", p->ssi_signo);
    js_set_obj_int(ctx, idx, "errno", p->ssi_errno);
    js_set_obj_int(ctx, idx, "code", p->ssi_code);
    js_set_obj_uint(ctx, idx, "pid", p->ssi_pid);
    js_set_obj_uint(ctx, idx, "uid", p->ssi_uid);
    js_set_obj_int(ctx, idx, "fd", p->ssi_fd);
    js_set_obj_uint(ctx, idx, "tid", p->ssi_tid);
    js_set_obj_uint(ctx, idx, "band", p->ssi_band);
    js_set_obj_uint(ctx, idx, "overrun", p->ssi_overrun);
    js_set_obj_uint(ctx, idx, "trapno", p->ssi_trapno);
    js_set_obj_int(ctx, idx, "status", p->ssi_status);
    js_set_obj_int(ctx, idx, "int", p->ssi_int);

    return 0;
}

int
libc_get_watcher_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    loop_watcher_t *p = (loop_watcher_t *)obj; os_objzero(p);

    p->fd       = js_get_obj_int(ctx, idx, "fd");
    p->father   = js_get_obj_int(ctx, idx, "father");
    p->type     = js_get_obj_int(ctx, idx, "type");
    p->flag     = js_get_obj_int(ctx, idx, "flag");

    return 0;
}

int
libc_set_watcher_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    loop_watcher_t *p = (loop_watcher_t *)obj; os_objzero(p);

    js_set_obj_int(ctx, idx, "fd",      p->fd);
    js_set_obj_int(ctx, idx, "father",  p->father);
    js_set_obj_int(ctx, idx, "type",    p->type);
    js_set_obj_int(ctx, idx, "flag",    p->flag);

    return 0;
}

int
libc_get_epoll_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct epoll_event *p = (struct epoll_event *)obj; os_objzero(p);
    
    p->events   = js_get_obj_uint(ctx, idx, "events");
    p->data.fd  = js_get_obj_int(ctx, idx, "fd");

    return 0;
}

int
libc_set_epoll_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct epoll_event *p = (struct epoll_event *)obj;
    
    js_set_obj_uint(ctx, idx, "events", p->events);
    js_set_obj_int(ctx, idx, "fd", p->data.fd);

    return 0;
}

int
libc_get_inotify_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct inotify_event *p = (struct inotify_event *)obj; os_objzero(p);
    
    p->wd   = js_get_obj_int(ctx, idx, "wd");
    p->mask = js_get_obj_uint(ctx, idx, "mask");
    p->cookie = js_get_obj_uint(ctx, idx, "cookie");
    js_copy_obj_string(ctx, idx, "name", p->name, 1+NAME_MAX);
    p->len  = os_strlen(p->name);

    return 0;
}

int
libc_set_inotify_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct inotify_event *p = (struct inotify_event *)obj;
    
    js_set_obj_int(ctx, idx, "wd", p->wd);
    js_set_obj_uint(ctx, idx, "mask", p->mask);
    js_set_obj_uint(ctx, idx, "cookie", p->cookie);
    js_set_obj_string(ctx, idx, "name", p->name);

    return 0;
}
#endif

int
libc_get_tms(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct tms *p = (struct tms *)obj; os_objzero(p);
    
    p->tms_utime    = js_get_obj_uint(ctx, idx, "utime");
    p->tms_stime    = js_get_obj_uint(ctx, idx, "stime");
    p->tms_cutime   = js_get_obj_uint(ctx, idx, "cutime");
    p->tms_cstime   = js_get_obj_uint(ctx, idx, "cstime");

    return 0;
}

int
libc_set_tms(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct tms *p = (struct tms *)obj;
    
    js_set_obj_uint(ctx, idx, "utime", p->tms_utime);
    js_set_obj_uint(ctx, idx, "stime", p->tms_stime);
    js_set_obj_uint(ctx, idx, "cutime", p->tms_cutime);
    js_set_obj_uint(ctx, idx, "cstime", p->tms_cstime);

    return 0;
}

int
libc_get_timezone(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timezone *p = (struct timezone *)obj; os_objzero(p);
    
    p->tz_minuteswest   = js_get_obj_uint(ctx, idx, "minuteswest");
    p->tz_dsttime       = js_get_obj_uint(ctx, idx, "dsttime");

    return 0;
}

int
libc_set_timezone(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timezone *p = (struct timezone *)obj;
    
    js_set_obj_uint(ctx, idx, "minuteswest", p->tz_minuteswest);
    js_set_obj_uint(ctx, idx, "dsttime", p->tz_dsttime);

    return 0;
}

int
libc_get_tm(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct tm *p = (struct tm *)obj; os_objzero(p);
    
    p->tm_sec   = js_get_obj_int(ctx, idx, "sec");
    p->tm_min   = js_get_obj_int(ctx, idx, "min");
    p->tm_hour  = js_get_obj_int(ctx, idx, "hour");
    p->tm_mday  = js_get_obj_int(ctx, idx, "mday");
    p->tm_mon   = js_get_obj_int(ctx, idx, "mon");
    p->tm_wday  = js_get_obj_int(ctx, idx, "wday");
    p->tm_yday  = js_get_obj_int(ctx, idx, "yday");
    p->tm_isdst = js_get_obj_int(ctx, idx, "isdst");
    p->tm_gmtoff= js_get_obj_int(ctx, idx, "gmtoff");
    p->tm_zone  = js_get_obj_string(ctx, idx, "zone", NULL);

    return 0;
}

int
libc_set_tm(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct tm *p = (struct tm *)obj;
    
    js_set_obj_int(ctx, idx, "sec", p->tm_sec);
    js_set_obj_int(ctx, idx, "min", p->tm_min);
    js_set_obj_int(ctx, idx, "hour", p->tm_hour);
    js_set_obj_int(ctx, idx, "mday", p->tm_mday);
    js_set_obj_int(ctx, idx, "mon", p->tm_mon);
    js_set_obj_int(ctx, idx, "wday", p->tm_wday);
    js_set_obj_int(ctx, idx, "yday", p->tm_yday);
    js_set_obj_int(ctx, idx, "isdst", p->tm_isdst);
    js_set_obj_int(ctx, idx, "gmtoff", p->tm_gmtoff);
    js_set_obj_string(ctx, idx, "zone", (char *)p->tm_zone);

    return 0;
}

int
libc_get_ntptimeval(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct ntptimeval *p = (struct ntptimeval *)obj; os_objzero(p);
    
    p->maxerror   = js_get_obj_int(ctx, idx, "maxerror");
    p->esterror  = js_get_obj_int(ctx, idx, "esterror");

    os_objzero(&p->time);

    js_obj_get(ctx, idx, libc_get_timeval, &p->time, "time");

    return 0;
}

int
libc_set_ntptimeval(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct ntptimeval *p = (struct ntptimeval *)obj;
    
    js_set_obj_int(ctx, idx, "maxerror", p->maxerror);
    js_set_obj_int(ctx, idx, "esterror", p->esterror);

    js_obj_set(ctx, idx, libc_set_timeval, &p->time, "time");

    return 0;
}

int
libc_get_timex(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timex *p = (struct timex *)obj; os_objzero(p);
    
    p->modes    = js_get_obj_uint(ctx, idx, "modes");
    p->offset   = js_get_obj_int(ctx, idx, "offset");
#if 0
    p->frequency= js_get_obj_int(ctx, idx, "frequency");
#endif
    p->maxerror = js_get_obj_int(ctx, idx, "maxerror");
    p->esterror = js_get_obj_int(ctx, idx, "esterror");
    p->status   = js_get_obj_int(ctx, idx, "status");
    p->constant = js_get_obj_int(ctx, idx, "constant");
    p->precision= js_get_obj_int(ctx, idx, "precision");
    p->tolerance= js_get_obj_int(ctx, idx, "tolerance");
    p->tick     = js_get_obj_int(ctx, idx, "tick");
    p->ppsfreq  = js_get_obj_int(ctx, idx, "ppsfreq");
    p->jitter   = js_get_obj_int(ctx, idx, "jitter");
    p->shift    = js_get_obj_int(ctx, idx, "shift");
    p->stabil   = js_get_obj_int(ctx, idx, "stabil");
    p->jitcnt   = js_get_obj_int(ctx, idx, "jitcnt");
    p->calcnt   = js_get_obj_int(ctx, idx, "calcnt");
    p->errcnt   = js_get_obj_int(ctx, idx, "errcnt");
    p->stbcnt   = js_get_obj_int(ctx, idx, "stbcnt");

    os_objzero(&p->time);

    js_obj_get(ctx, idx, libc_get_timeval, &p->time, "time");

    return 0;
}

int
libc_set_timex(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timex *p = (struct timex *)obj;
    
    js_set_obj_uint(ctx, idx, "modes", p->modes);
    js_set_obj_int(ctx, idx, "offset", p->offset);
#if 0
    js_set_obj_int(ctx, idx, "frequency", p->frequency);
#endif
    js_set_obj_int(ctx, idx, "maxerror", p->maxerror);
    js_set_obj_int(ctx, idx, "esterror", p->esterror);
    js_set_obj_int(ctx, idx, "status", p->status);
    js_set_obj_int(ctx, idx, "constant", p->constant);
    js_set_obj_int(ctx, idx, "precision", p->precision);
    js_set_obj_int(ctx, idx, "tolerance", p->tolerance);
    js_set_obj_int(ctx, idx, "tick", p->tick);
    js_set_obj_int(ctx, idx, "ppsfreq", p->ppsfreq);
    js_set_obj_int(ctx, idx, "jitter", p->jitter);
    js_set_obj_int(ctx, idx, "shift", p->shift);
    js_set_obj_int(ctx, idx, "stabil", p->stabil);
    js_set_obj_int(ctx, idx, "jitcnt", p->jitcnt);
    js_set_obj_int(ctx, idx, "calcnt", p->calcnt);
    js_set_obj_int(ctx, idx, "errcnt", p->errcnt);
    js_set_obj_int(ctx, idx, "stbcnt", p->stbcnt);

    js_obj_set(ctx, idx, libc_set_timeval, &p->time, "time");

    return 0;
}

int
libc_get_rusage(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct rusage *p = (struct rusage *)obj; os_objzero(p);
    
    p->ru_maxrss = js_get_obj_int(ctx, idx, "maxrss");
    p->ru_ixrss  = js_get_obj_int(ctx, idx, "ixrss");
    p->ru_idrss  = js_get_obj_int(ctx, idx, "idrss");
    p->ru_isrss  = js_get_obj_int(ctx, idx, "isrss");
    p->ru_minflt = js_get_obj_int(ctx, idx, "minflt");
    p->ru_majflt = js_get_obj_int(ctx, idx, "majflt");
    p->ru_nswap  = js_get_obj_int(ctx, idx, "nswap");
    p->ru_inblock= js_get_obj_int(ctx, idx, "inblock");
    p->ru_oublock= js_get_obj_int(ctx, idx, "oublock");
    p->ru_msgsnd = js_get_obj_int(ctx, idx, "msgsnd");
    p->ru_msgrcv = js_get_obj_int(ctx, idx, "msgrcv");
    p->ru_nsignals = js_get_obj_int(ctx, idx, "nsignals");
    p->ru_nvcsw  = js_get_obj_int(ctx, idx, "nvcsw");
    p->ru_nivcsw = js_get_obj_int(ctx, idx, "nivcsw");

    os_objzero(&p->ru_utime);
    os_objzero(&p->ru_stime);

    js_obj_get(ctx, idx, libc_get_timeval, &p->ru_utime, "utime");
    js_obj_get(ctx, idx, libc_get_timeval, &p->ru_stime, "stime");

    return 0;
}

int
libc_set_rusage(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct rusage *p = (struct rusage *)obj;
    
    js_set_obj_int(ctx, idx, "maxrss", p->ru_maxrss);
    js_set_obj_int(ctx, idx, "ixrss", p->ru_ixrss);
    js_set_obj_int(ctx, idx, "idrss", p->ru_idrss);
    js_set_obj_int(ctx, idx, "isrss", p->ru_isrss);
    js_set_obj_int(ctx, idx, "minflt", p->ru_minflt);
    js_set_obj_int(ctx, idx, "majflt", p->ru_majflt);
    js_set_obj_int(ctx, idx, "nswap", p->ru_nswap);
    js_set_obj_int(ctx, idx, "inblock", p->ru_inblock);
    js_set_obj_int(ctx, idx, "oublock", p->ru_oublock);
    js_set_obj_int(ctx, idx, "msgsnd", p->ru_msgsnd);
    js_set_obj_int(ctx, idx, "msgrcv", p->ru_msgrcv);
    js_set_obj_int(ctx, idx, "nsignals", p->ru_nsignals);
    js_set_obj_int(ctx, idx, "nvcsw", p->ru_nvcsw);
    js_set_obj_int(ctx, idx, "nivcsw", p->ru_nivcsw);
    
    js_obj_set(ctx, idx, libc_set_timeval, &p->ru_utime, "utime");
    js_obj_set(ctx, idx, libc_set_timeval, &p->ru_stime, "stime");

    return 0;
}

#if js_LIBC_VTIME
int
libc_get_vtimes(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct vtimes *p = (struct vtimes *)obj; os_objzero(p);
    
    p->vm_maxrss= js_get_obj_int(ctx, idx, "maxrss");
    p->vm_ixrss = js_get_obj_int(ctx, idx, "ixrss");
    p->vm_idsrss= js_get_obj_int(ctx, idx, "idsrss");
    p->vm_minflt= js_get_obj_int(ctx, idx, "minflt");
    p->vm_majflt= js_get_obj_int(ctx, idx, "majflt");
    p->vm_nswap = js_get_obj_int(ctx, idx, "nswap");
    p->vm_inblk = js_get_obj_int(ctx, idx, "inblk");
    p->vm_oublk = js_get_obj_int(ctx, idx, "oublk");
    p->vm_oublk = js_get_obj_int(ctx, idx, "oublk");
    p->vm_utime = js_get_obj_int(ctx, idx, "utime");
    p->vm_stime = js_get_obj_int(ctx, idx, "stime");

    return 0;
}

int
libc_set_vtimes(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct vtimes *p = (struct vtimes *)obj;
    
    js_set_obj_int(ctx, idx, "maxrss", p->vm_maxrss);
    js_set_obj_int(ctx, idx, "ixrss", p->vm_ixrss);
    js_set_obj_int(ctx, idx, "idsrss", p->vm_idsrss);
    js_set_obj_int(ctx, idx, "minflt", p->vm_minflt);
    js_set_obj_int(ctx, idx, "majflt", p->vm_majflt);
    js_set_obj_int(ctx, idx, "nswap", p->vm_nswap);
    js_set_obj_int(ctx, idx, "inblock", p->vm_inblk);
    js_set_obj_int(ctx, idx, "oublock", p->vm_oublk);
    js_set_obj_int(ctx, idx, "utime", p->vm_utime);
    js_set_obj_int(ctx, idx, "stime", p->vm_stime);

    return 0;
}
#endif

int
libc_get_flock(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct flock *p = (struct flock *)obj; os_objzero(p);
    
    p->l_type   = js_get_obj_int(ctx, idx, "type");
    p->l_whence = js_get_obj_int(ctx, idx, "whence");
    p->l_start  = js_get_obj_uint(ctx, idx, "start");
    p->l_len    = js_get_obj_uint(ctx, idx, "len");
    p->l_pid    = js_get_obj_uint(ctx, idx, "pid");

    return 0;
}

int
libc_set_flock(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct flock *p = (struct flock *)obj;
    
    js_set_obj_int(ctx, idx, "type", p->l_type);
    js_set_obj_int(ctx, idx, "whence", p->l_whence);
    js_set_obj_uint(ctx, idx, "start", p->l_start);
    js_set_obj_uint(ctx, idx, "len", p->l_len);
    js_set_obj_uint(ctx, idx, "pid", p->l_pid);

    return 0;
}

int
libc_get_termios(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct termios *p = (struct termios *)obj; os_objzero(p);
    int i;
    
    p->c_iflag  = js_get_obj_uint(ctx, idx, "iflag");
    p->c_oflag  = js_get_obj_uint(ctx, idx, "oflag");
    p->c_cflag  = js_get_obj_uint(ctx, idx, "cflag");
    p->c_lflag  = js_get_obj_uint(ctx, idx, "lflag");

    duk_get_prop_string(ctx, idx, "cc");
    if (duk_is_array(ctx, -1)) {
        for (i=0; i<NCCS; i++) {
            p->c_cc[i] = js_get_array_uint(ctx, -1, i);
        }
    }
    duk_pop(ctx);

    return 0;
}

int
libc_set_termios(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct termios *p = (struct termios *)obj;
    int i;
    
    js_set_obj_uint(ctx, idx, "iflag", p->c_iflag);
    js_set_obj_uint(ctx, idx, "oflag", p->c_oflag);
    js_set_obj_uint(ctx, idx, "cflag", p->c_cflag);
    js_set_obj_uint(ctx, idx, "lflag", p->c_lflag);

    duk_get_prop_string(ctx, idx, "cc");
    if (duk_is_array(ctx, -1)) {
        for (i=0; i<NCCS; i++) {
            js_set_array_uint(ctx, -1, i, p->c_cc[i]);
        }
    }
    duk_pop(ctx);

    return 0;
}

int
libc_get_div(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    div_t *p = (div_t *)obj; os_objzero(p);
    
    p->quot = js_get_obj_int(ctx, idx, "quot");
    p->rem  = js_get_obj_int(ctx, idx, "rem");

    return 0;
}

int
libc_set_div(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    div_t *p = (div_t *)obj;
    
    js_set_obj_int(ctx, idx, "quot", p->quot);
    js_set_obj_int(ctx, idx, "rem", p->rem);

    return 0;
}

int
libc_get_rlimit(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct rlimit *p = (struct rlimit *)obj; os_objzero(p);
    
    p->rlim_cur = js_get_obj_int(ctx, idx, "cur");
    p->rlim_max  = js_get_obj_int(ctx, idx, "max");

    return 0;
}

int
libc_set_rlimit(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct rlimit *p = (struct rlimit *)obj;
    
    js_set_obj_int(ctx, idx, "cur", p->rlim_cur);
    js_set_obj_int(ctx, idx, "max", p->rlim_max);

    return 0;
}

int
libc_get_sched_param(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct sched_param *p = (struct sched_param *)obj; os_objzero(p);
    
    p->sched_priority = js_get_obj_int(ctx, idx, "priority");

    return 0;
}

int
libc_set_sched_param(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct sched_param *p = (struct sched_param *)obj;
    
    js_set_obj_int(ctx, idx, "priority", p->sched_priority);

    return 0;
}

#if js_LIBC_SIG
int
libc_get_sigaction(duk_context *ctx, duk_idx_t idx, int sig, duk_object_t obj)
{
    js_priv_t *priv = js_priv(ctx);
    struct sigaction *p = (struct sigaction *)obj; os_objzero(p);
    
    p->sa_mask  = *(sigset_t *)js_get_obj_buffer(ctx, idx, "mask", NULL);
    p->sa_flags = js_get_obj_int(ctx, idx, "flags");
    
    duk_get_prop_string(ctx, idx, "handler");
    if (duk_is_function(ctx, -1)) {
        p->sa_handler = libc_sig_handler;

        priv->sig[sig].is_func = true;
        char *name = js_get_obj_string(ctx, idx, "name", NULL);
        
        os_free(priv->sig[sig].name);
        priv->sig[sig].name = name;
    }
    else if (duk_is_number(ctx, -1)) {
        __sighandler_t action = (__sighandler_t)(uintptr_t)duk_require_int(ctx, -1);
        if (SIG_DFL!=action && SIG_IGN!=action) {
            action = SIG_DFL;
        }
        
        p->sa_handler = action;

        priv->sig[sig].is_func = false;
        priv->sig[sig].name = (char *)action;
    }
    duk_pop(ctx);

    return 0;
}

int
libc_set_sigaction(duk_context *ctx, duk_idx_t idx, int sig, duk_object_t obj)
{
    js_priv_t *priv = js_priv(ctx);
    struct sigaction *p = (struct sigaction *)obj;

    *(sigset_t *)js_get_obj_buffer(ctx, idx, "mask", NULL) = p->sa_mask;
    js_set_obj_int(ctx, idx, "flags", p->sa_flags);

    if (SIG_DFL==p->sa_handler || SIG_IGN==p->sa_handler) {
        js_set_obj_int(ctx, idx, "handler", (uintptr_t)p->sa_handler);
    } else {
        duk_push_global_object(ctx);                    // global
        duk_get_prop_string(ctx, -1, priv->sig[sig].name);  // global function/undefined
        if (duk_is_function(ctx, -1)) {
            duk_put_prop_string(ctx, idx, "handler");   // global
        } else {
            duk_pop(ctx);                               // global
        }
        duk_pop(ctx);                                   // empty
    }

    return 0;
}
#endif

int
libc_get_exit_status(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct exit_status *p = (struct exit_status *)obj; os_objzero(p);
    
    p->e_termination = js_get_obj_int(ctx, idx, "termination");
    p->e_exit  = js_get_obj_int(ctx, idx, "exit");

    return 0;
}

int
libc_set_exit_status(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct exit_status *p = (struct exit_status *)obj;
    
    js_set_obj_int(ctx, idx, "termination", p->e_termination);
    js_set_obj_int(ctx, idx, "exit", p->e_exit);

    return 0;
}

int
libc_get_utmp(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct utmp *p = (struct utmp *)obj; os_objzero(p);
    int i;
    
    p->ut_type  = js_get_obj_int(ctx, idx, "type");
    p->ut_pid   = js_get_obj_int(ctx, idx, "pid");
    p->ut_session = js_get_obj_int(ctx, idx, "session");
    js_copy_obj_string(ctx, idx, "line", p->ut_line, sizeof(p->ut_line)-1);
    js_copy_obj_string(ctx, idx, "id", p->ut_id, sizeof(p->ut_id)-1);
    js_copy_obj_string(ctx, idx, "user", p->ut_user, sizeof(p->ut_user)-1);
    js_copy_obj_string(ctx, idx, "host", p->ut_host, sizeof(p->ut_host)-1);

    js_obj_get(ctx, idx, libc_get_exit_status, &p->ut_exit, "exit");

    duk_get_prop_string(ctx, idx, "addr_v6");
    if (duk_is_array(ctx, -1)) {
        for (i=0; i<4; i++) {
            p->ut_addr_v6[i] = js_get_array_uint(ctx, -1, i);
        }
    }
    duk_pop(ctx);

    return 0;
}

int
libc_set_utmp(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct utmp *p = (struct utmp *)obj;
    int i;
    
    js_set_obj_int(ctx, idx, "type", p->ut_type);
    js_set_obj_int(ctx, idx, "pid", p->ut_pid);
    js_set_obj_int(ctx, idx, "session", p->ut_session);
    js_set_obj_string(ctx, idx, "line", p->ut_line);
    js_set_obj_string(ctx, idx, "id", p->ut_id);
    js_set_obj_string(ctx, idx, "user", p->ut_user);
    js_set_obj_string(ctx, idx, "host", p->ut_host);

    js_obj_get(ctx, idx, libc_set_exit_status, &p->ut_exit, "exit");

    duk_get_prop_string(ctx, idx, "addr_v6");
    if (duk_is_array(ctx, -1)) {
        for (i=0; i<4; i++) {
             js_set_array_uint(ctx, -1, i, p->ut_addr_v6[i]);
        }
    }
    duk_pop(ctx);

    return 0;
}

int
libc_get_passwd(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct passwd *p = (struct passwd *)obj; os_objzero(p);
    
    p->pw_uid       = js_get_obj_uint(ctx, idx, "uid");
    p->pw_gid       = js_get_obj_uint(ctx, idx, "gid");
    p->pw_name      = js_get_obj_string(ctx, idx, "name", NULL);
    p->pw_passwd    = js_get_obj_string(ctx, idx, "passwd", NULL);
    p->pw_gecos     = js_get_obj_string(ctx, idx, "gecos", NULL);
    p->pw_dir       = js_get_obj_string(ctx, idx, "dir", NULL);
    p->pw_shell     = js_get_obj_string(ctx, idx, "shell", NULL);

    return 0;
}

int
libc_set_passwd(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct passwd *p = (struct passwd *)obj;
    
    js_set_obj_uint(ctx, idx, "uid", p->pw_uid);
    js_set_obj_uint(ctx, idx, "gid", p->pw_gid);
    js_set_obj_string(ctx, idx, "name", p->pw_name);
    js_set_obj_string(ctx, idx, "passwd", p->pw_passwd);
    js_set_obj_string(ctx, idx, "gecos", p->pw_gecos);
    js_set_obj_string(ctx, idx, "dir", p->pw_dir);
    js_set_obj_string(ctx, idx, "shell", p->pw_shell);

    return 0;
}

int
libc_set_group(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct group *p = (struct group *)obj;
    int i, count;
    char *tmp;
    
    js_set_obj_uint(ctx, idx, "gid", p->gr_gid);
    js_set_obj_string(ctx, idx, "name", p->gr_name);

    duk_get_prop_string(ctx, idx, "mem");
    if (duk_is_array(ctx, -1)) {
        for (count = 0, tmp = p->gr_mem[0]; tmp; count++, tmp++) {
            js_set_array_string(ctx, -1, count, tmp);
        }
    }
    duk_pop(ctx);

    return 0;
}

int
libc_get_utsname(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct utsname *p = (struct utsname *)obj; os_objzero(p);
    
    js_copy_obj_string(ctx, idx, "sysname", p->sysname, sizeof(p->sysname)-1);
    js_copy_obj_string(ctx, idx, "release", p->release, sizeof(p->release)-1);
    js_copy_obj_string(ctx, idx, "version", p->version, sizeof(p->version)-1);
    js_copy_obj_string(ctx, idx, "machine", p->machine, sizeof(p->machine)-1);
    js_copy_obj_string(ctx, idx, "nodename", p->nodename, sizeof(p->nodename)-1);

    return 0;
}

int
libc_set_utsname(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct utsname *p = (struct utsname *)obj;
    
    js_set_obj_string(ctx, idx, "sysname", p->sysname);
    js_set_obj_string(ctx, idx, "release", p->release);
    js_set_obj_string(ctx, idx, "version", p->version);
    js_set_obj_string(ctx, idx, "machine", p->machine);
    js_set_obj_string(ctx, idx, "nodename", p->nodename);

    return 0;
}

#if js_LIBC_MOUNT
int
libc_get_fstab(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct fstab *p = (struct fstab *)obj; os_objzero(p);
    
    p->fs_freq      = js_get_obj_int(ctx, idx, "freq");
    p->fs_passno    = js_get_obj_int(ctx, idx, "passno");
    p->fs_spec      = js_get_obj_string(ctx, idx, "spec", NULL);
    p->fs_file      = js_get_obj_string(ctx, idx, "file", NULL);
    p->fs_vfstype   = js_get_obj_string(ctx, idx, "vfstype", NULL);
    p->fs_mntops    = js_get_obj_string(ctx, idx, "mntops", NULL);
    p->fs_type      = js_get_obj_string(ctx, idx, "type", NULL);

    return 0;
}

int
libc_set_fstab(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct fstab *p = (struct fstab *)obj;
    
    js_set_obj_int(ctx, idx, "freq", p->fs_freq);
    js_set_obj_int(ctx, idx, "passno", p->fs_passno);
    js_set_obj_string(ctx, idx, "spec", p->fs_spec);
    js_set_obj_string(ctx, idx, "file", p->fs_file);
    js_set_obj_string(ctx, idx, "vfstype", p->fs_vfstype);
    js_set_obj_string(ctx, idx, "mntops", p->fs_mntops);
    js_set_obj_string(ctx, idx, "type", (char *)p->fs_type);

    return 0;
}
#endif

int
libc_get_mntent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct mntent *p = (struct mntent *)obj; os_objzero(p);
    
    p->mnt_freq     = js_get_obj_int(ctx, idx, "freq");
    p->mnt_passno   = js_get_obj_int(ctx, idx, "passno");
    p->mnt_fsname   = js_get_obj_string(ctx, idx, "fsname", NULL);
    p->mnt_dir      = js_get_obj_string(ctx, idx, "dir", NULL);
    p->mnt_type     = js_get_obj_string(ctx, idx, "type", NULL);
    p->mnt_opts     = js_get_obj_string(ctx, idx, "opts", NULL);

    return 0;
}

int
libc_set_mntent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct mntent *p = (struct mntent *)obj;
    
    js_set_obj_int(ctx, idx, "freq", p->mnt_freq);
    js_set_obj_int(ctx, idx, "passno", p->mnt_passno);
    js_set_obj_string(ctx, idx, "fsname", p->mnt_fsname);
    js_set_obj_string(ctx, idx, "dir", p->mnt_dir);
    js_set_obj_string(ctx, idx, "type", p->mnt_type);
    js_set_obj_string(ctx, idx, "opts", p->mnt_opts);

    return 0;
}
/******************************************************************************/
#if js_LIBC_SIG
static duk_context *__js_ctx;

void js_ctx_save(duk_context *ctx)
{
    if (NULL==__js_ctx) {
        __js_ctx = ctx;
    }
}

duk_context *js_ctx_get(void)
{
    return __js_ctx;
}

void libc_sig_handler(int sig)
{
    duk_context *ctx = js_ctx_get();
    js_priv_t *priv = js_priv(ctx);

    if (priv->sig[sig].is_func) {
        /*
        * global handler sig
        */
        duk_push_global_object(ctx);
        duk_get_prop_string(ctx, -1, priv->sig[sig].name);
        if (duk_is_function(ctx, -1)) {
            duk_push_int(ctx, sig);
            /*
            * global result
            */
            duk_pcall(ctx, 1);
        }
        duk_pop_2(ctx);
    }
}

STATIC void
__libc_atexit_handler(void)
{
    duk_context *ctx = js_ctx_get();
    
    duk_push_global_object(ctx);
    duk_get_prop_string(ctx, -1, js_priv(ctx)->atexit_name);
    if (duk_is_function(ctx, -1)) {
        duk_pcall(ctx, 0);
    }
    duk_pop_2(ctx);
}
#endif /* js_LIBC_SIG */

#if js_LIBC_LINUX
JS_PARAM(timerfd_create, 2);
STATIC duk_ret_t
duke_timerfd_create(duk_context *ctx)
{
    int clock = duk_require_int(ctx, 0);
    int flags = duk_require_int(ctx, 1);

    int fd = timerfd_create(clock, flags);

    return js_push_error(ctx, fd), 1;
}

JS_PARAM(timerfd_settime, 4);
STATIC duk_ret_t
duke_timerfd_settime(duk_context *ctx)
{
    struct itimerspec new, old;
    int fd    = duk_require_int(ctx, 0);
    int flags = duk_require_int(ctx, 1);

    libc_get_itimerspec(ctx, 2, &new);
    int err = timerfd_settime(fd, flags, &new, &old);
    if (0==err) { // 0 is ok
        libc_set_itimerspec(ctx, 3, &old);
    }
    
    return js_push_error(ctx, err), 1;
}

JS_PARAM(timerfd_gettime, 2);
STATIC duk_ret_t
duke_timerfd_gettime(duk_context *ctx)
{
    struct itimerspec current;
    int fd = duk_require_int(ctx, 0);

    int err = timerfd_gettime(fd, &current);
    if (0==err) { // 0 is ok
        libc_set_itimerspec(ctx, 1, &current);
    }
    
    return js_push_error(ctx, err), 1;
}

JS_PARAM(eventfd, 2);
STATIC duk_ret_t
duke_eventfd(duk_context *ctx)
{
    int intval= duk_require_int(ctx, 0);
    int flags = duk_require_int(ctx, 1);

    int err = eventfd(intval, flags);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(signalfd, 3);
STATIC duk_ret_t
duke_signalfd(duk_context *ctx)
{
    int err;
    duk_size_t bsize;
    
    int fd = duk_require_int(ctx, 0);
    sigset_t *set = (sigset_t *)duk_require_buffer_data(ctx, 1, &bsize);
    if (bsize!=sizeof(sigset_t)) {
        err = __js_seterrno(ctx, -EINVAL2); goto error;
    }
    int flags = duk_require_int(ctx, 2);
    
    err = signalfd(fd, set, flags);
    if (err<0) {
        js_seterrno(ctx);
    }
    
error:
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(epoll_create, 1);
STATIC duk_ret_t
duke_epoll_create(duk_context *ctx)
{
    int size = duk_require_int(ctx, 0);
    
    int fd = epoll_create(size);

    return js_push_error(ctx, fd), 1;
}

JS_PARAM(epoll_create1, 1);
STATIC duk_ret_t
duke_epoll_create1(duk_context *ctx)
{
    int flags = duk_require_int(ctx, 0);
    
    int fd = epoll_create1(flags);

    return js_push_error(ctx, fd), 1;
}

#define duk_epoll_event_size    (2*sizeof(uint32))

JS_PARAM(epoll_wait, 3);
STATIC duk_ret_t
duke_epoll_wait(duk_context *ctx)
{
    int err, count;
    duk_size_t bsize;
    struct epoll_event *ev;
    
    int epfd = duk_require_int(ctx, 0);
    duk_buffer_t buffer = duk_require_buffer_data(ctx, 1, &bsize);
    if (0!=bsize%duk_epoll_event_size) {
        err = __js_seterrno(ctx, -EINVAL2); goto error;
    } else {
        count = bsize/duk_epoll_event_size;
        ev = (struct epoll_event *)buffer;
    }
    int timeout = duk_require_int(ctx, 2);

    err = epoll_wait(epfd, ev, count, timeout);
    if (err<0) {
        js_seterrno(ctx);
    }

error:
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(epoll_pwait, 4);
STATIC duk_ret_t
duke_epoll_pwait(duk_context *ctx)
{
    int err, count;
    duk_size_t bsize;
    struct epoll_event *ev;
    
    int epfd = duk_require_int(ctx, 0);
    duk_buffer_t buffer = duk_require_buffer_data(ctx, 1, &bsize);
    if (0!=bsize%duk_epoll_event_size) {
        err = __js_seterrno(ctx, -EINVAL2); goto error;
    } else {
        count = bsize/duk_epoll_event_size;
        ev = (struct epoll_event *)buffer;
    }
    int timeout = duk_require_int(ctx, 2);
    sigset_t *set = (sigset_t *)duk_require_buffer_data(ctx, 3, &bsize);
    if (bsize!=sizeof(sigset_t)) {
        err = __js_seterrno(ctx, -EINVAL4); goto error;
    }
    
    err = epoll_pwait(epfd, ev, count, timeout, set);
    if (err<0) {
        js_seterrno(ctx);
    }

error:
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(epoll_ctl, 4);
STATIC duk_ret_t
duke_epoll_ctl(duk_context *ctx)
{
    struct epoll_event ev;
    
    int epfd = duk_require_int(ctx, 0);
    int op = duk_require_int(ctx, 1);
    int fd = duk_require_int(ctx, 2);
    libc_get_epoll_event(ctx, 3, &ev);
    
    int err = epoll_ctl(epfd, op, fd, &ev);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(inotify_init, 0);
STATIC duk_ret_t
duke_inotify_init(duk_context *ctx)
{
    int fd = inotify_init();

    return js_push_error(ctx, fd), 1;
}

JS_PARAM(inotify_init1, 1);
STATIC duk_ret_t
duke_inotify_init1(duk_context *ctx)
{
    int flags = duk_require_int(ctx, 0);
    
    int err = inotify_init1(flags);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(inotify_add_watch, 3);
STATIC duk_ret_t
duke_inotify_add_watch(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    char *path = (char *)duk_require_string(ctx, 1);
    duk_uint_t mask = duk_require_uint(ctx, 2);
    
    int err = inotify_add_watch(fd, path, mask);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(inotify_rm_watch, 2);
STATIC duk_ret_t
duke_inotify_rm_watch(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    int wd = duk_require_int(ctx, 1);
    
    int err = inotify_rm_watch(fd, wd);

    return js_push_error(ctx, err), 1;
}
#endif /* js_LIBC_LINUX */

#if js_LIBC_ERROR
// 2.3 Error Messages
JS_PARAM(strerror, 1);
STATIC duk_ret_t
duke_strerror(duk_context *ctx)
{
    int errnum = duk_require_int(ctx, 0);

    errnum = errnum<0?-errnum:errnum;

    const char *errstring = strerror(errnum);

    return js_push_string(ctx, errstring), 1;
}
#endif /* js_LIBC_ERROR */

// 3.2.2 Unconstrained Allocation
// malloc free realloc calloc

#if js_LIBC_CTYPE
// 4.1 Classification of Characters
JS_PARAM(islower, 1);
STATIC duk_ret_t
duke_islower(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_bool(ctx, islower(c)), 1;
}

JS_PARAM(isupper, 1);
STATIC duk_ret_t
duke_isupper(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);
    
    return duk_push_bool(ctx, isupper(c)), 1;
}

JS_PARAM(isalpha, 1);
STATIC duk_ret_t
duke_isalpha(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);
    
    return duk_push_bool(ctx, isalpha(c)), 1;
}

JS_PARAM(isdigit, 1);
STATIC duk_ret_t
duke_isdigit(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);
    
    return duk_push_bool(ctx, isdigit(c)), 1;
}

JS_PARAM(isalnum, 1);
STATIC duk_ret_t
duke_isalnum(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_bool(ctx, isalnum(c)), 1;
}

JS_PARAM(isxdigit, 1);
STATIC duk_ret_t
duke_isxdigit(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_bool(ctx, isxdigit(c)), 1;
}

JS_PARAM(ispunct, 1);
STATIC duk_ret_t
duke_ispunct(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_bool(ctx, ispunct(c)), 1;
}

JS_PARAM(isspace, 1);
STATIC duk_ret_t
duke_isspace(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_bool(ctx, isspace(c)), 1;
}

JS_PARAM(isblank, 1);
STATIC duk_ret_t
duke_isblank(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_bool(ctx, isblank(c)), 1;
}

JS_PARAM(isgraph, 1);
STATIC duk_ret_t
duke_isgraph(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_bool(ctx, isgraph(c)), 1;
}

JS_PARAM(isprint, 1);
STATIC duk_ret_t
duke_isprint(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_bool(ctx, isprint(c)), 1;
}

JS_PARAM(iscntrl, 1);
STATIC duk_ret_t
duke_iscntrl(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_bool(ctx, iscntrl(c)), 1;
}

JS_PARAM(isascii, 1);
STATIC duk_ret_t
duke_isascii(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_bool(ctx, isascii(c)), 1;
}

// 4.2 Case Conversion
JS_PARAM(tolower, 1);
STATIC duk_ret_t
duke_tolower(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_int(ctx, tolower(c)), 1;
}

JS_PARAM(toupper, 1);
STATIC duk_ret_t
duke_toupper(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_int(ctx, toupper(c)), 1;
}

JS_PARAM(toascii, 1);
STATIC duk_ret_t
duke_toascii(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_int(ctx, toascii(c)), 1;
}
#endif /* js_LIBC_CTYPE */

// 4.3 Character class determination for wide characters

// 5.5 String/Array Comparison
#if js_LIBC_MEMORY
JS_PARAM(memcmp, 3);
STATIC duk_ret_t
duke_memcmp(duk_context *ctx)
{
    duk_pointer_t a = duk_require_pointer(ctx, 0);
    duk_pointer_t b = duk_require_pointer(ctx, 1);
    int size = duk_require_int(ctx, 2);

    int cmp = os_memcmp(a, b, size);

    return duk_push_int(ctx, cmp), 1;
}
#endif

#if js_LIBC_PRIVATE
JS_PARAM(fsize, 1);
STATIC duk_ret_t
duke_fsize(duk_context *ctx)
{
    char *filename = (char *)duk_require_string(ctx, 0);

    return duk_push_int(ctx, os_fsize(filename)), 1;
}

JS_PARAM(fexist, 1);
STATIC duk_ret_t
duke_fexist(duk_context *ctx)
{
    char *filename  = (char *)duk_require_string(ctx, 0);

    return duk_push_bool(ctx, os_file_exist(filename)), 1;
}
#endif

// 12 Input/Output on Streams
JS_PARAM(fopen, 2);
STATIC duk_ret_t
duke_fopen(duk_context *ctx)
{
    char *filename  = (char *)duk_require_string(ctx, 0);
    char *mode      = (char *)duk_require_string(ctx, 1);

    STREAM f = os_fopen(filename, mode);
    
    return js_push_pointer(ctx, f), 1;
}

JS_PARAM(fclose, 1);
STATIC duk_ret_t
duke_fclose(duk_context *ctx)
{
    STREAM f = (STREAM)duk_require_pointer(ctx, 0);

    int err = __os_fclose(f);

	return duk_push_int(ctx, err), 1;
}

JS_PARAM(fread, 2);
STATIC duk_ret_t
duke_fread(duk_context *ctx)
{
    duk_size_t bsize = 0;

    STREAM f = (STREAM)duk_require_pointer(ctx, 0);
    duk_buffer_t buf = duk_require_buffer_data(ctx, 1, &bsize);

    int err = fread(buf, 1, bsize, f);
error:
	return duk_push_int(ctx, err), 1;
}

JS_PARAM(freadEx, 2);
STATIC duk_ret_t
duke_freadEx(duk_context *ctx)
{
    STREAM f     = (STREAM)duk_require_pointer(ctx, 0);
    int size    = duk_require_int(ctx, 1);

    duk_buffer_t buf = js_push_dynamic_buffer(ctx, size);
    if (NULL==buf) {
        return duk_push_null(ctx), 1;
    }
    
    int c = fread(buf, 1, size, f);
    if (0==c) {
        duk_pop(ctx);
        duk_push_null(ctx);
    } else if (c>0 && c<size) {
        duk_resize_buffer(ctx, 2, c);
    }
    
	return 1;
}

JS_PARAM(fwrite, 2);
STATIC duk_ret_t
duke_fwrite(duk_context *ctx)
{
    duk_buffer_t buf = NULL;
    duk_size_t bsize = 0;
    
    STREAM f = (STREAM)duk_require_pointer(ctx, 0);
    int err = duk_require_buffer_or_lstring(ctx, 1, &buf, &bsize);
    if (err<0) {
        __js_seterrno(ctx, err); goto error;
    }
    
    err = fwrite(buf, 1, bsize, f);
error:
    return duk_push_uint(ctx, err), 1;
}

JS_PARAM(feof, 1);
STATIC duk_ret_t
duke_feof(duk_context *ctx)
{
    STREAM f = (STREAM)duk_require_pointer(ctx, 0);

    bool eof = os_feof(f);

    return duk_push_bool(ctx, eof), 1;
}

JS_PARAM(ferror, 1);
STATIC duk_ret_t
duke_ferror(duk_context *ctx)
{
    STREAM f = (STREAM)duk_require_pointer(ctx, 0);

    int err = ferror(f);

    return duk_push_bool(ctx, err), 1;
}

JS_PARAM(ftell, 1);
STATIC duk_ret_t
duke_ftell(duk_context *ctx)
{
    STREAM f = (STREAM)duk_require_pointer(ctx, 0);

    int len = ftell(f);

    return duk_push_int(ctx, len), 1;
}

JS_PARAM(fseek, 3);
STATIC duk_ret_t
duke_fseek(duk_context *ctx)
{
    STREAM f     = (STREAM)duk_require_pointer(ctx, 0);
    int offset  = duk_require_int(ctx, 1);
    int where   = duk_require_int(ctx, 2);

    int err = fseek(f, offset, where);

    return duk_push_int(ctx, err), 1;
}

JS_PARAM(fflush, 1);
STATIC duk_ret_t
duke_fflush(duk_context *ctx)
{
    STREAM f = (STREAM)duk_require_pointer(ctx, 0);

    int err = fflush(f);

    return duk_push_int(ctx, err), 1;
}

// 13 Low-Level Input/Output
JS_PARAM(open, DUK_VARARGS);
STATIC duk_ret_t
duke_open(duk_context *ctx)
{
    char *file  = (char *)duk_require_string(ctx, 0);
    int flag    = duk_require_int(ctx, 1);
    int fd      = INVALID_FD;

    switch(duk_get_max_idx(ctx)) {
        default:
        case 1:
            fd = open(file, flag);

            break;
        case 2: {
            uint32 mode = duk_require_uint(ctx, 2);

            fd = open(file, flag, mode);

        }   break;
    }

    return js_push_error(ctx, fd), 1;
}

JS_PARAM(close, 1);
STATIC duk_ret_t
duke_close(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);

    int err = __os_close(fd);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(read, 2);
STATIC duk_ret_t
duke_read(duk_context *ctx)
{
    duk_size_t bsize = 0;
    
    int fd = duk_require_int(ctx, 0);
    duk_buffer_t buf = duk_require_buffer_data(ctx, 1, &bsize);
    
    int err = read(fd, buf, bsize);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(readEx, 2);
STATIC duk_ret_t
duke_readEx(duk_context *ctx)
{
    int fd      = duk_require_int(ctx, 0);
    int size    = duk_require_int(ctx, 1);

    duk_buffer_t buf = js_push_dynamic_buffer(ctx, size);
    if (NULL==buf) {
        __js_seterrno(ctx, -ENOMEM); goto error;
    }

    int err = read(fd, buf, size);
    if (err<0) {
        duk_pop(ctx);
        js_seterrno(ctx); goto error;
    }

	return 1;
error:
    return duk_push_null(ctx), 1;
}

JS_PARAM(write, 2);
STATIC duk_ret_t
duke_write(duk_context *ctx)
{
    duk_size_t bsize = 0;
    duk_buffer_t buf = NULL;
    
    int fd      = duk_require_int(ctx, 0);
    int err     = duk_require_buffer_or_lstring(ctx, 1, &buf, &bsize);
    if (err<0) {
        __js_seterrno(ctx, err); goto error;
    }
    
    err = write(fd, buf, bsize);
    if (err<0) {
        js_seterrno(ctx); goto error;
    }

error:
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(pread, 3);
STATIC duk_ret_t
duke_pread(duk_context *ctx)
{
    duk_size_t bsize = 0;
    int fd      = duk_require_int(ctx, 0);
    duk_buffer_t buf = duk_require_buffer_data(ctx, 1, &bsize);
    int offset  = duk_require_int(ctx, 2);

    int err = pread(fd, buf, bsize, offset);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(preadEx, 3);
STATIC duk_ret_t
duke_preadEx(duk_context *ctx)
{
    int fd      = duk_require_int(ctx, 0);
    int size    = duk_require_int(ctx, 1);
    int offset  = duk_require_int(ctx, 2);

    duk_buffer_t buf = js_push_dynamic_buffer(ctx, size);
    if (NULL==buf) {
        __js_seterrno(ctx, -ENOMEM); goto error;
    }

    int err = pread(fd, buf, size, offset);
    if (err<0) {
        duk_pop(ctx);
        js_seterrno(ctx); goto error;
    }

	return 1;
error:
    return duk_push_null(ctx), 1;
}

JS_PARAM(pwrite, 3);
STATIC duk_ret_t
duke_pwrite(duk_context *ctx)
{
    duk_size_t bsize = 0;
    duk_buffer_t buf;
    
    int fd      = duk_require_int(ctx, 0);
    int err     = duk_require_buffer_or_lstring(ctx, 1, &buf, &bsize);
    if (err<0) {
        __js_seterrno(ctx, err); goto error;
    }
    int offset  = duk_require_int(ctx, 2);
    
    err = pwrite(fd, buf, bsize, offset);
    if (err<0) {
        js_seterrno(ctx);
    }

error:
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(lseek, 3);
STATIC duk_ret_t
duke_lseek(duk_context *ctx)
{
    int fd      = duk_require_int(ctx, 0);
    int offset  = duk_require_int(ctx, 1);
    int where   = duk_require_int(ctx, 2);
    
    int err = lseek(fd, offset, where);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(fdopen, 2);
STATIC duk_ret_t
duke_fdopen(duk_context *ctx)
{
    int fd      = duk_require_int(ctx, 0);
    char *mode  = (char *)duk_require_string(ctx, 1);

    STREAM f = fdopen(fd, mode);
    
    return js_push_pointer(ctx, f), 1;
}

JS_PARAM(fileno, 1);
STATIC duk_ret_t
duke_fileno(duk_context *ctx)
{
    STREAM f = (STREAM)duk_require_pointer(ctx, 0);

    int fd = fileno(f);

	return js_push_error(ctx, fd), 1;
}

// 13.6 Fast Scatter-Gather I/O
JS_PARAM(readv, 2);
STATIC duk_ret_t
duke_readv(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    int count = js_get_array_length(ctx, 1);
    int i;

    struct iovec iov[count];
    for (i=0; i<count; i++) {
        iov[i].iov_base = js_get_array_buffer(ctx, 1, i, &iov[i].iov_len);
    }

    int err = readv(fd, iov, count);

	return js_push_error(ctx, err), 1;
}

JS_PARAM(writev, 2);
STATIC duk_ret_t
duke_writev(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    int count = js_get_array_length(ctx, 1);
    int i;

    struct iovec iov[count];
    for (i=0; i<count; i++) {
        iov[i].iov_base = js_get_array_buffer(ctx, 1, i, &iov[i].iov_len);
    }

    int err = writev(fd, iov, count);

	return js_push_error(ctx, err), 1;
}

// 13.7 Memory-mapped I/O
#if js_LIBC_MEMORY
JS_PARAM(mmap, 6);
STATIC duk_ret_t
duke_mmap(duk_context *ctx)
{
    duk_pointer_t address = duk_require_pointer(ctx, 0);
    duk_uint_t len  = duk_require_uint(ctx, 1);
    int protect     = duk_require_int(ctx, 2);
    int flags       = duk_require_int(ctx, 3);
    int fd          = duk_require_int(ctx, 4);
    duk_uint_t offset = duk_require_uint(ctx, 5);

    duk_pointer_t pointer = mmap(address, len, protect, flags, fd, offset);
    if (MAP_FAILED==pointer) {
        js_seterrno(ctx);
        pointer = NULL;
    }

    return js_push_pointer(ctx, pointer), 1;
}

JS_PARAM(munmap, 2);
STATIC duk_ret_t
duke_munmap(duk_context *ctx)
{
    duk_pointer_t address = duk_require_pointer(ctx, 0);
    duk_uint_t len  = duk_require_uint(ctx, 1);

    int err = munmap(address, len);

	return js_push_error(ctx, err), 1;
}

JS_PARAM(msync, 3);
STATIC duk_ret_t
duke_msync(duk_context *ctx)
{
    duk_pointer_t address = duk_require_pointer(ctx, 0);
    duk_uint_t len  = duk_require_uint(ctx, 1);
    int flags       = duk_require_int(ctx, 2);

    int err = msync(address, len, flags);

	return js_push_error(ctx, err), 1;
}

JS_PARAM(mremap, 4);
STATIC duk_ret_t
duke_mremap(duk_context *ctx)
{
    duk_pointer_t address = duk_require_pointer(ctx, 0);
    duk_uint_t len  = duk_require_uint(ctx, 1);
    duk_uint_t nlen = duk_require_uint(ctx, 2);
    int flags       = duk_require_int(ctx, 3);
    
    duk_pointer_t pointer = mremap(address, len, nlen, flags);
    if (MAP_FAILED==pointer) {
        js_seterrno(ctx);
        pointer = NULL;
    }
    
    return js_push_pointer(ctx, pointer), 1;
}

JS_PARAM(madvise, 3);
STATIC duk_ret_t
duke_madvise(duk_context *ctx)
{
    duk_pointer_t address = duk_require_pointer(ctx, 0);
    duk_uint_t len  = duk_require_uint(ctx, 1);
    int advice      = duk_require_int(ctx, 2);

    int err = madvise(address, len, advice);

	return js_push_error(ctx, err), 1;
}
#endif

// 13.8 Waiting for Input or Output
#if js_LIBC_FDSET
JS_PARAM(FD_NEW, 0);
STATIC duk_ret_t
duke_FD_NEW(duk_context *ctx)
{
    return js_push_dynamic_buffer(ctx, sizeof(fd_set)), 1;
}

JS_PARAM(FD_ZERO, 1);
STATIC duk_ret_t
duke_FD_ZERO(duk_context *ctx)
{
    int err = 0;
    duk_size_t bsize;
    
    fd_set *set = (fd_set *)duk_require_buffer_data(ctx, 0, &bsize);
    if (bsize!=sizeof(fd_set)) {
        err = __js_seterrno(ctx, -EINVAL1); goto error;
    }
    
    FD_ZERO(set);
    
error:
	return duk_push_int(ctx, err), 1;
}

JS_PARAM(FD_SET, 2);
STATIC duk_ret_t
duke_FD_SET(duk_context *ctx)
{
    int err = 0;
    duk_size_t bsize;
    
    fd_set *set = (fd_set *)duk_require_buffer_data(ctx, 0, &bsize);
    if (bsize!=sizeof(fd_set)) {
        err = __js_seterrno(ctx, -EINVAL1); goto error;
    }
    int fd = duk_require_int(ctx, 1);

    FD_SET(fd, set);
    
error:
	return duk_push_int(ctx, err), 1;
}

JS_PARAM(FD_CLR, 2);
STATIC duk_ret_t
duke_FD_CLR(duk_context *ctx)
{
    int err = 0;
    duk_size_t bsize;
    
    fd_set *set = (fd_set *)duk_require_buffer_data(ctx, 0, &bsize);
    if (bsize!=sizeof(fd_set)) {
        err = __js_seterrno(ctx, -EINVAL1); goto error;
    }
    int fd = duk_require_int(ctx, 1);

    FD_CLR(fd, set);
    
error:
	return duk_push_int(ctx, err), 1;
}

JS_PARAM(FD_ISSET, 2);
STATIC duk_ret_t
duke_FD_ISSET(duk_context *ctx)
{
    int err = 0;
    duk_size_t bsize;
    
    fd_set *set = (fd_set *)duk_require_buffer_data(ctx, 0, &bsize);
    if (bsize!=sizeof(fd_set)) {
        err = __js_seterrno(ctx, -EINVAL1); return duk_push_false(ctx), 1;
    }
    int fd = duk_require_int(ctx, 1);

    bool is_set = FD_ISSET(fd, set);
    
	return duk_push_bool(ctx, is_set), 1;
}
#endif

JS_PARAM(select, 5);
STATIC duk_ret_t
duke_select(duk_context *ctx)
{
    int err = 0;
    duk_size_t bsize;
    fd_set *rset = NULL, *wset = NULL, *eset = NULL;
    struct timeval tv;
    
    int nfds = duk_require_int(ctx, 0);
    if (duk_is_buffer(ctx, 1)) {
        rset = (fd_set *)duk_require_buffer_data(ctx, 1, &bsize);
        if (bsize!=sizeof(fd_set)) {
            err = __js_seterrno(ctx, -EINVAL2); goto error;
        }
    }

    if (duk_is_buffer(ctx, 2)) {
        wset = (fd_set *)duk_require_buffer_data(ctx, 2, &bsize);
        if (bsize!=sizeof(fd_set)) {
            err = __js_seterrno(ctx, -EINVAL3); goto error;
        }
    }

    if (duk_is_buffer(ctx, 3)) {
        eset = (fd_set *)duk_require_buffer_data(ctx, 3, &bsize);
        if (bsize!=sizeof(fd_set)) {
            err = __js_seterrno(ctx, -EINVAL4); goto error;
        }
    }
    
    libc_get_timeval(ctx, 4, &tv);
    err = select(nfds, rset, wset, eset, &tv);
    libc_set_timeval(ctx, 4, &tv);

    if (err<0) {
        js_seterrno(ctx);
    }

error:
	return duk_push_int(ctx, err), 1;
}

// 13.9 Synchronizing I/O operations
JS_PARAM(sync, 0);
STATIC duk_ret_t
duke_sync(duk_context *ctx)
{
    return sync(), 0;
}

JS_PARAM(fsync, 1);
STATIC duk_ret_t
duke_fsync(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);

    int err = fsync(fd);
    if (err<0) {
        js_seterrno(ctx);
    }
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(fcntl, 3);
STATIC duk_ret_t
duke_fcntl(duk_context *ctx)
{
    int err = 0;
    int fd  = duk_require_int(ctx, 0);
    int cmd = duk_require_int(ctx, 1);
    
    switch(cmd) {
        case F_DUPFD:
        case F_SETFD:
        case F_GETFD:
        case F_GETFL:
        case F_SETFL: {
            int new = duk_require_int(ctx, 2);
            err = fcntl(fd, cmd, new);
            if (err<0) {
                js_seterrno(ctx);
            }
            
        }   break;
        case F_GETLK:
        case F_SETLK:
        case F_SETLKW:
        case F_GETOWN:
        case F_SETOWN: {
            struct flock k;

            libc_get_flock(ctx, 2, &k);
            err = fcntl(fd, cmd, &k);
            if (err<0) {
                js_seterrno(ctx);
            }
            
        }   break;
        default:
            err = __js_seterrno(ctx, -ENOSUPPORT);
            break;
    }
    
	return duk_push_int(ctx, err), 1;
}

JS_PARAM(dup, 1);
STATIC duk_ret_t
duke_dup(duk_context *ctx)
{
    int old = duk_require_int(ctx, 0);

    int new = dup(old);

	return js_push_error(ctx, new), 1;
}

JS_PARAM(dup2, 2);
STATIC duk_ret_t
duke_dup2(duk_context *ctx)
{
    int old = duk_require_int(ctx, 0);
    int new = duk_require_int(ctx, 1);

    int fd = dup2(old, new);

	return js_push_error(ctx, fd), 1;
}

JS_PARAM(ioctl, 3);
STATIC duk_ret_t
duke_ioctl(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    int op = duk_require_int(ctx, 1);
    
	return 0;
}

// 14 File System Interface
// 14.1 Working Directory
JS_PARAM(getcwd, 0);
STATIC duk_ret_t
duke_getcwd(duk_context *ctx)
{
    char path[1+PATH_MAX] = {0};
    
    char *cwd = getcwd(path, PATH_MAX);
    if (NULL==cwd) {
        js_seterrno(ctx);
        duk_push_null(ctx);
    } else {
        duk_push_string(ctx, cwd);
    }

	return 1;
}

JS_PARAM(chdir, 1);
STATIC duk_ret_t
duke_chdir(duk_context *ctx)
{
    char *dirname = (char *)duk_require_string(ctx, 0);

    int err = chdir(dirname);

	return js_push_error(ctx, err), 1;
}

JS_PARAM(fchdir, 1);
STATIC duk_ret_t
duke_fchdir(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);

    int err = fchdir(fd);

	return js_push_error(ctx, err), 1;
}

// 14.2 Accessing Directories
JS_PARAM(opendir, 1);
STATIC duk_ret_t
duke_opendir(duk_context *ctx)
{
    char *dirname = (char *)duk_require_string(ctx, 0);

    DIR *dir = opendir(dirname);
    if (NULL==dir) {
        js_seterrno(ctx);
    }
    
    return js_push_pointer(ctx, dir), 1;
}

JS_PARAM(fdopendir, 1);
STATIC duk_ret_t
duke_fdopendir(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);

    DIR *dir = fdopendir(fd);
    if (NULL==dir) {
        js_seterrno(ctx);
    }
    
    return js_push_pointer(ctx, dir), 1;
}

JS_PARAM(dirfd, 1);
STATIC duk_ret_t
duke_dirfd(duk_context *ctx)
{
    DIR *dir = (DIR *)duk_require_pointer(ctx, 0);

    int fd = dirfd(dir);
    
	return duk_push_int(ctx, fd), 1;
}

JS_PARAM(readdir, 1);
STATIC duk_ret_t
duke_readdir(duk_context *ctx)
{
    DIR *dir = (DIR *)duk_require_pointer(ctx, 0);
        
    struct dirent *d = readdir(dir);
    if (NULL==d) {
        js_seterrno(ctx);
    }

	return js_obj_push(ctx, libc_set_dirent, readdir(dir)), 1;
}

JS_PARAM(closedir, 1);
STATIC duk_ret_t
duke_closedir(duk_context *ctx)
{
    DIR *dir = (DIR *)duk_require_pointer(ctx, 0);
    
    int err = closedir(dir);

	return js_push_error(ctx, err), 1;
}

JS_PARAM(rewinddir, 1);
STATIC duk_ret_t
duke_rewinddir(duk_context *ctx)
{
    DIR *dir = (DIR *)duk_require_pointer(ctx, 0);

    return rewinddir(dir), 0;
}

JS_PARAM(telldir, 1);
STATIC duk_ret_t
duke_telldir(duk_context *ctx)
{
    DIR *dir = (DIR *)duk_require_pointer(ctx, 0);

    int err = telldir(dir);

    return duk_push_int(ctx, err), 1;
}

JS_PARAM(seekdir, 2);
STATIC duk_ret_t
duke_seekdir(duk_context *ctx)
{
    DIR *dir = (DIR *)duk_require_pointer(ctx, 0);
    int pos  = duk_require_int(ctx, 1);
    
    return seekdir(dir, pos), 0;
}

// 14.4 Hard Links
JS_PARAM(link, 2);
STATIC duk_ret_t
duke_link(duk_context *ctx)
{
    char *old = (char *)duk_require_string(ctx, 0);
    char *new = (char *)duk_require_string(ctx, 1);

    int err = link(old, new);

	return js_push_error(ctx, err), 1;
}

// 14.5 Symbolic Links
JS_PARAM(symlink, 2);
STATIC duk_ret_t
duke_symlink(duk_context *ctx)
{
    char *old = (char *)duk_require_string(ctx, 0);
    char *new = (char *)duk_require_string(ctx, 1);

    int err = symlink(old, new);

	return js_push_error(ctx, err), 1;
}

JS_PARAM(readlink, 1);
STATIC duk_ret_t
duke_readlink(duk_context *ctx)
{
    char *filename = (char *)duk_require_string(ctx, 0);
    int size = OS_LINE_LEN;
    char *line = NULL;
    
    while(1) {
        line = (char *)os_realloc(line, 1+size);
        
        int len = readlink(filename, line, size);
        if (len<0) {
            js_seterrno(ctx); 
            duk_push_null(ctx);
            
            break;
        }
        else if (len<=size) {
            line[len] = 0;
            js_push_lstring(ctx, line, len);
            
            break;
        }

        size += 1+OS_LINE_LEN;
    }

error:
    os_free(line);
    return 1;
}

JS_PARAM(realpath, 1);
STATIC duk_ret_t
duke_realpath(duk_context *ctx)
{
    char *filename = (char *)duk_require_string(ctx, 0);

    char *path = realpath(filename, NULL);
    if (NULL==path) {
        js_seterrno(ctx);
    }

    return js_push_string(ctx, path), 1;
}

// 14.6 Deleting Files
JS_PARAM(unlink, 1);
STATIC duk_ret_t
duke_unlink(duk_context *ctx)
{    
    char *filename = (char *)duk_require_string(ctx, 0);

    int err = unlink(filename);

	return js_push_error(ctx, err), 1;
}

static inline int
__rmdir_r(duk_context *ctx, char *path)
{
    int err;
    struct dirent *file = NULL;
        
    DIR *dir = opendir(path);
    if (NULL==dir) {
        return errno;
    }

    while (NULL != (file=readdir(dir))) {
        struct stat st;
        char *filename = file->d_name; /* just name, not include path */
        
        if (false==is_current_dir(filename) && false==is_father_dir(filename)) {
            char line[1+OS_LINE_LEN];
            
            os_sprintf(line, "%s/%s", path, filename);
            
            if (stat(filename, &st) >= 0 && S_ISDIR(st.st_mode)) {
                __rmdir_r(ctx, line);
            } else {
                unlink(line);
            }
        }
    }
    
    os_closedir(dir);

    rmdir(path);
    
    return 0;
}

JS_PARAM(rmdir, DUK_VARARGS);
STATIC duk_ret_t
duke_rmdir(duk_context *ctx)
{
    int err;
    bool all = false;
    static char line[1+OS_LINE_LEN];
    
    char *filename = (char *)duk_require_string(ctx, 0);
    switch(duk_get_max_idx(ctx)) {
        default:
        case 0: 
            all = false; 
            break;
        case 1: 
            all = duk_require_bool(ctx, 1); 
            break;        
    }

    if (all) {
        err = __rmdir_r(ctx, filename);
        if (err<0) {
            __js_seterrno(ctx, err);
        }
    } else {
        err = rmdir(filename);
        if (err<0) {
            js_seterrno(ctx);
        }
    }

error:
	return duk_push_int(ctx, err), 1;
}

JS_PARAM(remove, 1);
STATIC duk_ret_t
duke_remove(duk_context *ctx)
{
    char *filename = (char *)duk_require_string(ctx, 0);

    int err = remove(filename);

	return js_push_error(ctx, err), 1;
}

// 14.7 Renaming Files
JS_PARAM(rename, 2);
STATIC duk_ret_t
duke_rename(duk_context *ctx)
{
    char *old = (char *)duk_require_string(ctx, 0);
    char *new = (char *)duk_require_string(ctx, 1);

    int err = rename(old, new);
    
	return js_push_error(ctx, err), 1;

}

// 14.8 Creating Directories
JS_PARAM(mkdir, DUK_VARARGS);
STATIC duk_ret_t
duke_mkdir(duk_context *ctx)
{
    char *filename = (char *)duk_require_string(ctx, 0);
    int mode = duk_require_uint(ctx, 1);
    bool ignore = false;

    if (2==duk_get_max_idx(ctx)) {
        ignore = duk_require_boolean(ctx, 2);
    }

    if (0==mode) {
        mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
    }
    
    int err = mkdir(filename, mode);
    if (err<0 && EEXIST==errno && ignore) {
        err = 0;
    }

    return duk_push_int(ctx, err), 1;
}

// 14.9 File Attributes
// 14.9.2 Reading the Attributes of a File
JS_PARAM(stat, 2);
STATIC duk_ret_t
duke_stat(duk_context *ctx)
{
    struct stat st;
    
    char *filename = (char *)duk_require_string(ctx, 0);

    int err = stat(filename, &st);
    if (0==err) { // 0 is ok
        libc_set_stat(ctx, 1, &st);
    }

    return js_push_error(ctx, err), 1;
}

JS_PARAM(lstat, 2);
STATIC duk_ret_t
duke_lstat(duk_context *ctx)
{
    struct stat st;
    
    char *filename = (char *)duk_require_string(ctx, 0);

    int err = lstat(filename, &st);
    if (0==err) { // 0 is ok
        libc_set_stat(ctx, 1, &st);
    }

    return js_push_error(ctx, err), 1;
}

JS_PARAM(fstat, 2);
STATIC duk_ret_t
duke_fstat(duk_context *ctx)
{
    struct stat st;
    
    int fd = duk_require_int(ctx, 0);

    int err = fstat(fd, &st);
    if (0==err) { // 0 is ok
        libc_set_stat(ctx, 1, &st);
    }

    return js_push_error(ctx, err), 1;
}

// 14.9.3 Testing the Type of a File
#if 0
JS_PARAM(S_ISDIR, 1);
STATIC duk_ret_t
duke_S_ISDIR(duk_context *ctx)
{
    duk_uint_t mode = duk_require_uint(ctx, 0);

    return duk_push_bool(ctx, S_ISDIR(mode)), 1;
}

JS_PARAM(S_ISCHR, 1);
STATIC duk_ret_t
duke_S_ISCHR(duk_context *ctx)
{
    duk_uint_t mode = duk_require_uint(ctx, 0);

    return duk_push_bool(ctx, S_ISCHR(mode)), 1;
}

JS_PARAM(S_ISBLK, 1);
STATIC duk_ret_t
duke_S_ISBLK(duk_context *ctx)
{
    duk_uint_t mode = duk_require_uint(ctx, 0);

    return duk_push_bool(ctx, S_ISBLK(mode)), 1;
}

JS_PARAM(S_ISREG, 1);
STATIC duk_ret_t
duke_S_ISREG(duk_context *ctx)
{
    duk_uint_t mode = duk_require_uint(ctx, 0);

    return duk_push_bool(ctx, S_ISREG(mode)), 1;
}

JS_PARAM(S_ISFIFO, 1);
STATIC duk_ret_t
duke_S_ISFIFO(duk_context *ctx)
{
    duk_uint_t mode = duk_require_uint(ctx, 0);

    return duk_push_bool(ctx, S_ISFIFO(mode)), 1;
}

JS_PARAM(S_ISLNK, 1);
STATIC duk_ret_t
duke_S_ISLNK(duk_context *ctx)
{
    duk_uint_t mode = duk_require_uint(ctx, 0);

    return duk_push_bool(ctx, S_ISLNK(mode)), 1;
}

JS_PARAM(S_ISSOCK, 1);
STATIC duk_ret_t
duke_S_ISSOCK(duk_context *ctx)
{
    duk_uint_t mode = duk_require_uint(ctx, 0);

    return duk_push_bool(ctx, S_ISSOCK(mode)), 1;
}

JS_PARAM(S_TYPEISMQ, 1);
STATIC duk_ret_t
duke_S_TYPEISMQ(duk_context *ctx)
{
    struct stat st;

    libc_get_stat(ctx, 0, &st);

    return duk_push_bool(ctx, S_TYPEISMQ(&st)), 1;
}

JS_PARAM(S_TYPEISSEM, 1);
STATIC duk_ret_t
duke_S_TYPEISSEM(duk_context *ctx)
{
    struct stat st;

    libc_get_stat(ctx, 0, &st);

    return duk_push_bool(ctx, S_TYPEISSEM(&st)), 1;
}

JS_PARAM(S_TYPEISSHM, 1);
STATIC duk_ret_t
duke_S_TYPEISSHM(duk_context *ctx)
{
    struct stat st;

    libc_get_stat(ctx, 0, &st);

    return duk_push_bool(ctx, S_TYPEISSHM(&st)), 1;
}
#endif

// 14.9.4 File Owner
JS_PARAM(chown, 3);
STATIC duk_ret_t
duke_chown(duk_context *ctx)
{
	char *filename = (char *)duk_require_string(ctx, 0);
	uid_t owner = duk_require_uint(ctx, 1);
	gid_t group = duk_require_uint(ctx, 2);

    int err = chown(filename, owner, group);

	return js_push_error(ctx, err), 1;
}

JS_PARAM(fchown, 3);
STATIC duk_ret_t
duke_fchown(duk_context *ctx)
{
	int fd = duk_require_int(ctx, 0);
	int owner = duk_require_int(ctx, 1);
	int group = duk_require_int(ctx, 2);

    int err = fchown(fd, owner, group);

	return js_push_error(ctx, err), 1;
}

// 14.9.7 Assigning File Permissions
JS_PARAM(umask, 1);
STATIC duk_ret_t
duke_umask(duk_context *ctx)
{
	mode_t mask = duk_require_uint(ctx, 0);

	int err = umask(mask);
	
	return duk_push_int(ctx, err), 1;
}

JS_PARAM(chmod, 2);
STATIC duk_ret_t
duke_chmod(duk_context *ctx)
{
    char *filename = (char *)duk_require_string(ctx, 0);
    mode_t mode = duk_require_uint(ctx, 1);
    
	int err = chmod(filename, mode);

	return js_push_error(ctx, err), 1;
}

JS_PARAM(fchmod, 2);
STATIC duk_ret_t
duke_fchmod(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    mode_t mode = duk_require_uint(ctx, 1);
    
	int err = fchmod(fd, mode);

	return js_push_error(ctx, err), 1;
}

// 14.9.8 Testing Permission to Access a File
JS_PARAM(access, 2);
STATIC duk_ret_t
duke_access(duk_context *ctx)
{
    char *filename = (char *)duk_require_string(ctx, 0);
    int how = duk_require_int(ctx, 1);
    
	int err = access(filename, how);

	return js_push_error(ctx, err), 1;
}

// 14.9.9 File Times
JS_PARAM(utime, 2);
STATIC duk_ret_t
duke_utime(duk_context *ctx)
{
    struct utimbuf b = {0};
    char *filename = (char *)duk_require_string(ctx, 0);
    libc_get_utimbuf(ctx, 1, &b);
    
	int err = utime(filename, &b);

	return js_push_error(ctx, err), 1;
}

// 14.9.10 File Size
JS_PARAM(truncate, 2);
STATIC duk_ret_t
duke_truncate(duk_context *ctx)
{
    char *filename = (char *)duk_require_string(ctx, 0);
    duk_uint_t len = duk_require_uint(ctx, 1);
    
	int err = truncate(filename, len);

	return js_push_error(ctx, err), 1;
}

JS_PARAM(ftruncate, 2);
STATIC duk_ret_t
duke_ftruncate(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    duk_uint_t len = duk_require_uint(ctx, 1);
    
	int err = ftruncate(fd, len);

	return js_push_error(ctx, err), 1;
}

// 14.10 Making Special Files
JS_PARAM(mknod, 3);
STATIC duk_ret_t
duke_mknod(duk_context *ctx)
{
    char *filename = (char *)duk_require_string(ctx, 0);
    int mode = duk_require_int(ctx, 1);
    int dev = duk_require_int(ctx, 2);
    
	int err = mknod(filename, mode, dev);

	return js_push_error(ctx, err), 1;
}

// 14.11 Temporary Files
JS_PARAM(tmpfile, 0);
STATIC duk_ret_t
duke_tmpfile(duk_context *ctx)
{
	STREAM f = tmpfile();

	return js_push_pointer(ctx, f), 1;
}

JS_PARAM(mkstemp, 1);
STATIC duk_ret_t
duke_mkstemp(duk_context *ctx)
{
    char *template = (char *)duk_require_string(ctx, 0);
    
	int err = mkstemp(template);

	return js_push_error(ctx, err), 1;
}

JS_PARAM(mkdtemp, 1);
STATIC duk_ret_t
duke_mkdtemp(duk_context *ctx)
{
    char *template = (char *)duk_require_string(ctx, 0);
    
	char *filename = mkdtemp(template);

	return js_push_string(ctx, filename), 1;
}

// 15 Pipes and FIFOs
// 15.1 Creating a Pipe
JS_PARAM(pipe, 1);
STATIC duk_ret_t
duke_pipe(duk_context *ctx)
{
	int fd[2] = {
        [0] = js_get_array_int(ctx, 0, 0),
        [1] = js_get_array_int(ctx, 0, 1),
	};
	
    int err = pipe(fd);

    js_set_array_int(ctx, 0, 0, fd[0]);
    js_set_array_int(ctx, 0, 1, fd[1]);

	return js_push_error(ctx, err), 1;
}


// 15.2 Pipe to a Subprocess
JS_PARAM(popen, 2);
STATIC duk_ret_t
duke_popen(duk_context *ctx)
{
    char *command = (char *)duk_require_string(ctx, 0);
    char *mode = (char *)duk_require_string(ctx, 1);
    
	STREAM f = popen(command, mode);

	return js_push_pointer(ctx, f), 1;
}

JS_PARAM(pclose, 1);
STATIC duk_ret_t
duke_pclose(duk_context *ctx)
{
    STREAM f = (STREAM)duk_require_pointer(ctx, 0);
    
	int err = pclose(f);
	
	return duk_push_int(ctx, err), 1;
}

// 15.3 FIFO Special Files
JS_PARAM(mkfifo, 2);
STATIC duk_ret_t
duke_mkfifo(duk_context *ctx)
{
    char *filename = (char *)duk_require_string(ctx, 0);
    int mode = duk_require_int(ctx, 1);
    
	int err = mkfifo(filename, mode);

	return js_push_error(ctx, err), 1;
}

// 16 Sockets
// 16.3 Socket Addresses
JS_PARAM(bind, 2);
STATIC duk_ret_t
duke_bind(duk_context *ctx)
{
    os_sockaddr_t sa = OS_SOCKADDR_ZERO();
    int fd = duk_require_int(ctx, 0);

    int err = libc_get_sockaddr(ctx, 1, &sa);
    if (err<0) {
        __js_seterrno(ctx, err);
        
        goto error;
    }
    
    err = bind(fd, &sa.c, os_sockaddr_len(&sa.c));
    if (err<0) {
        js_seterrno(ctx);

        goto error;
    }
    
error:
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(getsockname, 2);
STATIC duk_ret_t
duke_getsockname(duk_context *ctx)
{
    sockaddr_len_t len = sizeof(sockaddr_t);
    os_sockaddr_t sa = OS_SOCKADDR_ZERO();
    
    int fd = duk_require_int(ctx, 0);
    
    int err = getsockname(fd, &sa.c, &len);
    if (0==err) { // 0 is ok
        libc_set_sockaddr(ctx, 1, &sa);
    }
    
    return js_push_error(ctx, err), 1;
}

JS_PARAM(if_nametoindex, 1);
STATIC duk_ret_t
duke_if_nametoindex(duk_context *ctx)
{
    char *ifname = (char *)duk_require_string(ctx, 0);
    
	duk_uint_t ifindex = if_nametoindex(ifname);
	
	return duk_push_uint(ctx, ifindex), 1;
}

JS_PARAM(if_indextoname, 2);
STATIC duk_ret_t
duke_if_indextoname(duk_context *ctx)
{
    duk_uint_t ifindex = duk_require_uint(ctx, 0);
    char ifname[1+IFNAMSIZ] = {0};
    
	char *name = if_indextoname(ifindex, ifname);

	return js_push_string(ctx, name), 1;
}

JS_PARAM(if_nameindex, 0);
STATIC duk_ret_t
duke_if_nameindex(duk_context *ctx)
{
	struct if_nameindex *array = if_nameindex();
	if (NULL==array) {
        return duk_push_null(ctx), 1;
	}
    
	struct if_nameindex *p;
	int i;

	duk_push_object(ctx);
    for (i=0, p = &array[0]; p; p++, i++) {
        duk_push_object(ctx);
            js_set_obj_uint(ctx, -1, "index", p->if_index);
            js_set_obj_string(ctx, -1, "name", p->if_name);
        duk_put_prop_index(ctx, -1, i);
    }
	
	return 1;
}

// 16.6 The Internet Namespace
JS_PARAM(inet_aton, 1);
STATIC duk_ret_t
duke_inet_aton(duk_context *ctx)
{
    struct in_addr in;
    char *name = (char *)duk_require_string(ctx, 0);
    
	int err = inet_aton(name, &in);
	if (0==err) { // return 0 if failed
	    in.s_addr = 0;
	}

	return duk_push_uint(ctx, in.s_addr), 1;
}

JS_PARAM(inet_addr, 1);
STATIC duk_ret_t
duke_inet_addr(duk_context *ctx)
{
    char *name = (char *)duk_require_string(ctx, 0);

	duk_uint_t addr = inet_addr(name);
	
	return duk_push_uint(ctx, addr), 1;
}

JS_PARAM(inet_network, 1);
STATIC duk_ret_t
duke_inet_network(duk_context *ctx)
{
    char *name = (char *)duk_require_string(ctx, 0);

	duk_uint_t net = inet_network(name);

	return duk_push_uint(ctx, net), 1;
}

JS_PARAM(inet_ntoa, 1);
STATIC duk_ret_t
duke_inet_ntoa(duk_context *ctx)
{
    struct in_addr in;
    in.s_addr = duk_require_uint(ctx, 0);
    
	char *name = inet_ntoa(in);
	
	return js_push_string(ctx, name), 1;
}

JS_PARAM(inet_makeaddr, 2);
STATIC duk_ret_t
duke_inet_makeaddr(duk_context *ctx)
{
    duk_uint_t net = duk_require_uint(ctx, 0);
    duk_uint_t local = duk_require_uint(ctx, 1);

    struct in_addr in = inet_makeaddr(net, local);
	
	return duk_push_uint(ctx, in.s_addr), 1;
}

JS_PARAM(inet_lnaof, 1);
STATIC duk_ret_t
duke_inet_lnaof(duk_context *ctx)
{
    struct in_addr in;
    in.s_addr = duk_require_uint(ctx, 0);

	duk_uint_t host = inet_lnaof(in);
	
	return duk_push_uint(ctx, host), 1;
}

JS_PARAM(inet_netof, 1);
STATIC duk_ret_t
duke_inet_netof(duk_context *ctx)
{
    struct in_addr in;
    in.s_addr = duk_require_uint(ctx, 0);

	duk_uint_t host = inet_netof(in);
	
	return duk_push_uint(ctx, host), 1;
}

JS_PARAM(gethostbyname, 1);
STATIC duk_ret_t
duke_gethostbyname(duk_context *ctx)
{
    char *name = (char *)duk_require_string(ctx, 0);

    struct hostent *p = gethostbyname(name);

	return js_obj_push(ctx, libc_set_hostent, p), 1;
}

JS_PARAM(gethostbyname2, 2);
STATIC duk_ret_t
duke_gethostbyname2(duk_context *ctx)
{
    char *name = (char *)duk_require_string(ctx, 0);
    int af = duk_require_int(ctx, 1);
    
    struct hostent *p = gethostbyname2(name, af);

	return js_obj_push(ctx, libc_set_hostent, p), 1;
}

JS_PARAM(gethostbyaddr, 3);
STATIC duk_ret_t
duke_gethostbyaddr(duk_context *ctx)
{
    char *addr = (char *)duk_require_string(ctx, 0);
    int len = duk_require_int(ctx, 1);
    int af = duk_require_int(ctx, 2);
    
    struct hostent *p = gethostbyaddr(addr, len, af);

	return js_obj_push(ctx, libc_set_hostent, p), 1;
}

JS_PARAM(sethostent, 1);
STATIC duk_ret_t
duke_sethostent(duk_context *ctx)
{
    int stayopen = duk_require_int(ctx, 0);
    
	return sethostent(stayopen), 0;
}

JS_PARAM(gethostent, 0);
STATIC duk_ret_t
duke_gethostent(duk_context *ctx)
{
	struct hostent *p = gethostent();

	return js_obj_push(ctx, libc_set_hostent, p), 1;
}

JS_PARAM(endhostent, 0);
STATIC duk_ret_t
duke_endhostent(duk_context *ctx)
{
	return endhostent(), 0;
}

JS_PARAM(getservbyname, 2);
STATIC duk_ret_t
duke_getservbyname(duk_context *ctx)
{
    char *name = (char *)duk_require_string(ctx, 0);
    char *proto = (char *)duk_require_string(ctx, 1);
	
	struct servent *p = getservbyname(name, proto);

	return js_obj_push(ctx, libc_set_servent, p), 1;
}

JS_PARAM(getservbyport, 2);
STATIC duk_ret_t
duke_getservbyport(duk_context *ctx)
{
    int port = duk_require_int(ctx, 0);
    char *proto = (char *)duk_require_string(ctx, 1);
	
	struct servent *p = getservbyport(port, proto);

	return js_obj_push(ctx, libc_set_servent, p), 1;
}

JS_PARAM(setservent, 1);
STATIC duk_ret_t
duke_setservent(duk_context *ctx)
{
    int stayopen = duk_require_int(ctx, 0);
    
	return setservent(stayopen), 0;
}

JS_PARAM(getservent, 0);
STATIC duk_ret_t
duke_getservent(duk_context *ctx)
{
	struct servent *p = getservent();

	return js_obj_push(ctx, libc_set_servent, p), 1;
}

JS_PARAM(endservent, 0);
STATIC duk_ret_t
duke_endservent(duk_context *ctx)
{
	return endservent(), 0;
}

JS_PARAM(htons, 1);
STATIC duk_ret_t
duke_htons(duk_context *ctx)
{
	duk_uint_t n = duk_require_uint(ctx, 0);
    duk_uint_t v = htons(n);
	
	return duk_push_uint(ctx, v), 1;
}

JS_PARAM(ntohs, 1);
STATIC duk_ret_t
duke_ntohs(duk_context *ctx)
{
	duk_uint_t n = duk_require_uint(ctx, 0);
    duk_uint_t v = ntohs(n);
	
	return duk_push_uint(ctx, v), 1;
}

JS_PARAM(htonl, 1);
STATIC duk_ret_t
duke_htonl(duk_context *ctx)
{
	duk_uint_t n = duk_require_uint(ctx, 0);
    duk_uint_t v = htonl(n);
	
	return duk_push_uint(ctx, v), 1;
}

JS_PARAM(ntohl, 1);
STATIC duk_ret_t
duke_ntohl(duk_context *ctx)
{
	duk_uint_t n = duk_require_uint(ctx, 0);
    duk_uint_t v = ntohl(n);
	
	return duk_push_uint(ctx, v), 1;
}

JS_PARAM(getprotobyname, 1);
STATIC duk_ret_t
duke_getprotobyname(duk_context *ctx)
{
    char *name = (char *)duk_require_string(ctx, 0);
	
	struct protoent *p = getprotobyname(name);

	return js_obj_push(ctx, libc_set_protoent, p), 1;
}

JS_PARAM(getprotobynumber, 1);
STATIC duk_ret_t
duke_getprotobynumber(duk_context *ctx)
{
    int proto = duk_require_int(ctx, 0);
	
	struct protoent *p = getprotobynumber(proto);

	return js_obj_push(ctx, libc_set_protoent, p), 1;
}

JS_PARAM(setprotoent, 1);
STATIC duk_ret_t
duke_setprotoent(duk_context *ctx)
{
    int stayopen = duk_require_int(ctx, 0);
    
	return setprotoent(stayopen), 0;
}

JS_PARAM(getprotoent, 0);
STATIC duk_ret_t
duke_getprotoent(duk_context *ctx)
{
	struct protoent *p = getprotoent();

	return js_obj_push(ctx, libc_set_protoent, p), 1;
}

JS_PARAM(endprotoent, 0);
STATIC duk_ret_t
duke_endprotoent(duk_context *ctx)
{
	return endprotoent(), 0;
}

// 16.8 Opening and Closing Sockets
JS_PARAM(socket, 3);
STATIC duk_ret_t
duke_socket(duk_context *ctx)
{
    int domain  = duk_require_int(ctx, 0);
    int type    = duk_require_int(ctx, 1);
    int protocol= duk_require_int(ctx, 2);
    
    int fd = socket(domain, type, protocol);

    return js_push_error(ctx, fd), 1;
}

JS_PARAM(shutdown, 2);
STATIC duk_ret_t
duke_shutdown(duk_context *ctx)
{
    int fd  = duk_require_int(ctx, 0);
    int how = duk_require_int(ctx, 1);

    int err = shutdown(fd, how);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(socketpair, 4);
STATIC duk_ret_t
duke_socketpair(duk_context *ctx)
{
    int namespace = duk_require_int(ctx, 0);
    int style = duk_require_int(ctx, 1);
    int protocol = duk_require_int(ctx, 2);
    int fd[2] = {
        [0] = js_get_array_int(ctx, 3, 0),
        [1] = js_get_array_int(ctx, 3, 1),
    };

    int err = socketpair(namespace, style, protocol, fd);

    return js_push_error(ctx, err), 1;
}

// 16.9 Using Sockets with Connections
JS_PARAM(connect, 2);
STATIC duk_ret_t
duke_connect(duk_context *ctx)
{
    os_sockaddr_t sa = OS_SOCKADDR_ZERO();
    
    int fd = duk_require_int(ctx, 0);
    int err = libc_get_sockaddr(ctx, 1, &sa);
    if (err<0) {
        goto error;
    }
    
    err = connect(fd, &sa.c, os_sockaddr_len(&sa.c));

error:
    return js_push_error(ctx, err), 1;
}

JS_PARAM(listen, 2);
STATIC duk_ret_t
duke_listen(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    int backlog = duk_require_int(ctx, 1);

    int err = listen(fd, backlog);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(accept, 2);
STATIC duk_ret_t
duke_accept(duk_context *ctx)
{
    sockaddr_len_t len = sizeof(sockaddr_t);
    os_sockaddr_t sa = OS_SOCKADDR_ZERO();
    
    int fd = duk_require_int(ctx, 0);
    
    int err = accept(fd, &sa.c, &len);
    if (0==err) {
        libc_set_sockaddr(ctx, 1, &sa);
    }
    
    return js_push_error(ctx, err), 1;
}

JS_PARAM(getpeername, 2);
STATIC duk_ret_t
duke_getpeername(duk_context *ctx)
{
    sockaddr_len_t len = sizeof(sockaddr_t);
    os_sockaddr_t sa = OS_SOCKADDR_ZERO();
    
    int fd = duk_require_int(ctx, 0);
    
    int err = getpeername(fd, &sa.c, &len);
    if (0==err) { // 0 is ok
        libc_set_sockaddr(ctx, 1, &sa);
    }

    return js_push_error(ctx, err), 1;
}

JS_PARAM(send, 3);
STATIC duk_ret_t
duke_send(duk_context *ctx)
{
    duk_size_t bsize = 0;
    duk_buffer_t buf;
    
    int fd  = duk_require_int(ctx, 0);
    int err = duk_require_buffer_or_lstring(ctx, 1, &buf, &bsize);
    if (err<0) {
        __js_seterrno(ctx, err); goto error;
    }
    int flag= duk_require_int(ctx, 2);

    err = send(fd, buf, bsize, flag);
    if (err<0) {
        js_seterrno(ctx); goto error;
    }
    
error:
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(recv, 3);
STATIC duk_ret_t
duke_recv(duk_context *ctx)
{
    duk_size_t bsize = 0;
    int fd      = duk_require_int(ctx, 0);
    duk_buffer_t buf = duk_require_buffer_data(ctx, 1, &bsize);
    int flag    = duk_require_int(ctx, 2);
    
    int err = recv(fd, buf, bsize, flag);

    return js_push_error(ctx, err), 1;
}

// 16.10 Datagram Socket Operations
JS_PARAM(sendto, 4);
STATIC duk_ret_t
duke_sendto(duk_context *ctx)
{
    os_sockaddr_t sa = OS_SOCKADDR_ZERO();
    duk_size_t bsize = 0;
    duk_buffer_t buf;
    
    int fd      = duk_require_int(ctx, 0);
    int err     = duk_require_buffer_or_lstring(ctx, 1, &buf, &bsize);
    if (err<0) {
        __js_seterrno(ctx, err); goto error;
    }
    int flag    = duk_require_int(ctx, 2);
    err = libc_get_sockaddr(ctx, 3, &sa);
    if (err<0) {
        goto error;
    }
    
    err = sendto(fd, buf, bsize, flag, &sa.c, os_sockaddr_len(&sa.c));
    if (err<0) {
        js_seterrno(ctx); goto error;
    }
    
error:
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(recvfrom, 4);
STATIC duk_ret_t
duke_recvfrom(duk_context *ctx)
{
    sockaddr_len_t len = sizeof(sockaddr_t);
    os_sockaddr_t sa = OS_SOCKADDR_ZERO();
    duk_size_t bsize = 0;

    int fd      = duk_require_int(ctx, 0);
    duk_buffer_t buf = duk_require_buffer_data(ctx, 1, &bsize);
    int flag    = duk_require_int(ctx, 2);
    
    int err = recvfrom(fd, buf, bsize, flag, &sa.c, &len);
    if (err<0) {
        js_seterrno(ctx);
    } else {
        libc_set_sockaddr(ctx, 3, &sa);
    }

error:
    return duk_push_int(ctx, err), 1;
}

// 16.12 Socket Options
JS_PARAM(getsockopt, 4);
STATIC duk_ret_t
duke_getsockopt(duk_context *ctx)
{
	int fd      = duk_require_int(ctx, 0);
	int level   = duk_require_int(ctx, 1);
	int opt     = duk_require_int(ctx, 2);
    int err     = 0;
    int val     = 0;
    sockaddr_len_t size;
    
	switch(opt) {
        case SO_DEBUG:
        case SO_REUSEADDR:
        case SO_KEEPALIVE:
        case SO_DONTROUTE:
        case SO_BROADCAST:
        case SO_OOBINLINE:
        case SO_SNDBUF:
        case SO_RCVBUF:
        case SO_TYPE:
        case SO_ERROR:
            size = sizeof(val);
            err = getsockopt(fd, level, opt, &val, &size);
            if (err<0) {
                js_seterrno(ctx);
            } else {
                js_set_obj_int(ctx, 3, "optval", val);
            }

            break;
        case SO_LINGER: {
            struct linger linger;
            size = sizeof(linger);
            
            err = getsockopt(fd, level, opt, &linger, &size);
            if (err<0) {
                js_seterrno(ctx);
            } else {
                libc_set_linger(ctx, 3, &linger);
            }
            
        }   break;
        default:
            err = -ENOSUPPORT;

            break;
	}
	
	return duk_push_int(ctx, err), 1;
}

JS_PARAM(setsockopt, 4);
STATIC duk_ret_t
duke_setsockopt(duk_context *ctx)
{
	int fd      = duk_require_int(ctx, 0);
	int level   = duk_require_int(ctx, 1);
	int opt     = duk_require_int(ctx, 2);
    int err     = 0;
    int val     = 0;
    sockaddr_len_t size;
    
	switch(opt) {
        case SO_DEBUG:
        case SO_REUSEADDR:
        case SO_KEEPALIVE:
        case SO_DONTROUTE:
        case SO_BROADCAST:
        case SO_OOBINLINE:
        case SO_SNDBUF:
        case SO_RCVBUF:
            val = duk_require_int(ctx, 3);
            size = sizeof(val);
            err = setsockopt(fd, level, opt, &val, size);
            if (err<0) {
                js_seterrno(ctx);
            }
            
            break;
        case SO_LINGER: {
            struct linger linger;
            size = sizeof(linger);

            libc_get_linger(ctx, 3, &linger);
            
            err = setsockopt(fd, level, opt, &linger, size);
            if (err<0) {
                js_seterrno(ctx);
            }
            
        }   break;
        default:
            err = -ENOSUPPORT;
            break;
	}

    return duk_push_int(ctx, err), 1;
}

// 16.13 Networks Database
JS_PARAM(getnetbyname, 1);
STATIC duk_ret_t
duke_getnetbyname(duk_context *ctx)
{
	char *name = (char *)duk_require_string(ctx, 0);
	
    struct netent *ne = getnetbyname(name);

	return js_obj_push(ctx, libc_set_netent, ne), 1;
}

JS_PARAM(getnetbyaddr, 2);
STATIC duk_ret_t
duke_getnetbyaddr(duk_context *ctx)
{
	int net = duk_require_int(ctx, 0);
	int type= duk_require_int(ctx, 1);
	
    struct netent *ne = getnetbyaddr(net, type);

	return js_obj_push(ctx, libc_set_netent, ne), 1;
}

JS_PARAM(setnetent, 1);
STATIC duk_ret_t
duke_setnetent(duk_context *ctx)
{
	int stayopen = duk_require_int(ctx, 0);

	return setnetent(stayopen), 0;
}

JS_PARAM(getnetent, 0);
STATIC duk_ret_t
duke_getnetent(duk_context *ctx)
{
    struct netent *ne = getnetent();

	return js_obj_push(ctx, libc_set_netent, ne), 1;
}

JS_PARAM(endnetent, 0);
STATIC duk_ret_t
duke_endnetent(duk_context *ctx)
{
	return endnetent(), 0;
}

#if js_LIBC_TTY
// 17 Low-Level Terminal Interface
// 17.1 Identifying Terminals
JS_PARAM(isatty, 1);
STATIC duk_ret_t
duke_isatty(duk_context *ctx)
{
	int fd = duk_require_int(ctx, 0);

	return duk_push_bool(ctx, isatty(fd)), 1;
}

JS_PARAM(ttyname, 1);
STATIC duk_ret_t
duke_ttyname(duk_context *ctx)
{
	int fd = duk_require_int(ctx, 0);

    char *name = ttyname(fd);
    if (NULL==name) {
        js_seterrno(ctx);
    }
    
	return js_push_string(ctx, name), 1;
}

// 17.4 Terminal Modes
JS_PARAM(tcgetattr, 2);
STATIC duk_ret_t
duke_tcgetattr(duk_context *ctx)
{
    struct termios t;
    
	int fd = duk_require_int(ctx, 0);

    int err = tcgetattr(fd, &t);
    if (0==err) { // 0 is ok
        libc_set_termios(ctx, 1, &t);
    }
    
	return js_push_error(ctx, err), 1;
}

JS_PARAM(tcsetattr, 3);
STATIC duk_ret_t
duke_tcsetattr(duk_context *ctx)
{
    struct termios t;
    
	int fd  = duk_require_int(ctx, 0);
    int when= duk_require_int(ctx, 1);

    libc_get_termios(ctx, 2, &t);
    
    int err = tcsetattr(fd, when, &t);

	return js_push_error(ctx, err), 1;
}

JS_PARAM(cfgetospeed, 1);
STATIC duk_ret_t
duke_cfgetospeed(duk_context *ctx)
{
    struct termios t;
    
    libc_get_termios(ctx, 0, &t);
    
    speed_t speed = cfgetospeed(&t);
    
	return duk_push_uint(ctx, speed), 1;
}

JS_PARAM(cfgetispeed, 1);
STATIC duk_ret_t
duke_cfgetispeed(duk_context *ctx)
{
    struct termios t;
    
    libc_get_termios(ctx, 0, &t);
    
    speed_t speed = cfgetispeed(&t);
    
	return duk_push_uint(ctx, speed), 1;
}

JS_PARAM(cfsetospeed, 2);
STATIC duk_ret_t
duke_cfsetospeed(duk_context *ctx)
{
    struct termios t;
    
    libc_get_termios(ctx, 0, &t);
    speed_t speed = duk_require_uint(ctx, 1);

    int err = cfsetospeed(&t, speed);

	return js_push_error(ctx, err), 1;
}

JS_PARAM(cfsetispeed, 2);
STATIC duk_ret_t
duke_cfsetispeed(duk_context *ctx)
{
    struct termios t;
    
    libc_get_termios(ctx, 0, &t);
    speed_t speed = duk_require_uint(ctx, 1);

    int err = cfsetispeed(&t, speed);

	return js_push_error(ctx, err), 1;
}

JS_PARAM(cfsetspeed, 2);
STATIC duk_ret_t
duke_cfsetspeed(duk_context *ctx)
{
    struct termios t;
    
    libc_get_termios(ctx, 0, &t);
    speed_t speed = duk_require_uint(ctx, 1);

    int err = cfsetspeed(&t, speed);

	return js_push_error(ctx, err), 1;
}

JS_PARAM(cfmakeraw, 1);
STATIC duk_ret_t
duke_cfmakeraw(duk_context *ctx)
{
    struct termios t;
    
    libc_get_termios(ctx, 0, &t);

    return cfmakeraw(&t), 0;
}

// 17.6 Line Control Functions
JS_PARAM(tcsendbreak, 2);
STATIC duk_ret_t
duke_tcsendbreak(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    int du = duk_require_int(ctx, 1);

    int err = tcsendbreak(fd, du);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(tcdrain, 1);
STATIC duk_ret_t
duke_tcdrain(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);

    int err = tcdrain(fd);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(tcflush, 2);
STATIC duk_ret_t
duke_tcflush(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    int q  = duk_require_int(ctx, 1);

    int err = tcflush(fd, q);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(tcflow, 2);
STATIC duk_ret_t
duke_tcflow(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    int act= duk_require_int(ctx, 1);

    int err = tcflow(fd, act);

    return js_push_error(ctx, err), 1;
}

// 17.8 Pseudo-Terminals
JS_PARAM(getpt, 0);
STATIC duk_ret_t
duke_getpt(duk_context *ctx)
{
    int err = getpt();

    return js_push_error(ctx, err), 1;
}

JS_PARAM(grantpt, 1);
STATIC duk_ret_t
duke_grantpt(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);

    int err = grantpt(fd);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(unlockpt, 1);
STATIC duk_ret_t
duke_unlockpt(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);

    int err = unlockpt(fd);
    
    return js_push_error(ctx, err), 1;
}

JS_PARAM(ptsname, 1);
STATIC duk_ret_t
duke_ptsname(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);

    char *name = ptsname(fd);
    if (NULL==name) {
        js_seterrno(ctx);
    }
    
    return js_push_string(ctx, name), 1;
}
#endif /* js_LIBC_TTY */

#if js_LIBC_LOG
// 18 Syslog
// 18.2 Submitting Syslog Messages
JS_PARAM(openlog, 3);
STATIC duk_ret_t
duke_openlog(duk_context *ctx)
{
    char *ident = (char *)duk_require_string(ctx, 0);
    int opt     = duk_require_int(ctx, 1);
    int facility= duk_require_int(ctx, 2);

    return openlog(ident, opt, facility), 0;
}

JS_PARAM(syslog, 2);
STATIC duk_ret_t
duke_syslog(duk_context *ctx)
{
    int facility_priority = duk_require_int(ctx, 0);
    char *string = (char *)duk_require_string(ctx, 1);

    return syslog(facility_priority, "%s", string), 0;
}

JS_PARAM(closelog, 0);
STATIC duk_ret_t
duke_closelog(duk_context *ctx)
{
    return closelog(), 0;
}

// 18.2.4 setlogmask
JS_PARAM(setlogmask, 1);
STATIC duk_ret_t
duke_setlogmask(duk_context *ctx)
{
    int mask = duk_require_int(ctx, 0);

    return duk_push_int(ctx, setlogmask(mask)), 1;
}

JS_PARAM(LOG_MASK, 1);
STATIC duk_ret_t
duke_LOG_MASK(duk_context *ctx)
{
    int mask = duk_require_int(ctx, 0);

    return duk_push_int(ctx, LOG_MASK(mask)), 1;
}

JS_PARAM(LOG_UPTO, 1);
STATIC duk_ret_t
duke_LOG_UPTO(duk_context *ctx)
{
    int mask = duk_require_int(ctx, 0);

    return duk_push_int(ctx, LOG_UPTO(mask)), 1;
}
#endif /* js_LIBC_LOG */

#if js_LIBC_MATH
// 19 Mathematics
// 19.2 Trigonometric Functions
JS_PARAM(sin, 1);
STATIC duk_ret_t
duke_sin(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, sin(x)), 1;
}

JS_PARAM(cos, 1);
STATIC duk_ret_t
duke_cos(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, cos(x)), 1;
}

JS_PARAM(tan, 1);
STATIC duk_ret_t
duke_tan(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, tan(x)), 1;
}

// 19.3 Inverse Trigonometric Functions
JS_PARAM(asin, 1);
STATIC duk_ret_t
duke_asin(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, asin(x)), 1;
}

JS_PARAM(acos, 1);
STATIC duk_ret_t
duke_acos(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, acos(x)), 1;
}

JS_PARAM(atan, 1);
STATIC duk_ret_t
duke_atan(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, atan(x)), 1;
}

JS_PARAM(atan2, 2);
STATIC duk_ret_t
duke_atan2(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);

    return duk_push_number(ctx, atan2(x, y)), 1;
}

// 19.4 Exponentiation and Logarithms
JS_PARAM(exp, 1);
STATIC duk_ret_t
duke_exp(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, exp(x)), 1;
}

JS_PARAM(exp2, 1);
STATIC duk_ret_t
duke_exp2(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, exp2(x)), 1;
}

#ifndef __OPENWRT__
JS_PARAM(exp10, 1);
STATIC duk_ret_t
duke_exp10(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, exp10(x)), 1;
}
#endif

JS_PARAM(log, 1);
STATIC duk_ret_t
duke_log(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, log(x)), 1;
}

JS_PARAM(log2, 1);
STATIC duk_ret_t
duke_log2(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, log2(x)), 1;
}

JS_PARAM(log10, 1);
STATIC duk_ret_t
duke_log10(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, log10(x)), 1;
}

JS_PARAM(logb, 1);
STATIC duk_ret_t
duke_logb(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, logb(x)), 1;
}

JS_PARAM(ilogb, 1);
STATIC duk_ret_t
duke_ilogb(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_int(ctx, ilogb(x)), 1;
}

JS_PARAM(pow, 2);
STATIC duk_ret_t
duke_pow(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);

    return duk_push_number(ctx, pow(x, y)), 1;
}

JS_PARAM(sqrt, 1);
STATIC duk_ret_t
duke_sqrt(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_int(ctx, sqrt(x)), 1;
}

JS_PARAM(cbrt, 1);
STATIC duk_ret_t
duke_cbrt(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, cbrt(x)), 1;
}

JS_PARAM(hypot, 2);
STATIC duk_ret_t
duke_hypot(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);

    return duk_push_number(ctx, hypot(x, y)), 1;
}

JS_PARAM(expm1, 1);
STATIC duk_ret_t
duke_expm1(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, expm1(x)), 1;
}

JS_PARAM(log1p, 1);
STATIC duk_ret_t
duke_log1p(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, log1p(x)), 1;
}

// 19.5 Hyperbolic Functions
JS_PARAM(sinh, 1);
STATIC duk_ret_t
duke_sinh(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, sinh(x)), 1;
}

JS_PARAM(cosh, 1);
STATIC duk_ret_t
duke_cosh(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, cosh(x)), 1;
}

JS_PARAM(tanh, 1);
STATIC duk_ret_t
duke_tanh(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, tanh(x)), 1;
}

JS_PARAM(asinh, 1);
STATIC duk_ret_t
duke_asinh(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, asinh(x)), 1;
}

JS_PARAM(acosh, 1);
STATIC duk_ret_t
duke_acosh(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, acosh(x)), 1;
}

JS_PARAM(atanh, 1);
STATIC duk_ret_t
duke_atanh(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, atanh(x)), 1;
}

// 19.6 Special Functions
JS_PARAM(erf, 1);
STATIC duk_ret_t
duke_erf(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, erf(x)), 1;
}

JS_PARAM(erfc, 1);
STATIC duk_ret_t
duke_erfc(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, erfc(x)), 1;
}

JS_PARAM(lgamma, 1);
STATIC duk_ret_t
duke_lgamma(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, lgamma(x)), 1;
}

JS_PARAM(gamma, 1);
STATIC duk_ret_t
duke_gamma(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, gamma(x)), 1;
}

JS_PARAM(tgamma, 1);
STATIC duk_ret_t
duke_tgamma(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, tgamma(x)), 1;
}

#ifndef __OPENWRT__
JS_PARAM(j0, 1);
STATIC duk_ret_t
duke_j0(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, j0(x)), 1;
}

JS_PARAM(j1, 1);
STATIC duk_ret_t
duke_j1(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, j1(x)), 1;
}

JS_PARAM(jn, 2);
STATIC duk_ret_t
duke_jn(duk_context *ctx)
{
    int    n = duk_require_int(ctx, 0);
    double x = duk_require_number(ctx, 1);

    return duk_push_number(ctx, jn(n, x)), 1;
}

JS_PARAM(y0, 1);
STATIC duk_ret_t
duke_y0(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, y0(x)), 1;
}

JS_PARAM(y1, 1);
STATIC duk_ret_t
duke_y1(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, y1(x)), 1;
}

JS_PARAM(yn, 2);
STATIC duk_ret_t
duke_yn(duk_context *ctx)
{
    int    n = duk_require_int(ctx, 0);
    double x = duk_require_number(ctx, 1);

    return duk_push_number(ctx, yn(n, x)), 1;
}
#endif

// 19.8 Pseudo-Random Numbers
// 19.8.1 ISO C Random Number Functions
JS_PARAM(rand, 0);
STATIC duk_ret_t
duke_rand(duk_context *ctx)
{
    return duk_push_int(ctx, rand()), 1;
}

JS_PARAM(srand, 1);
STATIC duk_ret_t
duke_srand(duk_context *ctx)
{
    duk_uint_t seed = duk_require_uint(ctx, 0);
    
    return srand(seed), 0;
}

// 20 Arithmetic Functions
// 20.2 Integer Division
JS_PARAM(div, 2);
STATIC duk_ret_t
duke_div(duk_context *ctx)
{
    int numerator   = duk_require_int(ctx, 0);
    int denominator = duk_require_int(ctx, 1);

    div_t d = div(numerator, denominator);
    
    return js_obj_push(ctx, libc_set_div, &d), 1;
}

// 20.4 Floating-Point Number Classification Functions
JS_PARAM(fpclassify, 1);
STATIC duk_ret_t
duke_fpclassify(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_int(ctx, fpclassify(x)), 1;
}

JS_PARAM(isfinite, 1);
STATIC duk_ret_t
duke_isfinite(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_bool(ctx, isfinite(x)), 1;
}

JS_PARAM(isnormal, 1);
STATIC duk_ret_t
duke_isnormal(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_bool(ctx, isnormal(x)), 1;
}

JS_PARAM(isnan, 1);
STATIC duk_ret_t
duke_isnan(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_bool(ctx, isnan(x)), 1;
}

JS_PARAM(isinf, 1);
STATIC duk_ret_t
duke_isinf(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_bool(ctx, isinf(x)), 1;
}

// 20.8 Arithmetic Functions
JS_PARAM(abs, 1);
STATIC duk_ret_t
duke_abs(duk_context *ctx)
{
    int x = duk_require_int(ctx, 0);
    
    return duk_push_int(ctx, abs(x)), 1;
}

JS_PARAM(fabs, 1);
STATIC duk_ret_t
duke_fabs(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_number(ctx, fabs(x)), 1;
}

JS_PARAM(frexp, 2);
STATIC duk_ret_t
duke_frexp(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    int exponent;
    
    double n = frexp(x, &exponent);

    js_set_obj_int(ctx, 1, "exponent", exponent);
    
    return duk_push_number(ctx, n), 1;
}

JS_PARAM(ldexp, 2);
STATIC duk_ret_t
duke_ldexp(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    int exponent = duk_require_int(ctx, 1);
    
    return duk_push_number(ctx, ldexp(x, exponent)), 1;
}

JS_PARAM(scalb, 2);
STATIC duk_ret_t
duke_scalb(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    int exponent = duk_require_int(ctx, 1);
    
    return duk_push_number(ctx, scalb(x, exponent)), 1;
}

JS_PARAM(ceil, 1);
STATIC duk_ret_t
duke_ceil(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_number(ctx, ceil(x)), 1;
}

JS_PARAM(floor, 1);
STATIC duk_ret_t
duke_floor(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_number(ctx, floor(x)), 1;
}

JS_PARAM(trunc, 1);
STATIC duk_ret_t
duke_trunc(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_number(ctx, trunc(x)), 1;
}

JS_PARAM(rint, 1);
STATIC duk_ret_t
duke_rint(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_number(ctx, rint(x)), 1;
}

JS_PARAM(nearbyint, 1);
STATIC duk_ret_t
duke_nearbyint(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_number(ctx, nearbyint(x)), 1;
}

JS_PARAM(round, 1);
STATIC duk_ret_t
duke_round(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_number(ctx, round(x)), 1;
}

JS_PARAM(fmod, 2);
STATIC duk_ret_t
duke_fmod(duk_context *ctx)
{
    double numerator    = duk_require_number(ctx, 0);
    double denominator  = duk_require_number(ctx, 1);
    
    return duk_push_number(ctx, fmod(numerator, denominator)), 1;
}

JS_PARAM(drem, 2);
STATIC duk_ret_t
duke_drem(duk_context *ctx)
{
    double numerator    = duk_require_number(ctx, 0);
    double denominator  = duk_require_number(ctx, 1);
    
    return duk_push_number(ctx, drem(numerator, denominator)), 1;
}

JS_PARAM(remainder, 2);
STATIC duk_ret_t
duke_remainder(duk_context *ctx)
{
    double numerator    = duk_require_number(ctx, 0);
    double denominator  = duk_require_number(ctx, 1);
    
    return duk_push_number(ctx, remainder(numerator, denominator)), 1;
}

JS_PARAM(copysign, 2);
STATIC duk_ret_t
duke_copysign(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    
    return duk_push_number(ctx, copysign(x, y)), 1;
}

JS_PARAM(signbit, 1);
STATIC duk_ret_t
duke_signbit(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_number(ctx, signbit(x)), 1;
}

JS_PARAM(nextafter, 2);
STATIC duk_ret_t
duke_nextafter(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    
    return duk_push_number(ctx, nextafter(x, y)), 1;
}

#ifndef __OPENWRT__
JS_PARAM(nexttoward, 2);
STATIC duk_ret_t
duke_nexttoward(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    
    return duk_push_number(ctx, nexttoward(x, y)), 1;
}
#endif

JS_PARAM(nan, 1);
STATIC duk_ret_t
duke_nan(duk_context *ctx)
{
    char *tagp = (char *)duk_require_string(ctx, 0);
    
    return duk_push_number(ctx, nan(tagp)), 1;
}

// 20.8.6 Floating-Point Comparison Functions
JS_PARAM(isgreater, 2);
STATIC duk_ret_t
duke_isgreater(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    
    return duk_push_bool(ctx, isgreater(x, y)), 1;
}

JS_PARAM(isgreaterequal, 2);
STATIC duk_ret_t
duke_isgreaterequal(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    
    return duk_push_bool(ctx, isgreaterequal(x, y)), 1;
}

JS_PARAM(isless, 2);
STATIC duk_ret_t
duke_isless(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    
    return duk_push_bool(ctx, isless(x, y)), 1;
}

JS_PARAM(islessequal, 2);
STATIC duk_ret_t
duke_islessequal(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    
    return duk_push_bool(ctx, islessequal(x, y)), 1;
}

JS_PARAM(islessgreater, 2);
STATIC duk_ret_t
duke_islessgreater(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    
    return duk_push_bool(ctx, islessgreater(x, y)), 1;
}

JS_PARAM(isunordered, 2);
STATIC duk_ret_t
duke_isunordered(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    
    return duk_push_bool(ctx, isunordered(x, y)), 1;
}

// 20.8.7 Miscellaneous FP arithmetic functions
JS_PARAM(fmin, 2);
STATIC duk_ret_t
duke_fmin(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    
    return duk_push_number(ctx, fmin(x, y)), 1;
}

JS_PARAM(fmax, 2);
STATIC duk_ret_t
duke_fmax(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    
    return duk_push_number(ctx, fmax(x, y)), 1;
}

JS_PARAM(fdim, 2);
STATIC duk_ret_t
duke_fdim(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    
    return duk_push_number(ctx, fdim(x, y)), 1;
}

JS_PARAM(fma, 3);
STATIC duk_ret_t
duke_fma(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    double z = duk_require_number(ctx, 2);
    
    return duk_push_number(ctx, fma(x, y, z)), 1;
}
#endif /* js_LIBC_MATH */

#if js_LIBC_TIME
// 21 Date and Time
// 21.2 Elapsed Time
JS_PARAM(difftime, 2);
STATIC duk_ret_t
duke_difftime(duk_context *ctx)
{
    time_t time1 = duk_require_uint(ctx, 0);
    time_t time0 = duk_require_uint(ctx, 1);
    
    return duk_push_number(ctx, difftime(time1, time0)), 1;
}

// 21.3.1 CPU Time Inquiry
JS_PARAM(clock, 0);
STATIC duk_ret_t
duke_clock(duk_context *ctx)
{
    return duk_push_uint(ctx, clock()), 1;
}

JS_PARAM(times, 1);
STATIC duk_ret_t
duke_times(duk_context *ctx)
{
    struct tms t;

    libc_get_tms(ctx, 0, &t);

    return duk_push_uint(ctx, times(&t)), 1;
}

// 21.4 Calendar Time
JS_PARAM(time, 0);
STATIC duk_ret_t
duke_time(duk_context *ctx)
{
    return duk_push_uint(ctx, time(NULL)), 1;
}

JS_PARAM(gettimeofday, 2);
STATIC duk_ret_t
duke_gettimeofday(duk_context *ctx)
{
    struct timeval tv;
    struct timezone tz;

    int err = gettimeofday(&tv, &tz);

    libc_set_timeval(ctx, 0, &tv);
    libc_set_timezone(ctx, 1, &tz);
    
    return js_push_error(ctx, err), 1;
}

JS_PARAM(settimeofday, 2);
STATIC duk_ret_t
duke_settimeofday(duk_context *ctx)
{
    struct timeval tv;
    struct timezone tz;

    libc_get_timeval(ctx, 0, &tv);
    libc_get_timezone(ctx, 1, &tz);

    int err = settimeofday(&tv, &tz);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(adjtime, 2);
STATIC duk_ret_t
duke_adjtime(duk_context *ctx)
{
    struct timeval delta, old;

    libc_get_timeval(ctx, 0, &delta);

    int err = adjtime(&delta, &old);

    libc_set_timeval(ctx, 1, &old);
    
    return js_push_error(ctx, err), 1;
}

// 21.4.3 Broken-down Time
JS_PARAM(localtime, 1);
STATIC duk_ret_t
duke_localtime(duk_context *ctx)
{
    time_t time = duk_require_uint(ctx, 0);

    struct tm *tm = localtime(&time);

    return js_obj_push(ctx, libc_set_tm, tm), 1;
}

JS_PARAM(gmtime, 1);
STATIC duk_ret_t
duke_gmtime(duk_context *ctx)
{
    time_t time = duk_require_uint(ctx, 0);

    struct tm *tm = gmtime(&time);

    return js_obj_push(ctx, libc_set_tm, tm), 1;
}

JS_PARAM(mktime, 1);
STATIC duk_ret_t
duke_mktime(duk_context *ctx)
{
    struct tm tm;

    libc_get_tm(ctx, 0, &tm);
    
    time_t time = mktime(&tm);
    
    return duk_push_uint(ctx, time), 1;
}

JS_PARAM(timelocal, 1);
STATIC duk_ret_t
duke_timelocal(duk_context *ctx)
{
    struct tm tm;

    libc_get_tm(ctx, 0, &tm);
    
    time_t time = timelocal(&tm);
    
    return duk_push_uint(ctx, time), 1;
}

JS_PARAM(timegm, 1);
STATIC duk_ret_t
duke_timegm(duk_context *ctx)
{
    struct tm tm;

    libc_get_tm(ctx, 0, &tm);
    
    time_t time = timegm(&tm);
    
    return duk_push_uint(ctx, time), 1;
}

// 21.4.4 High Accuracy Clock
#ifndef __OPENWRT__
JS_PARAM(ntp_gettime, 1);
STATIC duk_ret_t
duke_ntp_gettime(duk_context *ctx)
{
    struct ntptimeval ntv;

    int err = ntp_gettime(&ntv);

    libc_set_ntptimeval(ctx, 0, &ntv);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(ntp_adjtime, 1);
STATIC duk_ret_t
duke_ntp_adjtime(duk_context *ctx)
{
    struct timex tx;

    int err = ntp_adjtime(&tx);

    libc_set_timex(ctx, 0, &tx);

    return js_push_error(ctx, err), 1;
}
#endif

// 21.4.5 Formatting Calendar Time
JS_PARAM(asctime, 1);
STATIC duk_ret_t
duke_asctime(duk_context *ctx)
{
    struct tm tm;

    libc_get_tm(ctx, 0, &tm);
    
    char *time = asctime(&tm);

    return js_push_string(ctx, time), 1;
}

JS_PARAM(ctime, 1);
STATIC duk_ret_t
duke_ctime(duk_context *ctx)
{
    time_t t = duk_require_uint(ctx, 0);

    char *time = ctime(&t);

    return js_push_string(ctx, time), 1;
}

JS_PARAM(strftime, 2);
STATIC duk_ret_t
duke_strftime(duk_context *ctx)
{
    struct tm tm;
    int size = OS_LINE_LEN;
    char *line = NULL;
    
    char *template = (char *)duk_require_string(ctx, 0);
    libc_get_tm(ctx, 1, &tm);


    while(1) {
        line = (char *)os_realloc(line, 1+size);
        
        size_t len = strftime(line, OS_LINE_LEN, template, &tm);
        if (len) {
            line[len] = 0;
            js_push_string(ctx, line);

            break;
        }
        
        size += 1+OS_LINE_LEN;
    }

    os_free(line);
    return 1;
}

// 21.5 Setting an Alarm
JS_PARAM(setitimer, 3);
STATIC duk_ret_t
duke_setitimer(duk_context *ctx)
{
    struct itimerval new, old;
    int which = duk_require_int(ctx, 0);

    libc_get_itimerval(ctx, 1, &new);
    int err = setitimer(which, &new, &old);
    if (0==err) { // 0 is ok
        libc_set_itimerval(ctx, 2, &old);
    }
    
    return js_push_error(ctx, err), 1;
}

JS_PARAM(getitimer, 2);
STATIC duk_ret_t
duke_getitimer(duk_context *ctx)
{
    struct itimerval old;
    int which = duk_require_int(ctx, 0);

    int err = getitimer(which, &old);
    if (0==err) { // 0 is ok
        libc_set_itimerval(ctx, 1, &old);
    }
    
    return js_push_error(ctx, err), 1;
}

JS_PARAM(alarm, 1);
STATIC duk_ret_t
duke_alarm(duk_context *ctx)
{
    duk_uint_t sec = duk_require_uint(ctx, 0);

    return duk_push_uint(ctx, alarm(sec)), 1;
}

// 21.6 Sleeping
JS_PARAM(sleep, 1);
STATIC duk_ret_t
duke_sleep(duk_context *ctx)
{
    duk_uint_t sec = duk_require_uint(ctx, 0);

    return duk_push_uint(ctx, sleep(sec)), 1;
}

JS_PARAM(nanosleep, 2);
STATIC duk_ret_t
duke_nanosleep(duk_context *ctx)
{
    struct timespec req, remain;
    duk_uint_t sec = duk_require_uint(ctx, 0);

    libc_get_timespec(ctx, 1, &req);
    int err = nanosleep(&req, &remain);
    if (0==err) { // 0 is ok
        libc_set_timespec(ctx, 2, &remain);
    }
    
    return js_push_error(ctx, err), 1;
}
#endif /* js_LIBC_TIME */

// 22 Resource Usage And Limitation
JS_PARAM(getrusage, 2);
STATIC duk_ret_t
duke_getrusage(duk_context *ctx)
{
    struct rusage u;
    int processes = duk_require_int(ctx, 0);

    int err = getrusage(processes, &u);
    if (0==err) { // 0 is ok
        libc_set_rusage(ctx, 1, &u);
    }
    
    return js_push_error(ctx, err), 1;
}

#if js_LIBC_VTIME
JS_PARAM(vtimes, 2);
STATIC duk_ret_t
duke_vtimes(duk_context *ctx)
{
    struct vtimes current, child;

    int err = vtimes(&current, &child);
    if (0==err) { // ???
        libc_set_vtimes(ctx, 0, &current);
        libc_set_vtimes(ctx, 1, &child);
    }
    
    return js_push_error(ctx, err), 1;
}
#endif

// 22.2 Limiting Resource Usage
JS_PARAM(getrlimit, 2);
STATIC duk_ret_t
duke_getrlimit(duk_context *ctx)
{
    struct rlimit r;
    int resource = duk_require_int(ctx, 0);

    int err = getrlimit(resource, &r);
    if (0==err) { // 0 is ok
        libc_set_rlimit(ctx, 1, &r);
    }
    
    return js_push_error(ctx, err), 1;
}

JS_PARAM(setrlimit, 2);
STATIC duk_ret_t
duke_setrlimit(duk_context *ctx)
{
    struct rlimit r;
    int resource = duk_require_int(ctx, 0);

    libc_get_rlimit(ctx, 1, &r);
    int err = setrlimit(resource, &r);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(ulimit, DUK_VARARGS);
STATIC duk_ret_t
duke_ulimit(duk_context *ctx)
{
    int cmd = duk_require_int(ctx, 0);
    int limit;
    int err = -ENOSUPPORT;
    
    switch(cmd) {
        case UL_GETFSIZE:
            err = ulimit(cmd);
            
            break;
        case UL_SETFSIZE:
            limit = duk_require_int(ctx, 1);
            err = ulimit(cmd, limit);
            
            break;
        default:
            break;
    }

    return js_push_error(ctx, err), 1;
}

#ifndef __OPENWRT__
JS_PARAM(vlimit, 2);
STATIC duk_ret_t
duke_vlimit(duk_context *ctx)
{
    int resource = duk_require_int(ctx, 0);
    int limit = duk_require_int(ctx, 1);

    int err = vlimit(resource, limit);
    
    return js_push_error(ctx, err), 1;
}
#endif

// 22.3 Process CPU Priority And Scheduling
JS_PARAM(sched_setscheduler, 3);
STATIC duk_ret_t
duke_sched_setscheduler(duk_context *ctx)
{
    struct sched_param param;
    
    pid_t pid = duk_require_int(ctx, 0);
    int policy = duk_require_int(ctx, 1);
    libc_get_sched_param(ctx, 2, &param);
    
    int err = sched_setscheduler(pid, policy, &param);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(sched_getscheduler, 1);
STATIC duk_ret_t
duke_sched_getscheduler(duk_context *ctx)
{
    pid_t pid = duk_require_int(ctx, 0);
    
    int err = sched_getscheduler(pid);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(sched_setparam, 2);
STATIC duk_ret_t
duke_sched_setparam(duk_context *ctx)
{
    struct sched_param param;
    
    pid_t pid = duk_require_int(ctx, 0);
    libc_get_sched_param(ctx, 1, &param);
    
    int err = sched_setparam(pid, &param);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(sched_getparam, 2);
STATIC duk_ret_t
duke_sched_getparam(duk_context *ctx)
{
    struct sched_param param;
    
    pid_t pid = duk_require_int(ctx, 0);
    
    int err = sched_getparam(pid, &param);
    if (0==err) { // 0 is ok
        libc_set_sched_param(ctx, 1, &param);
    }
    
    return js_push_error(ctx, err), 1;
}

JS_PARAM(sched_get_priority_min, 1);
STATIC duk_ret_t
duke_sched_get_priority_min(duk_context *ctx)
{
    int policy = duk_require_int(ctx, 0);

    int err = sched_get_priority_min(policy);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(sched_get_priority_max, 1);
STATIC duk_ret_t
duke_sched_get_priority_max(duk_context *ctx)
{
    int policy = duk_require_int(ctx, 0);

    int err = sched_get_priority_max(policy);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(sched_rr_get_interval, 2);
STATIC duk_ret_t
duke_sched_rr_get_interval(duk_context *ctx)
{
    struct timespec interval;
    pid_t pid = duk_require_int(ctx, 0);

    int err = sched_rr_get_interval(pid, &interval);
    if (0==err) { // 0 is ok
        libc_set_timespec(ctx, 1, &interval);
    }
    
    return js_push_error(ctx, err), 1;
}

JS_PARAM(sched_yield, 0);
STATIC duk_ret_t
duke_sched_yield(duk_context *ctx)
{
    int err = sched_yield();

    return js_push_error(ctx, err), 1;
}

JS_PARAM(getpriority, 2);
STATIC duk_ret_t
duke_getpriority(duk_context *ctx)
{
    int class = duk_require_uint(ctx, 0);
    int id = duk_require_uint(ctx, 1);
    
    int err = getpriority(class, id);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(setpriority, 3);
STATIC duk_ret_t
duke_setpriority(duk_context *ctx)
{
    int class = duk_require_uint(ctx, 0);
    int id = duk_require_uint(ctx, 1);
    int niceval = duk_require_uint(ctx, 2);
    
    int err = setpriority(class, id, niceval);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(nice, 1);
STATIC duk_ret_t
duke_nice(duk_context *ctx)
{
    int increment = duk_require_uint(ctx, 0);

    int err = nice(increment);

    return js_push_error(ctx, err), 1;
}

#ifdef _GNU_SOURCE
JS_PARAM(CPU_NEW, 0);
STATIC duk_ret_t
duke_CPU_NEW(duk_context *ctx)
{
    return js_push_dynamic_buffer(ctx, sizeof(cpu_set_t)), 1;
}

JS_PARAM(CPU_ZERO, 1);
STATIC duk_ret_t
duke_CPU_ZERO(duk_context *ctx)
{
    int err = 0;
    duk_size_t bsize;
    
    cpu_set_t *set = (cpu_set_t *)duk_require_buffer_data(ctx, 0, &bsize);
    if (bsize!=sizeof(cpu_set_t)) {
        err = __js_seterrno(ctx, -EINVAL1); goto error;
    }
    CPU_ZERO(set);
    
error:
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(CPU_SET, 2);
STATIC duk_ret_t
duke_CPU_SET(duk_context *ctx)
{
    int err = 0;
    duk_size_t bsize;
    
    int cpu = duk_require_int(ctx, 0);
    cpu_set_t *set = (cpu_set_t *)duk_require_buffer_data(ctx, 1, &bsize);
    if (bsize!=sizeof(cpu_set_t)) {
        err = __js_seterrno(ctx, -EINVAL2); goto error;
    }
    CPU_SET(cpu, set);
    
error:
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(CPU_CLR, 2);
STATIC duk_ret_t
duke_CPU_CLR(duk_context *ctx)
{
    int err = 0;
    duk_size_t bsize;
    
    int cpu = duk_require_int(ctx, 0);
    cpu_set_t *set = (cpu_set_t *)duk_require_buffer_data(ctx, 1, &bsize);
    if (bsize!=sizeof(cpu_set_t)) {
        err = __js_seterrno(ctx, -EINVAL2); goto error;
    }
    CPU_CLR(cpu, set);
    
error:
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(CPU_ISSET, 2);
STATIC duk_ret_t
duke_CPU_ISSET(duk_context *ctx)
{
    int err;
    duk_size_t bsize;
    
    int cpu = duk_require_int(ctx, 0);
    cpu_set_t *set = (cpu_set_t *)duk_require_buffer_data(ctx, 1, &bsize);
    if (bsize!=sizeof(cpu_set_t)) {
        err = __js_seterrno(ctx, -EINVAL2); return duk_push_false(ctx), 1;
    }
    
    return duk_push_bool(ctx, CPU_ISSET(cpu, set)), 1;
}

JS_PARAM(sched_getaffinity, 3);
STATIC duk_ret_t
duke_sched_getaffinity(duk_context *ctx)
{
    int err;
    duk_size_t bsize;
    
    pid_t pid = duk_require_int(ctx, 0);
    size_t size = duk_require_uint(ctx, 1);
    cpu_set_t *set = (cpu_set_t *)duk_require_buffer_data(ctx, 1, &bsize);
    if (bsize!=sizeof(cpu_set_t)) {
        err = __js_seterrno(ctx, -EINVAL3); return duk_push_false(ctx), 1;
    }

    err = sched_getaffinity(pid, size, set);
    if (err<0) {
        js_seterrno(ctx);
    }

error:
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(sched_setaffinity, 3);
STATIC duk_ret_t
duke_sched_setaffinity(duk_context *ctx)
{
    int err;
    duk_size_t bsize;
    
    pid_t pid = duk_require_int(ctx, 0);
    size_t size = duk_require_uint(ctx, 1);
    cpu_set_t *set = (cpu_set_t *)duk_require_buffer_data(ctx, 1, &bsize);
    if (bsize!=sizeof(cpu_set_t)) {
        err = __js_seterrno(ctx, -EINVAL3); return duk_push_false(ctx), 1;
    }

    err = sched_setaffinity(pid, size, set);
    if (err<0) {
        js_seterrno(ctx);
    }

error:
    return duk_push_int(ctx, err), 1;
}
#endif /* _GNU_SOURCE */

// 22.4 Querying memory available resources
JS_PARAM(getpagesize, 0);
STATIC duk_ret_t
duke_getpagesize(duk_context *ctx)
{
    return duk_push_int(ctx, getpagesize()), 1;
}

#ifndef __OPENWRT__
JS_PARAM(get_phys_pages, 0);
STATIC duk_ret_t
duke_get_phys_pages(duk_context *ctx)
{
    return duk_push_int(ctx, get_phys_pages()), 1;
}

JS_PARAM(get_avphys_pages, 0);
STATIC duk_ret_t
duke_get_avphys_pages(duk_context *ctx)
{
    return duk_push_int(ctx, get_avphys_pages()), 1;
}
#endif

// 22.5 Learn about the processors available
JS_PARAM(get_nprocs_conf, 0);
STATIC duk_ret_t
duke_get_nprocs_conf(duk_context *ctx)
{
    return duk_push_int(ctx, get_nprocs_conf()), 1;
}

JS_PARAM(get_nprocs, 0);
STATIC duk_ret_t
duke_get_nprocs(duk_context *ctx)
{
    return duk_push_int(ctx, get_nprocs()), 1;
}

#ifndef __OPENWRT__
JS_PARAM(getloadavg, 1);
STATIC duk_ret_t
duke_getloadavg(duk_context *ctx)
{
    double loadavg[3];

    int err = getloadavg(loadavg, os_count_of(loadavg));
    if (err>0) {
        int i;
        
        for (i=0; i<os_count_of(loadavg); i++) {
            js_set_array_number(ctx, 0, i, loadavg[i]);
        }
    }
    
    return js_push_error(ctx, err), 1;
}
#endif

// 23 Non-Local Exits
// jmp_buf/setjmp/longjmp
// sigjmp_buf/sigsetjmp/siglongjmp
// ucontext_t/getcontext/makecontext/setcontext/swapcontext

#if js_LIBC_SIG
// 24 Signal Handling
// 24.2.8 Signal Messages
JS_PARAM(strsignal, 1);
STATIC duk_ret_t
duke_strsignal(duk_context *ctx)
{
    int sig = duk_require_int(ctx, 0);

    char *string = strsignal(sig);

    return js_push_string(ctx, string), 1;
}

JS_PARAM(psignal, 2);
STATIC duk_ret_t
duke_psignal(duk_context *ctx)
{
    int sig = duk_require_int(ctx, 0);
    char *string = (char *)duk_require_string(ctx, 1);

    return psignal(sig, string), 0;
}

// 24.3 Specifying Signal Actions
JS_PARAM(signal, 2);
STATIC duk_ret_t
duke_signal(duk_context *ctx)
{
    __sighandler_t action, old;
    js_priv_t *priv = js_priv(ctx);
    int sig = duk_require_int(ctx, 0);
    
    if (duk_is_function(ctx, 1)) {
        action = libc_sig_handler;

        priv->sig[sig].is_func = true;

        /*
        * save Function name
        */
        char *name = js_get_obj_string(ctx, 1, "name", NULL);
        os_free(priv->sig[sig].name);
        priv->sig[sig].name = os_strdup(name);
    }
    else if (duk_is_number(ctx, 1)) {
        action = (__sighandler_t)(uintptr_t)duk_require_int(ctx, 1);
        if (SIG_DFL!=action && SIG_IGN!=action) {
            goto error;
        }

        priv->sig[sig].is_func = false;
        priv->sig[sig].name = (char *)action;
    }
    else {
        goto error;
    }

    old = signal(sig, action);
    if (SIG_ERR==old) {
        js_seterrno(ctx); goto error;
    }

    if (SIG_DFL==old || SIG_IGN==old) {
        duk_push_int(ctx, (uintptr_t)old);
    } else {
        duk_push_global_object(ctx);
        duk_get_prop_string(ctx, -1, priv->sig[sig].name);
        duk_swap(ctx, -1, -2);
        duk_pop(ctx);
    }
    
    return 1;
error:
    return duk_push_null(ctx), 1;
}

// 24.3.2 Advanced Signal Handling
JS_PARAM(sigaction, 2);
STATIC duk_ret_t
duke_sigaction(duk_context *ctx)
{
    struct sigaction action, old;
    int sig = duk_require_int(ctx, 0);

    libc_get_sigaction(ctx, 1, sig, &action);
    int err = sigaction(sig, &action, &old);
    if (0==err) { // 0 is ok
        libc_set_sigaction(ctx, 2, sig, &old);
    }
    
    return js_push_error(ctx, err), 1;
}

// 24.6 Generating Signals
JS_PARAM(raise, 1);
STATIC duk_ret_t
duke_raise(duk_context *ctx)
{
    int sig = duk_require_int(ctx, 0);

    int err = raise(sig);

    return duk_push_int(ctx, err), 1;
}

JS_PARAM(kill, 2);
STATIC duk_ret_t
duke_kill(duk_context *ctx)
{
    pid_t pid = duk_require_int(ctx, 0);
    int sig = duk_require_int(ctx, 1);

    int err = kill(pid, sig);
    
    return js_push_error(ctx, err), 1;
}

JS_PARAM(killpg, 2);
STATIC duk_ret_t
duke_killpg(duk_context *ctx)
{
    pid_t pid = duk_require_int(ctx, 0);
    int sig = duk_require_int(ctx, 1);

    int err = killpg(pid, sig);

    return js_push_error(ctx, err), 1;
}

// 24.7 Blocking Signals
JS_PARAM(signewset, 1);
STATIC duk_ret_t
duke_signewset(duk_context *ctx)
{
    return js_push_dynamic_buffer(ctx, sizeof(sigset_t)), 1;
}

JS_PARAM(sigemptyset, 1);
STATIC duk_ret_t
duke_sigemptyset(duk_context *ctx)
{
    int err;
    duk_size_t bsize;
    
    sigset_t *set = (sigset_t *)duk_require_buffer_data(ctx, 0, &bsize);
    if (bsize!=sizeof(sigset_t)) {
        err = __js_seterrno(ctx, -EINVAL1); goto error;
    }
    
    err = sigemptyset(set);

error:
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(sigfillset, 1);
STATIC duk_ret_t
duke_sigfillset(duk_context *ctx)
{
    int err;
    duk_size_t bsize;
    
    sigset_t *set = (sigset_t *)duk_require_buffer_data(ctx, 0, &bsize);
    if (bsize!=sizeof(sigset_t)) {
        err = __js_seterrno(ctx, -EINVAL1); goto error;
    }

    err = sigfillset(set);

error:
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(sigaddset, 2);
STATIC duk_ret_t
duke_sigaddset(duk_context *ctx)
{
    int err;
    duk_size_t bsize;
    
    sigset_t *set = (sigset_t *)duk_require_buffer_data(ctx, 0, &bsize);
    if (bsize!=sizeof(sigset_t)) {
        err = __js_seterrno(ctx, -EINVAL1); goto error;
    }
    int sig = duk_require_int(ctx, 1);
    
    err = sigaddset(set, sig);

error:
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(sigdelset, 2);
STATIC duk_ret_t
duke_sigdelset(duk_context *ctx)
{
    int err;
    duk_size_t bsize;
    
    sigset_t *set = (sigset_t *)duk_require_buffer_data(ctx, 0, &bsize);
    if (bsize!=sizeof(sigset_t)) {
        err = __js_seterrno(ctx, -EINVAL1); goto error;
    }
    int sig = duk_require_int(ctx, 1);
    
    err = sigdelset(set, sig);

error:
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(sigismember, 2);
STATIC duk_ret_t
duke_sigismember(duk_context *ctx)
{
    int err;
    duk_size_t bsize;
    
    sigset_t *set = (sigset_t *)duk_require_buffer_data(ctx, 0, &bsize);
    if (bsize!=sizeof(sigset_t)) {
        err = __js_seterrno(ctx, -EINVAL1); duk_push_false(ctx), 1;
    }
    int sig = duk_require_int(ctx, 1);
    
    return duk_push_bool(ctx, sigismember(set, sig)), 1;
}

JS_PARAM(sigprocmask, DUK_VARARGS);
STATIC duk_ret_t
duke_sigprocmask(duk_context *ctx)
{
    int err;
    duk_size_t bsize;
    sigset_t *new = NULL, *old = NULL;
    
    int how = duk_require_int(ctx, 0);
    new = (sigset_t *)duk_require_buffer_data(ctx, 1, &bsize);
    if (bsize!=sizeof(sigset_t)) {
        err = __js_seterrno(ctx, -EINVAL2); goto error;
    }

    if (2==duk_get_argc(ctx) && duk_is_buffer(ctx, 2)) {
        old = (sigset_t *)duk_require_buffer_data(ctx, 2, &bsize);
        if (bsize!=sizeof(sigset_t)) {
            err = __js_seterrno(ctx, -EINVAL3); goto error;
        }
    }
    
    err = sigprocmask(how, new, old);
    if (err<0) {
        js_seterrno(ctx);
    }

error:
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(sigpending, 1);
STATIC duk_ret_t
duke_sigpending(duk_context *ctx)
{
    int err;
    duk_size_t bsize;
    
    sigset_t *set = (sigset_t *)duk_require_buffer_data(ctx, 0, &bsize);
    if (bsize!=sizeof(sigset_t)) {
        err = __js_seterrno(ctx, -EINVAL1); goto error;
    }
    
    err = sigpending(set);

error:
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(pause, 0);
STATIC duk_ret_t
duke_pause(duk_context *ctx)
{
    int err = pause();

    return js_push_error(ctx, err), 1;
}

JS_PARAM(sigsuspend, 1);
STATIC duk_ret_t
duke_sigsuspend(duk_context *ctx)
{
    int err;
    duk_size_t bsize;
    
    sigset_t *set = (sigset_t *)duk_require_buffer_data(ctx, 0, &bsize);
    if (bsize!=sizeof(sigset_t)) {
        err = __js_seterrno(ctx, -EINVAL1); goto error;
    }
    
    err = sigsuspend(set);

error:
    return duk_push_int(ctx, err), 1;
}
#endif /* js_LIBC_SIG */

// 25 The Basic Program/System Interface
// 25.4 Environment Variables
JS_PARAM(getenv, 1);
STATIC duk_ret_t
duke_getenv(duk_context *ctx)
{
    char *k = (char *)duk_require_string(ctx, 0);

    char *env = getenv(k);

    return js_push_string(ctx, env), 1;
}

#if defined(_GNU_SOURCE) && 0
JS_PARAM(secure_getenv, 1);
STATIC duk_ret_t
duke_secure_getenv(duk_context *ctx)
{
    char *k = (char *)duk_require_string(ctx, 0);

    char *env = secure_getenv(k);

    return js_push_string(ctx, env), 1;
}
#endif

JS_PARAM(putenv, 1);
STATIC duk_ret_t
duke_putenv(duk_context *ctx)
{
    char *kv = (char *)duk_require_string(ctx, 0);

    int err = putenv(kv);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(setenv, 3);
STATIC duk_ret_t
duke_setenv(duk_context *ctx)
{
    char *k = (char *)duk_require_string(ctx, 0);
    char *v = (char *)duk_require_string(ctx, 1);
    bool replace = duk_require_bool(ctx, 2);

    int err = setenv(k, v, replace);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(unsetenv, 1);
STATIC duk_ret_t
duke_unsetenv(duk_context *ctx)
{
    char *k = (char *)duk_require_string(ctx, 0);

    int err = unsetenv(k);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(clearenv, 0);
STATIC duk_ret_t
duke_clearenv(duk_context *ctx)
{
    int err = clearenv();

    return js_push_error(ctx, err), 1;
}

// 25.6 System Calls

// 25.7 Program Termination
JS_PARAM(exit, 1);
STATIC duk_ret_t
duke_exit(duk_context *ctx)
{
    int status = duk_require_int(ctx, 0);
    
    return exit(status), 0;
}

#if js_LIBC_SIG
JS_PARAM(atexit, 1);
STATIC duk_ret_t
duke_atexit(duk_context *ctx)
{
    if (!duk_is_function(ctx, 0)) {
        return duk_push_null(ctx), 1;
    }

    char *atexit_name = js_get_obj_string(ctx, 0, "name", NULL);
    if (NULL==atexit_name) {
        return duk_push_int(ctx, -ENOEXIST), -1;
    }

    js_priv_t *priv = js_priv(ctx);
    os_free(priv->atexit_name);
    priv->atexit_name = os_strdup(atexit_name);
    
    int err = atexit(__libc_atexit_handler);

    return duk_push_int(ctx, err), 1;
}
#endif

JS_PARAM(abort, 0);
STATIC duk_ret_t
duke_abort(duk_context *ctx)
{
    return abort(), 0;
}

JS_PARAM(_exit, 1);
STATIC duk_ret_t
duke__exit(duk_context *ctx)
{
    int status = duk_require_int(ctx, 0);
    
    return _exit(status), 0;
}

JS_PARAM(_Exit, 1);
STATIC duk_ret_t
duke__Exit(duk_context *ctx)
{
    int status = duk_require_int(ctx, 0);
    
    return _Exit(status), 0;
}

// 26 Processes
// 26.3 Process Identification
JS_PARAM(getpid, 0);
STATIC duk_ret_t
duke_getpid(duk_context *ctx)
{
    return duk_push_int(ctx, getpid()), 1;
}

JS_PARAM(getppid, 0);
STATIC duk_ret_t
duke_getppid(duk_context *ctx)
{
    return duk_push_int(ctx, getppid()), 1;
}

// 26.4 Creating a Process
JS_PARAM(fork, 0);
STATIC duk_ret_t
duke_fork(duk_context *ctx)
{
    int pid = fork();

    return js_push_error(ctx, pid), 1;
}

JS_PARAM(vfork, 0);
STATIC duk_ret_t
duke_vfork(duk_context *ctx)
{
    int pid = vfork();

    return js_push_error(ctx, pid), 1;
}

JS_PARAM(execv, 2);
STATIC duk_ret_t
duke_execv(duk_context *ctx)
{
    int i;
    
    char *filename = (char *)duk_require_string(ctx, 0);
    int len = js_get_array_length(ctx, 1);
    char *arg[len];

    for (i=0; i<len-1; i++) {
        arg[i] = js_get_array_string(ctx, 1, i, NULL);
    }
    arg[len-1] = NULL;

    int err = execv(filename, arg);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(execve, 3);
STATIC duk_ret_t
duke_execve(duk_context *ctx)
{
    int i;
    
    char *filename = (char *)duk_require_string(ctx, 0);
    int arg_len = js_get_array_length(ctx, 1);
    int env_len = js_get_array_length(ctx, 2);
    char *arg[arg_len];
    char *env[env_len];

    for (i=0; i<arg_len-1; i++) {
        arg[i] = js_get_array_string(ctx, 1, i, NULL);
    }
    arg[arg_len-1] = NULL;

    for (i=0; i<env_len-1; i++) {
        env[i] = js_get_array_string(ctx, 2, i, NULL);
    }
    env[env_len-1] = NULL;

    int err = execve(filename, arg, env);
 
    return js_push_error(ctx, err), 1;
}

JS_PARAM(execvp, 2);
STATIC duk_ret_t
duke_execvp(duk_context *ctx)
{
    int i;
    
    char *filename = (char *)duk_require_string(ctx, 0);
    int len = js_get_array_length(ctx, 1);
    char *arg[len];

    for (i=0; i<len-1; i++) {
        arg[i] = js_get_array_string(ctx, 1, i, NULL);
    }
    arg[len-1] = NULL;

    int err = execvp(filename, arg);

    return js_push_error(ctx, err), 1;
}

// 26.6 Process Completion
JS_PARAM(waitpid, 3);
STATIC duk_ret_t
duke_waitpid(duk_context *ctx)
{
    int status;
    int pid = duk_require_int(ctx, 0);
    int opt = duk_require_int(ctx, 2);

    int err = waitpid(pid, &status, opt);
    if (err>0) {
        js_set_obj_int(ctx, 1, "status", status);
    }
    
    return js_push_error(ctx, err), 1;
}

JS_PARAM(wait, 1);
STATIC duk_ret_t
duke_wait(duk_context *ctx)
{
    int status;
    
    int err = wait(&status);
    if (err>0) {
        js_set_obj_int(ctx, 1, "status", status);
    }
    
    return js_push_error(ctx, err), 1;
}

JS_PARAM(wait4, 4);
STATIC duk_ret_t
duke_wait4(duk_context *ctx)
{
    int status;
    struct rusage r;
    int pid = duk_require_int(ctx, 0);
    int opt = duk_require_int(ctx, 2);

    int err = wait4(pid, &status, opt, &r);
    if (err>0) {
        js_set_obj_int(ctx, 1, "status", status);
        libc_set_rusage(ctx, 3, &r);
    }
    
    return js_push_error(ctx, err), 1;
}

JS_PARAM(WIFEXITED, 1);
STATIC duk_ret_t
duke_WIFEXITED(duk_context *ctx)
{
    int status = duk_require_int(ctx, 0);

    return duk_push_int(ctx, WIFEXITED(status)), 1;
}

JS_PARAM(WEXITSTATUS, 1);
STATIC duk_ret_t
duke_WEXITSTATUS(duk_context *ctx)
{
    int status = duk_require_int(ctx, 0);

    return duk_push_int(ctx, WEXITSTATUS(status)), 1;
}

JS_PARAM(WIFSIGNALED, 1);
STATIC duk_ret_t
duke_WIFSIGNALED(duk_context *ctx)
{
    int status = duk_require_int(ctx, 0);

    return duk_push_int(ctx, WIFSIGNALED(status)), 1;
}

JS_PARAM(WTERMSIG, 1);
STATIC duk_ret_t
duke_WTERMSIG(duk_context *ctx)
{
    int status = duk_require_int(ctx, 0);

    return duk_push_int(ctx, WTERMSIG(status)), 1;
}

JS_PARAM(WCOREDUMP, 1);
STATIC duk_ret_t
duke_WCOREDUMP(duk_context *ctx)
{
    int status = duk_require_int(ctx, 0);

    return duk_push_int(ctx, WCOREDUMP(status)), 1;
}

JS_PARAM(WIFSTOPPED, 1);
STATIC duk_ret_t
duke_WIFSTOPPED(duk_context *ctx)
{
    int status = duk_require_int(ctx, 0);

    return duk_push_int(ctx, WIFSTOPPED(status)), 1;
}

JS_PARAM(WSTOPSIG, 1);
STATIC duk_ret_t
duke_WSTOPSIG(duk_context *ctx)
{
    int status = duk_require_int(ctx, 0);

    return duk_push_int(ctx, WSTOPSIG(status)), 1;
}

// 27 Job Control
// 27.7 Functions for Job Control
JS_PARAM(setsid, 0);
STATIC duk_ret_t
duke_setsid(duk_context *ctx)
{
    int err = setsid();

    return js_push_error(ctx, err), 1;
}

JS_PARAM(getpgrp, 0);
STATIC duk_ret_t
duke_getpgrp(duk_context *ctx)
{
    int err = getpgrp();

    return js_push_error(ctx, err), 1;
}

JS_PARAM(getpgid, 0);
STATIC duk_ret_t
duke_getpgid(duk_context *ctx)
{
    int pid = duk_require_int(ctx, 0);
    
    int err = getpgid(pid);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(setpgid, 2);
STATIC duk_ret_t
duke_setpgid(duk_context *ctx)
{
    int pid = duk_require_int(ctx, 0);
    int pgid = duk_require_int(ctx, 1);
    
    int err = setpgid(pid, pgid);

    return js_push_error(ctx, err), 1;
}

// 27.7.3 Functions for Controlling Terminal Access
JS_PARAM(tcgetpgrp, 1);
STATIC duk_ret_t
duke_tcgetpgrp(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    
    int err = tcgetpgrp(fd);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(tcsetpgrp, 2);
STATIC duk_ret_t
duke_tcsetpgrp(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    int pgid = duk_require_int(ctx, 1);
    
    int err = tcsetpgrp(fd, pgid);

    return js_push_error(ctx, err), 1;
}

// 29 Users and Groups
JS_PARAM(getuid, 0);
STATIC duk_ret_t
duke_getuid(duk_context *ctx)
{
    return duk_push_uint(ctx, getuid()), 1;
}

JS_PARAM(getgid, 0);
STATIC duk_ret_t
duke_getgid(duk_context *ctx)
{
    return duk_push_uint(ctx, getgid()), 1;
}

JS_PARAM(geteuid, 0);
STATIC duk_ret_t
duke_geteuid(duk_context *ctx)
{
    return duk_push_uint(ctx, geteuid()), 1;
}

JS_PARAM(getegid, 0);
STATIC duk_ret_t
duke_getegid(duk_context *ctx)
{
    return duk_push_uint(ctx, getegid()), 1;
}

JS_PARAM(getgroups, 1);
STATIC duk_ret_t
duke_getgroups(duk_context *ctx)
{
    int i, err;
    int count = js_get_array_length(ctx, 0);
    if (0==count) {
        return duk_push_int(ctx, getgroups(0, NULL)), 1;
    }

    gid_t groups[count];
    err = getgroups(count, groups);
    if (err>0) {
        for (i=0; i<count; i++) {
            js_set_array_uint(ctx, 0, i, groups[i]);
        }
    }
    
    return js_push_error(ctx, err), 1;
}

JS_PARAM(seteuid, 1);
STATIC duk_ret_t
duke_seteuid(duk_context *ctx)
{
    uid_t uid = duk_require_uint(ctx, 0);

    int err = seteuid(uid);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(setuid, 1);
STATIC duk_ret_t
duke_setuid(duk_context *ctx)
{
    uid_t uid = duk_require_uint(ctx, 0);

    int err = setuid(uid);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(setreuid, 2);
STATIC duk_ret_t
duke_setreuid(duk_context *ctx)
{
    uid_t ruid = duk_require_uint(ctx, 0);
    uid_t euid = duk_require_uint(ctx, 1);

    int err = setreuid(ruid, euid);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(setegid, 1);
STATIC duk_ret_t
duke_setegid(duk_context *ctx)
{
    gid_t gid = duk_require_uint(ctx, 0);

    int err = setegid(gid);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(setgid, 1);
STATIC duk_ret_t
duke_setgid(duk_context *ctx)
{
    gid_t gid = duk_require_uint(ctx, 0);

    int err = setgid(gid);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(setregid, 2);
STATIC duk_ret_t
duke_setregid(duk_context *ctx)
{
    gid_t rgid = duk_require_uint(ctx, 0);
    gid_t egid = duk_require_uint(ctx, 1);

    int err = setregid(rgid, egid);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(setgroups, 2);
STATIC duk_ret_t
duke_setgroups(duk_context *ctx)
{
    int i, err;
    int count = js_get_array_length(ctx, 0);
    gid_t groups[count];

    for (i=0; i<count; i++) {
        groups[i] = js_get_array_uint(ctx, 0, i);
    }
    
    err = setgroups(count, groups);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(initgroups, 2);
STATIC duk_ret_t
duke_initgroups(duk_context *ctx)
{
    char *user = (char *)duk_require_string(ctx, 0);
    gid_t group = duk_require_uint(ctx, 1);
    
    int err = initgroups(user, group);

    return js_push_error(ctx, err), 1;
}

// 29.11 Identifying Who Logged In
JS_PARAM(getlogin, 0);
STATIC duk_ret_t
duke_getlogin(duk_context *ctx)
{
    char *name = getlogin();

    return js_push_string(ctx, name), 1;
}

// 29.12.1 Manipulating the User Accounting Database
JS_PARAM(setutent, 0);
STATIC duk_ret_t
duke_setutent(duk_context *ctx)
{
    return setutent(), 0;
}

JS_PARAM(getutent, 0);
STATIC duk_ret_t
duke_getutent(duk_context *ctx)
{
    struct utmp *p = getutent();

    return js_obj_push(ctx, libc_set_utmp, p), 1;
}

JS_PARAM(endutent, 0);
STATIC duk_ret_t
duke_endutent(duk_context *ctx)
{
    return endutent(), 0;
}

JS_PARAM(getutid, 1);
STATIC duk_ret_t
duke_getutid(duk_context *ctx)
{
    struct utmp u;

    libc_get_utmp(ctx, 0, &u);
    
    struct utmp *p = getutid(&u);

    return js_obj_push(ctx, libc_set_utmp, p), 1;
}

JS_PARAM(getutline, 1);
STATIC duk_ret_t
duke_getutline(duk_context *ctx)
{
    struct utmp u;

    libc_get_utmp(ctx, 0, &u);
    
    struct utmp *p = getutline(&u);

    return js_obj_push(ctx, libc_set_utmp, p), 1;
}

JS_PARAM(pututline, 1);
STATIC duk_ret_t
duke_pututline(duk_context *ctx)
{
    struct utmp u;

    libc_get_utmp(ctx, 0, &u);
    
    struct utmp *p = pututline(&u);

    return js_obj_push(ctx, libc_set_utmp, p), 1;
}

JS_PARAM(utmpname, 1);
STATIC duk_ret_t
duke_utmpname(duk_context *ctx)
{
    char *file = (char *)duk_require_string(ctx, 0);

    int err = utmpname(file);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(updwtmp, 2);
STATIC duk_ret_t
duke_updwtmp(duk_context *ctx)
{
    struct utmp u;
    char *file = (char *)duk_require_string(ctx, 0);
    libc_get_utmp(ctx, 1, &u);
    
    return updwtmp(file, &u), 0;
}

#if js_LIBC_UTIL
// 29.12.3 Logging In and Out
JS_PARAM(login_tty, 1);
STATIC duk_ret_t
duke_login_tty(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);

    int err = login_tty(fd);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(login, 1);
STATIC duk_ret_t
duke_login(duk_context *ctx)
{
    struct utmp u;

    libc_get_utmp(ctx, 0, &u);

    return login(&u), 0;
}

JS_PARAM(logout, 1);
STATIC duk_ret_t
duke_logout(duk_context *ctx)
{
    char *line = (char *)duk_require_string(ctx, 0);

    int err = logout(line);
    
    return duk_push_int(ctx, err), 1;
}

JS_PARAM(logwtmp, 3);
STATIC duk_ret_t
duke_logwtmp(duk_context *ctx)
{
    char *line = (char *)duk_require_string(ctx, 0);
    char *name = (char *)duk_require_string(ctx, 1);
    char *host = (char *)duk_require_string(ctx, 2);
    
    return logwtmp(line, name, host), 0;
}
#endif /* js_LIBC_UTIL */

// 29.13 User Database
JS_PARAM(getpwuid, 1);
STATIC duk_ret_t
duke_getpwuid(duk_context *ctx)
{
    uid_t uid = duk_require_uint(ctx, 0);
    
    struct passwd *p = getpwuid(uid);

    return js_obj_push(ctx, libc_set_passwd, p), 1;
}

JS_PARAM(getpwnam, 1);
STATIC duk_ret_t
duke_getpwnam(duk_context *ctx)
{
    char *name = (char *)duk_require_string(ctx, 0);
    
    struct passwd *p = getpwnam(name);

    return js_obj_push(ctx, libc_set_passwd, p), 1;
}

#if js_LIBC_PWENT
// 29.13.3 Scanning the List of All Users
JS_PARAM(fgetpwent, 1);
STATIC duk_ret_t
duke_fgetpwent(duk_context *ctx)
{
    STREAM f = (STREAM)duk_require_pointer(ctx, 0);
    
    struct passwd *p = fgetpwent(f);

    return js_obj_push(ctx, libc_set_passwd, p), 1;
}

JS_PARAM(setpwent, 0);
STATIC duk_ret_t
duke_setpwent(duk_context *ctx)
{
    return setpwent(), 0;
}

JS_PARAM(getpwent, 0);
STATIC duk_ret_t
duke_getpwent(duk_context *ctx)
{
    struct passwd *p = getpwent();

    return js_obj_push(ctx, libc_set_passwd, p), 1;
}

JS_PARAM(endpwent, 0);
STATIC duk_ret_t
duke_endpwent(duk_context *ctx)
{
    return endpwent(), 0;
}

// 29.13.4 Writing a User Entry
JS_PARAM(putpwent, 2);
STATIC duk_ret_t
duke_putpwent(duk_context *ctx)
{
    struct passwd u;
    
    libc_get_passwd(ctx, 0, &u);
    STREAM f = (STREAM)duk_require_pointer(ctx, 1);
    
    int err = putpwent(&u, f);

    return js_push_error(ctx, err), 1;
}
#endif

// 29.14 Group Database
#if js_LIBC_GWENT
JS_PARAM(getgrgid, 1);
STATIC duk_ret_t
duke_getgrgid(duk_context *ctx)
{
    gid_t gid = duk_require_uint(ctx, 0);
    
    struct group *p = getgrgid(gid);

    return js_obj_push(ctx, libc_set_group, p), 1;
}

JS_PARAM(getgrnam, 1);
STATIC duk_ret_t
duke_getgrnam(duk_context *ctx)
{
    char *name = (char *)duk_require_string(ctx, 0);
    
    struct group *p = getgrnam(name);

    return js_obj_push(ctx, libc_set_group, p), 1;
}

JS_PARAM(fgetgrent, 1);
STATIC duk_ret_t
duke_fgetgrent(duk_context *ctx)
{
    STREAM f = (STREAM)duk_require_pointer(ctx, 0);
    
    struct group *p = fgetgrent(f);

    return js_obj_push(ctx, libc_set_group, p), 1;
}

JS_PARAM(setgrent, 0);
STATIC duk_ret_t
duke_setgrent(duk_context *ctx)
{
    return setgrent(), 0;
}

JS_PARAM(getgrent, 0);
STATIC duk_ret_t
duke_getgrent(duk_context *ctx)
{
    struct group *p = getgrent();

    return js_obj_push(ctx, libc_set_group, p), 1;
}

JS_PARAM(endgrent, 0);
STATIC duk_ret_t
duke_endgrent(duk_context *ctx)
{
    return endgrent(), 0;
}
#endif

// 30 System Management
JS_PARAM(gethostname, 0);
STATIC duk_ret_t
duke_gethostname(duk_context *ctx)
{
    int err, size = OS_LINE_LEN;
    char *line = NULL;
    
    while(1) {
        line = (char *)os_realloc(line, 1+size);
        
        err = gethostname(line, size);
        if (0==err) { // 0 is ok
            js_push_string(ctx, line);

            break;
        }
        else if (err<0 && ENAMETOOLONG!=errno) {
            js_seterrno(ctx);
            duk_push_null(ctx);

            break;
        }
    }

error:
    os_free(line);
    return 1;
}

JS_PARAM(sethostname, 1);
STATIC duk_ret_t
duke_sethostname(duk_context *ctx)
{
    duk_size_t size;
    char *line = (char *)duk_require_lstring(ctx, 0, &size);

    int err = sethostname(line, size);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(getdomainname, 0);
STATIC duk_ret_t
duke_getdomainname(duk_context *ctx)
{
    int err, size = OS_LINE_LEN;
    char *line = NULL;
    
    while(1) {
        line = (char *)os_realloc(line, 1+size);
        
        err = getdomainname(line, size);
        if (0==err) { // 0 is ok
            js_push_string(ctx, line);

            break;
        }
        else if (err<0 && ENAMETOOLONG!=errno) {
            js_seterrno(ctx);
            duk_push_null(ctx);

            break;
        }
    }

error:
    os_free(line);
    return 1;
}

JS_PARAM(setdomainname, 1);
STATIC duk_ret_t
duke_setdomainname(duk_context *ctx)
{
    duk_size_t size;
    char *line = (char *)duk_require_lstring(ctx, 0, &size);

    int err = setdomainname(line, size);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(gethostid, 0);
STATIC duk_ret_t
duke_gethostid(duk_context *ctx)
{
    return duk_push_int(ctx, gethostid()), 1;
}

JS_PARAM(sethostid, 1);
STATIC duk_ret_t
duke_sethostid(duk_context *ctx)
{
    int id = duk_require_int(ctx, 0);

    int err = sethostid(id);

    return js_push_error(ctx, err), 1;
}

// 30.2 Platform Type Identification
JS_PARAM(uname, 0);
STATIC duk_ret_t
duke_uname(duk_context *ctx)
{
    struct utsname name;

    int err = uname(&name);
    if (err<0) {
        js_seterrno(ctx);

        duk_push_null(ctx);
    } else {
        js_obj_push(ctx, libc_set_utsname, &name);
    }

    return 1;
}

#if js_LIBC_MOUNT
// 30.3.1 Mount Information
JS_PARAM(setfsent, 0);
STATIC duk_ret_t
duke_setfsent(duk_context *ctx)
{
    return duk_push_int(ctx, setfsent()), 1;
}

JS_PARAM(endfsent, 0);
STATIC duk_ret_t
duke_endfsent(duk_context *ctx)
{
    return endfsent(), 0;
}

JS_PARAM(getfsent, 0);
STATIC duk_ret_t
duke_getfsent(duk_context *ctx)
{
    struct fstab *p = getfsent();

    return js_obj_push(ctx, libc_set_fstab, p), 1;
}

JS_PARAM(getfsspec, 1);
STATIC duk_ret_t
duke_getfsspec(duk_context *ctx)
{
    char *name = (char *)duk_require_string(ctx, 0);
    
    struct fstab *p = getfsspec(name);

    return js_obj_push(ctx, libc_set_fstab, p), 1;
}

JS_PARAM(getfsfile, 1);
STATIC duk_ret_t
duke_getfsfile(duk_context *ctx)
{
    char *name = (char *)duk_require_string(ctx, 0);
    
    struct fstab *p = getfsfile(name);

    return js_obj_push(ctx, libc_set_fstab, p), 1;
}
#endif

JS_PARAM(setmntent, 2);
STATIC duk_ret_t
duke_setmntent(duk_context *ctx)
{
    char *name = (char *)duk_require_string(ctx, 0);
    char *mode = (char *)duk_require_string(ctx, 1);
    
    STREAM f = setmntent(name, mode);

    return js_push_pointer(ctx, f), 1;
}

JS_PARAM(endmntent, 1);
STATIC duk_ret_t
duke_endmntent(duk_context *ctx)
{
    STREAM f = duk_require_pointer(ctx, 0);
    
    return duk_push_int(ctx, endmntent(f)), 1;
}

JS_PARAM(getmntent, 1);
STATIC duk_ret_t
duke_getmntent(duk_context *ctx)
{
    STREAM f = duk_require_pointer(ctx, 0);
    
    struct mntent *p = getmntent(f);

    return js_obj_push(ctx, libc_set_mntent, p), 1;
}

JS_PARAM(addmntent, 2);
STATIC duk_ret_t
duke_addmntent(duk_context *ctx)
{
    struct mntent m;
    
    STREAM f = duk_require_pointer(ctx, 0);
    libc_get_mntent(ctx, 1, &m);
    
    int err = addmntent(f, &m);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(hasmntopt, 2);
STATIC duk_ret_t
duke_hasmntopt(duk_context *ctx)
{
    struct mntent m;
    
    libc_get_mntent(ctx, 0, &m);
    char *mnt = (char *)duk_require_string(ctx, 1);
    
    char *p = hasmntopt(&m, mnt);
    
    return js_push_string(ctx, p), 1;
}

// 30.3.2 Mount, Unmount, Remount
JS_PARAM(mount, 4);
STATIC duk_ret_t
duke_mount(duk_context *ctx)
{
    char *file  = (char *)duk_require_string(ctx, 0);
    char *dir   = (char *)duk_require_string(ctx, 1);
    char *fstype= (char *)duk_require_string(ctx, 2);
    duk_uint_t opt = duk_require_uint(ctx, 3);
    
    int err = mount(file, dir, fstype, opt, "");

    return js_push_error(ctx, err), 1;
}

JS_PARAM(umount2, 2);
STATIC duk_ret_t
duke_umount2(duk_context *ctx)
{
    char *file  = (char *)duk_require_string(ctx, 0);
    int flags   = duk_require_uint(ctx, 1);
    
    int err = umount2(file, flags);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(umount, 1);
STATIC duk_ret_t
duke_umount(duk_context *ctx)
{
    char *file  = (char *)duk_require_string(ctx, 0);
    
    int err = umount(file);

    return js_push_error(ctx, err), 1;
}

// 30.4 System Parameters
// sysctl


// 31 System Configuration Parameters
// 31.4 Using sysconf
JS_PARAM(sysconf, 1);
STATIC duk_ret_t
duke_sysconf(duk_context *ctx)
{
    int parameter = duk_require_int(ctx, 0);
    
    int err = sysconf(parameter);

    return js_push_error(ctx, err), 1;
}

// 31.9 Using pathconf
JS_PARAM(pathconf, 2);
STATIC duk_ret_t
duke_pathconf(duk_context *ctx)
{
    char *filename = (char *)duk_require_string(ctx, 0);
    int parameter = duk_require_int(ctx, 1);
    
    int err = pathconf(filename, parameter);

    return js_push_error(ctx, err), 1;
}

JS_PARAM(fpathconf, 2);
STATIC duk_ret_t
duke_fpathconf(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    int parameter = duk_require_int(ctx, 1);
    
    int err = fpathconf(fd, parameter);

    return js_push_error(ctx, err), 1;
}

// 31.12 String-Valued Parameters
JS_PARAM(confstr, 1);
STATIC duk_ret_t
duke_confstr(duk_context *ctx)
{
    int err;
    
    int parameter = duk_require_int(ctx, 0);
    if (false==duk_is_dynamic_buffer(ctx, 1)) {
        err = __js_seterrno(ctx, -EINVAL); goto error;
    }
    
    size_t len = confstr(parameter, NULL, 0);
    char *buf = (char *)duk_resize_buffer(ctx, 1, 1+len);

    err = confstr(parameter, buf, len);
    if (err<0) {
        js_seterrno(ctx);
    }

error:
    return duk_push_int(ctx, err), 1;
}

// 32 DES Encryption and Password Handling


#include "libc/libcf.c"
#include "libc/libcn.c"
#include "libc/libcs.c"
#include "libc/libcp.c"

int js_libc_register(duk_context *ctx)
{
    duk_push_global_object(ctx);
        duk_push_object(ctx);
            js_libcf_register(ctx, -1);
            js_libcn_register(ctx, -1);
            js_libcs_register(ctx, -1);
            js_libcp_register(ctx, -1);
        duk_put_prop_string(ctx, -2, js_MOD_LIBC);
    duk_pop(ctx);

    debug_ok("register libc ok.");
    
    return 0;
}

