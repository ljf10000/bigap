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
extern int
libc_get_vtimes(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_vtimes(duk_context *ctx, duk_idx_t idx, duk_object_t obj);
#endif

extern int
libc_get_flock(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_flock(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_get_termios(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_termios(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_get_div(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_div(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_get_rlimit(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_rlimit(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_get_sched_param(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_sched_param(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

#if js_LIBC_SIG
extern void 
libc_sig_handler(int sig);

extern int
libc_get_sigaction(duk_context *ctx, duk_idx_t idx, int sig, duk_object_t obj);

extern int
libc_set_sigaction(duk_context *ctx, duk_idx_t idx, int sig, duk_object_t obj);
#endif

extern int
libc_get_exit_status(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_exit_status(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_get_utmp(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_utmp(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_get_passwd(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_passwd(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_group(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_get_utsname(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_utsname(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

#if js_LIBC_MOUNT
extern int
libc_get_fstab(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_fstab(duk_context *ctx, duk_idx_t idx, duk_object_t obj);
#endif

extern int
libc_get_mntent(duk_context *ctx, duk_idx_t idx, duk_object_t obj);

extern int
libc_set_mntent(duk_context *ctx, duk_idx_t idx, duk_object_t obj);
/******************************************************************************/
#endif /* __LIBC_H_46966fd8100c4718872a30b740b9561d__ */