#ifndef __AK_H_2e420d20ac8a47b188d92ef8448db5fa__
#define __AK_H_2e420d20ac8a47b188d92ef8448db5fa__
/******************************************************************************/
#ifndef OS_APPNAMELEN
#define OS_APPNAMELEN           (32 - sizeof(uint32_t) - 1)
#endif

#ifndef OS_AKNAME_LEN
#define OS_AKNAME_LEN           (32 - 1)
#endif

#ifndef AK_PATH
#define AK_PATH                 "/tmp/appkey"
#endif

#ifndef AK_PATH_DEFT
#define AK_PATH_DEFT            "/etc/appkey"
#endif

#ifndef AK_LIMIT
#define AK_LIMIT                1023
#endif

#define AK_DEBUG_NAME           "debug"
#define INVALID_AKID            0

#ifndef AK_DPRINT
#define AK_DPRINT               0
#endif

#if AK_DPRINT
#define ak_println(_fmt, _args...)  printf(_fmt "\n", ##_args)
#else
#define ak_println(_fmt, _args...)  os_do_nothing()
#endif

typedef uint32_t akid_t;

#if defined(__BOOT__)
#    define DECLARE_FAKE_DEBUGGER    extern akid_t *__THIS_DEBUG
#    define DECLARE_REAL_DEBUGGER    akid_t *__THIS_DEBUG
#    define DECLARE_DEBUGGER         DECLARE_REAL_DEBUGGER;
#else
#    define DECLARE_FAKE_DEBUGGER    extern akid_t __THIS_DEBUG
#    define DECLARE_REAL_DEBUGGER    akid_t __THIS_DEBUG
#    ifdef __BUSYBOX__
#        define DECLARE_DEBUGGER     DECLARE_FAKE_DEBUGGER
#    else
#        define DECLARE_DEBUGGER     DECLARE_REAL_DEBUGGER
#    endif
#endif

DECLARE_FAKE_DEBUGGER;
/******************************************************************************/
#define __AK_DEBUG_LIST(_)                  \
    _(____ak_debug_ok,      0, "ok"),       \
    _(____ak_debug_bug,     1, "bug"),      \
    _(____ak_debug_error,   2, "error"),    \
    _(____ak_debug_trace,   3, "trace"),    \
    _(____ak_debug_init,    4, "init"),     \
    _(____ak_debug_config,  5, "config"),   \
    _(____ak_debug_lib,     6, "lib"),      \
    _(____ak_debug_io,      7, "io"),       \
    _(____ak_debug_timer,   8, "timer"),    \
    _(____ak_debug_timeout, 9, "timeout"),  \
    _(____ak_debug_aging,   10,"aging"),    \
    _(____ak_debug_event,   11,"event"),    \
    _(____ak_debug_entry,   12,"entry"),    \
    _(____ak_debug_packet,  13,"packet"),   \
    _(____ak_debug_flow,    14,"flow"),     \
    _(____ak_debug_signal,  15,"signal"),   \
    _(____ak_debug_shell,   16,"shell"),    \
    _(____ak_debug_st,      17,"st"),       \
    _(____ak_debug_test,    18,"test"),     \
    /* end */

static inline bool is_good_ak_DEBUG(int id);
static inline char *ak_DEBUG_string(int id);
static inline int ak_DEBUG_idx(char *name);
DECLARE_ENUM(ak_DEBUG, __AK_DEBUG_LIST, ____ak_debug_end);

enum {
    __ak_debug_ok       = os_bit(____ak_debug_ok),
    __ak_debug_bug      = os_bit(____ak_debug_bug),
    __ak_debug_error    = os_bit(____ak_debug_error),
    __ak_debug_trace    = os_bit(____ak_debug_trace),
    __ak_debug_init     = os_bit(____ak_debug_init),
    __ak_debug_config   = os_bit(____ak_debug_config),
    __ak_debug_lib      = os_bit(____ak_debug_lib),
    __ak_debug_io       = os_bit(____ak_debug_io),
    __ak_debug_timer    = os_bit(____ak_debug_timer),
    __ak_debug_timeout  = os_bit(____ak_debug_timeout),
    __ak_debug_aging    = os_bit(____ak_debug_aging),
    __ak_debug_event    = os_bit(____ak_debug_event),
    __ak_debug_entry    = os_bit(____ak_debug_entry),
    __ak_debug_packet   = os_bit(____ak_debug_packet),
    __ak_debug_flow     = os_bit(____ak_debug_flow),
    __ak_debug_signal   = os_bit(____ak_debug_signal),
    __ak_debug_shell    = os_bit(____ak_debug_shell),
    __ak_debug_st       = os_bit(____ak_debug_st),
    __ak_debug_test     = os_bit(____ak_debug_test),
};

#define __ak_debug_all      os_mask(____ak_debug_end)

#ifndef __ak_debug_default
#define __ak_debug_default  (__ak_debug_error | __ak_debug_bug)
#endif

static inline uint32_t
__ak_debug_getbyname(char *name)
{
    int idx = ak_DEBUG_idx(name);

    return os_bit(idx);
}

static inline char *
__ak_debug_getname(uint32_t level)
{
    int idx = os_bitshift(level);

    return ak_DEBUG_string(idx);
}

#if defined(__BOOT__)
#   define __ak_debug        (__THIS_DEBUG?(*__THIS_DEBUG):__ak_debug_default)
#elif defined(__APP__) && defined(__DEAMON__)
#   define __ak_debug        ak_get(__THIS_DEBUG, __ak_debug_default)
#else
#   define __ak_debug        __THIS_DEBUG
#endif

#define __is_ak_debug(_level)   (os_hasflag(__ak_debug, _level))
#define __is_ak_debug_ok        __is_ak_debug(__ak_debug_ok)
#define __is_ak_debug_bug       __is_ak_debug(__ak_debug_bug)
#define __is_ak_debug_error     __is_ak_debug(__ak_debug_error)
#define __is_ak_debug_trace     __is_ak_debug(__ak_debug_trace)
#define __is_ak_debug_init      __is_ak_debug(__ak_debug_init)
#define __is_ak_debug_config    __is_ak_debug(__ak_debug_config)
#define __is_ak_debug_lib       __is_ak_debug(__ak_debug_lib)
#define __is_ak_debug_io        __is_ak_debug(__ak_debug_io)
#define __is_ak_debug_timer     __is_ak_debug(__ak_debug_timer)
#define __is_ak_debug_timeout   __is_ak_debug(__ak_debug_timeout)
#define __is_ak_debug_aging     __is_ak_debug(__ak_debug_aging)
#define __is_ak_debug_event     __is_ak_debug(__ak_debug_event)
#define __is_ak_debug_entry     __is_ak_debug(__ak_debug_entry)
#define __is_ak_debug_packet    __is_ak_debug(__ak_debug_packet)
#define __is_ak_debug_flow      __is_ak_debug(__ak_debug_flow)
#define __is_ak_debug_signal    __is_ak_debug(__ak_debug_signal)
#define __is_ak_debug_shell     __is_ak_debug(__ak_debug_shell)
#define __is_ak_debug_st        __is_ak_debug(__ak_debug_st)
#define __is_ak_debug_test      __is_ak_debug(__ak_debug_test)
/******************************************************************************/
/*
* enable app debug(use share memory)
*/
#if defined(__APP__) && defined(__DEAMON__)

typedef struct {
    char app[1+OS_APPNAMELEN];
    char k[1+OS_AKNAME_LEN];
    uint32_t v;
} ak_t;

typedef struct {
    uint32_t protect;
    bool inited;
    int count;
    int limit;
    ak_t entry[0];
} ak_hdr_t;

typedef union {
    akid_t akid;

    struct {
        uint16_t idx;
        uint16_t offset;
    } v;
} ak_u;

#define __AKU_MAKE(_idx, _offset)    {  \
    .v = {                              \
        .idx = (uint16_t)(_idx),        \
        .offset = (uint16_t)(_offset),  \
    }                                   \
}

#define __AKU_INITER(_akid)         { .akid = _akid }

static inline akid_t
__ak_make(int idx, int offset)
{
    ak_u aku = __AKU_MAKE(idx, offset);

    return aku.akid;
}

static inline int
__ak_idx(akid_t akid)
{
    ak_u aku = __AKU_INITER(akid);

    return (int)aku.v.idx;
}

static inline int
__ak_offset(akid_t akid)
{
    ak_u aku = __AKU_INITER(akid);

    return (int)aku.v.offset;
}

#define appkey_shm_size     (0  \
    + sizeof(ak_hdr_t)          \
    + sizeof(ak_t) * AK_LIMIT   \
    + sizeof(uint32_t)          \
)   /* end */

enum {
    __AK_SYS_DEBUG,

    __AK_SYS_END
};

typedef struct {
    os_shm_t shm;
    char *sys[__AK_SYS_END];
} ak_shm_t;

#define DECLARE_FAKE_AK  extern ak_shm_t __THIS_AK
#define DECLARE_REAL_AK  ak_shm_t __THIS_AK = { \
    .shm = OS_SHM_INITER(OS_AK_SHM_ID),         \
    .sys = {                                    \
        [__AK_SYS_DEBUG]  = AK_DEBUG_NAME,      \
    },                                          \
}

#ifdef __BUSYBOX__
#define DECLARE_AK      DECLARE_FAKE_AK
#else
#define DECLARE_AK      DECLARE_REAL_AK
#endif
DECLARE_FAKE_AK;

static inline int 
__ak_sys(char *k)
{
    int i;

    for (i=0; i<__AK_SYS_END; i++) {
        if (0==os_strcmp(k, __THIS_AK.sys[i])) {
            return i;
        }
    }

    return -ENOEXIST;
}

static inline ak_hdr_t *
__ak_hdr(void)
{
    return (ak_hdr_t *)(__THIS_AK.shm.address);
}

#define __ak_inited         __ak_hdr()->inited
#define __ak_count          __ak_hdr()->count
#define __ak_limit          __ak_hdr()->limit
#define __ak_entry(_idx)    (&__ak_hdr()->entry[_idx])
#define __ak_0              __ak_entry(0)
#define __ak_end            __ak_entry(__ak_count)
#define __ak_END            __ak_entry(__ak_limit)
#define __ak_foreach(ak)    for (ak=__ak_0; ak<__ak_end; ak++)
#define __ak_protect_0      __ak_hdr()->protect
#define __ak_protect_1      (*(uint32_t *)__ak_END)

static inline int 
__ak_getidx(ak_t *ak)
{
    return ak - __ak_0;
}

static inline int 
__ak_getoffset(ak_t *ak)
{
    return (char *)ak - (char *)__ak_hdr();
}

static inline ak_t *
__ak_getbyid(akid_t akid)
{
    char *address = (char *)__ak_hdr() + __ak_offset(akid);
    ak_t *ak = NULL;

    if (false==is_good_enum(__ak_idx(akid), __ak_count)) {
        return NULL;
    }
    
    ak = __ak_entry(__ak_idx(akid));

    return (address==(char *)ak)?ak:NULL;
}

static inline ak_t *
__ak_new(char *app, char *k)
{
    if (__ak_count < __ak_limit) {
        int idx = __ak_count++;
        ak_t *ak = __ak_entry(idx);
        
        os_strdcpy(ak->app, app);
        os_strdcpy(ak->k, k);
        
        return ak;
    } else {
        ak_println("count(%u) >= limit(%u)", __ak_count, __ak_limit);
        
        return NULL;
    }
}

static inline ak_t *
____ak_getbyname(char *app, char *k)
{
    ak_t *ak = NULL;
    
    __ak_foreach(ak) {
        if (0==os_stracmp(ak->app, app) && 0==os_stracmp(ak->k, k)) {
            return ak;
        }
    }
    
    return NULL;
}

static inline akid_t 
__ak_getbyname(char *app, char *k)
{
    ak_t *ak;

    if (NULL==app) {
        return os_assertV(INVALID_AKID);
    }
    else if (NULL==k) {
        return os_assertV(INVALID_AKID);
    }

    ak = ____ak_getbyname(app, k);

    return ak?__ak_make(__ak_getidx(ak), __ak_getoffset(ak)):INVALID_AKID;
}

static inline int 
__ak_get(akid_t akid, uint32_t *pv)
{
    ak_t *ak = __ak_getbyid(akid);

    if (NULL==pv) {
        return -EKEYNULL;
    }
    else if (NULL==ak) {
        return -ENOEXIST;
    }
    
    *pv = ak->v;
    
    return 0;
}

struct akinfo {
    char *filename;
    char *line;
    char app[1+OS_LINE_LEN];
    char key[1+OS_LINE_LEN];
    char var[1+OS_LINE_LEN];
    uint32_t v;
};
#define AKINFO_INITER(_filename, _line) {  \
    .filename = _filename, \
    .line = _line, \
}

static inline int
__ak_sys_debug(struct akinfo *info)
{
    char var[1+OS_LINE_LEN] = {0};
    char *name;
    uint32_t v = 0;

    os_strdcpy(var, info->var);
    os_strtok_foreach(name, var, "|") {
        if ('*'==name[0]) {
            v = __ak_debug_all;
        }
        else if ('-'==name[0]) {
            v &= ~ __ak_debug_getbyname(name+1);
        }
        else {
            v |= __ak_debug_getbyname(name);
        }
    }

    info->v = v;
    
    return 0;
}

static inline int
__ak_sys_value(int sys, struct akinfo *info)
{
    /*
    * try "*"
    */
    if ('*'==info->var[0] && 0==info->var[1]) {
        info->v = 0xffffffff;
        
        return 0;
    }
    
    switch(sys) {
        case __AK_SYS_DEBUG:
            return __ak_sys_debug(info);
        default:
            return -EKEYBAD;
    }
}

static inline int
__ak_load_line_app(struct akinfo *info)
{
    int len;

    /*
    * filename's format is "xxx.key"
    *   cannot use os_sscanf(filename, "%s.key", app)
    */
    if (1!=os_sscanf(info->filename, "%[^.]", info->app)) {
        ak_println("read file(%s) bad line(%s)", info->filename, info->line);
        
        return -EFORMAT;
    }
    
    len = os_strlen(info->app);
    if (len >= OS_APPNAMELEN) {
        ak_println("app(%s) length(%d) > %d", info->app, len, OS_APPNAMELEN);
        
        return -ETOOBIG;
    }

    return 0;
}

static inline int
__ak_load_line_kv(struct akinfo *info)
{
    int len;
    
    /* 
    * read key & var 
    */
    if (2!=os_sscanf(info->line, "%s %s", info->key, info->var)) {
        ak_println("read app(%s) bad line(%s)", info->app, info->line);
        
        return -EFORMAT;
    }
    len = os_strlen(info->key);
    if (len >= OS_AKNAME_LEN) {
        ak_println("key(%s) length(%d) > %d", info->key, len, OS_AKNAME_LEN);
        
        return -ETOOBIG;
    }

    return 0;
}

static inline int
__ak_load_line_value(struct akinfo *info)
{
    int sys = INVALID_VALUE;
    char *end = NULL;
    uint32_t v = 0;
    
    /*
    * first, try var as digit string
    */
    v = strtoul(info->var, &end, 0);
    if (NULL==end || 0==end[0]) {
        info->v = v;
        
        return 0;
    }
    
    /*
    * not digit string, try sys
    */
    sys = __ak_sys(info->key);
    if (sys<0) {
        return -EFORMAT;
    } else {
        return __ak_sys_value(sys, info);
    }
}

static inline mv_t 
__ak_load_line(char *filename/* not include path */, char *line)
{
    struct akinfo info = AKINFO_INITER(filename, line);
    ak_t *ak;
    int err;

    ak_println("load file(%s) line(%s)", filename, line);
    
    err = __ak_load_line_app(&info);
    if (err) {
        return mv2_go(err);
    }

    err = __ak_load_line_kv(&info);
    if (err) {
        return mv2_go(err);
    }

    err = __ak_load_line_value(&info);
    if (err) {
        return mv2_go(err);
    }
    
    ak = ____ak_getbyname(info.app, info.key);
    if (NULL==ak) {
        ak = __ak_new(info.app, info.key);
        if (NULL==ak) {
            ak_println("limit(%d)", __ak_limit);
            
            return mv2_break(-ELIMIT);
        }
    }

    ak_println("load %s.%s(0x%x==>0x%x)",
        ak->app,
        ak->k,
        ak->v,
        info.v);
    
    ak->v = info.v;
    
    return mv2_ok;
}

/*
* just handle file "*.key"
*/
static inline bool 
__ak_file_filter(char *path, char *filename)
{
    char *p = os_strchr(filename, '.');
    
    if (p && 0==os_strcmp(p, ".key")) {
        return false;
    } else {
        ak_println("ignore %s", filename);
        
        return true;
    }
}

static inline int 
__ak_load(void) 
{
    int ret = 0;
    char *path;
    
    DIR *dir = opendir(AK_PATH);
    if (dir) {
        closedir(dir);

        path = AK_PATH;
    } else {
        path = AK_PATH_DEFT;
    }
    
    ret = os_fscan_dir(path, false, __ak_file_filter, __ak_load_line);
    if (ret<0) {
        ak_println("load(%s) error%d", path, ret);
        
        return ret;
    }
    
    ak_println("load(%s) ak count=%u, limit=%u",
        path,
        __ak_count,
        __ak_limit);
    
    return 0;
}


static inline void 
__ak_show(void) 
{
    int i;
    
    for (i=0; i<__ak_count; i++) {
        ak_t *ak = __ak_entry(i);
        
        os_println("reload %s.%s=%u",
            ak->app,
            ak->k,
            ak->v);
    }
}

static inline int 
__ak_init(uint32_t limit)
{
    if (false==__ak_inited) {
        __ak_inited   = true;
        __ak_limit    = limit;
        __ak_protect_0 = OS_PROTECTED;
        __ak_protect_1 = OS_PROTECTED;
        
        __ak_load();
    }
    
    return 0;
}

#define ak_getbyname(_key)  __ak_getbyname(__THIS_NAME, _key)

static inline uint32_t
ak_get(akid_t akid, uint32_t deft)
{
    uint32_t v = deft;

    __ak_get(akid, &v);

    return v;
}

static inline int 
ak_set(akid_t akid, uint32_t v)
{
    ak_t *ak = __ak_getbyid(akid);

    if (NULL==ak) {
        return -ENOEXIST;
    }
    
    ak->v = v;
    
    return 0;
}

static inline int 
ak_reload(void)
{
    __ak_load();
    __ak_show();
    
    return 0;
}

static inline int 
ak_fini(void) 
{
    if (INVALID_SHM_ADDR != __THIS_AK.shm.address) {
        shmdt(__THIS_AK.shm.address);
        __THIS_AK.shm.address = INVALID_SHM_ADDR;
        
        ak_println("shm fini shmdt(address:%p)", __THIS_AK.shm.address);
    }

    return 0;
}

static inline int 
ak_init(void) 
{
    int err = 0;

    err = os_shm_create(&__THIS_AK.shm, appkey_shm_size, false);
    if (err<0) { /* >=0 is valid shm id */
        goto error;
    }
    
    __ak_init(AK_LIMIT);

    __THIS_DEBUG = ak_getbyname(AK_DEBUG_NAME);
    
    ak_println("init OK!");
    
    return 0;
error:
    ak_println("init failed!");
    
    ak_fini();

    return err;
}
#else
#define DECLARE_FAKE_AK     os_fake_declare
#define DECLARE_REAL_AK     os_fake_declare
#define DECLARE_AK          os_fake_declare
DECLARE_FAKE_AK;

/*
* kernel/boot
*/
#define ak_getbyname(_key)              0
#define ak_get(_akid, _deft)            (_deft)
#define ak_set(_akid, _value)           0

#define ak_reload()                     0
#define ak_init()                       0
#define ak_fini()                       0
#endif /* defined(__APP__) && defined(__DEAMON__) */
/******************************************************************************/
typedef struct {
    akid_t id;
    char *key;
    uint32_t deft;
} ak_var_t;

#define AK_VAR_INITER(_key, _deft)  { \
    .id = INVALID_VALUE,    \
    .key = _key,            \
    .deft = _deft,          \
}

static inline int
ak_var_init(ak_var_t *var)
{
    if (INVALID_VALUE==var->id) {
        var->id = ak_getbyname(var->key);

        return 0;
    } else {
        return -EREINIT;
    }
}

static inline uint32_t
ak_var_get(ak_var_t *var)
{
    return ak_get(var->id, var->deft);
}

static inline int 
ak_var_set(ak_var_t *var, uint32_t v)
{
    return ak_set(var->id, v);
}
/******************************************************************************/
#endif /* __AK_H_2e420d20ac8a47b188d92ef8448db5fa__ */
