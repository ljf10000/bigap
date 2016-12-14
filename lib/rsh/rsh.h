#ifndef __RSH_H_cd4ac08f199c4732a4decf3ae976b791__
#define __RSH_H_cd4ac08f199c4732a4decf3ae976b791__
/******************************************************************************/
#include "utils.h"
/******************************************************************************/
#ifndef RSH_VERSION
#define RSH_VERSION                 1
#endif

#ifndef RSH_MAGIC
#define RSH_MAGIC                   0x7765
#endif

#ifndef RSH_SEQ_UNKNOW
#define RSH_SEQ_UNKNOW              0
#endif
/******************************************************************************/
/*
    1. ping
        agent==>proxy
        cmd(echo) flag(syn)

    2. pong
        agent<==proxy
        cmd(echo) flag(ack)

    3. command-syn-request
        agent<==proxy
        cmd(command) flag(syn)

    4. command-syn-ack
        agent==>proxy
        cmd(command) flag(ack)

    5. command-asyn-request
        agent<==proxy
        cmd(command) flag(0)
*/
#if 1
#define RSH_CMD_ENUM_MAPPER(_)          \
    _(RSH_CMD_ECHO,     0, "echo"),     \
    _(RSH_CMD_COMMAND,  1, "command"),  \
    /* end */
DECLARE_ENUM(rsh_cmd, RSH_CMD_ENUM_MAPPER, RSH_CMD_END);

static inline enum_ops_t *rsh_cmd_ops(void);
static inline bool is_good_rsh_cmd(int id);
static inline char *rsh_cmd_getnamebyid(int id);
static inline int rsh_cmd_getidbyname(const char *name);

#define RSH_CMD_ECHO        RSH_CMD_ECHO
#define RSH_CMD_COMMAND     RSH_CMD_COMMAND
#define RSH_CMD_END         RSH_CMD_END
#endif

enum {
    RSH_F_SYN   = 0x01, // need ack
    RSH_F_ACK   = 0x02, // is ack
};

typedef struct {
    uint16 magic;
    byte version;
    byte cmd;
    
    byte mac[OS_MACSIZE];
    uint16 len;     // NOT include rsh header
    
    uint16 flag;
    uint16 _r0;     // must zero
    
    uint32 seq;
    uint32 ack;
    
    int32 error;
    uint32 _r1;     // must zero
    
    byte body[0];
} rsh_msg_t;        // 32 == sizeof(rsh_msg_t)

enum {
    RSH_MSG_BODYSIZE    = 1408, // 32*44
    RSH_MSG_ALLSIZE     = RSH_MSG_BODYSIZE + sizeof(rsh_msg_t),
};

#define rsh_msg_random(_msg, _idx)  (_msg)->h[_idx].random
#define rsh_msg_mac(_msg, _idx)     (_msg)->h[_idx].mac
#define rsh_msg_value(_msg, _idx)   (_msg)->h[_idx].value
#define rsh_msg_magic(_msg)         rsh_msg_value(_msg, RSH_H_MAGIC)
#define rsh_msg_version(_msg)       rsh_msg_value(_msg, RSH_H_VERSION)
#define rsh_msg_cmd(_msg)           rsh_msg_value(_msg, RSH_H_CMD)
#define rsh_msg_length(_msg)        rsh_msg_value(_msg, RSH_H_LENGTH)
#define rsh_msg_flag(_msg)          rsh_msg_value(_msg, RSH_H_FLAG)
#define rsh_msg_resv(_msg)          rsh_msg_value(_msg, RSH_H_RESV)
#define rsh_msg_body32(_msg)        ((uint32 *)(_msg)->body)

static inline void
rsh_msg_hton(rsh_msg_t *msg)
{
    msg->magic  = htons(msg->magic);
    msg->len    = htons(msg->len);
    msg->flag   = htons(msg->flag);
    msg->_r0    = htons(msg->_r0);
    
    msg->seq    = htonl(msg->seq);
    msg->ack    = htonl(msg->ack);
    msg->error  = htonl(msg->error);
    msg->_r1    = htonl(msg->_r1);
}
#define rsh_msg_ntoh(_msg)  rsh_msg_hton(_msg)

static inline int
rsh_msg_fill(rsh_msg_t *msg, void *buf, int len)
{
    int i;
    
    if (len > RSH_MSG_BODYSIZE) {
        return -ETOOBIG;
    }

    rsh_msg_length(msg) = len;
    os_memcpy(msg->body, buf, len);

    rsh_msg_magic(msg)      = RSH_MAGIC;
    rsh_msg_version(msg)    = RSH_VERSION;

    return 0;
}
/******************************************************************************/
#endif /* __RSH_H_cd4ac08f199c4732a4decf3ae976b791__ */
