#ifndef __BUILDIN_H_1e8d76851e034897b63a0defba950e58__
#define __BUILDIN_H_1e8d76851e034897b63a0defba950e58__
/******************************************************************************/
#if defined(__BOOT__) || defined(__APP__)
/* Force a compilation error if condition is true, but also produce a
   result (of value 0 and type size_t), so the expression can be used
   e.g. in a structure initializer (or where-ever else comma expressions
   aren't permitted). */
#ifndef BUILD_BUG_ON_ZERO
#define BUILD_BUG_ON_ZERO(_condition)       sizeof(struct { int:-!!(_condition); })
#endif

#ifndef BUILD_BUG_ON_NULL
#define BUILD_BUG_ON_NULL(_condition)       ((void *)BUILD_BUG_ON_ZERO(_condition))
#endif

/* Force a compilation error if condition is true */
#ifndef BUILD_BUG_ON
#define BUILD_BUG_ON(_condition)            ((void)BUILD_BUG_ON_ZERO(_condition))
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

#ifndef safe_container_of
#define safe_container_of(_ptr, _type, _member) \
	    ((_ptr)?container_of(_ptr, _type, _member):NULL)
#endif


#ifndef os_count_of
#define os_count_of(_x)         (sizeof(_x)/sizeof((_x)[0]))
#endif

#ifndef pointer_offsetof
#define pointer_offsetof(_a, _b)                        ({  \
    uintptr_t a_in_pointer_offsetof = (uintptr_t)(_a);      \
    uintptr_t b_in_pointer_offsetof = (uintptr_t)(_b);      \
                                                            \
    (a_in_pointer_offsetof > b_in_pointer_offsetof)?        \
        (a_in_pointer_offsetof - b_in_pointer_offsetof):    \
        (b_in_pointer_offsetof - a_in_pointer_offsetof);    \
})
#endif

#ifndef os_attribute
#define os_attribute(attrlist)          __attribute__(attrlist)
#endif

#ifndef os_is_const
#define os_is_const(expr)               __builtin_constant_p (expr)
#endif

#ifndef os_expect
#define os_expect(expr,value)           __builtin_expect ((expr),(value))
#endif

#ifndef os_prefetch
#define os_prefetch(addr,rw,locality)   __builtin_prefetch (addr, rw, locality)
#endif

#ifndef os_constructor
#   ifdef __ALLINONE__
#       define os_constructor
#   else
#       define os_constructor   os_attribute((constructor))
#   endif
#endif

#ifndef os_destructor
#   ifdef __ALLINONE__
#       define os_destructor
#   else
#       define os_destructor    os_attribute((destructor))
#   endif
#endif

typedef char *string_to_string_mapper_f(char *);
static inline char *
string_to_string_mapper(char *s)
{
    return s;
}

#ifndef os_do_nothing 
#define os_do_nothing()         do{}while(0)
#endif

#ifndef os_map_nothing
#define os_map_nothing(_var)    (_var)
#endif

#ifndef os_eq_buildin
#define os_eq_buildin(_d, _s)   do{ _d = (_s); }while(0)
#endif

#ifndef os_fake_declare
#define os_fake_declare         extern int __os_value_not_used_forever
#endif

#ifndef OS_WAIT_FOREVER
#define OS_WAIT_FOREVER         60 // second
#endif

#ifndef os_wait_forever
#define os_wait_forever()       do{ sleep(OS_WAIT_FOREVER); }while(1)
#endif

#ifndef os_yes_no
#define os_yes_no(_yes_no)      ((_yes_no)?"yes":"no")
#endif

#ifndef os_ok_error
#define os_ok_error(_ok_error)  ((_ok_error)?"ok":"error")
#endif

#ifndef ntohll
#define ntohll(_val)            __bswap_64(_val)
#endif

#ifndef htonll
#define htonll(_val)            ntohll(_val)
#endif

#define os_swap_value(_a, _b) do {  \
    typeof(_a) tmp_in_os_swap_value = (_a); \
    (_a) = (_b);                    \
    (_b) = tmp_in_os_swap_value;    \
}while(0)

#define __os_foreach(_i, _begin, _count)    for (_i=_begin; _i<_count; _i++)
#define os_foreach(_i, _count)              __os_foreach(_i, 0, _count)

#define INVALID_VALUE       (-1)
#define INVALID_COMMON_ID   INVALID_VALUE
#define INVALID_FD          INVALID_COMMON_ID

static inline bool is_good_common_id(int id)
{
    return id >= 0;
}

#define is_good_shmid(_shmid)   is_good_common_id(_shmid)
#define is_good_semid(_semid)   is_good_common_id(_semid)
#define is_good_fd(_fd)         is_good_common_id(_fd)

#define __is_good_fd(_fd)       ((_fd) > 0)

#define __os_close(_fd) ({              \
    int fd_in___os_close = (_fd);       \
    int err_in___os_close = 0;          \
                                        \
    if (is_good_fd(fd_in___os_close)) { \
        err_in___os_close = close(fd_in___os_close); \
    }                                   \
                                        \
    err_in___os_close;                  \
})  /* end */

#define os_close(_fd)   ({              \
    int err_in_os_close = __os_close(_fd); \
    _fd = INVALID_FD;                   \
    err_in_os_close;                    \
})  /* end */

#define os_closedir(_dir) do {  \
    if (_dir) {                 \
        closedir(_dir);         \
        _dir = NULL;            \
    }                           \
}while(0)  /* end */

/*
* check value with [begin, end)
*/
#define IS_GOOD_VALUE(_value, _begin, _end)     \
    (_value >= (_begin) && _value < (_end))

#define is_good_value(_value, _begin, _end) ({  \
    typeof(_value) v_in_is_good_value = (_value);     \
    IS_GOOD_VALUE(v_in_is_good_value, _begin, _end);  \
})  /* end */

/*
* check id with [0, end)
*/
#define IS_GOOD_ENUM(_id, _end)     IS_GOOD_VALUE(_id, 0, _end)
#define is_good_enum(_id, _end)     is_good_value(_id, 0, _end)

#define OS_SAFE_VALUE_DEFT(_value, _min, _max, _deft)   \
    (((_value)<(_min) || (_value)>(_max))?(_deft):(_value))

#define os_safe_value_deft(_value, _min, _max, _deft)   ({  \
    typeof(_value)  value_in_os_safe_value_deft   = _value; \
    typeof(_value)  deft_in_os_safe_value_deft    = _deft;  \
                                                            \
    OS_SAFE_VALUE_DEFT(value_in_os_safe_value_deft, _min, _max, deft_in_os_safe_value_deft); \
})  /* end */

#define OS_SAFE_VALUE(_value, _min, _max)   \
    ((_value)<(_min)?(_min):((_value)>(_max)?(_max):(_value)))

#define os_safe_value(_value, _min, _max) ({        \
    typeof(_value)  value_in_os_safe_value= _value; \
    typeof(_value)  min_in_os_safe_value  = _min    \
    typeof(_value)  max_in_os_safe_value  = _max;   \
                                                    \
    OS_SAFE_VALUE(value_in_os_safe_value, min_in_os_safe_value, max_in_os_safe_value); \
})  /* end */

#define os_safe_pointer(_pointer, _safe_pointer)    (_pointer)?(_pointer):(_safe_pointer)
/*
* ????min/max????????????????????????????
*/
#if 1
#define OS_MIN(_x, _y)  ((_x)<(_y)?(_x):(_y))
#define os_min(_x,_y)   ({  \
    typeof(_x) x_in_os_min = (_x);  \
    typeof(_y) y_in_os_min = (_y);  \
    (void) (&x_in_os_min == &y_in_os_min);  \
    OS_MIN(x_in_os_min, y_in_os_min);       \
})  /* end */

#define OS_MAX(_x, _y)  ((_x)>(_y)?(_x):(_y))
#define os_max(_x,_y)   ({  \
    typeof(_x) x_in_os_max = (_x);  \
    typeof(_y) y_in_os_max = (_y);  \
    (void) (&x_in_os_max == &y_in_os_max);  \
    OS_MAX(x_in_os_max, y_in_os_max);       \
})  /* end */
#endif

#define OS_CMP_OPERATOR(_ret)   ((0==(_ret))?'=':((_ret)<0?'<':'>'))
#define os_cmp_operator(_ret)   ({int m_ret = (int)(_ret); OS_CMP_OPERATOR(m_ret);})

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

/*
* struct timeval
*/
#define OS_TIMEVAL_INITER(_sec, _usec) { \
    .tv_sec = _sec,     \
    .tv_usec= _usec,    \
}   /* end */

/*
* struct itimerval
*/
#define OS_ITIMERVAL_INITER(_sec, _usec) { \
    .it_interval= OS_TIMEVAL_INITER(_sec, _usec),   \
    .it_value   = OS_TIMEVAL_INITER(_sec, _usec),   \
}   /* end */

/*
* struct timespec
*/
#define OS_TIMESPEC_INITER(_sec, _nsec) { \
    .tv_sec = _sec,     \
    .tv_nsec= _nsec,    \
}   /* end */

/* 
* struct itimerspec
*/
#define OS_ITIMESPEC_INITER(_sec, _nsec) { \
    .it_interval= OS_TIMESPEC_INITER(_sec, _nsec),  \
    .it_value   = OS_TIMESPEC_INITER(_sec, _nsec),  \
}   /* end */

/*
* copy from tcp.h
*
* seq1 is before seq2
* seq1 < seq2
*/
static inline bool os_seq_before(uint32 seq1, uint32 seq2)
{
    return ((int)(seq1-seq2)) < 0;
}

/*
* seq1 is after seq2
* seq1 > seq2
*/
#define os_seq_after(_seq1, _seq2)  os_seq_before(_seq2, _seq1)

static inline uint32 os_seq_offset(uint32 seq1, uint32 seq2)
{
    uint32 offset;

    if (os_seq_before(seq1, seq2)) {
        offset = (uint32)((int)seq2 - (int)seq1);
    } else {
        offset = (uint32)((int)seq1 - (int)seq2);
    }

    return offset;
}

#define os_seq_diff32(_seq1, _seq2) ({      \
    typeof(_seq1) m_seq1 = (_seq1);         \
    typeof(_seq2) m_seq2 = (_seq2);         \
                                            \
    OS_SEQ_BEFORE(m_seq1, m_seq2) ?                     \
        typeof(_seq1)((int32)m_seq1 - (int32)m_seq2) :  \
        typeof(_seq1)((int32)m_seq1 - (int32)m_seq2);   \
})  /* end */

#define os_seq_diff64(_seq1, _seq2) ({      \
    typeof(_seq1) m_seq1 = (_seq1);         \
    typeof(_seq2) m_seq2 = (_seq2);         \
                                            \
    OS_SEQ_BEFORE(m_seq1, m_seq2) ?                     \
        typeof(_seq1)((int64)m_seq1 - (int64)m_seq2) :  \
        typeof(_seq1)((int64)m_seq1 - (int64)m_seq2);   \
})  /* end */

#define os_calln(_new, _free, _call, _args...)  ({ \
    int m_err = 0;                      \
    void *m_obj = (void *)_new();       \
    if (m_obj) {                        \
        m_err = _call(m_obj, ##_args);  \
    }                                   \
    _free(m_obj);                       \
                                        \
    m_err;                              \
})  /* end */

static inline int os_call_nothing(void) { return 0; }

#define os_callv(_begin, _end, _call)  ({ \
    int m_err = _begin();               \
    if (0==m_err) {                     \
        m_err = _call();                \
    }                                   \
    _end();                             \
                                        \
    m_err;                              \
})  /* end */

#define os_call(_begin, _end, _call, _v1, _args...) ({ \
    int m_err = _begin();               \
    if (0==m_err) {                     \
        m_err = _call(_v1, ##_args);    \
    }                                   \
    _end();                             \
                                        \
    m_err;                              \
})  /* end */

#define os_call_1(_begin, _end, _call, _v1, _args...) ({ \
    int m_err = _begin(_v1);            \
    if (0==m_err) {                     \
        m_err = _call(_v1, ##_args);    \
    }                                   \
    _end(_v1);                          \
                                        \
    m_err;                              \
})  /* end */

#define os_call_2(_begin, _end, _call, _v1, _v2, _args...) ({ \
    int m_err = _begin(_v1, _v2);       \
    if (0==m_err) {                     \
        m_err = _call(_v1, _v2, ##_args); \
    }                                   \
    _end(_v1, _v2);                     \
                                        \
    m_err;                              \
})  /* end */

#define os_call_3(_begin, _end, _call, _v1, _v2, _v3, _args...) ({ \
    int m_err = _begin(_v1, _v2, _v3);  \
    if (0==m_err) {                     \
        m_err = _call(_v1, _v2, _v3, ##_args); \
    }                                   \
    _end(_v1, _v2, _v3);                \
                                        \
    m_err;                              \
})  /* end */

#define os_call_4(_begin, _end, _call, _v1, _v2, _v3, _v4, _args...) ({ \
    int m_err = _begin(_v1, _v2, _v3, _v4); \
    if (0==m_err) {                     \
        m_err = _call(_v1, _v2, _v3, _v4, ##_args); \
    }                                   \
    _end(_v1, _v2, _v3, _v4);           \
                                        \
    m_err;                              \
})  /* end */

#define os_call_5(_begin, _end, _call, _v1, _v2, _v3, _v4, _v5, _args...) ({ \
    int m_err = _begin(_v1, _v2, _v3, _v4, _v5);    \
    if (0==m_err) {                     \
        m_err = _call(_v1, _v2, _v3, _v4, _v5, ##_args); \
    }                                   \
    _end(_v1, _v2, _v3, _v4, _v5);      \
                                        \
    m_err;                              \
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

    return native_error(err);
}

typedef struct {
    char *k;
    uint32 v;
} os_kv_t;

#define OS_KV_ENTRY(_k, _v)         { .k = _k, .v = _v }
/******************************************************************************/
#endif /* __BUILDIN_H_1e8d76851e034897b63a0defba950e58__ */
