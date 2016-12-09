/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      timer
#endif

#include "utils.h"

OS_INITER;

#define TEST_TIMER_BYFD    1

struct test {
    tm_node_t   node;
    uint32_t expires;
};

int ms = 10;
int count = 1000000;

#define TV_SEC  0
#define TV_USEC (1000*ms)
#define TV_NSEC (TV_USEC*1000)

static mv_t
timer_cb(tm_node_t *timer)
{
    struct test *test = safe_container_of(timer, struct test, node);

    debug_test("timer(%d) timeout, expires(%d)", test->expires, tm_expires(timer));

    free(test);

    return tm_UNSAFE(0);
}

static void
install(void)
{
    int i;
    struct test *test;
    
    for (i=1; i<=count; i++) {
        test = (struct test *)os_zalloc(sizeof(struct test));
        if (test) {
            test->expires = i;

            tm_insert(&test->node, i, timer_cb, true);
        }
    }
}

#if TEST_TIMER_BYFD
int fd;

static int
setup_timers(void)
{
    fd = tm_fd(TV_SEC, TV_NSEC);

    install();
    
    while(1) {
        tm_fd_trigger(fd);
    }

    return 0;
}

#else

static void sigroutine(int id)
{
    switch(id) {
        case SIGALRM: /* timer */
            debug_test("trigger timer(%d)", 1);
            
            tm_trigger(1);
        
            signal(SIGALRM, sigroutine);
            
            break;
        default:
            break;
    }
}

static int 
setup_timers(void)
{
    struct itimerval itimer = OS_ITIMERVAL_INITER(TV_SEC, TV_USEC);
    
    signal(SIGALRM, sigroutine);
    debug_test("setup signal");
    
    /*
    * 安装定时器
    */
    setitimer(ITIMER_REAL, &itimer, NULL);
    debug_test("setup timer");

    install();
    debug_test("install timer");
    
    while(1) {
        usleep(1);
    }

    return 0;
}

#endif /* TEST_TIMER_BYFD */

int main(int argc, char *argv[])
{
    if (argc>=2) {
        ms = os_atoi(argv[1]);
    }
    if (argc>=3) {
        count = os_atoi(argv[2]);
    }
    
    setup_timers();

    return 0;
}

static os_destructor void
__fini(void)
{
    tm_fini();
    os_fini();
}

static os_constructor void
__init(void)
{
    os_init();
    tm_init();
}
/******************************************************************************/
