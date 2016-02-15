#ifndef __BIG_H_f0a91ca14381483387183497067dce26__
#define __BIG_H_f0a91ca14381483387183497067dce26__
/******************************************************************************/
typedef struct {
    byte mac[OS_MACSIZE];
    pid_t pid;
    time_t time;
    uint32_t rand;
} bigap_token_t;

#define AP_MODULE_LEN   31

struct bigap_domain;

struct bigap_group {
    struct {
        struct {
            struct list_head list;
            int count;
        } group, ap;
    } head;

    char *name;
    struct bigap_domain *domain;
};

struct bigap_domain {
    struct {
        struct {
            struct list_head list;
            int count;
        } group;
    } head;

    char *domain;
};

struct acbase {
    char *name;
    bigap_token_t token;
    uint32_t ip;
};

struct apbase {
    byte mac[OS_MACSIZE];
    char *group;
    char *domain;
    char *module;
    bigap_token_t token;

    struct acbase ac_last, ac_now;
};

struct ap {
    
};

#define AC_APMAC_HASHSIZE       1024
#define AC_APIPPORT_HASHSIZE    1024
#define AC_APDOMAIN_HASHSIZE    32
#define AC_APGROUP_HASHSIZE     128

struct ac {
    struct {
        struct {
            struct hlist_head mac[AC_APMAC_HASHSIZE];
            struct hlist_head ipport[AC_APIPPORT_HASHSIZE];
            struct hlist_head domain[AC_APDOMAIN_HASHSIZE];
            struct hlist_head group[AC_APGROUP_HASHSIZE];
            struct list_head list;
            uint32_t count;
        } ap;
    } head;
    
};

/******************************************************************************/
#endif /* __BIG_H_f0a91ca14381483387183497067dce26__ */
