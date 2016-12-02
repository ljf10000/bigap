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

/******************************************************************************/
