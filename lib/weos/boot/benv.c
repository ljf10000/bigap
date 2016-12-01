/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
bool
is_good_benv_cookie(benv_cookie_t *cookie)
{
    return  cookie->vendor[0]
        &&  cookie->company[0]
        &&  cookie->model[0]
        &&  cookie->version[0]
        &&  cookie->compile[0];
}

void
__benv_cookie_show(benv_cookie_t *cookie)
{
    os_println("vendor :%s", cookie->vendor);
    os_println("company:%s", cookie->company);
    os_println("model  :%s", cookie->model);
    os_println("version:%s", cookie->version);
    os_println("compile:%s", cookie->compile);
}

char *
benv_version_itoa(benv_version_t *version, char string[])
{
    os_sprintf(string, "%d.%d.%d.%d",
           version->number[0],
           version->number[1], 
           version->number[2], 
           version->number[3]);

    return string;
}

benv_version_t *
benv_version_atoi(benv_version_t *version, char *string)
{
    char line[1 + OS_LINE_LEN] = { 0 };
    char *number[4] = { line, NULL, NULL, NULL };
    int i;

    if (NULL==version) {
        return NULL;
    }
    else if (NULL==string) {
        return NULL;
    }
    
    os_strdcpy(line, string);

    /* begin 1 */
    for (i = 1; i < 4; i++) {
        number[i] = os_strchr(number[i - 1], '.');
        if (NULL == number[i]) {
            debug_error("bad version:%s", string);

            return NULL;
        }

        *(number[i])++ = 0;
    }

    for (i = 0; i < 4; i++) {
        char *end = NULL;

        version->number[i] = os_strtoul(number[i], &end, 0);
        if (false==os_strton_is_good_end(end)) {
            debug_error("bad-version:%s", string);

            return NULL;
        }
    }

    return version;
}

void
__benv_vcs_deft(benv_vcs_t *vcs)
{
    os_objzero(vcs);
    
    vcs->upgrade    = BENV_FSM_OK;
    vcs->other      = BENV_FSM_UNKNOW;
    vcs->self       = BENV_FSM_UNKNOW;
}

bool
is_benv_good_vcs(benv_vcs_t *vcs)
{
    if (false==is_benv_good(vcs->error)) {
        debug_trace("bad vcs error");
        
        return false;
    }
    else if (BENV_FSM_OK != vcs->upgrade) {
        debug_trace("vcs upgrade not ok");
        
        return false;
    }
    else if (false==is_canused_benv_fsm(vcs->self)) {
        debug_trace("vcs self failed");
    }

    return true;
}

#define __benv_os_show_obj(_os, _obj, _idx) do{ \
    char version_string[1+BENV_VERSION_STRING_LEN];         \
                                                            \
    os_println("%s%c %-7d %-7d %-7s %-7s %-7s %-15s %s",    \
        #_obj, _idx==_os->current?'*':' ',                   \
        _idx, _os->firmware[_idx]._obj.error,                \
        benv_fsm_getnamebyid(_os->firmware[_idx]._obj.upgrade),   \
        benv_fsm_getnamebyid(_os->firmware[_idx]._obj.self),      \
        benv_fsm_getnamebyid(_os->firmware[_idx]._obj.other),     \
        benv_version_itoa(&_os->firmware[_idx]._obj.version, version_string), \
        _os->firmware[_idx]._obj.cookie);                    \
}while(0)

void
__benv_os_show(benv_os_t *os)
{
    int i;
    
#if 0
        index   error   upgrade self    other   version         cookie
kernel  0       0       unknow  unknow  unknow  255.255.255.255 0123456789abcdef
rootfs  0       0       ok      ok      unknow
kernel* 1       0       fail    fail    unknow
rootfs* 1       0       fail    unknow  unknow
#endif
    
    os_println("        index   error   upgrade self    other   version         cookie");
    os_println("======================================================================");
    for (i=0; i<PRODUCT_FIRMWARE_COUNT; i++) {
        __benv_os_show_obj(os, kernel, i);
        __benv_os_show_obj(os, rootfs, i);
        
        if (i<(PRODUCT_FIRMWARE_COUNT-1)) {
            os_println
              ("----------------------------------------------------------------------");
        }
    }
    os_println("======================================================================");
}

int
__benv_ops_is(uint32 offset)
{
    if (offset >= sizeof(benv_env_t)) {
        return -ENOEXIST;
    }
    else if (offset >= offsetof(benv_env_t, info)) {
        return BENV_INFO;
    }
    else if (offset >= offsetof(benv_env_t, mark)) {
        return BENV_MARK;
    }
    else if (offset >= offsetof(benv_env_t, os)) {
        return BENV_OS;
    }
    else {
        return BENV_COOKIE;
    }
}

bool
benv_ops_match(benv_ops_t *ops, char *path, int len, bool wildcard)
{
    if (wildcard) {
        return benv_ops_match_wildcard(ops, path, len - 1);
    } else {
        return len == os_strlen(ops->path)
            && 0==os_memcmp(ops->path, path, len);
    }
}

int
benv_ops_check(benv_ops_t *ops, char *value)
{
    if (NULL==ops->write) {
        debug_error("no support write %s", ops->path);

        return -ENOSUPPORT;
    } else if (ops->check) {
        return (*ops->check)(ops, value);
    } else {
        return 0;
    }
}

bool
is_benv_good_kernel(int idx)
{
    if (false==is_good_benv_idx(idx)) {
        debug_trace("bad kernel index:%d", idx);
        
        return false;
    }
    else if (false==is_benv_good_vcs(benv_kernel(idx))) {
        debug_trace("bad kernel%d's vcs");
        
        return false;
    }

    return true;
}

bool
is_benv_good_rootfs(int idx)
{
    if (false==is_good_benv_idx(idx)) {
        debug_trace("bad rootfs index:%d", idx);
        
        return false;
    }
    else if (false==is_benv_good_vcs(benv_rootfs(idx))) {
        debug_trace("bad rootfs%d's vcs");
        
        return false;
    }

    return true;
}

int
benv_firmware_version_cmp(int a, int b)
{
    int ret;

    ret = benv_rootfs_version_cmp(a, b);
    if (ret) {
        return ret;
    }

    ret = benv_kernel_version_cmp(a, b);
    if (ret) {
        return ret;
    }

    return 0;
}

int
benv_vcs_cmp(char *obj, benv_vcs_t * a, benv_vcs_t * b)
{
    int ret;

    if (is_benv_good_vcs(a) && false==is_benv_good_vcs(b)) {
        /*
        * a is good, b is bad
        */
        return 1;
    }
    else if (false==is_benv_good_vcs(a) && is_benv_good_vcs(b)) {
        /*
        * a is bad, b is good
        */
        return -1;
    }
    
    ret = benv_version_cmp(&a->version, &b->version);
    if (ret) {
        return ret;
    }
    
    ret = benv_error_cmp(a->error, b->error);
    if (ret) {
        return ret;
    }

    ret = benv_fsm_cmp(a->upgrade, b->upgrade);
    if (ret) {
        return ret;
    }

    ret = benv_fsm_cmp(a->other, b->other);
    if (ret) {
        return ret;
    }

    ret = benv_fsm_cmp(a->self, b->self);
    if (ret) {
        return ret;
    }
    
    return 0;
}

int
__benv_obj_min(char *obj, int sort[], int count, int (*cmp)(int a, int b))
{
    int i, idx = 0;

    for (i=0; i<count; i++) {
        if (cmp(sort[i], sort[idx]) < 0) {
            idx = i;
        }
    }
    
#if BENV_DEBUG & (BENV_DEBUG_SORT | BENV_DEBUG_CMP)
    os_println("min %s is sort[%d]", obj, idx);
#endif

    return idx;
}

int
__benv_obj_max(char *obj, int sort[], int count, int (*cmp)(int a, int b))
{
    int i, idx = 0;

    for (i=0; i<count; i++) {
        if (cmp(sort[i], sort[idx]) > 0) {
            idx = i;
        }
    }
    
#if BENV_DEBUG & (BENV_DEBUG_SORT | BENV_DEBUG_CMP)
    os_println("max %s is sort[%d]", obj, idx);
#endif

    return idx;
}

void
__benv_sort(int sort[], int count, int (*maxmin)(int sort[], int count))
{
    if (count <= 1) {
        return;
    }
    
    int idx = (*maxmin)(sort, count);
    if (idx) {
        os_swap_value(sort[0], sort[idx]);
    }
    
    __benv_sort(sort+1, count-1, maxmin);
}

int
benv_first_idx(int current, int skips)
{
    int i;

    os_firmware_foreach(i) {
        if (false==is_benv_skip(skips, i)) {
            return i;
        }
    }

    trace_assert(0, "no found first idx");
    
    return (1==current?2:1);
}

int
__benv_sort_count(int skips, int sort[], int size)
{
    int i, count = 0;

    for (i=0; i<size; i++) {
        if (false==is_benv_skip(skips, i)) {
            sort[count++] = i;
        }
    }

    return count;
}

void
__benv_show_number(benv_ops_t *ops)
{
    __benv_show_header(ops);

    os_println("%u", *benv_ops_number(ops));
}

void
__benv_show_string(benv_ops_t *ops)
{
    char *string = benv_ops_string(ops);

    if (string[0]) {
        __benv_show_header(ops);

        os_println("%s", string);
    }
}

void
__benv_show_string_all(benv_ops_t *ops)
{
    char *string = benv_ops_string(ops);

    __benv_show_header(ops);

    os_println("%s", string[0]?string:__empty);
}

void
__benv_set_number(benv_ops_t * ops, char *value)
{
    *benv_ops_number(ops) = (uint32)(value[0] ? os_atoi(value) : 0);
}

void
__benv_set_string(benv_ops_t * ops, char *value)
{
    char *string = benv_ops_string(ops);

    if (value[0]) {
        os_strcpy(string, value);
    } else {
        string[0] = 0;
    }
}

int
__benv_check_version(benv_ops_t * ops, char *value)
{
    benv_version_t version = BENV_INVALID_VERSION;

    if (NULL==benv_version_atoi(&version, value)) {
        /*
         * when set version, must input value
         */
        return -EFORMAT;
    } else {
        return 0;
    }
}

void
__benv_show_version(benv_ops_t *ops)
{
    char version_string[1+BENV_VERSION_STRING_LEN];
    
    __benv_show_header(ops);

    os_println("%s", benv_version_itoa(benv_ops_version(ops), version_string));
}

void
__benv_set_version(benv_ops_t * ops, char *value)
{
    benv_version_t *v = benv_ops_version(ops);

    if (value[0]) {
        benv_version_atoi(v, value);
    } else {
        benv_version_t version = BENV_DEFT_VERSION;

        os_objscpy(v, &version);
    }
}

int
__benv_check_fsm(benv_ops_t * ops, char *value)
{
    if (0==value[0]) {
        /* 
         * clear fsm(to unknow)
         */
        return 0;
    } else if (is_good_benv_fsm(benv_fsm_getidbyname(value))) {
        return 0;
    } else {
        debug_error("bad fsm:%s", value);

        /*
         * when set self/other/upgrade, must input value
         */
        return -EFORMAT;
    }
}

void
__benv_show_fsm(benv_ops_t *ops)
{
    int fsm = *benv_ops_fsm(ops);

    __benv_show_header(ops);

    os_println("%s", benv_fsm_getnamebyid(fsm));
}

void
__benv_set_fsm(benv_ops_t * ops, char *value)
{
    uint32 fsm;

    if (value[0]) {
        fsm = benv_fsm_getidbyname(value);
    } else {
        fsm = BENV_FSM_UNKNOW;
    }

    *benv_ops_fsm(ops) = fsm;
}

int
__benv_check_current(benv_ops_t * ops, char *value)
{
    char *end = NULL;
    int idx = os_strtol(value, &end, 0);
    
    if (0==value[0]) {
        /*
         * when set kernel/rootfs current, must input value
         */
        debug_error("set current failed, must input value");

        return -EFORMAT;
    }
    else if (false==os_strton_is_good_end(end)) {
        debug_error("input invalid current:%s", value);
        
        return -EFORMAT;
    }
    else if (false==is_good_benv_idx(idx)) {
        debug_error("bad current %d", idx);

        return -EFORMAT;
    }
    else{
        return 0;
    }
}

int
__benv_check_string(benv_ops_t * ops, char *value)
{
    int size = 0;

    switch(__benv_ops_is(ops->offset)) {
        case BENV_OS:
            size = BENV_VCS_COOKIE_SIZE;
            break;
        case BENV_INFO:
            size = BENV_INFO_SIZE;
            break;
        case BENV_COOKIE:
        case BENV_MARK:
        default:
            /*
            * no support set string
            */
            break;
    }
    
    if (size && value[0]) {
        if (os_strlen(value) < size) {
            return 0;
        } else {
            debug_error("max string length %d", size-1);
            
            return -EFORMAT;
        }
    } else {
        /*
         * when set var string, may NOT input value
         */
        return 0;
    }
}

void
__benv_handle_write(benv_ops_t *ops)
{
    char *value = benv_cache_value(ops);

    if (value) {
        benv_ops_write(ops, value);
    }
}

void
__benv_handle(benv_ops_t *ops)
{
    /*
     * show
     */
    if (benv_cache_showit(ops)) {
        __benv_handle_show(ops);
    }
    /*
     * write
     */
    else if (benv_cache_value(ops)) {
        __benv_handle_write(ops);
    }
}

int
__benv_analysis_write(benv_ops_t *ops, char *args)
{
    char *path = args;
    char *eq = os_strchr(args, '=');
    char *value = eq + 1;
    int err;

    if (benv_ops_match(ops, path, eq - path, false)) {
        err = benv_ops_check(ops, value);
        if (err<0) {
            return err;
        }

        benv_cache_value(ops) = value;
    }
    
    return 0;
}

int
__benv_analysis(char *args)
{
    /*
     * first is '=', bad format
     */
    if ('=' == args[0]) {
        debug_error("first is '='");

        return -EFORMAT;
    }
    /*
     * first is '*', bad format
     */
    else if ('*' == args[0]) {
        debug_error("first is '*'");

        return -EFORMAT;
    }
    /*
     * found '=', is wirte
     */
    else if (os_strchr(args, '=')) {
        debug_trace("found '=', is write");
        
        return benv_analysis_write(args);
    }
    /*
     * no found '=', is show
     */
    else {
        debug_trace("no found '=', is show");
        
        return benv_analysis_show(args);
    }
}

int
benv_analysis(int argc, char *argv[])
{
    int i, err;
    
    for (i = 0; i < argc; i++) {
        err = __benv_analysis(argv[i]);
        if (err<0) {
            return err;
        }
    }

    return 0;
}

int
benv_command(int argc, char *argv[])
{
    int err, i;

    if (0==argc) {
        return benv_usage();
    }

    err = benv_analysis(argc, argv);
    if (err<0) {
        return err;
    }

    benv_cache_dump();

    benv_handle(argc, argv);
    
    return 0;
}

void
__benv_show_helper(benv_ops_t* ops)
{
    switch(ops->type) {
        case BENV_OPS_NUMBER:
            __benv_show_number(ops);
            break;
        case BENV_OPS_STRING:
            if (__benv_show_empty) {
                __benv_show_string_all(ops);
            } else {
                __benv_show_string(ops);
            }
            
            break;
        case BENV_OPS_VERSION:
            __benv_show_version(ops);
            break;
    }
}

int
benv_load(void)
{
    int env, err = 0;

    if (false==is_benv_loaded()) {
        for (env=0; env<BENV_COUNT; env++) {
            err = __benv_read(env, false);
            if (err<0) {
                return err;
            }
        }

        benv_repair();
    }

    benv_debug_init();

    /*
    * just for debug on PC
    */
#ifdef __PC__
    benv_check();
#endif
    
    return 0;
}

#if BENV_FLASH==BENV_FLASH_EMMC || defined(__APP__)
int
__benv_save(int idx)
{
    int err, env;
    
    /*
    * save all env's block(idx)
    */
    for (env=0; env<BENV_COUNT; env++) {
        err = __benv_write(env, idx);
        if (err<0) {
            return err;
        }
    }

    return 0;
}

#endif

/******************************************************************************/
