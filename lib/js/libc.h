#ifndef __LIBC_H_46966fd8100c4718872a30b740b9561d__
#define __LIBC_H_46966fd8100c4718872a30b740b9561d__
/******************************************************************************/
extern int
libc_get_sockaddr(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_sockaddr(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_get_linger(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_linger(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_get_dirent(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_dirent(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_get_stat(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_stat(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_get_if_nameindex(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_if_nameindex(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_hostent(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_servent(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_protoent(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_netent(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_get_utimbuf(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_utimbuf(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_get_timeval(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_timeval(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_get_itimerval(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_itimerval(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_get_timespec(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_timespec(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_get_itimerspec(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_itimerspec(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

#if js_LIBC_LINUX
extern int
libc_get_signalfd_siginfo(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_signalfd_siginfo(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_get_watcher_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_watcher_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_get_epoll_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_epoll_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_get_inotify_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_inotify_event(duk_context *ctx, duk_idx_t idx, duk_object_t obj);
#endif

extern int
libc_get_tms(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_tms(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_get_timezone(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_timezone(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_get_tm(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_tm(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_get_ntptimeval(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_ntptimeval(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_get_timex(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_timex(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_get_rusage(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_rusage(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

#if js_LIBC_VTIME
static inline int
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

static inline int
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

static inline int
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

static inline int
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

static inline int
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

static inline int
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

static inline int
libc_get_div(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    div_t *p = (div_t *)obj; os_objzero(p);
    
    p->quot = js_get_obj_int(ctx, idx, "quot");
    p->rem  = js_get_obj_int(ctx, idx, "rem");

    return 0;
}

static inline int
libc_set_div(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    div_t *p = (div_t *)obj;
    
    js_set_obj_int(ctx, idx, "quot", p->quot);
    js_set_obj_int(ctx, idx, "rem", p->rem);

    return 0;
}

static inline int
libc_get_rlimit(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct rlimit *p = (struct rlimit *)obj; os_objzero(p);
    
    p->rlim_cur = js_get_obj_int(ctx, idx, "cur");
    p->rlim_max  = js_get_obj_int(ctx, idx, "max");

    return 0;
}

static inline int
libc_set_rlimit(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct rlimit *p = (struct rlimit *)obj;
    
    js_set_obj_int(ctx, idx, "cur", p->rlim_cur);
    js_set_obj_int(ctx, idx, "max", p->rlim_max);

    return 0;
}

static inline int
libc_get_sched_param(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct sched_param *p = (struct sched_param *)obj; os_objzero(p);
    
    p->sched_priority = js_get_obj_int(ctx, idx, "priority");

    return 0;
}

static inline int
libc_set_sched_param(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct sched_param *p = (struct sched_param *)obj;
    
    js_set_obj_int(ctx, idx, "priority", p->sched_priority);

    return 0;
}

#if js_LIBC_SIG
extern void libc_sig_handler(int sig);

static inline int
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

static inline int
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

static inline int
libc_get_exit_status(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct exit_status *p = (struct exit_status *)obj; os_objzero(p);
    
    p->e_termination = js_get_obj_int(ctx, idx, "termination");
    p->e_exit  = js_get_obj_int(ctx, idx, "exit");

    return 0;
}

static inline int
libc_set_exit_status(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct exit_status *p = (struct exit_status *)obj;
    
    js_set_obj_int(ctx, idx, "termination", p->e_termination);
    js_set_obj_int(ctx, idx, "exit", p->e_exit);

    return 0;
}

static inline int
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

static inline int
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

static inline int
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

static inline int
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

static inline int
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

static inline int
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

static inline int
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
static inline int
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

static inline int
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

static inline int
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

static inline int
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
#endif /* __LIBC_H_46966fd8100c4718872a30b740b9561d__ */