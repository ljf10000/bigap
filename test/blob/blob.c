/******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      blob
#endif

#include "utils.h"

OS_INITER;

static slice_t BS;
static slice_t *bs = &BS;

#define BUFFER_SIZE     4096
int COUNT = 2;

#define BLOB_TEST   0

#if BLOB_TEST
#define tprintf(_fmt, _args...)     os_printf(_fmt, ##_args)
#define tprintln(_fmt, _args...)    os_println(_fmt, ##_args)
#else
#define tprintf(_fmt, _args...)     os_do_nothing()
#define tprintln(_fmt, _args...)    os_do_nothing()
#endif

static void
put_somthing(char *name)
{
    void *obj = NULL;
    
    if (name) {
        obj = blob_object_start(bs, name);
    }
    
    blob_put_bool(bs, "bool", true);
    blob_put_i32(bs, "i32", -1);
    blob_put_u32(bs, "u32", 32);
    blob_put_f32(bs, "f32", 32.32);
    blob_put_i64(bs, "i64", -1);
    blob_put_u64(bs, "u64", 64);
    blob_put_u64(bs, "f64", 64.64);
    blob_put_string(bs, "string", "ccccccc");
    blob_put_binary(bs, "binary", "bbbbbbb", 7);
//    blob_sprintf(bs "sprintf", "%s-%d", "bbbbbbb", 7);

    if (name) {
        blob_object_end(bs, obj);
    }
}

static jobj_t
array_btoj(void)
{
    char *name;
    void *arr, *obj;
    int i;
    char tmp[128];
    
    blob_root_array(bs);
    
    name = "FIRST";
    tprintln("%s begin", name);
    put_somthing(name);
    tprintln("%s end", name);

    name = tmp;
    for (i=0; i<COUNT; i++) {
        tprintln(name, "object-%.5d", i);
        tprintln("%s begin", name);
        put_somthing(name);
        tprintln("%s end", name);
    }
    
    name = "LAST";
    tprintln("%s begin", name);
    put_somthing(name);
    tprintln("%s end", name);

    blob_t *root = blob_root(bs);
    blob_dump(root);
    
    return blob_btoj(root);
}

static jobj_t
object_btoj(void)
{
    char *name;
    void *arr, *obj;
    int i;
    char tmp[128];
    
    blob_root_object(bs);

    put_somthing(NULL);
    
    name = "FIRST";
    tprintln("%s begin", name);
    put_somthing(name);
    tprintln("%s end", name);
    
    name = "OBJ";
    tprintln("%s begin", name);
    obj = blob_object_start(bs, name);
    for (i=0; i<COUNT; i++) {
        tprintf(tmp, "array-%.5d", i);
        
        tprintln("%s.%s begin", name, tmp);
        arr = blob_array_start(bs, tmp);
        put_somthing("sb1");
        put_somthing("sb2");
        put_somthing("sb3");
        blob_array_end(bs, arr);
        tprintln("%s.%s end", name, tmp);
    }
    blob_object_end(bs, obj);
    tprintln("%s end", name);
    
    name = "ARRAY";
    tprintln("%s begin", name);
    arr = blob_array_start(bs, name);
    for (i=0; i<COUNT; i++) {
        tprintf(tmp, "obj-%.5d", i);
        
        tprintln("%s.%s begin", name, tmp);
        put_somthing(tmp);
        tprintln("%s.%s end", name, tmp);
    }
    blob_array_end(bs, arr);
    tprintln("%s end", name);

    name = "LAST";
    tprintln("%s begin", name);
    put_somthing(name);
    tprintln("%s end", name);

    blob_t *root = blob_root(bs);
    blob_dump(root);
    
    return blob_btoj(root);
}


int __main(int argc, char *argv[])
{
    jobj_t obj;
    char *json;

    COUNT = env_geti("__BLOB_COUNT__", 2);
    
    slice_alloc(bs, BUFFER_SIZE);
    
    obj = array_btoj();
    json = jobj_string(obj);
    os_println("%s", json);
#if 1
    blob_jtob(bs, obj);
    blob_dump(blob_root(bs));
#endif
    jobj_put(obj);

    obj = object_btoj();
    json = jobj_string(obj);
    os_println("%s", json);
#if 1
    blob_jtob(bs, obj);
    blob_dump(blob_root(bs));
#endif
    jobj_put(obj);


    return 0;
}

int main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(__main, argc, argv);
}
/******************************************************************************/
