/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      jexec
#endif

#include "utils.h"

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
    char *string = NULL;
    
    jobj_t jobj = jobj_new_object();
    if (NULL==jobj) {
        return -ENOMEM;
    }

    jobj_add_string(jobj, "stdout", b64_encode((byte *)outsring, os_strlen(outsring)));
    jobj_add_string(jobj, "stderr", b64_encode((byte *)errstring, os_strlen(errstring)));
    jobj_add_i32(jobj, "errno", error);

    os_println("%s", jobj_json(jobj));

    jobj_put(jobj);
    
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
    jobj_t jobj, jvar, jargument;
    int i, count, err = 0;
    
    jobj = jobj_byjson(json);
    if (NULL==jobj) {
        return -EBADJSON;
    }

    jvar = jobj_get(jobj, "content");
    if (jvar) {
        if (jtype_string != jobj_type(jvar)) {
            return -EBADJSON;
        }
        
        jinfo.content = jobj_get_string(jvar);
        jinfo.content = b64_decode((byte *)jinfo.content, os_strlen(jinfo.content));
        if (NULL==jinfo.content) {
            return -EBASE64;
        }
    }

    jvar = jobj_get(jobj, "filename");
    if (jvar) {
        if (jtype_string != jobj_type(jvar)) {
            return -EBADJSON;
        }
        
        jinfo.file = jobj_get_string(jvar);
    }

    jargument = jobj_get(jobj, "argument");
    if (jargument) {
        if (jtype_array != jobj_type(jargument)) {
            return -EBADJSON;
        }

        count = jarray_length(jargument);
        jinfo.argv = (char **)os_zalloc((1+count) * sizeof(char *));
        if (NULL==jinfo.argv) {
            return -ENOMEM;
        }

        for (i=0; i<count; i++) {
            jvar = jarray_get(jargument, i);
            if (jtype_string != jobj_type(jvar)) {
                return -EBADJSON;
            }

            jinfo.argv[i] = jobj_get_string(jvar);
        }
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
