#ifndef __BUILDIN_H_1e8d76851e034897b63a0defba950e58__
#define __BUILDIN_H_1e8d76851e034897b63a0defba950e58__
/******************************************************************************/
#if defined(__BOOT__) || defined(__APP__)
/* Force a compilation error if condition is true */
#ifndef BUILD_BUG_ON
#define BUILD_BUG_ON(_condition)    ((void)BUILD_BUG_ON_ZERO(_condition))
#endif
/* Force a compilation error if condition is true, but also produce a
   result (of value 0 and type size_t), so the expression can be used
   e.g. in a structure initializer (or where-ever else comma expressions
   aren't permitted). */
#ifndef BUILD_BUG_ON_ZERO
#define BUILD_BUG_ON_ZERO(_e)       (sizeof(struct { int:-!!(_e); }))
#endif

#ifndef BUILD_BUG_ON_NULL
#define BUILD_BUG_ON_NULL(_e)       ((void *)sizeof(struct { int:-!!(_e); }))
#endif

#ifndef BUILD_BUG_NOT_ARRAY
#   ifdef  BUILD_BUG_NOT_ARRAY_SKIP
#       define BUILD_BUG_NOT_ARRAY(_array)  BUILD_BUG_ON(false)
#   else
#       define BUILD_BUG_NOT_ARRAY(_array)  BUILD_BUG_ON(sizeof(_array)==sizeof(void *))
#   endif
#endif

#define BUILD_BUG_NOT_OBJECT_SKIP
#ifndef BUILD_BUG_NOT_OBJECT
#   ifdef  BUILD_BUG_NOT_OBJECT_SKIP
#       define BUILD_BUG_NOT_OBJECT(_obj)   BUILD_BUG_ON(false)
#   else
#       define BUILD_BUG_NOT_OBJECT(_obj)   BUILD_BUG_ON(sizeof(_obj)==sizeof(void *))
#   endif
#endif

#ifndef offsetof
#define offsetof(_TYPE, _MEMBER)    __builtin_offsetof(_TYPE, _MEMBER)
#endif

#ifndef container_of
/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the container.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(_ptr, _type, _member) \
	    ((_type *)((char *)(_ptr) - offsetof(_type, _member)))
#endif
#endif /* defined(__BOOT__) || defined(__APP__) */

#ifndef os_count_of
#define os_count_of(_x)         (sizeof(_x)/sizeof((_x)[0]))
#endif

#ifndef os_pointer_offset
#define os_pointer_offset(_base, _pointer)  ((char *)(_pointer) - (char *)(_base))
#endif

#ifndef os_constructor
#   ifdef __BUSYBOX__
#       define os_constructor
#   else
#       define os_constructor   __attribute__((constructor))
#   endif
#endif

#ifndef os_destructor
#   ifdef __BUSYBOX__
#       define os_destructor
#   else
#       define os_destructor    __attribute__((destructor))
#   endif
#endif

#ifndef os_do_nothing 
#define os_do_nothing()     do{}while(0)
#endif

#ifndef os_fake_declare
#define os_fake_declare     extern int __os_value_not_used_forever
#endif

#ifndef os_wait_forever
#define os_wait_forever()   do{sleep(60);}while(1)
#endif

#define os_swap_value(_a, _b) do {  \
    typeof(_a) __tmp_in_os_swap_value = (_a); \
    (_a) = (_b);                    \
    (_b) = __tmp_in_os_swap_value;  \
}while(0)

#define INVALID_VALUE       (-1)
#define INVALID_COMMON_ID   INVALID_VALUE
#define INVALID_SEM_ID      INVALID_COMMON_ID
#define INVALID_FD          INVALID_COMMON_ID

static inline bool is_good_common_id(int id)
{
    return id >= 0;
}

#define is_good_shmid(_shmid)   is_good_common_id(_shmid)
#define is_good_semid(_semid)   is_good_common_id(_semid)
#define is_good_fd(_fd)         is_good_common_id(_fd)

#define __is_good_fd(_fd)       ((_fd) > 0)

#define __os_close(_fd) ({  \
    int __fd = (_fd);       \
    int __err = 0;          \
                            \
    if (is_good_fd(__fd)) { \
        __err = close(__fd);\
    }                       \
                            \
    __err;                  \
})  /* end */

#define os_close(_fd)   ({  \
    int __err = __os_close(_fd); \
    _fd = INVALID_FD;       \
    __err;                  \
})  /* end */

/*
* check value with [begin, end)
*/
#define IS_GOOD_VALUE(_value, _begin, _end)     \
    (_value >= (_begin) && _value < (_end))
#define is_good_value(_value, _begin, _end) ({  \
    typeof(_value) __v_in_is_good_value = (_value);     \
    IS_GOOD_VALUE(__v_in_is_good_value, _begin, _end);  \
})  /* end */

/*
* check id with [0, end)
*/
#define is_good_enum(_id, _end)     is_good_value(_id, 0, _end)

#define OS_SAFE_VALUE_DEFT(_value, _min, _max, _deft)   \
    (((_value)<(_min) || (_value)>(_max))?(_deft):(_value))

#define os_safe_value_deft(_value, _min, _max, _deft) ({ \
    typeof(_value)  __value_in_os_safe_value_deft   = _value;   \
    typeof(_value)  __deft_in_os_safe_value_deft    = _deft;    \
                                                                \
    OS_SAFE_VALUE_DEFT(__value_in_os_safe_value_deft, _min, _max, __deft_in_os_safe_value_deft); \
})  /* end */

#define OS_SAFE_VALUE(_value, _min, _max)   \
    ((_value)<(_min)?(_min):((_value)>(_max)?(_max):(_value)))

#define os_safe_value(_value, _min, _max) ({ \
    typeof(_value)  __value_in_os_safe_value= _value;   \
    typeof(_value)  __min_in_os_safe_value  = _min      \
    typeof(_value)  __max_in_os_safe_value  = _max;     \
                                                        \
    OS_SAFE_VALUE(__value_in_os_safe_value, __min_in_os_safe_value, __max_in_os_safe_value);   \
})  /* end */

/*
* 忘了min/max在哪个头文件定义了，先放这里
*/
#if 1
#define OS_MIN(_x, _y)  ((_x)<(_y)?(_x):(_y))
#define os_min(_x,_y)   ({  \
    typeof(_x) __x_in_os_min = (_x);  \
    typeof(_y) __y_in_os_min = (_y);  \
    (void) (&__x_in_os_min == &__y_in_os_min);  \
    OS_MIN(__x_in_os_min, __y_in_os_min);       \
})  /* end */

#define OS_MAX(_x, _y)  ((_x)>(_y)?(_x):(_y))
#define os_max(_x,_y)   ({  \
    typeof(_x) __x_in_os_max = (_x);  \
    typeof(_y) __y_in_os_max = (_y);  \
    (void) (&__x_in_os_max == &__y_in_os_max);  \
    OS_MAX(__x_in_os_max, __y_in_os_max);       \
})  /* end */
#endif

#define OS_CMP_OPERATOR(_ret)   ((0==(_ret))?'=':((_ret)<0?'<':'>'))
#define os_cmp_operator(_ret)   ({  \
    int __ret = (int)(_ret);        \
    OS_CMP_OPERATOR(__ret);         \
})  /* end */

#define OS_IOVEC_INITER(_base, _len) { \
    .iov_base   = _base,    \
    .iov_len    = _len,     \
}   /* end */

#define __OS_MSGHDR_INITER( \
    _iov,                   \
    _iovlen,                \
    _name,                  \
    _namelen,               \
    _control,               \
    _controllen             \
)                           \
{                           \
    .msg_iov        = _iov,         \
    .msg_iovlen     = _iovlen,      \
    .msg_name       = _name,        \
    .msg_namelen    = _namelen,     \
    .msg_control    = _control,     \
    .msg_controllen = _controllen,  \
    .msg_flags      = 0,            \
}   /* end */

#define OS_MSGHDR_INITER(_iov, _iovlen, _name, _namelen)    \
    __OS_MSGHDR_INITER(_iov, _iovlen, _name, _namelen, NULL, 0)

#define OS_TIMEVAL_INITER(_sec, _usec) { \
    .tv_sec = _sec,     \
    .tv_usec= _usec,    \
}   /* end */

#define OS_ITIMEVAL_INITER(_sec, _usec) { \
    .it_interval= OS_TIMEVAL_INITER(_sec, _usec),   \
    .it_value   = OS_TIMEVAL_INITER(_sec, _usec),   \
}   /* end */

#define OS_TIMESPEC_INITER(_sec, _nsec) { \
    .tv_sec = _sec,     \
    .tv_nsec= _nsec,    \
}   /* end */

#define OS_ITIMESPEC_INITER(_sec, _nsec) { \
    .it_interval= OS_TIMESPEC_INITER(_sec, _nsec),  \
    .it_value   = OS_TIMESPEC_INITER(_sec, _nsec),  \
}   /* end */

/*
* copy from tcp.h
*
* seq1 is before seq2
*/
static inline bool os_seq_before(uint32 seq1, uint32 seq2)
{
    return ((int)(seq1-seq2)) < 0;
}

/*
* seq1 is after seq2
*/
#define os_seq_after(_seq1, _seq2)  os_seq_before(_seq2, _seq1)

#define __ERRNO(_err)   ((_err)<0?-errno:(_err))
#define __errno(_err)   ({  \
    int __err = (_err);     \
    __ERRNO(__err);         \
})

#define __os_call(_new, _free, _call, _args...) ({ \
    int __err = 0;                      \
    void *__obj = (void *)_new();       \
    if (__obj) {                        \
        __err = _call(__obj, ##_args);  \
    }                                   \
    _free(__obj);                       \
                                        \
    __err;                              \
})  /* end */

#define os_call(_begin, _end, _call, _arg1, _args...) ({ \
    int __err = _begin();               \
    if (0==__err) {                     \
        __err = _call(_arg1, ##_args);  \
    }                                   \
    _end();                             \
                                        \
    shell_error(__err);                 \
})  /* end */

#define os_call_1(_begin, _end, _call, _arg1, _args...) ({ \
    int __err = _begin(_arg1);          \
    if (0==__err) {                     \
        __err = _call(_arg1, ##_args);  \
    }                                   \
    _end(_arg1);                        \
                                        \
    shell_error(__err);                 \
})  /* end */

#define os_call_2(_begin, _end, _call, _arg1, _arg2, _args...) ({ \
    int __err = _begin(_arg1, _arg2);   \
    if (0==__err) {                     \
        __err = _call(_arg1, _arg2, ##_args); \
    }                                   \
    _end(_arg1, _arg2);                 \
                                        \
    shell_error(__err);                 \
})  /* end */

#define os_call_3(_begin, _end, _call, _arg1, _arg2, _arg3, _args...) ({ \
    int __err = _begin(_arg1, _arg2, _arg3);    \
    if (0==__err) {                             \
        __err = _call(_arg1, _arg2, _arg3, ##_args); \
    }                                           \
    _end(_arg1, _arg2, _arg3);                  \
                                                \
    shell_error(__err);                         \
})  /* end */

#define os_call_4(_begin, _end, _call, _arg1, _arg2, _arg3, _arg4, _args...) ({ \
    int __err = _begin(_arg1, _arg2, _arg3, _arg4); \
    if (0==__err) {                                 \
        __err = _call(_arg1, _arg2, _arg3, _arg4, ##_args); \
    }                                               \
    _end(_arg1, _arg2, _arg3, _arg4);               \
                                                    \
    shell_error(__err);                             \
})  /* end */

#define os_call_5(_begin, _end, _call, _arg1, _arg2, _arg3, _arg4, _arg5, _args...) ({ \
    int __err = _begin(_arg1, _arg2, _arg3, _arg4, _arg5);  \
    if (0==__err) {                                         \
        __err = _call(_arg1, _arg2, _arg3, _arg4, _arg5, ##_args); \
    }                                                       \
    _end(_arg1, _arg2, _arg3, _arg4, _arg5);                \
                                                            \
    shell_error(__err);                                     \
})  /* end */

static inline void 
os_closexec(int fd)
{
#ifdef __APP__
    fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC);
#endif
}

static inline void 
os_noblock(int fd)
{
#ifdef __APP__
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK );
#endif
}

static inline int
__os_wait_error(int status)
{
    int err = 0;
    
#ifdef __APP__
    if (WIFEXITED(status)) {
        err = WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        if (WCOREDUMP(status)) {
            err = WCOREDUMP(status);
        }
        else if (WTERMSIG(status)) {
            err = WTERMSIG(status);
        }
    }
#endif

    return err;
}

typedef struct {
    char *k;
    uint32 v;
} os_kv_t;

#define OS_KV_INITER(_k, _v)    { .k = _k, .v = _v }
/******************************************************************************/
#endif /* __BUILDIN_H_1e8d76851e034897b63a0defba950e58__ */
