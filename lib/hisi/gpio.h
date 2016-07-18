#ifndef __GPIO_H_4b6ed674420e47bd924bf348f1c637ba__
#define __GPIO_H_4b6ed674420e47bd924bf348f1c637ba__
/******************************************************************************/
#ifdef __APP__
/******************************************************************************/
#if IS_PRODUCT_LTEFI_MD
#include "hi_unf_gpio.h"
#endif

static inline int hisi_gpio_init(void)   { return hisi_unf_call_0(GPIO, Init); }
static inline int hisi_gpio_fini(void)   { return hisi_unf_call_0(GPIO, DeInit); }

#define hisi_gpio_read_bit(gpio_number, gpio_high_volt_pointer) \
    hisi_unf_call_x(GPIO, ReadBit, gpio_number, gpio_high_volt_pointer)
#define hisi_gpio_write_bit(gpio_number, gpio_high_volt) \
    hisi_unf_call_x(GPIO, WriteBit, gpio_number, gpio_high_volt)

#define hisi_gpio_get_dir_bit(gpio_number, gpio_dir_pointer) \
    hisi_unf_call_x(GPIO, GetDirBit, gpio_number, gpio_dir_pointer)
#define hisi_gpio_set_dir_bit(gpio_number, gpio_dir) \
    hisi_unf_call_x(GPIO, SetDirBit, gpio_number, gpio_dir)

#define hisi_gpio_get_output_type(gpio_number, gpio_output_type_pointer) \
    hisi_unf_call_x(GPIO, GetOutputType, gpio_number, gpio_output_type_pointer)
#define hisi_gpio_set_output_type(gpio_number, gpio_output_type) \
    hisi_unf_call_x(GPIO, SetOutputType, gpio_number, gpio_output_type)

#define hisi_gpio_query_int(gpio_number_pointer, ms) \
    hisi_unf_call_x(GPIO, QueryInt, gpio_number_pointer, ms)
#define hisi_gpio_set_int_type(gpio_number, type) \
    hisi_unf_call_x(GPIO, SetIntType, gpio_number, type)
#define hisi_gpio_set_int_enable(gpio_number, enable) \
    hisi_unf_call_x(GPIO, SetIntEnable, gpio_number, enable)
/******************************************************************************/
#define HI_UNF_GPIO                 1

#define GPIO_INPUT                  1
#define GPIO_OUTPUT                 0

#define GPIO_R                      0x01
#define GPIO_W                      0x02
#define GPIO_RW                     (GPIO_R | GPIO_W)

#define GPIO_NAME_LEN               OS_IFNAME_LEN
#define GPIO_LINE_LEN               OS_LINE_LEN

#define GPIO_GROUP_MAX              7
#define GPIO_PIN_MAX                8
#define GPIO_MAX		            (GPIO_GROUP_MAX*GPIO_PIN_MAX)   //Hi3798MV  GPIOn<7
/******************************************************************************/
#define GPIO_DIR_REG(num)           (((num >= 0)&&(num <= 7))? 6:7)
#define GPIO_VAL_REG(num)           (((num >= 0)&&(num <= 7))? 2:3)

#define GPIO_I2C_NUM                1
#define GPIO_I2C_DEVADDR            0xe8
#define GPIO_I2C_REGADDR_COUNT      1

/*
* i2c-gpio extander using tca1116pwr, compatible pca953x tca6416 16bit
*/
static inline int
__tca1116_read(uint32 reg_addr, byte *data)
{
	return hisi_i2c_read(GPIO_I2C_NUM, 
	            GPIO_I2C_DEVADDR,
			    reg_addr, 
			    GPIO_I2C_REGADDR_COUNT, 
			    data, 
			    1);
}

static inline int
__tca1116_write(uint32 reg_addr, byte *data)
{
	return hisi_i2c_write(GPIO_I2C_NUM, 
	            GPIO_I2C_DEVADDR,
			    reg_addr, 
			    GPIO_I2C_REGADDR_COUNT, 
			    data, 
			    1);
}

static inline int
tca1116_read(uint32 reg_addr, uint32 gpio_num, int *val)
{
    int err;
	byte b = 0;
    
	err = __tca1116_read(reg_addr, &b);
	if (err<0) {
        return err;
	}
	
	*val = ((int)b) & (0x1 << (gpio_num%8));

    return 0;
}

static inline int
tca1116_write(uint32 reg_addr, uint32 gpio_num, int val)
{
    int err;
	byte b = 0;
    
	err = __tca1116_read(reg_addr, &b);
	if (err<0) {
        return err;
	}

	int tmp = b;
	if (0 == val) {
		b = tmp & (~(0x1 << (gpio_num%10)));
	} else if (1 == val) {
		b = tmp | (0x1 << (gpio_num%10));
	} else {
		return -1;
	}
	
	return __tca1116_write(reg_addr, &b);
}

static inline int
gpio_tca1116_read(unsigned int gpio_num, int *value)
{
	int err, val = 0;

	/* SET Direction */
	err = tca1116_write(GPIO_DIR_REG(gpio_num), gpio_num, GPIO_INPUT);
	if (err<0) {
        return 0;
	}

	/* GET Value */
	err = tca1116_read(GPIO_VAL_REG(gpio_num), gpio_num, &val);
	if (err<0) {
        return 0;
	}
	
	*value = val;
	
    return 0;
}

static inline int
gpio_tca1116_write(unsigned int gpio_num, int value)
{
    int err;
    
	/* SET Direction */
	err = tca1116_write(GPIO_DIR_REG(gpio_num), gpio_num, GPIO_OUTPUT);
	if (err<0) {
        return 0;
	}

	/* GET & SET Value */
	err = tca1116_write(GPIO_VAL_REG(gpio_num), gpio_num, value);
	if (err<0) {
        return 0;
	}

    return 0;
}


static inline int 
__gpio_read(uint32 gpio_num, int *pvalue)
{
    bool val;
    int err;

	if (gpio_num > GPIO_MAX) {
		return gpio_tca1116_read(gpio_num - GPIO_MAX, pvalue);
	}

    //设置成输入
    err = hisi_gpio_set_dir_bit(gpio_num, GPIO_INPUT);
    if (err<0) {
        return err;
    }

    err = hisi_gpio_read_bit(gpio_num, &val);
    if (err<0) {
        return err;
    }
    
    *pvalue = val;

    return 0;
}

static inline int
__gpio_write(uint32 gpio_num, int value)
{
    int err;
    
	if (gpio_num > GPIO_MAX) {
		return gpio_tca1116_write(gpio_num - GPIO_MAX, value);
	}

    //设置成输出
    err = hisi_gpio_set_dir_bit(gpio_num, GPIO_OUTPUT);
    if (err<0) {
        return err;
    }

    err = hisi_gpio_write_bit(gpio_num, !!value);
    if (err<0) {
        return err;
    }

    return 0;
}

static inline int
gpio_read(struct gpio *gpio)
{
    int err = 0;
    int value = 0;
    
    if (GPIO_R != (GPIO_R & gpio->flag)) {
        os_println("no support read gpio %s", gpio->name);

        return -1;
    }
    
    err = __gpio_read(gpio->number, &value);
    if (err) {
        os_println("read gpio %s error(%d)", gpio->name, err);

        return -1;
    }
    
    return !!value;
}

static inline int
gpio_write(struct gpio *gpio, int value)
{
    int err = 0;

    if (GPIO_W != (GPIO_W & gpio->flag)) {
        os_println("no support write gpio %s", gpio->name);

        return -1;
    }

    err = __gpio_write(gpio->number, !!value);
    if (err) {
        os_println("write gpio %s error(%d)", gpio->name, err);

        return -1;
    }

    return 0;
}
/******************************************************************************/
#define GPIO_NUMBER(group_number, pin_number)    (GPIO_PIN_MAX*(group_number) + (pin_number))
#define GPIO_NUMBER_MON_VCC         GPIO_NUMBER(0, 3)
#define GPIO_NUMBER_SHUT_FORCE		GPIO_NUMBER(5, 2)
#define GPIO_NUMBER_SHUT_DELAY		GPIO_NUMBER(5, 6)
#define GPIO_NUMBER_MON_ACC			GPIO_NUMBER(5, 5)
#define GPIO_NUMBER_RST_LTE1		GPIO_NUMBER(6, 1)
#define GPIO_NUMBER_RST_LTE2		GPIO_NUMBER(6, 2)
#define GPIO_NUMBER_RST_AP			GPIO_NUMBER(6, 3)
#define GPIO_NUMBER_MON_RST			GPIO_NUMBER(6, 4)
#define GPIO_NUMBER_PWR_LTE2		GPIO_NUMBER(6, 5)
#define GPIO_NUMBER_PWR_AP			GPIO_NUMBER(6, 6)
#define GPIO_NUMBER_PWR_LTE1		GPIO_NUMBER(6, 7)

#define GPIO_NAME_PWR_LTE1		    "pwr_lte1"
#define GPIO_NAME_PWR_LTE2		    "pwr_lte2"
#define GPIO_NAME_RST_LTE1		    "rst_lte1"
#define GPIO_NAME_RST_LTE2		    "rst_lte2"
#define GPIO_NAME_SHUT_DELAY	    "offdelay"
#define GPIO_NAME_SHUT_FORCE	    "forceshut"
#define GPIO_NAME_PWR_AP		    "apshut"
#define GPIO_NAME_RST_AP		    "rst_per"
#define GPIO_NAME_MON_RST		    "reset"
#define GPIO_NAME_MON_ACC		    "acc_mon"
#define GPIO_NAME_MON_VCC		    "vcc_mon"

#define I2C_GPIO_BASE				GPIO_MAX
#define I2C_GPIO_NUMBER_LED_SIM1	(I2C_GPIO_BASE + 4)
#define I2C_GPIO_NUMBER_LED_STAT1	(I2C_GPIO_BASE + 5)
#define I2C_GPIO_NUMBER_LED_STAT2	(I2C_GPIO_BASE + 6)
#define I2C_GPIO_NUMBER_LED_SIM2	(I2C_GPIO_BASE + 7)
#define I2C_GPIO_NUMBER_LED_LTE2	(I2C_GPIO_BASE + 10)
#define I2C_GPIO_NUMBER_LED_LTE1	(I2C_GPIO_BASE + 11)
#define I2C_GPIO_NUMBER_LED_GPS		(I2C_GPIO_BASE + 12)
#define I2C_GPIO_NUMBER_LED_SATA	(I2C_GPIO_BASE + 13)
#define I2C_GPIO_NUMBER_LED_SYS		(I2C_GPIO_BASE + 14)
#define I2C_GPIO_NUMBER_LED_PWR		(I2C_GPIO_BASE + 15)
#define I2C_GPIO_NUMBER_LED_CTRL	(I2C_GPIO_BASE + 17)

#define GPIO_NAME_LED_STAT1		    "stat1"
#define GPIO_NAME_LED_STAT2		    "stat2"
#define GPIO_NAME_LED_SIM1		    "sim1"
#define GPIO_NAME_LED_SIM2		    "sim2"
#define GPIO_NAME_LED_LTE1		    "lte1"
#define GPIO_NAME_LED_LTE2		    "lte2"
#define GPIO_NAME_LED_GPS		    "gps"
#define GPIO_NAME_LED_SATA		    "hdd"
#define GPIO_NAME_LED_SYS		    "sys"
#define GPIO_NAME_LED_PWR		    "pwr"
#define GPIO_NAME_LED_CTRL		    "ctrl"

typedef struct gpio {
	char name[1 + GPIO_NAME_LEN];
	int number;
	int flag;
} hisi_gpio_t;

#define GPIO_INIT(_name, _number, _flag)  {.name=_name, .number=_number, .flag=_flag}
#define GPIO_INITER { \
	/* input */                                                             \
	GPIO_INIT(GPIO_NAME_MON_VCC,    GPIO_NUMBER_MON_VCC,    GPIO_R),        \
	GPIO_INIT(GPIO_NAME_MON_ACC,    GPIO_NUMBER_MON_ACC,    GPIO_R),        \
	GPIO_INIT(GPIO_NAME_MON_RST,    GPIO_NUMBER_MON_RST,    GPIO_R),        \
	/* output */                                                            \
	GPIO_INIT(GPIO_NAME_SHUT_FORCE, GPIO_NUMBER_SHUT_FORCE, GPIO_W),        \
	GPIO_INIT(GPIO_NAME_SHUT_DELAY, GPIO_NUMBER_SHUT_DELAY, GPIO_W),        \
	GPIO_INIT(GPIO_NAME_RST_LTE1,   GPIO_NUMBER_RST_LTE1,   GPIO_W),        \
	GPIO_INIT(GPIO_NAME_RST_LTE2,   GPIO_NUMBER_RST_LTE2,   GPIO_W),        \
	GPIO_INIT(GPIO_NAME_PWR_LTE1,   GPIO_NUMBER_PWR_LTE1,   GPIO_W),        \
	GPIO_INIT(GPIO_NAME_PWR_LTE2,   GPIO_NUMBER_PWR_LTE2,   GPIO_W),        \
	GPIO_INIT(GPIO_NAME_RST_AP,     GPIO_NUMBER_RST_AP,     GPIO_W),        \
	GPIO_INIT(GPIO_NAME_PWR_AP,     GPIO_NUMBER_PWR_AP,     GPIO_W),        \
	/* i2c-gpio */                                                          \
	GPIO_INIT(GPIO_NAME_LED_SIM1,   I2C_GPIO_NUMBER_LED_SIM1,   GPIO_W),    \
	GPIO_INIT(GPIO_NAME_LED_SIM2,   I2C_GPIO_NUMBER_LED_SIM2,   GPIO_W),    \
	GPIO_INIT(GPIO_NAME_LED_STAT1,  I2C_GPIO_NUMBER_LED_STAT1,  GPIO_W),    \
	GPIO_INIT(GPIO_NAME_LED_STAT2,  I2C_GPIO_NUMBER_LED_STAT2,  GPIO_W),    \
	GPIO_INIT(GPIO_NAME_LED_LTE1,   I2C_GPIO_NUMBER_LED_LTE1,   GPIO_W),    \
	GPIO_INIT(GPIO_NAME_LED_LTE2,   I2C_GPIO_NUMBER_LED_LTE2,   GPIO_W),    \
	GPIO_INIT(GPIO_NAME_LED_GPS,    I2C_GPIO_NUMBER_LED_GPS,    GPIO_W),    \
	GPIO_INIT(GPIO_NAME_LED_SATA,   I2C_GPIO_NUMBER_LED_SATA,   GPIO_W),    \
	GPIO_INIT(GPIO_NAME_LED_SYS,    I2C_GPIO_NUMBER_LED_SYS,    GPIO_W),    \
	GPIO_INIT(GPIO_NAME_LED_PWR,    I2C_GPIO_NUMBER_LED_PWR,    GPIO_W),    \
	GPIO_INIT(GPIO_NAME_LED_CTRL,   I2C_GPIO_NUMBER_LED_CTRL,   GPIO_W),    \
}   /* end */

static inline hisi_gpio_t *
gpio_GPIO(int *count)
{
    static hisi_gpio_t array[] = GPIO_INITER;

    if (count) {
        *count = os_count_of(array);
    }
    
    return array;
}

static inline struct gpio *
gpio_getbyname(char *name)
{
    int i, count = 0, idx = 0;
    hisi_gpio_t *GPIO = gpio_GPIO(&count);
    struct gpio *gpio;
    
    for (i=0; i<count; i++) {
        gpio = &GPIO[i];
        
        if (os_streq(gpio->name, name)) {
            return gpio;
        }
    }

    return NULL;
}
/******************************************************************************/
#endif /* __APP__ */
#endif /* __GPIO_H_4b6ed674420e47bd924bf348f1c637ba__ */
