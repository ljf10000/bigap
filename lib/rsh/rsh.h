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

#ifndef RSH_AES_BITS
#define RSH_AES_BITS                256
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
    _(RSH_CMD_UNKNOW,   0, "unknow"),   \
    _(RSH_CMD_ECHO,     1, "echo"),     \
    _(RSH_CMD_COMMAND,  2, "command"),  \
    /* end */
DECLARE_ENUM(rsh_cmd, RSH_CMD_ENUM_MAPPER, RSH_CMD_END);

static inline enum_ops_t *rsh_cmd_ops(void);
static inline bool is_good_rsh_cmd(int id);
static inline char *rsh_cmd_getnamebyid(int id);
static inline int rsh_cmd_getidbyname(const char *name);

#define RSH_CMD_UNKNOW      RSH_CMD_UNKNOW
#define RSH_CMD_ECHO        RSH_CMD_ECHO
#define RSH_CMD_COMMAND     RSH_CMD_COMMAND
#define RSH_CMD_END         RSH_CMD_END

static inline bool is_valid_rsh_cmd(int id)
{
    return is_good_value(id, RSH_CMD_ECHO, RSH_CMD_END);
}

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
    
    char body[0];
} rsh_msg_t;        // 32 == sizeof(rsh_msg_t)

#define RSH_MSG_SIZE(_bodysize)  ((_bodysize) + sizeof(rsh_msg_t))

enum {
    RSH_MSG_BODYSIZE    = 1408, // 32*44
    RSH_MSG_ALLSIZE     = RSH_MSG_SIZE(RSH_MSG_BODYSIZE),
};

static inline bool
is_good_rsh_msg_size(int size)
{
    return size > 0 && 0 == (size % AES_BLOCK_SIZE);
}

static inline int
rsh_msg_size(rsh_msg_t *msg)
{
    return (int)RSH_MSG_SIZE(msg->len);
}

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
rsh_msg_bfill(rsh_msg_t *msg, byte mac[], void *buf, int len)
{
    os_objzero(msg);
    
    len = buf?len:0;

    msg->len = AES_ALIGN(len);
    if (msg->len > RSH_MSG_BODYSIZE) {
        return -ETOOBIG;
    }

    msg->magic      = RSH_MAGIC;
    msg->version    = RSH_VERSION;
    msg->_r0        = 0;
    msg->_r1        = 0;
    os_maccpy(msg->mac, mac);
    os_memcpy(msg->body, buf, len);
    os_memzero(msg->body + len, len % AES_BLOCK_SIZE);
    
    return 0;
}

static inline int
rsh_msg_sfill(rsh_msg_t *msg, byte mac[], char *string)
{
    return rsh_msg_bfill(msg, mac, string, os_strlen(string));
}

static inline int
rsh_msg_zfill(rsh_msg_t *msg, byte mac[])
{
    return rsh_msg_bfill(msg, mac, NULL, 0);
}


enum {
    RSH_E_PACKET,
    RSH_E_SERVER,
    
    RSH_E_OK,
    RSH_E_CMD,
    RSH_E_LEN,
    RSH_E_FLAG,
    RSH_E_ACK,
    RSH_E_MAC,
    RSH_E_MAGIC,
    RSH_E_VERSION,

    RSH_E_END
};
/******************************************************************************/
#endif /* __RSH_H_cd4ac08f199c4732a4decf3ae976b791__ */
