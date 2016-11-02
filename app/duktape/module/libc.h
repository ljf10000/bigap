#ifndef __LIBC_H_46966fd8100c4718872a30b740b9561d__
#define __LIBC_H_46966fd8100c4718872a30b740b9561d__
/******************************************************************************/
static inline int
__get_sockaddr(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

static inline int
__set_sockaddr(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

static inline int
__get_linger(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct linger *p = (struct linger *)obj; os_objzero(p);
    
    p->l_onoff   = js_get_obj_bool(ctx, idx, "onoff");
    p->l_linger  = js_get_obj_uint(ctx, idx, "linger");

    return 0;
}

static inline int
__set_linger(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct linger *p = (struct linger *)obj;
    
    js_set_obj_bool(ctx, idx, "onoff", p->l_onoff);
    js_set_obj_uint(ctx, idx, "linger", p->l_linger);

    return 0;
}

static inline int
__get_dirent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct dirent *p = (struct dirent *)obj; os_objzero(p);
    
    p->d_type   = js_get_obj_int(ctx, idx, "type");
    p->d_ino    = js_get_obj_int(ctx, idx, "ino");
    p->d_off    = js_get_obj_uint(ctx, idx, "off");
    js_copy_obj_string(ctx, idx, "name", p->d_name, sizeof(p->d_name));

    return 0;
}

static inline int
__set_dirent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct dirent *p = (struct dirent *)obj;
    
    js_set_obj_int(ctx, idx, "type", p->d_type);
    js_set_obj_int(ctx, idx, "ino", p->d_ino);
    js_set_obj_uint(ctx, idx, "off", p->d_off);
    js_set_obj_string(ctx, idx, "name", p->d_name);

    return 0;
}

static inline int
__get_stat(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

static inline int
__set_stat(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

static inline int
__get_if_nameindex(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct if_nameindex *p = (struct if_nameindex *)obj; os_objzero(p);
    
    p->if_index = js_get_obj_uint(ctx, idx, "index");
    p->if_name  = js_get_obj_string(ctx, idx, "name", NULL);

    return 0;
}

static inline int
__set_if_nameindex(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct if_nameindex *p = (struct if_nameindex *)obj;
    
    js_set_obj_uint(ctx, idx, "index", p->if_index);
    js_set_obj_string(ctx, idx, "name", p->if_name);

    return 0;
}

static inline int
__set_hostent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct hostent *p = (struct hostent *)obj;
    char *a;
    int i;
    
    js_set_obj_string(ctx, idx, "name", p->h_name);
    js_set_obj_int(ctx, idx, "addrtype", p->h_addrtype);
    js_set_obj_int(ctx, idx, "length", p->h_length);
    js_set_obj_string(ctx, idx, "addr", p->h_addr);
    
    duk_push_object(ctx);
    for (i=0, a=p->h_aliases[0];a; i++, a++) {
        js_set_array_string(ctx, -1, i, a);
    }
    duk_put_prop_string(ctx, idx, "aliases");
    
    duk_push_object(ctx);
    for (i=0, a=p->h_addr_list[0];a; i++, a++) {
        js_set_array_string(ctx, -1, i, a);
    }
    duk_put_prop_string(ctx, idx, "addr_list");

    return 0;
}

static inline int
__set_servent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

static inline int
__set_protoent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

static inline int
__set_netent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

static inline int
__get_utimbuf(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct utimbuf *p = (struct utimbuf *)obj; os_objzero(p);
    
    p->actime   = js_get_obj_uint(ctx, idx, "actime");
    p->modtime  = js_get_obj_uint(ctx, idx, "modtime");

    return 0;
}

static inline int
__set_utimbuf(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct utimbuf *p = (struct utimbuf *)obj;
    
    js_set_obj_uint(ctx, idx, "actime", p->actime);
    js_set_obj_uint(ctx, idx, "modtime", p->modtime);

    return 0;
}

static inline int
__get_timeval(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timeval *p = (struct timeval *)obj; os_objzero(p);
    
    p->tv_sec   = js_get_obj_uint(ctx, idx, "sec");
    p->tv_usec  = js_get_obj_uint(ctx, idx, "usec");

    return 0;
}

static inline int
__set_timeval(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timeval *p = (struct timeval *)obj;
    
    js_set_obj_uint(ctx, idx, "sec", p->tv_sec);
    js_set_obj_uint(ctx, idx, "usec", p->tv_usec);

    return 0;
}

static inline int
__get_itimerval(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct itimerval *p = (struct itimerval *)obj; os_objzero(p);

    js_obj_get(ctx, idx, __get_timeval, &p->it_interval, "interval");
    js_obj_get(ctx, idx, __get_timeval, &p->it_value, "value");

    return 0;
}

static inline int
__set_itimerval(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct itimerval *p = (struct itimerval *)obj;
    
    js_obj_set(ctx, idx, __set_timeval, &p->it_interval, "interval");
    js_obj_set(ctx, idx, __set_timeval, &p->it_value, "value");

    return 0;
}

static inline int
__get_timespec(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timespec *p = (struct timespec *)obj; os_objzero(p);
    
    p->tv_sec   = js_get_obj_uint(ctx, idx, "sec");
    p->tv_nsec  = js_get_obj_uint(ctx, idx, "nsec");

    return 0;
}

static inline int
__set_timespec(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timespec *p = (struct timespec *)obj;
    
    js_set_obj_uint(ctx, idx, "sec", p->tv_sec);
    js_set_obj_uint(ctx, idx, "nsec", p->tv_nsec);

    return 0;
}

static inline int
__get_itimerspec(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct itimerspec *p = (struct itimerspec *)obj; os_objzero(p);

    js_obj_get(ctx, idx, __get_timespec, &p->it_interval, "interval");
    js_obj_get(ctx, idx, __get_timespec, &p->it_value, "value");

    return 0;
}

static inline int
__set_itimerspec(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct itimerspec *p = (struct itimerspec *)obj;

    js_obj_set(ctx, idx, __set_timespec, &p->it_interval, "interval");
    js_obj_set(ctx, idx, __set_timespec, &p->it_value, "value");

    return 0;
}

#if js_LIBC_LINUX

static inline int
__get_signalfd_siginfo(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

static inline int
__set_signalfd_siginfo(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

static inline int
__get_watcher_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    loop_watcher_t *p = (loop_watcher_t *)obj; os_objzero(p);

    p->fd       = js_get_obj_int(ctx, idx, "fd");
    p->father   = js_get_obj_int(ctx, idx, "father");
    p->type     = js_get_obj_int(ctx, idx, "type");
    p->flag     = js_get_obj_int(ctx, idx, "flag");

    return 0;
}

static inline int
__set_watcher_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    loop_watcher_t *p = (loop_watcher_t *)obj; os_objzero(p);

    js_set_obj_int(ctx, idx, "fd",      p->fd);
    js_set_obj_int(ctx, idx, "father",  p->father);
    js_set_obj_int(ctx, idx, "type",    p->type);
    js_set_obj_int(ctx, idx, "flag",    p->flag);

    return 0;
}

static inline int
__get_epoll_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct epoll_event *p = (struct epoll_event *)obj; os_objzero(p);
    
    p->events   = js_get_obj_uint(ctx, idx, "events");
    p->data.fd  = js_get_obj_int(ctx, idx, "fd");

    return 0;
}

static inline int
__set_epoll_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct epoll_event *p = (struct epoll_event *)obj;
    
    js_set_obj_uint(ctx, idx, "events", p->events);
    js_set_obj_int(ctx, idx, "fd", p->data.fd);

    return 0;
}

static inline int
__get_inotify_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct inotify_event *p = (struct inotify_event *)obj; os_objzero(p);
    
    p->wd   = js_get_obj_int(ctx, idx, "wd");
    p->mask = js_get_obj_uint(ctx, idx, "mask");
    p->cookie = js_get_obj_uint(ctx, idx, "cookie");
    js_copy_obj_string(ctx, idx, "name", p->name, 1+NAME_MAX);
    p->len  = os_strlen(p->name);

    return 0;
}

static inline int
__set_inotify_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct inotify_event *p = (struct inotify_event *)obj;
    
    js_set_obj_int(ctx, idx, "wd", p->wd);
    js_set_obj_uint(ctx, idx, "mask", p->mask);
    js_set_obj_uint(ctx, idx, "cookie", p->cookie);
    js_set_obj_string(ctx, idx, "name", p->name);

    return 0;
}
#endif

static inline int
__get_tms(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct tms *p = (struct tms *)obj; os_objzero(p);
    
    p->tms_utime    = js_get_obj_uint(ctx, idx, "utime");
    p->tms_stime    = js_get_obj_uint(ctx, idx, "stime");
    p->tms_cutime   = js_get_obj_uint(ctx, idx, "cutime");
    p->tms_cstime   = js_get_obj_uint(ctx, idx, "cstime");

    return 0;
}

static inline int
__set_tms(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct tms *p = (struct tms *)obj;
    
    js_set_obj_uint(ctx, idx, "utime", p->tms_utime);
    js_set_obj_uint(ctx, idx, "stime", p->tms_stime);
    js_set_obj_uint(ctx, idx, "cutime", p->tms_cutime);
    js_set_obj_uint(ctx, idx, "cstime", p->tms_cstime);

    return 0;
}

static inline int
__get_timezone(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timezone *p = (struct timezone *)obj; os_objzero(p);
    
    p->tz_minuteswest   = js_get_obj_uint(ctx, idx, "minuteswest");
    p->tz_dsttime       = js_get_obj_uint(ctx, idx, "dsttime");

    return 0;
}

static inline int
__set_timezone(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timezone *p = (struct timezone *)obj;
    
    js_set_obj_uint(ctx, idx, "minuteswest", p->tz_minuteswest);
    js_set_obj_uint(ctx, idx, "dsttime", p->tz_dsttime);

    return 0;
}

static inline int
__get_tm(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

static inline int
__set_tm(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

static inline int
__get_ntptimeval(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct ntptimeval *p = (struct ntptimeval *)obj; os_objzero(p);
    
    p->maxerror   = js_get_obj_int(ctx, idx, "maxerror");
    p->esterror  = js_get_obj_int(ctx, idx, "esterror");

    os_objzero(&p->time);

    js_obj_get(ctx, idx, __get_timeval, &p->time, "time");

    return 0;
}

static inline int
__set_ntptimeval(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct ntptimeval *p = (struct ntptimeval *)obj;
    
    js_set_obj_int(ctx, idx, "maxerror", p->maxerror);
    js_set_obj_int(ctx, idx, "esterror", p->esterror);

    js_obj_set(ctx, idx, __set_timeval, &p->time, "time");

    return 0;
}

static inline int
__get_timex(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

    js_obj_get(ctx, idx, __get_timeval, &p->time, "time");

    return 0;
}

static inline int
__set_timex(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

    js_obj_set(ctx, idx, __set_timeval, &p->time, "time");

    return 0;
}

static inline int
__get_rusage(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

    js_obj_get(ctx, idx, __get_timeval, &p->ru_utime, "utime");
    js_obj_get(ctx, idx, __get_timeval, &p->ru_stime, "stime");

    return 0;
}

static inline int
__set_rusage(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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
    
    js_obj_set(ctx, idx, __set_timeval, &p->ru_utime, "utime");
    js_obj_set(ctx, idx, __set_timeval, &p->ru_stime, "stime");

    return 0;
}

#if js_LIBC_VTIME
static inline int
__get_vtimes(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

static inline int
__set_vtimes(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

static inline int
__get_flock(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct flock *p = (struct flock *)obj; os_objzero(p);
    
    p->l_type   = js_get_obj_int(ctx, idx, "type");
    p->l_whence = js_get_obj_int(ctx, idx, "whence");
    p->l_start  = js_get_obj_uint(ctx, idx, "start");
    p->l_len    = js_get_obj_uint(ctx, idx, "len");
    p->l_pid    = js_get_obj_uint(ctx, idx, "pid");

    return 0;
}

static inline int
__set_flock(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct flock *p = (struct flock *)obj;
    
    js_set_obj_int(ctx, idx, "type", p->l_type);
    js_set_obj_int(ctx, idx, "whence", p->l_whence);
    js_set_obj_uint(ctx, idx, "start", p->l_start);
    js_set_obj_uint(ctx, idx, "len", p->l_len);
    js_set_obj_uint(ctx, idx, "pid", p->l_pid);

    return 0;
}

static inline int
__get_termios(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

static inline int
__set_termios(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

static inline int
__get_div(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    div_t *p = (div_t *)obj; os_objzero(p);
    
    p->quot = js_get_obj_int(ctx, idx, "quot");
    p->rem  = js_get_obj_int(ctx, idx, "rem");

    return 0;
}

static inline int
__set_div(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    div_t *p = (div_t *)obj;
    
    js_set_obj_int(ctx, idx, "quot", p->quot);
    js_set_obj_int(ctx, idx, "rem", p->rem);

    return 0;
}

static inline int
__get_rlimit(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct rlimit *p = (struct rlimit *)obj; os_objzero(p);
    
    p->rlim_cur = js_get_obj_int(ctx, idx, "cur");
    p->rlim_max  = js_get_obj_int(ctx, idx, "max");

    return 0;
}

static inline int
__set_rlimit(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct rlimit *p = (struct rlimit *)obj;
    
    js_set_obj_int(ctx, idx, "cur", p->rlim_cur);
    js_set_obj_int(ctx, idx, "max", p->rlim_max);

    return 0;
}

static inline int
__get_sched_param(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct sched_param *p = (struct sched_param *)obj; os_objzero(p);
    
    p->sched_priority = js_get_obj_int(ctx, idx, "priority");

    return 0;
}

static inline int
__set_sched_param(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct sched_param *p = (struct sched_param *)obj;
    
    js_set_obj_int(ctx, idx, "priority", p->sched_priority);

    return 0;
}

#if js_LIBC_SIG
extern void __libc_sig_handler(int sig);

static inline int
__get_sigaction(duk_context *ctx, duk_idx_t idx, int sig, duk_object_t obj)
{
    js_priv_t *priv = js_priv(ctx);
    struct sigaction *p = (struct sigaction *)obj; os_objzero(p);
    
    p->sa_mask  = *(sigset_t *)js_get_obj_buffer(ctx, idx, "mask", NULL);
    p->sa_flags = js_get_obj_int(ctx, idx, "flags");
    
    duk_get_prop_string(ctx, idx, "handler");
    if (duk_is_function(ctx, -1)) {
        p->sa_handler = __libc_sig_handler;

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

static inline int
__set_sigaction(duk_context *ctx, duk_idx_t idx, int sig, duk_object_t obj)
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

static inline int
__get_exit_status(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct exit_status *p = (struct exit_status *)obj; os_objzero(p);
    
    p->e_termination = js_get_obj_int(ctx, idx, "termination");
    p->e_exit  = js_get_obj_int(ctx, idx, "exit");

    return 0;
}

static inline int
__set_exit_status(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct exit_status *p = (struct exit_status *)obj;
    
    js_set_obj_int(ctx, idx, "termination", p->e_termination);
    js_set_obj_int(ctx, idx, "exit", p->e_exit);

    return 0;
}

static inline int
__get_utmp(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

    js_obj_get(ctx, idx, __get_exit_status, &p->ut_exit, "exit");

    duk_get_prop_string(ctx, idx, "addr_v6");
    if (duk_is_array(ctx, -1)) {
        for (i=0; i<4; i++) {
            p->ut_addr_v6[i] = js_get_array_uint(ctx, -1, i);
        }
    }
    duk_pop(ctx);

    return 0;
}

static inline int
__set_utmp(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

    js_obj_get(ctx, idx, __set_exit_status, &p->ut_exit, "exit");

    duk_get_prop_string(ctx, idx, "addr_v6");
    if (duk_is_array(ctx, -1)) {
        for (i=0; i<4; i++) {
             js_set_array_uint(ctx, -1, i, p->ut_addr_v6[i]);
        }
    }
    duk_pop(ctx);

    return 0;
}

static inline int
__get_passwd(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

static inline int
__set_passwd(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

static inline int
__set_group(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

static inline int
__get_utsname(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct utsname *p = (struct utsname *)obj; os_objzero(p);
    
    js_copy_obj_string(ctx, idx, "sysname", p->sysname, sizeof(p->sysname)-1);
    js_copy_obj_string(ctx, idx, "release", p->release, sizeof(p->release)-1);
    js_copy_obj_string(ctx, idx, "version", p->version, sizeof(p->version)-1);
    js_copy_obj_string(ctx, idx, "machine", p->machine, sizeof(p->machine)-1);
    js_copy_obj_string(ctx, idx, "nodename", p->nodename, sizeof(p->nodename)-1);

    return 0;
}

static inline int
__set_utsname(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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
static inline int
__get_fstab(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

static inline int
__set_fstab(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

static inline int
__get_mntent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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

static inline int
__set_mntent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
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
#endif /* __LIBC_H_46966fd8100c4718872a30b740b9561d__ */