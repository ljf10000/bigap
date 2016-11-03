#ifndef __NSQ_H_700ea21373ce4d74a3027b48b6f332c8__
#define __NSQ_H_700ea21373ce4d74a3027b48b6f332c8__
/******************************************************************************/
#include "utils.h"
/******************************************************************************/
#if 1
#define NSQ_IDENTIFY_CLIENT_ID_STRING               "client_id"
#define NSQ_IDENTIFY_HOSTNAME_STRING                "hostname"
#define NSQ_IDENTIFY_FEATURE_NEGOTIATION_STRING     "feature_negotiation"
#define NSQ_IDENTIFY_HEARBEAT_INTERVAL_STRING       "heartbeat_interval"
#define NSQ_IDENTIFY_OUTPUT_BUFFER_SIZE_STRING      "output_buffer_size"
#define NSQ_IDENTIFY_OUTPUT_BUFFER_TIMEOUT_STRING   "output_buffer_timeout"
#define NSQ_IDENTIFY_USER_AGENT_STRING              "user_agent"
#define NSQ_IDENTIFY_MSG_TIMEOUT_STRING             "msg_timeout"

#define XCMEM_NSQ_IDENTIFY(_)                                                               \
    _(NSQ_IDENTIFY_CLIENT_ID,               0,  NSQ_IDENTIFY_CLIENT_ID_STRING),             \
    _(NSQ_IDENTIFY_HOSTNAME ,               1,  NSQ_IDENTIFY_HOSTNAME_STRING),              \
    _(NSQ_IDENTIFY_FEATURE_NEGOTIATION,     2,  NSQ_IDENTIFY_FEATURE_NEGOTIATION_STRING),   \
    _(NSQ_IDENTIFY_HEARBEAT_INTERVAL,       3,  NSQ_IDENTIFY_HEARBEAT_INTERVAL_STRING),     \
    _(NSQ_IDENTIFY_OUTPUT_BUFFER_SIZE,      4,  NSQ_IDENTIFY_OUTPUT_BUFFER_SIZE_STRING),    \
    _(NSQ_IDENTIFY_OUTPUT_BUFFER_TIMEOUT,   5,  NSQ_IDENTIFY_OUTPUT_BUFFER_TIMEOUT_STRING), \
    _(NSQ_IDENTIFY_USER_AGENT,              6,  NSQ_IDENTIFY_USER_AGENT_STRING),            \
    _(NSQ_IDENTIFY_MSG_TIMEOUT,             7,  NSQ_IDENTIFY_MSG_TIMEOUT_STRING),           \
    /* end */
DECLARE_ENUM(nsq_identify, XCMEM_NSQ_IDENTIFY, NSQ_IDENTIFY_END);

static inline bool is_good_nsq_identify(int id);
static inline char *nsq_identify_string(int id);
static inline int nsq_identify_idx(char *name);

#define NSQ_IDENTIFY_CLIENT_ID              NSQ_IDENTIFY_CLIENT_ID
#define NSQ_IDENTIFY_HOSTNAME               NSQ_IDENTIFY_HOSTNAME
#define NSQ_IDENTIFY_FEATURE_NEGOTIATION    NSQ_IDENTIFY_FEATURE_NEGOTIATION
#define NSQ_IDENTIFY_HEARBEAT_INTERVAL      NSQ_IDENTIFY_HEARBEAT_INTERVAL
#define NSQ_IDENTIFY_OUTPUT_BUFFER_SIZE     NSQ_IDENTIFY_OUTPUT_BUFFER_SIZE
#define NSQ_IDENTIFY_OUTPUT_BUFFER_TIMEOUT  NSQ_IDENTIFY_OUTPUT_BUFFER_TIMEOUT
#define NSQ_IDENTIFY_USER_AGENT             NSQ_IDENTIFY_USER_AGENT
#define NSQ_IDENTIFY_MSG_TIMEOUT            NSQ_IDENTIFY_MSG_TIMEOUT
#define NSQ_IDENTIFY_END                    NSQ_IDENTIFY_END   
#endif

enum {
    NSQ_IDENTIFY_TYPE_CLOUD,
    NSQ_IDENTIFY_TYPE_LOCAL,
    NSQ_IDENTIFY_TYPE_CONST,
    
    NSQ_IDENTIFY_TYPE_END
};

typedef struct {
    int jtype;
    int type;
} nsq_identify_rule_t;

#define NSQ_IDENTIFY_RULE(_id, _jtype, _type)   [_id] = { .jtype = _jtype, .type = _type }

static inline nsq_identify_rule_t *
nsq_identify_rule(void)
{
    static nsq_identify_rule_t rule[NSQ_IDENTIFY_END] = {
        NSQ_IDENTIFY_RULE(NSQ_IDENTIFY_CLIENT_ID,               jtype_string,   NSQ_IDENTIFY_TYPE_LOCAL),
        NSQ_IDENTIFY_RULE(NSQ_IDENTIFY_HOSTNAME,                jtype_string,   NSQ_IDENTIFY_TYPE_LOCAL),
        NSQ_IDENTIFY_RULE(NSQ_IDENTIFY_FEATURE_NEGOTIATION,     jtype_bool,     NSQ_IDENTIFY_TYPE_CLOUD),
        NSQ_IDENTIFY_RULE(NSQ_IDENTIFY_HEARBEAT_INTERVAL,       jtype_int,      NSQ_IDENTIFY_TYPE_CLOUD),
        NSQ_IDENTIFY_RULE(NSQ_IDENTIFY_OUTPUT_BUFFER_SIZE,      jtype_int,      NSQ_IDENTIFY_TYPE_CLOUD),
        NSQ_IDENTIFY_RULE(NSQ_IDENTIFY_OUTPUT_BUFFER_TIMEOUT,   jtype_int,      NSQ_IDENTIFY_TYPE_CLOUD),
        NSQ_IDENTIFY_RULE(NSQ_IDENTIFY_USER_AGENT,              jtype_string,   NSQ_IDENTIFY_TYPE_CONST),
        NSQ_IDENTIFY_RULE(NSQ_IDENTIFY_MSG_TIMEOUT,             jtype_string,   false),
    };

    return rule;
}

#if 1
#define NSQ_AUTH_IDENTIFY_STRING            "identity"
#define NSQ_AUTH_IDENTIFY_URL_STRING        "identity_url"
#define NSQ_AUTH_PERMISSION_COUNT_STRING    "permission_count"
    
#define XCMEM_NSQ_AUTH(_)                                                   \
    _(NSQ_AUTH_IDENTIFY,            0,  NSQ_AUTH_IDENTIFY_STRING),          \
    _(NSQ_AUTH_IDENTIFY_URL ,       1,  NSQ_AUTH_IDENTIFY_URL_STRING),      \
    _(NSQ_AUTH_PERMISSION_COUNT,    2,  NSQ_AUTH_PERMISSION_COUNT_STRING),  \
    /* end */
DECLARE_ENUM(nsq_auth, XCMEM_NSQ_AUTH, NSQ_AUTH_END);

static inline bool is_good_nsq_auth(int id);
static inline char *nsq_auth_string(int id);
static inline int nsq_auth_idx(char *name);

#define NSQ_AUTH_IDENTIFY           NSQ_AUTH_IDENTIFY
#define NSQ_AUTH_IDENTIFY_URL       NSQ_AUTH_IDENTIFY_URL
#define NSQ_AUTH_PERMISSION_COUNT   NSQ_AUTH_PERMISSION_COUNT
#define NSQ_AUTH_END                NSQ_AUTH_END  
#endif

#if 1
#define NSQ_E_OK_STRING             "OK"
#define NSQ_E_CLOSE_WAIT_STRING     "CLOSE_WAIT"
#define NSQ_E_ERROR_STRING          "E_ERROR"
#define NSQ_E_INVALID_STRING        "E_INVALID"
#define NSQ_E_BAD_TOPIC_STRING      "E_BAD_TOPIC"
#define NSQ_E_BAD_CHANNEL_STRING    "E_BAD_CHANNEL"
#define NSQ_E_BAD_BODY_STRING       "E_BAD_BODY"
#define NSQ_E_BAD_MESSAGE_STRING    "E_BAD_MESSAGE"
#define NSQ_E_PUB_FAILED_STRING     "E_PUB_FAILED"
#define NSQ_E_MPUB_FAILED_STRING    "E_MPUB_FAILED"
#define NSQ_E_FIN_FAILED_STRING     "E_FIN_FAILED"
#define NSQ_E_REQ_FAILED_STRING     "E_REQ_FAILED"
#define NSQ_E_TOUCH_FAILED_STRING   "E_TOUCH_FAILED"
#define NSQ_E_AUTH_FAILED_STRING    "E_AUTH_FAILED "
#define NSQ_E_UNAUTHORIZED_STRING   "E_UNAUTHORIZED"
    
#define XCMEM_NSQ_ERROR(_)                                  \
    _(NSQ_E_OK,             0,  NSQ_E_OK_STRING),           \
    _(NSQ_E_CLOSE_WAIT,     1,  NSQ_E_CLOSE_WAIT_STRING),   \
    _(NSQ_E_ERROR,          2,  NSQ_E_ERROR_STRING),        \
    _(NSQ_E_INVALID,        2,  NSQ_E_INVALID_STRING),      \
    _(NSQ_E_BAD_TOPIC,      3,  NSQ_E_BAD_TOPIC_STRING),    \
    _(NSQ_E_BAD_CHANNEL,    4,  NSQ_E_BAD_CHANNEL_STRING),  \
    _(NSQ_E_BAD_BODY,       5,  NSQ_E_BAD_BODY_STRING),     \
    _(NSQ_E_BAD_MESSAGE,    6,  NSQ_E_BAD_MESSAGE_STRING),  \
    _(NSQ_E_PUB_FAILED,     7,  NSQ_E_PUB_FAILED_STRING),   \
    _(NSQ_E_MPUB_FAILED,    8,  NSQ_E_MPUB_FAILED_STRING),  \
    _(NSQ_E_FIN_FAILED,     9,  NSQ_E_FIN_FAILED_STRING),   \
    _(NSQ_E_REQ_FAILED,     10, NSQ_E_REQ_FAILED_STRING),   \
    _(NSQ_E_TOUCH_FAILED,   11, NSQ_E_TOUCH_FAILED_STRING), \
    _(NSQ_E_AUTH_FAILED,    12, NSQ_E_AUTH_FAILED_STRING),  \
    _(NSQ_E_UNAUTHORIZED,   13, NSQ_E_UNAUTHORIZED_STRING), \
    /* end */
DECLARE_ENUM(nsq_error, XCMEM_NSQ_ERROR, NSQ_E_END);

static inline bool is_good_nsq_error(int id);
static inline char *nsq_error_string(int id);
static inline int nsq_error_idx(char *name);

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
#define XCMEM_NSQ_FRAME(_)                      \
    _(NSQ_FRAME_RESPONSE,   0,  "response"),    \
    _(NSQ_FRAME_ERROR,      1,  "error"),       \
    _(NSQ_FRAME_MESSAGE,    2,  "message"),     \
    /* end */
DECLARE_ENUM(nsq_frame, XCMEM_NSQ_FRAME, NSQ_FRAME_END);

static inline bool is_good_nsq_frame(int id);
static inline char *nsq_frame_string(int id);
static inline int nsq_frame_idx(char *name);

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

    char body[0];
} nsq_msg_t;

typedef simple_buffer_t nsq_buffer_t;

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
nsqb_data(nsq_buffer_t *b, char *data, uint32 len)
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
nsqb_MAGIC(nsq_buffer_t *b)
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
nsqb_CLS(nsq_buffer_t *b)
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
nsqb_IDENTIFY(nsq_buffer_t *b, char *json)
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
nsqb_SUB(nsq_buffer_t *b, char *topic, char *channel)
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
nsqb_PUB(nsq_buffer_t *b, char *topic, char *data, uint32 len)
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
nsqb_MPUB(nsq_buffer_t *b, char *topic, struct iovec *iov, uint32 count)
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
nsqb_RDY(nsq_buffer_t *b, uint32 count)
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
nsqb_FIN(nsq_buffer_t *b, char *msg_id)
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
nsqb_REQ(nsq_buffer_t *b, char *msg_id, int timeout)
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
nsqb_TOUCH(nsq_buffer_t *b, char *msg_id)
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
nsqb_NOP(nsq_buffer_t *b)
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
nsqb_AUTH(nsq_buffer_t *b, char *secret, uint32 len)
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
