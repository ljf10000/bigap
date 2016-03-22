#ifndef __THIS_APP
#define __THIS_APP      js
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     "libc"
#endif

#include "utils.h"
#include "dukc.h"

#if duk_LIBC_SIG
char *libc_sig_name[NSIG];
void libc_sig_handler(int sig)
{
    /*
    * global handler sig
    */
    duk_push_global_object(__ctx);
    duk_get_prop_string(__ctx, -1, libc_sig_name[sig]);
    if (duk_is_function(__ctx, -1)) {
        duk_push_int(__ctx, sig);
        /*
        * global result
        */
        duk_pcall(__ctx, 1);
    }
    duk_pop_2(__ctx);
}
#endif /* duk_LIBC_SIG */

static char *atexit_name;
static void
__atexit_handler(void)
{
    duk_push_global_object(__ctx);
    duk_get_prop_string(__ctx, -1, atexit_name);
    if (duk_is_function(__ctx, -1)) {
        duk_pcall(__ctx, 0);
    }
    duk_pop_2(__ctx);
}

#if duk_LIBC_LINUX
LIB_PARAM(timerfd_create, 2);
static duk_ret_t
duke_timerfd_create(duk_context *ctx)
{
    int clock = duk_require_int(ctx, 0);
    int flags = duk_require_int(ctx, 1);

    int fd = timerfd_create(clock, flags);

    return __push_error(ctx, fd), 1;
}

LIB_PARAM(timerfd_settime, 4);
static duk_ret_t
duke_timerfd_settime(duk_context *ctx)
{
    struct itimerspec new, old;
    int fd    = duk_require_int(ctx, 0);
    int flags = duk_require_int(ctx, 1);

    __get_itimerspec(ctx, 2, &new);
    int err = timerfd_settime(fd, flags, &new, &old);
    if (0==err) { // 0 is ok
        __set_itimerspec(ctx, 3, &old);
    }
    
    return __push_error(ctx, err), 1;
}

LIB_PARAM(timerfd_gettime, 2);
static duk_ret_t
duke_timerfd_gettime(duk_context *ctx)
{
    struct itimerspec current;
    int fd = duk_require_int(ctx, 0);

    int err = timerfd_gettime(fd, &current);
    if (0==err) { // 0 is ok
        __set_itimerspec(ctx, 1, &current);
    }
    
    return __push_error(ctx, err), 1;
}

LIB_PARAM(eventfd, 2);
static duk_ret_t
duke_eventfd(duk_context *ctx)
{
    int intval= duk_require_int(ctx, 0);
    int flags = duk_require_int(ctx, 1);

    int err = eventfd(intval, flags);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(signalfd, 3);
static duk_ret_t
duke_signalfd(duk_context *ctx)
{
    int err;
    duk_size_t bsize;
    
    int fd = duk_require_int(ctx, 0);
    sigset_t *set = (sigset_t *)duk_require_buffer_data(ctx, 1, &bsize);
    if (bsize!=sizeof(sigset_t)) {
        err = __seterrno(ctx, -EINVAL2); goto error;
    }
    int flags = duk_require_int(ctx, 2);
    
    err = signalfd(fd, set, flags);
    if (err<0) {
        seterrno(ctx);
    }
    
error:
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(epoll_create, 1);
static duk_ret_t
duke_epoll_create(duk_context *ctx)
{
    int size = duk_require_int(ctx, 0);
    
    int fd = epoll_create(size);

    return __push_error(ctx, fd), 1;
}

LIB_PARAM(epoll_create1, 1);
static duk_ret_t
duke_epoll_create1(duk_context *ctx)
{
    int flags = duk_require_int(ctx, 0);
    
    int fd = epoll_create1(flags);

    return __push_error(ctx, fd), 1;
}

#define duk_epoll_event_size    (2*sizeof(uint32_t))

LIB_PARAM(epoll_wait, 3);
static duk_ret_t
duke_epoll_wait(duk_context *ctx)
{
    int err, count;
    duk_size_t bsize;
    struct epoll_event *ev;
    
    int epfd = duk_require_int(ctx, 0);
    duk_buffer_t buffer = duk_require_buffer_data(ctx, 1, &bsize);
    if (0!=bsize%duk_epoll_event_size) {
        err = __seterrno(ctx, -EINVAL2); goto error;
    } else {
        count = bsize/duk_epoll_event_size;
        ev = (struct epoll_event *)buffer;
    }
    int timeout = duk_require_int(ctx, 2);

    err = epoll_wait(epfd, ev, count, timeout);
    if (err<0) {
        seterrno(ctx);
    }

error:
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(epoll_pwait, 4);
static duk_ret_t
duke_epoll_pwait(duk_context *ctx)
{
    int err, count;
    duk_size_t bsize;
    struct epoll_event *ev;
    
    int epfd = duk_require_int(ctx, 0);
    duk_buffer_t buffer = duk_require_buffer_data(ctx, 1, &bsize);
    if (0!=bsize%duk_epoll_event_size) {
        err = __seterrno(ctx, -EINVAL2); goto error;
    } else {
        count = bsize/duk_epoll_event_size;
        ev = (struct epoll_event *)buffer;
    }
    int timeout = duk_require_int(ctx, 2);
    sigset_t *set = (sigset_t *)duk_require_buffer_data(ctx, 3, &bsize);
    if (bsize!=sizeof(sigset_t)) {
        err = __seterrno(ctx, -EINVAL4); goto error;
    }
    
    err = epoll_pwait(epfd, ev, count, timeout, set);
    if (err<0) {
        seterrno(ctx);
    }

error:
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(epoll_ctl, 4);
static duk_ret_t
duke_epoll_ctl(duk_context *ctx)
{
    struct epoll_event ev;
    
    int epfd = duk_require_int(ctx, 0);
    int op = duk_require_int(ctx, 1);
    int fd = duk_require_int(ctx, 2);
    __get_epoll_event(ctx, 3, &ev);
    
    int err = epoll_ctl(epfd, op, fd, &ev);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(inotify_init, 0);
static duk_ret_t
duke_inotify_init(duk_context *ctx)
{
    int fd = inotify_init();

    return __push_error(ctx, fd), 1;
}

LIB_PARAM(inotify_init1, 1);
static duk_ret_t
duke_inotify_init1(duk_context *ctx)
{
    int flags = duk_require_int(ctx, 0);
    
    int err = inotify_init1(flags);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(inotify_add_watch, 3);
static duk_ret_t
duke_inotify_add_watch(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    char *path = (char *)duk_require_string(ctx, 1);
    duk_uint_t mask = duk_require_uint(ctx, 2);
    
    int err = inotify_add_watch(fd, path, mask);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(inotify_rm_watch, 2);
static duk_ret_t
duke_inotify_rm_watch(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    int wd = duk_require_int(ctx, 1);
    
    int err = inotify_rm_watch(fd, wd);

    return __push_error(ctx, err), 1;
}
#endif /* duk_LIBC_LINUX */

#if duk_LIBC_ERROR
// 2.3 Error Messages
LIB_PARAM(strerror, 1);
static duk_ret_t
duke_strerror(duk_context *ctx)
{
    int errnum = duk_require_int(ctx, 0);

    errnum = errnum<0?-errnum:errnum;

    const char *errstring = strerror(errnum);

    return __push_string(ctx, errstring), 1;
}
#endif /* duk_LIBC_ERROR */

// 3.2.2 Unconstrained Allocation
// malloc free realloc calloc

#if duk_LIBC_CHAR
// 4.1 Classification of Characters
LIB_PARAM(islower, 1);
static duk_ret_t
duke_islower(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_bool(ctx, islower(c)), 1;
}

LIB_PARAM(isupper, 1);
static duk_ret_t
duke_isupper(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);
    
    return duk_push_bool(ctx, isupper(c)), 1;
}

LIB_PARAM(isalpha, 1);
static duk_ret_t
duke_isalpha(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);
    
    return duk_push_bool(ctx, isalpha(c)), 1;
}

LIB_PARAM(isdigit, 1);
static duk_ret_t
duke_isdigit(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);
    
    return duk_push_bool(ctx, isdigit(c)), 1;
}

LIB_PARAM(isalnum, 1);
static duk_ret_t
duke_isalnum(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_bool(ctx, isalnum(c)), 1;
}

LIB_PARAM(isxdigit, 1);
static duk_ret_t
duke_isxdigit(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_bool(ctx, isxdigit(c)), 1;
}

LIB_PARAM(ispunct, 1);
static duk_ret_t
duke_ispunct(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_bool(ctx, ispunct(c)), 1;
}

LIB_PARAM(isspace, 1);
static duk_ret_t
duke_isspace(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_bool(ctx, isspace(c)), 1;
}

LIB_PARAM(isblank, 1);
static duk_ret_t
duke_isblank(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_bool(ctx, isblank(c)), 1;
}

LIB_PARAM(isgraph, 1);
static duk_ret_t
duke_isgraph(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_bool(ctx, isgraph(c)), 1;
}

LIB_PARAM(isprint, 1);
static duk_ret_t
duke_isprint(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_bool(ctx, isprint(c)), 1;
}

LIB_PARAM(iscntrl, 1);
static duk_ret_t
duke_iscntrl(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_bool(ctx, iscntrl(c)), 1;
}

LIB_PARAM(isascii, 1);
static duk_ret_t
duke_isascii(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_bool(ctx, isascii(c)), 1;
}

// 4.2 Case Conversion
LIB_PARAM(tolower, 1);
static duk_ret_t
duke_tolower(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_int(ctx, tolower(c)), 1;
}

LIB_PARAM(toupper, 1);
static duk_ret_t
duke_toupper(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_int(ctx, toupper(c)), 1;
}

LIB_PARAM(toascii, 1);
static duk_ret_t
duke_toascii(duk_context *ctx)
{
    int c = duk_require_int(ctx, 0);

    return duk_push_int(ctx, toascii(c)), 1;
}
#endif /* duk_LIBC_CHAR */

// 4.3 Character class determination for wide characters

// 5.5 String/Array Comparison
LIB_PARAM(memcmp, 3);
static duk_ret_t
duke_memcmp(duk_context *ctx)
{
    duk_pointer_t a = duk_require_pointer(ctx, 0);
    duk_pointer_t b = duk_require_pointer(ctx, 1);
    int size = duk_require_int(ctx, 2);

    int cmp = os_memcmp(a, b, size);

    return duk_push_int(ctx, cmp), 1;
}

LIB_PARAM(bufcmp, DUK_VARARGS);
static duk_ret_t
duke_bufcmp(duk_context *ctx)
{
    duk_size_t sizea, sizeb, size;
    duk_buffer_t a = duk_require_buffer_data(ctx, 0, &sizea);
    duk_buffer_t b = duk_require_buffer_data(ctx, 1, &sizeb);

    if (2==duk_get_max_idx(ctx)) {
        size = duk_require_int(ctx, 2);

        if (size > sizea || size > sizeb) {
            goto cmp;
        }
    } else {
cmp:
        if (sizea<sizeb) {
            return duk_push_int(ctx, -1), 1;
        } else if (sizea>sizeb) {
            return duk_push_int(ctx, 1), 1;
        }

        size = sizea;
    }
    
    int ret = os_memcmp(a, b, size);
    
    return duk_push_int(ctx, ret), 1;
}

// 12 Input/Output on Streams
LIB_PARAM(fopen, 2);
static duk_ret_t
duke_fopen(duk_context *ctx)
{
    char *filename  = (char *)duk_require_string(ctx, 0);
    char *mode      = (char *)duk_require_string(ctx, 1);

    FILE *f = os_fopen(filename, mode);

    return __push_pointer(ctx, f), 1;
}

LIB_PARAM(fclose, 1);
static duk_ret_t
duke_fclose(duk_context *ctx)
{
    FILE *f = (FILE *)duk_require_pointer(ctx, 0);

    int err = __os_fclose(f);

	return duk_push_int(ctx, err), 1;
}

LIB_PARAM(fread, 2);
static duk_ret_t
duke_fread(duk_context *ctx)
{
    duk_size_t bsize = 0;

    FILE *f = (FILE *)duk_require_pointer(ctx, 0);
    duk_buffer_t buf = duk_require_buffer_data(ctx, 1, &bsize);

    int err = fread(buf, 1, bsize, f);
error:
	return duk_push_int(ctx, err), 1;
}

LIB_PARAM(freadEx, 2);
static duk_ret_t
duke_freadEx(duk_context *ctx)
{
    FILE *f     = (FILE *)duk_require_pointer(ctx, 0);
    int size    = duk_require_int(ctx, 1);

    duk_buffer_t buf = __push_dynamic_buffer(ctx, size);
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

LIB_PARAM(fwrite, 2);
static duk_ret_t
duke_fwrite(duk_context *ctx)
{
    duk_buffer_t buf = NULL;
    duk_size_t bsize = 0;
    
    FILE *f = (FILE *)duk_require_pointer(ctx, 0);
    int err = duk_require_buffer_or_lstring(ctx, 1, &buf, &bsize);
    if (err<0) {
        __seterrno(ctx, err); goto error;
    }
    
    err = fwrite(buf, 1, bsize, f);
error:
    return duk_push_uint(ctx, err), 1;
}

LIB_PARAM(feof, 1);
static duk_ret_t
duke_feof(duk_context *ctx)
{
    FILE *f = (FILE *)duk_require_pointer(ctx, 0);

    bool eof = os_feof(f);

    return duk_push_bool(ctx, eof), 1;
}

LIB_PARAM(ferror, 1);
static duk_ret_t
duke_ferror(duk_context *ctx)
{
    FILE *f = (FILE *)duk_require_pointer(ctx, 0);

    int err = ferror(f);

    return duk_push_bool(ctx, err), 1;
}

LIB_PARAM(ftell, 1);
static duk_ret_t
duke_ftell(duk_context *ctx)
{
    FILE *f = (FILE *)duk_require_pointer(ctx, 0);

    int len = ftell(f);

    return duk_push_int(ctx, len), 1;
}

LIB_PARAM(fseek, 3);
static duk_ret_t
duke_fseek(duk_context *ctx)
{
    FILE *f     = (FILE *)duk_require_pointer(ctx, 0);
    int offset  = duk_require_int(ctx, 1);
    int where   = duk_require_int(ctx, 2);

    int err = fseek(f, offset, where);

    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(fflush, 1);
static duk_ret_t
duke_fflush(duk_context *ctx)
{
    FILE *f = (FILE *)duk_require_pointer(ctx, 0);

    int err = fflush(f);

    return duk_push_int(ctx, err), 1;
}

// 13 Low-Level Input/Output
LIB_PARAM(open, DUK_VARARGS);
static duk_ret_t
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
            uint32_t mode = duk_require_uint(ctx, 2);

            fd = open(file, flag, mode);

        }   break;
    }

    return __push_error(ctx, fd), 1;
}

LIB_PARAM(close, 1);
static duk_ret_t
duke_close(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);

    int err = __os_close(fd);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(read, 2);
static duk_ret_t
duke_read(duk_context *ctx)
{
    duk_size_t bsize = 0;
    
    int fd = duk_require_int(ctx, 0);
    duk_buffer_t buf = duk_require_buffer_data(ctx, 1, &bsize);
    
    int err = read(fd, buf, bsize);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(readEx, 2);
static duk_ret_t
duke_readEx(duk_context *ctx)
{
    int fd      = duk_require_int(ctx, 0);
    int size    = duk_require_int(ctx, 1);

    duk_buffer_t buf = __push_dynamic_buffer(ctx, size);
    if (NULL==buf) {
        __seterrno(ctx, -ENOMEM); goto error;
    }

    int err = read(fd, buf, size);
    if (err<0) {
        duk_pop(ctx);
        seterrno(ctx); goto error;
    }

	return 1;
error:
    return duk_push_null(ctx), 1;
}

LIB_PARAM(write, 2);
static duk_ret_t
duke_write(duk_context *ctx)
{
    duk_size_t bsize = 0;
    duk_buffer_t buf = NULL;
    
    int fd      = duk_require_int(ctx, 0);
    int err     = duk_require_buffer_or_lstring(ctx, 1, &buf, &bsize);
    if (err<0) {
        __seterrno(ctx, err); goto error;
    }
    
    err = write(fd, buf, bsize);
    if (err<0) {
        seterrno(ctx); goto error;
    }

error:
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(pread, 3);
static duk_ret_t
duke_pread(duk_context *ctx)
{
    duk_size_t bsize = 0;
    int fd      = duk_require_int(ctx, 0);
    duk_buffer_t buf = duk_require_buffer_data(ctx, 1, &bsize);
    int offset  = duk_require_int(ctx, 2);

    int err = pread(fd, buf, bsize, offset);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(preadEx, 3);
static duk_ret_t
duke_preadEx(duk_context *ctx)
{
    int fd      = duk_require_int(ctx, 0);
    int size    = duk_require_int(ctx, 1);
    int offset  = duk_require_int(ctx, 2);

    duk_buffer_t buf = __push_dynamic_buffer(ctx, size);
    if (NULL==buf) {
        __seterrno(ctx, -ENOMEM); goto error;
    }

    int err = pread(fd, buf, size, offset);
    if (err<0) {
        duk_pop(ctx);
        seterrno(ctx); goto error;
    }

	return 1;
error:
    return duk_push_null(ctx), 1;
}

LIB_PARAM(pwrite, 3);
static duk_ret_t
duke_pwrite(duk_context *ctx)
{
    duk_size_t bsize = 0;
    duk_buffer_t buf;
    
    int fd      = duk_require_int(ctx, 0);
    int err     = duk_require_buffer_or_lstring(ctx, 1, &buf, &bsize);
    if (err<0) {
        __seterrno(ctx, err); goto error;
    }
    int offset  = duk_require_int(ctx, 2);
    
    err = pwrite(fd, buf, bsize, offset);
    if (err<0) {
        seterrno(ctx);
    }

error:
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(lseek, 3);
static duk_ret_t
duke_lseek(duk_context *ctx)
{
    int fd      = duk_require_int(ctx, 0);
    int offset  = duk_require_int(ctx, 1);
    int where   = duk_require_int(ctx, 2);
    
    int err = lseek(fd, offset, where);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(fdopen, 2);
static duk_ret_t
duke_fdopen(duk_context *ctx)
{
    int fd      = duk_require_int(ctx, 0);
    char *mode  = (char *)duk_require_string(ctx, 1);

    FILE *f = fdopen(fd, mode);
    
    return __push_pointer(ctx, f), 1;
}

LIB_PARAM(fileno, 1);
static duk_ret_t
duke_fileno(duk_context *ctx)
{
    FILE *f = (FILE *)duk_require_pointer(ctx, 0);

    int fd = fileno(f);

	return __push_error(ctx, fd), 1;
}

// 13.6 Fast Scatter-Gather I/O
LIB_PARAM(readv, 2);
static duk_ret_t
duke_readv(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    int count = __get_array_length(ctx, 1);
    int i;

    struct iovec iov[count];
    for (i=0; i<count; i++) {
        iov[i].iov_base = __get_array_buffer(ctx, 1, i, &iov[i].iov_len);
    }

    int err = readv(fd, iov, count);

	return __push_error(ctx, err), 1;
}

LIB_PARAM(writev, 2);
static duk_ret_t
duke_writev(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    int count = __get_array_length(ctx, 1);
    int i;

    struct iovec iov[count];
    for (i=0; i<count; i++) {
        iov[i].iov_base = __get_array_buffer(ctx, 1, i, &iov[i].iov_len);
    }

    int err = writev(fd, iov, count);

	return __push_error(ctx, err), 1;
}

// 13.7 Memory-mapped I/O
LIB_PARAM(mmap, 6);
static duk_ret_t
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
        seterrno(ctx);
        pointer = NULL;
    }

    return __push_pointer(ctx, pointer), 1;
}

LIB_PARAM(munmap, 2);
static duk_ret_t
duke_munmap(duk_context *ctx)
{
    duk_pointer_t address = duk_require_pointer(ctx, 0);
    duk_uint_t len  = duk_require_uint(ctx, 1);

    int err = munmap(address, len);

	return __push_error(ctx, err), 1;
}

LIB_PARAM(msync, 3);
static duk_ret_t
duke_msync(duk_context *ctx)
{
    duk_pointer_t address = duk_require_pointer(ctx, 0);
    duk_uint_t len  = duk_require_uint(ctx, 1);
    int flags       = duk_require_int(ctx, 2);

    int err = msync(address, len, flags);

	return __push_error(ctx, err), 1;
}

LIB_PARAM(mremap, 4);
static duk_ret_t
duke_mremap(duk_context *ctx)
{
    duk_pointer_t address = duk_require_pointer(ctx, 0);
    duk_uint_t len  = duk_require_uint(ctx, 1);
    duk_uint_t nlen = duk_require_uint(ctx, 2);
    int flags       = duk_require_int(ctx, 3);
    
    duk_pointer_t pointer = mremap(address, len, nlen, flags);
    if (MAP_FAILED==pointer) {
        seterrno(ctx);
        pointer = NULL;
    }
    
    return __push_pointer(ctx, pointer), 1;
}

LIB_PARAM(madvise, 3);
static duk_ret_t
duke_madvise(duk_context *ctx)
{
    duk_pointer_t address = duk_require_pointer(ctx, 0);
    duk_uint_t len  = duk_require_uint(ctx, 1);
    int advice      = duk_require_int(ctx, 2);

    int err = madvise(address, len, advice);

	return __push_error(ctx, err), 1;
}

// 13.8 Waiting for Input or Output
LIB_PARAM(FD_NEW, 0);
static duk_ret_t
duke_FD_NEW(duk_context *ctx)
{
    return __push_dynamic_buffer(ctx, sizeof(fd_set)), 1;
}

LIB_PARAM(FD_ZERO, 1);
static duk_ret_t
duke_FD_ZERO(duk_context *ctx)
{
    int err = 0;
    duk_size_t bsize;
    
    fd_set *set = (fd_set *)duk_require_buffer_data(ctx, 0, &bsize);
    if (bsize!=sizeof(fd_set)) {
        err = __seterrno(ctx, -EINVAL1); goto error;
    }
    
    FD_ZERO(set);
    
error:
	return duk_push_int(ctx, err), 1;
}

LIB_PARAM(FD_SET, 2);
static duk_ret_t
duke_FD_SET(duk_context *ctx)
{
    int err = 0;
    duk_size_t bsize;
    
    fd_set *set = (fd_set *)duk_require_buffer_data(ctx, 0, &bsize);
    if (bsize!=sizeof(fd_set)) {
        err = __seterrno(ctx, -EINVAL1); goto error;
    }
    int fd = duk_require_int(ctx, 1);

    FD_SET(fd, set);
    
error:
	return duk_push_int(ctx, err), 1;
}

LIB_PARAM(FD_CLR, 2);
static duk_ret_t
duke_FD_CLR(duk_context *ctx)
{
    int err = 0;
    duk_size_t bsize;
    
    fd_set *set = (fd_set *)duk_require_buffer_data(ctx, 0, &bsize);
    if (bsize!=sizeof(fd_set)) {
        err = __seterrno(ctx, -EINVAL1); goto error;
    }
    int fd = duk_require_int(ctx, 1);

    FD_CLR(fd, set);
    
error:
	return duk_push_int(ctx, err), 1;
}

LIB_PARAM(FD_ISSET, 2);
static duk_ret_t
duke_FD_ISSET(duk_context *ctx)
{
    int err = 0;
    duk_size_t bsize;
    
    fd_set *set = (fd_set *)duk_require_buffer_data(ctx, 0, &bsize);
    if (bsize!=sizeof(fd_set)) {
        err = __seterrno(ctx, -EINVAL1); return duk_push_false(ctx), 1;
    }
    int fd = duk_require_int(ctx, 1);

    bool is_set = FD_ISSET(fd, set);
    
	return duk_push_bool(ctx, is_set), 1;
}

LIB_PARAM(select, 5);
static duk_ret_t
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
            err = __seterrno(ctx, -EINVAL2); goto error;
        }
    }

    if (duk_is_buffer(ctx, 2)) {
        wset = (fd_set *)duk_require_buffer_data(ctx, 2, &bsize);
        if (bsize!=sizeof(fd_set)) {
            err = __seterrno(ctx, -EINVAL3); goto error;
        }
    }

    if (duk_is_buffer(ctx, 3)) {
        eset = (fd_set *)duk_require_buffer_data(ctx, 3, &bsize);
        if (bsize!=sizeof(fd_set)) {
            err = __seterrno(ctx, -EINVAL4); goto error;
        }
    }
    
    __get_timeval(ctx, 4, &tv);
    err = select(nfds, rset, wset, eset, &tv);
    __set_timeval(ctx, 4, &tv);

    if (err<0) {
        seterrno(ctx);
    }

error:
	return duk_push_int(ctx, err), 1;
}

// 13.9 Synchronizing I/O operations
LIB_PARAM(sync, 0);
static duk_ret_t
duke_sync(duk_context *ctx)
{
    return sync(), 0;
}

LIB_PARAM(fsync, 1);
static duk_ret_t
duke_fsync(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);

    int err = fsync(fd);
    if (err<0) {
        seterrno(ctx);
    }
    
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(fcntl, 3);
static duk_ret_t
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
                seterrno(ctx);
            }
            
        }   break;
        case F_GETLK:
        case F_SETLK:
        case F_SETLKW:
        case F_GETOWN:
        case F_SETOWN: {
            struct flock k;

            __get_flock(ctx, 2, &k);
            err = fcntl(fd, cmd, &k);
            if (err<0) {
                seterrno(ctx);
            }
            
        }   break;
        default:
            err = __seterrno(ctx, -ENOSUPPORT);
            break;
    }
    
	return duk_push_int(ctx, err), 1;
}

LIB_PARAM(dup, 1);
static duk_ret_t
duke_dup(duk_context *ctx)
{
    int old = duk_require_int(ctx, 0);

    int new = dup(old);

	return __push_error(ctx, new), 1;
}

LIB_PARAM(dup2, 2);
static duk_ret_t
duke_dup2(duk_context *ctx)
{
    int old = duk_require_int(ctx, 0);
    int new = duk_require_int(ctx, 1);

    int fd = dup2(old, new);

	return __push_error(ctx, fd), 1;
}

LIB_PARAM(ioctl, 3);
static duk_ret_t
duke_ioctl(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    int op = duk_require_int(ctx, 1);
    
	return 0;
}

// 14 File System Interface
// 14.1 Working Directory
LIB_PARAM(getcwd, 0);
static duk_ret_t
duke_getcwd(duk_context *ctx)
{
    int size = 1+OS_LINE_LEN;
    duk_buffer_t buf = __push_dynamic_buffer(ctx, size);

    while(1) {
        if (buf==getcwd(buf, size-1)) {
            break;
        }
        else if (ERANGE!=errno) {
            seterrno(ctx);
            duk_pop(ctx);
            duk_push_null(ctx);
            break;
        }

        size += size;
        buf = duk_resize_buffer(ctx, -1, size);
    }

	return 1;
}

LIB_PARAM(chdir, 1);
static duk_ret_t
duke_chdir(duk_context *ctx)
{
    char *dirname = (char *)duk_require_string(ctx, 0);

    int err = chdir(dirname);

	return __push_error(ctx, err), 1;
}

LIB_PARAM(fchdir, 1);
static duk_ret_t
duke_fchdir(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);

    int err = fchdir(fd);

	return __push_error(ctx, err), 1;
}

// 14.2 Accessing Directories
LIB_PARAM(opendir, 1);
static duk_ret_t
duke_opendir(duk_context *ctx)
{
    char *dirname = (char *)duk_require_string(ctx, 0);

    DIR *dir = opendir(dirname);
    if (NULL==dir) {
        seterrno(ctx);
    }
    
    return __push_pointer(ctx, dir), 1;
}

LIB_PARAM(fdopendir, 1);
static duk_ret_t
duke_fdopendir(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);

    DIR *dir = fdopendir(fd);
    if (NULL==dir) {
        seterrno(ctx);
    }
    
    return __push_pointer(ctx, dir), 1;
}

LIB_PARAM(dirfd, 1);
static duk_ret_t
duke_dirfd(duk_context *ctx)
{
    DIR *dir = (DIR *)duk_require_pointer(ctx, 0);

    int fd = dirfd(dir);
    
	return duk_push_int(ctx, fd), 1;
}

LIB_PARAM(readdir, 1);
static duk_ret_t
duke_readdir(duk_context *ctx)
{
    DIR *dir = (DIR *)duk_require_pointer(ctx, 0);
        
    struct dirent *d = readdir(dir);
    if (NULL==d) {
        seterrno(ctx);
    }

	return __obj_push(ctx, __set_dirent, readdir(dir)), 1;
}

LIB_PARAM(closedir, 1);
static duk_ret_t
duke_closedir(duk_context *ctx)
{
    DIR *dir = (DIR *)duk_require_pointer(ctx, 0);
    
    int err = closedir(dir);

	return __push_error(ctx, err), 1;
}

LIB_PARAM(rewinddir, 1);
static duk_ret_t
duke_rewinddir(duk_context *ctx)
{
    DIR *dir = (DIR *)duk_require_pointer(ctx, 0);

    return rewinddir(dir), 0;
}

LIB_PARAM(telldir, 1);
static duk_ret_t
duke_telldir(duk_context *ctx)
{
    DIR *dir = (DIR *)duk_require_pointer(ctx, 0);

    int err = telldir(dir);

    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(seekdir, 2);
static duk_ret_t
duke_seekdir(duk_context *ctx)
{
    DIR *dir = (DIR *)duk_require_pointer(ctx, 0);
    int pos  = duk_require_int(ctx, 1);
    
    return seekdir(dir, pos), 0;
}

// 14.4 Hard Links
LIB_PARAM(link, 2);
static duk_ret_t
duke_link(duk_context *ctx)
{
    char *old = (char *)duk_require_string(ctx, 0);
    char *new = (char *)duk_require_string(ctx, 1);

    int err = link(old, new);

	return __push_error(ctx, err), 1;
}

// 14.5 Symbolic Links
LIB_PARAM(symlink, 2);
static duk_ret_t
duke_symlink(duk_context *ctx)
{
    char *old = (char *)duk_require_string(ctx, 0);
    char *new = (char *)duk_require_string(ctx, 1);

    int err = symlink(old, new);

	return __push_error(ctx, err), 1;
}

LIB_PARAM(readlink, 1);
static duk_ret_t
duke_readlink(duk_context *ctx)
{
    char *filename = (char *)duk_require_string(ctx, 0);
    int size = OS_LINE_LEN;
    char *line = NULL;
    
    while(1) {
        line = (char *)os_realloc(line, 1+size);
        
        int len = readlink(filename, line, size);
        if (len<0) {
            seterrno(ctx); 
            duk_push_null(ctx);
            
            break;
        }
        else if (len<=size) {
            line[len] = 0;
            __push_lstring(ctx, line, len);
            
            break;
        }

        size += 1+OS_LINE_LEN;
    }

error:
    os_free(line);
    return 1;
}

LIB_PARAM(realpath, 1);
static duk_ret_t
duke_realpath(duk_context *ctx)
{
    char *filename = (char *)duk_require_string(ctx, 0);

    char *path = realpath(filename, NULL);
    if (NULL==path) {
        seterrno(ctx);
    }

    return __push_string(ctx, path), 1;
}

// 14.6 Deleting Files
LIB_PARAM(unlink, 1);
static duk_ret_t
duke_unlink(duk_context *ctx)
{    
    char *filename = (char *)duk_require_string(ctx, 0);

    int err = unlink(filename);

	return __push_error(ctx, err), 1;
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
    
    closedir(dir);

    rmdir(path);
    
    return 0;
}

LIB_PARAM(rmdir, DUK_VARARGS);
static duk_ret_t
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
            __seterrno(ctx, err);
        }
    } else {
        err = rmdir(filename);
        if (err<0) {
            seterrno(ctx);
        }
    }

error:
	return duk_push_int(ctx, err), 1;
}

LIB_PARAM(remove, 1);
static duk_ret_t
duke_remove(duk_context *ctx)
{
    char *filename = (char *)duk_require_string(ctx, 0);

    int err = remove(filename);

	return __push_error(ctx, err), 1;
}

// 14.7 Renaming Files
LIB_PARAM(rename, 2);
static duk_ret_t
duke_rename(duk_context *ctx)
{
    char *old = (char *)duk_require_string(ctx, 0);
    char *new = (char *)duk_require_string(ctx, 1);

    int err = rename(old, new);
    
	return __push_error(ctx, err), 1;

}

// 14.8 Creating Directories
LIB_PARAM(mkdir, DUK_VARARGS);
static duk_ret_t
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
LIB_PARAM(stat, 2);
static duk_ret_t
duke_stat(duk_context *ctx)
{
    struct stat st;
    
    char *filename = (char *)duk_require_string(ctx, 0);

    int err = stat(filename, &st);
    if (0==err) { // 0 is ok
        __set_stat(ctx, 1, &st);
    }

    return __push_error(ctx, err), 1;
}

LIB_PARAM(fstat, 2);
static duk_ret_t
duke_fstat(duk_context *ctx)
{
    struct stat st;
    
    int fd = duk_require_int(ctx, 0);

    int err = fstat(fd, &st);
    if (0==err) { // 0 is ok
        __set_stat(ctx, 1, &st);
    }

    return __push_error(ctx, err), 1;
}

LIB_PARAM(lstat, 2);
static duk_ret_t
duke_lstat(duk_context *ctx)
{
    struct stat st;
    
    char *filename = (char *)duk_require_string(ctx, 0);

    int err = lstat(filename, &st);
    if (0==err) { // 0 is ok
        __set_stat(ctx, 1, &st);
    }

    return __push_error(ctx, err), 1;
}

// 14.9.3 Testing the Type of a File
LIB_PARAM(S_ISDIR, 1);
static duk_ret_t
duke_S_ISDIR(duk_context *ctx)
{
    duk_uint_t mode = duk_require_uint(ctx, 0);

    return duk_push_bool(ctx, S_ISDIR(mode)), 1;
}

LIB_PARAM(S_ISCHR, 1);
static duk_ret_t
duke_S_ISCHR(duk_context *ctx)
{
    duk_uint_t mode = duk_require_uint(ctx, 0);

    return duk_push_bool(ctx, S_ISCHR(mode)), 1;
}

LIB_PARAM(S_ISBLK, 1);
static duk_ret_t
duke_S_ISBLK(duk_context *ctx)
{
    duk_uint_t mode = duk_require_uint(ctx, 0);

    return duk_push_bool(ctx, S_ISBLK(mode)), 1;
}

LIB_PARAM(S_ISREG, 1);
static duk_ret_t
duke_S_ISREG(duk_context *ctx)
{
    duk_uint_t mode = duk_require_uint(ctx, 0);

    return duk_push_bool(ctx, S_ISREG(mode)), 1;
}

LIB_PARAM(S_ISFIFO, 1);
static duk_ret_t
duke_S_ISFIFO(duk_context *ctx)
{
    duk_uint_t mode = duk_require_uint(ctx, 0);

    return duk_push_bool(ctx, S_ISFIFO(mode)), 1;
}

LIB_PARAM(S_ISLNK, 1);
static duk_ret_t
duke_S_ISLNK(duk_context *ctx)
{
    duk_uint_t mode = duk_require_uint(ctx, 0);

    return duk_push_bool(ctx, S_ISLNK(mode)), 1;
}

LIB_PARAM(S_ISSOCK, 1);
static duk_ret_t
duke_S_ISSOCK(duk_context *ctx)
{
    duk_uint_t mode = duk_require_uint(ctx, 0);

    return duk_push_bool(ctx, S_ISSOCK(mode)), 1;
}

LIB_PARAM(S_TYPEISMQ, 1);
static duk_ret_t
duke_S_TYPEISMQ(duk_context *ctx)
{
    struct stat st;

    __get_stat(ctx, 0, &st);

    return duk_push_bool(ctx, S_TYPEISMQ(&st)), 1;
}

LIB_PARAM(S_TYPEISSEM, 1);
static duk_ret_t
duke_S_TYPEISSEM(duk_context *ctx)
{
    struct stat st;

    __get_stat(ctx, 0, &st);

    return duk_push_bool(ctx, S_TYPEISSEM(&st)), 1;
}

LIB_PARAM(S_TYPEISSHM, 1);
static duk_ret_t
duke_S_TYPEISSHM(duk_context *ctx)
{
    struct stat st;

    __get_stat(ctx, 0, &st);

    return duk_push_bool(ctx, S_TYPEISSHM(&st)), 1;
}

// 14.9.4 File Owner
LIB_PARAM(chown, 3);
static duk_ret_t
duke_chown(duk_context *ctx)
{
	char *filename = (char *)duk_require_string(ctx, 0);
	uid_t owner = duk_require_uint(ctx, 1);
	gid_t group = duk_require_uint(ctx, 2);

    int err = chown(filename, owner, group);

	return __push_error(ctx, err), 1;
}

LIB_PARAM(fchown, 3);
static duk_ret_t
duke_fchown(duk_context *ctx)
{
	int fd = duk_require_int(ctx, 0);
	int owner = duk_require_int(ctx, 1);
	int group = duk_require_int(ctx, 2);

    int err = fchown(fd, owner, group);

	return __push_error(ctx, err), 1;
}

// 14.9.7 Assigning File Permissions
LIB_PARAM(umask, 1);
static duk_ret_t
duke_umask(duk_context *ctx)
{
	mode_t mask = duk_require_uint(ctx, 0);

	int err = umask(mask);
	
	return duk_push_int(ctx, err), 1;
}

LIB_PARAM(chmod, 2);
static duk_ret_t
duke_chmod(duk_context *ctx)
{
    char *filename = (char *)duk_require_string(ctx, 0);
    mode_t mode = duk_require_uint(ctx, 1);
    
	int err = chmod(filename, mode);

	return __push_error(ctx, err), 1;
}

LIB_PARAM(fchmod, 2);
static duk_ret_t
duke_fchmod(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    mode_t mode = duk_require_uint(ctx, 1);
    
	int err = fchmod(fd, mode);

	return __push_error(ctx, err), 1;
}

// 14.9.8 Testing Permission to Access a File
LIB_PARAM(access, 2);
static duk_ret_t
duke_access(duk_context *ctx)
{
    char *filename = (char *)duk_require_string(ctx, 0);
    int how = duk_require_int(ctx, 1);
    
	int err = access(filename, how);

	return __push_error(ctx, err), 1;
}

// 14.9.9 File Times
LIB_PARAM(utime, 2);
static duk_ret_t
duke_utime(duk_context *ctx)
{
    struct utimbuf b = {0};
    char *filename = (char *)duk_require_string(ctx, 0);
    __get_utimbuf(ctx, 1, &b);
    
	int err = utime(filename, &b);

	return __push_error(ctx, err), 1;
}

// 14.9.10 File Size
LIB_PARAM(truncate, 2);
static duk_ret_t
duke_truncate(duk_context *ctx)
{
    char *filename = (char *)duk_require_string(ctx, 0);
    duk_uint_t len = duk_require_uint(ctx, 1);
    
	int err = truncate(filename, len);

	return __push_error(ctx, err), 1;
}

LIB_PARAM(ftruncate, 2);
static duk_ret_t
duke_ftruncate(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    duk_uint_t len = duk_require_uint(ctx, 1);
    
	int err = ftruncate(fd, len);

	return __push_error(ctx, err), 1;
}

// 14.10 Making Special Files
LIB_PARAM(mknod, 3);
static duk_ret_t
duke_mknod(duk_context *ctx)
{
    char *filename = (char *)duk_require_string(ctx, 0);
    int mode = duk_require_int(ctx, 1);
    int dev = duk_require_int(ctx, 2);
    
	int err = mknod(filename, mode, dev);

	return __push_error(ctx, err), 1;
}

// 14.11 Temporary Files
LIB_PARAM(tmpfile, 0);
static duk_ret_t
duke_tmpfile(duk_context *ctx)
{
	FILE *f = tmpfile();

	return __push_pointer(ctx, f), 1;
}

LIB_PARAM(mkstemp, 1);
static duk_ret_t
duke_mkstemp(duk_context *ctx)
{
    char *template = (char *)duk_require_string(ctx, 0);
    
	int err = mkstemp(template);

	return __push_error(ctx, err), 1;
}

LIB_PARAM(mkdtemp, 1);
static duk_ret_t
duke_mkdtemp(duk_context *ctx)
{
    char *template = (char *)duk_require_string(ctx, 0);
    
	char *filename = mkdtemp(template);

	return __push_string(ctx, filename), 1;
}

// 15 Pipes and FIFOs
// 15.1 Creating a Pipe
LIB_PARAM(pipe, 1);
static duk_ret_t
duke_pipe(duk_context *ctx)
{
	int fd[2] = {
        [0] = __get_array_int(ctx, 0, 0),
        [1] = __get_array_int(ctx, 0, 1),
	};
	
    int err = pipe(fd);

    __set_array_int(ctx, 0, 0, fd[0]);
    __set_array_int(ctx, 0, 1, fd[1]);

	return __push_error(ctx, err), 1;
}


// 15.2 Pipe to a Subprocess
LIB_PARAM(popen, 2);
static duk_ret_t
duke_popen(duk_context *ctx)
{
    char *command = (char *)duk_require_string(ctx, 0);
    char *mode = (char *)duk_require_string(ctx, 1);
    
	FILE *f = popen(command, mode);

	return __push_pointer(ctx, f), 1;
}

LIB_PARAM(pclose, 1);
static duk_ret_t
duke_pclose(duk_context *ctx)
{
    FILE *f = (FILE *)duk_require_pointer(ctx, 0);
    
	int err = pclose(f);
	
	return duk_push_int(ctx, err), 1;
}

// 15.3 FIFO Special Files
LIB_PARAM(mkfifo, 2);
static duk_ret_t
duke_mkfifo(duk_context *ctx)
{
    char *filename = (char *)duk_require_string(ctx, 0);
    int mode = duk_require_int(ctx, 1);
    
	int err = mkfifo(filename, mode);

	return __push_error(ctx, err), 1;
}

// 16 Sockets
// 16.3 Socket Addresses
LIB_PARAM(bind, 2);
static duk_ret_t
duke_bind(duk_context *ctx)
{
    os_sockaddr_t sa = OS_SOCKADDR_ZERO();
    int fd = duk_require_int(ctx, 0);

    int err = __get_sockaddr(ctx, 1, &sa);
    if (err<0) {
        __seterrno(ctx, err);
        
        goto error;
    }
    
    err = bind(fd, &sa.c, os_sockaddr_len(&sa.c));
    if (err<0) {
        seterrno(ctx);

        goto error;
    }
    
error:
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(getsockname, 2);
static duk_ret_t
duke_getsockname(duk_context *ctx)
{
    socklen_t len = sizeof(sockaddr_t);
    os_sockaddr_t sa = OS_SOCKADDR_ZERO();
    
    int fd = duk_require_int(ctx, 0);
    
    int err = getsockname(fd, &sa.c, &len);
    if (0==err) { // 0 is ok
        __set_sockaddr(ctx, 1, &sa);
    }
    
    return __push_error(ctx, err), 1;
}

LIB_PARAM(if_nametoindex, 1);
static duk_ret_t
duke_if_nametoindex(duk_context *ctx)
{
    char *ifname = (char *)duk_require_string(ctx, 0);
    
	duk_uint_t ifindex = if_nametoindex(ifname);
	
	return duk_push_uint(ctx, ifindex), 1;
}

LIB_PARAM(if_indextoname, 2);
static duk_ret_t
duke_if_indextoname(duk_context *ctx)
{
    duk_uint_t ifindex = duk_require_uint(ctx, 0);
    char ifname[1+IFNAMSIZ] = {0};
    
	char *name = if_indextoname(ifindex, ifname);

	return __push_string(ctx, name), 1;
}

LIB_PARAM(if_nameindex, 0);
static duk_ret_t
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
            __set_obj_uint(ctx, -1, "index", p->if_index);
            __set_obj_string(ctx, -1, "name", p->if_name);
        duk_put_prop_index(ctx, -1, i);
    }
	
	return 1;
}

// 16.6 The Internet Namespace
LIB_PARAM(inet_aton, 1);
static duk_ret_t
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

LIB_PARAM(inet_addr, 1);
static duk_ret_t
duke_inet_addr(duk_context *ctx)
{
    char *name = (char *)duk_require_string(ctx, 0);

	duk_uint_t addr = inet_addr(name);
	
	return duk_push_uint(ctx, addr), 1;
}

LIB_PARAM(inet_network, 1);
static duk_ret_t
duke_inet_network(duk_context *ctx)
{
    char *name = (char *)duk_require_string(ctx, 0);

	duk_uint_t net = inet_network(name);

	return duk_push_uint(ctx, net), 1;
}

LIB_PARAM(inet_ntoa, 1);
static duk_ret_t
duke_inet_ntoa(duk_context *ctx)
{
    struct in_addr in;
    in.s_addr = duk_require_uint(ctx, 0);
    
	char *name = inet_ntoa(in);
	
	return __push_string(ctx, name), 1;
}

LIB_PARAM(inet_makeaddr, 2);
static duk_ret_t
duke_inet_makeaddr(duk_context *ctx)
{
    duk_uint_t net = duk_require_uint(ctx, 0);
    duk_uint_t local = duk_require_uint(ctx, 1);

    struct in_addr in = inet_makeaddr(net, local);
	
	return duk_push_uint(ctx, in.s_addr), 1;
}

LIB_PARAM(inet_lnaof, 1);
static duk_ret_t
duke_inet_lnaof(duk_context *ctx)
{
    struct in_addr in;
    in.s_addr = duk_require_uint(ctx, 0);

	duk_uint_t host = inet_lnaof(in);
	
	return duk_push_uint(ctx, host), 1;
}

LIB_PARAM(inet_netof, 1);
static duk_ret_t
duke_inet_netof(duk_context *ctx)
{
    struct in_addr in;
    in.s_addr = duk_require_uint(ctx, 0);

	duk_uint_t host = inet_netof(in);
	
	return duk_push_uint(ctx, host), 1;
}

LIB_PARAM(gethostbyname, 1);
static duk_ret_t
duke_gethostbyname(duk_context *ctx)
{
    char *name = (char *)duk_require_string(ctx, 0);

    struct hostent *p = gethostbyname(name);

	return __obj_push(ctx, __set_hostent, p), 1;
}

LIB_PARAM(gethostbyname2, 2);
static duk_ret_t
duke_gethostbyname2(duk_context *ctx)
{
    char *name = (char *)duk_require_string(ctx, 0);
    int af = duk_require_int(ctx, 1);
    
    struct hostent *p = gethostbyname2(name, af);

	return __obj_push(ctx, __set_hostent, p), 1;
}

LIB_PARAM(gethostbyaddr, 3);
static duk_ret_t
duke_gethostbyaddr(duk_context *ctx)
{
    char *addr = (char *)duk_require_string(ctx, 0);
    int len = duk_require_int(ctx, 1);
    int af = duk_require_int(ctx, 2);
    
    struct hostent *p = gethostbyaddr(addr, len, af);

	return __obj_push(ctx, __set_hostent, p), 1;
}

LIB_PARAM(sethostent, 1);
static duk_ret_t
duke_sethostent(duk_context *ctx)
{
    int stayopen = duk_require_int(ctx, 0);
    
	return sethostent(stayopen), 0;
}

LIB_PARAM(gethostent, 0);
static duk_ret_t
duke_gethostent(duk_context *ctx)
{
	struct hostent *p = gethostent();

	return __obj_push(ctx, __set_hostent, p), 1;
}

LIB_PARAM(endhostent, 0);
static duk_ret_t
duke_endhostent(duk_context *ctx)
{
	return endhostent(), 0;
}

LIB_PARAM(getservbyname, 2);
static duk_ret_t
duke_getservbyname(duk_context *ctx)
{
    char *name = (char *)duk_require_string(ctx, 0);
    char *proto = (char *)duk_require_string(ctx, 1);
	
	struct servent *p = getservbyname(name, proto);

	return __obj_push(ctx, __set_servent, p), 1;
}

LIB_PARAM(getservbyport, 2);
static duk_ret_t
duke_getservbyport(duk_context *ctx)
{
    int port = duk_require_int(ctx, 0);
    char *proto = (char *)duk_require_string(ctx, 1);
	
	struct servent *p = getservbyport(port, proto);

	return __obj_push(ctx, __set_servent, p), 1;
}

LIB_PARAM(setservent, 1);
static duk_ret_t
duke_setservent(duk_context *ctx)
{
    int stayopen = duk_require_int(ctx, 0);
    
	return setservent(stayopen), 0;
}

LIB_PARAM(getservent, 0);
static duk_ret_t
duke_getservent(duk_context *ctx)
{
	struct servent *p = getservent();

	return __obj_push(ctx, __set_servent, p), 1;
}

LIB_PARAM(endservent, 0);
static duk_ret_t
duke_endservent(duk_context *ctx)
{
	return endservent(), 0;
}

LIB_PARAM(htons, 1);
static duk_ret_t
duke_htons(duk_context *ctx)
{
	duk_uint_t n = duk_require_uint(ctx, 0);
    duk_uint_t v = htons(n);
	
	return duk_push_uint(ctx, v), 1;
}

LIB_PARAM(ntohs, 1);
static duk_ret_t
duke_ntohs(duk_context *ctx)
{
	duk_uint_t n = duk_require_uint(ctx, 0);
    duk_uint_t v = ntohs(n);
	
	return duk_push_uint(ctx, v), 1;
}

LIB_PARAM(htonl, 1);
static duk_ret_t
duke_htonl(duk_context *ctx)
{
	duk_uint_t n = duk_require_uint(ctx, 0);
    duk_uint_t v = htonl(n);
	
	return duk_push_uint(ctx, v), 1;
}

LIB_PARAM(ntohl, 1);
static duk_ret_t
duke_ntohl(duk_context *ctx)
{
	duk_uint_t n = duk_require_uint(ctx, 0);
    duk_uint_t v = ntohl(n);
	
	return duk_push_uint(ctx, v), 1;
}

LIB_PARAM(getprotobyname, 1);
static duk_ret_t
duke_getprotobyname(duk_context *ctx)
{
    char *name = (char *)duk_require_string(ctx, 0);
	
	struct protoent *p = getprotobyname(name);

	return __obj_push(ctx, __set_protoent, p), 1;
}

LIB_PARAM(getprotobynumber, 1);
static duk_ret_t
duke_getprotobynumber(duk_context *ctx)
{
    int proto = duk_require_int(ctx, 0);
	
	struct protoent *p = getprotobynumber(proto);

	return __obj_push(ctx, __set_protoent, p), 1;
}

LIB_PARAM(setprotoent, 1);
static duk_ret_t
duke_setprotoent(duk_context *ctx)
{
    int stayopen = duk_require_int(ctx, 0);
    
	return setprotoent(stayopen), 0;
}

LIB_PARAM(getprotoent, 0);
static duk_ret_t
duke_getprotoent(duk_context *ctx)
{
	struct protoent *p = getprotoent();

	return __obj_push(ctx, __set_protoent, p), 1;
}

LIB_PARAM(endprotoent, 0);
static duk_ret_t
duke_endprotoent(duk_context *ctx)
{
	return endprotoent(), 0;
}

// 16.8 Opening and Closing Sockets
LIB_PARAM(socket, 3);
static duk_ret_t
duke_socket(duk_context *ctx)
{
    int domain  = duk_require_int(ctx, 0);
    int type    = duk_require_int(ctx, 1);
    int protocol= duk_require_int(ctx, 2);
    
    int fd = socket(domain, type, protocol);

    return __push_error(ctx, fd), 1;
}

LIB_PARAM(shutdown, 2);
static duk_ret_t
duke_shutdown(duk_context *ctx)
{
    int fd  = duk_require_int(ctx, 0);
    int how = duk_require_int(ctx, 1);

    int err = shutdown(fd, how);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(socketpair, 4);
static duk_ret_t
duke_socketpair(duk_context *ctx)
{
    int namespace = duk_require_int(ctx, 0);
    int style = duk_require_int(ctx, 1);
    int protocol = duk_require_int(ctx, 2);
    int fd[2] = {
        [0] = __get_array_int(ctx, 3, 0),
        [1] = __get_array_int(ctx, 3, 1),
    };

    int err = socketpair(namespace, style, protocol, fd);

    return __push_error(ctx, err), 1;
}

// 16.9 Using Sockets with Connections
LIB_PARAM(connect, 2);
static duk_ret_t
duke_connect(duk_context *ctx)
{
    os_sockaddr_t sa = OS_SOCKADDR_ZERO();
    
    int fd = duk_require_int(ctx, 0);
    int err = __get_sockaddr(ctx, 1, &sa);
    if (err<0) {
        goto error;
    }
    
    err = connect(fd, &sa.c, os_sockaddr_len(&sa.c));

error:
    return __push_error(ctx, err), 1;
}

LIB_PARAM(listen, 2);
static duk_ret_t
duke_listen(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    int backlog = duk_require_int(ctx, 1);

    int err = listen(fd, backlog);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(accept, 2);
static duk_ret_t
duke_accept(duk_context *ctx)
{
    socklen_t len = sizeof(sockaddr_t);
    os_sockaddr_t sa = OS_SOCKADDR_ZERO();
    
    int fd = duk_require_int(ctx, 0);
    
    int err = accept(fd, &sa.c, &len);
    if (0==err) {
        __set_sockaddr(ctx, 1, &sa);
    }
    
    return __push_error(ctx, err), 1;
}

LIB_PARAM(getpeername, 2);
static duk_ret_t
duke_getpeername(duk_context *ctx)
{
    socklen_t len = sizeof(sockaddr_t);
    os_sockaddr_t sa = OS_SOCKADDR_ZERO();
    
    int fd = duk_require_int(ctx, 0);
    
    int err = getpeername(fd, &sa.c, &len);
    if (0==err) { // 0 is ok
        __set_sockaddr(ctx, 1, &sa);
    }

    return __push_error(ctx, err), 1;
}

LIB_PARAM(send, 3);
static duk_ret_t
duke_send(duk_context *ctx)
{
    duk_size_t bsize = 0;
    duk_buffer_t buf;
    
    int fd  = duk_require_int(ctx, 0);
    int err = duk_require_buffer_or_lstring(ctx, 1, &buf, &bsize);
    if (err<0) {
        __seterrno(ctx, err); goto error;
    }
    int flag= duk_require_int(ctx, 2);

    err = send(fd, buf, bsize, flag);
    if (err<0) {
        seterrno(ctx); goto error;
    }
    
error:
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(recv, 3);
static duk_ret_t
duke_recv(duk_context *ctx)
{
    duk_size_t bsize = 0;
    int fd      = duk_require_int(ctx, 0);
    duk_buffer_t buf = duk_require_buffer_data(ctx, 1, &bsize);
    int flag    = duk_require_int(ctx, 2);
    
    int err = recv(fd, buf, bsize, flag);

    return __push_error(ctx, err), 1;
}

// 16.10 Datagram Socket Operations
LIB_PARAM(sendto, 4);
static duk_ret_t
duke_sendto(duk_context *ctx)
{
    os_sockaddr_t sa = OS_SOCKADDR_ZERO();
    duk_size_t bsize = 0;
    duk_buffer_t buf;
    
    int fd      = duk_require_int(ctx, 0);
    int err     = duk_require_buffer_or_lstring(ctx, 1, &buf, &bsize);
    if (err<0) {
        __seterrno(ctx, err); goto error;
    }
    int flag    = duk_require_int(ctx, 2);
    err = __get_sockaddr(ctx, 3, &sa);
    if (err<0) {
        goto error;
    }
    
    err = sendto(fd, buf, bsize, flag, &sa.c, os_sockaddr_len(&sa.c));
    if (err<0) {
        seterrno(ctx); goto error;
    }
    
error:
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(recvfrom, 4);
static duk_ret_t
duke_recvfrom(duk_context *ctx)
{
    socklen_t len = sizeof(sockaddr_t);
    os_sockaddr_t sa = OS_SOCKADDR_ZERO();
    duk_size_t bsize = 0;

    int fd      = duk_require_int(ctx, 0);
    duk_buffer_t buf = duk_require_buffer_data(ctx, 1, &bsize);
    int flag    = duk_require_int(ctx, 2);
    
    int err = recvfrom(fd, buf, bsize, flag, &sa.c, &len);
    if (err<0) {
        seterrno(ctx);
    } else {
        __set_sockaddr(ctx, 3, &sa);
    }

error:
    return duk_push_int(ctx, err), 1;
}

// 16.12 Socket Options
LIB_PARAM(getsockopt, 4);
static duk_ret_t
duke_getsockopt(duk_context *ctx)
{
	int fd      = duk_require_int(ctx, 0);
	int level   = duk_require_int(ctx, 1);
	int opt     = duk_require_int(ctx, 2);
    int err     = 0;
    int val     = 0;
    socklen_t size;
    
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
                seterrno(ctx);
            } else {
                __set_obj_int(ctx, 3, "optval", val);
            }

            break;
        case SO_LINGER: {
            struct linger linger;
            size = sizeof(linger);
            
            err = getsockopt(fd, level, opt, &linger, &size);
            if (err<0) {
                seterrno(ctx);
            } else {
                __set_linger(ctx, 3, &linger);
            }
            
        }   break;
        default:
            err = -ENOSUPPORT;

            break;
	}
	
	return duk_push_int(ctx, err), 1;
}

LIB_PARAM(setsockopt, 4);
static duk_ret_t
duke_setsockopt(duk_context *ctx)
{
	int fd      = duk_require_int(ctx, 0);
	int level   = duk_require_int(ctx, 1);
	int opt     = duk_require_int(ctx, 2);
    int err     = 0;
    int val     = 0;
    socklen_t size;
    
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
                seterrno(ctx);
            }
            
            break;
        case SO_LINGER: {
            struct linger linger;
            size = sizeof(linger);

            __get_linger(ctx, 3, &linger);
            
            err = setsockopt(fd, level, opt, &linger, size);
            if (err<0) {
                seterrno(ctx);
            }
            
        }   break;
        default:
            err = -ENOSUPPORT;
            break;
	}

    return duk_push_int(ctx, err), 1;
}

// 16.13 Networks Database
LIB_PARAM(getnetbyname, 1);
static duk_ret_t
duke_getnetbyname(duk_context *ctx)
{
	char *name = (char *)duk_require_string(ctx, 0);
	
    struct netent *ne = getnetbyname(name);

	return __obj_push(ctx, __set_netent, ne), 1;
}

LIB_PARAM(getnetbyaddr, 2);
static duk_ret_t
duke_getnetbyaddr(duk_context *ctx)
{
	int net = duk_require_int(ctx, 0);
	int type= duk_require_int(ctx, 1);
	
    struct netent *ne = getnetbyaddr(net, type);

	return __obj_push(ctx, __set_netent, ne), 1;
}

LIB_PARAM(setnetent, 1);
static duk_ret_t
duke_setnetent(duk_context *ctx)
{
	int stayopen = duk_require_int(ctx, 0);

	return setnetent(stayopen), 0;
}

LIB_PARAM(getnetent, 0);
static duk_ret_t
duke_getnetent(duk_context *ctx)
{
    struct netent *ne = getnetent();

	return __obj_push(ctx, __set_netent, ne), 1;
}

LIB_PARAM(endnetent, 0);
static duk_ret_t
duke_endnetent(duk_context *ctx)
{
	return endnetent(), 0;
}

#if duk_LIBC_TTY
// 17 Low-Level Terminal Interface
// 17.1 Identifying Terminals
LIB_PARAM(isatty, 1);
static duk_ret_t
duke_isatty(duk_context *ctx)
{
	int fd = duk_require_int(ctx, 0);

	return duk_push_bool(ctx, isatty(fd)), 1;
}

LIB_PARAM(ttyname, 1);
static duk_ret_t
duke_ttyname(duk_context *ctx)
{
	int fd = duk_require_int(ctx, 0);

    char *name = ttyname(fd);
    if (NULL==name) {
        seterrno(ctx);
    }
    
	return __push_string(ctx, name), 1;
}

// 17.4 Terminal Modes
LIB_PARAM(tcgetattr, 2);
static duk_ret_t
duke_tcgetattr(duk_context *ctx)
{
    struct termios t;
    
	int fd = duk_require_int(ctx, 0);

    int err = tcgetattr(fd, &t);
    if (0==err) { // 0 is ok
        __set_termios(ctx, 1, &t);
    }
    
	return __push_error(ctx, err), 1;
}

LIB_PARAM(tcsetattr, 3);
static duk_ret_t
duke_tcsetattr(duk_context *ctx)
{
    struct termios t;
    
	int fd  = duk_require_int(ctx, 0);
    int when= duk_require_int(ctx, 1);

    __get_termios(ctx, 2, &t);
    
    int err = tcsetattr(fd, when, &t);

	return __push_error(ctx, err), 1;
}

LIB_PARAM(cfgetospeed, 1);
static duk_ret_t
duke_cfgetospeed(duk_context *ctx)
{
    struct termios t;
    
    __get_termios(ctx, 0, &t);
    
    speed_t speed = cfgetospeed(&t);
    
	return duk_push_uint(ctx, speed), 1;
}

LIB_PARAM(cfgetispeed, 1);
static duk_ret_t
duke_cfgetispeed(duk_context *ctx)
{
    struct termios t;
    
    __get_termios(ctx, 0, &t);
    
    speed_t speed = cfgetispeed(&t);
    
	return duk_push_uint(ctx, speed), 1;
}

LIB_PARAM(cfsetospeed, 2);
static duk_ret_t
duke_cfsetospeed(duk_context *ctx)
{
    struct termios t;
    
    __get_termios(ctx, 0, &t);
    speed_t speed = duk_require_uint(ctx, 1);

    int err = cfsetospeed(&t, speed);

	return __push_error(ctx, err), 1;
}

LIB_PARAM(cfsetispeed, 2);
static duk_ret_t
duke_cfsetispeed(duk_context *ctx)
{
    struct termios t;
    
    __get_termios(ctx, 0, &t);
    speed_t speed = duk_require_uint(ctx, 1);

    int err = cfsetispeed(&t, speed);

	return __push_error(ctx, err), 1;
}

LIB_PARAM(cfsetspeed, 2);
static duk_ret_t
duke_cfsetspeed(duk_context *ctx)
{
    struct termios t;
    
    __get_termios(ctx, 0, &t);
    speed_t speed = duk_require_uint(ctx, 1);

    int err = cfsetspeed(&t, speed);

	return __push_error(ctx, err), 1;
}

LIB_PARAM(cfmakeraw, 1);
static duk_ret_t
duke_cfmakeraw(duk_context *ctx)
{
    struct termios t;
    
    __get_termios(ctx, 0, &t);

    return cfmakeraw(&t), 0;
}

// 17.6 Line Control Functions
LIB_PARAM(tcsendbreak, 2);
static duk_ret_t
duke_tcsendbreak(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    int du = duk_require_int(ctx, 1);

    int err = tcsendbreak(fd, du);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(tcdrain, 1);
static duk_ret_t
duke_tcdrain(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);

    int err = tcdrain(fd);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(tcflush, 2);
static duk_ret_t
duke_tcflush(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    int q  = duk_require_int(ctx, 1);

    int err = tcflush(fd, q);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(tcflow, 2);
static duk_ret_t
duke_tcflow(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    int act= duk_require_int(ctx, 1);

    int err = tcflow(fd, act);

    return __push_error(ctx, err), 1;
}

// 17.8 Pseudo-Terminals
LIB_PARAM(getpt, 0);
static duk_ret_t
duke_getpt(duk_context *ctx)
{
    int err = getpt();

    return __push_error(ctx, err), 1;
}

LIB_PARAM(grantpt, 1);
static duk_ret_t
duke_grantpt(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);

    int err = grantpt(fd);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(unlockpt, 1);
static duk_ret_t
duke_unlockpt(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);

    int err = unlockpt(fd);
    
    return __push_error(ctx, err), 1;
}

LIB_PARAM(ptsname, 1);
static duk_ret_t
duke_ptsname(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);

    char *name = ptsname(fd);
    if (NULL==name) {
        seterrno(ctx);
    }
    
    return __push_string(ctx, name), 1;
}
#endif /* duk_LIBC_TTY */

#if duk_LIBC_LOG
// 18 Syslog
// 18.2 Submitting Syslog Messages
LIB_PARAM(openlog, 3);
static duk_ret_t
duke_openlog(duk_context *ctx)
{
    char *ident = (char *)duk_require_string(ctx, 0);
    int opt     = duk_require_int(ctx, 1);
    int facility= duk_require_int(ctx, 2);

    return openlog(ident, opt, facility), 0;
}

LIB_PARAM(syslog, 2);
static duk_ret_t
duke_syslog(duk_context *ctx)
{
    int facility_priority = duk_require_int(ctx, 0);
    char *string = (char *)duk_require_string(ctx, 1);

    return syslog(facility_priority, "%s", string), 0;
}

LIB_PARAM(closelog, 0);
static duk_ret_t
duke_closelog(duk_context *ctx)
{
    return closelog(), 0;
}

// 18.2.4 setlogmask
LIB_PARAM(setlogmask, 1);
static duk_ret_t
duke_setlogmask(duk_context *ctx)
{
    int mask = duk_require_int(ctx, 0);

    return duk_push_int(ctx, setlogmask(mask)), 1;
}

LIB_PARAM(LOG_MASK, 1);
static duk_ret_t
duke_LOG_MASK(duk_context *ctx)
{
    int mask = duk_require_int(ctx, 0);

    return duk_push_int(ctx, LOG_MASK(mask)), 1;
}

LIB_PARAM(LOG_UPTO, 1);
static duk_ret_t
duke_LOG_UPTO(duk_context *ctx)
{
    int mask = duk_require_int(ctx, 0);

    return duk_push_int(ctx, LOG_UPTO(mask)), 1;
}
#endif /* duk_LIBC_LOG */

#if duk_LIBC_MATH
// 19 Mathematics
// 19.2 Trigonometric Functions
LIB_PARAM(sin, 1);
static duk_ret_t
duke_sin(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, sin(x)), 1;
}

LIB_PARAM(cos, 1);
static duk_ret_t
duke_cos(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, cos(x)), 1;
}

LIB_PARAM(tan, 1);
static duk_ret_t
duke_tan(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, tan(x)), 1;
}

// 19.3 Inverse Trigonometric Functions
LIB_PARAM(asin, 1);
static duk_ret_t
duke_asin(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, asin(x)), 1;
}

LIB_PARAM(acos, 1);
static duk_ret_t
duke_acos(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, acos(x)), 1;
}

LIB_PARAM(atan, 1);
static duk_ret_t
duke_atan(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, atan(x)), 1;
}

LIB_PARAM(atan2, 2);
static duk_ret_t
duke_atan2(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);

    return duk_push_number(ctx, atan2(x, y)), 1;
}

// 19.4 Exponentiation and Logarithms
LIB_PARAM(exp, 1);
static duk_ret_t
duke_exp(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, exp(x)), 1;
}

LIB_PARAM(exp2, 1);
static duk_ret_t
duke_exp2(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, exp2(x)), 1;
}

LIB_PARAM(exp10, 1);
static duk_ret_t
duke_exp10(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, exp10(x)), 1;
}

LIB_PARAM(log, 1);
static duk_ret_t
duke_log(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, log(x)), 1;
}

LIB_PARAM(log2, 1);
static duk_ret_t
duke_log2(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, log2(x)), 1;
}

LIB_PARAM(log10, 1);
static duk_ret_t
duke_log10(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, log10(x)), 1;
}

LIB_PARAM(logb, 1);
static duk_ret_t
duke_logb(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, logb(x)), 1;
}

LIB_PARAM(ilogb, 1);
static duk_ret_t
duke_ilogb(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_int(ctx, ilogb(x)), 1;
}

LIB_PARAM(pow, 2);
static duk_ret_t
duke_pow(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);

    return duk_push_number(ctx, pow(x, y)), 1;
}

LIB_PARAM(sqrt, 1);
static duk_ret_t
duke_sqrt(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_int(ctx, sqrt(x)), 1;
}

LIB_PARAM(cbrt, 1);
static duk_ret_t
duke_cbrt(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, cbrt(x)), 1;
}

LIB_PARAM(hypot, 2);
static duk_ret_t
duke_hypot(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);

    return duk_push_number(ctx, hypot(x, y)), 1;
}

LIB_PARAM(expm1, 1);
static duk_ret_t
duke_expm1(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, expm1(x)), 1;
}

LIB_PARAM(log1p, 1);
static duk_ret_t
duke_log1p(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, log1p(x)), 1;
}

// 19.5 Hyperbolic Functions
LIB_PARAM(sinh, 1);
static duk_ret_t
duke_sinh(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, sinh(x)), 1;
}

LIB_PARAM(cosh, 1);
static duk_ret_t
duke_cosh(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, cosh(x)), 1;
}

LIB_PARAM(tanh, 1);
static duk_ret_t
duke_tanh(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, tanh(x)), 1;
}

LIB_PARAM(asinh, 1);
static duk_ret_t
duke_asinh(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, asinh(x)), 1;
}

LIB_PARAM(acosh, 1);
static duk_ret_t
duke_acosh(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, acosh(x)), 1;
}

LIB_PARAM(atanh, 1);
static duk_ret_t
duke_atanh(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, atanh(x)), 1;
}

// 19.6 Special Functions
LIB_PARAM(erf, 1);
static duk_ret_t
duke_erf(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, erf(x)), 1;
}

LIB_PARAM(erfc, 1);
static duk_ret_t
duke_erfc(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, erfc(x)), 1;
}

LIB_PARAM(lgamma, 1);
static duk_ret_t
duke_lgamma(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, lgamma(x)), 1;
}

LIB_PARAM(gamma, 1);
static duk_ret_t
duke_gamma(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, gamma(x)), 1;
}

LIB_PARAM(tgamma, 1);
static duk_ret_t
duke_tgamma(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, tgamma(x)), 1;
}

LIB_PARAM(j0, 1);
static duk_ret_t
duke_j0(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, j0(x)), 1;
}

LIB_PARAM(j1, 1);
static duk_ret_t
duke_j1(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, j1(x)), 1;
}

LIB_PARAM(jn, 2);
static duk_ret_t
duke_jn(duk_context *ctx)
{
    int    n = duk_require_int(ctx, 0);
    double x = duk_require_number(ctx, 1);

    return duk_push_number(ctx, jn(n, x)), 1;
}

LIB_PARAM(y0, 1);
static duk_ret_t
duke_y0(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, y0(x)), 1;
}

LIB_PARAM(y1, 1);
static duk_ret_t
duke_y1(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);

    return duk_push_number(ctx, y1(x)), 1;
}

LIB_PARAM(yn, 2);
static duk_ret_t
duke_yn(duk_context *ctx)
{
    int    n = duk_require_int(ctx, 0);
    double x = duk_require_number(ctx, 1);

    return duk_push_number(ctx, yn(n, x)), 1;
}

// 19.8 Pseudo-Random Numbers
// 19.8.1 ISO C Random Number Functions
LIB_PARAM(rand, 0);
static duk_ret_t
duke_rand(duk_context *ctx)
{
    return duk_push_int(ctx, rand()), 1;
}

LIB_PARAM(srand, 1);
static duk_ret_t
duke_srand(duk_context *ctx)
{
    duk_uint_t seed = duk_require_uint(ctx, 0);
    
    return srand(seed), 0;
}

// 20 Arithmetic Functions
// 20.2 Integer Division
LIB_PARAM(div, 2);
static duk_ret_t
duke_div(duk_context *ctx)
{
    int numerator   = duk_require_int(ctx, 0);
    int denominator = duk_require_int(ctx, 1);

    div_t d = div(numerator, denominator);
    
    return __obj_push(ctx, __set_div, &d), 1;
}

// 20.4 Floating-Point Number Classification Functions
LIB_PARAM(fpclassify, 1);
static duk_ret_t
duke_fpclassify(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_int(ctx, fpclassify(x)), 1;
}

LIB_PARAM(isfinite, 1);
static duk_ret_t
duke_isfinite(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_bool(ctx, isfinite(x)), 1;
}

LIB_PARAM(isnormal, 1);
static duk_ret_t
duke_isnormal(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_bool(ctx, isnormal(x)), 1;
}

LIB_PARAM(isnan, 1);
static duk_ret_t
duke_isnan(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_bool(ctx, isnan(x)), 1;
}

LIB_PARAM(isinf, 1);
static duk_ret_t
duke_isinf(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_bool(ctx, isinf(x)), 1;
}

// 20.8 Arithmetic Functions
LIB_PARAM(abs, 1);
static duk_ret_t
duke_abs(duk_context *ctx)
{
    int x = duk_require_int(ctx, 0);
    
    return duk_push_int(ctx, abs(x)), 1;
}

LIB_PARAM(fabs, 1);
static duk_ret_t
duke_fabs(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_number(ctx, fabs(x)), 1;
}

LIB_PARAM(frexp, 2);
static duk_ret_t
duke_frexp(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    int exponent;
    
    double n = frexp(x, &exponent);

    __set_obj_int(ctx, 1, "exponent", exponent);
    
    return duk_push_number(ctx, n), 1;
}

LIB_PARAM(ldexp, 2);
static duk_ret_t
duke_ldexp(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    int exponent = duk_require_int(ctx, 1);
    
    return duk_push_number(ctx, ldexp(x, exponent)), 1;
}

LIB_PARAM(scalb, 2);
static duk_ret_t
duke_scalb(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    int exponent = duk_require_int(ctx, 1);
    
    return duk_push_number(ctx, scalb(x, exponent)), 1;
}

LIB_PARAM(ceil, 1);
static duk_ret_t
duke_ceil(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_number(ctx, ceil(x)), 1;
}

LIB_PARAM(floor, 1);
static duk_ret_t
duke_floor(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_number(ctx, floor(x)), 1;
}

LIB_PARAM(trunc, 1);
static duk_ret_t
duke_trunc(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_number(ctx, trunc(x)), 1;
}

LIB_PARAM(rint, 1);
static duk_ret_t
duke_rint(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_number(ctx, rint(x)), 1;
}

LIB_PARAM(nearbyint, 1);
static duk_ret_t
duke_nearbyint(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_number(ctx, nearbyint(x)), 1;
}

LIB_PARAM(round, 1);
static duk_ret_t
duke_round(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_number(ctx, round(x)), 1;
}

LIB_PARAM(fmod, 2);
static duk_ret_t
duke_fmod(duk_context *ctx)
{
    double numerator    = duk_require_number(ctx, 0);
    double denominator  = duk_require_number(ctx, 1);
    
    return duk_push_number(ctx, fmod(numerator, denominator)), 1;
}

LIB_PARAM(drem, 2);
static duk_ret_t
duke_drem(duk_context *ctx)
{
    double numerator    = duk_require_number(ctx, 0);
    double denominator  = duk_require_number(ctx, 1);
    
    return duk_push_number(ctx, drem(numerator, denominator)), 1;
}

LIB_PARAM(remainder, 2);
static duk_ret_t
duke_remainder(duk_context *ctx)
{
    double numerator    = duk_require_number(ctx, 0);
    double denominator  = duk_require_number(ctx, 1);
    
    return duk_push_number(ctx, remainder(numerator, denominator)), 1;
}

LIB_PARAM(copysign, 2);
static duk_ret_t
duke_copysign(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    
    return duk_push_number(ctx, copysign(x, y)), 1;
}

LIB_PARAM(signbit, 1);
static duk_ret_t
duke_signbit(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    
    return duk_push_number(ctx, signbit(x)), 1;
}

LIB_PARAM(nextafter, 2);
static duk_ret_t
duke_nextafter(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    
    return duk_push_number(ctx, nextafter(x, y)), 1;
}

LIB_PARAM(nexttoward, 2);
static duk_ret_t
duke_nexttoward(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    
    return duk_push_number(ctx, nexttoward(x, y)), 1;
}

LIB_PARAM(nan, 1);
static duk_ret_t
duke_nan(duk_context *ctx)
{
    char *tagp = (char *)duk_require_string(ctx, 0);
    
    return duk_push_number(ctx, nan(tagp)), 1;
}

// 20.8.6 Floating-Point Comparison Functions
LIB_PARAM(isgreater, 2);
static duk_ret_t
duke_isgreater(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    
    return duk_push_bool(ctx, isgreater(x, y)), 1;
}

LIB_PARAM(isgreaterequal, 2);
static duk_ret_t
duke_isgreaterequal(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    
    return duk_push_bool(ctx, isgreaterequal(x, y)), 1;
}

LIB_PARAM(isless, 2);
static duk_ret_t
duke_isless(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    
    return duk_push_bool(ctx, isless(x, y)), 1;
}

LIB_PARAM(islessequal, 2);
static duk_ret_t
duke_islessequal(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    
    return duk_push_bool(ctx, islessequal(x, y)), 1;
}

LIB_PARAM(islessgreater, 2);
static duk_ret_t
duke_islessgreater(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    
    return duk_push_bool(ctx, islessgreater(x, y)), 1;
}

LIB_PARAM(isunordered, 2);
static duk_ret_t
duke_isunordered(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    
    return duk_push_bool(ctx, isunordered(x, y)), 1;
}

// 20.8.7 Miscellaneous FP arithmetic functions
LIB_PARAM(fmin, 2);
static duk_ret_t
duke_fmin(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    
    return duk_push_number(ctx, fmin(x, y)), 1;
}

LIB_PARAM(fmax, 2);
static duk_ret_t
duke_fmax(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    
    return duk_push_number(ctx, fmax(x, y)), 1;
}

LIB_PARAM(fdim, 2);
static duk_ret_t
duke_fdim(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    
    return duk_push_number(ctx, fdim(x, y)), 1;
}

LIB_PARAM(fma, 3);
static duk_ret_t
duke_fma(duk_context *ctx)
{
    double x = duk_require_number(ctx, 0);
    double y = duk_require_number(ctx, 1);
    double z = duk_require_number(ctx, 2);
    
    return duk_push_number(ctx, fma(x, y, z)), 1;
}
#endif /* duk_LIBC_MATH */

#if duk_LIBC_TIME
// 21 Date and Time
// 21.2 Elapsed Time
LIB_PARAM(difftime, 2);
static duk_ret_t
duke_difftime(duk_context *ctx)
{
    time_t time1 = duk_require_uint(ctx, 0);
    time_t time0 = duk_require_uint(ctx, 1);
    
    return duk_push_number(ctx, difftime(time1, time0)), 1;
}

// 21.3.1 CPU Time Inquiry
LIB_PARAM(clock, 0);
static duk_ret_t
duke_clock(duk_context *ctx)
{
    return duk_push_uint(ctx, clock()), 1;
}

LIB_PARAM(times, 1);
static duk_ret_t
duke_times(duk_context *ctx)
{
    struct tms t;

    __get_tms(ctx, 0, &t);

    return duk_push_uint(ctx, times(&t)), 1;
}

// 21.4 Calendar Time
LIB_PARAM(time, 0);
static duk_ret_t
duke_time(duk_context *ctx)
{
    return duk_push_uint(ctx, time(NULL)), 1;
}

LIB_PARAM(gettimeofday, 2);
static duk_ret_t
duke_gettimeofday(duk_context *ctx)
{
    struct timeval tv;
    struct timezone tz;

    int err = gettimeofday(&tv, &tz);

    __set_timeval(ctx, 0, &tv);
    __set_timezone(ctx, 1, &tz);
    
    return __push_error(ctx, err), 1;
}

LIB_PARAM(settimeofday, 2);
static duk_ret_t
duke_settimeofday(duk_context *ctx)
{
    struct timeval tv;
    struct timezone tz;

    __get_timeval(ctx, 0, &tv);
    __get_timezone(ctx, 1, &tz);

    int err = settimeofday(&tv, &tz);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(adjtime, 2);
static duk_ret_t
duke_adjtime(duk_context *ctx)
{
    struct timeval delta, old;

    __get_timeval(ctx, 0, &delta);

    int err = adjtime(&delta, &old);

    __set_timeval(ctx, 1, &old);
    
    return __push_error(ctx, err), 1;
}

// 21.4.3 Broken-down Time
LIB_PARAM(localtime, 1);
static duk_ret_t
duke_localtime(duk_context *ctx)
{
    time_t time = duk_require_uint(ctx, 0);

    struct tm *tm = localtime(&time);

    return __obj_push(ctx, __set_tm, tm), 1;
}

LIB_PARAM(gmtime, 1);
static duk_ret_t
duke_gmtime(duk_context *ctx)
{
    time_t time = duk_require_uint(ctx, 0);

    struct tm *tm = gmtime(&time);

    return __obj_push(ctx, __set_tm, tm), 1;
}

LIB_PARAM(mktime, 1);
static duk_ret_t
duke_mktime(duk_context *ctx)
{
    struct tm tm;

    __get_tm(ctx, 0, &tm);
    
    time_t time = mktime(&tm);
    
    return duk_push_uint(ctx, time), 1;
}

LIB_PARAM(timelocal, 1);
static duk_ret_t
duke_timelocal(duk_context *ctx)
{
    struct tm tm;

    __get_tm(ctx, 0, &tm);
    
    time_t time = timelocal(&tm);
    
    return duk_push_uint(ctx, time), 1;
}

LIB_PARAM(timegm, 1);
static duk_ret_t
duke_timegm(duk_context *ctx)
{
    struct tm tm;

    __get_tm(ctx, 0, &tm);
    
    time_t time = timegm(&tm);
    
    return duk_push_uint(ctx, time), 1;
}

// 21.4.4 High Accuracy Clock
LIB_PARAM(ntp_gettime, 1);
static duk_ret_t
duke_ntp_gettime(duk_context *ctx)
{
    struct ntptimeval ntv;

    int err = ntp_gettime(&ntv);

    __set_ntptimeval(ctx, 0, &ntv);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(ntp_adjtime, 1);
static duk_ret_t
duke_ntp_adjtime(duk_context *ctx)
{
    struct timex tx;

    int err = ntp_adjtime(&tx);

    __set_timex(ctx, 0, &tx);

    return __push_error(ctx, err), 1;
}

// 21.4.5 Formatting Calendar Time
LIB_PARAM(asctime, 1);
static duk_ret_t
duke_asctime(duk_context *ctx)
{
    struct tm tm;

    __get_tm(ctx, 0, &tm);
    
    char *time = asctime(&tm);

    return __push_string(ctx, time), 1;
}

LIB_PARAM(ctime, 1);
static duk_ret_t
duke_ctime(duk_context *ctx)
{
    time_t t = duk_require_uint(ctx, 0);

    char *time = ctime(&t);

    return __push_string(ctx, time), 1;
}

LIB_PARAM(strftime, 2);
static duk_ret_t
duke_strftime(duk_context *ctx)
{
    struct tm tm;
    int size = OS_LINE_LEN;
    char *line = NULL;
    
    char *template = (char *)duk_require_string(ctx, 0);
    __get_tm(ctx, 1, &tm);


    while(1) {
        line = (char *)os_realloc(line, 1+size);
        
        size_t len = strftime(line, OS_LINE_LEN, template, &tm);
        if (len) {
            line[len] = 0;
            __push_string(ctx, line);

            break;
        }
        
        size += 1+OS_LINE_LEN;
    }

    os_free(line);
    return 1;
}

// 21.5 Setting an Alarm
LIB_PARAM(setitimer, 3);
static duk_ret_t
duke_setitimer(duk_context *ctx)
{
    struct itimerval new, old;
    int which = duk_require_int(ctx, 0);

    __get_itimerval(ctx, 1, &new);
    int err = setitimer(which, &new, &old);
    if (0==err) { // 0 is ok
        __set_itimerval(ctx, 2, &old);
    }
    
    return __push_error(ctx, err), 1;
}

LIB_PARAM(getitimer, 2);
static duk_ret_t
duke_getitimer(duk_context *ctx)
{
    struct itimerval old;
    int which = duk_require_int(ctx, 0);

    int err = getitimer(which, &old);
    if (0==err) { // 0 is ok
        __set_itimerval(ctx, 1, &old);
    }
    
    return __push_error(ctx, err), 1;
}

LIB_PARAM(alarm, 1);
static duk_ret_t
duke_alarm(duk_context *ctx)
{
    duk_uint_t sec = duk_require_uint(ctx, 0);

    return duk_push_uint(ctx, alarm(sec)), 1;
}

// 21.6 Sleeping
LIB_PARAM(sleep, 1);
static duk_ret_t
duke_sleep(duk_context *ctx)
{
    duk_uint_t sec = duk_require_uint(ctx, 0);

    return duk_push_uint(ctx, sleep(sec)), 1;
}

LIB_PARAM(nanosleep, 2);
static duk_ret_t
duke_nanosleep(duk_context *ctx)
{
    struct timespec req, remain;
    duk_uint_t sec = duk_require_uint(ctx, 0);

    __get_timespec(ctx, 1, &req);
    int err = nanosleep(&req, &remain);
    if (0==err) { // 0 is ok
        __set_timespec(ctx, 2, &remain);
    }
    
    return __push_error(ctx, err), 1;
}
#endif /* duk_LIBC_TIME */

// 22 Resource Usage And Limitation
LIB_PARAM(getrusage, 2);
static duk_ret_t
duke_getrusage(duk_context *ctx)
{
    struct rusage u;
    int processes = duk_require_int(ctx, 0);

    int err = getrusage(processes, &u);
    if (0==err) { // 0 is ok
        __set_rusage(ctx, 1, &u);
    }
    
    return __push_error(ctx, err), 1;
}

LIB_PARAM(vtimes, 2);
static duk_ret_t
duke_vtimes(duk_context *ctx)
{
    struct vtimes current, child;

    int err = vtimes(&current, &child);
    if (0==err) { // ???
        __set_vtimes(ctx, 0, &current);
        __set_vtimes(ctx, 1, &child);
    }
    
    return __push_error(ctx, err), 1;
}

// 22.2 Limiting Resource Usage
LIB_PARAM(getrlimit, 2);
static duk_ret_t
duke_getrlimit(duk_context *ctx)
{
    struct rlimit r;
    int resource = duk_require_int(ctx, 0);

    int err = getrlimit(resource, &r);
    if (0==err) { // 0 is ok
        __set_rlimit(ctx, 1, &r);
    }
    
    return __push_error(ctx, err), 1;
}

LIB_PARAM(setrlimit, 2);
static duk_ret_t
duke_setrlimit(duk_context *ctx)
{
    struct rlimit r;
    int resource = duk_require_int(ctx, 0);

    __get_rlimit(ctx, 1, &r);
    int err = setrlimit(resource, &r);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(ulimit, DUK_VARARGS);
static duk_ret_t
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

    return __push_error(ctx, err), 1;
}

LIB_PARAM(vlimit, 2);
static duk_ret_t
duke_vlimit(duk_context *ctx)
{
    int resource = duk_require_int(ctx, 0);
    int limit = duk_require_int(ctx, 1);

    int err = vlimit(resource, limit);
    
    return __push_error(ctx, err), 1;
}

// 22.3 Process CPU Priority And Scheduling
LIB_PARAM(sched_setscheduler, 3);
static duk_ret_t
duke_sched_setscheduler(duk_context *ctx)
{
    struct sched_param param;
    
    pid_t pid = duk_require_int(ctx, 0);
    int policy = duk_require_int(ctx, 1);
    __get_sched_param(ctx, 2, &param);
    
    int err = sched_setscheduler(pid, policy, &param);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(sched_getscheduler, 1);
static duk_ret_t
duke_sched_getscheduler(duk_context *ctx)
{
    pid_t pid = duk_require_int(ctx, 0);
    
    int err = sched_getscheduler(pid);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(sched_setparam, 2);
static duk_ret_t
duke_sched_setparam(duk_context *ctx)
{
    struct sched_param param;
    
    pid_t pid = duk_require_int(ctx, 0);
    __get_sched_param(ctx, 1, &param);
    
    int err = sched_setparam(pid, &param);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(sched_getparam, 2);
static duk_ret_t
duke_sched_getparam(duk_context *ctx)
{
    struct sched_param param;
    
    pid_t pid = duk_require_int(ctx, 0);
    
    int err = sched_getparam(pid, &param);
    if (0==err) { // 0 is ok
        __set_sched_param(ctx, 1, &param);
    }
    
    return __push_error(ctx, err), 1;
}

LIB_PARAM(sched_get_priority_min, 1);
static duk_ret_t
duke_sched_get_priority_min(duk_context *ctx)
{
    int policy = duk_require_int(ctx, 0);

    int err = sched_get_priority_min(policy);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(sched_get_priority_max, 1);
static duk_ret_t
duke_sched_get_priority_max(duk_context *ctx)
{
    int policy = duk_require_int(ctx, 0);

    int err = sched_get_priority_max(policy);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(sched_rr_get_interval, 2);
static duk_ret_t
duke_sched_rr_get_interval(duk_context *ctx)
{
    struct timespec interval;
    pid_t pid = duk_require_int(ctx, 0);

    int err = sched_rr_get_interval(pid, &interval);
    if (0==err) { // 0 is ok
        __set_timespec(ctx, 1, &interval);
    }
    
    return __push_error(ctx, err), 1;
}

LIB_PARAM(sched_yield, 0);
static duk_ret_t
duke_sched_yield(duk_context *ctx)
{
    int err = sched_yield();

    return __push_error(ctx, err), 1;
}

LIB_PARAM(getpriority, 2);
static duk_ret_t
duke_getpriority(duk_context *ctx)
{
    int class = duk_require_uint(ctx, 0);
    int id = duk_require_uint(ctx, 1);
    
    int err = getpriority(class, id);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(setpriority, 3);
static duk_ret_t
duke_setpriority(duk_context *ctx)
{
    int class = duk_require_uint(ctx, 0);
    int id = duk_require_uint(ctx, 1);
    int niceval = duk_require_uint(ctx, 2);
    
    int err = setpriority(class, id, niceval);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(nice, 1);
static duk_ret_t
duke_nice(duk_context *ctx)
{
    int increment = duk_require_uint(ctx, 0);

    int err = nice(increment);

    return __push_error(ctx, err), 1;
}

#ifdef _GNU_SOURCE
LIB_PARAM(CPU_NEW, 0);
static duk_ret_t
duke_CPU_NEW(duk_context *ctx)
{
    return __push_dynamic_buffer(ctx, sizeof(cpu_set_t)), 1;
}

LIB_PARAM(CPU_ZERO, 1);
static duk_ret_t
duke_CPU_ZERO(duk_context *ctx)
{
    int err = 0;
    duk_size_t bsize;
    
    cpu_set_t *set = (cpu_set_t *)duk_require_buffer_data(ctx, 0, &bsize);
    if (bsize!=sizeof(cpu_set_t)) {
        err = __seterrno(ctx, -EINVAL1); goto error;
    }
    CPU_ZERO(set);
    
error:
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(CPU_SET, 2);
static duk_ret_t
duke_CPU_SET(duk_context *ctx)
{
    int err = 0;
    duk_size_t bsize;
    
    int cpu = duk_require_int(ctx, 0);
    cpu_set_t *set = (cpu_set_t *)duk_require_buffer_data(ctx, 1, &bsize);
    if (bsize!=sizeof(cpu_set_t)) {
        err = __seterrno(ctx, -EINVAL2); goto error;
    }
    CPU_SET(cpu, set);
    
error:
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(CPU_CLR, 2);
static duk_ret_t
duke_CPU_CLR(duk_context *ctx)
{
    int err = 0;
    duk_size_t bsize;
    
    int cpu = duk_require_int(ctx, 0);
    cpu_set_t *set = (cpu_set_t *)duk_require_buffer_data(ctx, 1, &bsize);
    if (bsize!=sizeof(cpu_set_t)) {
        err = __seterrno(ctx, -EINVAL2); goto error;
    }
    CPU_CLR(cpu, set);
    
error:
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(CPU_ISSET, 2);
static duk_ret_t
duke_CPU_ISSET(duk_context *ctx)
{
    int err;
    duk_size_t bsize;
    
    int cpu = duk_require_int(ctx, 0);
    cpu_set_t *set = (cpu_set_t *)duk_require_buffer_data(ctx, 1, &bsize);
    if (bsize!=sizeof(cpu_set_t)) {
        err = __seterrno(ctx, -EINVAL2); return duk_push_false(ctx), 1;
    }
    
    return duk_push_bool(ctx, CPU_ISSET(cpu, set)), 1;
}

LIB_PARAM(sched_getaffinity, 3);
static duk_ret_t
duke_sched_getaffinity(duk_context *ctx)
{
    int err;
    duk_size_t bsize;
    
    pid_t pid = duk_require_int(ctx, 0);
    size_t size = duk_require_uint(ctx, 1);
    cpu_set_t *set = (cpu_set_t *)duk_require_buffer_data(ctx, 1, &bsize);
    if (bsize!=sizeof(cpu_set_t)) {
        err = __seterrno(ctx, -EINVAL3); return duk_push_false(ctx), 1;
    }

    err = sched_getaffinity(pid, size, set);
    if (err<0) {
        seterrno(ctx);
    }

error:
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(sched_setaffinity, 3);
static duk_ret_t
duke_sched_setaffinity(duk_context *ctx)
{
    int err;
    duk_size_t bsize;
    
    pid_t pid = duk_require_int(ctx, 0);
    size_t size = duk_require_uint(ctx, 1);
    cpu_set_t *set = (cpu_set_t *)duk_require_buffer_data(ctx, 1, &bsize);
    if (bsize!=sizeof(cpu_set_t)) {
        err = __seterrno(ctx, -EINVAL3); return duk_push_false(ctx), 1;
    }

    err = sched_setaffinity(pid, size, set);
    if (err<0) {
        seterrno(ctx);
    }

error:
    return duk_push_int(ctx, err), 1;
}
#endif /* _GNU_SOURCE */

// 22.4 Querying memory available resources
LIB_PARAM(getpagesize, 0);
static duk_ret_t
duke_getpagesize(duk_context *ctx)
{
    return duk_push_int(ctx, getpagesize()), 1;
}

LIB_PARAM(get_phys_pages, 0);
static duk_ret_t
duke_get_phys_pages(duk_context *ctx)
{
    return duk_push_int(ctx, get_phys_pages()), 1;
}

LIB_PARAM(get_avphys_pages, 0);
static duk_ret_t
duke_get_avphys_pages(duk_context *ctx)
{
    return duk_push_int(ctx, get_avphys_pages()), 1;
}

// 22.5 Learn about the processors available
LIB_PARAM(get_nprocs_conf, 0);
static duk_ret_t
duke_get_nprocs_conf(duk_context *ctx)
{
    return duk_push_int(ctx, get_nprocs_conf()), 1;
}

LIB_PARAM(get_nprocs, 0);
static duk_ret_t
duke_get_nprocs(duk_context *ctx)
{
    return duk_push_int(ctx, get_nprocs()), 1;
}

LIB_PARAM(getloadavg, 1);
static duk_ret_t
duke_getloadavg(duk_context *ctx)
{
    double loadavg[3];

    int err = getloadavg(loadavg, os_count_of(loadavg));
    if (err>0) {
        int i;
        
        for (i=0; i<os_count_of(loadavg); i++) {
            __set_array_number(ctx, 0, i, loadavg[i]);
        }
    }
    
    return __push_error(ctx, err), 1;
}

// 23 Non-Local Exits
// jmp_buf/setjmp/longjmp
// sigjmp_buf/sigsetjmp/siglongjmp
// ucontext_t/getcontext/makecontext/setcontext/swapcontext

#if duk_LIBC_SIG
// 24 Signal Handling
// 24.2.8 Signal Messages
LIB_PARAM(strsignal, 1);
static duk_ret_t
duke_strsignal(duk_context *ctx)
{
    int sig = duk_require_int(ctx, 0);

    char *string = strsignal(sig);

    return __push_string(ctx, string), 1;
}

LIB_PARAM(psignal, 2);
static duk_ret_t
duke_psignal(duk_context *ctx)
{
    int sig = duk_require_int(ctx, 0);
    char *string = (char *)duk_require_string(ctx, 1);

    return psignal(sig, string), 0;
}

// 24.3 Specifying Signal Actions
LIB_PARAM(signal, 2);
static duk_ret_t
duke_signal(duk_context *ctx)
{
    __sighandler_t action, old;
    int sig = duk_require_int(ctx, 0);

    if (duk_is_function(ctx, 1)) {
        action = libc_sig_handler;

        // save Function name
        libc_sig_name[sig] = __get_obj_string(ctx, 1, "name", NULL);
    }
    else if (duk_is_number(ctx, 1)) {
        action = (__sighandler_t)(uintptr_t)duk_require_int(ctx, 1);
        if (SIG_DFL!=action && SIG_IGN!=action) {
            goto error;
        }

        libc_sig_name[sig] = (char *)action;
    }
    else {
        goto error;
    }

    old = signal(sig, action);
    if (SIG_ERR==old) {
        seterrno(ctx); goto error;
    }

    if (SIG_DFL==old || SIG_IGN==old) {
        duk_push_int(ctx, (uintptr_t)old);
    } else {
        duk_push_global_object(ctx);
        duk_get_prop_string(ctx, -1, libc_sig_name[sig]);
        duk_swap(ctx, -1, -2);
        duk_pop(ctx);
    }
    
    return 1;
error:
    return duk_push_null(ctx), 1;
}

// 24.3.2 Advanced Signal Handling
LIB_PARAM(sigaction, 2);
static duk_ret_t
duke_sigaction(duk_context *ctx)
{
    struct sigaction action, old;
    int sig = duk_require_int(ctx, 0);

    __get_sigaction(ctx, 1, sig, &action);
    int err = sigaction(sig, &action, &old);
    if (0==err) { // 0 is ok
        __set_sigaction(ctx, 2, sig, &old);
    }
    
    return __push_error(ctx, err), 1;
}

// 24.6 Generating Signals
LIB_PARAM(raise, 1);
static duk_ret_t
duke_raise(duk_context *ctx)
{
    int sig = duk_require_int(ctx, 0);

    int err = raise(sig);

    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(kill, 2);
static duk_ret_t
duke_kill(duk_context *ctx)
{
    pid_t pid = duk_require_int(ctx, 0);
    int sig = duk_require_int(ctx, 1);

    int err = kill(pid, sig);
    
    return __push_error(ctx, err), 1;
}

LIB_PARAM(killpg, 2);
static duk_ret_t
duke_killpg(duk_context *ctx)
{
    pid_t pid = duk_require_int(ctx, 0);
    int sig = duk_require_int(ctx, 1);

    int err = killpg(pid, sig);

    return __push_error(ctx, err), 1;
}

// 24.7 Blocking Signals
LIB_PARAM(signewset, 1);
static duk_ret_t
duke_signewset(duk_context *ctx)
{
    return __push_dynamic_buffer(ctx, sizeof(sigset_t)), 1;
}

LIB_PARAM(sigemptyset, 1);
static duk_ret_t
duke_sigemptyset(duk_context *ctx)
{
    int err;
    duk_size_t bsize;
    
    sigset_t *set = (sigset_t *)duk_require_buffer_data(ctx, 0, &bsize);
    if (bsize!=sizeof(sigset_t)) {
        err = __seterrno(ctx, -EINVAL1); goto error;
    }
    
    err = sigemptyset(set);

error:
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(sigfillset, 1);
static duk_ret_t
duke_sigfillset(duk_context *ctx)
{
    int err;
    duk_size_t bsize;
    
    sigset_t *set = (sigset_t *)duk_require_buffer_data(ctx, 0, &bsize);
    if (bsize!=sizeof(sigset_t)) {
        err = __seterrno(ctx, -EINVAL1); goto error;
    }

    err = sigfillset(set);

error:
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(sigaddset, 2);
static duk_ret_t
duke_sigaddset(duk_context *ctx)
{
    int err;
    duk_size_t bsize;
    
    sigset_t *set = (sigset_t *)duk_require_buffer_data(ctx, 0, &bsize);
    if (bsize!=sizeof(sigset_t)) {
        err = __seterrno(ctx, -EINVAL1); goto error;
    }
    int sig = duk_require_int(ctx, 1);
    
    err = sigaddset(set, sig);

error:
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(sigdelset, 2);
static duk_ret_t
duke_sigdelset(duk_context *ctx)
{
    int err;
    duk_size_t bsize;
    
    sigset_t *set = (sigset_t *)duk_require_buffer_data(ctx, 0, &bsize);
    if (bsize!=sizeof(sigset_t)) {
        err = __seterrno(ctx, -EINVAL1); goto error;
    }
    int sig = duk_require_int(ctx, 1);
    
    err = sigdelset(set, sig);

error:
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(sigismember, 2);
static duk_ret_t
duke_sigismember(duk_context *ctx)
{
    int err;
    duk_size_t bsize;
    
    sigset_t *set = (sigset_t *)duk_require_buffer_data(ctx, 0, &bsize);
    if (bsize!=sizeof(sigset_t)) {
        err = __seterrno(ctx, -EINVAL1); duk_push_false(ctx), 1;
    }
    int sig = duk_require_int(ctx, 1);
    
    return duk_push_bool(ctx, sigismember(set, sig)), 1;
}

LIB_PARAM(sigprocmask, DUK_VARARGS);
static duk_ret_t
duke_sigprocmask(duk_context *ctx)
{
    int err;
    duk_size_t bsize;
    sigset_t *new = NULL, *old = NULL;
    
    int how = duk_require_int(ctx, 0);
    new = (sigset_t *)duk_require_buffer_data(ctx, 1, &bsize);
    if (bsize!=sizeof(sigset_t)) {
        err = __seterrno(ctx, -EINVAL2); goto error;
    }

    if (2==duk_get_argc(ctx) && duk_is_buffer(ctx, 2)) {
        old = (sigset_t *)duk_require_buffer_data(ctx, 2, &bsize);
        if (bsize!=sizeof(sigset_t)) {
            err = __seterrno(ctx, -EINVAL3); goto error;
        }
    }
    
    err = sigprocmask(how, new, old);
    if (err<0) {
        seterrno(ctx);
    }

error:
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(sigpending, 1);
static duk_ret_t
duke_sigpending(duk_context *ctx)
{
    int err;
    duk_size_t bsize;
    
    sigset_t *set = (sigset_t *)duk_require_buffer_data(ctx, 0, &bsize);
    if (bsize!=sizeof(sigset_t)) {
        err = __seterrno(ctx, -EINVAL1); goto error;
    }
    
    err = sigpending(set);

error:
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(pause, 0);
static duk_ret_t
duke_pause(duk_context *ctx)
{
    int err = pause();

    return __push_error(ctx, err), 1;
}

LIB_PARAM(sigsuspend, 1);
static duk_ret_t
duke_sigsuspend(duk_context *ctx)
{
    int err;
    duk_size_t bsize;
    
    sigset_t *set = (sigset_t *)duk_require_buffer_data(ctx, 0, &bsize);
    if (bsize!=sizeof(sigset_t)) {
        err = __seterrno(ctx, -EINVAL1); goto error;
    }
    
    err = sigsuspend(set);

error:
    return duk_push_int(ctx, err), 1;
}
#endif /* duk_LIBC_SIG */

// 25 The Basic Program/System Interface
// 25.4 Environment Variables
LIB_PARAM(getenv, 1);
static duk_ret_t
duke_getenv(duk_context *ctx)
{
    char *k = (char *)duk_require_string(ctx, 0);

    char *env = getenv(k);

    return __push_string(ctx, env), 1;
}

#if defined(_GNU_SOURCE) && 0
LIB_PARAM(secure_getenv, 1);
static duk_ret_t
duke_secure_getenv(duk_context *ctx)
{
    char *k = (char *)duk_require_string(ctx, 0);

    char *env = secure_getenv(k);

    return __push_string(ctx, env), 1;
}
#endif

LIB_PARAM(putenv, 1);
static duk_ret_t
duke_putenv(duk_context *ctx)
{
    char *kv = (char *)duk_require_string(ctx, 0);

    int err = putenv(kv);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(setenv, 3);
static duk_ret_t
duke_setenv(duk_context *ctx)
{
    char *k = (char *)duk_require_string(ctx, 0);
    char *v = (char *)duk_require_string(ctx, 1);
    bool replace = duk_require_bool(ctx, 2);

    int err = setenv(k, v, replace);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(unsetenv, 1);
static duk_ret_t
duke_unsetenv(duk_context *ctx)
{
    char *k = (char *)duk_require_string(ctx, 0);

    int err = unsetenv(k);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(clearenv, 0);
static duk_ret_t
duke_clearenv(duk_context *ctx)
{
    int err = clearenv();

    return __push_error(ctx, err), 1;
}

// 25.6 System Calls

// 25.7 Program Termination
LIB_PARAM(exit, 1);
static duk_ret_t
duke_exit(duk_context *ctx)
{
    int status = duk_require_int(ctx, 0);
    
    return exit(status), 0;
}

LIB_PARAM(atexit, 1);
static duk_ret_t
duke_atexit(duk_context *ctx)
{
    if (!duk_is_function(ctx, 0)) {
        return duk_push_null(ctx), 1;
    }

    atexit_name = __get_obj_string(ctx, 0, "name", NULL);
    
    int err = atexit(__atexit_handler);

    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(abort, 0);
static duk_ret_t
duke_abort(duk_context *ctx)
{
    return abort(), 0;
}

LIB_PARAM(_exit, 1);
static duk_ret_t
duke__exit(duk_context *ctx)
{
    int status = duk_require_int(ctx, 0);
    
    return _exit(status), 0;
}

LIB_PARAM(_Exit, 1);
static duk_ret_t
duke__Exit(duk_context *ctx)
{
    int status = duk_require_int(ctx, 0);
    
    return _Exit(status), 0;
}

// 26 Processes
// 26.3 Process Identification
LIB_PARAM(getpid, 0);
static duk_ret_t
duke_getpid(duk_context *ctx)
{
    return duk_push_int(ctx, getpid()), 1;
}

LIB_PARAM(getppid, 0);
static duk_ret_t
duke_getppid(duk_context *ctx)
{
    return duk_push_int(ctx, getppid()), 1;
}

// 26.4 Creating a Process
LIB_PARAM(fork, 0);
static duk_ret_t
duke_fork(duk_context *ctx)
{
    int pid = fork();

    return __push_error(ctx, pid), 1;
}

LIB_PARAM(vfork, 0);
static duk_ret_t
duke_vfork(duk_context *ctx)
{
    int pid = vfork();

    return __push_error(ctx, pid), 1;
}

LIB_PARAM(execv, 2);
static duk_ret_t
duke_execv(duk_context *ctx)
{
    int i;
    
    char *filename = (char *)duk_require_string(ctx, 0);
    int len = __get_array_length(ctx, 1);
    char *arg[len];

    for (i=0; i<len-1; i++) {
        arg[i] = __get_array_string(ctx, 1, i, NULL);
    }
    arg[len-1] = NULL;

    int err = execv(filename, arg);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(execve, 3);
static duk_ret_t
duke_execve(duk_context *ctx)
{
    int i;
    
    char *filename = (char *)duk_require_string(ctx, 0);
    int arg_len = __get_array_length(ctx, 1);
    int env_len = __get_array_length(ctx, 2);
    char *arg[arg_len];
    char *env[env_len];

    for (i=0; i<arg_len-1; i++) {
        arg[i] = __get_array_string(ctx, 1, i, NULL);
    }
    arg[arg_len-1] = NULL;

    for (i=0; i<env_len-1; i++) {
        env[i] = __get_array_string(ctx, 2, i, NULL);
    }
    env[env_len-1] = NULL;

    int err = execve(filename, arg, env);
 
    return __push_error(ctx, err), 1;
}

LIB_PARAM(execvp, 2);
static duk_ret_t
duke_execvp(duk_context *ctx)
{
    int i;
    
    char *filename = (char *)duk_require_string(ctx, 0);
    int len = __get_array_length(ctx, 1);
    char *arg[len];

    for (i=0; i<len-1; i++) {
        arg[i] = __get_array_string(ctx, 1, i, NULL);
    }
    arg[len-1] = NULL;

    int err = execvp(filename, arg);

    return __push_error(ctx, err), 1;
}

// 26.6 Process Completion
LIB_PARAM(waitpid, 3);
static duk_ret_t
duke_waitpid(duk_context *ctx)
{
    int status;
    int pid = duk_require_int(ctx, 0);
    int opt = duk_require_int(ctx, 2);

    int err = waitpid(pid, &status, opt);
    if (err>0) {
        __set_obj_int(ctx, 1, "status", status);
    }
    
    return __push_error(ctx, err), 1;
}

LIB_PARAM(wait, 1);
static duk_ret_t
duke_wait(duk_context *ctx)
{
    int status;
    
    int err = wait(&status);
    if (err>0) {
        __set_obj_int(ctx, 1, "status", status);
    }
    
    return __push_error(ctx, err), 1;
}

LIB_PARAM(wait4, 4);
static duk_ret_t
duke_wait4(duk_context *ctx)
{
    int status;
    struct rusage r;
    int pid = duk_require_int(ctx, 0);
    int opt = duk_require_int(ctx, 2);

    int err = wait4(pid, &status, opt, &r);
    if (err>0) {
        __set_obj_int(ctx, 1, "status", status);
        __set_rusage(ctx, 3, &r);
    }
    
    return __push_error(ctx, err), 1;
}

LIB_PARAM(WIFEXITED, 1);
static duk_ret_t
duke_WIFEXITED(duk_context *ctx)
{
    int status = duk_require_int(ctx, 0);

    return duk_push_int(ctx, WIFEXITED(status)), 1;
}

LIB_PARAM(WEXITSTATUS, 1);
static duk_ret_t
duke_WEXITSTATUS(duk_context *ctx)
{
    int status = duk_require_int(ctx, 0);

    return duk_push_int(ctx, WEXITSTATUS(status)), 1;
}

LIB_PARAM(WIFSIGNALED, 1);
static duk_ret_t
duke_WIFSIGNALED(duk_context *ctx)
{
    int status = duk_require_int(ctx, 0);

    return duk_push_int(ctx, WIFSIGNALED(status)), 1;
}

LIB_PARAM(WTERMSIG, 1);
static duk_ret_t
duke_WTERMSIG(duk_context *ctx)
{
    int status = duk_require_int(ctx, 0);

    return duk_push_int(ctx, WTERMSIG(status)), 1;
}

LIB_PARAM(WCOREDUMP, 1);
static duk_ret_t
duke_WCOREDUMP(duk_context *ctx)
{
    int status = duk_require_int(ctx, 0);

    return duk_push_int(ctx, WCOREDUMP(status)), 1;
}

LIB_PARAM(WIFSTOPPED, 1);
static duk_ret_t
duke_WIFSTOPPED(duk_context *ctx)
{
    int status = duk_require_int(ctx, 0);

    return duk_push_int(ctx, WIFSTOPPED(status)), 1;
}

LIB_PARAM(WSTOPSIG, 1);
static duk_ret_t
duke_WSTOPSIG(duk_context *ctx)
{
    int status = duk_require_int(ctx, 0);

    return duk_push_int(ctx, WSTOPSIG(status)), 1;
}

// 27 Job Control
// 27.7 Functions for Job Control
LIB_PARAM(setsid, 0);
static duk_ret_t
duke_setsid(duk_context *ctx)
{
    int err = setsid();

    return __push_error(ctx, err), 1;
}

LIB_PARAM(getpgrp, 0);
static duk_ret_t
duke_getpgrp(duk_context *ctx)
{
    int err = getpgrp();

    return __push_error(ctx, err), 1;
}

LIB_PARAM(getpgid, 0);
static duk_ret_t
duke_getpgid(duk_context *ctx)
{
    int pid = duk_require_int(ctx, 0);
    
    int err = getpgid(pid);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(setpgid, 2);
static duk_ret_t
duke_setpgid(duk_context *ctx)
{
    int pid = duk_require_int(ctx, 0);
    int pgid = duk_require_int(ctx, 1);
    
    int err = setpgid(pid, pgid);

    return __push_error(ctx, err), 1;
}

// 27.7.3 Functions for Controlling Terminal Access
LIB_PARAM(tcgetpgrp, 1);
static duk_ret_t
duke_tcgetpgrp(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    
    int err = tcgetpgrp(fd);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(tcsetpgrp, 2);
static duk_ret_t
duke_tcsetpgrp(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    int pgid = duk_require_int(ctx, 1);
    
    int err = tcsetpgrp(fd, pgid);

    return __push_error(ctx, err), 1;
}

// 29 Users and Groups
LIB_PARAM(getuid, 0);
static duk_ret_t
duke_getuid(duk_context *ctx)
{
    return duk_push_uint(ctx, getuid()), 1;
}

LIB_PARAM(getgid, 0);
static duk_ret_t
duke_getgid(duk_context *ctx)
{
    return duk_push_uint(ctx, getgid()), 1;
}

LIB_PARAM(geteuid, 0);
static duk_ret_t
duke_geteuid(duk_context *ctx)
{
    return duk_push_uint(ctx, geteuid()), 1;
}

LIB_PARAM(getegid, 0);
static duk_ret_t
duke_getegid(duk_context *ctx)
{
    return duk_push_uint(ctx, getegid()), 1;
}

LIB_PARAM(getgroups, 1);
static duk_ret_t
duke_getgroups(duk_context *ctx)
{
    int i, err;
    int count = __get_array_length(ctx, 0);
    if (0==count) {
        return duk_push_int(ctx, getgroups(0, NULL)), 1;
    }

    gid_t groups[count];
    err = getgroups(count, groups);
    if (err>0) {
        for (i=0; i<count; i++) {
            __set_array_uint(ctx, 0, i, groups[i]);
        }
    }
    
    return __push_error(ctx, err), 1;
}

LIB_PARAM(seteuid, 1);
static duk_ret_t
duke_seteuid(duk_context *ctx)
{
    uid_t uid = duk_require_uint(ctx, 0);

    int err = seteuid(uid);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(setuid, 1);
static duk_ret_t
duke_setuid(duk_context *ctx)
{
    uid_t uid = duk_require_uint(ctx, 0);

    int err = setuid(uid);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(setreuid, 2);
static duk_ret_t
duke_setreuid(duk_context *ctx)
{
    uid_t ruid = duk_require_uint(ctx, 0);
    uid_t euid = duk_require_uint(ctx, 1);

    int err = setreuid(ruid, euid);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(setegid, 1);
static duk_ret_t
duke_setegid(duk_context *ctx)
{
    gid_t gid = duk_require_uint(ctx, 0);

    int err = setegid(gid);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(setgid, 1);
static duk_ret_t
duke_setgid(duk_context *ctx)
{
    gid_t gid = duk_require_uint(ctx, 0);

    int err = setgid(gid);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(setregid, 2);
static duk_ret_t
duke_setregid(duk_context *ctx)
{
    gid_t rgid = duk_require_uint(ctx, 0);
    gid_t egid = duk_require_uint(ctx, 1);

    int err = setregid(rgid, egid);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(setgroups, 2);
static duk_ret_t
duke_setgroups(duk_context *ctx)
{
    int i, err;
    int count = __get_array_length(ctx, 0);
    gid_t groups[count];

    for (i=0; i<count; i++) {
        groups[i] = __get_array_uint(ctx, 0, i);
    }
    
    err = setgroups(count, groups);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(initgroups, 2);
static duk_ret_t
duke_initgroups(duk_context *ctx)
{
    char *user = (char *)duk_require_string(ctx, 0);
    gid_t group = duk_require_uint(ctx, 1);
    
    int err = initgroups(user, group);

    return __push_error(ctx, err), 1;
}

// 29.11 Identifying Who Logged In
LIB_PARAM(getlogin, 0);
static duk_ret_t
duke_getlogin(duk_context *ctx)
{
    char *name = getlogin();

    return __push_string(ctx, name), 1;
}

// 29.12.1 Manipulating the User Accounting Database
LIB_PARAM(setutent, 0);
static duk_ret_t
duke_setutent(duk_context *ctx)
{
    return setutent(), 0;
}

LIB_PARAM(getutent, 0);
static duk_ret_t
duke_getutent(duk_context *ctx)
{
    struct utmp *p = getutent();

    return __obj_push(ctx, __set_utmp, p), 1;
}

LIB_PARAM(endutent, 0);
static duk_ret_t
duke_endutent(duk_context *ctx)
{
    return endutent(), 0;
}

LIB_PARAM(getutid, 1);
static duk_ret_t
duke_getutid(duk_context *ctx)
{
    struct utmp u;

    __get_utmp(ctx, 0, &u);
    
    struct utmp *p = getutid(&u);

    return __obj_push(ctx, __set_utmp, p), 1;
}

LIB_PARAM(getutline, 1);
static duk_ret_t
duke_getutline(duk_context *ctx)
{
    struct utmp u;

    __get_utmp(ctx, 0, &u);
    
    struct utmp *p = getutline(&u);

    return __obj_push(ctx, __set_utmp, p), 1;
}

LIB_PARAM(pututline, 1);
static duk_ret_t
duke_pututline(duk_context *ctx)
{
    struct utmp u;

    __get_utmp(ctx, 0, &u);
    
    struct utmp *p = pututline(&u);

    return __obj_push(ctx, __set_utmp, p), 1;
}

LIB_PARAM(utmpname, 1);
static duk_ret_t
duke_utmpname(duk_context *ctx)
{
    char *file = (char *)duk_require_string(ctx, 0);

    int err = utmpname(file);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(updwtmp, 2);
static duk_ret_t
duke_updwtmp(duk_context *ctx)
{
    struct utmp u;
    char *file = (char *)duk_require_string(ctx, 0);
    __get_utmp(ctx, 1, &u);
    
    return updwtmp(file, &u), 0;
}

#if duk_LIBC_UTIL
// 29.12.3 Logging In and Out
LIB_PARAM(login_tty, 1);
static duk_ret_t
duke_login_tty(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);

    int err = login_tty(fd);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(login, 1);
static duk_ret_t
duke_login(duk_context *ctx)
{
    struct utmp u;

    __get_utmp(ctx, 0, &u);

    return login(&u), 0;
}

LIB_PARAM(logout, 1);
static duk_ret_t
duke_logout(duk_context *ctx)
{
    char *line = (char *)duk_require_string(ctx, 0);

    int err = logout(line);
    
    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(logwtmp, 3);
static duk_ret_t
duke_logwtmp(duk_context *ctx)
{
    char *line = (char *)duk_require_string(ctx, 0);
    char *name = (char *)duk_require_string(ctx, 1);
    char *host = (char *)duk_require_string(ctx, 2);
    
    return logwtmp(line, name, host), 0;
}
#endif /* duk_LIBC_UTIL */

// 29.13 User Database
LIB_PARAM(getpwuid, 1);
static duk_ret_t
duke_getpwuid(duk_context *ctx)
{
    uid_t uid = duk_require_uint(ctx, 0);
    
    struct passwd *p = getpwuid(uid);

    return __obj_push(ctx, __set_passwd, p), 1;
}

LIB_PARAM(getpwnam, 1);
static duk_ret_t
duke_getpwnam(duk_context *ctx)
{
    char *name = (char *)duk_require_string(ctx, 0);
    
    struct passwd *p = getpwnam(name);

    return __obj_push(ctx, __set_passwd, p), 1;
}

// 29.13.3 Scanning the List of All Users
LIB_PARAM(fgetpwent, 1);
static duk_ret_t
duke_fgetpwent(duk_context *ctx)
{
    FILE *f = (FILE *)duk_require_pointer(ctx, 0);
    
    struct passwd *p = fgetpwent(f);

    return __obj_push(ctx, __set_passwd, p), 1;
}

LIB_PARAM(setpwent, 0);
static duk_ret_t
duke_setpwent(duk_context *ctx)
{
    return setpwent(), 0;
}

LIB_PARAM(getpwent, 0);
static duk_ret_t
duke_getpwent(duk_context *ctx)
{
    struct passwd *p = getpwent();

    return __obj_push(ctx, __set_passwd, p), 1;
}

LIB_PARAM(endpwent, 0);
static duk_ret_t
duke_endpwent(duk_context *ctx)
{
    return endpwent(), 0;
}

// 29.13.4 Writing a User Entry
LIB_PARAM(putpwent, 2);
static duk_ret_t
duke_putpwent(duk_context *ctx)
{
    struct passwd u;
    
    __get_passwd(ctx, 0, &u);
    FILE *f = (FILE *)duk_require_pointer(ctx, 1);
    
    int err = putpwent(&u, f);

    return __push_error(ctx, err), 1;
}

// 29.14 Group Database
LIB_PARAM(getgrgid, 1);
static duk_ret_t
duke_getgrgid(duk_context *ctx)
{
    gid_t gid = duk_require_uint(ctx, 0);
    
    struct group *p = getgrgid(gid);

    return __obj_push(ctx, __set_group, p), 1;
}

LIB_PARAM(getgrnam, 1);
static duk_ret_t
duke_getgrnam(duk_context *ctx)
{
    char *name = (char *)duk_require_string(ctx, 0);
    
    struct group *p = getgrnam(name);

    return __obj_push(ctx, __set_group, p), 1;
}

LIB_PARAM(fgetgrent, 1);
static duk_ret_t
duke_fgetgrent(duk_context *ctx)
{
    FILE *f = (FILE *)duk_require_pointer(ctx, 0);
    
    struct group *p = fgetgrent(f);

    return __obj_push(ctx, __set_group, p), 1;
}

LIB_PARAM(setgrent, 0);
static duk_ret_t
duke_setgrent(duk_context *ctx)
{
    return setgrent(), 0;
}

LIB_PARAM(getgrent, 0);
static duk_ret_t
duke_getgrent(duk_context *ctx)
{
    struct group *p = getgrent();

    return __obj_push(ctx, __set_group, p), 1;
}

LIB_PARAM(endgrent, 0);
static duk_ret_t
duke_endgrent(duk_context *ctx)
{
    return endgrent(), 0;
}


// 30 System Management
LIB_PARAM(gethostname, 0);
static duk_ret_t
duke_gethostname(duk_context *ctx)
{
    int err, size = OS_LINE_LEN;
    char *line = NULL;
    
    while(1) {
        line = (char *)os_realloc(line, 1+size);
        
        err = gethostname(line, size);
        if (0==err) { // 0 is ok
            __push_string(ctx, line);

            break;
        }
        else if (err<0 && ENAMETOOLONG!=errno) {
            seterrno(ctx);
            duk_push_null(ctx);

            break;
        }
    }

error:
    os_free(line);
    return 1;
}

LIB_PARAM(sethostname, 1);
static duk_ret_t
duke_sethostname(duk_context *ctx)
{
    duk_size_t size;
    char *line = (char *)duk_require_lstring(ctx, 0, &size);

    int err = sethostname(line, size);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(getdomainname, 0);
static duk_ret_t
duke_getdomainname(duk_context *ctx)
{
    int err, size = OS_LINE_LEN;
    char *line = NULL;
    
    while(1) {
        line = (char *)os_realloc(line, 1+size);
        
        err = getdomainname(line, size);
        if (0==err) { // 0 is ok
            __push_string(ctx, line);

            break;
        }
        else if (err<0 && ENAMETOOLONG!=errno) {
            seterrno(ctx);
            duk_push_null(ctx);

            break;
        }
    }

error:
    os_free(line);
    return 1;
}

LIB_PARAM(setdomainname, 1);
static duk_ret_t
duke_setdomainname(duk_context *ctx)
{
    duk_size_t size;
    char *line = (char *)duk_require_lstring(ctx, 0, &size);

    int err = setdomainname(line, size);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(gethostid, 0);
static duk_ret_t
duke_gethostid(duk_context *ctx)
{
    return duk_push_int(ctx, gethostid()), 1;
}

LIB_PARAM(sethostid, 1);
static duk_ret_t
duke_sethostid(duk_context *ctx)
{
    int id = duk_require_int(ctx, 0);

    int err = sethostid(id);

    return __push_error(ctx, err), 1;
}

// 30.2 Platform Type Identification
LIB_PARAM(uname, 0);
static duk_ret_t
duke_uname(duk_context *ctx)
{
    struct utsname name;

    int err = uname(&name);
    if (err<0) {
        seterrno(ctx, err);

        duk_push_null(ctx);
    } else {
        __obj_push(ctx, __set_utsname, &name);
    }

    return 1;
}

// 30.3.1 Mount Information
LIB_PARAM(setfsent, 0);
static duk_ret_t
duke_setfsent(duk_context *ctx)
{
    return duk_push_int(ctx, setfsent()), 1;
}

LIB_PARAM(endfsent, 0);
static duk_ret_t
duke_endfsent(duk_context *ctx)
{
    return endfsent(), 0;
}

LIB_PARAM(getfsent, 0);
static duk_ret_t
duke_getfsent(duk_context *ctx)
{
    struct fstab *p = getfsent();

    return __obj_push(ctx, __set_fstab, p), 1;
}

LIB_PARAM(getfsspec, 1);
static duk_ret_t
duke_getfsspec(duk_context *ctx)
{
    char *name = (char *)duk_require_string(ctx, 0);
    
    struct fstab *p = getfsspec(name);

    return __obj_push(ctx, __set_fstab, p), 1;
}

LIB_PARAM(getfsfile, 1);
static duk_ret_t
duke_getfsfile(duk_context *ctx)
{
    char *name = (char *)duk_require_string(ctx, 0);
    
    struct fstab *p = getfsfile(name);

    return __obj_push(ctx, __set_fstab, p), 1;
}

LIB_PARAM(setmntent, 2);
static duk_ret_t
duke_setmntent(duk_context *ctx)
{
    char *name = (char *)duk_require_string(ctx, 0);
    char *mode = (char *)duk_require_string(ctx, 1);
    
    FILE *f = setmntent(name, mode);

    return __push_pointer(ctx, f), 1;
}

LIB_PARAM(endmntent, 1);
static duk_ret_t
duke_endmntent(duk_context *ctx)
{
    FILE *f = duk_require_pointer(ctx, 0);
    
    return duk_push_int(ctx, endmntent(f)), 1;
}

LIB_PARAM(getmntent, 1);
static duk_ret_t
duke_getmntent(duk_context *ctx)
{
    FILE *f = duk_require_pointer(ctx, 0);
    
    struct mntent *p = getmntent(f);

    return __obj_push(ctx, __set_mntent, p), 1;
}

LIB_PARAM(addmntent, 2);
static duk_ret_t
duke_addmntent(duk_context *ctx)
{
    struct mntent m;
    
    FILE *f = duk_require_pointer(ctx, 0);
    __get_mntent(ctx, 1, &m);
    
    int err = addmntent(f, &m);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(hasmntopt, 2);
static duk_ret_t
duke_hasmntopt(duk_context *ctx)
{
    struct mntent m;
    
    __get_mntent(ctx, 0, &m);
    char *mnt = (char *)duk_require_string(ctx, 1);
    
    char *p = hasmntopt(&m, mnt);
    
    return __push_string(ctx, p), 1;
}

// 30.3.2 Mount, Unmount, Remount
LIB_PARAM(mount, 4);
static duk_ret_t
duke_mount(duk_context *ctx)
{
    char *file  = (char *)duk_require_string(ctx, 0);
    char *dir   = (char *)duk_require_string(ctx, 1);
    char *fstype= (char *)duk_require_string(ctx, 2);
    duk_uint_t opt = duk_require_uint(ctx, 3);
    
    int err = mount(file, dir, fstype, opt, "");

    return __push_error(ctx, err), 1;
}

LIB_PARAM(umount2, 2);
static duk_ret_t
duke_umount2(duk_context *ctx)
{
    char *file  = (char *)duk_require_string(ctx, 0);
    int flags   = duk_require_uint(ctx, 1);
    
    int err = umount2(file, flags);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(umount, 1);
static duk_ret_t
duke_umount(duk_context *ctx)
{
    char *file  = (char *)duk_require_string(ctx, 0);
    
    int err = umount(file);

    return __push_error(ctx, err), 1;
}

// 30.4 System Parameters
// sysctl


// 31 System Configuration Parameters
// 31.4 Using sysconf
LIB_PARAM(sysconf, 1);
static duk_ret_t
duke_sysconf(duk_context *ctx)
{
    int parameter = duk_require_int(ctx, 0);
    
    int err = sysconf(parameter);

    return __push_error(ctx, err), 1;
}

// 31.9 Using pathconf
LIB_PARAM(pathconf, 2);
static duk_ret_t
duke_pathconf(duk_context *ctx)
{
    char *filename = (char *)duk_require_string(ctx, 0);
    int parameter = duk_require_int(ctx, 1);
    
    int err = pathconf(filename, parameter);

    return __push_error(ctx, err), 1;
}

LIB_PARAM(fpathconf, 2);
static duk_ret_t
duke_fpathconf(duk_context *ctx)
{
    int fd = duk_require_int(ctx, 0);
    int parameter = duk_require_int(ctx, 1);
    
    int err = fpathconf(fd, parameter);

    return __push_error(ctx, err), 1;
}

// 31.12 String-Valued Parameters
LIB_PARAM(confstr, 1);
static duk_ret_t
duke_confstr(duk_context *ctx)
{
    int err;
    
    int parameter = duk_require_int(ctx, 0);
    if (false==duk_is_dynamic_buffer(ctx, 1)) {
        err = __seterrno(ctx, -EINVAL); goto error;
    }
    
    size_t len = confstr(parameter, NULL, 0);
    char *buf = (char *)duk_resize_buffer(ctx, 1, 1+len);

    err = confstr(parameter, buf, len);
    if (err<0) {
        seterrno(ctx);
    }

error:
    return duk_push_int(ctx, err), 1;
}

// 32 DES Encryption and Password Handling


#include "libc/libcf.c"
#include "libc/libcn.c"
#include "libc/libcs.c"
#include "libc/libcp.c"

int libc_register(duk_context *ctx)
{
    duk_push_global_object(ctx);
        duk_push_object(ctx);
            libcf_register(ctx, -1);
            libcn_register(ctx, -1);
            libcs_register(ctx, -1);
            libcp_register(ctx, -1);
        duk_put_prop_string(ctx, -2, duk_MOD_LIBC);
    duk_pop(ctx);
    
    return 0;
}

