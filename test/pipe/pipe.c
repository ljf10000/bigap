/******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      pipe
#endif

#include "utils.h"

OS_INITER;

#if 1

static int
father(char *line, int fd_in[2], int fd_out[2], int fd_err[2])
{
    char buf[1024*1024];
    fd_set rset, eset;
    int err = 0, max, len;
    
    close(fd_out[1]);
    close(fd_err[1]);

    max = os_max(fd_out[0], fd_err[0]);
    
    while(1) {
        struct timeval tv = {
            .tv_sec     = 3,
            .tv_usec    = 0,
        };
        
        FD_ZERO(&rset);
        FD_SET(fd_out[0], &rset);
        FD_SET(fd_err[0], &rset);

        int err = select(max+1, &rset, NULL, NULL, &tv);
        switch(err) {
            case -1:/* error */
                if (EINTR==errno) {
                    // is breaked
                    
                } else {
                    os_println("select error:%d", -errno);
                }
                break;
            case 0: /* timeout, retry */
                os_println("select timeout");
                
                break;
            default: /* to accept */
                os_println("select ok");
                
                break;
        }

        if (FD_ISSET(fd_out[0], &rset)) {
            os_memzero(buf, sizeof(buf));
            
            len = read(fd_out[0], buf, sizeof(buf)-1);
            if (len<0) {
                os_println("read fd_out error:%d", -errno);
            } else {
                os_println("read fd_out: %s", buf);
            }
        }

        if (FD_ISSET(fd_err[0], &rset)) {
            os_memzero(buf, sizeof(buf));
            
            len = read(fd_err[0], buf, sizeof(buf)-1);
            if (len<0) {
                os_println("read fd_err error:%d", -errno);
            } else {
                os_println("read fd_err: %s", buf);
            }
        }

        int status = 0;
        int pid = waitpid(-1, &status, WNOHANG);
        
        if (pid>0) {
            err = __os_wait_error(status);

            os_noblock(fd_out[0]);
            os_noblock(fd_err[0]);
    
            os_memzero(buf, sizeof(buf));
            
            len = read(fd_out[0], buf, sizeof(buf)-1);
            if (len<0) {
                os_println("read fd_out error:%d", -errno);
            } else {
                os_println("read fd_out: %s", buf);
            }

            os_memzero(buf, sizeof(buf));
            
            len = read(fd_err[0], buf, sizeof(buf)-1);
            if (len<0) {
                os_println("read fd_err error:%d", -errno);
            } else {
                os_println("read fd_err: %s", buf);
            }
            
            os_println("wait pid:%d, error:%d", pid, err);
            
            break;
        }
    }

    close(fd_out[0]);
    close(fd_err[0]);

    return err;
}
#else

static int
father(char *line, int fd_in[2], int fd_out[2], int fd_err[2])
{
    char buf[1024*1024];
    fd_set rset, eset;
    int err = 0, max, len;
    
    close(fd_out[1]);
    close(fd_err[1]);

    os_noblock(fd_out[0]);
    os_noblock(fd_err[0]);

    int status = 0;
    int pid = waitpid(-1, &status, 0);
    
    if (pid>0) {
        err = __os_wait_error(status);
        
        os_println("wait pid:%d, error:%d", pid, err);
    }
    
    os_memzero(buf, sizeof(buf));
    
    len = read(fd_out[0], buf, sizeof(buf)-1);
    if (len<0) {
        os_println("read fd_out error:%d", -errno);
    } else {
        os_println("read fd_out: %s", buf);
    }

    os_memzero(buf, sizeof(buf));
    
    len = read(fd_err[0], buf, sizeof(buf)-1);
    if (len<0) {
        os_println("read fd_err error:%d", -errno);
    } else {
        os_println("read fd_err: %s", buf);
    }

    close(fd_out[0]);
    close(fd_err[0]);

    return err;
}

#endif

static int
son(char *line, int fd_in[2], int fd_out[2], int fd_err[2])
{
    close(1); dup2(fd_out[1], 1); close(fd_out[0]); close(fd_out[1]);
    close(2); dup2(fd_err[1], 2); close(fd_err[0]); close(fd_err[1]);
    
    execl("/bin/sh", "sh", "-c", line, NULL);

    os_println("execl(/bin/sh, sh, -c, %s, NULL) error:%d", line, -errno);
    
    return -errno;
}

static int
run(char *line)
{
    int fd_in[2]    = {INVALID_FD, INVALID_FD};
    int fd_out[2]   = {INVALID_FD, INVALID_FD};
    int fd_err[2]   = {INVALID_FD, INVALID_FD};
    int err = 0;
    int pid = 0;
    
    /*
    * father: read,  close 1
    *    son: write, close 0
    */
    err = pipe(fd_out);
    if (err) {
        return err;
    }

    /*
    * father: read,  close 1
    *    son: write, close 0
    */
    err = pipe(fd_err);
    if (err) {
        return err;
    }

    pid = fork();
    if (pid<0) {
        os_println("fork error:%d", -errno);
        
        err = -errno;
    }
    else if (pid>0) { // father
        os_println("father");
        
        err = father(line, fd_in, fd_out, fd_err);
    }
    else { // child
        os_println("son");
        
        err = son(line, fd_in, fd_out, fd_err);
    }

    return err;
}

int main(int argc, char *argv[])
{
    int err = 0;
    char buf[1024];
    char *line;
    
    while(1) {
        os_memzero(buf, sizeof(buf));
        
        line = fgets(buf, sizeof(buf)-1, stdin);
        if (false==is_good_str(line) || '\n'==line[0]) {
            continue;
        }

        os_str_strim_both(line, NULL);
        
        os_println("get-line:%s", line);
        
        run(line);
    }

    return 0;
}
/******************************************************************************/
