#ifndef __XCC_H_072856b21d1641b4a926c32254d350d1__
#define __XCC_H_072856b21d1641b4a926c32254d350d1__
/******************************************************************************/
#if defined(__APP__) && IS_PRODUCT_LTEFI_MD
#include "hisi/acc.h"
#include "hisi/vcc.h"
/******************************************************************************/
#define XCC_OPEN            1
#define XCC_CLOSE           0

#define XCC_GPIO_COUNT      OS_MAX(ACC_GPIO_COUNT, VCC_GPIO_COUNT)

typedef struct {
    char *name;
    int timeout;
    int interval;
    uint times;
    char *path;
    char *script;

    /*
    * 0: acc/vcc gpio
    * [1,count): when acc/vcc shutdown, set it
    */
    struct {
        hisi_gpio_t *gpio;
        char *name;
        int value;
    } map[1 + XCC_GPIO_COUNT];
    
    int count;
} 
xcc_t;

static inline void
xcc_script(xcc_t *xcc, char *action)
{
    if (xcc->script) {
        os_p_system("%s %s &", xcc->script, action);
    }
}

static inline void
xcc_report(xcc_t *xcc, int value, int duration)
{
    if (xcc->script) {
        os_v_fseti(&value,      "%s/status &",  xcc->path);
        os_v_fseti(&duration,   "%s/duration &",xcc->path);
    }
}

static inline void
xcc_shutdown(xcc_t *xcc)
{
    int i;
        
    xcc_script(xcc, "shutdown");

    for (i=0; i<xcc->count; i++) {
        gpio_write(xcc->map[i].gpio, xcc->map[i].value);

        usleep(os_usecond(xcc->interval));
    }
}

static inline void
__xcc_monitor(xcc_t *xcc)
{
    int value = gpio_read(xcc->map[0].gpio);
    if (XCC_OPEN==value) {
        if (xcc->times) {
            xcc_script(xcc, "open");
        }
        
        xcc->times = 0;
    } else {
        if (0==xcc->times) {
            xcc_script(xcc, "close");
        }
        
        xcc->times++;
    }

    int duration = xcc->interval * xcc->times;
    if (duration > 1000* xcc->timeout) {
        xcc_shutdown(xcc);
    }

    xcc_report(xcc, value, duration);
}

static inline int
init_xcc_env(xcc_t *xcc)
{
    char *env;
    int val;
    
    env = env_gets(OS_ENV(TIMEOUT), NULL);
    if (env) {
        val = os_atoi(env);
        if (val <= 0) {
            debug_error("invalid " OS_ENV(TIMEOUT) ":%s", env);

            return -EBADENV;
        }

        xcc->timeout = val;
    }

    env = env_gets(OS_ENV(INTERVAL), NULL);
    if (env) {
        val = os_atoi(env);
        if (val <= 0) {
            debug_error("invalid " OS_ENV(INTERVAL) ":%s", env);

            return -EBADENV;
        }

        xcc->interval = val;
    }

    env = env_gets(OS_ENV(PATH), NULL);
    if (env) {
        xcc->path = env;
    }
    if (false==os_file_exist(xcc->path)) {
        os_p_system("mkdir -p %s", xcc->path);
    }

    env = env_gets(OS_ENV(SCRIPT), NULL);
    if (env) {        
        xcc->script = env;
    }
    if (false==os_file_exist(env)) {
        debug_trace("%s script:%s not exist!", xcc->name, env);

        xcc->script = NULL;
    }
    
    return 0;
}

static inline int
init_xcc_gpio(xcc_t *xcc)
{
    int i;
    
    for (i=0; i<xcc->count; i++) {
        hisi_gpio_t *gpio = gpio_getbyname(xcc->map[i].name);
        if (NULL==gpio) {
            debug_error("no found gpio %s", xcc->map[i].name);

            return -ENOEXIST;
        }
        
        xcc->map[i].gpio = gpio;
    }
}

static inline int
xcc_monitor(xcc_t *xcc)
{
    int err;
    
    err = init_xcc_env(xcc);
    if (err<0) {
        return err;
    }

    err = init_xcc_gpio(xcc);
    if (err<0) {
        return err;
    }
    
    while(1) {
        __xcc_monitor(xcc);

        usleep(os_usecond(xcc->interval));
    }
}

static inline int
xcc_usage(xcc_t *xcc)
{
    os_eprintln(__THIS_APPNAME "%s monitor", xcc->name);

    return -EFORMAT;
}

/******************************************************************************/
#endif
#endif /* __XCC_H_072856b21d1641b4a926c32254d350d1__ */
