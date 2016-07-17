/*******************************************************************************
Copyright (c) 2016-2018, Supper Wali Technology. All rights reserved.
*******************************************************************************/
#if IS_PRODUCT_LTEFI_MD
static int
cmd_clock_show(int argc, char *argv[])
{
#define CLOCK_DATASIZE  7
#define CLOCK_I2CNUM    1
#define CLOCK_DEVADDR   0xd0
#define CLOCK_REGADDR   0x00
    int err;

    byte sec = 0, sec_data = 0;
    byte min = 0, min_data = 0;
    byte hour = 0, hour_data = 0;
    byte day = 0, day_data = 0;
    byte date = 0, date_data = 0;
    byte month = 0, month_data = 0;
    byte year = 0, year_data = 0;
    byte mode, am_pm = 0;
    
    byte *week[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    byte data[CLOCK_DATASIZE];

    err = hisi_i2c_read(CLOCK_I2CNUM, CLOCK_DEVADDR, CLOCK_REGADDR, 1, data, CLOCK_DATASIZE);
    if (err<0) {
        return err;
    }

    sec_data = data[0];
    min_data = data[1];
    hour_data = data[2];
    day = data[3];
    date_data = data[4];
    month_data = data[5];
    year_data = data[6];

    sec = ((sec_data >> 4) & 0x7) * 10 + (sec_data & 0xf);
    min = ((min_data >> 4) & 0x7) * 10 + (min_data & 0xf);
    
    mode = (hour_data >> 6) & 0x1;
    
    if (mode) {//12-hour mode 
        hour = ((hour_data >> 4) & 0x01) * 10 + (hour_data & 0xf); 
        am_pm = (hour_data >> 5) & 0x1;
    }
    else {//24-hour mode
        hour = ((hour_data >> 4) & 0x03) * 10 + (hour_data & 0xf); 
    }
    date = ((date_data >> 4) & 0x3) * 10 + (date_data & 0xf);
    month = ((month_data >> 4) & 0x01) * 10 + (month_data & 0xf);
    year = ((year_data >> 4) & 0xf) * 10 + (year_data & 0xf);

    if (mode) 
        os_println("%04d-%02d-%02d %s, %02d:%02d:%02d %s",
                    2000+year, month, date, week[day%7], 
                    ((0 == am_pm) && (12 == hour) ? 0 : hour), 
                    min, sec, 
                    (am_pm == 1) ? "PM" : "AM");
    else 
        os_println("%04d-%02d-%02d %s, %02d:%02d:%02d",
                    2000+year, month, date, week[day%7], hour, min, sec);

    return 0;
}

static int
clock_usage(void)
{
#define USAGE_MODULE    __THIS_APPNAME " clock"
    os_eprintln(USAGE_MODULE "show");
#undef USAGE_MODULE

    return -EFORMAT;
}

static cmd_table_t cmd_clock[] = {
    {
        .argc   = 1,
        .argv   = {"show"},
        .handle = cmd_clock_show,
    },
};
#endif /* IS_PRODUCT_LTEFI_MD */

