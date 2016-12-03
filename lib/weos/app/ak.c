/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
bool __THIS_COMMAND;

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

STATIC akid_t
__ak_make(int idx, int offset)
{
    ak_u aku = __AKU_MAKE(idx, offset);

    return aku.akid;
}

STATIC int
__ak_idx(akid_t akid)
{
    ak_u aku = __AKU_INITER(akid);

    return (int)aku.v.idx;
}

STATIC int
__ak_offset(akid_t akid)
{
    ak_u aku = __AKU_INITER(akid);

    return (int)aku.v.offset;
}

#define ak_shm_size     (0  \
    + sizeof(ak_hdr_t)          \
    + sizeof(ak_t) * AK_LIMIT   \
    + sizeof(uint32)          \
)   /* end */

STATIC os_shm_t *
__this_ak(void)
{
    static os_shm_t ak = OS_SHM_INITER(OS_AK_SHM_ID);
    
    return &ak;
}
#define __ak_address    __this_ak()->address

STATIC ak_hdr_t *
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

STATIC void
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

STATIC int 
__ak_getidx(ak_t *ak)
{
    return ak - __ak_0;
}

STATIC int 
__ak_getoffset(ak_t *ak)
{
    return (char *)ak - (char *)__ak_hdr();
}

ak_t *
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

STATIC ak_t *
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

STATIC ak_t *
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

akid_t 
__ak_getbyname(char *app, char *k)
{
    if (NULL==app) {
        return os_assertV(INVALID_AKID);
    }
    else if (NULL==k) {
        return os_assertV(INVALID_AKID);
    }
    
    ak_t *ak = __ak_getbyname2(app, k);

    if (ak) {
        return __ak_make(__ak_getidx(ak), __ak_getoffset(ak));
    } else {
        return INVALID_AKID;
    }
}

int 
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

typedef struct {
    char *filename;
    char *line;
    char app[1+OS_LINE_LEN];
    char key[1+OS_LINE_LEN];
    char var[1+OS_LINE_LEN];
    uint32 v;
} ak_info_t;
#define AKINFO_INITER(_filename, _line) {  \
    .filename = (char *)_filename, \
    .line = _line, \
}

STATIC int
__ak_load_line_app(ak_info_t *info)
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
        ak_println("app(%s) length(%d) > %d", info->app, len, (int)OS_APPNAMELEN);
        
        return -ETOOBIG;
    }

    return 0;
}

STATIC int
__ak_load_line_kv(ak_info_t *info)
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

STATIC mv_t 
__ak_load_line(const char *file/* not include path */, char *line)
{
    ak_info_t info = AKINFO_INITER(file, line);
    int err;

    ak_println("load file(%s) line(%s)", file, line);
    
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
STATIC bool 
__ak_file_filter(const char *path, const char *file)
{
    char *p = os_strchr(file, '.');
    
    if (p && os_streq(p, ".key")) {
        return false;
    } else {
        ak_println("ignore %s", file);
        
        return true;
    }
}

int 
ak_load(void)
{
    int ret = 0;
    char *path;
    
    DIR *dir = opendir(AK_PATH);
    if (dir) {
        os_closedir(dir);

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

void 
ak_show(char *app, char *key) 
{
    int i;
    ak_t *ak;
    
    for (i=0; i<__ak_count; i++) {
        ak = __ak_entry(i);

        if ((NULL==app || os_streq(app, ak->app)) &&
            (NULL==key || os_streq(key, ak->k))) {
            os_println(__tab "%s.%s=%u",
                ak->app,
                ak->k,
                ak->v);
        }
    }
}

STATIC int
__ak_apps_find(char *apps[], int count, char *app)
{
    int i;

    for (i=0; i<count; i++) {
        if (os_streq(app, apps[i])) {
            return i;
        }
    }

    return INVALID_COMMON_ID;
}

STATIC int
__ak_apps_save(char *apps[])
{
    int i, id, count = 0;
    ak_t *ak;
    
    for (i=0; i<__ak_count; i++) {
        ak = __ak_entry(i);

        id = __ak_apps_find(apps, count, ak->app);
        if (false==is_good_common_id(id)) {
            apps[count++] = ak->app;
        }
    }

    return count;
}

void 
ak_jshow(char *app, char *key) 
{
    int i, j, id;
    char *apps[__ak_count];
    ak_t *ak;
    simple_buffer_t sb;;

    sb_init(&sb, 0, 0, 0, SB_F_EXPAND_AUTO);

    int count = __ak_apps_save(apps);
    int iapp = 0;
    
    sb_sprintf(&sb, "{" __crlf);
    for (i=0; i<count; i++) {
        int ikey = 0;

        if (app && os_strneq(app, apps[i])) {
            continue;
        }
        
        sb_sprintf(&sb, __tab "\"%s\": {" __crlf, apps[i]);
        
        for (j=0; j<__ak_count; j++) {
            ak = __ak_entry(j);
            
            if (os_strneq(apps[i], ak->app)) {
                continue;
            }
            else if (key && os_strneq(key, ak->k)) {
                continue;
            }

            sb_sprintf(&sb, __tab2 "\"%s\": %u,", ak->k, ak->v);
            ikey++;
        }

        if (ikey) {
            sb_backspace(&sb, 1); sb_backspace(&sb, __crlf);
        }
        
        sb_sprintf(&sb, __tab "},");
        iapp++;
    }
    
    if (iapp) {
        sb_backspace(&sb, 1); sb_backspace(&sb, __crlf);
    }
    sb_sprintf(&sb, "}" __crlf);
}

int 
ak_fini(void) 
{
    if (INVALID_SHM_ADDR != __ak_address) {
        shmdt(__ak_address);
        __ak_address = INVALID_SHM_ADDR;
        
        ak_println("shm fini shmdt(address:%p)", __ak_address);
    }

    return 0;
}

int 
ak_init(void) 
{
    int err = 0;
    
    err = os_shm_create(__this_ak(), ak_shm_size, false);
    if (err<0) { /* >=0 is valid shm id */
        goto error;
    }
    
    if (false==__ak_inited) {
        ak_println("ak first load");
        
        __ak_inited     = true;
        __ak_limit      = AK_LIMIT;
        __ak_protect_0  = OS_PROTECTED;
        __ak_protect_1  = OS_PROTECTED;
        
        ak_load();
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

/******************************************************************************/
