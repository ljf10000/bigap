/*******************************************************************************
Copyright (c) 2016-2018, Supper Wali Technology. All rights reserved.
*******************************************************************************/
#if IS_PRODUCT_LTEFI_MD

static void
gpio_showname(int type)
{
    int i, count = 0;
    hisi_gpio_t *GPIO = gpio_GET(&count);
    struct gpio *gpio;

    for (i=0; i<count; i++) {
        gpio = &GPIO[i];

        if (type & gpio->flag) {
            os_println("\t%s", gpio->name);
        }
    }
}

static int
gpio_help(int argc, char *argv[])
{
    os_println("%s ==> get all input gpio", argv[0]);
	
    os_println("\n%s name ==> get input gpio by name", argv[0]);
	os_println("\t[input gpio name]");
	gpio_showname(GPIO_R);
	
    os_println("\n%s name1=value1 name2=value2 ... ==> set output gpio by name and value", argv[0]);
	os_println("\t[output gpio name]");
	gpio_showname(GPIO_W);

	return -EFORMAT;
}

static int 
gpio_showall(void)
{
    int i, count = 0, var;
    hisi_gpio_t *GPIO = gpio_GET(&count);
    hisi_gpio_t *gpio;
    
    for (i=0; i<count; i++) {
        gpio = &GPIO[i];

        if (GPIO_R & gpio->flag) {
            var = gpio_read(gpio);
            if (var<0) {
                continue;
            }
            
            os_println("%s=%d", gpio->name, var);
        }
    }

    return 0;
}

static int
gpio_getter(char *name)
{
    hisi_gpio_t *gpio = gpio_getbyname(name);
    if (NULL==gpio) {
        os_println("bad gpio name:%s", name);

        return -1;
    }
    
    int var = gpio_read(gpio);
    if (var<0) {
        return var;
    }

    os_println("%d", var);

    return 0;
}

static int
gpio_setter(char *name, char *value)
{
    hisi_gpio_t *gpio = gpio_getbyname(name);
    if (NULL==gpio) {
        os_println("bad gpio name:%s", name);

        return -1;
    }
    
    return gpio_write(gpio, atoi(value));
}

static cmd_op_t gpio_op = {
    .help   = gpio_help,
    .showall= gpio_showall,
    .getter = gpio_getter,
    .setter = gpio_setter,
};

static int
gpio_handle(int argc, char *argv[])
{
    return cmd_getsetter(argc, argv, &gpio_op);
}

static int
gpio_main(int argc, char *argv[])
{
    return os_call(hisi_gpio_init, hisi_gpio_fini, gpio_handle, argc, argv);
}

#endif /* IS_PRODUCT_LTEFI_MD */

