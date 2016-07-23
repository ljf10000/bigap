#ifndef __VOLTAGE_H_6f0fb08e77004591a90671891af4fc22__
#define __VOLTAGE_H_6f0fb08e77004591a90671891af4fc22__
/******************************************************************************/
#if defined(__APP__) && IS_PRODUCT_LTEFI_MD
/******************************************************************************/
#define VOLTAGE_DEV_ADDRESS         0xa4
#define VOLTAGE_I2C_NUMBER          1
#define VOLTAGE_REG_ADDRESS         0
#define VOLTAGE_REG_ADDRESS_COUNT   0

static inline int
__get_voltage(float *voltage)
{
    byte buf[2];
	int itmp, err;
	float ftmp;
    
    /* Read data from Device */
    err = hisi_i2c_read(VOLTAGE_I2C_NUMBER,
                VOLTAGE_DEV_ADDRESS,
                VOLTAGE_REG_ADDRESS, 
                VOLTAGE_REG_ADDRESS_COUNT, 
                buf, 
                os_count_of(buf));
    if (err<0) {
        return err;
    }

	itmp = buf[0];
	itmp = (itmp << 8) + buf[1];
	itmp = (itmp >> 4);

	ftmp = itmp*13*3.3/256;

	if (voltage) {
	    *voltage = ftmp;
	}
	
    return 0;
}

static inline int
get_voltage(float *voltage)
{
    return os_call(hisi_i2c_init, hisi_i2c_fini, __get_voltage, voltage);
}
/******************************************************************************/
#ifndef VOLTAGE_LOW
#define VOLTAGE_LOW             2   // low offset
#endif

#ifndef VOLTAGE_HIGH
#define VOLTAGE_HIGH            2   // high offset
#endif

#ifndef VOLTAGE_STANTARD
#define VOLTAGE_STANTARD        24
#endif

#ifndef VOLTAGE_INTERVAL
#define VOLTAGE_INTERVAL        5 // second
#endif

#ifndef VOLTAGE_SCRIPT
#define VOLTAGE_SCRIPT          "/tmp/cb/voltage.cb"
#endif

#ifndef VOLTAGE_FORMAT
#define VOLTAGE_FORMAT          "%3.4f"
#endif
/******************************************************************************/
#endif
#endif /* __VOLTAGE_H_6f0fb08e77004591a90671891af4fc22__ */
