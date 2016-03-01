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
put_somthing(void)
{
    blob_put_bool(bs, "bool", true);
    blob_put_binary(bs, "binary", "bbbbbbb", 7);
    blob_put_string(bs, "string", "sssssss");
    blob_sprintf(bs "sprintf", "%s-%d", "sssssss", 7);
    blob_put_u32(bs, "u32", 0xffffffff);
    blob_put_u64(bs, "u64", 0xffffffffffffffff);
    blob_put_i32(bs, "i32", -1);
    blob_put_i64(bs, "i64", -1);
}

#define COUNT   1000
int __main(int argc, char *argv[])
{
    char *json;
    void *arr, *obj;
    int i;

    slice_alloc(bs, BUFFER_SIZE);    
    blob_root_init(bs, BLOB_T_OBJECT, "root");
    blob_t *root = blob_root(bs);

    put_somthing();
    debug_ok("1:root blob vlen=%d", root->vlen);

    obj = blob_object_start(bs, "obj");
    for (i=0; i<COUNT; i++) {
        debug_trace("obj %d begin", i);
        arr = blob_array_start(bs, "array");
        put_somthing();
        blob_array_end(bs, arr);
        debug_trace("obj %d end", i);
    }
    blob_object_end(bs, obj);
    debug_ok("2:root blob vlen=%d", root->vlen);

    arr = blob_array_start(bs, "array");
    for (i=0; i<COUNT; i++) {
        debug_trace("array %d begin", i);
        obj = blob_object_start(bs, "obj");
        put_somthing();
        blob_object_end(bs, obj);
        debug_trace("array %d end", i);
    }
    blob_array_end(bs, arr);

    debug_ok("3:root blob vlen=%d", root->vlen);

    put_somthing();
    debug_ok("4:root blob vlen=%d", root->vlen);

    jobj_t j = blob_btoj(root);
    os_println("%s", jobj_string(j));
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
