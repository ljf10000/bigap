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
    _clock[CLOCK_YEAR] = (((_clock[CLOCK_YEAR] / 10) & 0x0F) << 4) | (_clock[CLOCK_YEAR] - (_clock[CLOCK_YEAR] / 10) * 10);
}while(0)

static void
get_clock_auto(byte clock[])
{
	time_t timep = 0;    
	struct tm *p;

	time(&timep);
	p = localtime(&timep);

	p->tm_mon += 1;
	
	SET_CLOCK_SEC(clock, p->tm_sec);
	SET_CLOCK_MIN(clock, p->tm_min);
	SET_CLOCK_HOUR(clock, p->tm_hour);
	SET_CLOCK_DAY(clock, p->tm_wday);
	SET_CLOCK_DATE(clock, p->tm_mday);
	SET_CLOCK_MONTH(clock, p->tm_mon);
	SET_CLOCK_YEAR(clock, p->tm_year);
}

static void
get_clock_manual(byte clock[])
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
		scanf("%c", &clock[CLOCK_DAY]);

		if (clock[CLOCK_DAY] < 1 || clock[CLOCK_DAY] > 7) {
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

	clock[CLOCK_YEAR] = (year % 10) + ((year % 100) / 10 ) * 0x10;
	clock[CLOCK_MONTH] = (month % 10) + (month / 10) * 0x10;
	clock[CLOCK_DATE] = (date % 10) + (date / 10) * 0x10;
	clock[CLOCK_HOUR] = (hour % 10) + (hour / 10) * 0x10;
	if (12 == mode)
		clock[CLOCK_HOUR] = clock[CLOCK_HOUR] | (1 << 6) | (pm << 5);
	clock[CLOCK_MIN] = (minute % 10) + (minute / 10) * 0x10;
	clock[CLOCK_SEC] = (second % 10) + (second / 10) * 0x10;
}

static void 
clock2date(byte clock[CLOCK_END])
{
    return os_p_system("date -s '%04d-%02d-%02d %02d:%02d:%02d'", 
            GET_CLOCK_YEAR(clock) + 2000,
            GET_CLOCK_MONTH(clock), 
            GET_CLOCK_DATE(clock),
        	GET_CLOCK_HOUR(clock), 
        	GET_CLOCK_MIN(clock), 
        	GET_CLOCK_SEC(clock));
}

static int
clock_save(void (*get)(byte *))
{
    byte clock[CLOCK_END];
    int err;
    
    (*get)(clock);

    return hisi_i2c_write(CLOCK_I2CNUM, 
                CLOCK_DEVADDR,
	            CLOCK_REGADDR, 
	            CLOCK_I2CNUM, 
	            clock, 
	            os_count_of(clock));
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
    byte clock[CLOCK_END];
    int err;
    
    (void)argc;
    (void)argv;
    
    err = hisi_i2c_read(CLOCK_I2CNUM, 
                CLOCK_DEVADDR,
	            CLOCK_REGADDR, 
	            CLOCK_I2CNUM, 
	            clock, 
	            os_count_of(clock));
    if (err<0) {
        return err;
    }

    return clock2date(clock);
}

static int
cmd_clock_show(int argc, char *argv[])
{
    char *week[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    byte clock[CLOCK_END];
    int err;

    (void)argc;
    (void)argv;

    err = hisi_i2c_read(CLOCK_I2CNUM, CLOCK_DEVADDR, CLOCK_REGADDR, 1, clock, os_count_of(clock));
    if (err<0) {
        return err;
    }

    if (GET_CLOCK_MODE12(clock)) {
        byte hour = GET_CLOCK_HOUR12(clock);
        byte am = GET_CLOCK_AM(clock);
        
        os_println("%04d-%02d-%02d %s, %02d:%02d:%02d %s",
                    2000+GET_CLOCK_YEAR(clock), 
                    GET_CLOCK_MONTH(clock), 
                    GET_CLOCK_DATE(clock), 
                    week[GET_CLOCK_DAY(clock)%7], 
                    (am && 12 == hour) ? 0 : hour, 
                    GET_CLOCK_MIN(clock), 
                    GET_CLOCK_SEC(clock),
                    am?"AM":"PM");
    } else {
        os_println("%04d-%02d-%02d %s, %02d:%02d:%02d",
                    2000+GET_CLOCK_YEAR(clock),
                    GET_CLOCK_MONTH(clock), 
                    GET_CLOCK_DATE(clock), 
                    week[GET_CLOCK_DAY(clock)%7], 
                    GET_CLOCK_HOUR24(clock), 
                    GET_CLOCK_MIN(clock), 
                    GET_CLOCK_SEC(clock));
    }
    
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
    {
        .argc   = 2,
        .argv   = {"save", "auto"},
        .handle = cmd_clock_save_auto,
    },
    {
        .argc   = 1,
        .argv   = {"save"},
        .handle = cmd_clock_save_manual,
    },
    {
        .argc   = 1,
        .argv   = {"sync"},
        .handle = cmd_clock_sync,
    },
};
#endif /* IS_PRODUCT_LTEFI_MD */

