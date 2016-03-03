/******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      testblob
#endif

#define OS_EXTEND

#include "utils.h"

OS_INITER;

static slice_t BS;
static slice_t *bs = &BS;

#define BUFFER_SIZE     1024

static void
put_somthing(char *name)
{
    void *obj = NULL;
    
    if (name) {
        obj = blob_object_start(bs, name);
    }
    
    blob_put_bool(bs, "bool", true);
//    blob_put_binary(bs, "binary", "bbbbbbb", 7);
    blob_put_string(bs, "string", "sssssss");
//    blob_sprintf(bs "sprintf", "%s-%d", "bbbbbbb", 7);
    blob_put_u32(bs, "u32", 0x00000032);
    blob_put_u64(bs, "u64", 0x0000000000000064);
    blob_put_i32(bs, "i32", -1);
    blob_put_i64(bs, "i64", -1);

    if (name) {
        blob_object_end(bs, obj);
    }
}

#define COUNT   2

static jobj_t
array_btoj(void)
{
    char *name;
    void *arr, *obj;
    int i;
    char tmp[128];
    
    slice_alloc(bs, BUFFER_SIZE);  

    blob_root_array(bs);
    blob_t *root = blob_root(bs);
    
    name = "FIRST";
    os_println("%s begin", name);
    put_somthing(name);
    os_println("%s end", name);
    
    name = "LAST";
    os_println("%s begin", name);
    put_somthing(name);
    os_println("%s end", name);

    debug_ok("4:root blob vlen=%d", root->vlen);

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
    
    slice_alloc(bs, BUFFER_SIZE);  

    blob_root_object(bs);
    blob_t *root = blob_root(bs);
    
    name = "FIRST";
    os_println("%s begin", name);
    put_somthing(name);
    os_println("%s end", name);
    
    debug_ok("1:root blob vlen=%d", root->vlen);

    name = "OBJ";
    os_println("%s begin", name);
    obj = blob_object_start(bs, name);
    for (i=0; i<COUNT; i++) {
        os_sprintf(tmp, "array-%d", i);
        
        os_println("%s.%s begin", name, tmp);
        arr = blob_array_start(bs, tmp);
        put_somthing("sb");
        blob_array_end(bs, arr);
        os_println("%s.%s end", name, tmp);
    }
    blob_object_end(bs, obj);
    os_println("%s end", name);
    
    debug_ok("2:root blob vlen=%d", root->vlen);

    name = "ARRAY";
    os_println("%s begin", name);
    arr = blob_array_start(bs, name);
    for (i=0; i<COUNT; i++) {
        os_sprintf(tmp, "obj-%d", i);
        
        os_println("%s.%s begin", name, tmp);
//        obj = blob_object_start(bs, tmp);
        put_somthing("sb");
//        blob_object_end(bs, obj);
        os_println("%s.%s end", name, tmp);
    }
    blob_array_end(bs, arr);
    os_println("%s end", name);

    debug_ok("3:root blob vlen=%d", root->vlen);

    name = "LAST";
    os_println("%s begin", name);
    put_somthing(name);
    os_println("%s end", name);

    debug_ok("4:root blob vlen=%d", root->vlen);

    blob_dump(root);
    
    return blob_btoj(root);
}


int __main(int argc, char *argv[])
{
    jobj_t obj;
    char *json;
    
    obj = array_btoj();
    json = jobj_string(obj);
    os_println("%s", json);
    jobj_put(obj);
    
    obj = object_btoj();
    json = jobj_string(obj);
    os_println("%s", json);

    blob_jtob(bs, json);
    blob_dump(blob_root(bs));

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
