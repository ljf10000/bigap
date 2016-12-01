#ifndef __CERT_H_832f0e1840a446cfb271062ae46dd4cb__
#define __CERT_H_832f0e1840a446cfb271062ae46dd4cb__
/******************************************************************************/
#ifndef OS_CERT_COUNT
#define OS_CERT_COUNT           100
#endif

typedef struct {
    char *cert;
    char *key;
} os_cert_t;

#define __OS_CERT_INITER(_cert, _key)  { \
    .cert       = _cert,    \
    .key        = _key,     \
}   /* end */

/******************************************************************************/
#endif /* __CERT_H_832f0e1840a446cfb271062ae46dd4cb__ */
