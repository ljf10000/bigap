STATIC const dukc_func_entry_t js_libc_func[] = {
#if js_LIBC_LINUX
    JS_FUNC(timerfd_create),
    JS_FUNC(timerfd_settime),
    JS_FUNC(timerfd_gettime),
    JS_FUNC(eventfd),
    JS_FUNC(signalfd),
    JS_FUNC(epoll_create),
    JS_FUNC(epoll_create1),
    JS_FUNC(epoll_wait),
    JS_FUNC(epoll_pwait),
    JS_FUNC(epoll_ctl),
    JS_FUNC(inotify_init),
    JS_FUNC(inotify_init1),
    JS_FUNC(inotify_add_watch),
    JS_FUNC(inotify_rm_watch),
    
#endif

#if js_LIBC_ERROR
    // 2.3 Error Messages
    JS_FUNC(strerror),
#endif

#if js_LIBC_CTYPE
    // 4.1 Classification of Characters
    JS_FUNC(islower),
    JS_FUNC(isupper),
    JS_FUNC(isalpha),
    JS_FUNC(isdigit),
    JS_FUNC(isalnum),
    JS_FUNC(isxdigit),
    JS_FUNC(ispunct),
    JS_FUNC(isspace),
    JS_FUNC(isblank),
    JS_FUNC(isgraph),
    JS_FUNC(isprint),
    JS_FUNC(iscntrl),
    JS_FUNC(isascii),
    
    // 4.2 Case Conversion
    JS_FUNC(tolower),
    JS_FUNC(toupper),
    JS_FUNC(toascii),
#endif

    // 5.5 String/Array Comparison
#if js_LIBC_MEMORY
    JS_FUNC(memcmp),
#endif

    // 12 Input/Output on Streams
#if js_LIBC_PRIVATE
    JS_FUNC(fsize),
    JS_FUNC(fexist),
#endif
    JS_FUNC(fopen),
    JS_FUNC(fclose),
    JS_FUNC(fread),
    JS_FUNC(freadEx),
    JS_FUNC(fwrite),
    JS_FUNC(feof),
    JS_FUNC(ferror),
    JS_FUNC(ftell),
    JS_FUNC(fseek),
    JS_FUNC(fflush),

    // 13.1 Opening and Closing Files
    JS_FUNC(open),
    JS_FUNC(close),

    // 13.2 Input and Output Primitives
    JS_FUNC(read),
    JS_FUNC(readEx),
    JS_FUNC(write),
    JS_FUNC(pread),
    JS_FUNC(preadEx),
    JS_FUNC(pwrite),

    // 13.3 Setting the File Position of a Descriptor
    JS_FUNC(lseek),

    // 13.4 Descriptors and Streams
    JS_FUNC(fdopen),
    JS_FUNC(fileno),

    // 13.6 Fast Scatter-Gather I/O
    JS_FUNC(readv),
    JS_FUNC(writev),

    // 13.7 Memory-mapped I/O
#if js_LIBC_MEMORY
    JS_FUNC(mmap),
    JS_FUNC(munmap),
    JS_FUNC(msync),
    JS_FUNC(mremap),
    JS_FUNC(madvise),
#endif

    // 13.8 Waiting for Input or Output
#if js_LIBC_FDSET
    JS_FUNC(FD_NEW),
    JS_FUNC(FD_ZERO),
    JS_FUNC(FD_SET),
    JS_FUNC(FD_CLR),
    JS_FUNC(FD_ISSET),
#endif
    JS_FUNC(select),

    // 13.9 Synchronizing I/O operations
    JS_FUNC(sync),
    JS_FUNC(fsync),

    // 13.11 Control Operations on Files
    JS_FUNC(fcntl),

    // 13.12 Duplicating Descriptors
    JS_FUNC(dup),
    JS_FUNC(dup2),

    // 13.17 Generic I/O Control operations
    JS_FUNC(ioctl),
    
    // 14 File System Interface
    // 14.1 Working Directory
    JS_FUNC(getcwd),
    JS_FUNC(chdir),
    JS_FUNC(fchdir),

    // 14.2 Accessing Directories
    JS_FUNC(opendir),
    JS_FUNC(fdopendir),
    JS_FUNC(dirfd),
    JS_FUNC(readdir),
    JS_FUNC(closedir),
    JS_FUNC(rewinddir),
    JS_FUNC(telldir),
    JS_FUNC(seekdir),

    // 14.4 Hard Links
    JS_FUNC(link),
    
    // 14.5 Symbolic Links
    JS_FUNC(symlink),
    JS_FUNC(readlink),
    JS_FUNC(realpath),
    
    // 14.6 Deleting Files
    JS_FUNC(unlink),
    JS_FUNC(rmdir),
    JS_FUNC(remove),
    
    // 14.7 Renaming Files
    JS_FUNC(rename),
    
    // 14.8 Creating Directories
    JS_FUNC(mkdir),
    
    // 14.9 File Attributes
    // 14.9.2 Reading the Attributes of a File
    JS_FUNC(stat),
    JS_FUNC(lstat),
    JS_FUNC(fstat),

    // 14.9.3 Testing the Type of a File
#if 0
    JS_FUNC(S_ISDIR),
    JS_FUNC(S_ISCHR),
    JS_FUNC(S_ISBLK),
    JS_FUNC(S_ISREG),
    JS_FUNC(S_ISFIFO),
    JS_FUNC(S_ISLNK),
    JS_FUNC(S_ISSOCK),
    JS_FUNC(S_TYPEISMQ),
    JS_FUNC(S_TYPEISSEM),
    JS_FUNC(S_TYPEISSHM),
#endif

    // 14.9.4 File Owner
    JS_FUNC(chown),
    JS_FUNC(fchown),

    // 14.9.7 Assigning File Permissions
    JS_FUNC(umask),
    JS_FUNC(chmod),
    JS_FUNC(fchmod),

    // 14.9.8 Testing Permission to Access a File
    JS_FUNC(access),

    // 14.9.9 File Times
    JS_FUNC(utime),

    // 14.9.10 File Size
    JS_FUNC(truncate),
    JS_FUNC(ftruncate),

    // 14.10 Making Special Files
    JS_FUNC(mknod),

    // 14.11 Temporary Files
    JS_FUNC(tmpfile),
    JS_FUNC(mkstemp),
    JS_FUNC(mkdtemp),

    // 15 Pipes and FIFOs
    // 15.1 Creating a Pipe
    JS_FUNC(pipe),

    // 15.2 Pipe to a Subprocess
    JS_FUNC(popen),
    JS_FUNC(pclose),

    // 15.3 FIFO Special Files
    JS_FUNC(mkfifo),

    // 16 Sockets
    // 16.3.2 Setting the Address of a Socket
    JS_FUNC(bind),

    // 16.3.3 Reading the Address of a Socket
    JS_FUNC(getsockname),

    // 16.4 Interface Naming
    JS_FUNC(if_nametoindex),
    JS_FUNC(if_indextoname),
    JS_FUNC(if_nameindex),

    // 16.6 The Internet Namespace
    JS_FUNC(inet_aton),
    JS_FUNC(inet_addr),
    JS_FUNC(inet_network),
    JS_FUNC(inet_ntoa),
    JS_FUNC(inet_makeaddr),
    JS_FUNC(inet_lnaof),
    JS_FUNC(inet_netof),
    JS_FUNC(gethostbyname),
    JS_FUNC(gethostbyname2),
    JS_FUNC(gethostbyaddr),
    JS_FUNC(sethostent),
    JS_FUNC(gethostent),
    JS_FUNC(endhostent),
    JS_FUNC(getservbyname),
    JS_FUNC(getservbyport),
    JS_FUNC(setservent),
    JS_FUNC(getservent),
    JS_FUNC(endservent),
    JS_FUNC(htons),
    JS_FUNC(ntohs),
    JS_FUNC(htonl),
    JS_FUNC(ntohl),
    JS_FUNC(getprotobyname),
    JS_FUNC(getprotobynumber),
    JS_FUNC(setprotoent),
    JS_FUNC(getprotoent),
    JS_FUNC(endprotoent),

    // 16.8 Opening and Closing Sockets
    JS_FUNC(socket),
    JS_FUNC(shutdown),
    JS_FUNC(socketpair),

    // 16.9 Using Sockets with Connections
    JS_FUNC(connect),
    JS_FUNC(listen),
    JS_FUNC(accept),
    JS_FUNC(getpeername),
    JS_FUNC(send),
    JS_FUNC(recv),

    // 16.10 Datagram Socket Operations
    JS_FUNC(sendto),
    JS_FUNC(recvfrom),

    // 16.12 Socket Options
    JS_FUNC(getsockopt),
    JS_FUNC(setsockopt),

    // 16.13 Networks Database
    JS_FUNC(getnetbyname),
    JS_FUNC(getnetbyaddr),
    JS_FUNC(setnetent),
    JS_FUNC(getnetent),
    JS_FUNC(endnetent),

#if js_LIBC_TTY
    // 17 Low-Level Terminal Interface
    // 17.1 Identifying Terminals
    JS_FUNC(isatty),
    JS_FUNC(ttyname),

    // 17.4 Terminal Modes
    JS_FUNC(tcgetattr),
    JS_FUNC(tcsetattr),
    JS_FUNC(cfgetospeed),
    JS_FUNC(cfgetispeed),
    JS_FUNC(cfsetospeed),
    JS_FUNC(cfsetispeed),
    JS_FUNC(cfsetspeed),
    JS_FUNC(cfmakeraw),

    // 17.6 Line Control Functions
    JS_FUNC(tcsendbreak),
    JS_FUNC(tcdrain),
    JS_FUNC(tcflush),
    JS_FUNC(tcflow),

    // 17.8 Pseudo-Terminals
    JS_FUNC(getpt),
    JS_FUNC(grantpt),
    JS_FUNC(unlockpt),
    JS_FUNC(ptsname),
#endif

#if js_LIBC_LOG
    // 18 Syslog
    // 18.2 Submitting Syslog Messages
    JS_FUNC(openlog),
    JS_FUNC(syslog),
    JS_FUNC(closelog),
    JS_FUNC(setlogmask),
    JS_FUNC(LOG_MASK),
    JS_FUNC(LOG_UPTO),
#endif

#if js_LIBC_MATH
    // 19 Mathematics
    // 19.2 Trigonometric Functions
    JS_FUNC(sin),
    JS_FUNC(cos),
    JS_FUNC(tan),
    JS_FUNC(asin),
    JS_FUNC(acos),
    JS_FUNC(atan),
    JS_FUNC(atan2),
    JS_FUNC(exp),
    JS_FUNC(exp2),
#ifndef __OPENWRT__
    JS_FUNC(exp10),
#endif
    JS_FUNC(log),
    JS_FUNC(log2),
    JS_FUNC(log10),
    JS_FUNC(logb),
    JS_FUNC(ilogb),
    JS_FUNC(pow),
    JS_FUNC(sqrt),
    JS_FUNC(cbrt),
    JS_FUNC(hypot),
    JS_FUNC(expm1),
    JS_FUNC(log1p),
    JS_FUNC(sinh),
    JS_FUNC(cosh),
    JS_FUNC(tanh),
    JS_FUNC(asinh),
    JS_FUNC(acosh),
    JS_FUNC(atanh),
    JS_FUNC(erf),
    JS_FUNC(erfc),
    JS_FUNC(lgamma),
    JS_FUNC(gamma),
    JS_FUNC(tgamma),
#ifndef __OPENWRT__
    JS_FUNC(j0),
    JS_FUNC(j1),
    JS_FUNC(jn),
    JS_FUNC(y0),
    JS_FUNC(y1),
    JS_FUNC(yn),
#endif
    JS_FUNC(rand),
    JS_FUNC(srand),
    JS_FUNC(div),
    JS_FUNC(fpclassify),
    JS_FUNC(isfinite),
    JS_FUNC(isnormal),
    JS_FUNC(isnan),
    JS_FUNC(isinf),
    JS_FUNC(abs),
    JS_FUNC(fabs),
    JS_FUNC(frexp),
    JS_FUNC(ldexp),
    JS_FUNC(scalb),
    JS_FUNC(ceil),
    JS_FUNC(floor),
    JS_FUNC(trunc),
    JS_FUNC(rint),
    JS_FUNC(nearbyint),
    JS_FUNC(round),
    JS_FUNC(fmod),
    JS_FUNC(drem),
    JS_FUNC(remainder),
    JS_FUNC(copysign),
    JS_FUNC(signbit),
    JS_FUNC(nextafter),
#ifndef __OPENWRT__
    JS_FUNC(nexttoward),
#endif
    JS_FUNC(nan),
    JS_FUNC(isgreater),
    JS_FUNC(isgreaterequal),
    JS_FUNC(isless),
    JS_FUNC(islessequal),
    JS_FUNC(islessgreater),
    JS_FUNC(isunordered),
    JS_FUNC(fmin),
    JS_FUNC(fmax),
    JS_FUNC(fdim),
    JS_FUNC(fma),
#endif
    
#if js_LIBC_TIME
    // 21 Date and Time
    JS_FUNC(difftime),
    JS_FUNC(clock),
    JS_FUNC(times),
    JS_FUNC(time),
    JS_FUNC(gettimeofday),
    JS_FUNC(settimeofday),
    JS_FUNC(adjtime),
    JS_FUNC(localtime),
    JS_FUNC(gmtime),
    JS_FUNC(mktime),
    JS_FUNC(timelocal),
    JS_FUNC(timegm),
#ifndef __OPENWRT__
    JS_FUNC(ntp_gettime),
    JS_FUNC(ntp_adjtime),
#endif
    JS_FUNC(asctime),
    JS_FUNC(ctime),
    JS_FUNC(strftime),
    JS_FUNC(setitimer),
    JS_FUNC(getitimer),
    JS_FUNC(alarm),
    JS_FUNC(sleep),
    JS_FUNC(nanosleep),
#endif

    // 22 Resource Usage And Limitation
    JS_FUNC(getrusage),
#if js_LIBC_VTIME
    JS_FUNC(vtimes),
#endif
    JS_FUNC(getrlimit),
    JS_FUNC(setrlimit),
    JS_FUNC(ulimit),
#ifndef __OPENWRT__
    JS_FUNC(vlimit),
#endif
    JS_FUNC(sched_setscheduler),
    JS_FUNC(sched_getscheduler),
    JS_FUNC(sched_setparam),
    JS_FUNC(sched_getparam),
    JS_FUNC(sched_get_priority_min),
    JS_FUNC(sched_get_priority_max),
    JS_FUNC(sched_rr_get_interval),
    JS_FUNC(sched_yield),
    JS_FUNC(getpriority),
    JS_FUNC(setpriority),
    JS_FUNC(nice),
#ifdef _GNU_SOURCE
    JS_FUNC(CPU_NEW),
    JS_FUNC(CPU_ZERO),
    JS_FUNC(CPU_SET),
    JS_FUNC(CPU_CLR),
    JS_FUNC(CPU_ISSET),
    JS_FUNC(sched_getaffinity),
    JS_FUNC(sched_setaffinity),
#endif
    JS_FUNC(getpagesize),
#ifndef __OPENWRT__
    JS_FUNC(get_phys_pages),
    JS_FUNC(get_avphys_pages),
#endif
    JS_FUNC(get_nprocs_conf),
    JS_FUNC(get_nprocs),
#ifndef __OPENWRT__
    JS_FUNC(getloadavg),
#endif

#if js_LIBC_SIG
    // 24 Signal Handling
    JS_FUNC(strsignal),
    JS_FUNC(psignal),
    JS_FUNC(signal),
    JS_FUNC(sigaction),
    JS_FUNC(raise),
    JS_FUNC(kill),
    JS_FUNC(killpg),
    JS_FUNC(signewset),
    JS_FUNC(sigemptyset),
    JS_FUNC(sigfillset),
    JS_FUNC(sigaddset),
    JS_FUNC(sigdelset),
    JS_FUNC(sigismember),
    JS_FUNC(sigprocmask),
    JS_FUNC(sigpending),
    JS_FUNC(pause),
    JS_FUNC(sigsuspend),
#endif
    // 25 The Basic Program/System Interface
    JS_FUNC(getenv),
#if defined(_GNU_SOURCE) && 0
    JS_FUNC(secure_getenv),
#endif
    JS_FUNC(putenv),
    JS_FUNC(setenv),
    JS_FUNC(unsetenv),
    JS_FUNC(clearenv),
    JS_FUNC(exit),
#if js_LIBC_SIG
    JS_FUNC(atexit),
#endif
    JS_FUNC(abort),
    JS_FUNC(_exit),
    JS_FUNC(_Exit),
    
    // 26 Processes
    JS_FUNC(getpid),
    JS_FUNC(getppid),
    JS_FUNC(fork),
    JS_FUNC(vfork),
    JS_FUNC(execv),
    JS_FUNC(execve),
    JS_FUNC(execvp),
    JS_FUNC(waitpid),
    JS_FUNC(wait),
    JS_FUNC(wait4),
    JS_FUNC(WIFEXITED),
    JS_FUNC(WEXITSTATUS),
    JS_FUNC(WIFSIGNALED),
    JS_FUNC(WTERMSIG),
    JS_FUNC(WCOREDUMP),
    JS_FUNC(WIFSTOPPED),
    JS_FUNC(WSTOPSIG),

    // 27 Job Control
    JS_FUNC(setsid),
    JS_FUNC(getpgrp),
    JS_FUNC(getpgid),
    JS_FUNC(setpgid),
    JS_FUNC(tcgetpgrp),
    JS_FUNC(tcsetpgrp),

    // 29 Users and Groups
    JS_FUNC(getuid),
    JS_FUNC(getgid),
    JS_FUNC(geteuid),
    JS_FUNC(getegid),
    JS_FUNC(getgroups),
    JS_FUNC(seteuid),
    JS_FUNC(setuid),
    JS_FUNC(setreuid),
    JS_FUNC(setegid),
    JS_FUNC(setgid),
    JS_FUNC(setregid),
    JS_FUNC(setgroups),
    JS_FUNC(initgroups),
    JS_FUNC(getlogin),
    JS_FUNC(setutent),
    JS_FUNC(getutent),
    JS_FUNC(endutent),
    JS_FUNC(getutid),
    JS_FUNC(getutline),
    JS_FUNC(pututline),
    JS_FUNC(utmpname),
    JS_FUNC(updwtmp),
#if js_LIBC_UTIL
    JS_FUNC(login_tty),
    JS_FUNC(login),
    JS_FUNC(logout),
    JS_FUNC(logwtmp),
#endif
    JS_FUNC(getpwuid),
    JS_FUNC(getpwnam),
#if js_LIBC_PWENT
    JS_FUNC(fgetpwent),
    JS_FUNC(setpwent),
    JS_FUNC(getpwent),
    JS_FUNC(endpwent),
    JS_FUNC(putpwent),
#endif
#if js_LIBC_GWENT
    JS_FUNC(getgrgid),
    JS_FUNC(getgrnam),
    JS_FUNC(fgetgrent),
    JS_FUNC(setgrent),
    JS_FUNC(getgrent),
    JS_FUNC(endgrent),
#endif

    // 30 System Management
    JS_FUNC(gethostname),
    JS_FUNC(sethostname),
    JS_FUNC(getdomainname),
    JS_FUNC(setdomainname),
    JS_FUNC(gethostid),
    JS_FUNC(sethostid),
    JS_FUNC(uname),
#if js_LIBC_MOUNT
    JS_FUNC(setfsent),
    JS_FUNC(endfsent),
    JS_FUNC(getfsent),
    JS_FUNC(getfsspec),
    JS_FUNC(getfsfile),
#endif
    JS_FUNC(setmntent),
    JS_FUNC(endmntent),
    JS_FUNC(getmntent),
    JS_FUNC(addmntent),
    JS_FUNC(hasmntopt),
    JS_FUNC(mount),
    JS_FUNC(umount2),
    JS_FUNC(umount),
    
    // 31 System Configuration Parameters
    JS_FUNC(sysconf),
    JS_FUNC(pathconf),
    JS_FUNC(fpathconf),
    JS_FUNC(confstr),

    JS_FUNC_END
};

STATIC void
js_libcf_register(duk_context *ctx, duk_idx_t idx)
{
    js_put_functions(ctx, idx, js_libc_func);
}
