/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifdef __APP__

char *
os_basename(char *filename)
{
    int len = strlen(filename);
    char *p = filename + len - 1;

    while(p>filename && *p && *p != '/') {
        p--;
    }

    if (*p == '/') {
        p++;
    }
    
    return p;
}

int
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

static int
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

int
os_v_fsize(const char *fmt, ...)
{
    int err;
    va_list args;
    
    va_start(args, (char *)fmt);
    err = __v_fsize(fmt, args);
    va_end(args);
    
    return err;
}

int
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

int
os_system(const char *fmt, ...)
{
    int err;
    va_list args;
    
    va_start(args, (char *)fmt);
    err = os_vsystem(fmt, args);
    va_end(args);
    
    return err;
}

int
os_readfile(char *filename, void *buf, int size)
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

int
os_readfileall(char *filename, char **content, uint32 *filesize, bool bin)
{
    char *buf = NULL;
    int pad = bin?0:1;
    int size, err = 0;
    
    size = os_fsize(filename);
    if (size<0) {
        goto error;
    }

    buf = (char *)os_malloc(pad+size);
    if (NULL==buf) {
        goto error;
    }
    
    err = os_readfile(filename, buf, size);
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

int
os_writefile(char *filename, void *buf, int size, bool append, bool bin)
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

char *
os_readfd(int fd, int block) 
{
	FILE *f = NULL;
	char *buf = NULL;
	long sz = 0, len = 0, left, ret;  /* ANSI C typing */

	if (fd<0) {
		goto error;
	}
	
	f = os_fdopen(fd, "r");
	if (!f) {
		goto error;
	}

	while(!os_feof(f) && !os_ferror(f)) {
        if (0==sz) {
            sz += block;
            buf = (char *)os_malloc(sz);
        }
        else if (sz - len < 512) {
            sz += block;
            buf = (char *)os_realloc(buf, sz);
        }
        else {
            /*
            * use the buf, do nothing
            */
        }
        
        if (NULL==buf) {
            goto error;
        }

	    left = sz - len;
        ret = os_fread(f, buf + len, left);
        if (ret > left || ret < 0) {
            goto error;
        }
        
        len += ret;
	}
	buf[len++] = 0;

    if ('#'==buf[0] && '!'==buf[1]) {
        buf[0] = '/';
        buf[1] = '/';
    }

	return buf;
error:
    os_free(buf);
    os_fclose(f);
    
    return NULL;
}

static mv_t 
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

    if (file_handle) {
        file_println("user file handle %s/%s ...", path, filename);
        mv.v = (*file_handle)(path, filename, line_handle);
        file_println("user file handle %s/%s %d", path, filename, mv2_error(mv));

        return mv.v
    }
    else if (NULL==line_handle) {
        file_println("no line handle !!!");
        
        return mv2_go(-ENOSUPPORT);
    }
    
    file_println("inline file handle %s/%s ...", path, filename);
    
    stream = os_v_fopen("r", "%s/%s", path, filename);
    if (NULL==stream) {
        file_println("open %s/%s error:%d", path, filename, -errno);
        
        mv2_error(mv) = -errno;
        
        goto error;
    }
    
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
            file_println("skip blank line");
            
            continue;
        }
        
        /*
        * skip notes line
        */
        if (__is_notes_line_deft(line)) {
            file_println("skip notes line");
            
            continue;
        }
        
        mv.v = (*line_handle)(filename, line);
        file_println("line handle %s:%s error:%d", filename, line, mv2_error(mv));
        if (is_mv2_break(mv)) {
            goto error;
        }
    }

    mv.v = mv2_ok;
error:
    os_fclose(stream);
    file_println("inline file handle %s/%s error:%d", path, filename, mv2_error(mv));
    
    return mv.v;
}

int 
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
    struct dirent *d = NULL;
    struct stat st;
    mv_u mv;
    int err = 0;

    if (NULL==path) {
        return os_assertV(-EINVAL1);
    }

    file_println("begin scan %s", path);
    
    dir = opendir(path);
    if (NULL == dir) {
        file_println("open dir %s error:%d", path, -errno);
        
        err = -errno; goto error;
    }
    
    while (NULL != (d=readdir(dir))) {
        /* 
        * d->d_name
        *   just name, not include path 
        */
        file_println("scan %s/%s", path, d->d_name);

        /*
        * skip . and ..
        */
        if (is_current_dir(d->d_name) || is_father_dir(d->d_name)) {
            file_println("skip %s/%s", path, d->d_name);
            
            continue;
        }

        err = stat(d->d_name, &st);
        if (err<0) {
            file_println("stat %s error:%d", d->d_name, err);
            
            continue;
        }
        
        /*
        * dir
        */
        if (S_ISDIR(st.st_mode)) {
            if (recur) {
                char line[1+OS_LINE_LEN];

                os_sprintf(line, "%s/%s", path, d->d_name);
                
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
        if (file_filter && (*file_filter)(path, d->d_name)) {
            file_println("filter %s/%s", path, d->d_name);
            
            continue;
        }
        
        /*
        * file handle
        */
        mv.v = __os_fscan_file_handle(path, d->d_name, file_handle, line_handle);
        if (is_mv2_break(mv)) {
            err = mv2_error(mv);

            file_println("handle %s/%s error:%d", path, d->d_name, err);
            
            goto error;
        }
    }

    file_println("end scan %s error:%d", path, err);
error:
    os_closedir(dir);

    return err;
}

int
os_fsearch_path(char *file, char *path, int (*handle)(char *filename, void *user), void *user)
{
    char fullname[1+OS_LINE_LEN] = {0};
    
    if (NULL==path) {
        return -ENOEXIST;
    }
    
    /*
    * path last char is '/'
    */
    int len = os_strlen(path);
    if (len>1 && '/'==path[len-1]) {
        os_saprintf(fullname, "%s%s", path, file);
    } else {
        os_saprintf(fullname, "%s/%s", path, file);
    }

    if (os_file_exist(fullname)) {
        return (*handle)(fullname, user);
    } else {
        return -ENOEXIST;
    }
}

int
os_fsearch_paths(char *file, char *PATH, int (*handle)(char *filename, void *user), void *user)
{
    char *paths = NULL, *path;
    int err = 0;

    paths = (char *)os_strdup(PATH);
    if (NULL==paths) {
        return -ENOMEM;
    }

    os_strtok_foreach(path, paths, ":") {
        err = os_fsearch_path(file, path, handle, user);
        if (0==err) {
            goto error;
        }
    }

    err = -ENOEXIST;
error:
    os_free(paths);

    return err;
}

int __THIS_LOCKFD = INVALID_FD;

    
int
__os_file_lock(char *file, int open_mode, int permit, bool block)
{
    int fd = os_file_open(file, open_mode, permit);
    if (fd<0) {
        return fd;
    }
    
    int err = flock(fd, LOCK_EX | (block?0:LOCK_NB));
    if (err<0) {
        return -errno;
    }
    
    return fd;
}

int
__os_file_unlock(int fd)
{
    int err = 0;

    if (false==is_good_fd(fd)) {
        return -ENOLOCK;
    }
    
    err = flock(fd, LOCK_UN);
    if (err<0) {
        /* NOT return */
    }

    err = close(fd);
    if (err<0) {
        return err;
    }

    return 0;
}

int
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

#endif
/******************************************************************************/
