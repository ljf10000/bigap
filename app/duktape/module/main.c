#ifndef __THIS_APP
#define __THIS_APP      js
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     "main"
#endif

#include "utils.h"
#include "dukc.h"

OS_INITER;

duk_context *__ctx;
int __argc;
char **__argv;

static int
__main(int argc, char *argv[])
{
    __argc = argc;
    __argv = argv;
    
    duk_context *ctx = __ctx = duk_create_heap_default();
    if (NULL==ctx) {
        return -ENOMEM;
    }
    
    global_register(ctx);
    duktape_register(ctx);
    my_register(ctx, argc, argv);
    libc_register(ctx);
    libcurl_register(ctx);
    
    duk_peval_file(ctx, argv[1]);
    duk_pop(ctx);

    duk_destroy_heap(ctx);
    
    return 0;
}

#ifndef __BUSYBOX__
#define duk_main  main
#endif

int duk_main(int argc, char *argv[])
{
    return os_main(__main, argc, argv);
}

