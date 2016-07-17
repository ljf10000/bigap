#ifndef __OTP_H_ee6f799d4c104240a8e7800f797e0c04__
#define __OTP_H_ee6f799d4c104240a8e7800f797e0c04__
/******************************************************************************/
#ifdef __APP__
/******************************************************************************/
#if IS_PRODUCT_LTEFI_MD
#include "hi_unf_otp.h"
#include "hi_unf_advca.h"
#endif
#include "benv/bcookie.h"

enum {
    OTP_SIZE = 16,
};

#define hisi_otp_init()             hisi_unf_method_0(OTP, Init)
#define hisi_otp_fini()             hisi_unf_method_0(OTP, DeInit)

#define hisi_otp_get_customer_key(_otp)         hisi_unf_method_x(OTP, GetCustomerKey, _otp, OTP_SIZE)
#define hisi_otp_set_customer_key(_otp)         hisi_unf_method_x(OTP, SetCustomerKey, _otp, OTP_SIZE)
#define hisi_otp_get_private_key(_otp, _idx)    hisi_unf_method_x(OTP, GetStbPrivData, _idx, &_otp[_idx])
#define hisi_otp_set_private_key(_otp, _idx)    hisi_unf_method_x(OTP, SetStbPrivData, _idx, &_otp[_idx])

#ifndef OTP_ZERO
#define OTP_ZERO            "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
#endif

#ifndef OTP_PRIVATE
#define OTP_PRIVATE         "\x73\x90\x39\xF1\x32\xEB\x88\x33\xCE\x84\x49\x5D\x18\x33\x64\xC7"
#endif

#ifndef __OTP_CUSTOM
#define __OTP_CUSTOM        ((byte *)PRODUCT_VENDOR_DOMAIN)
#endif

#ifndef OTP_CUSTOM
#define OTP_CUSTOM          ((byte *)PRODUCT_VENDOR)
#endif

enum {
    OTP_ERR_HACKED  = -1,
    OTP_ERR_READ    = -2,
    OTP_ERR_WRITE   = -3,
};

#define OTP_PRINT    0

#if OTP_PRINT
#define otp_println(_fmt, _args...)     os_println(_fmt, ##_args)
#else
#define otp_println(_fmt, _args...)     os_do_nothing()
#endif

#define __otp_eq(_a, _b)    os_memeq(_a, _b, OTP_SIZE)

static inline char *
__otp_string(byte otp[OTP_SIZE])
{
    static char string[2*OTP_SIZE+1];

    os_sprintf(string,
        "%.2X%.2X%.2X%.2X"
        "%.2X%.2X%.2X%.2X"
        "%.2X%.2X%.2X%.2X"
        "%.2X%.2X%.2X%.2X"
        ,
        otp[0],  otp[1],  otp[2],  otp[3],
        otp[4],  otp[5],  otp[6],  otp[7],
        otp[8],  otp[9],  otp[10], otp[11],
        otp[12], otp[13], otp[14], otp[15]);

    return string;
}

static inline byte *
__otp_get_bystring(byte otp[OTP_SIZE], char *string)
{
    int i, len;

    len = os_strlen(string);
    if (2*OTP_SIZE != len) {
        return NULL;
    }

    for (i=0; i<OTP_SIZE; i++) {
        otp[i] = os_hex2number(string + 2*i, 2, 16, int);
    }
    
    return otp;
}

static inline void
__otp_dump(byte otp[OTP_SIZE])
{
    os_println("%s", __otp_string(otp));
}

#if IS_PRODUCT_LTEFI_AP /* use boot as otp register */
#define __otp_init()        0
#define __otp_fini()        0

#ifndef OTP_FILE
#define OTP_FILE            "/dev/mtdblock1"
#endif

#ifndef OTP_FILE_SIZE
#define OTP_FILE_SIZE       (512*1024)
#endif

#ifndef OTP_COOKIE_HEAD
#define OTP_COOKIE_HEAD     "\x01\x03\x05\x07\x09\x0b\x0d\x0f\x11\x13\x15\x17\x19\x1b\x1d\x1f"
#endif

#ifndef OTP_COOKIE_BODY
#define OTP_COOKIE_BODY     OTP_ZERO OTP_ZERO
#endif

#ifndef OTP_COOKIE_TAIL
#define OTP_COOKIE_TAIL     "\x1e\x1c\x1a\x18\x16\x14\x12\x10\x0e\x0c\x0a\x08\x06\x04\x02\x00"
#endif

#ifndef OTP_COOKIE
#define OTP_COOKIE          OTP_COOKIE_HEAD OTP_COOKIE_BODY OTP_COOKIE_TAIL
#endif

enum {
    OTP_ID_CUSTOM,
    OTP_ID_PRIVATE,

    OTP_ID_END
};

static inline int
__otp_file_offset(STREAM f)
{
    int fd, err;
    char *buf = (char *)os_malloc(OTP_FILE_SIZE);
    if (NULL==buf) {
        return -ENOMEM;
    }

    err = os_fread(f, buf, OTP_FILE_SIZE);
    if (err<0) {
        goto error;
    }
    else if (OTP_FILE_SIZE!=err) {
        err = -E2BIG; goto error;
    }

    char *begin = buf;
    while(begin < buf + OTP_FILE_SIZE - (sizeof(OTP_COOKIE) - 1)) {
        char *head = (char *)os_memmem(begin, OTP_FILE_SIZE, OTP_COOKIE_HEAD, sizeof(OTP_COOKIE_HEAD)-1);
        if (NULL==head) {
            err = -ENOEXIST; goto error;
        }
        
        char *tail = head + sizeof(OTP_COOKIE_HEAD OTP_COOKIE_BODY) - 1;
        if (0==os_memcmp(tail, OTP_COOKIE_TAIL)) {
            char *body = head + sizeof(OTP_COOKIE_HEAD) - 1;
            err = body - buf;

            goto error;
        }
        
        begin = head + sizeof(OTP_COOKIE_HEAD) - 1;
    }
    
error:
    os_free(buf);
    
    return err;
}

static inline int
__otp_file_op(int id, char *mode, byte otp[OTP_SIZE])
{
    static int otp_offset = 0;
    int offset, err = 0;

    STREAM f = os_fopen(OTP_FILE, mode);
    if (NULL==f) {
        err = -EACCES; goto error;
    }

    if (0==otp_offset) {
        otp_offset = __otp_file_offset(f);
        if (otp_offset<0) {
            err = otp_offset; goto error;
        }
    }

    offset = otp_offset 
                + sizeof(OTP_COOKIE_HEAD) - 1
                + id * (sizeof(OTP_ZERO) - 1);

    err = fseek(f, offset, 0);
    if (err<0) {
        goto error;
    }

    if (os_streq(mode, "r")) {
        err = os_fread(f, otp, OTP_SIZE);
    }
    else if (os_streq(mode, "r+")) {
        err = os_fwrite(f, otp, OTP_SIZE);
    }
    else {
        err = -EINVAL9;
    }
    
    if (err<0) {
        goto error;
    }
    else if (OTP_SIZE!=err) {
        err = -EFORMAT; goto error;
    }
    else {
        err = 0;
    }
    
error:
    os_fclose(f);

    return err;
}

static inline int
__otp_custom_read(byte otp[OTP_SIZE])
{
    return __otp_file_op(OTP_ID_CUSTOM, "r", otp);
}

static inline int
__otp_custom_write(byte otp[OTP_SIZE])
{
    return __otp_file_op(OTP_ID_CUSTOM, "r+", otp);
}

static inline int
__otp_private_read(byte otp[OTP_SIZE])
{
    return __otp_file_op(OTP_ID_PRIVATE, "r", otp);
}

static inline int
__otp_private_write(byte otp[OTP_SIZE])
{
    return __otp_file_op(OTP_ID_PRIVATE, "r+", otp);
}
#elif IS_PRODUCT_LTEFI_MD
static inline int
__otp_init(void)
{
    int err;

    do{
        err = hisi_otp_init();
        if (err<0) {
            
            sleep(1);
        }
    } while(err);
    
    return 0;
}

static inline int
__otp_fini(void)
{
    int err;
    
    err = hisi_otp_fini();
    if (err<0) {
    }

    return err;
}

static inline int
__otp_custom_read(byte otp[OTP_SIZE])
{
    int err;

    err = hisi_otp_get_customer_key(otp);
    if (err<0) {
        err = OTP_ERR_READ;
    }
    
    return err;
}

static inline int
__otp_custom_write(byte otp[OTP_SIZE])
{
    int err;

    err = hisi_otp_set_customer_key(otp);
    if (err<0) {
        err = OTP_ERR_WRITE;
    } else {
        os_println("custom inited.");
    }

    return err;
}

static inline int
__otp_private_read(byte otp[OTP_SIZE])
{
    int i, err, errs = 0;

    for (i=0; i<OTP_SIZE; i++) {
        err = hisi_otp_get_private_key(otp, i);
        if (err<0) {
            errs = err;
        }
    }
    
    debug_trace_error(errs, "read private");
    if (errs) {
        
        errs = OTP_ERR_READ;
    }

    return errs;
}

static inline int
__otp_private_write(byte otp[OTP_SIZE])
{
    int i, err, errs = 0;

    for (i=0; i<OTP_SIZE; i++) {
        err = hisi_otp_set_private_key(otp, i);
        if (err<0) {
            errs = err;
        }
    }
    
    debug_trace_error(errs, "write private");
    if (errs) {
        errs = OTP_ERR_WRITE;
    } else {
        os_println("private inited.");
    }

    return errs;
}
#elif IS_PRODUCT_PC
#define __otp_init()        0
#define __otp_fini()        0

#define FILE_OTP_CUSTOM     "otp.custom"
#define FILE_OTP_PRIVATE    "otp.private"

static inline int
__otp_custom_read(byte otp[OTP_SIZE])
{
    return os_fgetb(otp, OTP_SIZE, FILE_OTP_CUSTOM);
}

static inline int
__otp_custom_write(byte otp[OTP_SIZE])
{
    return os_fsetb(otp, OTP_SIZE, FILE_OTP_CUSTOM);
}

static inline int
__otp_private_read(byte otp[OTP_SIZE])
{
    return os_fgetb(otp, OTP_SIZE, FILE_OTP_PRIVATE);
}

static inline int
__otp_private_write(byte otp[OTP_SIZE])
{
    return os_fsetb(otp, OTP_SIZE, FILE_OTP_PRIVATE);
}

#endif

static inline int
__otp_check(
    byte val[OTP_SIZE], 
    int (*read)(byte otp[]), 
    int (*write)(byte otp[])
)
{
    byte otp[OTP_SIZE] = {0};
    int err;

    err = (*read)(otp);
    if (err<0) {
        return err;
    }

    if (__otp_eq(otp, OTP_ZERO)) {
        otp_println("otp is zero");

        if (__otp_eq(val, OTP_ZERO)) {
            otp_println("try set otp zero");

            return 0;
        }

        err = (*write)(val);
        if (err<0) {
            return err;
        }

        return 0;
    }
    else if (__otp_eq(otp, __OTP_CUSTOM) || __otp_eq(otp, OTP_CUSTOM)) {
        otp_println("otp set==get");

        return 0;
    }
    else if (__otp_eq(otp, val)) {
        otp_println("otp set==get");

        return 0;
    }
    else {
        otp_println("otp is hacked");

        return OTP_ERR_HACKED;
    }
}

static inline int
__otp_show(byte otp[OTP_SIZE], int (*read)(byte otp[]), bool is_string)
{
    int err = (*read)(otp);
    if (0==err) {
        os_println("%s", is_string?(char *)otp:__otp_string(otp));
    }

    return err;
}

#define __otp_call(_call, _otp, _args...)   os_call(__otp_init, __otp_fini, _call, _otp, ##_args)

static inline int
otp_custom_show(void)
{
    byte otp[OTP_SIZE] = {0};
    
    return __otp_call(__otp_show, otp, __otp_custom_read, true);
}

static inline int
otp_custom_check(void)
{
    return __otp_call(__otp_check, OTP_CUSTOM, __otp_custom_read, __otp_custom_write);
}

static inline int
otp_custom_write(byte val[OTP_SIZE])
{
    return __otp_call(__otp_custom_write, val);
}

static inline int
otp_private_show(void)
{
    byte otp[OTP_SIZE] = {0};
 
    return __otp_call(__otp_show, otp, __otp_private_read, false);
}

static inline int
otp_private_check(byte val[OTP_SIZE])
{
    return __otp_call(__otp_check, val, __otp_private_read, __otp_private_write);
}

static inline int
otp_private_write(byte val[OTP_SIZE])
{
    return __otp_call(__otp_private_write, val);
}
/******************************************************************************/
#endif /* __APP__ */
#endif /* __OTP_H_ee6f799d4c104240a8e7800f797e0c04__ */
