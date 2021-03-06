/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#if IS_PRODUCT_LTEFI_MD


static struct {
    int times;
    int current;
    int interval;
    
    int stantard;   // stantard value
    int low;        // low value
    int high;       // high value

    char *script;
    char time[1+FULLTIME_STRING_LEN];
    hisi_gpio_t *gpio_delay;
} voltage = {
    .low        = VOLTAGE_STANTARD - VOLTAGE_LOW,
    .high       = VOLTAGE_STANTARD + VOLTAGE_HIGH,
    .stantard   = VOLTAGE_STANTARD,
    .interval   = VOLTAGE_INTERVAL,
    .script     = VOLTAGE_SCRIPT,
};

static int
voltage_script(char *action, char *time)
{
    if (os_file_exist(voltage.script)) {
        /*
        * $1: action
        * $2: action_time
        * $3: last_time
        * $4: voltage
        * $5: low
        * $6: high
        */
        os_shell("%s %s '%s' '%s' " VOLTAGE_FORMAT " %d %d &", 
            voltage.script, 
            action,
            voltage.time,
            time,
            voltage.current,
            voltage.low,
            voltage.high);
    }
}

static int
voltage_report(void)
{
    voltage_script("log", __unknow);
}

static void
voltage_high(void)
{
    static char high[1+FULLTIME_STRING_LEN];

    if (0==high[0]) {
        os_strcpy(high, unsafe_fulltime_string(time(NULL)));
    }
    
    voltage_script("high", high);
}

static void
voltage_low(void)
{
    static char low[1+FULLTIME_STRING_LEN];

    gpio_write(voltage.gpio_delay, 0);

    if (0==low[0]) {
        os_strcpy(low, unsafe_fulltime_string(time(NULL)));
    }

    voltage_script("low", low);
}

static void
voltage_monitor(void)
{
    int err = __get_voltage(&voltage.current);
    if (err<0) {
        debug_error("get voltage error:%d", err);
    }
    
    os_strcpy(voltage.time, unsafe_fulltime_string(time(NULL)));
    
    voltage_report();
    
    if (voltage.current < voltage.low) {
        voltage_low();
    } else if (voltage.current > voltage.high) {
        voltage_high();
    }
}

static int
init_voltage_env(void)
{
    char *env;
    int val;

    env = env_gets(OS_ENV(STANTARD), NULL);
    if (env) {
        val = os_atoi(env);
        if (val <= 0) {
            debug_error("invalid " OS_ENV(STANTARD) ":%s", env);
    
            return -EBADENV;
        }
    
        voltage.stantard = val;
    }
    
    env = env_gets(OS_ENV(LOW), NULL);
    if (env) {
        val = os_atoi(env);
        if (val <= 0) {
            debug_error("invalid " OS_ENV(LOW) ":%s", env);
    
            return -EBADENV;
        }
    
        voltage.low = voltage.stantard - val;
    }
    
    env = env_gets(OS_ENV(HIGH), NULL);
    if (env) {
        val = os_atoi(env);
        if (val <= 0) {
            debug_error("invalid " OS_ENV(HIGH) ":%s", env);
    
            return -EBADENV;
        }
    
        voltage.high = voltage.stantard + val;
    }
    
    return 0;
}

static int
init_voltage(void)
{
    int err;

    hisi_gpio_t *gpio = gpio_getbyname(GPIO_IDX_SHUT_DELAY);
    if (NULL==gpio) {
        debug_error("no found gpio %s", GPIO_IDX_SHUT_DELAY);

        return -ENOEXIST;
    }
    voltage.gpio_delay = gpio;

    err = init_voltage_env();
    if (err<0) {
        return err;
    }
    
    return 0;
}

static int
cmd_voltage_monitor(int argc, char *argv[])
{
    int err;
    
    (void)argc;
    (void)argv;

    err = init_voltage();
    if (err<0) {
        return err;
    }
    
    while(1) {
        voltage_monitor();

        sleep(voltage.interval);
    }

    return 0;
}

static int
cmd_voltage_show(int argc, char *argv[])
{
    float voltage = 0;
    int err;
    
    (void)argc;
    (void)argv;
    
    err = __get_voltage(&voltage);
    if (err<0) {
        return err;
    }

    os_println(VOLTAGE_FORMAT, voltage);

    return 0;
}

static int
voltage_usage(void)
{
#define USAGE_MODULE    __THIS_APPNAME " voltage "
    os_eprintln(USAGE_MODULE "monitor");
#undef USAGE_MODULE

    return -EFORMAT;
}

static cmd_table_t cmd_voltage[] = {
    CMD_TABLE_ENTRY(cmd_voltage_monitor, 1, "monitor"),
    CMD_TABLE_ENTRY(cmd_voltage_show, 1, "show"),
};

static int
voltage_main(int argc, char *argv[])
{
    int err = os_call(hisi_i2c_init, hisi_i2c_fini, cmd_handle, cmd_voltage, argc, argv, voltage_usage);

    return shell_error(err);
}

#endif /* IS_PRODUCT_LTEFI_MD */

