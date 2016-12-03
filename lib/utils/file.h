#ifndef __FILE_H_aa663bfdbdf54b3b9d97875399c8404f__
#define __FILE_H_aa663bfdbdf54b3b9d97875399c8404f__
#ifdef __APP__
/******************************************************************************/
#ifndef FILE_DPRINT
#define FILE_DPRINT             0
#endif

#if FILE_DPRINT
#define file_println(_fmt, _args...)    os_println(_fmt, ##_args)
#else
#define file_println(_fmt, _args...)    os_do_nothing()
#endif

extern char *
os_vfmtline(const char *fmt, va_list args);

extern char *
os_fmtline(const char *fmt, ...);

extern char *
os_basename(const char *file);

static inline bool
os_file_exist(const char *file)
{
    int fd = open(file, O_RDONLY, S_IRUSR | S_IRGRP);

    return fd<0?false:(close(fd), true);
}

static inline bool
os_file_absolute(const char *file)
{
    return is_good_str(file) && '/'==file[0];
}

static inline bool
os_file_relative(const char *file)
{
    if (is_good_str(file)) {
        int len = os_strlen(file);
        
        if (len>2 && '.'==file[0] && '/'==file[1]) {
            return true;
        }
        else if (len>3 && '.'==file[0] && '.'==file[1] && '/'==file[2]) {
            return true;
        }
    }

    return false;
}

#define __v_xopen(_type, _func, _mod, _fmt, _args...) ({  \
    char *__line = NULL;                    \
    _type __v = (_type)0;                   \
                                            \
    if (os_asprintf(&__line, _fmt, ##_args) > 0 && __line) { \
        file_println("__v_xopen %s", __line); \
        __v = _func(__line, _mod);          \
        free(__line);                       \
    }                                       \
                                            \
    __v;                                    \
})

#define os_v_popen(_mod, _fmt, _args...)    \
    __v_xopen(STREAM, popen, _mod, _fmt, ##_args)
#define os_v_fopen(_mod, _fmt, _args...)    \
    __v_xopen(STREAM, fopen, _mod, _fmt, ##_args)
#define os_v_open(_flag, _fmt, _args...)    \
    __v_xopen(int, open, _flag, _fmt, ##_args)

/*
* get file size by full filename(include path)
*/
extern int
os_fsize(const char *file);

extern int
os_v_fsize(const char *fmt, ...);

extern int
os_system_helper(char *cmd);

extern int
os_vsystem(const char *fmt, va_list args);

extern int
os_system(const char *fmt, ...);

#define os_fopen(_file, _mode)      fopen(_file, _mode)
#define os_fclean(_file)            __os_fclose(fopen(_file, "w"))

static inline int
__os_fclose(STREAM stream)
{
    int err = 0;

    if (stream) {
        err = fclose(stream);
    }

    return err;
}

#define os_fclose(_stream)          ({  \
    int __err = __os_fclose(_stream);   \
    _stream = NULL;                     \
    __err;                              \
})  /* end */

#define os_feof(_stream)            (_stream?!!feof(_stream):true)
#define os_fflush(_stream)          fflush(_stream)
#define os_ferror(_stream)          ferror(_stream)
#define os_fdopen(_fd, _flag)       fdopen(_fd, _flag)

static inline int 
os_fseek(STREAM stream, int offset, int fromwhere)
{
    int err = fseek(stream, offset, fromwhere);

    return err<0?-errno:err;
}

static inline int
os_fwrite(STREAM stream, const void *buf, int size)
{
    int err = fwrite(buf, 1, size, stream);
    
    os_fflush(stream);
    
    return (err<0)?-errno:err;
}

static inline int
os_fwrite_crlf(STREAM stream)
{
    return os_fwrite(stream, __crlf, sizeof(__crlf) - 1);
}

static inline int
os_fread(STREAM stream, void *buf, int size)
{
    int err = fread(buf, 1, size, stream);

    return (err<0)?-errno:err;
}

static inline char *
os_freadline(STREAM stream, char *line, int size)
{
    char *p = NULL;

    if (stream && line && size>0) {
        p = fgets(line, size, stream);
    }

    return p;
}

static inline int
os_fwriteline(STREAM stream, const char *line)
{
    int len = 0;

    if (stream && line) {
        len = fputs(line, stream);
        fputc('\n', stream);
    }

    return len;
}

extern int
os_readfile(const char *file, void *buf, int size);

extern int
os_readfileall(const char *file, char **content, uint32 *filesize, bool bin);

extern int
os_writefile(const char *file, void *buf, int size, bool append, bool bin);

static inline int
os_getfile_byurl(const char *file, const char *url)
{
    return os_system("curl %s -o %s --create-dirs", url, file);
}

extern char *
__os_readfd(int fd, int block) ;

#define os_readfd(_fd)      __os_readfd(_fd, 32*1024)

static inline bool
is_current_dir(const char *file/* not include path */)
{
    return '.'==file[0] && 0==file[1];
}

static inline bool
is_father_dir(const char *file/* not include path */)
{
    return '.'==file[0] && '.'==file[1] && 0==file[2];
}

/*
* @filename: not include path
*/
typedef mv_t os_fscan_line_handle_f(const char *file, char *line);
typedef mv_t os_fscan_file_handle_f(const char *path, const char *file, os_fscan_line_handle_f *line_handle);
typedef bool os_fscan_file_filter_f(const char *path, const char *file);

/*
* @filefilter: if return true, ignore file
*/
extern int 
os_fscan_dir
(
    const char *path, 
    bool recur,
    os_fscan_file_filter_f *file_filter,
    os_fscan_file_handle_f *file_handle,
    os_fscan_line_handle_f *line_handle
);

extern int
os_fsearch_path(const char *file, const char *path, int (*handle)(const char *file, void *user), void *user);

extern int
os_fsearch_paths(const char *file, const char *PATH, int (*handle)(const char *file, void *user), void *user);
/******************************************************************************/
#define ____os_xgetv(_prefix, _stream, _vfmt, _pv) ({ \
    int __err = 0;          \
                            \
    if (NULL==(_stream)) {  \
        __err = -errno;     \
    } else if (1!=fscanf(_stream, _vfmt, _pv)) {  \
        __err = -EFORMAT;   \
    }                       \
                            \
    if (_stream) {          \
        __err = __os_wait_error(_prefix##close(_stream)); \
    }                       \
                            \
    __err;                  \
})

#define ____os_xgets(_prefix, _stream, _line, _space) ({ \
    int __err = 0;              \
                                \
    if (NULL==(_line)) {        \
        __err = -EINVAL9;       \
    } else if ((_space)<=0) {   \
        __err = -EINVAL8;       \
    } else if (NULL==_stream) { \
        __err = -EINVAL7;       \
    } else if (NULL==os_freadline(_stream, _line, _space)) { \
        __err = -errno;         \
    } else {                    \
        os_str_strim_both(_line, NULL); \
    }                           \
                                \
    if (_stream) {              \
        __err = __os_wait_error(_prefix##close(_stream)); \
    }                           \
                                \
    __err;                      \
}) /* end */

#define ____os_xgetb(_prefix, _stream, _buf, _size) ({ \
    int __err = 0;              \
                                \
    if (NULL==(_buf)) {         \
        __err = -EINVAL9;       \
    } else if ((_size)<=0) {    \
        __err = -EINVAL8;       \
    } else if (NULL==_stream) { \
        __err = -EINVAL7;       \
    } else if (os_fread(_stream, _buf, _size)<0) { \
        __err = -errno;         \
    }                           \
                                \
    if (_stream) {              \
        __err = __os_wait_error(_prefix##close(_stream)); \
    }                           \
                                \
    __err;                      \
}) /* end */

#define __os_xgetv(_prefix, _vfmt, _pv, _filename) ({ \
    STREAM __f = _prefix##open(_filename, "r");  \
    ____os_xgetv(_prefix, __f, _vfmt, _pv);     \
})  /* end */

#define __os_xgets(_prefix, _line, _space, _filename) ({ \
    STREAM __f = _prefix##open(_filename, "r");  \
    ____os_xgets(_prefix, __f, _line, _space);  \
})  /* end */

#define __os_xgetb(_prefix, _buf, _size, _filename) ({ \
    STREAM __f = _prefix##open(_filename, "r");  \
    ____os_xgetb(_prefix, __f, _buf, _size);    \
})  /* end */

#define __os_xgeti(_prefix, _pi, _filename)      __os_xgetv(_prefix, "%u", _pi, _filename)
#define __os_xgetll(_prefix, _pll, _filename)    __os_xgetv(_prefix, "%llu", _pll, _filename)

#define __os_v_xgetv(_prefix, _vfmt, _pv, _fmt, _args...) ({ \
    STREAM __f = os_v_##_prefix##open("r", _fmt, ##_args);   \
    ____os_xgetv(_prefix, __f, _vfmt, _pv);                 \
})  /* end */

#define __os_v_xgets(_prefix, _line, _space, _fmt, _args...) ({ \
    STREAM __f = os_v_##_prefix##open("r", _fmt, ##_args);   \
    ____os_xgets(_prefix, __f, _line, _space);              \
})  /* end */

#define __os_v_xgetb(_prefix, _buf, _size, _fmt, _args...) ({ \
    STREAM __f = os_v_##_prefix##open("r", _fmt, ##_args);   \
    ____os_xgetb(_prefix, __f, _buf, _size);                \
})  /* end */

#define __os_v_xgeti(_prefix, _pi, _fmt, _args...)      __os_v_xgetv(_prefix, "%u", _pi, _fmt, ##_args)
#define __os_v_xgetll(_prefix, _pll, _fmt, _args...)    __os_v_xgetv(_prefix, "%u", _pll, _fmt, ##_args)

/*
* get from file
*/
#define os_fgets(_line, _space, _filename)          __os_xgets(f, _line, _space, _filename)
#define os_fgetb(_buf, _size, _filename)            __os_xgetb(f, _buf, _size, _filename)
#define os_fgeti(_pi, _filename)                    __os_xgeti(f, _pi, _filename)
#define os_fgetll(_pll, _filename)                  __os_xgetll(f, _pll, _filename)

#define os_v_fgets(_line, _space, _fmt, _args...)   __os_v_xgets(f, _line, _space, _fmt, ##_args)
#define os_v_fgetb(_buf, _size, _fmt, _args...)     __os_v_xgetb(f, _buf, _size, _fmt, ##_args)
#define os_v_fgeti(_pi, _fmt, _args...)             __os_v_xgeti(f, _pi, _fmt, ##_args)
#define os_v_fgetll(_pll, _fmt, _args...)           __os_v_xgeti(f, _pll, _fmt, ##_args)

/*
* get from pipe
*/
#define os_pgets(_line, _space, _filename)          __os_xgets(p, _line, _space, _filename)
#define os_pgetb(_buf, _size, _filename)            __os_xgetb(p, _buf, _size, _filename)
#define os_pgeti(_pi, _filename)                    __os_xgeti(p, _pi, _filename)
#define os_pgetll(_pll, _filename)                  __os_xgetll(p, _pll, _filename)

#define os_v_pgets(_line, _space, _fmt, _args...)   __os_v_xgets(p, _line, _space, _fmt, ##_args)
#define os_v_pgetb(_buf, _size, _fmt, _args...)     __os_v_xgetb(p, _buf, _size, _fmt, ##_args)
#define os_v_pgeti(_pi, _fmt, _args...)             __os_v_xgeti(p, _pi, _fmt, ##_args)
#define os_v_pgetll(_pll, _fmt, _args...)           __os_v_xgeti(p, _pll, _fmt, ##_args)

#define ____os_xsetv(_prefix, _stream, _vfmt, v) ({ \
    int __err = 0;              \
                                \
    if (NULL==(_stream)) {      \
        __err = -errno;         \
    } else {                    \
        __err = fprintf(_stream, _vfmt, v); \
    }                           \
                                \
    if (_stream) {              \
        __err = __os_wait_error(_prefix##close(_stream)); \
    }                           \
                                \
    __err;                      \
})  /* end */

#define ____os_xsetb(_prefix, _stream, _buf, _size) ({ \
    int __err = 0;              \
                                \
    if (NULL==(_stream)) {      \
        __err = -errno;         \
    } else {                    \
        __err = os_fwrite(_stream, _buf, _size); \
    }                           \
                                \
    if (_stream) {              \
        __err = __os_wait_error(_prefix##close(_stream)); \
    }                           \
                                \
    __err;                      \
})  /* end */

#define __os_xsetv(_prefix, _vfmt, _v, _filename) ({ \
    STREAM __f = _prefix##open(_filename, "w");  \
    ____os_xsetv(_prefix, __f, _vfmt, _v);      \
})  /* end */

#define __os_xsetb(_prefix, _buf, _size, _filename) ({ \
    STREAM __f = _prefix##open(_filename, "w");  \
    ____os_xsetb(_prefix, __f, _buf, _size);    \
})  /* end */

#define __os_v_xsetv(_prefix, _vfmt, _v, _fmt, _args...) ({ \
    STREAM __f = os_v_##_prefix##open("w", _fmt, ##_args);   \
    ____os_xsetv(_prefix, __f, _vfmt, _v);                  \
})  /* end */

#define __os_v_xsetb(_prefix, _buf, _size, _fmt, _args...) ({ \
    STREAM __f = os_v_##_prefix##open("w", _fmt, ##_args);   \
    ____os_xsetb(_prefix, __f, _buf, _size);                  \
})  /* end */

#define __os_xsets(_prefix, _string, _filename)         __os_xsetv(_prefix, "%s", _string, _filename)
#define __os_xseti(_prefix, _vi, _filename)             __os_xsetv(_prefix, "%u", _vi, _filename)
#define __os_xsetll(_prefix, _vll, _filename)           __os_xsetv(_prefix, "%llu", _vll, _filename)

#define __os_v_xsets(_prefix, _string, _fmt, _args...)      __os_v_xsetv(_prefix, "%s", _string, _fmt, ##_args)
#define __os_v_xseti(_prefix, _vi, _fmt, _args...)          __os_v_xsetv(_prefix, "%u", _vi, _fmt, ##_args)
#define __os_v_xsetll(_prefix, _vll, _fmt, _args...)        __os_v_xsetv(_prefix, "%llu", _vll, _fmt, ##_args)

/*
* set to file
*/
#define os_fsets(_string, _filename)        __os_xsets(f, _string, _filename)
#define os_fsetb(_buf, _size, _filename)    __os_xsetb(f, _buf, _size, _filename)
#define os_fseti(_vi, _filename)            __os_xseti(f, _vi, _filename)
#define os_fsetll(_vll, _filename)          __os_xsetll(f, _vll, _filename)

#define os_v_fsets(_string, _fmt, _args...)     __os_v_xsets(f, _string, _fmt, ##_args)
#define os_v_fsetb(_buf, _size, _fmt, _args...) __os_v_xsetb(f, _buf, _size, _fmt, ##_args)
#define os_v_fseti(_vi, _fmt, _args...)         __os_v_xseti(f, _vi, _fmt, ##_args)
#define os_v_fsetll(_vll, _fmt, _args...)       __os_v_xsetll(f, _vll, _fmt, ##_args)

static inline int
os_file_open(const char *file, int open_mode, int permit)
{
    int deft_permit = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
    int deft_mode   = O_RDONLY | O_CREAT;
    
    int fd = open(file, open_mode?open_mode:deft_mode, permit?permit:deft_permit);
    if (fd<0) {
        return -errno;
    }

    return fd;
}

extern int
__os_file_unlock(int fd);

extern int
__os_file_lock(const char *file, int open_mode, int permit, bool block);

extern int __THIS_LOCKFD;

static inline int
os_file_lock(bool block)
{
    __THIS_LOCKFD = __os_file_lock(__THIS_LOCKFILE, 0, (S_IRUSR | S_IRGRP), block);

    return is_good_fd(__THIS_LOCKFD)?0:__THIS_LOCKFD;
}

#define os_file_unlock()        ({  \
    __os_file_unlock(__THIS_LOCKFD); \
    __THIS_LOCKFD = INVALID_FD;     \
})

extern int
os_fgeti_ex(const char *file, int max, int min, int deft);

#ifndef SCRIPT_SHELL_SYSTEM
#define SCRIPT_SHELL_SYSTEM     SCRIPT_FILE("system.script")
#endif

#define os_shell(_fmt, _args...)    ({  \
    int __os_p_system_err = 0;          \
    int __os_p_system_code = 0;         \
                                        \
    if (os_file_exist(SCRIPT_SHELL_SYSTEM)) { \
        __os_p_system_err = os_v_pgeti(&__os_p_system_code, SCRIPT_SHELL_SYSTEM __space _fmt, ##_args); \
        if (0==__os_p_system_err) {     \
            __os_p_system_err = __os_p_system_code; \
        }                               \
    } else {                            \
        __os_p_system_err = os_system(_fmt, ##_args); \
    }                                   \
                                        \
    native_error(__os_p_system_err);    \
})  /* end */

/******************************************************************************/
#endif /* __APP__ */
#endif /* __FILE_H_aa663bfdbdf54b3b9d97875399c8404f__ */
