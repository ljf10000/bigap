#ifndef __SLICE_H_fad6afb9599d4a349e26aae52beca022__
#define __SLICE_H_fad6afb9599d4a349e26aae52beca022__
/******************************************************************************/
/*
|<--slice_SIZE --------------------------------------------------------------->|
|                |<--slice_size ---------------------------------------------->|
|<--slice_resv-->|<--slice_len----------------------------->|<--slice_remain-->|
|                |<--slice_offset----->|                    |                  |
+----------------+---------------------+--------------------+------------------+
|      resv      |           data(used)                     |  data(no used)   |
+----------------+---------------------+--------------------+------------------+
|<--slice_head   |<--slice_data        |<--slice_cookie     |<--slice_tail     |<--slice_end

slice rules:
(1) slice_SIZE  == slice_size + slice_resv
(2) slice_size  == slice_len  + slice_remain
(3) slice_data  == slice_head + slice_resv
(4) slice_cookie== slice_data + slice_offset
(5) slice_tail  == slice_data + slice_len
(6) slice_end   == slice_tail + slice_remain
*/
enum {
    SLICE_STACK = 0x01,
};

typedef struct {
    uint32_t    flag;
    uint32_t    resv;
    uint32_t    offset;
    uint32_t    len;
    uint32_t    size;
    
    byte *head; /* fixed */
} slice_t;

#define slice_len(_slice)       (_slice)->len
#define slice_size(_slice)      (_slice)->size
#define slice_resv(_slice)      (_slice)->resv
#define slice_offset(_slice)    (_slice)->offset
#define slice_head(_slice)      (_slice)->head
#define slice_flag(_slice)      (_slice)->flag

static inline bool
slice_in_stack(const slice_t *slice)
{
    return os_hasflag(slice_flag(slice), SLICE_STACK);
}

#define slice_in_heap(_slice)   (false==slice_in_stack(_slice))

static inline bool 
slice_is_clean(const slice_t *slice)
{
    return  0==slice_len(slice)     &&
            0==slice_size(slice)    &&
            0==slice_resv(slice)    &&
            NULL==slice_head(slice);
}

static inline bool
slice_is_empty(const slice_t *slice)
{
    trace_assert(NULL!=slice, "slice is nil");
    
    return !!slice_len(slice);
}

#define __SLICE_INITER(_data, _size, _resv, _is_local) { \
    .flag   = (_is_local)?SLICE_STACK:0,    \
    .len    = 0,                            \
    .size   = (int)(_size) - (int)(_resv),  \
    .resv   = (int)(_resv),                 \
    .head   = (byte *)(_data),     \
}

#define SLICE_INITER(_data, _size, _is_local) \
        __SLICE_INITER(_data, _size, 0, _is_local)

#define __SLICE_LOCAL_GUID(_slice, _size, _resv, _guid) \
    byte buffer_##_guid[_size]; \
    slice_t slice_##_guid = __SLICE_INITER(buffer_##_guid, _size, _resv, true); \
    slice_t *_slice = &slice_##_guid

#define __SLICE_LOCAL(_slice, _size, _resv) \
        __SLICE_LOCAL_GUID(_slice, _size, _resv, 91462AFE324E762AF291F223339C3F7E)

#define SLICE_LOCAL(_slice, _size) \
        __SLICE_LOCAL(_slice, _size, 0)

#define __SLICE_CLONER(_slice, _len) { \
    .len    = _len,                 \
    .size   = slice_size(_slice),   \
    .resv   = slice_resv(_slice),   \
    .head   = slice_head(_slice),   \
    .flag   = slice_flag(_slice),   \
}

#define SLICE_CLONER(_slice) \
        __SLICE_CLONER(_slice, slice_len(_slice))

#define SLICE_CLONE_INITER(_slice)  \
        __SLICE_CLONER(_slice, 0)

#define SLICE_TO_IOV(_slice, _iov)      do{ \
    (_iov)->iov_base = slice_data(_slice);  \
    (_iov)->iov_len  = slice_len(_slice);   \
}while(0)

static inline uint32_t
slice_offset_save(slice_t *slice, uint32_t offset)
{
    uint32_t old = slice_offset(slice);

    slice_offset(slice) = offset;

    return old;
}

static inline int 
slice_reinit(slice_t *slice, uint32_t size, uint32_t resv, bool local)
{
    if (0==size) {
        debug_error("slice size==0");
        
        return -EINVAL1;
    }
    else if (false==slice_is_clean(slice) && resv >= size) {
        debug_error("resv=%d, size=%d", resv, size);
        
        return -EINVAL2;
    }

    slice_flag(slice)   = local?SLICE_STACK:0;
    slice_len(slice)    = 0;
    slice_size(slice)   = size - resv;
    slice_resv(slice)   = resv;
    slice_offset(slice) = 0;
    
    return 0;
}

static inline void 
slice_init_resv(slice_t *slice, byte *buf, uint32_t size, uint32_t resv, bool local)
{
    slice_head(slice) = buf;

    slice_reinit(slice, size, resv, local);
}

static inline void
slice_init(slice_t *slice, byte *buf, uint32_t size, bool local)
{
    slice_init_resv(slice, buf, size, 0, local);
}

static inline slice_t * 
slice_clean(slice_t *slice)
{
    slice_len(slice)    = 0;
    slice_offset(slice) = 0;

    return slice;
}

/* real seize */
static inline uint32_t 
slice_SIZE(const slice_t *slice)
{
    return slice_size(slice) + slice_resv(slice);
}

static inline uint32_t 
slice_remain(const slice_t *slice)
{
    return slice_size(slice) - slice_len(slice);
}

static inline byte *
slice_data(const slice_t *slice)
{
    return slice_head(slice) + slice_resv(slice);
}

static inline byte *
slice_cookie(const slice_t *slice)
{
    return slice_data(slice) + slice_offset(slice);
}

static inline byte *
slice_end(const slice_t *slice)
{
    return slice_data(slice) + slice_size(slice);
}

static inline byte *
slice_tail(const slice_t *slice)
{
    return slice_data(slice) + slice_len(slice);
}

static inline void 
slice_zero(slice_t *slice)
{
    os_memzero(slice_data(slice), slice_size(slice));

    slice_clean(slice);
}

static inline int
slice_alloc(slice_t *slice, uint32_t size)
{
    void *buf = NULL;

    buf = os_zalloc(size);
    if (NULL==buf) {        
        return -ENOMEM;
    }

    slice_init(slice, buf, size, false);

    return 0;
}

static inline void
slice_release(slice_t *slice)
{
    if (slice && slice_head(slice)) {
        os_free(slice_head(slice));
    }
}

static inline slice_t *
slice_clone(slice_t *dst, const slice_t *src)
{
    void *buf;
    
    buf = os_zalloc(slice_SIZE(src));
    if (NULL==buf) {
        return NULL;
    }
    
    os_memcpy(buf, slice_head(src), slice_resv(src) + slice_len(src));
    
    os_objdcpy(dst, src);
    slice_flag(dst) = (slice_flag(src) & ~SLICE_STACK);
    slice_head(dst) = buf;

    return dst;
}

#ifndef SLICE_GROW_DOUBLE_LIMIT
#define SLICE_GROW_DOUBLE_LIMIT     (128*1024)
#endif

#ifndef SLICE_GROW_STEP
#define SLICE_GROW_STEP             (4*1024)
#endif

static inline int
slice_grow(slice_t *slice, uint32_t grow)
{
    void *buf;
    int size = slice_SIZE(slice);

    if (slice_in_stack(slice)) {
        debug_error("slice is in statck, can not grow");
        
        return os_assertV(-ENOSUPPORT);
    }

    if (size < SLICE_GROW_DOUBLE_LIMIT) {
        grow = OS_MAX(size, grow);
    }
    grow = OS_MAX(SLICE_GROW_STEP, grow);
    
    buf = os_realloc(slice_head(slice), size + grow);
    if (NULL==buf) {
        return -ENOMEM;
    }

    slice_head(slice) = buf;
    slice_size(slice) += grow;

    debug_trace("slice_grow: size %d, grow %d", slice_size(slice) - grow, grow);
    
    return 0;
}

/*
* as skb_pull 
*   remove data from the start of a buffer
*/
static inline byte *
slice_pull(slice_t *slice, uint32_t len)
{
    trace_assert(NULL!=slice, "slice is nil");

    if (slice_size(slice) < len) {
        return os_assertV(NULL);
    }
    
    if (0==slice_len(slice)) {
        /*
        * slice �ճ�ʼ����ϣ���û��������ݣ�
        *
        * ���� pull��������� slice_len
        */
    }
    else if (slice_len(slice) < len) {
        /*
        * slice �Ѿ�ʹ�ò����������
        *   �� (�������ݳ���) < (����pull�ĳ���)
        *
        * pull ʧ��
        */
        return NULL;
    } 
    else {
        /*
        * slice �Ѿ�ʹ�ò����������
        *   �� (�������ݳ���) >= (����pull�ĳ���)
        *
        * ���� pull����Ҫ���� slice_len
        */
        slice_len(slice) -= len;
    } 
    
    slice_size(slice) -= len;
    slice_resv(slice) += len;
    
    return slice_data(slice);
}

/*
* as skb_push
*   add data to the start of a buffer
*/
static inline byte *
slice_push(slice_t *slice, uint32_t len)
{
    trace_assert(NULL!=slice,  "slice is nil");

    if (slice_resv(slice) < len) {
        return os_assertV(NULL);
    }
    
    slice_len(slice)    += len;
    slice_size(slice)   += len;
    slice_resv(slice)   -= len;

    return slice_data(slice);
}

static inline byte *
slice_unpull(slice_t *slice)
{
    return slice_push(slice, slice_resv(slice));
}

/*
* as skb_put
*   add data to a buffer
*/
static inline byte *
slice_put(slice_t *slice, uint32_t len)
{
    trace_assert(NULL!=slice, "slice is nil");
    
    if (len > slice_remain(slice)) {
        return os_assertV(NULL);
    }
    
    slice_len(slice) += len;
    
    return slice_tail(slice);
}

static inline byte *
slice_trim(slice_t *slice, uint32_t len)
{
    trace_assert(NULL!=slice, "slice is nil");
    
    if (len > slice_len(slice)) {
        return os_assertV(NULL);
    }
    
    slice_len(slice) -= len;
    
    return slice_tail(slice);
}

static inline byte *
slice_put_char(slice_t *slice, int ch)
{
    byte *new;
    
    trace_assert(NULL!=slice, "slice is nil");

    new  = slice_put(slice, 1);
    if (new) {
        new[0] = (ch & 0xff);
    }
    
    return new;
}

static inline byte *
slice_put_buf(slice_t *slice, void *buf, uint32_t len)
{
    trace_assert(NULL!=slice, "slice is nil");

    if (NULL==buf) {
        return os_assertV(NULL);
    }
    
    os_memcpy(slice_tail(slice), buf, len);
    
    return slice_put(slice, len);
}

/*
* �������� snprintf
* 
* (1)�� slice_tail ��ʼд��
*
* (2)�ռ��СΪ slice_remain
*
* (3)�ռ��㹻 : 
*   slice_tail д����������(���� '/0'), ���賤��Ϊ L(������ '/0')��
*   slice_len  ���� L����ʱ slice_remain > 0
*   �������� L
*
* (4)�ռ䲻�� : 
*   slice_tail д�벻��������(���賤��Ϊ S, ʵ����Ҫ���� L, S��L��������'/0'), 
*   slice_len  ���� S, ��ʱ slice_remain == 1
*   �������� L
*
* (5)���ñ�����ǰ����Ҫ�ȱ��ػ��� slice_remain;
*    ���ñ��������� os_snprintf_is_full �ж�д���Ƿ�����
*    ����
*       int space = slice_remain(slice);
*       int len = slice_sprintf(slice, fmt, ...);
*       if (os_snprintf_is_full(space, len)) {
*           ��ʱд�����ݲ�����
*       } else {
*           ��ʱд����������
*       }
*/
static inline int
slice_vsprintf(slice_t *slice, bool grow, char *fmt, va_list args)
{
    int len = 0, space;

    if (NULL==slice) {
        return os_vsprintf_size(fmt, args);
    }

try_again:
    space = slice_remain(slice);
    debug_trace("slice_vsprintf: remain %d", space);

    /*
    * ������ slice_remain Ӧ�ô��ڵ��� 0
    *
    * �� space ����Ϊ 1(���ں�����һ����)
    *
    * ����, ��ʣ��ռ�Ϊ1��0ʱ��
    *   vsnprintf �������κ�д�����
    */
    space = (space>0)?space:1;

    len = os_vsnprintf((char *)slice_tail(slice), space, fmt, args);
    debug_trace("slice_vsprintf: needed %d", len);

    if (os_snprintf_is_full(space, len)) { /* no space */
        debug_trace("slice_vsprintf: full");
        if (grow && 0==slice_grow(slice, len + 1 - space)) {
            debug_trace("slice_vsprintf: grow and try");

            goto try_again;
        } else {
            /* do nothing */
        }
    } else {
        slice_put(slice, len);
    }

    return len;
}

static inline int
slice_sprintf(slice_t *slice, bool grow, char *fmt, ...)
{
    va_list args;
    
    va_start(args, fmt);
    int len = slice_vsprintf(slice, grow, fmt, args);
    va_end(args);
    
    return len;
}

#if 0
#define slice_sprintf(_slice, _flag, _fmt, _args...) ({ \
    __label__ try_again, ok;                            \
    int len = 0, space;                                 \
                                                        \
    if (NULL==(_slice)) {                               \
        char tmp[4];                                    \
                                                        \
        /*                                              \
        * ����ֻ�Ǽ�����Ҫ���ٿռ�                      \
        */                                              \
        len = os_snprintf(tmp, 0, _fmt, ##_args);       \
                                                        \
        goto ok;                                        \
    }                                                   \
                                                        \
try_again:                                              \
    space = slice_remain(_slice);                       \
    debug_trace("slice_vsprintf: remain %d", space);    \
                                                        \
    /*                                                  \
    * ������ slice_remain Ӧ�ô��ڵ��� 0                \
    *                                                   \
    * �� space ����Ϊ 1(���ں�����һ����)               \
    *                                                   \
    * ����, ��ʣ��ռ�Ϊ1��0ʱ��                        \
    *   vsnprintf �������κ�д�����                    \
    */                                                  \
    space = (space>0)?space:1;                          \
                                                        \
    len = os_snprintf((char *)slice_tail(_slice),       \
                space, _fmt, ##_args);                  \
    debug_trace("slice_vsprintf: needed %d", len);      \
                                                        \
    if (os_snprintf_is_full(space, len)) { /* no space */ \
        debug_trace("slice_vsprintf: full");            \
        if (os_hasflag(_flag, SLICE_F_GROW)             \
            && 0==slice_grow(_slice, len + 1 - space)) {\
            debug_trace("slice_vsprintf: grow and try");\
                                                        \
            goto try_again;                             \
        } else {                                        \
            /* do nothing */                            \
        }                                               \
    } else {                                            \
        slice_put(_slice, len);                         \
    }                                                   \
                                                        \
ok:                                                     \
    len;                                                \
}) /* end */
#endif

#ifdef __APP__

static inline void 
__slice_to_msg
(
    slice_t    *slice, 
    bool            is_send,
    struct iovec    *iov,
    struct msghdr   *msg,
    sockaddr_t      *remote
)
{
    iov->iov_base   = slice_data(slice);
 	iov->iov_len    = (true == is_send)?slice_len(slice):slice_size(slice);
 	msg->msg_name   = remote;
 	msg->msg_namelen= sizeof(*remote);
 	msg->msg_iov    = iov;
 	msg->msg_iovlen = 1;
}

static inline int slice_send(int fd, slice_t *slice, sockaddr_t *remote, int flag)
{
    struct iovec    iov = {0};
    struct msghdr   msg = {0};

    __slice_to_msg(slice, true, &iov, &msg, remote);
    
    return sendmsg(fd, &msg, flag);
}

static inline int slice_recv(int fd, slice_t *slice, sockaddr_t *remote, int flag)
{
    struct iovec    iov = {0};
    struct msghdr   msg = {0};
    
    __slice_to_msg(slice, false, &iov, &msg, remote);
    
    return recvmsg(fd, &msg, flag);
}
#endif

/******************************************************************************/
#endif /* __SLICE_H_fad6afb9599d4a349e26aae52beca022__ */
