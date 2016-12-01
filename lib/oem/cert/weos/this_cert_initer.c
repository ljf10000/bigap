#define __THIS_CERT_INITER(_idx)    [_idx] = __OS_CERT_INITER(OS_CERT_client_crt##_idx, OS_CERT_client_key##_idx)
#include "oem/cert/weos/client.key.c"
#include "oem/cert/weos/client.crt.c"
#define __THIS_CERT_INITERS { \
    __THIS_CERT_INITER(0), \
    __THIS_CERT_INITER(1), \
    __THIS_CERT_INITER(2), \
    __THIS_CERT_INITER(3), \
    __THIS_CERT_INITER(4), \
    __THIS_CERT_INITER(5), \
    __THIS_CERT_INITER(6), \
    __THIS_CERT_INITER(7), \
    __THIS_CERT_INITER(8), \
    __THIS_CERT_INITER(9), \
    __THIS_CERT_INITER(10), \
    __THIS_CERT_INITER(11), \
    __THIS_CERT_INITER(12), \
    __THIS_CERT_INITER(13), \
    __THIS_CERT_INITER(14), \
    __THIS_CERT_INITER(15), \
    __THIS_CERT_INITER(16), \
    __THIS_CERT_INITER(17), \
    __THIS_CERT_INITER(18), \
    __THIS_CERT_INITER(19), \
    __THIS_CERT_INITER(20), \
    __THIS_CERT_INITER(21), \
    __THIS_CERT_INITER(22), \
    __THIS_CERT_INITER(23), \
    __THIS_CERT_INITER(24), \
    __THIS_CERT_INITER(25), \
    __THIS_CERT_INITER(26), \
    __THIS_CERT_INITER(27), \
    __THIS_CERT_INITER(28), \
    __THIS_CERT_INITER(29), \
    __THIS_CERT_INITER(30), \
    __THIS_CERT_INITER(31), \
} /* end */
