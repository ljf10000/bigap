#ifndef __MEMORY_H_ee1cba9efbf54f0b88b7ba0ee5ee3c4a__
#define __MEMORY_H_ee1cba9efbf54f0b88b7ba0ee5ee3c4a__
/******************************************************************************/
#ifdef __BOOT__
#define size_t uint32_t
#endif

static inline void *
os_memcpy(void *dst, const void *src, size_t n)
{
    if (dst && src) {
        return memcpy(dst, src, n);
    } else {
        return os_assertV(dst);
    }
}

static inline void *
os_memset(void *s, int ch, size_t n)
{
    if (s) {
        return memset(s, ch, n);
    } else {
        return os_assertV(s);
    }
}

static inline void *
os_memzero(void *s, size_t n)
{
    return os_memset(s, 0, n);
}

static inline int
os_memcmp(const void *a, const void *b, size_t n)
{
    if (a) {
        if (b) {
            return a==b || n?memcmp(a, b, n):0;
        } else {
            return os_assertV(1);
        }
    } else {
        if (b) {
            return os_assertV(-1);
        } else {
            return os_assertV(0);
        }
    }
}

static inline bool
os_memeq(const void *a, const void *b, size_t n)
{
    return 0==os_memcmp(a, b, n);
}

static inline void *
os_memmem(const void *mem, size_t mem_size,
		     const void *obj, size_t obj_size)
{
#ifdef __BOOT__
	register const char *ph;
	register const char *pn;
	const char *plast;
	size_t n;

	if (obj_size == 0) {
		return (void *) mem;
	}

	if (mem_size >= obj_size) {
		ph = (const char *) mem;
		pn = (const char *) obj;
		plast = ph + (mem_size - obj_size);

		do {
			n = 0;
			while (ph[n] == pn[n]) {
				if (++n == obj_size) {
					return (void *) ph;
				}
			}
		} while (++ph <= plast);
	}

	return NULL;
#else
    return memmem(mem, mem_size, obj, obj_size);
#endif
}

/*
* use _array's size
*   _array is array name
*/
#ifndef os_arrayzero
#define os_arrayzero(_array)            ({  \
    BUILD_BUG_NOT_ARRAY(_array);            \
    os_memzero(_array, sizeof(_array));     \
})
#endif

/*
* use _dst's size
*   _dst and _src is array name
*/
#ifndef os_arraydcpy
#define os_arraydcpy(_dst, _src)        ({  \
    BUILD_BUG_NOT_ARRAY(_dst);              \
    os_memcpy(_dst, _src, sizeof(_dst));    \
})
#endif

/*
* use _src's size
*   _dst and _src is array name
*/
#ifndef os_arrayscpy
#define os_arrayscpy(_dst, _src)        ({  \
    BUILD_BUG_NOT_ARRAY(_src);              \
    os_memcpy(_dst, _src, sizeof(_src));    \
})
#endif

/*
* use _a's size
*   _a and _b is array name
*/
#ifndef os_arraycmp
#define os_arraycmp(_a, _b)         ({  \
    BUILD_BUG_NOT_ARRAY(_a);            \
    os_memcmp(_a, _b, sizeof(_a));      \
})
#endif

/*
* use _a's size
*   _a and _b is array name
*/
#ifndef os_arrayeq
#define os_arrayeq(_a, _b)          (0==os_arraycmp(_a, _b))
#endif

/*
* use (*_pobj)'s size
*
* obj is a object(struct/array/native type)
*   pobj is the obj's address
*
* if obj is array
*   (1)the array MUST be real array(not function param)
*   (2)the _pobj MUST be the address of array(use as &array)
*
* array sizeof test
*   sizeof(array)==sizeof(*&array)
*   sizeof(*array)==sizeof(array[0])
*   sizeof(&array)==sizeof(void *)
*/
#ifndef os_objzero
#define os_objzero(_obj)                ({  \
    BUILD_BUG_NOT_OBJECT(*(_obj));          \
    os_memzero(_obj, sizeof(*(_obj)));      \
})
#endif

/*
* use (*_dst)'s size
*/
#ifndef os_objdcpy
#define os_objdcpy(_dst, _src)          ({  \
    BUILD_BUG_NOT_OBJECT(*(_dst));          \
    os_memcpy(_dst, _src, sizeof(*(_dst))); \
})
#endif

/*
* use (*_src)'s size
*/
#ifndef os_objscpy
#define os_objscpy(_dst, _src)          ({  \
    BUILD_BUG_NOT_OBJECT(*(_src));          \
    os_memcpy(_dst, _src, sizeof(*(_src))); \
})
#endif

/*
* use (*_dst)'s size
*/
#ifndef os_objcpy
#define os_objcpy(_dst, _src)       os_objdcpy(_dst, _src)
#endif

/*
* use (*_a)'s size
*/
#ifndef os_objacmp
#define os_objacmp(_a, _b)          ({  \
    BUILD_BUG_NOT_OBJECT(*(_a));        \
    os_memcmp(_a, _b, sizeof(*(_a)));   \
})
#endif

/*
* use (*_b)'s size
*/
#ifndef os_objbcmp
#define os_objbcmp(_a, _b)          ({  \
    BUILD_BUG_NOT_OBJECT(*(_b)):        \
    os_memcmp(_a, _b, sizeof(*(_b)));   \
})
#endif

/*
* use (*_a)'s size
*/
#ifndef os_objcmp
#define os_objcmp(_a, _b)           os_objacmp(_a, _b)
#endif

/*
* use (*_a)'s size
*/
#ifndef os_objaeq
#define os_objaeq(_a, _b)           (0==os_objacmp(_a, _b))
#endif

/*
* use (*_b)'s size
*/
#ifndef os_objbeq
#define os_objbeq(_a, _b)           (0==os_objbcmp(_a, _b))
#endif

/*
* use (*_a)'s size
*/
#ifndef os_objeq
#define os_objeq(_a, _b)            (0==os_objcmp(_a, _b))
#endif

#define os_objdeft(_obj, _deft) do{ \
    typeof(*_obj) __new_in_os_objdeft = _deft;      \
    os_objcpy(_obj, &__new_in_os_objdeft);          \
}while(0)

#define os_vcmp(_a, _b)     ({  \
    typeof(_a)  __a_in_os_vcmp = (_a); \
    typeof(_b)  __b_in_os_vcmp = (_b); \
    int ret;                    \
                                \
    if (__a_in_os_vcmp > __b_in_os_vcmp) { \
        ret = 1;                \
    }                           \
    else if (__a_in_os_vcmp==__b_in_os_vcmp) { \
        ret = 0;                \
    }                           \
    else {                      \
        ret = -1;               \
    }                           \
                                \
    ret;                        \
})

#define os_cmp_always_eq(_a, _b)    0
/*
* bad < good
* bad == bad
* all is good, use _objcmp
*/
#define __os_objcmp(_a, _b, _is_good, _objcmp) ({ \
    typeof(_a)  __a_in___os_objcmp = (_a);  \
    typeof(_b)  __b_in___os_objcmp = (_b);  \
    int ret;                                \
                                            \
    if (_is_good(__a_in___os_objcmp)) {     \
        if (_is_good(__b_in___os_objcmp)) { \
            ret = _objcmp(__a_in___os_objcmp, __b_in___os_objcmp); \
        } else {                            \
            ret = 1;                        \
        }                                   \
    } else {                                \
        if (_is_good(__b_in___os_objcmp)) { \
            ret = -1;                       \
        } else {                            \
            ret = 0;                        \
        }                                   \
    }                                       \
                                            \
    ret;                                    \
}) /* end */

#ifndef __os_getobjarrayidx
#define __os_getobjarrayidx(_array, _obj, _cmp, _begin, _end) ({ \
    int __i, __idx = (_end);                \
                                            \
    for (__i=(_begin); __i<(_end); __i++) { \
        if (0==_cmp((_array)[__i], _obj)) { \
            __idx = __i;                    \
            break;                          \
        }                                   \
    }                                       \
                                            \
    __idx;                                  \
})  /* end */
#endif

#ifndef os_getobjarrayidx
#define os_getobjarrayidx(_array, _obj, _begin, _end) \
        __os_getobjarrayidx(_array, _obj, os_objcmp, _begin, _end)
#endif

#if defined(__BOOT__) || defined(__APP__)
#define os_malloc(_size)            malloc(_size)
#define os_realloc(_ptr, _size)     realloc(_ptr, _size)
#define os_free(_ptr) \
        do{ if (_ptr) { free(_ptr); (_ptr) = NULL; } }while(0)
#else
#define os_malloc(_size)            kmalloc(_size, GFP_KERNEL)
#define os_realloc(_ptr, _size)     krealloc(_ptr, _size, GFP_KERNEL)
#define os_free(_ptr) \
        do{ if (_ptr) { kfree(_ptr); (_ptr) = NULL; } }while(0)
#endif

#ifdef __APP__
#define os_calloc(_count, _size)    calloc(_count, _size)
#else
static inline void *
os_calloc(uint32_t count, uint32_t size)
{
    void *obj = os_malloc(count*size);
    if (obj) {
        os_memzero(obj, count*size);
    }
    
    return obj;
}
#endif

#define os_zalloc(_size)            os_calloc(1, _size)

/******************************************************************************/
#endif /* __MEMORY_H_ee1cba9efbf54f0b88b7ba0ee5ee3c4a__ */
