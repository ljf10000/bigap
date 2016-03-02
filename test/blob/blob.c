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
    void *obj;
    
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
int __main(int argc, char *argv[])
{
    char *json;
    void *arr, *obj;
    int i;

    slice_alloc(bs, BUFFER_SIZE);    
    blob_root_init(bs, BLOB_T_OBJECT, "ROOT");
    blob_t *root = blob_root(bs);

    put_somthing("FIRST");
    //debug_ok("1:root blob vlen=%d", root->vlen);

    obj = blob_object_start(bs, "OBJ");
    for (i=0; i<COUNT; i++) {
        char buf[128];

        os_sprintf(buf, "array-%d", i);
        os_println("obj %d begin", i);
        arr = blob_array_start(bs, buf);
        put_somthing(NULL);
        blob_array_end(bs, arr);
        os_println("obj %d end", i);
    }
    blob_object_end(bs, obj);
    debug_ok("2:root blob vlen=%d", root->vlen);

    arr = blob_array_start(bs, "ARRAY");
    for (i=0; i<COUNT; i++) {
        char buf[128];

        os_sprintf(buf, "obj-%d", i);
        os_println("array %d begin", i);
        obj = blob_object_start(bs, buf);
        put_somthing(NULL);
        blob_object_end(bs, obj);
        os_println("array %d end", i);
    }
    blob_array_end(bs, arr);

    debug_ok("3:root blob vlen=%d", root->vlen);

    put_somthing("LAST");
    //debug_ok("4:root blob vlen=%d", root->vlen);

    jobj_t j = blob_btoj(root);
    os_println("begin get json");
    json = jobj_string(j);
    os_println("end get json");
    os_println("%s", json);
    os_println("end print json");
    jobj_put(j);

    return 0;
}

int main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(__main, argc, argv);
}
/******************************************************************************/
