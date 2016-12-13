#ifndef __ADDR_H_a60fcc799b2f44c38dcbf510eb97f0c6__
#define __ADDR_H_a60fcc799b2f44c38dcbf510eb97f0c6__
#ifdef __APP__
/******************************************************************************/
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

typedef struct sockaddr_un sockaddr_un_t;
typedef struct sockaddr_in sockaddr_in_t;
typedef struct sockaddr_ll sockaddr_ll_t;
typedef struct sockaddr_nl sockaddr_nl_t;
typedef struct sockaddr    sockaddr_t;

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

extern bool
is_abstract_sockaddr(void *addr);

static inline char *
get_abstract_path(sockaddr_un_t *addr)
{
    return &addr->sun_path[1];
}

extern void
set_abstract_path(sockaddr_un_t *addr, char *path);

extern void
abstract_path_vsprintf(sockaddr_un_t *addr, const char *fmt, va_list args);

extern void
abstract_path_sprintf(sockaddr_un_t *addr, const char *fmt, ...);

enum { unix_path_size = sizeof(sockaddr_un_t) - offsetof(sockaddr_un_t, sun_path) };
enum { abstract_path_size  = unix_path_size - 1 };

static inline void
set_abstract_sockaddr_len(sockaddr_un_t *addr, int addrlen)
{
    addr->sun_path[addrlen - sizeof(addr->sun_family)] = 0;
}

static inline socklen_t
get_abstract_sockaddr_len(sockaddr_un_t *addr)
{
    return sizeof(addr->sun_family) + 1 + os_strlen(get_abstract_path(addr));
}

extern socklen_t
os_sockaddr_len(sockaddr_t *addr);

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
extern char *
os_getmacby(char *script);
/******************************************************************************/
/*
* just for single-thread, unsafe for multi-thread
*
* @ip: network sort
*/
static inline char *
os_ipstring(uint32 ip)
{
    struct in_addr in = {.s_addr = ip};
    
    return (char *)inet_ntoa(in);
}

static inline uint32
os_ipaddr(char *ipstring)
{
    return inet_addr(ipstring);
}

static inline bool
is_good_ipstring(char *ipstring)
{
    return is_good_str(ipstring) && INADDR_NONE!=inet_addr(ipstring);
}
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
__is_good_macchar(int ch)
{
    return (ch >= '0' && ch <= '9')
        || (ch >= 'a' && ch <= 'f')
        || (ch >= 'A' && ch <= 'F');
}

EXTERN bool
__is_good_macstring(char *macstring);

EXTERN byte *
__os_getmac_bystring(byte mac[], char macstring[]);

EXTERN byte *
os_getmac_bystring(byte mac[], char macstring[]);

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

EXTERN int
os_macsnprintf(byte mac[], char macstring[], int size, int type, int sep);

#define os_macsaprintf(_mac, _macstring, _type, _sep)   ({  \
    BUILD_BUG_NOT_ARRAY(_macstring);                        \
    os_macsnprintf(_mac, _macstring, sizeof(_macstring), _type, _sep); \
})

/*
*  multi-thread unsafe
*/
EXTERN char *
os_getmacstring(byte mac[], int type, int sep);

/*
*  multi-thread unsafe
*/
static inline char *
os_macstring(byte mac[])
{
    return os_getmacstring(mac, MACSTRINGLEN_L, ':');
}

static inline bool
os_ipmatch(uint32 ipa, uint32 ipb, uint32 mask)
{
    return (ipa & mask)==(ipb & mask);
}

#ifdef __BOOT__
#include "weos/boot/addr.c"
#endif
/******************************************************************************/
#endif /* __ADDR_H_a60fcc799b2f44c38dcbf510eb97f0c6__ */
