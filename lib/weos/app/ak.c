/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
/*
* app deamon
*/
typedef struct {
    char app[1+OS_APPNAMELEN];
    char k[1+OS_APPKEYLEN];
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
        uint32 idx: AK_BITS;
        uint32 offset: 8*sizeof(akid_t) - AK_BITS;
    } v;
} ak_u;

#define __AKU_MAKER(_idx, _offset)  {   \
    .v = {                              \
        .idx = _idx,                    \
        .offset = _offset,              \
    }                                   \
}

#define __AKU_INITER(_akid)         { .akid = _akid }

ALWAYS_INLINE akid_t
AK_MAKE(uint32 idx, uint32 offset)
{
    ak_u u = __AKU_MAKER(idx, offset);

    return u.akid;
}

ALWAYS_INLINE uint32
AK_IDX(akid_t akid)
{
    ak_u u = __AKU_INITER(akid);

    return u.v.idx;
}

ALWAYS_INLINE uint32
AK_OFFSET(akid_t akid)
{
    ak_u u = __AKU_INITER(akid);

    return (int)u.v.offset;
}

#define ak_shm_size         (0  \
    + sizeof(ak_hdr_t)          \
    + sizeof(ak_t) * AK_LIMIT   \
    + sizeof(uint32)            \
)   /* end */

ALWAYS_INLINE os_shm_t *
__this_ak(void)
{
    static os_shm_t ak = OS_SHM_INITER(OS_AK_SHM_ID);
    
    return &ak;
}
#define __ak_address    __this_ak()->address

ALWAYS_INLINE ak_hdr_t *
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

ALWAYS_INLINE uint32 
__ak_idx(ak_t *ak)
{
    return ak - __ak_0;
}

ALWAYS_INLINE uint32 
__ak_offset(ak_t *ak)
{
    return (char *)ak - (char *)__ak_hdr();
}

ALWAYS_INLINE akid_t
__ak_make(ak_t *ak)
{
    return AK_MAKE(__ak_idx(ak), __ak_offset(ak));
}

STATIC ak_t *
__ak_new(char *app, char *k)
{
    if (__ak_count < __ak_limit) {
        ak_t *ak = __ak_entry(__ak_count++);
        
        os_strdcpy(ak->app, app);
        os_strdcpy(ak->k, k);
        
        return ak;
    } else {
        ak_println("count(%u) >= limit(%u)", __ak_count, __ak_limit);
        
        return NULL;
    }
}

STATIC ak_t *
__ak_getbyid(akid_t akid)
{
    if (is_good_enum(AK_IDX(akid), __ak_count)) {
        char *address = (char *)__ak_hdr() + AK_OFFSET(akid);
        ak_t *ak = __ak_entry(AK_IDX(akid));

        return (address==(char *)ak)?ak:NULL;
    } else {
        return NULL;
    }
}

STATIC ak_t *
__ak_getbyname(char *app, char *k, bool CreateIfNotExist)
{
    ak_t *ak = NULL;
    
    if (NULL==app || os_strlen(app) > OS_APPNAMELEN) {
        return NULL;
    }
    else if (NULL==k || os_strlen(k) > OS_APPKEYLEN) {
        return NULL;
    }
    
    __ak_foreach(ak) {
        if (os_straeq(ak->app, app) && os_straeq(ak->k, k)) {
            return ak;
        }
    }

    if (CreateIfNotExist) {
        ak = __ak_new(app, k);
        if (NULL==ak) {
            ak_println("limit(%d)", __ak_limit);
        }

        return ak;
    } else {
        return NULL;
    }
}

akid_t 
__ak_getidbyname(char *app, char *k)
{
    ak_t *ak = __ak_getbyname(app, k, false);

    return ak?__ak_make(ak):INVALID_AKID;
}

akid_t 
__ak_getidbynameEx(char *app, char *k)
{
    ak_t *ak = __ak_getbyname(app, k, true);

    return ak?__ak_make(ak):INVALID_AKID;
}

char * 
__ak_getnamebyid(akid_t akid)
{
    ak_t *ak = __ak_getbyid(akid);
    if (NULL==ak) {
        return NULL;
    }
    
    return ak->app;
}

char * 
__ak_getkeybyid(akid_t akid)
{
    ak_t *ak = __ak_getbyid(akid);
    if (NULL==ak) {
        return NULL;
    }
    
    return ak->k;
}

uint32 * 
__ak_getvaluebyid(akid_t akid)
{
    ak_t *ak = __ak_getbyid(akid);
    if (NULL==ak) {
        return NULL;
    }
    
    return &ak->v;
}

int 
__ak_setvaluebyid(akid_t akid, uint32 v)
{
    ak_t *ak = __ak_getbyid(akid);
    if (NULL==ak) {
        return -ENOEXIST;
    }
    
    ak->v = v;
    
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
#define AK_INFO_INITER(_filename, _line) {  \
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
    if (len >= OS_APPKEYLEN) {
        ak_println("key(%s) length(%d) > %d", info->key, len, OS_APPKEYLEN);
        
        return -ETOOBIG;
    }

    return 0;
}

STATIC mv_t 
__ak_load_line(const char *file/* not include path */, char *line)
{
    ak_info_t info = AK_INFO_INITER(file, line);
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
    
    ak_t *ak = __ak_getbyname(info.app, info.key, true);
    if (NULL==ak) {
        return mv2_break(-ELIMIT);
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

STATIC char *
__ak_path(char *path, char *deft)
{
    DIR *dir = opendir(path);
    if (dir) {
        os_closedir(dir);

        return path;
    } else {
        return deft;
    }
}

int 
ak_load(void)
{
    char *path = __ak_path(AK_PATH, AK_PATH_DEFT);
    int err = 0;
    
    err = os_fscan_dir(path, false, __ak_file_filter, NULL, __ak_load_line);
    if (err<0) {
        ak_println("load(%s) error%d", path, err);
        
        return err;
    }
    
    ak_println("load(%s) ak count=%u, limit=%u",
        path,
        __ak_count,
        __ak_limit);
    
    return 0;
}

int
ak_foreach(mv_t (*foreach)(char *app, char *k, uint32 v))
{
    if (foreach) {
        ak_t *ak;
        mv_u mv;
    
        __ak_foreach(ak) {
            mv.v = (*foreach)(ak->app, ak->k, ak->v);
            if (is_mv2_break(mv)) {
                return mv.v;
            }
        }
    }

    return 0;
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
ak_init_helper(void) 
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
        os_println(__THIS_APPNAME " " __THIS_FILENAME " ak load OK!");
    }

    __ak_dump();

    return 0;
error:    
    ak_fini();

    return err;
}

/******************************************************************************/
