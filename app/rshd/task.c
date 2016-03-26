/*******************************************************************************
Copyright (c) 2015-2016, xxx Networks. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      rshd
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     task
#endif

#include "rshd.h"
/******************************************************************************/
typedef struct {
    pid_t pid;
    time_t create;

    int fd[2];

    tm_node_t tm;
    sockaddr_in_t client;
    rsh_msg_t req;
    rsh_multi_msg_t multi;
    
    struct {
        struct list_head list;

        struct {
            struct hlist_node pid;
            struct hlist_node client;
        } hash;
    } node;
}
rsh_task_t;

static inline rsh_task_t *
task_entry(tm_node_t *timer)
{
    return container_of(timer, rsh_task_t, tm);
}

#define RSH_HASH_CLIENT     256
#define RSH_HASH_PID        256

static struct {
    struct {
        struct {
            struct hlist_head pid[RSH_HASH_PID];
            struct hlist_head client[RSH_HASH_CLIENT];
        } hash;
        
        struct list_head  list;
        
        uint32_t count;
    } head;
    
    rsh_task_t *pipe[1024];
} binding = {
    .head = {
        .list = LIST_HEAD_INIT(binding.head.list),
    },
};

static inline void
task_bind(rsh_task_t *task)
{
    binding[task->fd[0]] = task;
}

static inline void
task_unbind(rsh_task_t *task)
{
    binding[task->fd[0]] = NULL;
}

static inline void
task_free(rsh_task_t *task)
{
    if (__is_good_fd(task->fd[0])) {
        task_unbind(task);
        
        __os_close(task->fd[0]);
    }
    
    if (__is_good_fd(task->fd[1])) {
        __os_close(task->fd[1]);
    }
    
    os_free(task->multi.buffer);
    os_free(task);
}

static rsh_task_t *
task_new(rsh_msg_t *msg, sockaddr_in_t *client)
{
    int i, err;
    
    rsh_task_t *task = (rsh_task_t *)os_zalloc(sizeof(*task));
    if (NULL==task) {
        return NULL;
    }

    task->multi.buffer = (char *)os_zalloc(RSH_MSG_DSIZE);
    if (NULL==task->multi.buffer) {
        goto error;
    }
    
    time(&task->create);
    task->client    = *client;
    task->req       = *msg;

    int maxrun = msg->u.maxrun;
    task->req.u.error = 0;
    
    err = pipe(task->fd);
    if (err<0) {
        return err;
    }
    
    task_bind(task);
    
    return task;
error:
    task_free(task);

    return NULL;
}

static int
task_insert(rsh_task_t *task)
{
    if (NULL==task) {
        return -EINVAL;
    }
    
    list_add_tail(&task->node.list, &rsh.head.list);
    rsh.head.count++;

    return 0;
}

static int
task_remove(rsh_task_t *task)
{
    if (NULL==task) {
        return -EINVAL;
    }
    
    rsh.head.count--;
    list_del(&task->node.list);

    return 0;
}

static int
task_foreach(mv_t (*cb)(rsh_task_t *task))
{
    rsh_task_t *task, *n;
    mv_u mv;
    
    list_for_each_entry_safe(task, n, &rsh.head.list, node.list) {
        mv.v = (*cb)(task);
        if (is_mv2_break(mv)) {
            return mv2_error(mv);
        }
    }

    return 0;
}

static rsh_task_t *
task_getbypipe(int fd)
{
    return binding[fd];
}

static rsh_task_t *
task_getbypid(pid_t pid)
{
    rsh_task_t *found = NULL;
    
    mv_t cb(rsh_task_t *task)
    {
        if (pid==task->pid) {
            found = task;

            return mv2_break(0);
        } else {
            return mv2_ok;
        }
    }

    task_foreach(cb);

    return found;
}

static int
task_create(pid_t pid)
{
    if (task_getbypid(pid)) {
        return -EEXIST;
    } else {
        return task_insert(task_new(pid));
    }
}

static int
task_delete(pid_t pid)
{
    mv_t cb(rsh_task_t *task)
    {
        if (pid==task->pid) {
            task_remove(task);

            return mv2_break(0);
        } else {
            return mv2_ok;
        }
    }
    
    task_foreach(cb);

    return 0;
}

static int
task_timeout(void)
{
    time_t now = time(NULL);

    mv_t cb(rsh_task_t *task)
    {
        if ((now - task->create) > RSH_RUNTIME) {
            log_debug("task pid:%d timeout", task->pid);
            
            kill(task->pid, SIGKILL);
            
            task_remove(task);
        }

        return mv2_ok;
    }
    
    return task_foreach(cb);
}

static void
task_wait(void)
{
    pid_t pid;

    while((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        log_debug("waited pid:%d", pid);
        
        task_delete(pid);
    }
}

static mv_t
task_timeout(struct __tm_node *timer)
{
    rsh_task_t *task = task_entry(timer);

    task_remove(task);
    task_free(task);
}

static void
__del_timer(int type)
{
    tm_remove(&rsh.timer[type].node);
}

static int
__add_timer(int type)
{
    rsh_timer_t *timer = &rsh.timer[type];
        
    return tm_insert(&timer->node, timer->interval, timer->cb, false);
}

static int
add_timer(int type)
{
    int err = 0;
    
    switch(type) {
        case RSH_TIMER_APING:
            __del_timer(RSH_TIMER_IPING);
            break;
        case RSH_TIMER_IPING:
            __del_timer(RSH_TIMER_APING);
            break;
        default:
            break;
    }

    __del_timer(type);
    
    return __add_timer(type);
}

static int
add_pinger(void)
{
    int type;
    
    if (rsh.alive) {
        type = RSH_TIMER_APING;
    } else {
        type = RSH_TIMER_IPING;
    }

    return add_timer(type);
}

mv_t
rsh_pinger(tm_node_t *timer)
{
    rsh.ping++;
    
    return tm_SAFE(0);
}

mv_t
rsh_linker(tm_node_t *timer)
{
    return tm_SAFE(0);
}

mv_t
rsh_tasker(tm_node_t *timer)
{
    return tm_SAFE(0);
}

mv_t
rsh_connecter(tm_node_t *timer)
{
    int err;
    
    err = __connect();
    if (err<0) {
        return tm_SAFE(0);
    }

    err = __register();
    if (err<0) {
        return tm_SAFE(0);
    }
    
    /*
    * connect and register ok
    *   1. auto remove connect timer
    *   2. add iping timer
    */
    tm_once(timer);

    add_pinger();
    
    return tm_SAFE(0);
}

int
rsh_timer_init(void)
{
    int fd, err;
    
    tm_init();

    err = add_timer(RSH_TIMER_LINK);
    if (err<0) {
        return err;
    }

    err = add_timer(RSH_TIMER_CONNECT);
    if (err<0) {
        return err;
    }

    return 0;
}

int
rsh_task_wait(struct signalfd_siginfo *siginfo)
{
    int pid, status, err;

    while((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        rsh_task_t *task = task_getbypid(pid);
        if (NULL==task) {
            // log
            return 0;
        }

        pipe_info_t *info = &task->info;

        os_noblock(info->std[__stdout].fd[__pipe_father]);
        err = __pipe_std_read(&info->std[__stdout]);
        if (err<0) {
            return err;
        }

        os_noblock(info->std[__stderr].fd[__pipe_father]);
        err = __pipe_std_read(&info->std[__stderr]);
        if (err<0) {
            return err;
        }

        rsh_msg_t *msg = &task->multi.msg;
        rsh_tlv_t *tlv;

        tlv = rsh_m_tlv(msg, RSH_TLV_ERRNO);
        rsh_tlv_number(tlv) = __os_wait_error(status);

        tlv = rsh_m_tlv(msg, RSH_TLV_STDOUT);
        rsh_tlv_pointer(tlv) = info->std[__stdout].buf; info->std[__stdout].buf = NULL;

        tlv = rsh_m_tlv(msg, RSH_TLV_STDERR);
        rsh_tlv_pointer(tlv) = info->std[__stderr].buf; info->std[__stderr].buf = NULL;
    }
    
    return 0;
}

int
rsh_child(rsh_msg_t *msg, sockaddr_in_t *client)
{
    int i, err = 0;
    rsh_task_t *task = task_new(msg, client);
    if (NULL==task) {
        return -ENOMEM;
    }

    int maxrun = 0;
    char *input = NULL;
    
    pid_t pid = fork();
    if (pid<0) {
        err = -errno;
    }
    else if (pid>0) { // father
        task->pid = pid;
        
        err = tm_insert(&task->tm, 
                    os_ms2tick(1000*maxrun, RSH_TICKS), 
                    task_timeout, 
                    true);
        if (err<0) {
            return err;
        }
    }
    else { // child
        close(0); dup2(task->fd, 0);
        close(1); dup2(task->fd, 1);
        close(task->fd);
        
        execl("/bin/sh", "sh", "-c", msg->body.data, NULL);
    }

    return err;
}

int
rsh_pipe_handle(int fd)
{
    int i, err, len;
    
    rsh_task_t *task = task_getbypipe(fd);
    if (NULL==task) {
        // log
        return 0;
    }

    return 0;
}

/******************************************************************************/
