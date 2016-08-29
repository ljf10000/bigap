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
static inline int
os_fsize(char *filename)
{
    struct stat st;
    int err;
    
    err = stat(filename, &st);
    if (err<0) {
        return -errno;
    } else {
        return st.st_size;
    }
}

static inline int
__v_fsize(const char *fmt, va_list args)
{
    int err = 0;
    char *filename = NULL;

    err = os_vasprintf(&filename, fmt, args);
    if (err>0) {
        err = os_fsize(filename);
    }
    os_free(filename);

    return err;
}

static inline int
os_v_fsize(const char *fmt, ...)
{
    int err;
    va_list args;
    
    va_start(args, (char *)fmt);
    err = __v_fsize(fmt, args);
    va_end(args);
    
    return err;
}

static inline int __os_system(char *cmd);

static inline int
os_vsystem(const char *fmt, va_list args)
{
    int err = 0;
    char *cmd = NULL;

    err = os_vasprintf(&cmd, fmt, args);
    if (err>0) {
        err = __os_system(cmd);
    }
    os_free(cmd);

    return native_error(err);
}

static inline int
os_system(const char *fmt, ...)
{
    int err;
    va_list args;
    
    va_start(args, (char *)fmt);
    err = os_vsystem(fmt, args);
    va_end(args);
    
    return err;
}

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


static inline int
os_readfile(char *filename, char *buf, int size)
{
    STREAM f = NULL;
    int err = 0;

    f = os_fopen(filename, "r");
    if (NULL==f) {
        err = -errno; goto error;
    }

    int len = os_fread(f, buf, size);
    if (size!=len) {
        err = -errno; goto error;
    }

error:
    os_fclose(f);

    return err;
}

static inline int
os_readfileall(char *filename, char **content, uint32 *filesize, bool bin)
{
    char *buf = NULL;
    int pad = bin?0:1;
    
    int size = os_fsize(filename);
    if (size<0) {
        goto error;
    }

    buf = (char *)os_malloc(pad+size);
    if (NULL==buf) {
        goto error;
    }
    
    int err = os_readfile(filename, buf, size);
    if (err<0) {
        goto error;
    }

    if (false==bin) {
        int tail = buf[size-1];
        
        /*
        * cut tail crlf
        */
        while(size>0 && ('\r'==tail || '\n'==tail)) {
            size--;
            tail = buf[size-1];
        }
        
        buf[size] = 0;
    }

    *filesize   = size;
    *content    = buf;
    
    return err;
error:
    os_free(buf);

    return err;
}

static inline int
os_writefile(char *filename, byte *buf, int size, bool append, bool bin)
{
    STREAM f = NULL;
    int err = 0;

    f = os_fopen(filename, append?"a":"w");
    if (NULL==f) {
        err = -errno; goto error;
    }

    int len = os_fwrite(f, buf, size);
    if (size!=len) {
        err = -errno; goto error;
    }

    if (false==bin) {
        os_fwrite_crlf(f);
    }
    
error:
    os_fclose(f);

    return err;
}

static inline bool
is_current_dir(char *filename/* not include path */)
{
    return '.'==filename[0] && 0==filename[1];
}

static inline bool
is_father_dir(char *filename/* not include path */)
{
    return '.'==filename[0] && '.'==filename[1] && 0==filename[2];
}

/*
* @filename: not include path
*/
typedef mv_t os_fscan_line_handle_f(char *filename, char *line);
typedef mv_t os_fscan_file_handle_f(char *path, char *filename, os_fscan_line_handle_f *line_handle);
typedef bool os_fscan_file_filter_f(char *path, char *filename);

static inline mv_t 
__os_fscan_file_handle
(
    char *path,
    char *filename,
    os_fscan_file_handle_f *file_handle,
    os_fscan_line_handle_f *line_handle
)
{
    STREAM stream = NULL;
    char line[1+OS_LINE_LEN];
    mv_u mv;

    if (NULL==path) {
        return os_assertV(mv2_go(-EINVAL1));
    }
    else if (NULL==filename) {
        return os_assertV(mv2_go(-EINVAL2));
    }

    stream = os_v_fopen("r", "%s/%s", path, filename);
    if (NULL==stream) {
        file_println("open %s/%s error:%d", path, filename, -errno);
        
        mv2_error(mv) = -errno;
        
        goto error;
    }

    if (file_handle) {
        mv.v = (*file_handle)(path, filename, line_handle);
    } else if (line_handle) {
        while(!os_feof(stream)) {
            line[0] = 0;
            os_freadline(stream, line, OS_LINE_LEN);
            
            /*
            * strim left/right blank
            */
            os_str_strim_both(line, NULL);
            
            /*
            * replace blank("\t \r\n") to ' '
            */
            os_str_replace(line, NULL, ' ');
            
            /*
            * reduce ' '
            */
            os_str_reduce(line, NULL);
            
            /*
            * skip blank line
            */
            if (__is_blank_line(line)) {
                continue;
            }
            
            /*
            * skip notes line
            */
            if (__is_notes_line_deft(line)) {
                continue;
            }
            
            mv.v = (*line_handle)(filename, line);
            if (is_mv2_break(mv)) {
                goto error;
            }
        }
    }
    
    mv.v = mv2_ok;
error:
    os_fclose(stream);
    
    return mv.v;
}

/*
* @filefilter: if return true, ignore file
*/
static inline int 
os_fscan_dir
(
    char *path, 
    bool recur,
    os_fscan_file_filter_f *file_filter,
    os_fscan_file_handle_f *file_handle,
    os_fscan_line_handle_f *line_handle
)
{
    DIR *dir = NULL;
    struct dirent *file = NULL;
    struct stat st;
    mv_u mv;
    int err = 0;

    if (NULL==path) {
        return os_assertV(-EINVAL1);
    }

    file_println("begin scan %s", path);
    
    dir = opendir(path);
    if (NULL == dir) {
        err = -errno;

        file_println("open dir %s error:%d", path, -errno);
        
        goto error;
    }
    
    while (NULL != (file=readdir(dir))) {
        char *filename = file->d_name; /* just name, not include path */

        file_println("scan %s/%s", path, filename);

        /*
        * skip . and ..
        */
        if (is_current_dir(filename) || is_father_dir(filename)) {
            file_println("skip %s/%s", path, filename);
            
            continue;
        }
        
        /*
        * dir
        */
        if (stat(filename, &st) >= 0 && S_ISDIR(st.st_mode)) {
            if (recur) {
                char line[1+OS_LINE_LEN];

                os_sprintf(line, "%s/%s", path, filename);
                
                err = os_fscan_dir(line, recur, file_filter, file_handle, line_handle);
                if (err<0) {
                    goto error;
                }
            } else {
                continue;
            }
        }
        
        /*
        * file filter
        */
        if (file_filter && (*file_filter)(path, filename)) {
            file_println("filter %s/%s", path, filename);
            
            continue;
        }
        
        /*
        * file handle
        */
        mv.v = __os_fscan_file_handle(path, filename, file_handle, line_handle);
        if (is_mv2_break(mv)) {
            err = mv2_error(mv);

            file_println("handle %s/%s error:%d", path, filename, err);
            
            goto error;
        }
    }

    file_println("end scan %s error:%d", path, err);
error:
    if (dir) {
        closedir(dir);
    }

    return err;
}

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

#define DECLARE_FAKE_FLOCK  extern int __THIS_LOCKFD
#define DECLARE_REAL_FLOCK  int __THIS_LOCKFD = INVALID_FD

#ifdef __BUSYBOX__
#   define DECLARE_FLOCK    DECLARE_FAKE_FLOCK
#else
#   define DECLARE_FLOCK    DECLARE_REAL_FLOCK
#endif

DECLARE_FAKE_FLOCK;

static inline int
__os_file_lock(char *file, int open_mode, int permit, bool block)
{
    int err, fd;
    int deft_permit = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
    int deft_mode   = O_RDONLY | O_CREAT;
    
    fd = open(file, open_mode?open_mode:deft_mode, permit?permit:deft_permit);
    file_println("open:%s error:%d", file, (err?-errno:0));
    if (fd<0) {
        return -errno;
    }
    
    err = flock(fd, LOCK_EX | (block?0:LOCK_NB));
    file_println("lock:%s error:%d", file, (err?-errno:0));
    if (err<0) {
        return -errno;
    }
    
    return fd;
}

static int
os_file_lock(bool block)
{
    __THIS_LOCKFD = __os_file_lock(__THIS_LOCKFILE, 0, (S_IRUSR | S_IRGRP), block);

    return is_good_fd(__THIS_LOCKFD)?0:__THIS_LOCKFD;
}

static int
__os_file_unlock(char *file, int fd)
{
    int err = 0;

    if (false==is_good_fd(fd)) {
        return -ENOLOCK;
    }
    
    err = flock(fd, LOCK_UN);
    file_println("unlock:%s error:%d", file, (err?-errno:0));
    if (err<0) {
        /* NOT return */
    }

    err = close(fd);
    file_println("close:%s error:%d", file, (err?-errno:0));
    if (err<0) {
        return err;
    }

    return 0;
}

#define os_file_unlock()        ({  \
    __THIS_LOCKFD = INVALID_FD;     \
    __os_file_unlock(__THIS_LOCKFILE, __THIS_LOCKFD); \
})

static inline bool
os_file_exist(const char *file)
{
    int fd = open(file, O_RDONLY, S_IRUSR | S_IRGRP);

    return fd<0?false:(close(fd), true);
}

static inline int
os_fgeti_ex(char *file, int max, int min, int deft)
{
    if (os_file_exist(file)) {
        int val = deft;
        
        os_fgeti(&val, file);

        return OS_SAFE_VALUE_DEFT(val, min,  max, deft);
    } else {
        return deft;
    }
}

#ifndef SCRIPT_SHELL_SYSTEM
#define SCRIPT_SHELL_SYSTEM     SCRIPT_FILE("system.script")
#endif

#define os_p_system(_fmt, _args...) ({  \
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
