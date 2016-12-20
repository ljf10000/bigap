/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
ALWAYS_INLINE bool
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

ALWAYS_INLINE bool
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

ALWAYS_INLINE bool
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

DECLARE bool
__is_good_macstring(char *macstring)
{
    switch(os_strlen(macstring)) {
        case MACSTRINGLEN_L: return __is_good_macstring_L(macstring);
        case MACSTRINGLEN_M: return __is_good_macstring_M(macstring);
        case MACSTRINGLEN_S: return __is_good_macstring_S(macstring);
        default: return false;
    }
}

ALWAYS_INLINE byte *
__os_getmac_bystring_L(byte mac[], char macstring[])
{
    int i;
    
    for (i=0; i<OS_MACSIZE; i++) {
        mac[i] = os_hex2number(macstring + 3*i, 2, 16, int);
    }

    return mac;
}

ALWAYS_INLINE byte *
__os_getmac_bystring_M(byte mac[], char macstring[])
{
    int i;
    
    for (i=0; i<OS_MACSIZE/2; i++) {
        mac[i]  = os_hex2number(macstring + 5*i,     2, 16, int);
        mac[i+1]= os_hex2number(macstring + 5*i + 2, 2, 16, int);
    }

    return mac;
}

ALWAYS_INLINE byte *
__os_getmac_bystring_S(byte mac[], char macstring[])
{
    int i;
    
    for (i=0; i<OS_MACSIZE; i++) {
        mac[i] = os_hex2number(macstring + 2*i, 2, 16, int);
    }

    return mac;
}

DECLARE byte *
__os_getmac_bystring(byte mac[], char macstring[])
{
    switch(os_strlen(macstring)) {
        case MACSTRINGLEN_L: return __os_getmac_bystring_L(mac, macstring);
        case MACSTRINGLEN_M: return __os_getmac_bystring_M(mac, macstring);
        case MACSTRINGLEN_S: return __os_getmac_bystring_S(mac, macstring);
        default: return OS_2BMAC;
    }
}

DECLARE byte *
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

ALWAYS_INLINE int
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

ALWAYS_INLINE int
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

ALWAYS_INLINE int
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

DECLARE int
os_macsnprintf(byte mac[], char macstring[], int size, int type, int sep)
{
    switch(type) {
        case MACSTRINGLEN_L: return __os_macsnprintf_L(mac, macstring, size, sep);
        case MACSTRINGLEN_M: return __os_macsnprintf_M(mac, macstring, size, sep);
        case MACSTRINGLEN_S: default: return __os_macsnprintf_S(mac, macstring, size, sep);
    }
}

DECLARE char *
os_getmacstring(byte mac[], int type, int sep)
{
    static char macstring[1+MACSTRINGLEN_L] = {0};

    os_macsaprintf(mac, macstring, type, sep);

    return macstring;
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

bool
is_abstract_sockaddr(void *addr)
{
    sockaddr_un_t *uaddr = (sockaddr_un_t *)addr;

    if (AF_UNIX==uaddr->sun_family) {
        return 0==uaddr->sun_path[0] && uaddr->sun_path[1];
    } else {
        return false;
    }
}

void
set_abstract_path(sockaddr_un_t *addr, char *path)
{
    addr->sun_path[0] = 0;

    os_memcpy(get_abstract_path(addr), path, os_strlen(path));
}

void
abstract_path_vsprintf(sockaddr_un_t *addr, const char *fmt, va_list args)
{
    char path[1+OS_LINE_LEN] = {0};

    os_vsprintf(path, fmt, args);
    
    set_abstract_path(addr, path);
}

void
abstract_path_sprintf(sockaddr_un_t *addr, const char *fmt, ...)
{
    va_list args;

    va_start(args, (char *)fmt);
    abstract_path_vsprintf(addr, fmt, args);
    va_end(args);
}

sockaddr_len_t
os_sockaddr_len(sockaddr_t *addr)
{
    switch(addr->sa_family) {
        case AF_UNIX:
            if (is_abstract_sockaddr(addr)) {
                return get_abstract_sockaddr_len((sockaddr_un_t *)addr);
            } else {
                return sizeof(sockaddr_un_t);
            }
        case AF_INET:   return sizeof(sockaddr_in_t);
        case AF_PACKET: return sizeof(sockaddr_ll_t);
#if 0
        case AF_INET6:  return sizeof(struct sockaddr_in6);
#endif
        case AF_NETLINK:return sizeof(sockaddr_nl_t);
        default: return sizeof(sockaddr_t);
    }
}

#endif

/******************************************************************************/
