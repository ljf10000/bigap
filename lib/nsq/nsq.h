#ifndef __NSQ_H_700ea21373ce4d74a3027b48b6f332c8__
#define __NSQ_H_700ea21373ce4d74a3027b48b6f332c8__
/******************************************************************************/
#include "utils.h"
/******************************************************************************/
#if 1
#define __XLIST_NSQ_ERROR(_)                        \
    _(NSQ_E_OK,             0,  "OK"),              \
    _(NSQ_E_CLOSE_WAIT,     1,  "CLOSE_WAIT"),      \
    _(NSQ_E_ERROR,          2,  "E_ERROR"),         \
    _(NSQ_E_INVALID,        2,  "E_INVALID"),       \
    _(NSQ_E_BAD_TOPIC,      3,  "E_BAD_TOPIC"),     \
    _(NSQ_E_BAD_CHANNEL,    4,  "E_BAD_CHANNEL"),   \
    _(NSQ_E_BAD_BODY,       5,  "E_BAD_BODY"),      \
    _(NSQ_E_BAD_MESSAGE,    6,  "E_BAD_MESSAGE"),   \
    _(NSQ_E_PUB_FAILED,     7,  "E_PUB_FAILED"),    \
    _(NSQ_E_MPUB_FAILED,    8,  "E_MPUB_FAILED"),   \
    _(NSQ_E_FIN_FAILED,     9,  "E_FIN_FAILED"),    \
    _(NSQ_E_REQ_FAILED,     10, "E_REQ_FAILED"),    \
    _(NSQ_E_TOUCH_FAILED,   11, "E_TOUCH_FAILED"),  \
    _(NSQ_E_AUTH_FAILED ,   12, "E_AUTH_FAILED "),  \
    _(NSQ_E_UNAUTHORIZED,   13, "E_UNAUTHORIZED"),  \
    /* end */

static inline bool is_good_nsq_error(int id);
static inline char *nsq_error_string(int id);
static inline int nsq_error_idx(char *name);
DECLARE_ENUM(nsq_error, __XLIST_NSQ_ERROR, NSQ_E_END);

static inline bool is_valid_nsq_error(int id)
{
    return is_good_nsq_error(id) && id>=NSQ_E_ERROR;
}

#define NSQ_E_OK            NSQ_E_OK
#define NSQ_E_CLOSE_WAIT    NSQ_E_CLOSE_WAIT
#define NSQ_E_ERROR         NSQ_E_ERROR
#define NSQ_E_INVALID       NSQ_E_INVALID
#define NSQ_E_BAD_TOPIC     NSQ_E_BAD_TOPIC
#define NSQ_E_BAD_CHANNEL   NSQ_E_BAD_CHANNEL
#define NSQ_E_BAD_BODY      NSQ_E_BAD_BODY
#define NSQ_E_BAD_MESSAGE   NSQ_E_BAD_MESSAGE
#define NSQ_E_PUB_FAILED    NSQ_E_PUB_FAILED
#define NSQ_E_MPUB_FAILED   NSQ_E_MPUB_FAILED
#define NSQ_E_MPUB_FAILED   NSQ_E_MPUB_FAILED
#define NSQ_E_FIN_FAILED    NSQ_E_FIN_FAILED
#define NSQ_E_REQ_FAILED    NSQ_E_REQ_FAILED
#define NSQ_E_TOUCH_FAILED  NSQ_E_TOUCH_FAILED
#define NSQ_E_AUTH_FAILED   NSQ_E_AUTH_FAILED
#define NSQ_E_UNAUTHORIZED  NSQ_E_UNAUTHORIZED
#define NSQ_E_END           NSQ_E_END       
#endif

#if 1
#define __XLIST_NSQ_FRAME(_)                    \
    _(NSQ_FRAME_RESPONSE,   0,  "response"),    \
    _(NSQ_FRAME_ERROR,      1,  "error"),       \
    _(NSQ_FRAME_MESSAGE,    2,  "message"),     \
    /* end */

static inline bool is_good_nsq_frame(int id);
static inline char *nsq_frame_string(int id);
static inline int nsq_frame_idx(char *name);
DECLARE_ENUM(nsq_frame, __XLIST_NSQ_FRAME, NSQ_FRAME_END);

#define NSQ_FRAME_RESPONSE  NSQ_FRAME_RESPONSE
#define NSQ_FRAME_ERROR     NSQ_FRAME_ERROR
#define NSQ_FRAME_MESSAGE   NSQ_FRAME_MESSAGE
#define NSQ_FRAME_END       NSQ_FRAME_END     
#endif

enum { 
    NSQ_MSGID_SIZE = 16,
    NSQ_NAME_SIZE = 64,
};

typedef struct {
    uint32 size;
    uint32 type;

    uint64 timestamp;
    uint16 attempts;
    byte id[NSQ_MSGID_SIZE];

    char data[0];
} nsq_msg_t;

typedef struct {
    char *identity;
    char *identity_url;
    char *permission_count;
} nsq_identity_t;

typedef simple_buffer_t nsqb_t;

#define NSQ_MAGIC       "  V2"
#define NSQ_IDENTIFY    "IDENTIFY\n"
#define NSQ_AUTH        "AUTH\n"
#define NSQ_NOP         "NOP\n"
#define NSQ_CLS         "CLS\n"
#define NSQ_SUB         "SUB "
#define NSQ_PUB         "PUB "
#define NSQ_MPUB        "MPUB "
#define NSQ_RDY         "RDY "
#define NSQ_FIN         "FIN "
#define NSQ_REQ         "REQ "
#define NSQ_TOUCH       "TOUCH "

#define nsqb_init(_b, _size)    simple_buffer_init(_b, _size)
#define nsqb_fini(_b)           simple_buffer_fini(_b)
#define nsqb_clean(_b)          simple_buffer_clean(_b)

#define nsqb_number(_b, _number)            simple_buffer_append_number(_b, _number)
#define nsqb_buffer(_b, _buf, _len)         simple_buffer_append_buffer(_b, _buf, _len)
#define nsqb_string(_b, _string)            simple_buffer_append_string(_b, _string)
#define nsqb_sprintf(_b, _fmt, _args...)    simple_buffer_sprintf(_b, _fmt, ##_args)
#define nsqb_command(_b, _COMMAND)          nsqb_buffer(_b, _COMMAND, sizeof(_COMMAND)-1)

static inline bool
is_good_nsq_name_char(int ch)
{
    return  '.'==ch || '_'==ch ||
            (ch>='a' && ch<='z') ||
            (ch>='A' && ch<='Z') ||
            (ch>='0' && ch<='9');
}

static inline bool
is_good_nsq_name(char *name)
{
    char *p = name;
    int len = 0;

    while(*p) {
        len++;
        
        if (false==is_good_nsq_name_char(*p)) {
            return false;
        }
        else if (len>NSQ_NAME_SIZE) {
            return false;
        }
    }

    return true;
}

static inline int
nsqb_data(nsqb_t *b, char *data, uint32 len)
{
    uint32 len_n = htonl(len);
    int err = 0;

    err = nsqb_number(b, len_n);
    if (err<0) {
        return err;
    }
    
    err = nsqb_buffer(b, data, len);
    if (err<0) {
        return err;
    }
    
    return 0;
}

static inline int
nsqb_MAGIC(nsqb_t *b)
{
    int err;
    
    nsqb_clean(b);

    err = nsqb_command(b, NSQ_MAGIC);
    if (err<0) {
        return err;
    }

    return 0;
}

static inline int
nsqb_CLS(nsqb_t *b)
{
    int err;
    
    nsqb_clean(b);
    
    err = nsqb_command(b, NSQ_CLS);
    if (err<0) {
        return err;
    }

    return 0;
}

static inline int
nsqb_IDENTIFY(nsqb_t *b, char *json)
{
    int err;
    
    nsqb_clean(b);

    err = nsqb_command(b, NSQ_IDENTIFY);
    if (err<0) {
        return err;
    }

    err = nsqb_data(b, json, os_strlen(json));
    if (err<0) {
        return err;
    }

    return 0;
}

static inline int
nsqb_SUB(nsqb_t *b, char *topic, char *channel)
{
    int err;
    
    nsqb_clean(b);
    
    err = nsqb_sprintf(b, NSQ_SUB "%s %s\n", topic, channel);
    if (err<0) {
        return err;
    }

    return 0;
}

static inline int
nsqb_PUB(nsqb_t *b, char *topic, char *data, uint32 len)
{
    int err;
    
    nsqb_clean(b);

    err = nsqb_sprintf(b, NSQ_PUB "%s\n", topic);
    if (err<0) {
        return err;
    }

    err = nsqb_data(b, data, len);
    if (err<0) {
        return err;
    }

    return 0;
}

static inline int
nsqb_MPUB(nsqb_t *b, char *topic, struct iovec *iov, uint32 count)
{
    uint32 count_n = htonl(count);
    int i, err;
    
    nsqb_clean(b);

    err = nsqb_sprintf(b, NSQ_MPUB "%s\n", topic);
    if (err<0) {
        return err;
    }

    err = nsqb_number(b, count_n);
    if (err<0) {
        return err;
    }

    for (i=0; i<count; i++) {
        err = nsqb_data(b, iov[i].iov_base, iov[i].iov_len);
        if (err<0) {
            return err;
        }
    }
    
    return 0;
}

static inline int
nsqb_RDY(nsqb_t *b, uint32 count)
{
    int err;
    
    nsqb_clean(b);
    
    err = nsqb_sprintf(b, NSQ_RDY "%u\n", count);
    if (err<0) {
        return err;
    }

    return 0;
}

static inline int
nsqb_FIN(nsqb_t *b, char *msg_id)
{
    int err;
    
    nsqb_clean(b);

    err = nsqb_command(b, NSQ_FIN);
    if (err<0) {
        return err;
    }
    
    err = nsqb_buffer(b, msg_id, NSQ_MSGID_SIZE);
    if (err<0) {
        return err;
    }
    
    err = nsqb_buffer(b, "\n", 1);
    if (err<0) {
        return err;
    }

    return 0;
}

static inline int
nsqb_REQ(nsqb_t *b, char *msg_id, int timeout)
{
    int err;
    
    nsqb_clean(b);

    err = nsqb_command(b, NSQ_REQ);
    if (err<0) {
        return err;
    }
    
    err = nsqb_buffer(b, msg_id, NSQ_MSGID_SIZE);
    if (err<0) {
        return err;
    }

    err = nsqb_sprintf(b, "%d\n", timeout);
    if (err<0) {
        return err;
    }
    
    return 0;
}

static inline int
nsqb_TOUCH(nsqb_t *b, char *msg_id)
{
    int err;
    
    nsqb_clean(b);

    err = nsqb_command(b, NSQ_REQ);
    if (err<0) {
        return err;
    }
    
    err = nsqb_buffer(b, msg_id, NSQ_MSGID_SIZE);
    if (err<0) {
        return err;
    }

    err = nsqb_buffer(b, "\n", 1);
    if (err<0) {
        return err;
    }
    
    return 0;
}

static inline int
nsqb_NOP(nsqb_t *b)
{
    int err;
    
    nsqb_clean(b);
    
    err = nsqb_command(b, NSQ_NOP);
    if (err<0) {
        return err;
    }

    return 0;
}

static inline int
nsqb_AUTH(nsqb_t *b, char *secret, uint32 len)
{
    int err;
    
    nsqb_clean(b);

    err = nsqb_command(b, NSQ_AUTH);
    if (err<0) {
        return err;
    }

    err = nsqb_data(b, secret, len);
    if (err<0) {
        return err;
    }

    return 0;
}

/******************************************************************************/
#endif /* __NSQ_H_700ea21373ce4d74a3027b48b6f332c8__ */
