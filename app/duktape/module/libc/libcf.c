static const dukc_func_entry_t libc_func[] = {
#if duk_LIBC_LINUX
    LIB_FUNC(timerfd_create),
    LIB_FUNC(timerfd_settime),
    LIB_FUNC(timerfd_gettime),
    LIB_FUNC(eventfd),
    LIB_FUNC(signalfd),
    LIB_FUNC(epoll_create),
    LIB_FUNC(epoll_create1),
    LIB_FUNC(epoll_wait),
    LIB_FUNC(epoll_pwait),
    LIB_FUNC(epoll_ctl),
    LIB_FUNC(inotify_init),
    LIB_FUNC(inotify_init1),
    LIB_FUNC(inotify_add_watch),
    LIB_FUNC(inotify_rm_watch),
    
#endif

#if duk_LIBC_ERROR
    // 2.3 Error Messages
    LIB_FUNC(strerror),
#endif

#if duk_LIBC_CTYPE
    // 4.1 Classification of Characters
    LIB_FUNC(islower),
    LIB_FUNC(isupper),
    LIB_FUNC(isalpha),
    LIB_FUNC(isdigit),
    LIB_FUNC(isalnum),
    LIB_FUNC(isxdigit),
    LIB_FUNC(ispunct),
    LIB_FUNC(isspace),
    LIB_FUNC(isblank),
    LIB_FUNC(isgraph),
    LIB_FUNC(isprint),
    LIB_FUNC(iscntrl),
    LIB_FUNC(isascii),
    
    // 4.2 Case Conversion
    LIB_FUNC(tolower),
    LIB_FUNC(toupper),
    LIB_FUNC(toascii),
#endif

    // 5.5 String/Array Comparison
#if duk_LIBC_MEMORY
    LIB_FUNC(memcmp),
#endif

    // 12 Input/Output on Streams
#if duk_LIBC_PRIVATE
    LIB_FUNC(fsize),
    LIB_FUNC(fexist),
#endif
    LIB_FUNC(fopen),
    LIB_FUNC(fclose),
    LIB_FUNC(fread),
    LIB_FUNC(freadEx),
    LIB_FUNC(fwrite),
    LIB_FUNC(feof),
    LIB_FUNC(ferror),
    LIB_FUNC(ftell),
    LIB_FUNC(fseek),
    LIB_FUNC(fflush),

    // 13.1 Opening and Closing Files
    LIB_FUNC(open),
    LIB_FUNC(close),

    // 13.2 Input and Output Primitives
    LIB_FUNC(read),
    LIB_FUNC(readEx),
    LIB_FUNC(write),
    LIB_FUNC(pread),
    LIB_FUNC(preadEx),
    LIB_FUNC(pwrite),

    // 13.3 Setting the File Position of a Descriptor
    LIB_FUNC(lseek),

    // 13.4 Descriptors and Streams
    LIB_FUNC(fdopen),
    LIB_FUNC(fileno),

    // 13.6 Fast Scatter-Gather I/O
    LIB_FUNC(readv),
    LIB_FUNC(writev),

    // 13.7 Memory-mapped I/O
#if duk_LIBC_MEMORY
    LIB_FUNC(mmap),
    LIB_FUNC(munmap),
    LIB_FUNC(msync),
    LIB_FUNC(mremap),
    LIB_FUNC(madvise),
#endif

    // 13.8 Waiting for Input or Output
    LIB_FUNC(FD_NEW),
    LIB_FUNC(FD_ZERO),
    LIB_FUNC(FD_SET),
    LIB_FUNC(FD_CLR),
    LIB_FUNC(FD_ISSET),
    LIB_FUNC(select),

    // 13.9 Synchronizing I/O operations
    LIB_FUNC(sync),
    LIB_FUNC(fsync),

    // 13.11 Control Operations on Files
    LIB_FUNC(fcntl),

    // 13.12 Duplicating Descriptors
    LIB_FUNC(dup),
    LIB_FUNC(dup2),

    // 13.17 Generic I/O Control operations
    LIB_FUNC(ioctl),
    
    // 14 File System Interface
    // 14.1 Working Directory
    LIB_FUNC(getcwd),
    LIB_FUNC(chdir),
    LIB_FUNC(fchdir),

    // 14.2 Accessing Directories
    LIB_FUNC(opendir),
    LIB_FUNC(fdopendir),
    LIB_FUNC(dirfd),
    LIB_FUNC(readdir),
    LIB_FUNC(closedir),
    LIB_FUNC(rewinddir),
    LIB_FUNC(telldir),
    LIB_FUNC(seekdir),

    // 14.4 Hard Links
    LIB_FUNC(link),
    
    // 14.5 Symbolic Links
    LIB_FUNC(symlink),
    LIB_FUNC(readlink),
    LIB_FUNC(realpath),
    
    // 14.6 Deleting Files
    LIB_FUNC(unlink),
    LIB_FUNC(rmdir),
    LIB_FUNC(remove),
    
    // 14.7 Renaming Files
    LIB_FUNC(rename),
    
    // 14.8 Creating Directories
    LIB_FUNC(mkdir),
    
    // 14.9 File Attributes
    // 14.9.2 Reading the Attributes of a File
    LIB_FUNC(stat),
    LIB_FUNC(fstat),
    LIB_FUNC(lstat),

    // 14.9.3 Testing the Type of a File
    LIB_FUNC(S_ISDIR),
    LIB_FUNC(S_ISCHR),
    LIB_FUNC(S_ISBLK),
    LIB_FUNC(S_ISREG),
    LIB_FUNC(S_ISFIFO),
    LIB_FUNC(S_ISLNK),
    LIB_FUNC(S_ISSOCK),
    LIB_FUNC(S_TYPEISMQ),
    LIB_FUNC(S_TYPEISSEM),
    LIB_FUNC(S_TYPEISSHM),

    // 14.9.4 File Owner
    LIB_FUNC(chown),
    LIB_FUNC(fchown),

    // 14.9.7 Assigning File Permissions
    LIB_FUNC(umask),
    LIB_FUNC(chmod),
    LIB_FUNC(fchmod),

    // 14.9.8 Testing Permission to Access a File
    LIB_FUNC(access),

    // 14.9.9 File Times
    LIB_FUNC(utime),

    // 14.9.10 File Size
    LIB_FUNC(truncate),
    LIB_FUNC(ftruncate),

    // 14.10 Making Special Files
    LIB_FUNC(mknod),

    // 14.11 Temporary Files
    LIB_FUNC(tmpfile),
    LIB_FUNC(mkstemp),
    LIB_FUNC(mkdtemp),

    // 15 Pipes and FIFOs
    // 15.1 Creating a Pipe
    LIB_FUNC(pipe),

    // 15.2 Pipe to a Subprocess
    LIB_FUNC(popen),
    LIB_FUNC(pclose),

    // 15.3 FIFO Special Files
    LIB_FUNC(mkfifo),

    // 16 Sockets
    // 16.3.2 Setting the Address of a Socket
    LIB_FUNC(bind),

    // 16.3.3 Reading the Address of a Socket
    LIB_FUNC(getsockname),

    // 16.4 Interface Naming
    LIB_FUNC(if_nametoindex),
    LIB_FUNC(if_indextoname),
    LIB_FUNC(if_nameindex),

    // 16.6 The Internet Namespace
    LIB_FUNC(inet_aton),
    LIB_FUNC(inet_addr),
    LIB_FUNC(inet_network),
    LIB_FUNC(inet_ntoa),
    LIB_FUNC(inet_makeaddr),
    LIB_FUNC(inet_lnaof),
    LIB_FUNC(inet_netof),
    LIB_FUNC(gethostbyname),
    LIB_FUNC(gethostbyname2),
    LIB_FUNC(gethostbyaddr),
    LIB_FUNC(sethostent),
    LIB_FUNC(gethostent),
    LIB_FUNC(endhostent),
    LIB_FUNC(getservbyname),
    LIB_FUNC(getservbyport),
    LIB_FUNC(setservent),
    LIB_FUNC(getservent),
    LIB_FUNC(endservent),
    LIB_FUNC(htons),
    LIB_FUNC(ntohs),
    LIB_FUNC(htonl),
    LIB_FUNC(ntohl),
    LIB_FUNC(getprotobyname),
    LIB_FUNC(getprotobynumber),
    LIB_FUNC(setprotoent),
    LIB_FUNC(getprotoent),
    LIB_FUNC(endprotoent),

    // 16.8 Opening and Closing Sockets
    LIB_FUNC(socket),
    LIB_FUNC(shutdown),
    LIB_FUNC(socketpair),

    // 16.9 Using Sockets with Connections
    LIB_FUNC(connect),
    LIB_FUNC(listen),
    LIB_FUNC(accept),
    LIB_FUNC(getpeername),
    LIB_FUNC(send),
    LIB_FUNC(recv),

    // 16.10 Datagram Socket Operations
    LIB_FUNC(sendto),
    LIB_FUNC(recvfrom),

    // 16.12 Socket Options
    LIB_FUNC(getsockopt),
    LIB_FUNC(setsockopt),

    // 16.13 Networks Database
    LIB_FUNC(getnetbyname),
    LIB_FUNC(getnetbyaddr),
    LIB_FUNC(setnetent),
    LIB_FUNC(getnetent),
    LIB_FUNC(endnetent),

#if duk_LIBC_TTY
    // 17 Low-Level Terminal Interface
    // 17.1 Identifying Terminals
    LIB_FUNC(isatty),
    LIB_FUNC(ttyname),

    // 17.4 Terminal Modes
    LIB_FUNC(tcgetattr),
    LIB_FUNC(tcsetattr),
    LIB_FUNC(cfgetospeed),
    LIB_FUNC(cfgetispeed),
    LIB_FUNC(cfsetospeed),
    LIB_FUNC(cfsetispeed),
    LIB_FUNC(cfsetspeed),
    LIB_FUNC(cfmakeraw),

    // 17.6 Line Control Functions
    LIB_FUNC(tcsendbreak),
    LIB_FUNC(tcdrain),
    LIB_FUNC(tcflush),
    LIB_FUNC(tcflow),

    // 17.8 Pseudo-Terminals
    LIB_FUNC(getpt),
    LIB_FUNC(grantpt),
    LIB_FUNC(unlockpt),
    LIB_FUNC(ptsname),
#endif

#if duk_LIBC_LOG
    // 18 Syslog
    // 18.2 Submitting Syslog Messages
    LIB_FUNC(openlog),
    LIB_FUNC(syslog),
    LIB_FUNC(closelog),
    LIB_FUNC(setlogmask),
    LIB_FUNC(LOG_MASK),
    LIB_FUNC(LOG_UPTO),
#endif

#if duk_LIBC_MATH
    // 19 Mathematics
    // 19.2 Trigonometric Functions
    LIB_FUNC(sin),
    LIB_FUNC(cos),
    LIB_FUNC(tan),
    LIB_FUNC(asin),
    LIB_FUNC(acos),
    LIB_FUNC(atan),
    LIB_FUNC(atan2),
    LIB_FUNC(exp),
    LIB_FUNC(exp2),
    LIB_FUNC(exp10),
    LIB_FUNC(log),
    LIB_FUNC(log2),
    LIB_FUNC(log10),
    LIB_FUNC(logb),
    LIB_FUNC(ilogb),
    LIB_FUNC(pow),
    LIB_FUNC(sqrt),
    LIB_FUNC(cbrt),
    LIB_FUNC(hypot),
    LIB_FUNC(expm1),
    LIB_FUNC(log1p),
    LIB_FUNC(sinh),
    LIB_FUNC(cosh),
    LIB_FUNC(tanh),
    LIB_FUNC(asinh),
    LIB_FUNC(acosh),
    LIB_FUNC(atanh),
    LIB_FUNC(erf),
    LIB_FUNC(erfc),
    LIB_FUNC(lgamma),
    LIB_FUNC(gamma),
    LIB_FUNC(tgamma),
    LIB_FUNC(j0),
    LIB_FUNC(j1),
    LIB_FUNC(jn),
    LIB_FUNC(y0),
    LIB_FUNC(y1),
    LIB_FUNC(yn),
    LIB_FUNC(rand),
    LIB_FUNC(srand),
    LIB_FUNC(div),
    LIB_FUNC(fpclassify),
    LIB_FUNC(isfinite),
    LIB_FUNC(isnormal),
    LIB_FUNC(isnan),
    LIB_FUNC(isinf),
    LIB_FUNC(abs),
    LIB_FUNC(fabs),
    LIB_FUNC(frexp),
    LIB_FUNC(ldexp),
    LIB_FUNC(scalb),
    LIB_FUNC(ceil),
    LIB_FUNC(floor),
    LIB_FUNC(trunc),
    LIB_FUNC(rint),
    LIB_FUNC(nearbyint),
    LIB_FUNC(round),
    LIB_FUNC(fmod),
    LIB_FUNC(drem),
    LIB_FUNC(remainder),
    LIB_FUNC(copysign),
    LIB_FUNC(signbit),
    LIB_FUNC(nextafter),
    LIB_FUNC(nexttoward),
    LIB_FUNC(nan),
    LIB_FUNC(isgreater),
    LIB_FUNC(isgreaterequal),
    LIB_FUNC(isless),
    LIB_FUNC(islessequal),
    LIB_FUNC(islessgreater),
    LIB_FUNC(isunordered),
    LIB_FUNC(fmin),
    LIB_FUNC(fmax),
    LIB_FUNC(fdim),
    LIB_FUNC(fma),
#endif
    
#if duk_LIBC_TIME
    // 21 Date and Time
    LIB_FUNC(difftime),
    LIB_FUNC(clock),
    LIB_FUNC(times),
    LIB_FUNC(time),
    LIB_FUNC(gettimeofday),
    LIB_FUNC(settimeofday),
    LIB_FUNC(adjtime),
    LIB_FUNC(localtime),
    LIB_FUNC(gmtime),
    LIB_FUNC(mktime),
    LIB_FUNC(timelocal),
    LIB_FUNC(timegm),
    LIB_FUNC(ntp_gettime),
    LIB_FUNC(ntp_adjtime),
    LIB_FUNC(asctime),
    LIB_FUNC(ctime),
    LIB_FUNC(strftime),
    LIB_FUNC(setitimer),
    LIB_FUNC(getitimer),
    LIB_FUNC(alarm),
    LIB_FUNC(sleep),
    LIB_FUNC(nanosleep),
#endif

    // 22 Resource Usage And Limitation
    LIB_FUNC(getrusage),
    LIB_FUNC(vtimes),
    LIB_FUNC(getrlimit),
    LIB_FUNC(setrlimit),
    LIB_FUNC(ulimit),
    LIB_FUNC(vlimit),
    LIB_FUNC(sched_setscheduler),
    LIB_FUNC(sched_getscheduler),
    LIB_FUNC(sched_setparam),
    LIB_FUNC(sched_getparam),
    LIB_FUNC(sched_get_priority_min),
    LIB_FUNC(sched_get_priority_max),
    LIB_FUNC(sched_rr_get_interval),
    LIB_FUNC(sched_yield),
    LIB_FUNC(getpriority),
    LIB_FUNC(setpriority),
    LIB_FUNC(nice),
#ifdef _GNU_SOURCE
    LIB_FUNC(CPU_NEW),
    LIB_FUNC(CPU_ZERO),
    LIB_FUNC(CPU_SET),
    LIB_FUNC(CPU_CLR),
    LIB_FUNC(CPU_ISSET),
    LIB_FUNC(sched_getaffinity),
    LIB_FUNC(sched_setaffinity),
#endif
    LIB_FUNC(getpagesize),
    LIB_FUNC(get_phys_pages),
    LIB_FUNC(get_avphys_pages),
    LIB_FUNC(get_nprocs_conf),
    LIB_FUNC(get_nprocs),
    LIB_FUNC(getloadavg),

#if duk_LIBC_SIG
    // 24 Signal Handling
    LIB_FUNC(strsignal),
    LIB_FUNC(psignal),
    LIB_FUNC(signal),
    LIB_FUNC(sigaction),
    LIB_FUNC(raise),
    LIB_FUNC(kill),
    LIB_FUNC(killpg),
    LIB_FUNC(signewset),
    LIB_FUNC(sigemptyset),
    LIB_FUNC(sigfillset),
    LIB_FUNC(sigaddset),
    LIB_FUNC(sigdelset),
    LIB_FUNC(sigismember),
    LIB_FUNC(sigprocmask),
    LIB_FUNC(sigpending),
    LIB_FUNC(pause),
    LIB_FUNC(sigsuspend),
#endif
    // 25 The Basic Program/System Interface
    LIB_FUNC(getenv),
#if defined(_GNU_SOURCE) && 0
    LIB_FUNC(secure_getenv),
#endif
    LIB_FUNC(putenv),
    LIB_FUNC(setenv),
    LIB_FUNC(unsetenv),
    LIB_FUNC(clearenv),
    LIB_FUNC(exit),
    LIB_FUNC(atexit),
    LIB_FUNC(abort),
    LIB_FUNC(_exit),
    LIB_FUNC(_Exit),
    
    // 26 Processes
    LIB_FUNC(getpid),
    LIB_FUNC(getppid),
    LIB_FUNC(fork),
    LIB_FUNC(vfork),
    LIB_FUNC(execv),
    LIB_FUNC(execve),
    LIB_FUNC(execvp),
    LIB_FUNC(waitpid),
    LIB_FUNC(wait),
    LIB_FUNC(wait4),
    LIB_FUNC(WIFEXITED),
    LIB_FUNC(WEXITSTATUS),
    LIB_FUNC(WIFSIGNALED),
    LIB_FUNC(WTERMSIG),
    LIB_FUNC(WCOREDUMP),
    LIB_FUNC(WIFSTOPPED),
    LIB_FUNC(WSTOPSIG),

    // 27 Job Control
    LIB_FUNC(setsid),
    LIB_FUNC(getpgrp),
    LIB_FUNC(getpgid),
    LIB_FUNC(setpgid),
    LIB_FUNC(tcgetpgrp),
    LIB_FUNC(tcsetpgrp),

    // 29 Users and Groups
    LIB_FUNC(getuid),
    LIB_FUNC(getgid),
    LIB_FUNC(geteuid),
    LIB_FUNC(getegid),
    LIB_FUNC(getgroups),
    LIB_FUNC(seteuid),
    LIB_FUNC(setuid),
    LIB_FUNC(setreuid),
    LIB_FUNC(setegid),
    LIB_FUNC(setgid),
    LIB_FUNC(setregid),
    LIB_FUNC(setgroups),
    LIB_FUNC(initgroups),
    LIB_FUNC(getlogin),
    LIB_FUNC(setutent),
    LIB_FUNC(getutent),
    LIB_FUNC(endutent),
    LIB_FUNC(getutid),
    LIB_FUNC(getutline),
    LIB_FUNC(pututline),
    LIB_FUNC(utmpname),
    LIB_FUNC(updwtmp),
#if duk_LIBC_UTIL
    LIB_FUNC(login_tty),
    LIB_FUNC(login),
    LIB_FUNC(logout),
    LIB_FUNC(logwtmp),
#endif
    LIB_FUNC(getpwuid),
    LIB_FUNC(getpwnam),
    LIB_FUNC(fgetpwent),
    LIB_FUNC(setpwent),
    LIB_FUNC(getpwent),
    LIB_FUNC(endpwent),
    LIB_FUNC(putpwent),
    LIB_FUNC(getgrgid),
    LIB_FUNC(getgrnam),
    LIB_FUNC(fgetgrent),
    LIB_FUNC(setgrent),
    LIB_FUNC(getgrent),
    LIB_FUNC(endgrent),

    // 30 System Management
    LIB_FUNC(gethostname),
    LIB_FUNC(sethostname),
    LIB_FUNC(getdomainname),
    LIB_FUNC(setdomainname),
    LIB_FUNC(gethostid),
    LIB_FUNC(sethostid),
    LIB_FUNC(uname),
    LIB_FUNC(setfsent),
    LIB_FUNC(endfsent),
    LIB_FUNC(getfsent),
    LIB_FUNC(getfsspec),
    LIB_FUNC(getfsfile),
    LIB_FUNC(setmntent),
    LIB_FUNC(endmntent),
    LIB_FUNC(getmntent),
    LIB_FUNC(addmntent),
    LIB_FUNC(hasmntopt),
    LIB_FUNC(mount),
    LIB_FUNC(umount2),
    LIB_FUNC(umount),
    
    // 31 System Configuration Parameters
    LIB_FUNC(sysconf),
    LIB_FUNC(pathconf),
    LIB_FUNC(fpathconf),
    LIB_FUNC(confstr),

    LIB_FUNC_END
};

static void
libcf_register(duk_context *ctx, duk_idx_t idx)
{
    duk_put_functions(ctx, idx, libc_func);
}
