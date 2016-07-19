/*******************************************************************************
Copyright (c) 2016-2018, Supper Wali Technology. All rights reserved.
*******************************************************************************/
#if IS_PRODUCT_LTEFI_MD

#define CLOCK_I2CNUM    1
#define CLOCK_DEVADDR   0xd0
#define CLOCK_REGADDR   0x00

enum {
    CLOCK_SEC   = 0,
    CLOCK_MIN   = 1,
    CLOCK_HOUR  = 2,
    CLOCK_DAY   = 3,
    CLOCK_DATE  = 4,
    CLOCK_MONTH = 5,
    CLOCK_YEAR  = 6,

    CLOCK_END
};

#define GET_CLOCK_SEC(_clock)       (((_clock[CLOCK_SEC] >> 4) & 0x7) * 10 + (_clock[CLOCK_SEC] & 0xf))
#define GET_CLOCK_MIN(_clock)       (((_clock[CLOCK_MIN] >> 4) & 0x7) * 10 + (_clock[CLOCK_MIN] & 0xf))
#define GET_CLOCK_HOUR(_clock)      (((_clock[CLOCK_HOUR] >> 4) & 0x03) * 10 + (_clock[CLOCK_HOUR] & 0xf))
#define GET_CLOCK_DAY(_clock)       (_clock[CLOCK_DAY])
#define GET_CLOCK_DATE(_clock)      (((_clock[CLOCK_DATE] >> 4) & 0x3) * 10 + (_clock[CLOCK_DATE] & 0xf))
#define GET_CLOCK_MONTH(_clock)     (((_clock[CLOCK_MONTH] >> 4) & 0x01) * 10 + (_clock[CLOCK_MONTH] & 0xf))
#define GET_CLOCK_YEAR(_clock)      (((_clock[CLOCK_YEAR] >> 4) & 0xf) * 10 + (_clock[CLOCK_YEAR] & 0xf))

#define GET_CLOCK_MODE(_clock)      ((_clock[CLOCK_HOUR] >> 6) & 0x01)
#define GET_CLOCK_MODE12(_clock)    GET_CLOCK_MODE(_clock)
#define GET_CLOCK_MODE24(_clock)    (!(GET_CLOCK_MODE12(_clock)))

#define GET_CLOCK_AM(_clock)        ((_clock[CLOCK_HOUR] >> 5) & 0x01)
#define GET_CLOCK_HOUR12(_clock)    (((_clock[CLOCK_HOUR] >> 4) & 0x01) * 10 + (_clock[CLOCK_HOUR] & 0xf))
#define GET_CLOCK_HOUR24(_clock)    GET_CLOCK_HOUR(_clock)


#define SET_CLOCK_SEC(_clock, _sec)     do{ \
    _clock[CLOCK_SEC] = (((_sec / 10) & 0x07) << 4) | (_sec - (_sec / 10) * 10); \
}while(0)

#define SET_CLOCK_MIN(_clock, _min)     do{ \
    _clock[CLOCK_MIN] = (((_min / 10) & 0x07) << 4) | (_min - (_min / 10) * 10); \
}while(0)

#define SET_CLOCK_HOUR(_clock, _hour)   do{ \
    if ((_hour/10) >= 2) {                                  \
        _clock[CLOCK_HOUR] = 0x20 | ((_hour - 20) & 0x0f);  \
    } else if ((_hour/10) >= 1) {                           \
        _clock[CLOCK_HOUR] = 0x10 | ((_hour - 10) & 0x0f);  \
    } else {                                                \
        _clock[CLOCK_HOUR] = 0x00 | ((_hour) & 0x0f);       \
    }                                                       \
                                                            \
    _clock[CLOCK_HOUR] = _clock[CLOCK_HOUR] & 0xBF;         \
}while(0)

#define SET_CLOCK_DAY(_clock, _day)     do{ \
    _clock[CLOCK_DAY] = _day;               \
}while(0)

#define SET_CLOCK_DATE(_clock, _date)   do{ \
    _clock[CLOCK_DATE] = (((_date / 10) & 0x03) << 4) | (_date - (_date / 10) * 10); \
}while(0)

#define SET_CLOCK_MONTH(_clock, _month) do{ \
    _clock[CLOCK_MONTH] = (((_month / 10) & 0x01) << 4) | (_month - (_month / 10) * 10); \
}while(0)

#define SET_CLOCK_YEAR(_clock, _year)   do{ \
    _clock[CLOCK_YEAR] = _year - (2000-1900); \
    _clock[CLOCK_YEAR] = (((_clock[CLOCK_YEAR] / 10) & 0x0F) << 4) | (_clock[CLOCK_YEAR] - (_clock[CLOCK_YEAR] / 10) * 10); \
}while(0)

static void
get_clock_auto(byte data[])
{
	time_t timep = 0;    
	struct tm *p;

	time(&timep);
	p = localtime(&timep);

	p->tm_mon += 1;
	
	SET_CLOCK_SEC(data, p->tm_sec);
	SET_CLOCK_MIN(data, p->tm_min);
	SET_CLOCK_HOUR(data, p->tm_hour);
	SET_CLOCK_DAY(data, p->tm_wday);
	SET_CLOCK_DATE(data, p->tm_mday);
	SET_CLOCK_MONTH(data, p->tm_mon);
	SET_CLOCK_YEAR(data, p->tm_year);
}

static void
get_clock_manual(byte data[])
{
	int	year = 0;
	byte 	month = 0, date = 0, mode = 0, pm = 0,
			hour = 0,  minute = 0, second = 0;

	int err;

	while (1) {
		os_printf("Please input mode(12/24):");
		scanf("%c", &mode);
		
		if (12 != mode && 24 != mode) {
			os_println("Wrong mode!");
			continue;
		}
		
		break;
	}
	
	while (1) {
		os_printf("Please input date(xxxx-xx-xx):");
		err = scanf("%hd-%hhd-%hhd", &year, &month, &date);

		if (err < 3 || (month < 0 || month > 12) || (date < 0 || date > 31)) {
			os_println("Wrong format");
			continue ;
		}
		
		break;
	}

	while(1) {
		os_printf("Please input day(1-7):");
		scanf("%c", &data[CLOCK_DAY]);

		if (data[CLOCK_DAY] < 1 || data[CLOCK_DAY] > 7) {
			os_println("Wrong format");
			continue;
		}
		
		break;
	}

	while(1) {
		os_printf("Please input time(xx:xx:xx):");
		err = scanf("%hhd:%hhd:%hhd", &hour, &minute, &second);
		if (err < 3 || (minute < 0 || minute > 59) || (second < 0 || second > 59)) {
			os_println("Wrong format");
			continue;
		}
		
		if (24 == mode) {
			if ((hour < 0 || hour > 24) ) {
				os_println("Wrong format");
				
				continue;
			}
		 } else { 
			if ((hour < 0 || hour > 12) ) {
				os_println("Wrong format");
				
				continue;
			}
		 }
		 
		 break;
	}
		
	if (12 == mode) {
		while (1) {
			os_printf("Choose 0(AM)/1(PM)):");
			scanf("%hhd", &pm);
			if (0 != pm && 1 != pm) {
				os_println("Wrong format");
				continue;
			}
			
			break;
		}
	}

	data[CLOCK_YEAR] = (year % 10) + ((year % 100) / 10 ) * 0x10;
	data[CLOCK_MONTH] = (month % 10) + (month / 10) * 0x10;
	data[CLOCK_DATE] = (date % 10) + (date / 10) * 0x10;
	data[CLOCK_HOUR] = (hour % 10) + (hour / 10) * 0x10;
	if (12 == mode)
		data[CLOCK_HOUR] = data[CLOCK_HOUR] | (1 << 6) | (pm << 5);
	data[CLOCK_MIN] = (minute % 10) + (minute / 10) * 0x10;
	data[CLOCK_SEC] = (second % 10) + (second / 10) * 0x10;
}

static int 
clock2date(byte data[CLOCK_END])
{
    return os_p_system("date -s '%04d-%02d-%02d %02d:%02d:%02d'", 
            GET_CLOCK_YEAR(data) + 2000,
            GET_CLOCK_MONTH(data), 
            GET_CLOCK_DATE(data),
        	GET_CLOCK_HOUR(data), 
        	GET_CLOCK_MIN(data), 
        	GET_CLOCK_SEC(data));
}

static int
clock_save(void (*get)(byte *))
{
    byte data[CLOCK_END];
    int err;
    
    (*get)(data);

    return hisi_i2c_write(CLOCK_I2CNUM, 
                CLOCK_DEVADDR,
	            CLOCK_REGADDR, 
	            CLOCK_I2CNUM, 
	            data, 
	            os_count_of(data));
}

static int
cmd_clock_save_auto(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    return clock_save(get_clock_auto);
}

static int
cmd_clock_save_manual(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    return clock_save(get_clock_manual);
}

static int
cmd_clock_sync(int argc, char *argv[])
{
    byte data[CLOCK_END];
    int err;
    
    (void)argc;
    (void)argv;
    
    err = hisi_i2c_read(CLOCK_I2CNUM, 
                CLOCK_DEVADDR,
	            CLOCK_REGADDR, 
	            CLOCK_I2CNUM, 
	            data, 
	            os_count_of(data));
    if (err<0) {
        return err;
    }

    return clock2date(data);
}

static int
cmd_clock_show(int argc, char *argv[])
{
    char *week[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    byte data[CLOCK_END];
    int err;

    (void)argc;
    (void)argv;

    err = hisi_i2c_read(CLOCK_I2CNUM, CLOCK_DEVADDR, CLOCK_REGADDR, 1, data, os_count_of(data));
    if (err<0) {
        return err;
    }

    if (GET_CLOCK_MODE12(data)) {
        byte hour = GET_CLOCK_HOUR12(data);
        byte am = GET_CLOCK_AM(data);
        
        os_println("%04d-%02d-%02d %s, %02d:%02d:%02d %s",
                    2000+GET_CLOCK_YEAR(data), 
                    GET_CLOCK_MONTH(data), 
                    GET_CLOCK_DATE(data), 
                    week[GET_CLOCK_DAY(data)%7], 
                    (am && 12 == hour) ? 0 : hour, 
                    GET_CLOCK_MIN(data), 
                    GET_CLOCK_SEC(data),
                    am?"AM":"PM");
    } else {
        os_println("%04d-%02d-%02d %s, %02d:%02d:%02d",
                    2000+GET_CLOCK_YEAR(data),
                    GET_CLOCK_MONTH(data), 
                    GET_CLOCK_DATE(data), 
                    week[GET_CLOCK_DAY(data)%7], 
                    GET_CLOCK_HOUR24(data), 
                    GET_CLOCK_MIN(data), 
                    GET_CLOCK_SEC(data));
    }
    
    return 0;
}

static int
clock_usage(void)
{
#define USAGE_MODULE    __THIS_APPNAME " clock"
    os_eprintln(USAGE_MODULE "show");
    os_eprintln(USAGE_MODULE "save auto");
    os_eprintln(USAGE_MODULE "save");
    os_eprintln(USAGE_MODULE "sync");
#undef USAGE_MODULE

    return -EFORMAT;
}

static cmd_table_t cmd_clock[] = {
    CMD_TABLE_ENTRY(cmd_clock_show, 1, "show"),
    CMD_TABLE_ENTRY(cmd_clock_save_auto, 2, "save", "auto"),
    CMD_TABLE_ENTRY(cmd_clock_save_manual, 1, "save"),
    CMD_TABLE_ENTRY(cmd_clock_sync, 1, "sync"),
};

static int
clock_main(int argc, char *argv[])
{
    return os_call(hisi_i2c_init, hisi_i2c_fini, cmd_handle, cmd_clock, argc, argv, clock_usage);
}
#endif /* IS_PRODUCT_LTEFI_MD */

