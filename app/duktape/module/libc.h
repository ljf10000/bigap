#ifndef __LIBC_H_46966fd8100c4718872a30b740b9561d__
#define __LIBC_H_46966fd8100c4718872a30b740b9561d__
/******************************************************************************/
static inline int
__get_sockaddr(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    os_sockaddr_t *p = (os_sockaddr_t *)obj; os_objzero(p);
    
    p->c.sa_family = __get_obj_int(ctx, idx, "family");
    
    switch(p->c.sa_family) {
        case AF_INET:
            p->in.sin_addr.s_addr = __get_obj_uint(ctx, idx, "ip");
            p->in.sin_port = __get_obj_int(ctx, idx, "port");

            break;
        case AF_UNIX: {
            bool abstract = __get_obj_bool(ctx, idx, "abstract");
            
            __copy_obj_string(ctx, idx, "path",
                p->un.sun_path + !!abstract,
                sizeof(p->un.sun_path) - !!abstract);
        }   break;
        case AF_PACKET:
            p->ll.sll_protocol = __get_obj_int(ctx, idx, "protocol");
            p->ll.sll_ifindex = __get_obj_int(ctx, idx, "ifindex");
            p->ll.sll_hatype = __get_obj_int(ctx, idx, "hatype");
            p->ll.sll_pkttype = __get_obj_int(ctx, idx, "pkttype");
            p->ll.sll_halen = __get_obj_int(ctx, idx, "halen");

            __copy_obj_buffer(ctx, idx, "addr", (char *)p->ll.sll_addr, 8);

            break;
        case AF_NETLINK:
            p->nl.nl_pid = __get_obj_int(ctx, idx, "pid");
            p->nl.nl_groups = __get_obj_int(ctx, idx, "groups");

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
    
    __set_obj_int(ctx, idx, "family", p->c.sa_family);
    
    switch(p->c.sa_family) {
        case AF_INET:
            __set_obj_uint(ctx, idx, "ip", p->in.sin_addr.s_addr);
            __set_obj_uint(ctx, idx, "port", p->in.sin_port);
            
            break;
        case AF_UNIX: {
            bool abstract = is_abstract_sockaddr(&p->un);

            __set_obj_bool(ctx, idx, "abstract", abstract);
            __set_obj_string(ctx, idx, "path", p->un.sun_path + !!abstract);
            
        }   break;
        case AF_PACKET:
            __set_obj_int(ctx, idx, "protocol", p->ll.sll_protocol);
            __set_obj_uint(ctx, idx, "ifindex", p->ll.sll_ifindex);
            __set_obj_int(ctx, idx, "hatype", p->ll.sll_hatype);
            __set_obj_int(ctx, idx, "pkttype", p->ll.sll_pkttype);
            __set_obj_int(ctx, idx, "halen", p->ll.sll_halen);
            __set_obj_buffer(ctx, idx, "addr", p->ll.sll_addr, 8);
            
            break;
        case AF_NETLINK:
            __set_obj_int(ctx, idx, "pid", p->nl.nl_pid);
            __set_obj_uint(ctx, idx, "groups", p->nl.nl_groups);
            
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
    
    p->l_onoff   = __get_obj_bool(ctx, idx, "onoff");
    p->l_linger  = __get_obj_uint(ctx, idx, "linger");

    return 0;
}

static inline int
__set_linger(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct linger *p = (struct linger *)obj;
    
    __set_obj_bool(ctx, idx, "onoff", p->l_onoff);
    __set_obj_uint(ctx, idx, "linger", p->l_linger);

    return 0;
}

static inline int
__get_dirent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct dirent *p = (struct dirent *)obj; os_objzero(p);
    
    p->d_type   = __get_obj_int(ctx, idx, "type");
    p->d_ino    = __get_obj_int(ctx, idx, "ino");
    p->d_off    = __get_obj_uint(ctx, idx, "off");
    __copy_obj_string(ctx, idx, "name", p->d_name, sizeof(p->d_name));

    return 0;
}

static inline int
__set_dirent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct dirent *p = (struct dirent *)obj;
    
    __set_obj_int(ctx, idx, "type", p->d_type);
    __set_obj_int(ctx, idx, "ino", p->d_ino);
    __set_obj_uint(ctx, idx, "off", p->d_off);
    __set_obj_string(ctx, idx, "name", p->d_name);

    return 0;
}

static inline int
__get_stat(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct stat *p = (struct stat *)obj; os_objzero(p);
    
    p->st_dev   = __get_obj_uint(ctx, idx, "dev");
    p->st_rdev  = __get_obj_uint(ctx, idx, "rdev");
    p->st_ino   = __get_obj_uint(ctx, idx, "ino");
    p->st_mode  = __get_obj_uint(ctx, idx, "mode");
    p->st_nlink = __get_obj_uint(ctx, idx, "nlink");
    p->st_uid   = __get_obj_uint(ctx, idx, "uid");
    p->st_gid   = __get_obj_uint(ctx, idx, "gid");
    p->st_atime = __get_obj_uint(ctx, idx, "atime");
    p->st_mtime = __get_obj_uint(ctx, idx, "mtime");
    p->st_ctime = __get_obj_uint(ctx, idx, "ctime");
    p->st_size  = __get_obj_int(ctx, idx, "size");
    p->st_blksize = __get_obj_int(ctx, idx, "blksize");
    p->st_blocks= __get_obj_int(ctx, idx, "blocks");

    return 0;
}

static inline int
__set_stat(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct stat *p = (struct stat *)obj;
    
    __set_obj_uint(ctx, idx, "dev", p->st_dev);
    __set_obj_uint(ctx, idx, "rdev", p->st_rdev);
    __set_obj_uint(ctx, idx, "ino", p->st_ino);
    __set_obj_uint(ctx, idx, "mode", p->st_mode);
    __set_obj_uint(ctx, idx, "nlink", p->st_nlink);
    __set_obj_uint(ctx, idx, "uid", p->st_uid);
    __set_obj_uint(ctx, idx, "gid", p->st_gid);
    __set_obj_uint(ctx, idx, "atime", p->st_atime);
    __set_obj_uint(ctx, idx, "mtime", p->st_mtime);
    __set_obj_uint(ctx, idx, "ctime", p->st_ctime);
    __set_obj_int(ctx, idx, "size", p->st_size);
    __set_obj_int(ctx, idx, "blksize", p->st_blksize);
    __set_obj_int(ctx, idx, "blocks", p->st_blocks);

    return 0;
}

static inline int
__get_if_nameindex(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct if_nameindex *p = (struct if_nameindex *)obj; os_objzero(p);
    
    p->if_index = __get_obj_uint(ctx, idx, "index");
    p->if_name  = __get_obj_string(ctx, idx, "name", NULL);

    return 0;
}

static inline int
__set_if_nameindex(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct if_nameindex *p = (struct if_nameindex *)obj;
    
    __set_obj_uint(ctx, idx, "index", p->if_index);
    __set_obj_string(ctx, idx, "name", p->if_name);

    return 0;
}

static inline int
__set_hostent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct hostent *p = (struct hostent *)obj;
    char *a;
    int i;
    
    __set_obj_string(ctx, idx, "name", p->h_name);
    __set_obj_int(ctx, idx, "addrtype", p->h_addrtype);
    __set_obj_int(ctx, idx, "length", p->h_length);
    __set_obj_string(ctx, idx, "addr", p->h_addr);
    
    duk_push_object(ctx);
    for (i=0, a=p->h_aliases[0];a; i++, a++) {
        __set_array_string(ctx, -1, i, a);
    }
    duk_put_prop_string(ctx, idx, "aliases");
    
    duk_push_object(ctx);
    for (i=0, a=p->h_addr_list[0];a; i++, a++) {
        __set_array_string(ctx, -1, i, a);
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
    
    __set_obj_string(ctx, idx, "name", p->s_name);
    __set_obj_int(ctx, idx, "port", p->s_port);
    __set_obj_string(ctx, idx, "proto", p->s_proto);
    
    duk_push_object(ctx);
    for (i=0, a=p->s_aliases[0]; a; i++, a++) {
        __set_array_string(ctx, -1, i, a);
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
    
    __set_obj_string(ctx, idx, "name", p->p_name);
    __set_obj_int(ctx, idx, "proto", p->p_proto);
    
    duk_push_object(ctx);
    for (i=0, a=p->p_aliases[0];a; i++, a++) {
        __set_array_string(ctx, -1, i, a);
    }
    duk_put_prop_string(ctx, idx, "aliases");

    return 0;
}

static inline int
__set_netent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct netent *p = (struct netent *)obj;
    int i;
        
    __set_obj_string(ctx, idx, "n_name", p->n_name);
    __set_obj_int(ctx, idx, "n_addrtype", p->n_addrtype);
    __set_obj_int(ctx, idx, "n_net", p->n_net);

    duk_push_object(ctx);
    for (i=0; p->n_aliases[i]; i++) {
        __set_array_string(ctx, -1, i, p->n_aliases[i]);
    }
    duk_put_prop_string(ctx, idx, "n_aliases");

    return 0;
}

static inline int
__get_utimbuf(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct utimbuf *p = (struct utimbuf *)obj; os_objzero(p);
    
    p->actime   = __get_obj_uint(ctx, idx, "actime");
    p->modtime  = __get_obj_uint(ctx, idx, "modtime");

    return 0;
}

static inline int
__set_utimbuf(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct utimbuf *p = (struct utimbuf *)obj;
    
    __set_obj_uint(ctx, idx, "actime", p->actime);
    __set_obj_uint(ctx, idx, "modtime", p->modtime);

    return 0;
}

static inline int
__get_timeval(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timeval *p = (struct timeval *)obj; os_objzero(p);
    
    p->tv_sec   = __get_obj_uint(ctx, idx, "sec");
    p->tv_usec  = __get_obj_uint(ctx, idx, "usec");

    return 0;
}

static inline int
__set_timeval(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timeval *p = (struct timeval *)obj;
    
    __set_obj_uint(ctx, idx, "sec", p->tv_sec);
    __set_obj_uint(ctx, idx, "usec", p->tv_usec);

    return 0;
}

static inline int
__get_itimerval(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct itimerval *p = (struct itimerval *)obj; os_objzero(p);

    __obj_get(ctx, idx, __get_timeval, &p->it_interval, "interval");
    __obj_get(ctx, idx, __get_timeval, &p->it_value, "value");

    return 0;
}

static inline int
__set_itimerval(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct itimerval *p = (struct itimerval *)obj;
    
    __obj_set(ctx, idx, __set_timeval, &p->it_interval, "interval");
    __obj_set(ctx, idx, __set_timeval, &p->it_value, "value");

    return 0;
}

static inline int
__get_timespec(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timespec *p = (struct timespec *)obj; os_objzero(p);
    
    p->tv_sec   = __get_obj_uint(ctx, idx, "sec");
    p->tv_nsec  = __get_obj_uint(ctx, idx, "nsec");

    return 0;
}

static inline int
__set_timespec(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timespec *p = (struct timespec *)obj;
    
    __set_obj_uint(ctx, idx, "sec", p->tv_sec);
    __set_obj_uint(ctx, idx, "nsec", p->tv_nsec);

    return 0;
}

static inline int
__get_itimerspec(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct itimerspec *p = (struct itimerspec *)obj; os_objzero(p);

    __obj_get(ctx, idx, __get_timespec, &p->it_interval, "interval");
    __obj_get(ctx, idx, __get_timespec, &p->it_value, "value");

    return 0;
}

static inline int
__set_itimerspec(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct itimerspec *p = (struct itimerspec *)obj;

    __obj_set(ctx, idx, __set_timespec, &p->it_interval, "interval");
    __obj_set(ctx, idx, __set_timespec, &p->it_value, "value");

    return 0;
}

#if duk_LIBC_LINUX

static inline int
__get_signalfd_siginfo(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct signalfd_siginfo *p = (struct signalfd_siginfo *)obj; os_objzero(p);
    
    p->ssi_signo    = __get_obj_uint(ctx, idx, "signo");
    p->ssi_errno    = __get_obj_int(ctx, idx, "errno");
    p->ssi_code     = __get_obj_int(ctx, idx, "code");
    p->ssi_pid      = __get_obj_uint(ctx, idx, "pid");
    p->ssi_uid      = __get_obj_uint(ctx, idx, "uid");
    p->ssi_fd       = __get_obj_int(ctx, idx, "fd");
    p->ssi_tid      = __get_obj_uint(ctx, idx, "tid");
    p->ssi_band     = __get_obj_uint(ctx, idx, "band");
    p->ssi_overrun  = __get_obj_uint(ctx, idx, "overrun");
    p->ssi_trapno   = __get_obj_uint(ctx, idx, "trapno");
    p->ssi_status   = __get_obj_int(ctx, idx, "status");
    p->ssi_int      = __get_obj_int(ctx, idx, "int");

    return 0;
}

static inline int
__set_signalfd_siginfo(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct signalfd_siginfo *p = (struct signalfd_siginfo *)obj;
    
    __set_obj_uint(ctx, idx, "signo", p->ssi_signo);
    __set_obj_int(ctx, idx, "errno", p->ssi_errno);
    __set_obj_int(ctx, idx, "code", p->ssi_code);
    __set_obj_uint(ctx, idx, "pid", p->ssi_pid);
    __set_obj_uint(ctx, idx, "uid", p->ssi_uid);
    __set_obj_int(ctx, idx, "fd", p->ssi_fd);
    __set_obj_uint(ctx, idx, "tid", p->ssi_tid);
    __set_obj_uint(ctx, idx, "band", p->ssi_band);
    __set_obj_uint(ctx, idx, "overrun", p->ssi_overrun);
    __set_obj_uint(ctx, idx, "trapno", p->ssi_trapno);
    __set_obj_int(ctx, idx, "status", p->ssi_status);
    __set_obj_int(ctx, idx, "int", p->ssi_int);

    return 0;
}

static inline int
__get_epoll_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct epoll_event *p = (struct epoll_event *)obj; os_objzero(p);
    
    p->events   = __get_obj_uint(ctx, idx, "events");
    p->data.fd  = __get_obj_int(ctx, idx, "fd");

    return 0;
}

static inline int
__set_epoll_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct epoll_event *p = (struct epoll_event *)obj;
    
    __set_obj_uint(ctx, idx, "events", p->events);
    __set_obj_int(ctx, idx, "fd", p->data.fd);

    return 0;
}

static inline int
__get_inotify_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct inotify_event *p = (struct inotify_event *)obj; os_objzero(p);
    
    p->wd   = __get_obj_int(ctx, idx, "wd");
    p->mask = __get_obj_uint(ctx, idx, "mask");
    p->cookie = __get_obj_uint(ctx, idx, "cookie");
    __copy_obj_string(ctx, idx, "name", p->name, 1+NAME_MAX);
    p->len  = os_strlen(p->name);

    return 0;
}

static inline int
__set_inotify_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct inotify_event *p = (struct inotify_event *)obj;
    
    __set_obj_int(ctx, idx, "wd", p->wd);
    __set_obj_uint(ctx, idx, "mask", p->mask);
    __set_obj_uint(ctx, idx, "cookie", p->cookie);
    __set_obj_string(ctx, idx, "name", p->name);

    return 0;
}
#endif

static inline int
__get_tms(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct tms *p = (struct tms *)obj; os_objzero(p);
    
    p->tms_utime    = __get_obj_uint(ctx, idx, "utime");
    p->tms_stime    = __get_obj_uint(ctx, idx, "stime");
    p->tms_cutime   = __get_obj_uint(ctx, idx, "cutime");
    p->tms_cstime   = __get_obj_uint(ctx, idx, "cstime");

    return 0;
}

static inline int
__set_tms(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct tms *p = (struct tms *)obj;
    
    __set_obj_uint(ctx, idx, "utime", p->tms_utime);
    __set_obj_uint(ctx, idx, "stime", p->tms_stime);
    __set_obj_uint(ctx, idx, "cutime", p->tms_cutime);
    __set_obj_uint(ctx, idx, "cstime", p->tms_cstime);

    return 0;
}

static inline int
__get_timezone(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timezone *p = (struct timezone *)obj; os_objzero(p);
    
    p->tz_minuteswest   = __get_obj_uint(ctx, idx, "minuteswest");
    p->tz_dsttime       = __get_obj_uint(ctx, idx, "dsttime");

    return 0;
}

static inline int
__set_timezone(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timezone *p = (struct timezone *)obj;
    
    __set_obj_uint(ctx, idx, "minuteswest", p->tz_minuteswest);
    __set_obj_uint(ctx, idx, "dsttime", p->tz_dsttime);

    return 0;
}

static inline int
__get_tm(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct tm *p = (struct tm *)obj; os_objzero(p);
    
    p->tm_sec   = __get_obj_int(ctx, idx, "sec");
    p->tm_min   = __get_obj_int(ctx, idx, "min");
    p->tm_hour  = __get_obj_int(ctx, idx, "hour");
    p->tm_mday  = __get_obj_int(ctx, idx, "mday");
    p->tm_mon   = __get_obj_int(ctx, idx, "mon");
    p->tm_wday  = __get_obj_int(ctx, idx, "wday");
    p->tm_yday  = __get_obj_int(ctx, idx, "yday");
    p->tm_isdst = __get_obj_int(ctx, idx, "isdst");
    p->tm_gmtoff= __get_obj_int(ctx, idx, "gmtoff");
    p->tm_zone  = __get_obj_string(ctx, idx, "zone", NULL);

    return 0;
}

static inline int
__set_tm(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct tm *p = (struct tm *)obj;
    
    __set_obj_int(ctx, idx, "sec", p->tm_sec);
    __set_obj_int(ctx, idx, "min", p->tm_min);
    __set_obj_int(ctx, idx, "hour", p->tm_hour);
    __set_obj_int(ctx, idx, "mday", p->tm_mday);
    __set_obj_int(ctx, idx, "mon", p->tm_mon);
    __set_obj_int(ctx, idx, "wday", p->tm_wday);
    __set_obj_int(ctx, idx, "yday", p->tm_yday);
    __set_obj_int(ctx, idx, "isdst", p->tm_isdst);
    __set_obj_int(ctx, idx, "gmtoff", p->tm_gmtoff);
    __set_obj_string(ctx, idx, "zone", (char *)p->tm_zone);

    return 0;
}

static inline int
__get_ntptimeval(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct ntptimeval *p = (struct ntptimeval *)obj; os_objzero(p);
    
    p->maxerror   = __get_obj_int(ctx, idx, "maxerror");
    p->esterror  = __get_obj_int(ctx, idx, "esterror");

    os_objzero(&p->time);

    __obj_get(ctx, idx, __get_timeval, &p->time, "time");

    return 0;
}

static inline int
__set_ntptimeval(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct ntptimeval *p = (struct ntptimeval *)obj;
    
    __set_obj_int(ctx, idx, "maxerror", p->maxerror);
    __set_obj_int(ctx, idx, "esterror", p->esterror);

    __obj_set(ctx, idx, __set_timeval, &p->time, "time");

    return 0;
}

static inline int
__get_timex(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timex *p = (struct timex *)obj; os_objzero(p);
    
    p->modes    = __get_obj_uint(ctx, idx, "modes");
    p->offset   = __get_obj_int(ctx, idx, "offset");
#if 0
    p->frequency= __get_obj_int(ctx, idx, "frequency");
#endif
    p->maxerror = __get_obj_int(ctx, idx, "maxerror");
    p->esterror = __get_obj_int(ctx, idx, "esterror");
    p->status   = __get_obj_int(ctx, idx, "status");
    p->constant = __get_obj_int(ctx, idx, "constant");
    p->precision= __get_obj_int(ctx, idx, "precision");
    p->tolerance= __get_obj_int(ctx, idx, "tolerance");
    p->tick     = __get_obj_int(ctx, idx, "tick");
    p->ppsfreq  = __get_obj_int(ctx, idx, "ppsfreq");
    p->jitter   = __get_obj_int(ctx, idx, "jitter");
    p->shift    = __get_obj_int(ctx, idx, "shift");
    p->stabil   = __get_obj_int(ctx, idx, "stabil");
    p->jitcnt   = __get_obj_int(ctx, idx, "jitcnt");
    p->calcnt   = __get_obj_int(ctx, idx, "calcnt");
    p->errcnt   = __get_obj_int(ctx, idx, "errcnt");
    p->stbcnt   = __get_obj_int(ctx, idx, "stbcnt");

    os_objzero(&p->time);

    __obj_get(ctx, idx, __get_timeval, &p->time, "time");

    return 0;
}

static inline int
__set_timex(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct timex *p = (struct timex *)obj;
    
    __set_obj_uint(ctx, idx, "modes", p->modes);
    __set_obj_int(ctx, idx, "offset", p->offset);
#if 0
    __set_obj_int(ctx, idx, "frequency", p->frequency);
#endif
    __set_obj_int(ctx, idx, "maxerror", p->maxerror);
    __set_obj_int(ctx, idx, "esterror", p->esterror);
    __set_obj_int(ctx, idx, "status", p->status);
    __set_obj_int(ctx, idx, "constant", p->constant);
    __set_obj_int(ctx, idx, "precision", p->precision);
    __set_obj_int(ctx, idx, "tolerance", p->tolerance);
    __set_obj_int(ctx, idx, "tick", p->tick);
    __set_obj_int(ctx, idx, "ppsfreq", p->ppsfreq);
    __set_obj_int(ctx, idx, "jitter", p->jitter);
    __set_obj_int(ctx, idx, "shift", p->shift);
    __set_obj_int(ctx, idx, "stabil", p->stabil);
    __set_obj_int(ctx, idx, "jitcnt", p->jitcnt);
    __set_obj_int(ctx, idx, "calcnt", p->calcnt);
    __set_obj_int(ctx, idx, "errcnt", p->errcnt);
    __set_obj_int(ctx, idx, "stbcnt", p->stbcnt);

    __obj_set(ctx, idx, __set_timeval, &p->time, "time");

    return 0;
}

static inline int
__get_rusage(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct rusage *p = (struct rusage *)obj; os_objzero(p);
    
    p->ru_maxrss = __get_obj_int(ctx, idx, "maxrss");
    p->ru_ixrss  = __get_obj_int(ctx, idx, "ixrss");
    p->ru_idrss  = __get_obj_int(ctx, idx, "idrss");
    p->ru_isrss  = __get_obj_int(ctx, idx, "isrss");
    p->ru_minflt = __get_obj_int(ctx, idx, "minflt");
    p->ru_majflt = __get_obj_int(ctx, idx, "majflt");
    p->ru_nswap  = __get_obj_int(ctx, idx, "nswap");
    p->ru_inblock= __get_obj_int(ctx, idx, "inblock");
    p->ru_oublock= __get_obj_int(ctx, idx, "oublock");
    p->ru_msgsnd = __get_obj_int(ctx, idx, "msgsnd");
    p->ru_msgrcv = __get_obj_int(ctx, idx, "msgrcv");
    p->ru_nsignals = __get_obj_int(ctx, idx, "nsignals");
    p->ru_nvcsw  = __get_obj_int(ctx, idx, "nvcsw");
    p->ru_nivcsw = __get_obj_int(ctx, idx, "nivcsw");

    os_objzero(&p->ru_utime);
    os_objzero(&p->ru_stime);

    __obj_get(ctx, idx, __get_timeval, &p->ru_utime, "utime");
    __obj_get(ctx, idx, __get_timeval, &p->ru_stime, "stime");

    return 0;
}

static inline int
__set_rusage(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct rusage *p = (struct rusage *)obj;
    
    __set_obj_int(ctx, idx, "maxrss", p->ru_maxrss);
    __set_obj_int(ctx, idx, "ixrss", p->ru_ixrss);
    __set_obj_int(ctx, idx, "idrss", p->ru_idrss);
    __set_obj_int(ctx, idx, "isrss", p->ru_isrss);
    __set_obj_int(ctx, idx, "minflt", p->ru_minflt);
    __set_obj_int(ctx, idx, "majflt", p->ru_majflt);
    __set_obj_int(ctx, idx, "nswap", p->ru_nswap);
    __set_obj_int(ctx, idx, "inblock", p->ru_inblock);
    __set_obj_int(ctx, idx, "oublock", p->ru_oublock);
    __set_obj_int(ctx, idx, "msgsnd", p->ru_msgsnd);
    __set_obj_int(ctx, idx, "msgrcv", p->ru_msgrcv);
    __set_obj_int(ctx, idx, "nsignals", p->ru_nsignals);
    __set_obj_int(ctx, idx, "nvcsw", p->ru_nvcsw);
    __set_obj_int(ctx, idx, "nivcsw", p->ru_nivcsw);
    
    __obj_set(ctx, idx, __set_timeval, &p->ru_utime, "utime");
    __obj_set(ctx, idx, __set_timeval, &p->ru_stime, "stime");

    return 0;
}

static inline int
__get_vtimes(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct vtimes *p = (struct vtimes *)obj; os_objzero(p);
    
    p->vm_maxrss= __get_obj_int(ctx, idx, "maxrss");
    p->vm_ixrss = __get_obj_int(ctx, idx, "ixrss");
    p->vm_idsrss= __get_obj_int(ctx, idx, "idsrss");
    p->vm_minflt= __get_obj_int(ctx, idx, "minflt");
    p->vm_majflt= __get_obj_int(ctx, idx, "majflt");
    p->vm_nswap = __get_obj_int(ctx, idx, "nswap");
    p->vm_inblk = __get_obj_int(ctx, idx, "inblk");
    p->vm_oublk = __get_obj_int(ctx, idx, "oublk");
    p->vm_oublk = __get_obj_int(ctx, idx, "oublk");
    p->vm_utime = __get_obj_int(ctx, idx, "utime");
    p->vm_stime = __get_obj_int(ctx, idx, "stime");

    return 0;
}

static inline int
__set_vtimes(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct vtimes *p = (struct vtimes *)obj;
    
    __set_obj_int(ctx, idx, "maxrss", p->vm_maxrss);
    __set_obj_int(ctx, idx, "ixrss", p->vm_ixrss);
    __set_obj_int(ctx, idx, "idsrss", p->vm_idsrss);
    __set_obj_int(ctx, idx, "minflt", p->vm_minflt);
    __set_obj_int(ctx, idx, "majflt", p->vm_majflt);
    __set_obj_int(ctx, idx, "nswap", p->vm_nswap);
    __set_obj_int(ctx, idx, "inblock", p->vm_inblk);
    __set_obj_int(ctx, idx, "oublock", p->vm_oublk);
    __set_obj_int(ctx, idx, "utime", p->vm_utime);
    __set_obj_int(ctx, idx, "stime", p->vm_stime);

    return 0;
}

static inline int
__get_flock(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct flock *p = (struct flock *)obj; os_objzero(p);
    
    p->l_type   = __get_obj_int(ctx, idx, "type");
    p->l_whence = __get_obj_int(ctx, idx, "whence");
    p->l_start  = __get_obj_uint(ctx, idx, "start");
    p->l_len    = __get_obj_uint(ctx, idx, "len");
    p->l_pid    = __get_obj_uint(ctx, idx, "pid");

    return 0;
}

static inline int
__set_flock(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct flock *p = (struct flock *)obj;
    
    __set_obj_int(ctx, idx, "type", p->l_type);
    __set_obj_int(ctx, idx, "whence", p->l_whence);
    __set_obj_uint(ctx, idx, "start", p->l_start);
    __set_obj_uint(ctx, idx, "len", p->l_len);
    __set_obj_uint(ctx, idx, "pid", p->l_pid);

    return 0;
}

static inline int
__get_termios(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct termios *p = (struct termios *)obj; os_objzero(p);
    int i;
    
    p->c_iflag  = __get_obj_uint(ctx, idx, "iflag");
    p->c_oflag  = __get_obj_uint(ctx, idx, "oflag");
    p->c_cflag  = __get_obj_uint(ctx, idx, "cflag");
    p->c_lflag  = __get_obj_uint(ctx, idx, "lflag");

    duk_get_prop_string(ctx, idx, "cc");
    if (duk_is_array(ctx, -1)) {
        for (i=0; i<NCCS; i++) {
            p->c_cc[i] = __get_array_uint(ctx, -1, i);
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
    
    __set_obj_uint(ctx, idx, "iflag", p->c_iflag);
    __set_obj_uint(ctx, idx, "oflag", p->c_oflag);
    __set_obj_uint(ctx, idx, "cflag", p->c_cflag);
    __set_obj_uint(ctx, idx, "lflag", p->c_lflag);

    duk_get_prop_string(ctx, idx, "cc");
    if (duk_is_array(ctx, -1)) {
        for (i=0; i<NCCS; i++) {
            __set_array_uint(ctx, -1, i, p->c_cc[i]);
        }
    }
    duk_pop(ctx);

    return 0;
}

static inline int
__get_div(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    div_t *p = (div_t *)obj; os_objzero(p);
    
    p->quot = __get_obj_int(ctx, idx, "quot");
    p->rem  = __get_obj_int(ctx, idx, "rem");

    return 0;
}

static inline int
__set_div(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    div_t *p = (div_t *)obj;
    
    __set_obj_int(ctx, idx, "quot", p->quot);
    __set_obj_int(ctx, idx, "rem", p->rem);

    return 0;
}

static inline int
__get_rlimit(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct rlimit *p = (struct rlimit *)obj; os_objzero(p);
    
    p->rlim_cur = __get_obj_int(ctx, idx, "cur");
    p->rlim_max  = __get_obj_int(ctx, idx, "max");

    return 0;
}

static inline int
__set_rlimit(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct rlimit *p = (struct rlimit *)obj;
    
    __set_obj_int(ctx, idx, "cur", p->rlim_cur);
    __set_obj_int(ctx, idx, "max", p->rlim_max);

    return 0;
}

static inline int
__get_sched_param(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct sched_param *p = (struct sched_param *)obj; os_objzero(p);
    
    p->sched_priority = __get_obj_int(ctx, idx, "priority");

    return 0;
}

static inline int
__set_sched_param(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct sched_param *p = (struct sched_param *)obj;
    
    __set_obj_int(ctx, idx, "priority", p->sched_priority);

    return 0;
}

#if duk_LIBC_SIG
static inline int
__get_sigaction(duk_context *ctx, duk_idx_t idx, int sig, duk_object_t obj)
{
    struct sigaction *p = (struct sigaction *)obj; os_objzero(p);
    
    p->sa_mask  = *(sigset_t *)__get_obj_buffer(ctx, idx, "mask", NULL);
    p->sa_flags = __get_obj_int(ctx, idx, "flags");
    
    duk_get_prop_string(ctx, idx, "handler");
    if (duk_is_function(ctx, -1)) {
        p->sa_handler = libc_sig_handler;

        libc_sig_name[sig] = __get_obj_string(ctx, idx, "name", NULL);
    }
    else if (duk_is_number(ctx, -1)) {
        __sighandler_t action = (__sighandler_t)(uintptr_t)duk_require_int(ctx, -1);
        if (SIG_DFL!=action && SIG_IGN!=action) {
            action = SIG_DFL;
        }
        
        p->sa_handler = action;
        libc_sig_name[sig] = (char *)action;
    }
    duk_pop(ctx);

    return 0;
}

static inline int
__set_sigaction(duk_context *ctx, duk_idx_t idx, int sig, duk_object_t obj)
{
    struct sigaction *p = (struct sigaction *)obj;
    
    *(sigset_t *)__get_obj_buffer(ctx, idx, "mask", NULL) = p->sa_mask;
    __set_obj_int(ctx, idx, "flags", p->sa_flags);

    if (SIG_DFL==p->sa_handler || SIG_IGN==p->sa_handler) {
        __set_obj_int(ctx, idx, "handler", (uintptr_t)p->sa_handler);
    } else {
        duk_push_global_object(ctx);                    // global
        duk_get_prop_string(ctx, -1, libc_sig_name[sig]);  // global function/undefined
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
    
    p->e_termination = __get_obj_int(ctx, idx, "termination");
    p->e_exit  = __get_obj_int(ctx, idx, "exit");

    return 0;
}

static inline int
__set_exit_status(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct exit_status *p = (struct exit_status *)obj;
    
    __set_obj_int(ctx, idx, "termination", p->e_termination);
    __set_obj_int(ctx, idx, "exit", p->e_exit);

    return 0;
}

static inline int
__get_utmp(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct utmp *p = (struct utmp *)obj; os_objzero(p);
    int i;
    
    p->ut_type  = __get_obj_int(ctx, idx, "type");
    p->ut_pid   = __get_obj_int(ctx, idx, "pid");
    p->ut_session = __get_obj_int(ctx, idx, "session");
    __copy_obj_string(ctx, idx, "line", p->ut_line, sizeof(p->ut_line)-1);
    __copy_obj_string(ctx, idx, "id", p->ut_id, sizeof(p->ut_id)-1);
    __copy_obj_string(ctx, idx, "user", p->ut_user, sizeof(p->ut_user)-1);
    __copy_obj_string(ctx, idx, "host", p->ut_host, sizeof(p->ut_host)-1);

    __obj_get(ctx, idx, __get_exit_status, &p->ut_exit, "exit");

    duk_get_prop_string(ctx, idx, "addr_v6");
    if (duk_is_array(ctx, -1)) {
        for (i=0; i<4; i++) {
            p->ut_addr_v6[i] = __get_array_uint(ctx, -1, i);
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
    
    __set_obj_int(ctx, idx, "type", p->ut_type);
    __set_obj_int(ctx, idx, "pid", p->ut_pid);
    __set_obj_int(ctx, idx, "session", p->ut_session);
    __set_obj_string(ctx, idx, "line", p->ut_line);
    __set_obj_string(ctx, idx, "id", p->ut_id);
    __set_obj_string(ctx, idx, "user", p->ut_user);
    __set_obj_string(ctx, idx, "host", p->ut_host);

    __obj_get(ctx, idx, __set_exit_status, &p->ut_exit, "exit");

    duk_get_prop_string(ctx, idx, "addr_v6");
    if (duk_is_array(ctx, -1)) {
        for (i=0; i<4; i++) {
             __set_array_uint(ctx, -1, i, p->ut_addr_v6[i]);
        }
    }
    duk_pop(ctx);

    return 0;
}

static inline int
__get_passwd(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct passwd *p = (struct passwd *)obj; os_objzero(p);
    
    p->pw_uid       = __get_obj_uint(ctx, idx, "uid");
    p->pw_gid       = __get_obj_uint(ctx, idx, "gid");
    p->pw_name      = __get_obj_string(ctx, idx, "name", NULL);
    p->pw_passwd    = __get_obj_string(ctx, idx, "passwd", NULL);
    p->pw_gecos     = __get_obj_string(ctx, idx, "gecos", NULL);
    p->pw_dir       = __get_obj_string(ctx, idx, "dir", NULL);
    p->pw_shell     = __get_obj_string(ctx, idx, "shell", NULL);

    return 0;
}

static inline int
__set_passwd(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct passwd *p = (struct passwd *)obj;
    
    __set_obj_uint(ctx, idx, "uid", p->pw_uid);
    __set_obj_uint(ctx, idx, "gid", p->pw_gid);
    __set_obj_string(ctx, idx, "name", p->pw_name);
    __set_obj_string(ctx, idx, "passwd", p->pw_passwd);
    __set_obj_string(ctx, idx, "gecos", p->pw_gecos);
    __set_obj_string(ctx, idx, "dir", p->pw_dir);
    __set_obj_string(ctx, idx, "shell", p->pw_shell);

    return 0;
}

static inline int
__set_group(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct group *p = (struct group *)obj;
    int i, count;
    char *tmp;
    
    __set_obj_uint(ctx, idx, "gid", p->gr_gid);
    __set_obj_string(ctx, idx, "name", p->gr_name);

    duk_get_prop_string(ctx, idx, "mem");
    if (duk_is_array(ctx, -1)) {
        for (count = 0, tmp = p->gr_mem[0]; tmp; count++, tmp++) {
            __set_array_string(ctx, -1, count, tmp);
        }
    }
    duk_pop(ctx);

    return 0;
}

static inline int
__get_utsname(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct utsname *p = (struct utsname *)obj; os_objzero(p);
    
    __copy_obj_string(ctx, idx, "sysname", p->sysname, sizeof(p->sysname)-1);
    __copy_obj_string(ctx, idx, "release", p->release, sizeof(p->release)-1);
    __copy_obj_string(ctx, idx, "version", p->version, sizeof(p->version)-1);
    __copy_obj_string(ctx, idx, "machine", p->machine, sizeof(p->machine)-1);
    __copy_obj_string(ctx, idx, "nodename", p->nodename, sizeof(p->nodename)-1);

    return 0;
}

static inline int
__set_utsname(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct utsname *p = (struct utsname *)obj;
    
    __set_obj_string(ctx, idx, "sysname", p->sysname);
    __set_obj_string(ctx, idx, "release", p->release);
    __set_obj_string(ctx, idx, "version", p->version);
    __set_obj_string(ctx, idx, "machine", p->machine);
    __set_obj_string(ctx, idx, "nodename", p->nodename);

    return 0;
}

static inline int
__get_fstab(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct fstab *p = (struct fstab *)obj; os_objzero(p);
    
    p->fs_freq      = __get_obj_int(ctx, idx, "freq");
    p->fs_passno    = __get_obj_int(ctx, idx, "passno");
    p->fs_spec      = __get_obj_string(ctx, idx, "spec", NULL);
    p->fs_file      = __get_obj_string(ctx, idx, "file", NULL);
    p->fs_vfstype   = __get_obj_string(ctx, idx, "vfstype", NULL);
    p->fs_mntops    = __get_obj_string(ctx, idx, "mntops", NULL);
    p->fs_type      = __get_obj_string(ctx, idx, "type", NULL);

    return 0;
}

static inline int
__set_fstab(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct fstab *p = (struct fstab *)obj;
    
    __set_obj_int(ctx, idx, "freq", p->fs_freq);
    __set_obj_int(ctx, idx, "passno", p->fs_passno);
    __set_obj_string(ctx, idx, "spec", p->fs_spec);
    __set_obj_string(ctx, idx, "file", p->fs_file);
    __set_obj_string(ctx, idx, "vfstype", p->fs_vfstype);
    __set_obj_string(ctx, idx, "mntops", p->fs_mntops);
    __set_obj_string(ctx, idx, "type", (char *)p->fs_type);

    return 0;
}

static inline int
__get_mntent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct mntent *p = (struct mntent *)obj; os_objzero(p);
    
    p->mnt_freq     = __get_obj_int(ctx, idx, "freq");
    p->mnt_passno   = __get_obj_int(ctx, idx, "passno");
    p->mnt_fsname   = __get_obj_string(ctx, idx, "fsname", NULL);
    p->mnt_dir      = __get_obj_string(ctx, idx, "dir", NULL);
    p->mnt_type     = __get_obj_string(ctx, idx, "type", NULL);
    p->mnt_opts     = __get_obj_string(ctx, idx, "opts", NULL);

    return 0;
}

static inline int
__set_mntent(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct mntent *p = (struct mntent *)obj;
    
    __set_obj_int(ctx, idx, "freq", p->mnt_freq);
    __set_obj_int(ctx, idx, "passno", p->mnt_passno);
    __set_obj_string(ctx, idx, "fsname", p->mnt_fsname);
    __set_obj_string(ctx, idx, "dir", p->mnt_dir);
    __set_obj_string(ctx, idx, "type", p->mnt_type);
    __set_obj_string(ctx, idx, "opts", p->mnt_opts);

    return 0;
}
/******************************************************************************/
#endif /* __LIBC_H_46966fd8100c4718872a30b740b9561d__ */