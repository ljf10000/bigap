#ifndef __RSH_H_cd4ac08f199c4732a4decf3ae976b791__
#define __RSH_H_cd4ac08f199c4732a4decf3ae976b791__
/******************************************************************************/
#include "utils.h"
/******************************************************************************/
#ifndef RSH_VERSION
#define RSH_VERSION                 1
#endif

#ifndef RSH_TICKS
#define RSH_TICKS                   5000 /* ms */
#endif

#ifndef ENV_RSH_CONFIG
#define ENV_RSH_CONFIG              "__RSH_CONFIG__"
#endif

#ifndef ENV_RSH_CLOUD_CONFIG
#define ENV_RSH_CLOUD_CONFIG        "__RSH_CLOUD_CONFIG__"
#endif

#ifndef ENV_RSH_SLOT_LOCAL
#define ENV_RSH_SLOT_LOCAL          "__RSH_SLOT_LOCAL__"
#endif

#ifndef ENV_RSH_SLOT_MASTER
#define ENV_RSH_SLOT_MASTER         "__RSH_SLOT_MASTER__"
#endif

#ifndef ENV_RSH_REQUEST_SCRIPT
#define ENV_RSH_REQUEST_SCRIPT      "__RSH_REQUEST__"
#endif

#ifndef ENV_RSH_RESPONSE_SCRIPT
#define ENV_RSH_RESPONSE_SCRIPT     "__RSH_RESPONSE__"
#endif

#ifndef RSH_SLOT_COUNT
#define RSH_SLOT_COUNT              2
#endif

#define RSH_SLOT_MULTI              (RSH_SLOT_COUNT>1)

#ifndef RSH_PORT
#define RSH_PORT                    8740
#endif

#ifdef __PC__
#define RSH_CFG_PREFIX              ""
#define RSH_BIN_PREFIX              ""
#else
#define RSH_CFG_PREFIX              "/tmp/config/"
#define RSH_BIN_PREFIX              "/usr/sbin/"
#endif

#ifndef RSH_CONFIG_FILE
#define RSH_CONFIG_FILE             RSH_CFG_PREFIX "rsh.config"
#endif

#ifndef RSH_CLOUD_CONFIG_FILE
#define RSH_CLOUD_CONFIG_FILE       RSH_CFG_PREFIX "rsh.cloud"
#endif

#ifndef RSH_SLOT_FILE
#define RSH_SLOT_FILE               RSH_CFG_PREFIX "rsh.%d.slot"
#endif

#ifndef RSH_REQUEST_SCRIPT
#define RSH_REQUEST_SCRIPT          RSH_BIN_PREFIX "rsh.request"
#endif

#ifndef RSH_RESPONSE_SCRIPT
#define RSH_RESPONSE_SCRIPT         RSH_BIN_PREFIX "rsh.response"
#endif

#define RSH_SLOT_MASTER         0
#define RSH_SLOT_END            RSH_SLOT_COUNT
#define RSH_SLOT_CLOUD          RSH_SLOT_COUNT

#define rsh_foreach_slot(_slot) for(_slot=0; _slot<RSH_SLOT_END; _slot++)

static inline bool
is_local_rsh_slot(int slot)
{
    return IS_GOOD_ENUM(slot, RSH_SLOT_CLOUD);
}

#ifndef RSH_ECHO_INTERVAL
#define RSH_ECHO_INTERVAL       (60*1000/RSH_TICKS)
#endif

#ifndef RSH_ECHO_TIMES
#define RSH_ECHO_TIMES          3
#endif
/******************************************************************************/
/*
cmd:command/echo
mode:asyn/syn/ack/reflect
url:
    http(s)://domain:port/service
    tcp://domain:port

request(echo):  agent==>cloud
response(echo): agent<==cloud
{
    "version": VERSION,
    "mac":"MAC",
    "cmd":"echo",
}

request(shell, asyn): 
    agent<==cloud
    rshc==>xinetd
{
    "version": VERSION,
    "mac":"MAC",
    "slot":SLOT,
    "cmd":"command",
    "mode":"asyn",
    "seq":SEQ,
    "stdin":"STDIN"
}

request(shell, ack): 
    agent<==cloud
    rshc==>xinetd
{
    "version": VERSION,
    "mac":"MAC",
    "slot":SLOT,
    "cmd":"command",
    "mode":"asyn",
    "seq":SEQ,
    "stdin":"STDIN"
}

response(shell, ack): 
    agent==>cloud
    rshc<==xinetd
{
    "version": VERSION,
    "mac":"MAC",
    "slot":SLOT,
    "cmd":"command",
    "mode":"ack",
    "seq":SEQ,
    "stdin":"STDIN"
}

request(shell, reflect): agent<==cloud
{
    "version": VERSION,
    "mac":"MAC",
    "slot":SLOT,
    "cmd":"command",
    "mode":"syn",
    "seq":SEQ,
    "stdin":"STDIN",
    "redirect": {
        "url":"URL"
    }
}

response(shell, reflect): xinetd==>cloud
{
    "version": VERSION,
    "mac":"MAC",
    "slot":SLOT,
    "cmd":"command",
    "mode":"syn",
    "seq":SEQ,
    "stdin":"STDIN",
    "redirect": {
        "url":"URL"
    },
    
    "stdout":"STDOUT",
    "stderr":"STDERR",
    "errno":ERRNO
}

request(shell, syn): rshc==>xinetd
{
    "version": VERSION,
    "mac":"MAC",
    "slot":SLOT,
    "cmd":"command",
    "mode":"syn",
    "seq":SEQ,
    "stdin":"STDIN",
}

response(shell, syn): rshc<==xinetd
{
    "version": VERSION,
    "mac":"MAC",
    "slot":SLOT,
    "cmd":"command",
    "mode":"syn",
    "seq":SEQ,
    "stdin":"STDIN",

    "stdout":"STDOUT",
    "stderr":"STDERR",
    "errno":ERRNO
}

echo: cloud==(echo-request)==>agent==(echo-response)==>cloud
asyn: cloud==(asyn-request)==>agent==(asyn-request)==>xinetd
                                |
                                |
                                ==(asyn-request)==>fork
reflect:cloud==(reflect-request)==>agent==(reflect-request)==>xinetd==(reflect-response)==>cloud
                                |
                                |
                                ==(reflect-request)==>fork==(reflect-response)==>cloud
syn: rshc==(syn-request)==>xinetd==(syn-response)==>rshc
*/

#define RSH_ECHO_REQUEST    "{\"version\":%d, \"mac\":\"%s\", \"cmd\":\"echo\"}"

#define __XLIST_RSH_CMD(_)              \
    _(RSH_CMD_COMMAND,  0, "command"),  \
    _(RSH_CMD_ECHO,     1, "echo"),     \
    /* end */

static inline bool is_good_rsh_cmd(int cmd);
static inline char *rsh_cmd_string(int cmd);
static inline int rsh_cmd_idx(char *cmd_string);
DECLARE_ENUM(rsh_cmd, __XLIST_RSH_CMD, RSH_CMD_END);

#if 1 /* just for sourceinsight */
#define RSH_CMD_COMMAND     RSH_CMD_COMMAND
#define RSH_CMD_ECHO        RSH_CMD_ECHO
#define RSH_CMD_END         RSH_CMD_END
#endif /* just for sourceinsight */

#define __XLIST_RSH_MODE(_)             \
    _(RSH_MODE_SYN,     0, "syn"),      \
    _(RSH_MODE_ACK,     1, "ack"),      \
    _(RSH_MODE_ASYN,    2, "asyn"),     \
    _(RSH_MODE_REFLECT, 3, "reflect"),  \
    /* end */

static inline bool is_good_rsh_mode(int mode);
static inline char *rsh_mode_string(int mode);
static inline int rsh_mode_idx(char *mode_string);
DECLARE_ENUM(rsh_mode, __XLIST_RSH_MODE, RSH_MODE_END);

#if 1 /* just for sourceinsight */
#define RSH_MODE_SYN        RSH_MODE_SYN
#define RSH_MODE_ACK        RSH_MODE_ACK
#define RSH_MODE_ASYN       RSH_MODE_ASYN
#define RSH_MODE_REFLECT    RSH_MODE_REFLECT
#define RSH_MODE_END        RSH_MODE_END
#endif /* just for sourceinsight */
/******************************************************************************/
#endif /* __RSH_H_cd4ac08f199c4732a4decf3ae976b791__ */
