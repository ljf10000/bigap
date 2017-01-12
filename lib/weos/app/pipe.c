/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
typedef struct {
    simple_buffer_t sb;

    int fd[2];      /* father/son */
} pipe_std_t;

typedef struct {
    pipe_std_t std[3];  // std in/out/err
    pipinfo_t info;
    
    int err;
} pipexec_t;

enum {
    __pipe_father   = 0,
    __pipe_son      = 1,
};

ALWAYS_INLINE char *
__pipe_std_cursor(pipe_std_t *std)
{
    return sb_cursor(&std->sb);
}

ALWAYS_INLINE uint32
__pipe_std_left(pipe_std_t *std)
{
    return sb_left(&std->sb);
}

ALWAYS_INLINE int
__pipe_std_expand(pipe_std_t *std)
{
    return sb_expand(&std->sb, 0);
}

STATIC int
__pipe_std_read(pipe_std_t *std)
{
    int err, len, left;

    while(1) {
        left = __pipe_std_left(std);
        
        len = read(std->fd[__pipe_father], __pipe_std_cursor(std), left);
        if (len<0) {
            pipe_println("__pipe_std_read read error:%d", -errno);
            
            return -errno;
        }
        else if (len < left) {
            sb_shift(&std->sb, len);
            
            return len;
        }

        err = __pipe_std_expand(std);
        if (err<0) {
            return err;
        }
    }

    return -ENOMEM;
}

STATIC int
__pipe_init(pipexec_t *pe, pipinfo_t *info)
{
    int i, err;
    char **env;

    os_objzero(pe);
    os_objcpy(&pe->info, info);

    for (i=1; i<3; i++) {
        err = sb_init(&pe->std[i].sb, SB_AUTO_INITER(0, 0, 0));
        if (err<0) {
            return err;
        }

        pe->std[i].fd[0] = INVALID_FD;
        pe->std[i].fd[1] = INVALID_FD;

        err = pipe(pe->std[i].fd);
        if (err<0) {
            return -errno;
        }
    }
    
    return 0;
}

STATIC void
__pipe_fini(pipexec_t *pe)
{
    if (pe) {
#if 1
        os_close(pe->std[1].fd[__pipe_father]);
        os_close(pe->std[2].fd[__pipe_father]);
        
        os_close(pe->std[1].fd[__pipe_son]);
        os_close(pe->std[2].fd[__pipe_son]);
#endif
        sb_fini(&pe->std[1].sb);
        sb_fini(&pe->std[2].sb);
    }
}

STATIC bool
__pipe_father_wait(pipexec_t *pe, int son)
{
    int err, status = 0;
    int pid = waitpid(son, &status, WNOHANG);

    if (pid==son) {
        err = pe->err = __os_wait_error(status);

        os_noblock(pe->std[1].fd[__pipe_father]);
        __pipe_std_read(&pe->std[1]);

        os_noblock(pe->std[2].fd[__pipe_father]);
        __pipe_std_read(&pe->std[2]);

        return true;
    }
    else {
        return false;
    }
}

STATIC int
__pipe_father_handle(pipexec_t *pe, int son)
{
    struct timeval tv = {
        .tv_sec     = os_second(pe->info.timeout),
        .tv_usec    = os_usecond(pe->info.timeout),
    };
    fd_set rset;
    
    int max = OS_MAX(pe->std[1].fd[__pipe_father], pe->std[2].fd[__pipe_father]);
    int err = 0;

    os_close(pe->std[1].fd[__pipe_son]);
    os_close(pe->std[2].fd[__pipe_son]);
    
    while(1) {        
        FD_ZERO(&rset);
        FD_SET(pe->std[1].fd[__pipe_father], &rset);
        FD_SET(pe->std[2].fd[__pipe_father], &rset);

        int err = select(1+max, &rset, NULL, NULL, &tv);
        switch(err) {
            case -1:/* error */
                if (EINTR==errno) {
                    break;
                } else {
                    pipe_println("__pipe_father_handle select error:%d", -errno);
                    
                    err = -errno; goto error;
                }
            case 0: /* timeout, retry */
                pipe_println("__pipe_father_handle select timeout");
                
                err = -ETIMEOUT; goto error;
            default: /* to accept */
                break;
        }

        if (FD_ISSET(pe->std[1].fd[__pipe_father], &rset)) {
            err = __pipe_std_read(&pe->std[1]);
            if (err<0) {
                pipe_println("__pipe_father_handle read stdout error:%d", err);
                
                goto error;
            }
        }
        
        if (FD_ISSET(pe->std[2].fd[__pipe_father], &rset)) {
            err = __pipe_std_read(&pe->std[2]);
            if (err<0) {
                pipe_println("__pipe_father_handle read stderr error:%d", err);
                
                goto error;
            }
        }

        if (__pipe_father_wait(pe, son)) {
            pipe_println("__pipe_father_handle wait ok");
            err = 0; goto error;
        }
    }

error:
    os_close(pe->std[1].fd[__pipe_father]);
    os_close(pe->std[2].fd[__pipe_father]);

    return err;
}

STATIC void
__pipe_son_exec(pipexec_t *pe, char *path, char *argv[], char *env[])
{
    envs_dump("exec argv", argv, debug_trace);
    envs_dump("exec env",  env,  debug_trace);
    
    envs_dump("exec argv", argv, pipe_println);
    envs_dump("exec env",  env,  pipe_println);

    pipe_println("exec path:%s", path);
    // re-link 1/2
    close(1);
    close(2);

    dup2(pe->std[1].fd[__pipe_son], 1);
    dup2(pe->std[2].fd[__pipe_son], 2);
    
    os_close(pe->std[1].fd[__pipe_father]);
    os_close(pe->std[2].fd[__pipe_father]);
    os_close(pe->std[1].fd[__pipe_son]);
    os_close(pe->std[2].fd[__pipe_son]);

#ifdef __UCLIBC__
    execvp(path, argv);
#else
    execvpe(path, argv, env);
#endif
}

STATIC int
__pipe_son_handle(pipexec_t *pe)
{
    pipinfo_t *info = &pe->info;
    int err, count;
    
    // append env(private + global)
    info->env = envs_merge(environ, info->env);

    if (info->content) {
        /*
        * SHELL:
        *   /bin/bash
        *   /bin/js
        */
        char *shell = env_gets("SHELL", "/bin/bash");
        char *argv[] = {os_basename(shell), "-c", info->content, NULL};

        __pipe_son_exec(pe, shell, argv, info->env);
    }
    else if (info->file) {
        char *argv[] = {os_basename(info->file), NULL};

        info->argv = envs_merge(info->argv, argv);
        
        __pipe_son_exec(pe, info->file, info->argv, info->env);
    }
    else {
        return -ENOSUPPORT;
    }
    
    // go here is error
    return -errno;
}

#define dump_pipinfo(_info, _dump)          do{ \
    _dump("size=%u", (_info)->size);            \
    _dump("minsize=%u", (_info)->minsize);      \
    _dump("expand=%u", (_info)->expand);        \
    _dump("timeout=%d", (_info)->timeout);      \
    if ((_info)->content) {                     \
        _dump("content=%s", (_info)->content);  \
    }                                           \
    if ((_info)->file) {                        \
        _dump("file=%s", (_info)->file);        \
    }                                           \
    if ((_info)->env) {                         \
        envs_dump("old", (_info)->env, _dump);  \
    }                                           \
    if ((_info)->argv) {                        \
        envs_dump("old", (_info)->argv, _dump); \
    }                                           \
}while(0)

int
os_pexecv(pipinfo_t *info)
{
    pipexec_t pe;
    int err = 0, pid = 0;
    
    if (NULL==info) {
        return -EINVAL0;
    }
    else if (NULL==info->cb) {
        return -EINVAL1;
    }

    dump_pipinfo(info, debug_trace);
    dump_pipinfo(info, pipe_println);
    
    __pipe_init(&pe, info);
    
    pid = fork();
    if (pid<0) {
        err = -errno;
    }
    else if (pid>0) { // father
        pipe_println("os_pexecv father ...");
        err = __pipe_father_handle(&pe, pid);
        pipe_println("os_pexecv father error:%d", err);
        if (0==err) {
            (*info->cb)(pe.err, pe.std[1].sb.buf, pe.std[2].sb.buf);
        }
    }
    else { // child
        err = __pipe_son_handle(&pe);
    }

error:
    __pipe_fini(&pe);

    return err;
}

int
os_vpexec(pipinfo_t *info, const char *fmt, va_list args)
{
    int err = 0;
    char *cmd = NULL;
    pipinfo_t clone;

    err = os_vasprintf(&cmd, fmt, args);
    if (err>0) {
        os_objcpy(&clone, info);

        clone.content = cmd;

        err = os_pexecv(&clone);
    }
    os_free(cmd);

    return err;
}

int
os_pexec(pipinfo_t *info, const char *fmt, ...)
{
    va_list args;

    va_start(args, (char *)fmt);
    int err = os_vpexec(info, fmt, args);
    va_end(args);

    return err;
}

int
os_pexecline(pipinfo_t *info, char *line)
{
    int err, count = os_strcount(line, ' ');
    char *input = os_strdup(line);
    char *argv[1+count];
    char *p;
    
    count = 0;
    os_strtok_foreach(p, input, " ") {
        argv[count++] = p;
    }
    argv[count] = NULL;
    
    if (os_file_exist(argv[0])) {
        info->file = argv[0];
        if (count>1) {
            info->argv = &argv[1];
        }
        
        err = os_pexecv(info);
    } else {
        info->content = line;
        
        err = os_pexec(info, "%s", line);
    }
    
    os_free(input);

    return 0;
}

int 
os_pexec_jcallback(int error, char *outstring, char *errstring)
{
    bool encode = env_geti(OS_ENV(ENCODE), 1);
    char *newoutstring = NULL;
    char *newerrstring = NULL;

    if (encode) {
        newoutstring = b64_encode((byte *)outstring, os_strlen(outstring));
        newerrstring = b64_encode((byte *)errstring, os_strlen(errstring));
    } else {
        newoutstring = outstring;
        newerrstring = errstring;
    }
    
    os_println( "{"
                    "\"stdout\":\"%s\","
                    "\"stderr\":\"%s\","
                    "\"errno\":%d"
                "}", 
        newoutstring,
        newerrstring,
        error);

    if (encode) {
        os_free(newoutstring);
        os_free(newerrstring);
    }
    
    return 0;
}

int
os_pexec_jmap(pipinfo_t *info, char *json)
{
    jobj_t jobj = NULL, jval, jargument;
    int i, count = 0, err = 0;
    
    jobj = jobj_byjson(json);
    if (NULL==jobj) {
        return -EBADJSON;
    }

    jval = jobj_get(jobj, "content");
    if (jval) {
        if (jtype_string != jobj_type(jval)) {
            err = -EBADJSON; goto error;
        }
        
        char *content = jobj_get_string_ex(jval);
        if (content) {
            info->content = b64_decode((byte *)content, os_strlen(content));
            if (NULL==info->content) {
                err = -EBASE64; goto error;
            }
        }
    }

    jval = jobj_get(jobj, "filename");
    if (jval) {
        if (jtype_string != jobj_type(jval)) {
            err = -EBADJSON; goto error;
        }

        char *filename = jobj_get_string_ex(jval);
        if (filename) {
            info->file = os_strdup(filename);
        }
    }

    if (NULL==info->file && NULL==info->content) {
        err = -EBADJSON; goto error;
    }

    jargument = jobj_get(jobj, "argument");
    if (jargument) {
        if (jtype_array != jobj_type(jargument)) {
            err = -EBADJSON; goto error;
        }
        
        count = jarray_length(jargument);
        info->argv = (char **)os_zalloc((1+count) * sizeof(char *));
        if (NULL==info->argv) {
            err = -ENOMEM; goto error;
        }
        
        for (i=0; i<count; i++) {
            jval = jarray_get(jargument, i);
            if (jtype_string != jobj_type(jval)) {
                err = -EBADJSON; goto error;
            }

            info->argv[i] = os_strdup(jobj_get_string(jval));
        }
    }
        
    err = 0;
error:
    jobj_put(jobj);

    return err;
}

void
os_pexec_clean(pipinfo_t *info)
{
    os_free(info->content);
    os_free(info->file);
    
    if (info->argv) {
        char *args;

        for (args = info->argv[0]; args; args++) {
            free(args); // NOT os_free
        }

        os_free(info->argv);
    }
}

int
os_pexec_json(char *json, os_pexec_callback_f *cb)
{
    pipinfo_t info = PIPINFO_INITER(NULL, cb?cb:os_pexec_jcallback);
    int err;
    
    err = os_pexec_jmap(&info, json);
    if (err<0) {
        goto error;
    }

    err = os_pexecv(&info);
    if (err>0) {
        goto error;
    }

error:
    os_pexec_clean(&info);
    
    return err;
}

/******************************************************************************/

