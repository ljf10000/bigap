/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
DECLARE_ENUM(benv_fsm, BENV_FSM_ENUM_MAPPER, BENV_FSM_END);

DECLARE bool
is_good_benv_cookie(benv_cookie_t *cookie)
{
    return  cookie->vendor[0]
        &&  cookie->company[0]
        &&  cookie->model[0]
        &&  cookie->version[0]
        &&  cookie->compile[0];
}

DECLARE void
__benv_cookie_show(benv_cookie_t *cookie)
{
    os_println("vendor :%s", cookie->vendor);
    os_println("company:%s", cookie->company);
    os_println("model  :%s", cookie->model);
    os_println("version:%s", cookie->version);
    os_println("compile:%s", cookie->compile);
}

DECLARE char *
benv_version_itoa(benv_version_t *version, char string[])
{
    os_sprintf(string, "%d.%d.%d.%d",
           version->number[0],
           version->number[1], 
           version->number[2], 
           version->number[3]);

    return string;
}

DECLARE benv_version_t *
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

DECLARE void
__benv_vcs_deft(benv_vcs_t *vcs)
{
    os_objzero(vcs);
    
    vcs->upgrade    = BENV_FSM_OK;
    vcs->other      = BENV_FSM_UNKNOW;
    vcs->self       = BENV_FSM_UNKNOW;
}

DECLARE bool
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

DECLARE void
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

DECLARE int
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

DECLARE int
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

DECLARE int
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

DECLARE int
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

DECLARE void
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

DECLARE int
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

DECLARE int
__benv_check_version(benv_ops_t *ops, char *value)
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

DECLARE int
__benv_check_fsm(benv_ops_t *ops, char *value)
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

DECLARE int
__benv_check_current(benv_ops_t *ops, char *value)
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

DECLARE int
__benv_check_string(benv_ops_t *ops, char *value)
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

/******************************************************************************/
