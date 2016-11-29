/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      jexec
#endif

#include "utils.h"

OS_INITER;

#define JSON_FORMAT                                     \
    __tab   "json format"                       __crlf  \
    __tab   "{"                                 __crlf  \
    __tab2      "\"content\": \"content\","     __crlf  \
    __tab2      "\"filename\": \"filename\","   __crlf  \
    __tab2      "\"argument\": ["               __crlf  \
    __tab3          "\"arg1\","                 __crlf  \
    __tab3          "\"arg2\","                 __crlf  \
    __tab3          "\"arg3\","                 __crlf  \
    __tab3          "..."                       __crlf  \
    __tab2      "]"                             __crlf  \
    __tab   "}"                                 __crlf  \
    /* end */

static int
usage(int error)
{
    os_eprintln(__THIS_APPNAME " json");
    os_eprintln(JSON_FORMAT);

    return error;
}

/*
* not care memory leak
*/
static int 
jcallback(int error, char *outsring, char *errstring)
{
    os_println( "{"
                    "\"stdout\":\"%s\","
                    "\"stderr\":\"%s\","
                    "\"errno\":%d"
                "}", 
        outsring,
        errstring,
        error);

    return 0;
}

#if 1
static char *jenv[] = {
    "SB:SB",
    "SB:SBSB",
    "SB:SBSBSB",
    NULL,
};
#else
#defien jenv NULL
#endif

static pipinfo_t jinfo = PIPINFO_INITER(jenv, jcallback);

/*
* not care memory leak
*/
static int
jmap(char *json)
{
    jobj_t jobj, jval, jargument;
    int i, count, err = 0;
    
    jobj = jobj_byjson(json);
    if (NULL==jobj) {
        return -EBADJSON;
    }


    jval = jobj_get(jobj, "content");
    if (NULL==jval || jtype_string != jobj_type(jval)) {
        return -EBADJSON;
    }
    
    char *content = jobj_get_string(jval);
    jinfo.content = b64_decode((byte *)content, os_strlen(content));
    if (NULL==jinfo.content) {
        return -EBASE64;
    }


    jval = jobj_get(jobj, "filename");
    if (NULL==jval || jtype_string != jobj_type(jval)) {
        return -EBADJSON;
    }
    jinfo.file = jobj_get_string(jval);


    jargument = jobj_get(jobj, "argument");
    if (NULL==jargument || jtype_array != jobj_type(jargument)) {
        return -EBADJSON;
    }
    
    count = jarray_length(jargument);
    jinfo.argv = (char **)os_zalloc((1+count) * sizeof(char *));
    if (NULL==jinfo.argv) {
        return -ENOMEM;
    }
    
    for (i=0; i<count; i++) {
        jval = jarray_get(jargument, i);
        if (jtype_string != jobj_type(jval)) {
            return -EBADJSON;
        }

        jinfo.argv[i] = jobj_get_string(jval);
    }

    debug_trace("input json=%s", jobj_json(jobj));
    
    return 0;
}

static int
__main(int argc, char *argv[])
{
    int err;

    if (2!=argc) {
        return usage(-EHELP);
    }
    
    err = jmap(argv[1]);
    if (err<0) {
        return err;
    }
    
    return os_pexecv(&jinfo);
}

int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(__main, argc, argv);
}
/******************************************************************************/
