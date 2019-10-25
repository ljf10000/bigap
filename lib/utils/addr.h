#ifndef __ADDR_H_a60fcc799b2f44c38dcbf510eb97f0c6__
#define __ADDR_H_a60fcc799b2f44c38dcbf510eb97f0c6__
#ifdef __APP__
/******************************************************************************/
enum { INVALID_IPADDR = INADDR_NONE };

#define OS_FORMAT_IPSTRING      "255.255.255.255"
#define OS_IPSTRING_LEN         sizeof(OS_FORMAT_IPSTRING)
typedef char ip_string_t[OS_IPSTRING_LEN];

static inline char *
os_ipstring(uint32 ip, ip_string_t string)
{
    return inet_ntop(AF_INET, &ip, string, OS_IPSTRING_LEN);
}

static inline char *
unsafe_ipstring(uint32 ip)
{
    static ip_string_t string;

    return unsafe_ipstring(ip, string);
}


static inline uint32
os_ipaddr(char *ipstring)
{
    return inet_addr(ipstring);
}

static inline bool
is_good_ipaddr(uint32 ip)
{
    return INVALID_IPADDR != ip;
}

static inline bool
is_good_ipstring(char *ipstring)
{
    return is_good_str(ipstring) && is_good_ipaddr(os_ipaddr(ipstring));
}

static inline uint32
os_ipdns(char *ipstring_or_domain)
{
    uint32 ip = os_ipaddr(ipstring_or_domain);
    if (is_good_ipaddr(ip)) {
        return ip;
    } else {
        struct hostent *host = gethostbyname(ipstring_or_domain);

        return host?*(uint32 *)(host->h_addr):INVALID_IPADDR;
    }
}

#define OS_UNIX_PATH(_PATH)             "/tmp/." _PATH ".unix"
#define __ABSTRACT_PATH(_PATH)          __zero _PATH
#define OS_ABSTRACT_PATH(_PATH)         __ABSTRACT_PATH(OS_UNIX_PATH(_PATH))

#define OS_SOCKADDR_UNSPEC()    {   \
    .sa_family = AF_UNSPEC,         \
}   /* end */

#define OS_SOCKADDR_UNIX(_PATH) {   \
    .sun_family = AF_UNIX,          \
    .sun_path   = _PATH,            \
}   /* end */
#define OS_SOCKADDR_ABSTRACT(_PATH) OS_SOCKADDR_UNIX(OS_ABSTRACT_PATH(_PATH))

#define OS_SOCKADDR_INET(_ip, _port) { \
    .sin_family = AF_INET,          \
    .sin_port   = _port,            \
    .sin_addr   = {                 \
        .s_addr = _ip,              \
    }                               \
}   /* end */

#define OS_SOCKADDR_NETLINK(_pid, _groups) { \
    .nl_family  = AF_NETLINK,       \
    .nl_pid     = _pid,             \
    .nl_groups  = _groups,          \
}   /* end */

typedef struct sockaddr_un  sockaddr_un_t;
typedef struct sockaddr_in  sockaddr_in_t;
typedef struct sockaddr_ll  sockaddr_ll_t;
typedef struct sockaddr_nl  sockaddr_nl_t;
typedef struct sockaddr     sockaddr_t;
typedef socklen_t           sockaddr_len_t;

typedef union {
    sockaddr_un_t un;
    sockaddr_in_t in;
    sockaddr_ll_t ll;
    sockaddr_nl_t nl;
    sockaddr_t    c;
} os_sockaddr_t;

#define OS_SOCKADDR_INITER(_family)         {.c = {.sa_family = _family}}
#define OS_SOCKADDR_ZERO()                  OS_SOCKADDR_INITER(0)
#define OS_SOCKADDR_IN_INITER(_ip, _port)   {.in = OS_SOCKADDR_INET(_ip, _port)}
#define OS_SOCKADDR_UN_INITER(_path)        {.un = OS_SOCKADDR_UNIX(_path)}
#define OS_SOCKADDR_ABSTRACT_INITER(_name)  {.un = OS_SOCKADDR_ABSTRACT(_name)}

static inline bool
is_abstract_sockaddr(void *addr)
{
    sockaddr_un_t *uaddr = (sockaddr_un_t *)addr;

    if (AF_UNIX==uaddr->sun_family) {
        return 0==uaddr->sun_path[0] && uaddr->sun_path[1];
    } else {
        return false;
    }
}

static inline char *
get_abstract_path(sockaddr_un_t *addr)
{
    return &addr->sun_path[1];
}

static inline void
set_abstract_path(sockaddr_un_t *addr, char *path)
{
    addr->sun_path[0] = 0;

    os_memcpy(get_abstract_path(addr), path, os_strlen(path));
}

static inline void
abstract_path_vsprintf(sockaddr_un_t *addr, const char *fmt, va_list args)
{
    char path[1+OS_LINE_LEN] = {0};

    os_vsprintf(path, fmt, args);
    
    set_abstract_path(addr, path);
}

static inline void
abstract_path_sprintf(sockaddr_un_t *addr, const char *fmt, ...)
{
    va_list args;

    va_start(args, (char *)fmt);
    abstract_path_vsprintf(addr, fmt, args);
    va_end(args);
}


enum { unix_path_size = sizeof(sockaddr_un_t) - offsetof(sockaddr_un_t, sun_path) };
enum { abstract_path_size  = unix_path_size - 1 };

static inline void
set_abstract_sockaddr_len(sockaddr_un_t *addr, int addrlen)
{
    addr->sun_path[addrlen - sizeof(addr->sun_family)] = 0;
}

static inline sockaddr_len_t
get_abstract_sockaddr_len(sockaddr_un_t *addr)
{
    return sizeof(addr->sun_family) + 1 + os_strlen(get_abstract_path(addr));
}

static inline sockaddr_len_t
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


#ifndef SCRIPT_GETMAC
#define SCRIPT_GETMAC       PC_VAL( "ip link show eth0 | grep link | awk '{print $2}'", \
                                    "ifconfig | grep 'eth0 ' | awk '{print $5}'")
#endif

#ifndef SCRIPT_GETBASEMAC
#define SCRIPT_GETBASEMAC   PC_VAL(SCRIPT_GETMAC, "/usr/sbin/getbasemac")
#endif

/*
* todo: md1 or md3 handle
*/
static inline char *
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
/******************************************************************************/
#endif /* __APP__ */

/******************************************************************************/
enum {
    OS_OUISIZE      = 3,
    OS_MACSIZE      = 6,
    
    OS_IPSTRINGLEN  = sizeof("255.255.255.255") - 1,
};

enum {
    /*
    * short macstring is "XXXXXXXXXXXX"
    *
    * length: 12
    */
    MACSTRINGLEN_S  = (2*OS_MACSIZE),

    /*
    * middle  macstring is "XXXX:XXXX:XXXX" or "XXXX-XXXX-XXXX"
    *
    * length: 14
    */
    MACSTRINGLEN_M  = (MACSTRINGLEN_S + 2),

    /*
    * long  macstring is "XX:XX:XX:XX:XX:XX" or "XX-XX-XX-XX-XX-XX"
    *
    * length: 17
    */
    MACSTRINGLEN_L  = (MACSTRINGLEN_S + 5),
    
    MACSTRINGLEN  = MACSTRINGLEN_L,
};

#define OS_ZEROMAC_STRING   "00:00:00:00:00:00"
#define OS_FULLMAC_STRING   "ff:ff:ff:ff:ff:ff"
#define OS_2BMAC_STRING     "2b:2b:2b:2b:2b:2b"
#define OS_TEMPMAC_STRING   "00:00:01:00:00:01"

#define OS_ZEROMAC  ((byte *)"\x00\x00\x00\x00\x00\x00")
#define OS_FULLMAC  ((byte *)"\xff\xff\xff\xff\xff\xff")
#define OS_2BMAC    ((byte *)"\x2b\x2b\x2b\x2b\x2b\x2b")
#define OS_TEMPMAC  ((byte *)"\x00\x00\x01\x00\x00\x01")

static inline byte *
os_maccpy(byte dst[], byte src[])
{
    *(uint32 *)(dst+0) = *(uint32 *)(src+0);
    *(uint16 *)(dst+4) = *(uint16 *)(src+4);

    return dst;
}

static inline byte *
os_maczero(byte mac[])
{
    *(uint32 *)(mac+0) = 0;
    *(uint16 *)(mac+4) = 0;
    
    return mac;
}

static inline byte *
os_macfull(byte mac[])
{
    *(uint32 *)(mac+0) = 0xffffffff;
    *(uint16 *)(mac+4) = 0xffff;
    
    return mac;
}

static inline int
os_maccmp(byte a[], byte b[])
{
    return os_memcmp(a, b, OS_MACSIZE);
}

static inline bool
os_maceq(byte a[], byte b[])
{
    return *(uint32 *)(a+0) == *(uint32 *)(b+0)
        && *(uint16 *)(a+4) == *(uint16 *)(b+4);
}

static inline bkdr_t
os_macbkdr(byte mac[])
{
    return os_bin_bkdr(mac, OS_MACSIZE);
}

static inline bool
os_macmaskmach(byte a[], byte b[], byte mask[])
{
    return os_bmask_match(a, b, mask, OS_MACSIZE);
}

static inline bool
is_zero_mac(byte mac[])
{
    return *(uint32 *)(mac+0) == 0
        && *(uint16 *)(mac+4) == 0;
}

static inline bool
is_full_mac(byte mac[])
{
    return *(uint32 *)(mac+0) == 0xffffffff
        && *(uint16 *)(mac+4) == 0xffff;
}

static inline bool
is_good_mac(byte mac[])
{
    return false==is_zero_mac(mac) 
        && false==is_full_mac(mac);
}

static inline bool
__is_good_macifs(int ifs)
{
    return ':'==ifs || '-'==ifs;
}

static inline bool
__is_good_macchar(int ch)
{
    return (ch >= '0' && ch <= '9')
        || (ch >= 'a' && ch <= 'f')
        || (ch >= 'A' && ch <= 'F');
}

static inline bool
__is_good_macstring_L(char *macstring)
{
    /*
    * long  macstring is "XX:XX:XX:XX:XX:XX" or "XX-XX-XX-XX-XX-XX"
    */
    int ifs = macstring[2];

    return __is_good_macifs(ifs)
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

static inline bool
__is_good_macstring_M(char *macstring)
{
    /*
    * middle  macstring is "XXXX:XXXX:XXXX" or "XXXX-XXXX-XXXX"
    */
    int ifs = macstring[4];

    return __is_good_macifs(ifs)
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

static inline bool
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

static inline bool
__is_good_macstring(char *macstring)
{
    switch(os_strlen(macstring)) {
        case MACSTRINGLEN_L: return __is_good_macstring_L(macstring);
        case MACSTRINGLEN_M: return __is_good_macstring_M(macstring);
        case MACSTRINGLEN_S: return __is_good_macstring_S(macstring);
        default: return false;
    }
}

static inline byte *
__os_getmac_bystring_L(byte mac[], char macstring[])
{
    int i;
    
    for (i=0; i<OS_MACSIZE; i++) {
        mac[i] = os_hex2number(macstring + 3*i, 2, 16, int);
    }

    return mac;
}

static inline byte *
__os_getmac_bystring_M(byte mac[], char macstring[])
{
    int i;
    
    for (i=0; i<OS_MACSIZE/2; i++) {
        mac[i]  = os_hex2number(macstring + 5*i,     2, 16, int);
        mac[i+1]= os_hex2number(macstring + 5*i + 2, 2, 16, int);
    }

    return mac;
}

static inline byte *
__os_getmac_bystring_S(byte mac[], char macstring[])
{
    int i;
    
    for (i=0; i<OS_MACSIZE; i++) {
        mac[i] = os_hex2number(macstring + 2*i, 2, 16, int);
    }

    return mac;
}

static inline byte *
__os_getmac_bystring(byte mac[], char macstring[])
{
    switch(os_strlen(macstring)) {
        case MACSTRINGLEN_L: return __os_getmac_bystring_L(mac, macstring);
        case MACSTRINGLEN_M: return __os_getmac_bystring_M(mac, macstring);
        case MACSTRINGLEN_S: return __os_getmac_bystring_S(mac, macstring);
        default: return OS_2BMAC;
    }
}

static inline byte *
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

static inline bool
is_good_macstring(char *macstring)
{
    byte mac[6];
    
    return NULL!=os_getmac_bystring(mac, macstring);
}

/*
*  multi-thread unsafe
*/
static inline byte *
os_mac(char *macstring)
{
    static byte mac[OS_MACSIZE] = {0};
    
    return __is_good_macstring(macstring)?__os_getmac_bystring(mac, macstring):OS_ZEROMAC;
}

static inline int
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

static inline int
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

static inline int
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

static inline int
os_macsnprintf(byte mac[], char macstring[], int size, int type, int sep)
{
    switch(type) {
        case MACSTRINGLEN_L: return __os_macsnprintf_L(mac, macstring, size, sep);
        case MACSTRINGLEN_M: return __os_macsnprintf_M(mac, macstring, size, sep);
        case MACSTRINGLEN_S: default: return __os_macsnprintf_S(mac, macstring, size, sep);
    }
}

#define os_macsaprintf(_mac, _macstring, _type, _sep)   ({  \
    BUILD_BUG_NOT_ARRAY(_macstring);                        \
    os_macsnprintf(_mac, _macstring, sizeof(_macstring), _type, _sep); \
})

/*
*  multi-thread unsafe
*/
static inline char *
unsafe_getmacstring(byte mac[], int type, int sep)
{
    static char macstring[1+MACSTRINGLEN_L] = {0};

    os_macsaprintf(mac, macstring, type, sep);

    return macstring;
}

/*
*  multi-thread unsafe
*/
static inline char *
unsafe_macstring(byte mac[])
{
    return unsafe_getmacstring(mac, MACSTRINGLEN_L, ':');
}

static inline bool
os_ipmatch(uint32 ipa, uint32 ipb, uint32 mask)
{
    return (ipa & mask)==(ipb & mask);
}

static inline bkdr_t
os_ipbkdr(uint32 ip)
{
    return os_bin_bkdr(&ip, sizeof(ip));
}
/******************************************************************************/
#endif /* __ADDR_H_a60fcc799b2f44c38dcbf510eb97f0c6__ */
