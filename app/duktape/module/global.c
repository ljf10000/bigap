#ifndef __THIS_APP
#define __THIS_APP      js
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     "global"
#endif

#include "utils.h"
#include "dukc.h"

#if 0
LIB_PARAM(BufferToString, DUK_VARARGS);
static duk_ret_t
duke_BufferToString(duk_context *ctx)
{
    int i, size = 0, offset = 0;
    
    int count = duk_get_argc(ctx);
    if (count<1) {
        return __push_string(ctx, __empty), 1;
    }
    
    void *buf[count];
    duk_size_t bsize[count];
    for (i=0; i<count; i++) {
        buf[i] = duk_require_buffer_data(ctx, i, &bsize[i]);
        
        bsize[i] = (bsize[i]<0)?0:bsize[i];
        size += bsize[i];
    }
    
    char string[1+size];
    for (i=0; i<count; i++) {
        os_memcpy(string + offset, buf[i], bsize[i]);
        offset += bsize[i];
    }
    string[size] = 0;
    
    return __push_lstring(ctx, string, size), 1;
}

LIB_PARAM(StringToBuffer, DUK_VARARGS);
static duk_ret_t
duke_StringToBuffer(duk_context *ctx)
{
    int i, size = 0, offset = 0;
    
    int count = duk_get_argc(ctx);
    if (count<1) {
        return duk_push_null(ctx), 1;
    }
    
    char *string[count];
    duk_size_t bsize[count];
    for (i=0; i<count; i++) {
        string[i] = (char *)duk_require_lstring(ctx, i, &bsize[i]);
        
        bsize[i] = (bsize[i]<0)?0:bsize[i];
        size += bsize[i];
    }
    
    void *buf = __push_dynamic_buffer(ctx, size);
    for (i=0; i<count; i++) {
        os_memcpy(buf + offset, string[i], bsize[i]);
        offset += bsize[i];
    }
    
    return 1;
}
#endif

static const dukc_func_entry_t global_func[] = {
#if 0
    LIB_FUNC(BufferToString),
    LIB_FUNC(StringToBuffer),
#endif
    LIB_FUNC_END
};

int global_register(duk_context *ctx)
{
    duk_push_global_object(ctx);
	    duk_put_functions(ctx, -1, global_func);
	duk_pop(ctx);
	
    return 0;
}

