/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
STATIC INLINE bool
__is_good_macstring_L(char *macstring)
{
    /*
    * long  macstring is "XX:XX:XX:XX:XX:XX" or "XX-XX-XX-XX-XX-XX"
    */
    int ifs = macstring[2];
    
    return (':'==ifs || '-'==ifs)
        && macstring[5] ==ifs
        && macstring[8] ==ifs
        && macstring[11]==ifs
        && macstring[14]==ifs
        && __is_good_macchar(macstring[0])
        && __is_good_macchar(macstring[1])
        /* macstring[2] is ifs */
        && __is_good_macchar(macstring[3])
        && __is_good_macchar(macstring[4])
        /* macstring[5] is ifs */
        && __is_good_macchar(macstring[6])
        && __is_good_macchar(macstring[7])
        /* macstring[8] is ifs */
        && __is_good_macchar(macstring[9])
        && __is_good_macchar(macstring[10])
        /* macstring[11] is ifs */
        && __is_good_macchar(macstring[12])
        && __is_good_macchar(macstring[13])
        /* macstring[14] is ifs */
        && __is_good_macchar(macstring[15])
        && __is_good_macchar(macstring[16]);
}

STATIC INLINE bool
__is_good_macstring_M(char *macstring)
{
    /*
    * middle  macstring is "XXXX:XXXX:XXXX" or "XXXX-XXXX-XXXX"
    */
    int ifs = macstring[4];
    
    return (':'==ifs || '-'==ifs)
        && macstring[9] ==ifs
        && __is_good_macchar(macstring[0])
        && __is_good_macchar(macstring[1])
        && __is_good_macchar(macstring[2])
        && __is_good_macchar(macstring[3])
        /* macstring[4] is ifs */
        && __is_good_macchar(macstring[5])
        && __is_good_macchar(macstring[6])
        && __is_good_macchar(macstring[7])
        && __is_good_macchar(macstring[8])
        /* macstring[9] is ifs */
        && __is_good_macchar(macstring[10])
        && __is_good_macchar(macstring[11])
        && __is_good_macchar(macstring[12])
        && __is_good_macchar(macstring[13]);
}

STATIC INLINE bool
__is_good_macstring_S(char *macstring)
{
    /*
    * short macstring is "XXXXXXXXXXXX"
    */
    return __is_good_macchar(macstring[0])
        && __is_good_macchar(macstring[1])
        && __is_good_macchar(macstring[2])
        && __is_good_macchar(macstring[3])
        && __is_good_macchar(macstring[4])
        && __is_good_macchar(macstring[5])
        && __is_good_macchar(macstring[6])
        && __is_good_macchar(macstring[7])
        && __is_good_macchar(macstring[8])
        && __is_good_macchar(macstring[9])
        && __is_good_macchar(macstring[10])
        && __is_good_macchar(macstring[11]);

}

bool
__is_good_macstring(char *macstring)
{
    switch(os_strlen(macstring)) {
        case MACSTRINGLEN_L: return __is_good_macstring_L(macstring);
        case MACSTRINGLEN_M: return __is_good_macstring_M(macstring);
        case MACSTRINGLEN_S: return __is_good_macstring_S(macstring);
        default: return false;
    }
}

STATIC INLINE byte *
__os_getmac_bystring_L(byte mac[], char macstring[])
{
    int i;
    
    for (i=0; i<OS_MACSIZE; i++) {
        mac[i] = os_hex2number(macstring + 3*i, 2, 16, int);
    }

    return mac;
}

STATIC INLINE byte *
__os_getmac_bystring_M(byte mac[], char macstring[])
{
    int i;
    
    for (i=0; i<OS_MACSIZE/2; i++) {
        mac[i]  = os_hex2number(macstring + 5*i,     2, 16, int);
        mac[i+1]= os_hex2number(macstring + 5*i + 2, 2, 16, int);
    }

    return mac;
}

STATIC INLINE byte *
__os_getmac_bystring_S(byte mac[], char macstring[])
{
    int i;
    
    for (i=0; i<OS_MACSIZE; i++) {
        mac[i] = os_hex2number(macstring + 2*i, 2, 16, int);
    }

    return mac;
}

byte *
__os_getmac_bystring(byte mac[], char macstring[])
{
    switch(os_strlen(macstring)) {
        case MACSTRINGLEN_L: return __os_getmac_bystring_L(mac, macstring);
        case MACSTRINGLEN_M: return __os_getmac_bystring_M(mac, macstring);
        case MACSTRINGLEN_S: return __os_getmac_bystring_S(mac, macstring);
        default: return OS_2BMAC;
    }
}

byte *
os_getmac_bystring(byte mac[], char macstring[])
{
    byte tmp[6];
    
    if (__is_good_macstring(macstring) && 
        __os_getmac_bystring(tmp, macstring) &&
        is_good_mac(tmp)) {
        return os_maccpy(mac, tmp);
    } else {
        return NULL;
    }
}

STATIC INLINE int
__os_macsnprintf_L(byte mac[], char macstring[], int size, int sep)
{
    return os_snprintf(macstring, size,
        "%.2x"  "%c"
        "%.2x"  "%c"
        "%.2x"  "%c"
        "%.2x"  "%c"
        "%.2x"  "%c"
        "%.2x",
        mac[0], sep,
        mac[1], sep,
        mac[2], sep,
        mac[3], sep,
        mac[4], sep,
        mac[5]);
}

STATIC INLINE int
__os_macsnprintf_M(byte mac[], char macstring[], int size, int sep)
{
    return os_snprintf(macstring, size,
        "%.2x%.2x"  "%c"
        "%.2x%.2x"  "%c"
        "%.2x%.2x",
        mac[0], mac[1], sep,
        mac[2], mac[3], sep,
        mac[4], mac[5]);
}

STATIC INLINE int
__os_macsnprintf_S(byte mac[], char macstring[], int size, int sep)
{
    return os_snprintf(macstring, size,
        "%.2x%.2x"
        "%.2x%.2x"
        "%.2x%.2x",
        mac[0], mac[1],
        mac[2], mac[3],
        mac[4], mac[5]);
}

int
os_macsnprintf(byte mac[], char macstring[], int size, int type, int sep)
{
    switch(type) {
        case MACSTRINGLEN_L: return __os_macsnprintf_L(mac, macstring, size, sep);
        case MACSTRINGLEN_M: return __os_macsnprintf_M(mac, macstring, size, sep);
        case MACSTRINGLEN_S: default: return __os_macsnprintf_S(mac, macstring, size, sep);
    }
}

#ifdef __APP__
char *
os_getmacby(char *script)
{
    static char line[1+OS_LINE_LEN];

    if (0==line[0]) {
        int err = os_pgets(line, OS_LINE_LEN, script);
        if (err) {
            return NULL;
        }
        else if (false==is_good_macstring(line)) {
            return NULL;
        }
    }

    return line;
}
#endif

/******************************************************************************/
