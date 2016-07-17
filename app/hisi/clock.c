/*******************************************************************************
Copyright (c) 2016-2018, Supper Wali Technology. All rights reserved.
*******************************************************************************/
#if IS_PRODUCT_LTEFI_MD

#define CLOCK_DATASIZE  7
#define CLOCK_I2CNUM    1
#define CLOCK_DEVADDR   0xd0
#define CLOCK_REGADDR   0x00

static void
get_clock_auto(
    byte *pyear, 
    byte *pmonth, 
    byte *pdate, 
    byte *pday, 
    byte *phour, 
    byte *pminute, 
    byte *psecond)
{
	time_t timep = 0;    
	struct tm *p;

	time(&timep);
	p = localtime(&timep);

	p->tm_mon += 1;
	*psecond= (((p->tm_sec / 10) & 0x07) << 4) | (p->tm_sec - (p->tm_sec / 10) * 10); 

	*pminute= (((p->tm_min / 10) & 0x07) << 4) | (p->tm_min - (p->tm_min / 10) * 10);  
	if ((p->tm_hour/10) >= 2) {        
		*phour = 0x20 | ((p->tm_hour - 20) & 0x0f);
	} else if ((p->tm_hour/10) >= 1) {        
		*phour = 0x10 | ((p->tm_hour - 10) & 0x0f);
	} else    {       
		*phour = 0x00 | ((p->tm_hour) & 0x0f);
	}    
	*phour = *phour & 0xBF;  
	*pday = p->tm_wday;    
	*pdate = (((p->tm_mday / 10) & 0x03) << 4) | (p->tm_mday - (p->tm_mday / 10) * 10);   
	*pmonth= (((p->tm_mon / 10) & 0x01) << 4) | (p->tm_mon - (p->tm_mon / 10) * 10);    
	*pyear= p->tm_year - (2000-1900);
	*pyear = (((*pyear / 10) & 0x0F) << 4) | (*pyear - (*pyear / 10) * 10);
}

static void
get_clock_manual(
    byte *pyear, 
    byte *pmonth, 
    byte *pdate, 
    byte *pday, 
    byte *phour, 
    byte *pminute, 
    byte *psecond)
{

	HI_S16	year = 0;
	HI_S8 	month = 0,date = 0, mode = 0, am_pm = 0,
			hour = 0,  minute = 0, second = 0;

	int err;

	while (1) {
		os_printf("Please input mode(12/24):");
		scanf("%hhd", &mode);
		
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
		scanf("%hhd", pday);

		if (*pday < 1 || *pday > 7) {
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
			scanf("%hhd", &am_pm);
			if (0 != am_pm && 1 != am_pm) {
				os_println("Wrong format");
				continue;
			}
			
			break;
		}
	}

	*pyear = (year % 10) + ((year % 100) / 10 ) * 0x10; 
	*pmonth = (month % 10) + (month / 10) * 0x10;
	*pdate = (date % 10) + (date / 10) * 0x10;
	*phour = (hour % 10) + (hour / 10) * 0x10;
	if (12 == mode)
		*phour = *phour | (1 << 6) | (am_pm << 5);
	*pminute = (minute % 10) + (minute / 10) * 0x10;
	*psecond = (second % 10) + (second / 10) * 0x10;
}

static void 
sync_clock(byte data[CLOCK_DATASIZE])
{   
	byte sec_data = 0, sec = 0, min_data = 0, min = 0, hour_data = 0, hour = 0;
	byte day = 0, date_data = 0, date = 0, month_data = 0, month = 0, year_data = 0, year = 0;
	
	sec_data = data[0];
	min_data = data[1];
	hour_data = data[2];
	day = data[3];
	date_data = data[4];
	month_data = data[5];
	year_data = data[6];

	sec = ((sec_data >> 4) & 0x7) * 10 + (sec_data & 0xf);
	min = ((min_data >> 4) & 0x7) * 10 + (min_data & 0xf);
	hour = ((hour_data >> 4) & 0x03) * 10 + (hour_data & 0xf); 
	date = ((date_data >> 4) & 0x3) * 10 + (date_data & 0xf);
	month = ((month_data >> 4) & 0x01) * 10 + (month_data & 0xf);
	year = ((year_data >> 4) & 0xf) * 10 + (year_data & 0xf);

    return os_p_system("date -s '%04d-%02d-%02d %02d:%02d:%02d'", 
            year+2000, month, date,
        	hour, min, sec);
}

static int
cmd_clock_save_auto(int argc, char *argv[])
{
    byte data[CLOCK_DATASIZE];
    int err;
    
    get_clock_auto(&data[6],    // year
    		    &data[5],       // month
    		    &data[4],       // date
    		    &data[3],       // day
    		    &data[2],       // hour
    		    &data[1],       // minute
    		    &data[0]);      // second

    return hisi_i2c_write(CLOCK_I2CNUM, 
                CLOCK_DEVADDR,
	            CLOCK_REGADDR, 
	            CLOCK_I2CNUM, 
	            data, 
	            CLOCK_DATASIZE);
}

static int
cmd_clock_save_manual(int argc, char *argv[])
{

    byte data[CLOCK_DATASIZE];
    int err;
    
    get_clock_manual(&data[6],  // year
    		    &data[5],       // month
    		    &data[4],       // date
    		    &data[3],       // day
    		    &data[2],       // hour
    		    &data[1],       // minute
    		    &data[0]);      // second

    return hisi_i2c_write(CLOCK_I2CNUM, 
                CLOCK_DEVADDR,
	            CLOCK_REGADDR, 
	            CLOCK_I2CNUM, 
	            data, 
	            CLOCK_DATASIZE);
}

static int
cmd_clock_sync(int argc, char *argv[])
{
    byte data[CLOCK_DATASIZE];
    int err;
    
    err = hisi_i2c_read(CLOCK_I2CNUM, 
                CLOCK_DEVADDR,
	            CLOCK_REGADDR, 
	            CLOCK_I2CNUM, 
	            data, 
	            CLOCK_DATASIZE);
    if (err<0) {
        return err;
    }

    return sync_clock(data);
}

static int
cmd_clock_show(int argc, char *argv[])
{
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

