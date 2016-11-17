/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
static inline char *
__pipe_std_cursor(pipe_std_t *std)
{
    return sb_cursor(&std->sb);
}

static inline uint32
__pipe_std_left(pipe_std_t *std)
{
    return sb_left(&std->sb);
}

static inline int
__pipe_std_expand(pipe_std_t *std)
{
    return sb_expand(&std->sb, 0);
}

static int
__pipe_std_read(pipe_std_t *std)
{
    int err, len, left;

    while(1) {
        left = __pipe_std_left(std);
        
        len = read(std->fd[__pipe_father], __pipe_std_cursor(std), left);
        if (len<0) {
            return -errno;
        }
        else if (len < left) {
            std->sb.len += len;
            
            return len;
        }

        err = __pipe_std_expand(std);
        if (err<0) {
            return err;
        }
    }

    return -ENOMEM;
}

static int
__pipe_init(pipexec_t *pe, pipinfo_t *info)
{
    int err;
    char **env;
    
    os_objcpy(&pe->info, info);
    
    err = sb_init(&pe->std[1].sb, 0, 0, 0, 0);
    if (err<0) {
        return err;
    }

    err = sb_init(&pe->std[2].sb, 0, 0, 0, 0);
    if (err<0) {
        return err;
    }
    
    err = pipe(pe->std[1].fd);
    if (err<0) {
        return -errno;
    }
    
    err = pipe(pe->std[2].fd);
    if (err<0) {
        return -errno;
    }

    return 0;
}

static void
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

static bool
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

static int
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
                    err = -errno; goto error;
                }
            case 0: /* timeout, retry */
                err = -ETIMEOUT; goto error;
            default: /* to accept */
                break;
        }

        if (FD_ISSET(pe->std[1].fd[__pipe_father], &rset)) {
            err = __pipe_std_read(&pe->std[1]);
            if (err<0) {
                goto error;
            }
        }
        
        if (FD_ISSET(pe->std[2].fd[__pipe_father], &rset)) {
            err = __pipe_std_read(&pe->std[2]);
            if (err<0) {
                goto error;
            }
        }

        if (__pipe_father_wait(pe, son)) {
            err = 0; goto error;
        }
    }

error:
    os_close(pe->std[1].fd[__pipe_father]);
    os_close(pe->std[2].fd[__pipe_father]);

    return err;
}

static void
__pipe_son_exec(pipexec_t *pe, char *path, char *argv[], char *env[])
{
    envs_dump("exec argv", argv, debug_trace);
    envs_dump("exec env",  env,  debug_trace);
    
    // re-link 1/2
    close(1);
    close(2);

    dup2(pe->std[1].fd[__pipe_son], 1);
    dup2(pe->std[2].fd[__pipe_son], 2);
    
    os_close(pe->std[1].fd[__pipe_father]);
    os_close(pe->std[2].fd[__pipe_father]);
    os_close(pe->std[1].fd[__pipe_son]);
    os_close(pe->std[2].fd[__pipe_son]);

    execvpe(path, argv, env);
}

static int
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

int
os_pexecv(pipinfo_t *info)
{
    pipexec_t pe = PIPEXEC_INITER(info->size, info->minsize, info->expand);
    int err = 0, pid = 0;
    
    if (NULL==info) {
        return -EINVAL0;
    }
    else if (NULL==info->cb) {
        return -EINVAL1;
    }

    dump_pipinfo(info, debug_trace);
    
    __pipe_init(&pe, info);
    
    pid = fork();
    if (pid<0) {
        err = -errno;
    }
    else if (pid>0) { // father
        err = __pipe_father_handle(&pe, pid);
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
/******************************************************************************/

