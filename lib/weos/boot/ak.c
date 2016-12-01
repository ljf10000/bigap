/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
uint32
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

uint32
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

int
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
    sys = ak_sys_getidbyname(key);
    
    if (is_good_ak_sys(sys)) {
        return __ak_sys_value(sys, value);
    } else {
        return __ak_debug_default;
    }
}









/******************************************************************************/
