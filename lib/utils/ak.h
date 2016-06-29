#ifndef __AK_H_2e420d20ac8a47b188d92ef8448db5fa__
#define __AK_H_2e420d20ac8a47b188d92ef8448db5fa__
/******************************************************************************/
#ifndef OS_APPNAMELEN
#define OS_APPNAMELEN           (32 - sizeof(uint32) - 1)
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

#ifndef AK_DEBUG_NAME
#define AK_DEBUG_NAME           "debug"
#endif

#ifndef JS_DEBUG_NAME
#define JS_DEBUG_NAME           "jsdebug"
#endif

#ifndef ENV_AK_DEBUG
#define ENV_AK_DEBUG            "__AK_DEBUG__"
#endif

#ifndef ENV_JS_DEBUG
#define ENV_JS_DEBUG            "__JS_DEBUG__"
#endif

#ifndef ENV_RUNAS_DEAMON
#define ENV_RUNAS_DEAMON        "__RUN_AS_DEAMON__"
#endif

enum {
    INVALID_AKID    = 0,
};

#ifndef AK_DPRINT
#define AK_DPRINT               0
#endif

#if AK_DPRINT
#define ak_println(_fmt, _args...)  printf(_fmt "\n", ##_args)
#else
#define ak_println(_fmt, _args...)  os_do_nothing()
#endif

typedef uint32 akid_t;

#if defined(__BOOT__)
#    define DECLARE_FAKE_DEBUGGER       extern akid_t *__THIS_DEBUG
#    define DECLARE_REAL_DEBUGGER       akid_t *__THIS_DEBUG
#    define DECLARE_DEBUGGER            DECLARE_REAL_DEBUGGER

#    define DECLARE_FAKE_JDEBUGGER      extern akid_t *__THIS_JDEBUG
#    define DECLARE_REAL_JDEBUGGER      akid_t *__THIS_JDEBUG
#    define DECLARE_JDEBUGGER           DECLARE_REAL_JDEBUGGER
#else
#    define DECLARE_FAKE_DEBUGGER       extern akid_t __THIS_DEBUG
#    define DECLARE_REAL_DEBUGGER       akid_t __THIS_DEBUG
#    define DECLARE_FAKE_JDEBUGGER      extern akid_t __THIS_JDEBUG
#    define DECLARE_REAL_JDEBUGGER      akid_t __THIS_JDEBUG
#    ifdef __BUSYBOX__
#        define DECLARE_DEBUGGER        DECLARE_FAKE_DEBUGGER
#        define DECLARE_JDEBUGGER       DECLARE_FAKE_JDEBUGGER
#    else
#        define DECLARE_DEBUGGER        DECLARE_REAL_DEBUGGER
#        define DECLARE_JDEBUGGER       DECLARE_REAL_JDEBUGGER
#    endif
#endif

DECLARE_FAKE_DEBUGGER;
DECLARE_FAKE_JDEBUGGER;
/******************************************************************************/
#define __XLIST_AK_DEBUG(_)                 \
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
    _(____ak_debug_proto,   14,"proto"),    \
    _(____ak_debug_flow,    15,"flow"),     \
    _(____ak_debug_file,    16,"file"),     \
    _(____ak_debug_signal,  17,"signal"),   \
    _(____ak_debug_shell,   18,"shell"),    \
    _(____ak_debug_st,      19,"st"),       \
    _(____ak_debug_gc,      20,"gc"),       \
    _(____ak_debug_js,      21,"js"),       \
    _(____ak_debug_blob,    22,"blob"),     \
    _(____ak_debug_json,    23,"json"),     \
    _(____ak_debug_test,    24,"test"),     \
    /* end */

static inline bool is_good_ak_DEBUG(int id);
static inline char *ak_DEBUG_string(int id);
static inline int ak_DEBUG_idx(char *name);
DECLARE_ENUM(ak_DEBUG, __XLIST_AK_DEBUG, ____ak_debug_end);

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
    __ak_debug_proto    = os_bit(____ak_debug_proto),
    __ak_debug_flow     = os_bit(____ak_debug_flow),
    __ak_debug_file     = os_bit(____ak_debug_file),
    __ak_debug_signal   = os_bit(____ak_debug_signal),
    __ak_debug_shell    = os_bit(____ak_debug_shell),
    __ak_debug_st       = os_bit(____ak_debug_st),
    __ak_debug_gc       = os_bit(____ak_debug_gc),
    __ak_debug_js       = os_bit(____ak_debug_js),
    __ak_debug_blob     = os_bit(____ak_debug_blob),
    __ak_debug_json     = os_bit(____ak_debug_json),
    __ak_debug_test     = os_bit(____ak_debug_test),

    __ak_debug_all      = os_mask(____ak_debug_end),
};

#ifndef __ak_debug_default
#define __ak_debug_default          (__ak_debug_error | __ak_debug_bug)
#endif

#ifndef __js_debug_default
#define __js_debug_default          0
#endif

#ifndef __ak_debug_string_default
#define __ak_debug_string_default   "error|bug"
#endif

#ifndef __js_debug_string_default
#define __js_debug_string_default   "0"
#endif

static inline uint32
__ak_debug_getbyname(char *name)
{
    int idx = ak_DEBUG_idx(name);

    return os_bit(idx);
}

static inline char *
__ak_debug_getname(uint32 level)
{
    int idx = os_bitshift(level);

    return ak_DEBUG_string(idx);
}

#if defined(__BOOT__)
#   define __ak_debug       (__THIS_DEBUG?(*__THIS_DEBUG):__ak_debug_default)
#   define __js_debug       (__THIS_JDEBUG?(*__THIS_JDEBUG):__js_debug_default)
#elif defined(__APP__)
#   if __RUNAS__==RUN_AS_COMMAND
#       define __ak_debug   __THIS_DEBUG
#       define __js_debug   __THIS_JDEBUG
#   else /* run as deamon/unknow */
#       define __ak_debug   ak_get(__THIS_DEBUG, __ak_debug_default)
#       define __js_debug   ak_get(__THIS_JDEBUG, __js_debug_default)
#   endif
#elif defined(__KERNEL__)
#   define __ak_debug       __THIS_DEBUG
#   define __js_debug       __THIS_JDEBUG
#else
#   error "invalid __THIS_DEBUG"
#   error "invalid __THIS_JDEBUG"
#endif

#define __is_ak_debug(_level)   (os_hasflag(__ak_debug, _level))
#define __is_js_debug(_level)   (os_hasflag(__js_debug, _level))

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
#define __is_ak_debug_proto     __is_ak_debug(__ak_debug_proto)
#define __is_ak_debug_flow      __is_ak_debug(__ak_debug_flow)
#define __is_ak_debug_file      __is_ak_debug(__ak_debug_file)
#define __is_ak_debug_signal    __is_ak_debug(__ak_debug_signal)
#define __is_ak_debug_shell     __is_ak_debug(__ak_debug_shell)
#define __is_ak_debug_st        __is_ak_debug(__ak_debug_st)
#define __is_ak_debug_gc        __is_ak_debug(__ak_debug_gc)
#define __is_ak_debug_js        __is_ak_debug(__ak_debug_js)
#define __is_ak_debug_blob      __is_ak_debug(__ak_debug_blob)
#define __is_ak_debug_json      __is_ak_debug(__ak_debug_json)
#define __is_ak_debug_test      __is_ak_debug(__ak_debug_test)

#define __XLIST_AK_SYS(_) \
    _(__AK_SYS_DEBUG,   0, AK_DEBUG_NAME), \
    _(__AK_SYS_JDEBUG,  1, JS_DEBUG_NAME), \
    /* end */
    
static inline bool is_good_ak_sys(int id);
static inline int ak_sys_idx(char *name);
DECLARE_ENUM(ak_sys, __XLIST_AK_SYS, __AK_SYS_END);

#if 1 /* just for sourceinsight */
#define __AK_SYS_DEBUG  __AK_SYS_DEBUG
#define __AK_SYS_JDEBUG __AK_SYS_JDEBUG
#define __AK_SYS_END    __AK_SYS_END
#endif /* just for sourceinsight */

static inline uint32
__ak_sys_debug(char *var)
{
    char line[1+OS_LINE_LEN] = {0};
    char *name;
    uint32 v = 0;

    os_strdcpy(line, var);
    os_strtok_foreach(name, line, "|") {
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
    
    return v;
}

static inline uint32
__ak_sys_value(int sys, char *line)
{
    /*
    * try "*"
    */
    if ('*'==line[0] && 0==line[1]) {
        return __ak_debug_all;
    }
    
    switch(sys) {
        case __AK_SYS_DEBUG:
            return __ak_sys_debug(line);
        default:
            return __ak_debug_default;
    }
}

static inline int
__ak_get_value(char *key, char *value)
{
    int sys = INVALID_VALUE;
    char *end = NULL;
    uint32 v = 0;
    
    /*
    * first, try value as digit string
    */
    v = strtoul(value, &end, 0);
    if (NULL==end || 0==end[0]) {
        return v;
    }
    
    /*
    * not digit string, try sys
    */
    sys = ak_sys_idx(key);
    
    if (is_good_ak_sys(sys)) {
        return __ak_sys_value(sys, value);
    } else {
        return __ak_debug_default;
    }
}

/******************************************************************************/
#define DECLARE_FAKE_COMMAND  extern bool __THIS_COMMAND
#define DECLARE_REAL_COMMAND  bool __THIS_COMMAND;

#ifdef __BUSYBOX__
#   define DECLARE_COMMAND  DECLARE_FAKE_COMMAND
#else
#   define DECLARE_COMMAND  DECLARE_REAL_COMMAND
#endif

DECLARE_FAKE_COMMAND;

static inline int __ak_init(void);

#if !defined(__APP__) || __RUNAS__==RUN_AS_COMMAND
#define DECLARE_FAKE_AK     os_fake_declare
#define DECLARE_REAL_AK     os_fake_declare
#define DECLARE_AK          os_fake_declare
DECLARE_FAKE_AK;
/*
* kernel/boot/(app cmd)
*/
#define ak_getbyname(_key)              0

#define ak_get(_akid, _deft)            (_akid)
#define ak_set(_akid, _value)           0

#define ak_reload()                     0
#define ak_fini()                       0

static inline int 
ak_init(void)
{    
    __ak_init();

    return 0;
}
#else /* defined(__APP__) && !defined(__COMMAND__) */
/*
* app deamon
*/
typedef struct {
    char app[1+OS_APPNAMELEN];
    char k[1+OS_AKNAME_LEN];
    uint32 v;
} ak_t;

typedef struct {
    uint32 protect;
    bool inited;
    int count;
    int limit;
    ak_t entry[0];
} ak_hdr_t;

typedef union {
    akid_t akid;

    struct {
        uint16 idx;
        uint16 offset;
    } v;
} ak_u;

#define __AKU_MAKE(_idx, _offset)    {  \
    .v = {                              \
        .idx = (uint16)(_idx),          \
        .offset = (uint16)(_offset),    \
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
    + sizeof(uint32)          \
)   /* end */

#define DECLARE_FAKE_AK  extern os_shm_t __THIS_AK
#define DECLARE_REAL_AK  os_shm_t __THIS_AK = OS_SHM_INITER(OS_AK_SHM_ID)

#ifdef __BUSYBOX__
#   define DECLARE_AK   DECLARE_FAKE_AK
#else
#   define DECLARE_AK   DECLARE_REAL_AK
#endif

DECLARE_FAKE_AK;

static inline os_shm_t *
__this_ak(void)
{
    return &__THIS_AK;
}
#define __ak_address    __this_ak()->address

static inline ak_hdr_t *
__ak_hdr(void)
{
    return (ak_hdr_t *)__ak_address;
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
#define __ak_protect_1      (*(uint32 *)__ak_END)

static inline void
__ak_dump(void)
{
    ak_t *ak = NULL;

    ak_println("ak dump:");
    ak_println(__tab "inited:%d", __ak_inited);
    ak_println(__tab "count:%d", __ak_count);
    ak_println(__tab "limit:%d", __ak_limit);
    
    __ak_foreach(ak) {
        ak_println(__tab "app:%s, k:%s, v:0x%x", ak->app, ak->k, ak->v);
    }
}

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
    if (is_good_enum(__ak_idx(akid), __ak_count)) {
        char *address = (char *)__ak_hdr() + __ak_offset(akid);
        ak_t *ak = __ak_entry(__ak_idx(akid));

        return (address==(char *)ak)?ak:NULL;
    } else {
        return NULL;
    }
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
__ak_getbyname2(char *app, char *k)
{
    ak_t *ak = NULL;
    
    __ak_foreach(ak) {
        if (os_straeq(ak->app, app) && os_straeq(ak->k, k)) {
            return ak;
        }
    }
    
    return NULL;
}

static inline akid_t 
__ak_getbyname(char *app, char *k)
{
    if (NULL==app) {
        return os_assertV(INVALID_AKID);
    }
    else if (NULL==k) {
        return os_assertV(INVALID_AKID);
    }
    
    ak_t *ak = __ak_getbyname2(app, k);

    return ak?__ak_make(__ak_getidx(ak), __ak_getoffset(ak)):INVALID_AKID;
}

static inline int 
__ak_get(akid_t akid, uint32 *pv)
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
    uint32 v;
};
#define AKINFO_INITER(_filename, _line) {  \
    .filename = _filename, \
    .line = _line, \
}

static inline int
__ak_load_line_app(struct akinfo *info)
{
    /*
    * filename's format is "xxx.key"
    *   cannot use os_sscanf(filename, "%s.key", app)
    */
    if (1!=os_sscanf(info->filename, "%[^.]", info->app)) {
        ak_println("read file(%s) bad line(%s)", info->filename, info->line);
        
        return -EFORMAT;
    }
    
    int len = os_strlen(info->app);
    if (len >= OS_APPNAMELEN) {
        ak_println("app(%s) length(%d) > %d", info->app, len, OS_APPNAMELEN);
        
        return -ETOOBIG;
    }

    return 0;
}

static inline int
__ak_load_line_kv(struct akinfo *info)
{
    /* 
    * read key & var 
    */
    if (2!=os_sscanf(info->line, "%s %s", info->key, info->var)) {
        ak_println("read app(%s) bad line(%s)", info->app, info->line);
        
        return -EFORMAT;
    }
    
    int len = os_strlen(info->key);
    if (len >= OS_AKNAME_LEN) {
        ak_println("key(%s) length(%d) > %d", info->key, len, OS_AKNAME_LEN);
        
        return -ETOOBIG;
    }

    return 0;
}

static inline mv_t 
__ak_load_line(char *filename/* not include path */, char *line)
{
    struct akinfo info = AKINFO_INITER(filename, line);
    int err;

    ak_println("load file(%s) line(%s)", filename, line);
    
    err = __ak_load_line_app(&info);
    if (err<0) {
        return mv2_go(err);
    }

    err = __ak_load_line_kv(&info);
    if (err<0) {
        return mv2_go(err);
    }
    info.v = __ak_get_value(info.key, info.var);
    
    ak_t *ak = __ak_getbyname2(info.app, info.key);
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
    
    if (p && os_streq(p, ".key")) {
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
    
    ret = os_fscan_dir(path, false, __ak_file_filter, NULL, __ak_load_line);
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

static inline akid_t 
ak_getbyname(char *k)
{
    if (__THIS_COMMAND) {
        return 0;
    } else {
        return __ak_getbyname(__THIS_APPNAME, k);
    }
}

static inline uint32
ak_get(akid_t akid, uint32 deft)
{
    if (__THIS_COMMAND) {
        return akid;
    } else {
        uint32 v = deft;

        __ak_get(akid, &v);

        return v;
    }
}

static inline int 
ak_set(akid_t akid, uint32 v)
{
    if (false==__THIS_COMMAND) {
        ak_t *ak = __ak_getbyid(akid);

        if (NULL==ak) {
            return -ENOEXIST;
        }
        
        ak->v = v;
    }
    
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
    if (INVALID_SHM_ADDR != __ak_address) {
        shmdt(__ak_address);
        __ak_address = INVALID_SHM_ADDR;
        
        ak_println("shm fini shmdt(address:%p)", __ak_address);
    }

    return 0;
}

static inline int 
ak_init(void) 
{
    int err = 0;
    
    err = os_shm_create(__this_ak(), appkey_shm_size, false);
    if (err<0) { /* >=0 is valid shm id */
        goto error;
    }
    
    if (false==__ak_inited) {
        ak_println("ak first load");
        
        __ak_inited     = true;
        __ak_limit      = AK_LIMIT;
        __ak_protect_0  = OS_PROTECTED;
        __ak_protect_1  = OS_PROTECTED;
        
        __ak_load();
    }
    
    __ak_init();
    __ak_dump();
    
    ak_println("init OK!");
    
    return 0;
error:
    ak_println("init failed!");
    
    ak_fini();

    return err;
}
#endif /* !defined(__APP__) || defined(__COMMAND__) */

#if defined(__APP__) && (__RUNAS__ & RUN_AS_COMMAND)
static inline void 
__ak_init_command() 
{
    char *value;
    
    value = env_gets(ENV_AK_DEBUG, __ak_debug_string_default);
    __THIS_DEBUG = __ak_get_value(AK_DEBUG_NAME, value);
    ak_println("__THIS_DEBUG=%s==>0x%x", value, __THIS_DEBUG);

    value = env_gets(ENV_JS_DEBUG, __js_debug_string_default);
    __THIS_JDEBUG = __ak_get_value(JS_DEBUG_NAME, value);
    ak_println("__THIS_JDEBUG=%s==>0x%x", value, __THIS_JDEBUG);
}
#endif

#if defined(__APP__) && (__RUNAS__ & RUN_AS_DEAMON)
static inline void 
__ak_init_deamon() 
{    
    __THIS_DEBUG    = ak_getbyname(AK_DEBUG_NAME);
    __THIS_JDEBUG   = ak_getbyname(JS_DEBUG_NAME);
}
#endif

#if defined(__APP__) && (__RUNAS__==RUN_AS_UNKNOW)
static inline void 
__ak_init_unknow() 
{
    char *value = getenv(ENV_RUNAS_DEAMON);
    
    __THIS_COMMAND = (NULL==value);
    ak_println(ENV_RUNAS_DEAMON "=%s, __THIS_COMMAND=%d", value, __THIS_COMMAND);

    if (__THIS_COMMAND) {
        __ak_init_command();
    } else {
        __ak_init_deamon();
    }
}
#endif

static inline int 
__ak_init(void)
{
    ak_println("ak runas %d", __RUNAS__);
    
#ifdef __APP__
#if __RUNAS__==RUN_AS_COMMAND
    __ak_init_command();
#elif __RUNAS__==RUN_AS_DEAMON
    __ak_init_deamon();
#elif __RUNAS__==RUN_AS_UNKNOW
    __ak_init_unknow();
#else
#   error "bad __RUNAS__"
#endif
#endif

    return 0;
}
/******************************************************************************/
typedef struct {
    akid_t id;
    char *key;
    uint32 deft;
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

static inline uint32
ak_var_get(ak_var_t *var)
{
    return ak_get(var->id, var->deft);
}

static inline int 
ak_var_set(ak_var_t *var, uint32 v)
{
    return ak_set(var->id, v);
}
/******************************************************************************/
#endif /* __AK_H_2e420d20ac8a47b188d92ef8448db5fa__ */
