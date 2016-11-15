#ifndef __NSQ_H_700ea21373ce4d74a3027b48b6f332c8__
#define __NSQ_H_700ea21373ce4d74a3027b48b6f332c8__
/******************************************************************************/
#include "utils.h"
/******************************************************************************/
#ifndef NSQ_NAMESIZE
#define NSQ_NAMESIZE        64
#endif

#ifndef NSQ_PORT
#define NSQ_PORT            4151
#endif

#ifndef NSQ_TICKS
#define NSQ_TICKS           PC_VAL(5000, 5000)  // ms
#endif

#ifndef NSQ_CONF
#define NSQ_CONF            PC_FILE("/tmp/config", "nsqa.conf")
#endif

#ifndef NSQ_CACHE
#define NSQ_CACHE           "/tmp/cache"
#endif

#ifndef NSQ_SCRIPT
#define NSQ_SCRIPT          PC_FILE("/lib/script", "nsq.script")
#endif

#ifndef NSQ_USER_AGENT
#define NSQ_USER_AGENT      "weos-nsq-agent"
#endif

#ifndef NSQ_TOPIC
#define NSQ_TOPIC           "weos.nsq.topic.default"
#endif

#ifndef NSQ_RDY
#define NSQ_RDY                     PC_VAL(128, 32)
#endif

#ifndef NSQ_RDY_MIN
#define NSQ_RDY_MIN                 PC_VAL(32, 8)
#endif

#ifndef NSQ_NEGOTIATION
#define NSQ_NEGOTIATION             false
#endif

#ifndef NSQ_HEARBEAT_INTERVAL
#define NSQ_HEARBEAT_INTERVAL       (30*1000)
#endif

#ifndef NSQ_RECV_BUFFER_SIZE
#define NSQ_RECV_BUFFER_SIZE        (4*1024)    // for nsqd
#endif

#ifndef NSQ_SEND_BUFFER_SIZE
#define NSQ_SEND_BUFFER_SIZE        (16*1024)   // for nsqd
#endif

#ifndef NSQ_SEND_BUFFER_TIMEOUT
#define NSQ_SEND_BUFFER_TIMEOUT     250         // for nsqd
#endif

#ifndef NSQ_MSG_TIMEOUT
#define NSQ_MSG_TIMEOUT             PC_VAL(5*1000, 10*1000)
#endif

#define NSQ_MSG_HEARTBEAT           "_heartbeat_"

enum { 
    NSQ_MSGID_SIZE  = 16,
    NSQ_NAME_SIZE   = 64,
};

#if 1
#define NSQ_FSM_ENUM_MAPPER(_)                  \
    _(NSQ_FSM_INIT,         0,  "init"),        \
    _(NSQ_FSM_RESOLVED,     1,  "resolved"),    \
    _(NSQ_FSM_CONNECTED,    2,  "connected"),   \
    _(NSQ_FSM_HANDSHAKED,   3,  "handshaked"),  \
    _(NSQ_FSM_IDENTIFYING,  4,  "identifying"), \
    _(NSQ_FSM_IDENTIFIED,   5,  "identified"),  \
    _(NSQ_FSM_AUTHING,      6,  "authing"),     \
    _(NSQ_FSM_AUTHED,       7,  "authed"),      \
    _(NSQ_FSM_SUBSCRIBING,  8,  "subscribing"), \
    _(NSQ_FSM_SUBSCRIBED,   9,  "subscribed"),  \
    _(NSQ_FSM_RUN,         10,  "run"),         \
    /* end */
DECLARE_ENUM(nsq_fsm, NSQ_FSM_ENUM_MAPPER, NSQ_FSM_END);

static inline bool is_good_nsq_fsm(int id);
static inline char *nsq_fsm_string(int id);
static inline int nsq_fsm_idx(char *name);

#define NSQ_FSM_INIT            NSQ_FSM_INIT
#define NSQ_FSM_RESOLVED        NSQ_FSM_RESOLVED
#define NSQ_FSM_CONNECTED       NSQ_FSM_CONNECTED
#define NSQ_FSM_HANDSHAKED      NSQ_FSM_HANDSHAKED
#define NSQ_FSM_IDENTIFYING     NSQ_FSM_IDENTIFYING
#define NSQ_FSM_IDENTIFIED      NSQ_FSM_IDENTIFIED
#define NSQ_FSM_AUTHING         NSQ_FSM_AUTHING
#define NSQ_FSM_AUTHED          NSQ_FSM_AUTHED
#define NSQ_FSM_SUBSCRIBING     NSQ_FSM_SUBSCRIBING
#define NSQ_FSM_SUBSCRIBED      NSQ_FSM_SUBSCRIBED
#define NSQ_FSM_RUN             NSQ_FSM_RUN
#define NSQ_FSM_END             NSQ_FSM_END
#endif

#if 1
#define NSQ_IDENTIFY_FEATURE_NEGOTIATION_NAME   "feature_negotiation"

#define NSQ_IDENTIFY_ENUM_MAPPER(_)                                         \
    _(NSQ_IDENTIFY_CLIENT_ID,               0,  "client_id"),               \
    _(NSQ_IDENTIFY_HOSTNAME,                1,  "hostname"),                \
    _(NSQ_IDENTIFY_FEATURE_NEGOTIATION,     2,  NSQ_IDENTIFY_FEATURE_NEGOTIATION_NAME), \
    _(NSQ_IDENTIFY_HEARBEAT_INTERVAL,       3,  "heartbeat_interval"),      \
    _(NSQ_IDENTIFY_OUTPUT_BUFFER_SIZE,      4,  "output_buffer_size"),      \
    _(NSQ_IDENTIFY_OUTPUT_BUFFER_TIMEOUT,   5,  "output_buffer_timeout"),   \
    _(NSQ_IDENTIFY_USER_AGENT,              6,  "user_agent"),              \
    _(NSQ_IDENTIFY_MSG_TIMEOUT,             7,  "msg_timeout"),             \
    /* end */
DECLARE_ENUM(nsq_identify, NSQ_IDENTIFY_ENUM_MAPPER, NSQ_IDENTIFY_END);

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

#define NSQ_IDENTIFY_RULE_INITER {                  \
    J_RULE(NSQ_IDENTIFY_CLIENT_ID,                  \
        jtype_string,                               \
        JRULE_DYNAMIC,                              \
        os_getbasemac),                             \
    J_RULE(NSQ_IDENTIFY_HOSTNAME,                   \
        jtype_string,                               \
        JRULE_DYNAMIC,                              \
        os_getbasemac),                             \
    J_RULE(NSQ_IDENTIFY_FEATURE_NEGOTIATION,        \
        jtype_bool,                                 \
        JRULE_CONST,                                \
        NSQ_NEGOTIATION),                           \
    J_RULE(NSQ_IDENTIFY_HEARBEAT_INTERVAL,          \
        jtype_int,                                  \
        0,                                          \
        NSQ_HEARBEAT_INTERVAL),                     \
    J_RULE(NSQ_IDENTIFY_OUTPUT_BUFFER_SIZE,         \
        jtype_int,                                  \
        0,                                          \
        NSQ_SEND_BUFFER_SIZE),                      \
    J_RULE(NSQ_IDENTIFY_OUTPUT_BUFFER_TIMEOUT,      \
        jtype_int,                                  \
        0,                                          \
        NSQ_SEND_BUFFER_TIMEOUT),                   \
    J_RULE(NSQ_IDENTIFY_USER_AGENT,                 \
        jtype_string,                               \
        JRULE_CONST,                                \
        NSQ_USER_AGENT),                            \
    J_RULE(NSQ_IDENTIFY_MSG_TIMEOUT,                \
        jtype_int,                                  \
        0,                                          \
        NSQ_MSG_TIMEOUT),                           \
}   /* end */

static inline jrule_ops_t *
nsq_identify_jrule_ops(void)
{
    static jrule_t rule[NSQ_IDENTIFY_END] = NSQ_IDENTIFY_RULE_INITER;
    static jrule_ops_t ops = JRULE_OPS_INITER(rule,
                                is_good_nsq_identify, 
                                nsq_identify_string, 
                                nsq_identify_idx);

    return &ops;
}

static inline int
nsq_identify_jcheck(jobj_t jobj)
{
    return jrule_apply(jobj, nsq_identify_jrule_ops(), NULL);
}

static inline int
nsq_identify_jrepair(jobj_t jobj)
{
    int err;

    err = jrule_apply(jobj, nsq_identify_jrule_ops(), jrule_repair);
    if (err<0) {
        return err;
    }
    
#if 0
    jobj_add_string(jobj, NSQ_IDENTIFY_USER_AGENT_NAME, NSQ_USER_AGENT);
    jobj_add_string(jobj, NSQ_IDENTIFY_CLIENT_ID_NAME,  nsqa.cfg.client_id);
    jobj_add_string(jobj, NSQ_IDENTIFY_HOSTNAME_NAME,   nsqa.cfg.hostname);
#endif

    return 0;
}
#endif

#if 1
#define NSQ_INSTANCE_NAME_NAME      "name"
#define NSQ_INSTANCE_DOMAIN_NAME    "domain"
#define NSQ_INSTANCE_PORT_NAME      "port"
#define NSQ_INSTANCE_TOPIC_NAME     "topic"
#define NSQ_INSTANCE_IDENTIFY_NAME  "identify"

#define NSQ_INSTANCE_ENUM_MAPPER(_)                                 \
    _(NSQ_INSTANCE_NAME,        0,  NSQ_INSTANCE_NAME_NAME),        \
    _(NSQ_INSTANCE_DOMAIN,      1,  NSQ_INSTANCE_DOMAIN_NAME),      \
    _(NSQ_INSTANCE_PORT,        2,  NSQ_INSTANCE_PORT_NAME),        \
    _(NSQ_INSTANCE_TOPIC,       3,  NSQ_INSTANCE_TOPIC_NAME),       \
    _(NSQ_INSTANCE_IDENTIFY,    4,  NSQ_INSTANCE_IDENTIFY_NAME),    \
    /* end */
DECLARE_ENUM(nsq_instance, NSQ_INSTANCE_ENUM_MAPPER, NSQ_INSTANCE_END);

static inline bool is_good_nsq_instance(int id);
static inline char *nsq_instance_string(int id);
static inline int nsq_instance_idx(char *name);

#define NSQ_INSTANCE_NAME       NSQ_INSTANCE_NAME
#define NSQ_INSTANCE_DOMAIN     NSQ_INSTANCE_DOMAIN
#define NSQ_INSTANCE_PORT       NSQ_INSTANCE_PORT
#define NSQ_INSTANCE_TOPIC      NSQ_INSTANCE_TOPIC
#define NSQ_INSTANCE_IDENTIFY   NSQ_INSTANCE_IDENTIFY
#define NSQ_INSTANCE_END        NSQ_INSTANCE_END

#define NSQ_INSTANCE_RULE_INITER {  \
    J_RULE(NSQ_INSTANCE_NAME,       \
        jtype_string,               \
        JRULE_MUST,                 \
        NULL),                      \
    J_RULE(NSQ_INSTANCE_DOMAIN,     \
        jtype_string,               \
        JRULE_MUST,                 \
        NULL),                      \
    J_RULE(NSQ_INSTANCE_PORT,       \
        jtype_int,                  \
        0,                          \
        NSQ_PORT),                  \
    J_RULE(NSQ_INSTANCE_TOPIC,      \
        jtype_string,               \
        0,                          \
        NSQ_TOPIC),                 \
    J_RULE(NSQ_INSTANCE_IDENTIFY,   \
        jtype_object,               \
        JRULE_MUST,                 \
        nsq_identify_jrepair),      \
}   /* end */

static inline jrule_ops_t *
nsq_instance_jrule_ops(void)
{
    static jrule_t rule[NSQ_INSTANCE_END] = NSQ_INSTANCE_RULE_INITER;
    static jrule_ops_t ops = JRULE_OPS_INITER(rule,
                                is_good_nsq_instance, 
                                nsq_instance_string, 
                                nsq_instance_idx);

    return &ops;
}

static inline int
nsq_instance_jcheck(jobj_t jobj)
{
    return jrule_apply(jobj, nsq_instance_jrule_ops(), NULL);
}

static inline int
nsq_instance_jrepair(jobj_t jobj)
{
    int err;

    err = jrule_apply(jobj, nsq_instance_jrule_ops(), jrule_repair);
    if (err<0) {
        return err;
    }

    return 0;
}
#endif

#if 1
#define NSQ_AUTH_IDENTIFY_NAME            "identity"
#define NSQ_AUTH_IDENTIFY_URL_NAME        "identity_url"
#define NSQ_AUTH_PERMISSION_COUNT_NAME    "permission_count"

#define NSQ_AUTH_ENUM_MAPPER(_)                                             \
    _(NSQ_AUTH_IDENTIFY,            0,  NSQ_AUTH_IDENTIFY_NAME),            \
    _(NSQ_AUTH_IDENTIFY_URL ,       1,  NSQ_AUTH_IDENTIFY_URL_NAME),        \
    _(NSQ_AUTH_PERMISSION_COUNT,    2,  NSQ_AUTH_PERMISSION_COUNT_NAME),    \
    /* end */
DECLARE_ENUM(nsq_auth, NSQ_AUTH_ENUM_MAPPER, NSQ_AUTH_END);

static inline bool is_good_nsq_auth(int id);
static inline char *nsq_auth_string(int id);
static inline int nsq_auth_idx(char *name);

#define NSQ_AUTH_IDENTIFY           NSQ_AUTH_IDENTIFY
#define NSQ_AUTH_IDENTIFY_URL       NSQ_AUTH_IDENTIFY_URL
#define NSQ_AUTH_PERMISSION_COUNT   NSQ_AUTH_PERMISSION_COUNT
#define NSQ_AUTH_END                NSQ_AUTH_END  
#endif

#if 1
#define NSQ_E_OK_NAME             "OK"
#define NSQ_E_CLOSE_WAIT_NAME     "CLOSE_WAIT"
#define NSQ_E_ERROR_NAME          "E_ERROR"
#define NSQ_E_INVALID_NAME        "E_INVALID"
#define NSQ_E_BAD_TOPIC_NAME      "E_BAD_TOPIC"
#define NSQ_E_BAD_CHANNEL_NAME    "E_BAD_CHANNEL"
#define NSQ_E_BAD_BODY_NAME       "E_BAD_BODY"
#define NSQ_E_BAD_MESSAGE_NAME    "E_BAD_MESSAGE"
#define NSQ_E_PUB_FAILED_NAME     "E_PUB_FAILED"
#define NSQ_E_MPUB_FAILED_NAME    "E_MPUB_FAILED"
#define NSQ_E_FIN_FAILED_NAME     "E_FIN_FAILED"
#define NSQ_E_REQ_FAILED_NAME     "E_REQ_FAILED"
#define NSQ_E_TOUCH_FAILED_NAME   "E_TOUCH_FAILED"
#define NSQ_E_AUTH_FAILED_NAME    "E_AUTH_FAILED "
#define NSQ_E_UNAUTHORIZED_NAME   "E_UNAUTHORIZED"

#define NSQ_ERROR_ENUM_MAPPER(_)                            \
    _(NSQ_E_OK,             0,  NSQ_E_OK_NAME),             \
    _(NSQ_E_CLOSE_WAIT,     1,  NSQ_E_CLOSE_WAIT_NAME),     \
    _(NSQ_E_ERROR,          2,  NSQ_E_ERROR_NAME),          \
    _(NSQ_E_INVALID,        2,  NSQ_E_INVALID_NAME),        \
    _(NSQ_E_BAD_TOPIC,      3,  NSQ_E_BAD_TOPIC_NAME),      \
    _(NSQ_E_BAD_CHANNEL,    4,  NSQ_E_BAD_CHANNEL_NAME),    \
    _(NSQ_E_BAD_BODY,       5,  NSQ_E_BAD_BODY_NAME),       \
    _(NSQ_E_BAD_MESSAGE,    6,  NSQ_E_BAD_MESSAGE_NAME),    \
    _(NSQ_E_PUB_FAILED,     7,  NSQ_E_PUB_FAILED_NAME),     \
    _(NSQ_E_MPUB_FAILED,    8,  NSQ_E_MPUB_FAILED_NAME),    \
    _(NSQ_E_FIN_FAILED,     9,  NSQ_E_FIN_FAILED_NAME),     \
    _(NSQ_E_REQ_FAILED,     10, NSQ_E_REQ_FAILED_NAME),     \
    _(NSQ_E_TOUCH_FAILED,   11, NSQ_E_TOUCH_FAILED_NAME),   \
    _(NSQ_E_AUTH_FAILED,    12, NSQ_E_AUTH_FAILED_NAME),    \
    _(NSQ_E_UNAUTHORIZED,   13, NSQ_E_UNAUTHORIZED_NAME),   \
    /* end */
DECLARE_ENUM(nsq_error, NSQ_ERROR_ENUM_MAPPER, NSQ_E_END);

static inline bool is_good_nsq_error(int id);
static inline char *nsq_error_string(int id);
static inline int nsq_error_idx(char *name);

static inline bool is_valid_nsq_error(int id)
{
    return IS_GOOD_VALUE(id, NSQ_E_ERROR, NSQ_E_END);
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
#define NSQ_FRAME_ENUM_MAPPER(_)                \
    _(NSQ_FRAME_RESPONSE,   0,  "response"),    \
    _(NSQ_FRAME_ERROR,      1,  "error"),       \
    _(NSQ_FRAME_MESSAGE,    2,  "message"),     \
    /* end */
DECLARE_ENUM(nsq_frame, NSQ_FRAME_ENUM_MAPPER, NSQ_FRAME_END);

static inline bool is_good_nsq_frame(int id);
static inline char *nsq_frame_string(int id);
static inline int nsq_frame_idx(char *name);

#define NSQ_FRAME_RESPONSE  NSQ_FRAME_RESPONSE
#define NSQ_FRAME_ERROR     NSQ_FRAME_ERROR
#define NSQ_FRAME_MESSAGE   NSQ_FRAME_MESSAGE
#define NSQ_FRAME_END       NSQ_FRAME_END

#define is_nsq_frame_response(_code)    (NSQ_FRAME_RESPONSE==(_code))
#define is_nsq_frame_error(_code)       (NSQ_FRAME_ERROR==(_code))
#define is_nsq_frame_message(_code)     (NSQ_FRAME_MESSAGE==(_code))
#endif

typedef struct {
    /*
    * msg size
    *   include body size
    *   not include the 'size' self
    */
    uint32 size;
    uint32 type;

    uint64 timestamp;
    uint16 attempts;
    byte id[NSQ_MSGID_SIZE];

    char body[0];
} nsq_msg_t;

static inline void
nsq_msg_ntoh(nsq_msg_t *msg)
{
    msg->size       = ntohl(msg->size);
    msg->type       = ntohl(msg->type);
    msg->timestamp  = ntohll(msg->timestamp);
    msg->attempts   = ntohs(msg->attempts);
}
#define nsq_msg_hton(_msg)  nsq_msg_ntoh(_msg)

static inline uint32
nsq_msg_body_size(nsq_msg_t *msg)
{
    return msg->size - (sizeof(nsq_msg_t) - sizeof(uint32));
}

static inline bool
is_nsq_response_heartbeat(nsq_msg_t *msg)
{
    return os_streq(msg->body, NSQ_MSG_HEARTBEAT);
}

static inline bool
is_nsq_response_ok(nsq_msg_t *msg)
{
    int error = nsq_error_idx(msg->body);

    return is_good_nsq_error(error) && NSQ_E_OK==error;
}

static inline bool
is_nsq_response_error(nsq_msg_t *msg)
{
    int error = nsq_error_idx(msg->body);

    return is_good_nsq_error(error) && NSQ_E_OK!=error;
}

#define nsq_msg_dump(_msg, _dump) \
    _dump("size=%d, type=%s, timestamp=%llu, attempts=%d, body=%s", \
        (_msg)->size,       \
        nsq_frame_string((_msg)->type), \
        (_msg)->timestamp,  \
        (_msg)->attempts,   \
        (_msg)->body)       \
        /* end */

typedef simple_buffer_t nsq_buffer_t;

#define NSQ_INSTRUCTION_MAGIC       "  V2"
#define NSQ_INSTRUCTION_IDENTIFY    "IDENTIFY\n"
#define NSQ_INSTRUCTION_AUTH        "AUTH\n"
#define NSQ_INSTRUCTION_NOP         "NOP\n"
#define NSQ_INSTRUCTION_CLS         "CLS\n"
#define NSQ_INSTRUCTION_SUB         "SUB "
#define NSQ_INSTRUCTION_PUB         "PUB "
#define NSQ_INSTRUCTION_MPUB        "MPUB "
#define NSQ_INSTRUCTION_RDY         "RDY "
#define NSQ_INSTRUCTION_FIN         "FIN "
#define NSQ_INSTRUCTION_REQ         "REQ "
#define NSQ_INSTRUCTION_TOUCH       "TOUCH "

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

    err = nsqb_command(b, NSQ_INSTRUCTION_MAGIC);
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
    
    err = nsqb_command(b, NSQ_INSTRUCTION_CLS);
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

    err = nsqb_command(b, NSQ_INSTRUCTION_IDENTIFY);
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
    
    err = nsqb_sprintf(b, NSQ_INSTRUCTION_SUB "%s %s\n", topic, channel);
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

    err = nsqb_sprintf(b, NSQ_INSTRUCTION_PUB "%s\n", topic);
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

    err = nsqb_sprintf(b, NSQ_INSTRUCTION_MPUB "%s\n", topic);
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
    
    err = nsqb_sprintf(b, NSQ_INSTRUCTION_RDY "%u\n", count);
    if (err<0) {
        return err;
    }

    return 0;
}

static inline int
nsqb_FIN(nsq_buffer_t *b, byte *msg_id)
{
    int err;
    
    nsqb_clean(b);

    err = nsqb_command(b, NSQ_INSTRUCTION_FIN);
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
nsqb_REQ(nsq_buffer_t *b, byte *msg_id, int timeout)
{
    int err;
    
    nsqb_clean(b);

    err = nsqb_command(b, NSQ_INSTRUCTION_REQ);
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

    err = nsqb_command(b, NSQ_INSTRUCTION_REQ);
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
    
    err = nsqb_command(b, NSQ_INSTRUCTION_NOP);
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

    err = nsqb_command(b, NSQ_INSTRUCTION_AUTH);
    if (err<0) {
        return err;
    }

    err = nsqb_data(b, secret, len);
    if (err<0) {
        return err;
    }

    return 0;
}

static inline int
nsqb_send(int fd, nsq_buffer_t *b)
{
    return io_send(fd, b->buf, b->len);
}

static inline int
nsqb_recv(int fd, nsq_buffer_t *b)
{
    nsq_msg_t *msg;
    int err, len;

    nsqb_clean(b);

    err = len = __io_recv(fd, b->buf, sizeof(nsq_msg_t), 0);
    if (err<0) {
        return err;
    }
    msg = (nsq_msg_t *)b->buf;
    nsq_msg_ntoh(msg);
    if (len!=msg->size) {
        return -ETOOSMALL;
    }
    b->len += len;

    err = len = __io_recv(fd, b->buf + b->len, nsq_msg_body_size(msg), 0);
    if (err<0) {
        return err;
    }
    if (len!=msg->size) {
        return -ETOOSMALL;
    }
    b->len += len;
    b->buf[b->len] = 0;

    nsq_msg_dump(msg, debug_proto);
    
    return 0;
}
/******************************************************************************/
#endif /* __NSQ_H_700ea21373ce4d74a3027b48b6f332c8__ */
