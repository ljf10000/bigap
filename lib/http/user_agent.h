#ifndef __USER_AGENT_H_ff5b9069118d4865af611eb53e78ddfd__
#define __USER_AGENT_H_ff5b9069118d4865af611eb53e78ddfd__
/******************************************************************************/
enum {
    UA_ANDROID,
    UA_IOS,
    UA_WINDOWS,
    UA_UNKNOW,
    
    UA_END
};

static inline int
is_good_user_agent(int os)
{
    return os>=0 && os<UA_END;
}

#define UA_STRINGS              {   \
    [UA_ANDROID]    = "android",    \
    [UA_IOS]        = "ios",        \
    [UA_WINDOWS]    = "windows",    \
    [UA_UNKNOW]     = "unknow",     \
}   /* end */

static inline char *
user_agent_string(int os)
{
    static char *array[UA_END] = UA_STRINGS;
    
    return is_good_user_agent(os)?array[os]:"unknow";
}

static inline int
user_agent_get(char *agent)
{
    static struct {
        int os;
        char *ua;
    } ua[] = {
        { UA_ANDROID,   "Android"   },
        { UA_ANDROID,   "android"   },
        { UA_IOS,       "iPhone"    },
        { UA_IOS,       "iphone"    },
        { UA_IOS,       "iPad"      },
        { UA_IOS,       "ipad"      },
        { UA_WINDOWS,   "Windows"   },
        { UA_WINDOWS,   "windows"   },
    };

    int i;
    for (i=0; i<sizeof(ua)/sizeof(ua[0]); i++) {
        if (strstr(agent, ua[i].ua)) {
            return ua[i].os;
        }
    }

    return UA_UNKNOW;
}

/******************************************************************************/
#endif /* __USER_AGENT_H_ff5b9069118d4865af611eb53e78ddfd__ */
