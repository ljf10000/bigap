/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
DECLARE int
envs_count(char *env[])
{
    int count;

    if (NULL==env) {
        return 0;
    }
    
    for (count=0; env[count]; count++) {
        os_do_nothing();
    }

    return count;
}

DECLARE void
envs_append(char *dst[], char *src[])
{
    int i, count = envs_count(dst);

    if (src) {
        for (i=0; src[i]; i++) {
            dst[i+count] = src[i];
        }
    }
}

DECLARE char **
envs_merge(char **old, char **new)
{
    os_println("envs_merge 1");
    int count_old = envs_count(old);
    int count_new = envs_count(new);
    
    if (0==count_old) {
        return new;
    }
    else if (0==count_new) {
        return NULL;
    }

    os_println("envs_merge 2");
    
    int count = 1 + count_old + count_new;
    os_println("envs_merge 3");
    char **array = (char **)os_zalloc(sizeof(char *) * count);
    if (array) {
        os_println("envs_merge 3.1");
        envs_append(array, new);
        os_println("envs_merge 3.2");
        envs_append(array, old);
        os_println("envs_merge 3.3");
    }

    return array;
}

DECLARE int 
zip2line_helper(char line[], int size, char *s, int sep)
{
    int len = os_strlen(s);

    /*
    * 2: '\0' and sep
    */
    if (size < (len+2)) {
        return -ENOSPACE;
    }

    os_memcpy(line, s, len);

    line[len++] = sep;
    line[len] = 0;
    
    return len;
}

DECLARE int 
zip2line(char buf[], int size, char *ss[], bool (*is_good)(int idx), int sep)
{
    int i, offset = 0, len;
    
    for (i=0; (*is_good)(i); i++) {
        len = zip2line_helper(buf + offset, size - offset, ss[i], sep);
        if (len<0) {
            return len;
        }

        offset += len;
    }

    buf[offset++] = 0; // append last '\0'

    return offset;
}

DECLARE int 
argv_zip2line(char buf[], int size, int argc, char *argv[], int sep)
{
    bool is_good(int idx)
    {
        return idx < argc;
    }
    
    return zip2line(buf, size, argv, is_good, sep);
}

DECLARE int 
envs_zip2line(char buf[], int size, char *env[], int sep)
{
    bool is_good(int idx)
    {
        return !!env[idx];
    }

    return zip2line(buf, size, env, is_good, sep);
}

DECLARE int
argv_unzipbin(char buf[], int argc, char *argv[])
{
    char *p = buf;
    int i;
    
    for (i=0; i<argc; i++) {
        if (*p) {
            argv[i] = p;

            p += 1 + os_strlen(p); // point to next
        } else {
            return i;
        }
    }

    return -ENOSPACE;
}

DECLARE int
envs_unzipbin(char buf[], int count, char *env[])
{
    int c = argv_unzipbin(buf, count, env);
    if (c<0) {
        return c;
    }
    else if (c>=count) {
        return -ENOSPACE;
    }
    else {
        env[c++] = NULL;

        return c;
    }
}

DECLARE char *
env_gets(char *envname, char *deft) 
{
    if (envname) {
        char *env = getenv(envname);
        
        if (is_good_env(env)) {
            env_println("get env:%s=%s", envname, env);
            
            return env;
        } else {
            env_println("no-found env:%s, use default:%s", envname, deft);

            return deft;
        }
    } else {
        env_println("empty env, use default:%s", deft);

        return deft;
    }
}

DECLARE int
__env_copy(char *envname, char *deft, char s[], int size) 
{
    char *env = env_gets(envname, deft);
    if (NULL==env) {
        return 0;
    }
    else if (os_strlen(env) > size - 1) {
        return os_assertV(-ETOOBIG);
    }
    
    os_strlcpy(s, env, size);
    
    return 0;
}

DECLARE int
env_geti(char *envname, int deft) 
{
    if (NULL==envname) {
        return os_assertV(deft);
    }
    
    char *env = getenv(envname);
    if (false==is_good_env(env)) {
        env_println("no-found env:%s, use default:%d", envname, deft);
        
        return deft;
    } else {
        int value = os_atoi(env);

        env_println("get env:%s=%d", envname, value);

        return value;
    }
}
/******************************************************************************/
