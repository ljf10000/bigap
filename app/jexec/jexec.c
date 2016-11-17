/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      jexec
#endif

#include "utils.h"

OS_INITER;

/*
* not care memory leak
*/
static int 
jcallback(int error, char *outsring, char *errstring)
{

    
    os_println("stdout:%s", outsring);
    os_println("stderr:%s", errstring);
    os_println("errno:%d",  error);

    return 0;
}

static pipinfo_t jinfo = PIPINFO_INITER(NULL, jcallback);

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
        jinfo.content = b64_decode(jinfo.content, os_strlen(jinfo.content));
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
    
    return 0;
}

/*
{
    "content": "content",       #must base64 encode
    "filename": "filename",
    "argument": [               #option
        "arg1",
        "arg2",
        "arg3",
        ...
    ]
}
*/

static int
usage(int error)
{
    os_eprintln(__THIS_APPNAME " json");
    os_eprintln(__tab   "json format");
    os_eprintln(__tab   "{");
    os_eprintln(__tab __tab "\"content\": \"content\",       #must base64 encode");
    os_eprintln(__tab __tab "\"filename\": \"filename\",");
    os_eprintln(__tab __tab "\"argument\": [               #option");
    os_eprintln(__tab __tab __tab "\"arg1\",");
    os_eprintln(__tab __tab __tab "\"arg2\",");
    os_eprintln(__tab __tab __tab "\"arg3\",");
    os_eprintln(__tab __tab __tab "...");
    os_eprintln(__tab __tab "]");
    os_eprintln(__tab   "}");

    return error;
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
