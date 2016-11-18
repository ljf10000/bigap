/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifdef __APP__

int
LIBCALLv(void *f, libproto_t *proto)
{
    switch(proto->result.size) {
        case 0: LIBFUN(func_0_0, f)(); return 0;
        case 4: proto->result.u.u4 = LIBFUN(func_4_0, f)(); return 0;
        case 8: proto->result.u.u8 = LIBFUN(func_8_0, f)(); return 0;
        default: return -EDLLRESULTSIZE;
    }
}

static int
__libcall(void *f, libproto_t *proto)
{
    switch(proto->count) {
        case 0: return LIBCALLv(f, proto);
        case 1: return LIBCALLx(f, proto, 1);
#if LIBPARAMCOUNT(2)
        case 2: return LIBCALLx(f, proto, 2);
#endif
#if LIBPARAMCOUNT(3)
        case 3: return LIBCALLx(f, proto, 3);
#endif
#if LIBPARAMCOUNT(4)
        case 4: return LIBCALLx(f, proto, 4);
#endif
#if LIBPARAMCOUNT(5)
        case 5: return LIBCALLx(f, proto, 5);
#endif
#if LIBPARAMCOUNT(6)
        case 6: return LIBCALLx(f, proto, 6);
#endif
#if LIBPARAMCOUNT(7)
        case 7: return LIBCALLx(f, proto, 7);
#endif
#if LIBPARAMCOUNT(8)
        case 8: return LIBCALLx(f, proto, 8);
#endif
#if LIBPARAMCOUNT(9)
        case 9: return LIBCALLx(f, proto, 9);
#endif
#if LIBPARAMCOUNT(10)
        case 10:return LIBCALLx(f, proto, 10);
#endif
#if LIBPARAMCOUNT(11)
        case 11:return LIBCALLx(f, proto, 11);
#endif
#if LIBPARAMCOUNT(12)
        case 12:return LIBCALLx(f, proto, 12);
#endif
#if LIBPARAMCOUNT(13)
        case 13:return LIBCALLx(f, proto, 13);
#endif
#if LIBPARAMCOUNT(14)
        case 14:return LIBCALLx(f, proto, 14);
#endif
#if LIBPARAMCOUNT(15)
        case 15:return LIBCALLx(f, proto, 15);
#endif
#if LIBPARAMCOUNT(16)
        case 16:return LIBCALLx(f, proto, 16);
#endif
#if LIBPARAMCOUNT(17)
        case 17:return LIBCALLx(f, proto, 17);
#endif
#if LIBPARAMCOUNT(18)
        case 18:return LIBCALLx(f, proto, 18);
#endif
#if LIBPARAMCOUNT(19)
        case 19:return LIBCALLx(f, proto, 19);
#endif
        default:return -EDLLPARAMCOUNT;
    }
}

int
os_libcall(const char *lib, const char *sym, libproto_t *proto)
{
    int err = 0;
    
    void *h = dlopen(lib, RTLD_NOW | RTLD_GLOBAL);
    if (NULL==h) {
        debug_error("load %s error:%d", lib, -errno);
        err = -EDLLLOAD; goto error;
    }
    
    dlerror();

    char *errstring;
    void *f = dlsym(h, sym);
    if (NULL==f) {
        err = -EDLLSYMLOAD; goto error;
    }
    else if (NULL!=(errstring=dlerror())) {
        debug_error("load %s:%s error:%s", lib, sym, errstring);

        err = -EDLLSYMLOAD; goto error;
    }
    debug_trace("load %s:%s=%p", lib, sym, f);

    err = __libcall(f, proto);
    if (err<0) {
        goto error;
    }
    else if (proto->result.u.u4) {
        err = (int)proto->result.u.u4; goto error;
    }
    
error:
    if (h) {
        dlclose(h);
    }
    
    return err;
}

#endif
/******************************************************************************/
